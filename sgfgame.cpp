#include <QtCore/QQueue>
#include <QtCore/QHash>
#include "sgfgame.h"

const QHash <QString, SgfVariant::Type> SgfGame::m_typeHash = SgfGame::createSgfTypeHash();
const QHash <SgfGame::Error, QString> SgfGame::m_errorStrings = SgfGame::createErrorStringsHash();
const QHash <SgfGame::MoveError, QString> SgfGame::m_moveErrorStrings = SgfGame::createMoveErrorHash();


SgfGame::SgfGame(QSize size /* =QSize(19, 19) */) : m_killed(3, 0), m_square(3, 0), m_size(0, 0)
{
	resize(size);
	m_io = new QFile();
	codec = NULL;
	setEncoding("UTF-8");
	m_tree = new SgfTree();
	m_current = m_tree;
	m_st = 0;
	m_error = ENo;
	m_turn = StoneBlack;
}

SgfGame::~SgfGame()
{
	saveToFile("./tests/output.sgf");
	delete m_io;
	delete m_tree;
}

/*
  TODO: maybe create some pointers for comments/marks/etc?
*/

bool SgfGame::makeMove(qint8 col, qint8 row)
{
	SgfTree* newNode = NULL;
	foreach (SgfTree* node, m_current->children())
	{
		SgfVariant v = node->attrValue("B");
		if (m_turn == StoneBlack && v.type() == SgfVariant::Move && v.toMove() == Point(col, row))
		{
			newNode = node;
			break;
		}
		v = node->attrValue("W");
		if (m_turn == StoneWhite && v.type() == SgfVariant::Move && v.toMove() == Point(col, row))
		{
			newNode = node;
			break;
		}
	}

	if (!newNode)
	{
		if (m_board[row][col] == StoneVoid)
		{
			newNode = new SgfTree(m_current);

			if (m_turn == StoneBlack)
				newNode->setAttribute("B", SgfVariant(col, row));
			else
				newNode->setAttribute("W", SgfVariant(col, row));

			m_board[row][col] = m_turn;
			setKills(newNode);
			// suicide
			if (newNode->killed().isEmpty() && isDead(col, row))
			{
				m_board[row][col] = StoneVoid;
				emitMoveError(MESuicide);
				delete newNode;
				return  false;
			}
			m_board[row][col] = StoneVoid;

			m_current->addChild(newNode);
		}
		else
		{
			return false;
		}
	}
	setCurrentMove(newNode);
	return true;
}

bool SgfGame::validatePoint(qint8 col, qint8 row)
{
	return col < m_size.width() && row < m_size.width() && col >=0 && row >= 0;
}

bool SgfGame::validatePoint(Point point)
{
	return validatePoint(point.first, point.second);
}

void SgfGame::validateAndAddKilled(SgfTree *node, qint8 col, qint8 row, const StoneColor killedColor)
{
	if ( validatePoint(col, row) && m_board[row][col]==killedColor && isDead(col, row))
		node->addKilled(Point(col, row), killedColor);
}

void SgfGame::setKills(SgfTree *node)
{
	SgfVariant var;
	var = node->attrValue("B");
	StoneColor killColor = StoneWhite;
	if (var.type() != SgfVariant::Move)
	{
		var = node->attrValue("W");
		killColor = StoneBlack;
		if (var.type() != SgfVariant::Move)
			return;
	}

	Point move = var.toMove();

	// right
	validateAndAddKilled(node, move.first+1, move.second, killColor);
	//left
	validateAndAddKilled(node, move.first-1, move.second, killColor);
	//top
	validateAndAddKilled(node, move.first, move.second-1, killColor);
	//bottom
	validateAndAddKilled(node, move.first, move.second+1, killColor);
	// check suicide
	if (node->killed().isEmpty())
	{
		validateAndAddKilled(node, move.first, move.second, invertColor(killColor));
	}
}

bool SgfGame::isDead(qint8 col, qint8 row)
{
	StoneColor color = m_board[row][col];
	if (color == StoneVoid)
		return false;

	QQueue < Point > q;
	QVector < QVector<qint8> > matrix;

	resizeMatrix(matrix, m_size, qint8(0));
	matrix[row][col] = 1;
	q.append( Point(col, row) );

	while (!q.isEmpty())
	{
		// Python-style lol
		// slow but readable
		QVector < Point > points;
		points << Point (q.first().first+1, q.first().second)
			   << Point (q.first().first-1, q.first().second)
			   << Point (q.first().first, q.first().second+1)
			   << Point (q.first().first, q.first().second-1);

		foreach (Point point, points)
		{
			if ( validatePoint(point) && !matrix[point.second][point.first] )
			{
				if (m_board[point.second][point.first] == color)
					q.enqueue( point );
				else if (m_board[point.second][point.first] == StoneVoid)
					return false;
				matrix[point.second][point.first] = 1;
			}
		}
		q.dequeue();
	}
	return true;
}

int SgfGame::fillGroup(qint8 col, qint8 row, StoneColor fillColor)
{
	int res = 0;
	QQueue < Point > q;
	QVector < QVector<qint8> > matrix;
	StoneColor color = m_board[row][col];

	resizeMatrix(matrix, m_size, qint8(0));
	matrix[row][col] = 1;
	q.append(Point(col, row));

	while (!q.isEmpty())
	{
		// Python-style lol
		// slow but readable
		QVector < Point > points;
		points << Point (q.first().first+1, q.first().second)
			   << Point (q.first().first-1, q.first().second)
			   << Point (q.first().first, q.first().second+1)
			   << Point (q.first().first, q.first().second-1);

		foreach (Point point, points)
		{
			if ( validatePoint(point) && !matrix[point.second][point.first] )
			{
				if (m_board[point.second][point.first] == color)
					q.enqueue( point );
				matrix[point.second][point.first] = 1;
			}
		}
		m_board[q.first().second][q.first().first] = fillColor;
		++res;
		q.dequeue();
	}
	return res;
}

void SgfGame::stepForward(SgfTree *next)
{
	//TODO: Full support of attributes

	m_current = next;

	StoneColor color = StoneBlack;
	SgfVariant val = next->attrValue("B");
	if (val.type() != SgfVariant::Move)
	{
		val = next->attrValue("W");
		color = StoneWhite;
	}

	if (val.type() == SgfVariant::Move)
	{
		// make move
		Point p = val.toMove();
		if (validatePoint(p)) // pass?
		{
			m_board[p.second][p.first] = color;

			if (next->killed().isEmpty())
				setKills(next);

			typedef QPair <Point, StoneColor> Stone;
			foreach (Stone killedStone, next->killed())
			{
				int square = fillGroup(killedStone.first.first, killedStone.first.second, StoneVoid);
				m_killed[color] += square;
			}
		}


		if (color == StoneBlack)
			m_turn = StoneWhite;
		else
			m_turn = StoneBlack;
	}
	else
	{
		static QHash <QString, StoneColor> w;
		if (w.isEmpty())
		{
			w.insert("AW", StoneWhite);
			w.insert("AB", StoneBlack);
			w.insert("AE", StoneVoid);
		}

		for (QHash<QString, StoneColor>::const_iterator it = w.constBegin(); it != w.constEnd(); ++it)
		{
			QList <SgfVariant> vals = next->attrValues(it.key());
			foreach (val, vals)
			{
				if (val.type() == SgfVariant::Move)
					setStone(val.toMove(), it.value());
				else if (val.type() == SgfVariant::Compose &&
						 val.toCompose().first.type() == SgfVariant::Move &&
						 val.toCompose().second.type() == SgfVariant::Move )
					{
						Point leftUp, rightDn;
						leftUp = val.toCompose().first.toMove();
						rightDn = val.toCompose().second.toMove();
						for (int x = leftUp.first; x<=rightDn.first; ++x)
							for (int y = leftUp.second; y<=rightDn.second; ++y)
								setStone(x, y, it.value());
					}
			}
		}
	}

	QList <SgfVariant> vals = next->attrValues("DD"); // dim
	foreach (SgfVariant val, vals)
	{
		m_markup[ val.toMove().second ][ val.toMove().first ] = true;
	}

	val = next->attrValue("PL");
	if (val.type() == SgfVariant::Color)
	{
		m_turn = val.toColor();
	}

}

void SgfGame::stepBackward()
{
	StoneColor killColor = StoneWhite;
	SgfVariant val = m_current->attrValue("B");
	if (val.type() != SgfVariant::Move)
	{
		val = m_current->attrValue("W");
		killColor = StoneBlack;
	}

	if (val.type() == SgfVariant::Move)
	{
		// delete move
		if (validatePoint(val.toMove())) // pass?
		{
			typedef QPair <Point, StoneColor> Stone;
			foreach (Stone killedStone, m_current->killed())
			{
				// TODO: 2 pairs one in another are horrible, need make special structure
				int square = fillGroup(killedStone.first.first, killedStone.first.second, killedStone.second);
				m_killed[killColor] -= square;
			}

			m_board[val.toMove().second][val.toMove().first] = StoneVoid;
		}

		if (killColor == StoneBlack)
			m_turn = StoneWhite;
		else
			m_turn = StoneBlack;
	}
	else
	{
		static QSet <QString> w;
		if (w.isEmpty())
		{
			w.insert("AW");
			w.insert("AB");
			w.insert("AE");
		}

		for (QSet<QString>::const_iterator it = w.constBegin(); it != w.constEnd(); ++it)
		{
			QList <SgfVariant> vals = m_current->attrValues(*it);
			foreach (val, vals)
			{
				if (val.type() == SgfVariant::Move)
					setStone(val.toMove(), StoneVoid, true);
				else if (val.type() == SgfVariant::Compose &&
						 val.toCompose().first.type() == SgfVariant::Move &&
						 val.toCompose().second.type() == SgfVariant::Move )
					{
						Point leftUp, rightDn;
						leftUp = val.toCompose().first.toMove();
						rightDn = val.toCompose().second.toMove();
						for (int x = leftUp.first; x<=rightDn.first; ++x)
							for (int y = leftUp.second; y<=rightDn.second; ++y)
								setStone(x, y, StoneVoid, true);
					}
			}
		}
		// repair moves
		typedef QPair <Point, StoneColor> RewritedStone;
		foreach (RewritedStone pnt, m_current->rewrites())
		{
			setStone(pnt.first, pnt.second);
		}
	}

	QList <SgfVariant> vals = m_current->attrValues("DD"); // dim
	foreach (val, vals)
	{
		m_markup[ val.toMove().second ][ val.toMove().first ] = false;
	}

	val = m_current->attrValue("PL");
	if (val.type() == SgfVariant::Color)
	{
		m_turn = val.toColor();
	}

	m_current = m_current->parent();
}

bool SgfGame::setStone(Point p, StoneColor color, bool force /* = false*/)
{
	return setStone(p.first, p.second, color, force);
}

bool SgfGame::setStone(qint8 col, qint8 row, StoneColor color, bool force /* = false*/)
{
	if (validatePoint(col, row))
	{
		if (m_board[row][col] != StoneVoid && !force)
		{
			m_current->addRewrite(col, row, m_board[row][col]);
		}
		m_board[row][col] = color;
		return true;
	}
	else
	{
		emitError(EInvalidPoint);
		qWarning("Wrong stone value!");
		return false;
	}
}

bool SgfGame::setCurrentMove(SgfTree *newCurr)
{
	if (newCurr == m_current)
		return true;

	QHash <SgfTree*, SgfTree*> w; // from second to first
	QQueue <SgfTree*> q;

	w.insert(newCurr, 0);
	q.enqueue(newCurr);
	while (q.size())
	{
		SgfTree* next;
		foreach (SgfTree* next, q.first()->children())
		{
			if (!w.contains(next))
			{
				w.insert(next, q.first());
				if (next == m_current)
				{
					q.clear();
					q.enqueue(next);
					break;
				}
				q.enqueue(next);
			}
		}
		next = q.first()->parent();
		if (next && !w.contains(next))
		{
			// code copy >_<
			w.insert(next, q.first());
			if (next == m_current)
			{
				q.clear();
				break;
			}
			q.enqueue(next);
		}
		q.dequeue();
	}

	SgfTree *nextNode;
	do
	{
		nextNode = w.value(m_current, 0);
		Q_ASSERT(nextNode);
		if (m_current->moveIndex() > nextNode->moveIndex())
		{
			stepBackward();
		}
		else
		{
			stepForward(nextNode);
		}
	}
	while (m_current != newCurr);

	m_current = newCurr;
	m_comment = m_current->attrValue("C").toString();
	emit currentNodeChanged(m_current);
	return true; // false if hasn't found
}

void SgfGame::setMarkup(qint8 col, qint8 row, Markup m)
{
	if (m!=MVoid)
	{
		if (markupNames.contains(m))
			m_current->attributes().insertMulti( markupNames.value(m), SgfVariant(col, row) );
	}
	else
	{
		QHash <Markup,QString>::const_iterator i;
		for (i=markupNames.constBegin(); i!=markupNames.constEnd(); ++i)
		{
			m_current->attributes().remove(i.value(), SgfVariant(col, row));
		}
	}
}

bool SgfGame::canMove(qint8 col, qint8 row)
{
	return m_board[row][col] == StoneVoid;
}

QFile::FileError SgfGame::loadBufferFromFile(const QString& filename)
{
	m_io->setFileName(filename);
	m_io->open(QIODevice::ReadOnly);
	if (m_io->error() == QFile::NoError)
		m_buffer = m_io->readAll();
	m_io->close();
	return m_io->error();
}

void SgfGame::encodeBuffer()
{
	m_encodedBuffer = codec->toUnicode(m_buffer);
}


bool SgfGame::readGameFromBuffer()
{
	m_encodedBuffer = codec->toUnicode(m_buffer);

	delete m_tree;
	m_tree = readNodeFromBuffer();
	if (m_tree)
	{
		m_current = m_tree;
		return true;
	}
	else
	{
		qWarning("SgfGame: can not load game from buffer with error %d (%s)", m_error, errorToString(m_error).toLatin1().data());
		m_tree = new SgfTree();
		m_current = m_tree;
		return false;
	}
}

void SgfGame::setEncoding(QString encoding)
{
	m_encoding = encoding;
	codec = QTextCodec::codecForName(encoding.toLatin1().data());
	if (! codec)
		setEncoding("UTF-8");
}

QString SgfGame::readEncodingFromBuffer()
{
	QByteArray::iterator pos = m_buffer.begin();
	if (m_buffer.size() >=7 && *pos == '(' && *(pos+1) == ';')
	{
		pos+=2;
		while (pos < m_buffer.end() && *pos != ';')
		{
			// parser is evil
			if (*pos <='A' || *pos >='Z')
			{
				++pos;
				continue;
			}
			QString attrName;
			QString data;
			for (; pos < m_buffer.end() && (*pos>= 'A' && *pos <= 'Z'); ++pos)
				attrName.append(*pos);
			if (pos < m_buffer.end())
			{
				while (*pos == '[')
				{
					++pos;
					for (; pos < m_buffer.end() && (*pos != ']' || *(pos-1) == '\\'); ++pos)
						data.append(*pos);
					++pos;
				}
			}
			else
			{
				emitError(EBadSyntax);
				return QString();
			}
			if (attrName == "CA")
			{
				return data;
			}
		}
	}
	else
	{
		emitError(EBadSyntax);
		return QString();
	}
	return QString();
}

SgfTree* SgfGame::readNodeFromBuffer(SgfTree *parent /*=NULL*/)
{
	// add static const QChar for all needed charters
	// ( ; ) [ ] \ A Z
	static const QChar cOB('('), cTZ(';'), cCB(')'), cOSB('['), cCSB(']'), cSLASH('\\'), cA('A'), cZ('Z');
	static QString::iterator pos;
	SgfTree *node = NULL, *ret = NULL;
	QString attrName, data;

	if (parent == NULL)
		pos = m_encodedBuffer.begin();
	// skip this tree bracket
	if (*(pos) != cOB || *(++pos) != cTZ)
	{
		emitError(EBadSyntax);
		return NULL;
	}
	attrName.reserve(3);
	data.reserve(10);

	// read nodes in cycle
	for (; pos != m_encodedBuffer.end() && *pos != cCB; )
	{
		// adding subtree
		if (*pos == cOB)
		{
			SgfTree* newChild = readNodeFromBuffer(node);
			if (newChild)
			{
				node->addChild( newChild );
			}
			else
			{
				delete ret;
				return NULL;
			}
			continue;
		}
		// if REAL start of node
		if (*pos == cTZ)
		{
			++pos;
			// change current node
			if (node != NULL)
			{
				SgfTree* newNode = new SgfTree(node);
				node->addChild(newNode);
				node = newNode;
			}
			else
				ret = node = new SgfTree(parent);
		}
		else
		{
			// other symbols
			if (*pos<cA || *pos>cZ )
			{
				if (! (*pos).isSpace())
				{
					emitError(EBadSyntax);
					delete ret;
					return NULL;
				}
				// skip it
				++pos;
				continue;
			}
		}
		// read node
		for (; pos != m_encodedBuffer.end() && *pos!=cTZ && *pos!=cCB && *pos!=cOB; )
		{
			// if good name for attribute - read it
			if (*pos>=cA && *pos<=cZ)
			{
				// read name
				attrName.clear();
				for (; pos < m_encodedBuffer.end() && (*pos>=cA && *pos<=cZ ); ++pos)
					attrName += *pos;

				// read values
				while (int depth = (*pos==cOSB))
				{
					data.clear();
					for (++pos; pos != m_encodedBuffer.end() && depth; ++pos)
					{
						if (*pos == cSLASH)
						{
							++pos;
							if (pos == m_encodedBuffer.end() || !depth)
								break;
						}
						else if (*pos == cCSB)
							--depth;
						if (depth)
							data.append(*pos);
					}
					while ((*pos).isSpace() && pos!=m_encodedBuffer.end())
						++pos;

					SgfVariant value = strToAttrValue(attrName, data);
					if ( value.type() == SgfVariant::None && value.type() != m_typeHash.value(attrName) )
						emit wrongValue(attrName, data);
					else
					{
						if (parent==NULL && isRootAttr(attrName))
						{
							if (attrName == "GM" && value.toNumber()!=1)
							{
								emitError(EWrongGM);
								emit wrongValue(attrName, data);
								return ret;
							}
							setRootAttr(attrName, value);
						}
						else
							node->addAttribute(attrName, value);
					}
				}
			}
			else
			{
				if (! (*pos).isSpace())
				{
					emitError(EBadSyntax);
					delete ret;
					return NULL;
				}
				++pos;
			}
		}
	}
	if (*pos == cCB && pos < m_encodedBuffer.end())
		++pos;
	return ret;
}

QFile::FileError SgfGame::saveToFile(const QString& filename)
{
	m_io->setFileName(filename);
	m_io->open(QIODevice::WriteOnly);
	if (m_io->error() == QFile::NoError)
	{
		m_buffer.clear();
		writeNodeToBuffer(m_tree);
		m_io->write(m_buffer);
		m_io->close();
	}
	return m_io->error();
}


void SgfGame::writeNodeToBuffer(SgfTree *node)
{
	static QString attrvalue("[%1]");
	QMultiHash<QString,SgfVariant>::iterator i;
	QString activeKey;

	m_buffer.append("(");
	if (node == m_tree)
	{
		// maybe FF != 4 will be after
		QString s_size, data;
		if (m_size.height() == m_size.width())
			s_size = QString::number(m_size.width());
		else
			s_size = QString("%1:%2").arg(m_size.width()).arg(m_size.height());
		// TODO: write that correct
		data = QString(";GM[1]FF[4]CA[%1]SZ[%2]ST[%3]AP[%4]").arg(m_encoding,
																  s_size,
																  QString::number(m_st),
																  VERSION_STRING);
		m_buffer.append( codec->fromUnicode(data) );
	}
	while (node != NULL)
	{
		if (node != m_tree)
			m_buffer.append(";");
		activeKey.clear();
		for (i = node->attributes().begin(); i!=node->attributes().end(); ++i)
		{
			if ( !(activeKey == i.key()) )
			{
				activeKey = i.key();
				m_buffer.append( codec->fromUnicode(activeKey) );
			}
			m_buffer.append(codec->fromUnicode( attrvalue.arg(i.value().toSgfRecordFormat()) ));
		}

		if (node->children().count() > 1)
		{
			foreach (SgfTree *node, node->children())
			{
				m_buffer.append(codec->fromUnicode("\n"));
				writeNodeToBuffer(node);
			}
			break;
		}
		else
			node = node->child(0);
	}
	m_buffer.append(")");
}


SgfVariant SgfGame::strToAttrValue(const QString& attr, const QString& data)
{
	SgfVariant::Type type = m_typeHash.value(attr);
	SgfVariant ret;
	bool ok = true;
	if (type == SgfVariant::Number && attr == "SZ")
	{
		data.toInt(&ok);
		if (!ok)
			type = SgfVariant::Compose;
	}
	switch (type)
	{
	case SgfVariant::Number:
		ret = SgfVariant(data.toInt(&ok));
		break;

	case SgfVariant::Real:
		ret = SgfVariant(data.toDouble(&ok));
		break;

	case SgfVariant::Double:
		if (data.size() == 1)
		{
			if (data[0] == '2')
				ret = SgfVariant(true);
			else if (data[0] == '1')
				ret = SgfVariant(false);
			else
				ok = false;
		}
		break;

	case SgfVariant::Color:
		if (data.size() == 1)
		{
			if (data[0] == 'B')
				ret = SgfVariant(StoneBlack);
			else if (data[0] == 'W')
				ret = SgfVariant(StoneWhite);
			else if (data[0] == '1')
				ret = SgfVariant(StoneWhite);
			else if (data[0] == '2')
				ret = SgfVariant(StoneBlack);
			else
				ok = false;
		}
		break;

	case SgfVariant::SimpleText:
	{
		QString p_data = data;
		p_data.replace('\n', "").replace('\r', "").replace(QRegExp("\\(\\:|\\\\|\\])"), "\\1");
		ret = SgfVariant(p_data, true);
		break;
	}
	case SgfVariant::Text:
	{
		QString p_data = data;
		p_data.replace(QRegExp("\\(\\:|\\\\|\\])"), "\\1");
		ret = SgfVariant(p_data, false);
		break;
	}

	case SgfVariant::Move:	
	{
		ret = SgfVariant::strToMove(data);
		Point move = ret.toMove();
		if ( m_size.width() <= move.first || m_size.height() <= move.second )
			ret = SgfVariant(-1, -1);
		ok = ret.type() == SgfVariant::Move;
		if (ok)
			break;
	}

	case SgfVariant::Compose:
	{
		QPair <QString,QString> dataPair = splitCompose(data);
		QPair <SgfVariant,SgfVariant> val;
		if (attr == "AR" || attr == "LN" || attr == "AB" || attr == "AW" || attr == "AE" ||
			markupNames.key(attr, MVoid)!=MVoid || attr == "TW" || attr == "TB" || attr == "DD")
		{
			val.first = SgfVariant::strToMove(dataPair.first);
			val.second = SgfVariant::strToMove(dataPair.second);
			ok = val.first.type() == SgfVariant::Move &&
				 val.second.type() == SgfVariant::Move;
			if (ok)
				ret = SgfVariant(val);
		}
		else if (attr == "LB")
		{
			val.first = SgfVariant::strToMove(dataPair.first);
			ok = val.first.type() == SgfVariant::Move;
			if (!ok)
				break;
			val.second = SgfVariant(dataPair.second, true);
			ret = SgfVariant(val);
		}
		else if (attr == "AP")
		{
			val.first = SgfVariant(dataPair.first, true);
			val.second = SgfVariant(dataPair.second, true);
			ret = SgfVariant(val);
		}
		else if (attr == "SZ")
		{
			val.first = SgfVariant(dataPair.first.toInt(&ok));
			if (!ok)
				break;
			val.second = SgfVariant(dataPair.first.toInt(&ok));
			ret = SgfVariant(val);
		}
		else
		{
#ifdef DEBUG
			qDebug("sgfgame.cpp:%d Oops", __LINE__);
#endif
			ok = false;
		}
		break;
	}

	default:
		ret = SgfVariant(data, false);
	}

	if (!ok)
		return SgfVariant();
	return ret;
}

QPair <QString,QString> SgfGame::splitCompose(const QString& s)
{
	QPair<QString,QString> ret;
	QString::const_iterator it;
	for (it = s.begin(); it != s.end(); ++it)
	{
		if (*it == ':')
		{
			ret.first = s.left( it - s.begin() );
			ret.second = s.right( s.length() - (it-s.begin()+1) );
			return ret;
		}
		else
			++it;
	}
	ret.first = s;
	return ret;
}

bool SgfGame::isRootAttr(const QString &s)
{
	if (s.size() != 2)
		return false;
	return s == "AP" || // App ver
		   s == "CA" || // CA, encoding
		   s == "FF" || // FF
		   s == "GM" || // GM
		   s == "ST" || // ST, how to show children
		   s == "SZ";	// size
}

void SgfGame::setRootAttr(const QString &attr, const SgfVariant &data)
{
	if (attr == "SZ")
	{
		if (data.type() == SgfVariant::Number)
			resize(data.toNumber());
		else if (data.type() == SgfVariant::Compose)
			resize(data.toCompose().first.toNumber(),
				   data.toCompose().second.toNumber());
	}
	else if (attr == "ST")
		m_st = data.toNumber();
}

void SgfGame::resize(qint8 col, qint8 row /*=-1*/)
{
	if (row < 0)
		row = col;
	m_size.setWidth(col);
	m_size.setHeight(row);
	resize(m_size);
}

void SgfGame::resize(QSize s)
{
	m_size = s;
	resizeMatrix(m_board, s, StoneVoid);
	resizeMatrix(m_markup, s, false);
}

void SgfGame::emitError(Error errorcode)
{
	m_error = errorcode;
	emit errorOccured(errorcode);
	emit errorOccured(errorToString(errorcode));
}

void SgfGame::emitMoveError(MoveError errcode)
{
	emit moveErrorOccured(errcode);
	emit moveErrorOccured(moveErrorToString(errcode));
}

void SgfGame::setComment(const QString &comment)
{
	m_comment = comment;
	m_current->setAttribute("C", comment);
}

QHash <SgfGame::Error, QString> SgfGame::createErrorStringsHash()
{
	QHash <Error, QString> hash;
	hash[ENo] = tr("No errors");
	hash[EBadAttrName] = tr("Bad attribute name");
	hash[EBadSyntax] = tr("Bad syntax");
	hash[EWrongGM] = tr("This is not a Go GSF file, GM attribute must have value \'1\'");
	hash[EUnknownEncoding] = tr("This file has no charset information or CA value is wrong");
	hash[EInvalidPoint] = tr("Point is out of bounds");
	return hash;
}

// Yes, i can write that as const char[][] and some cycles, but that is more useful and readable
QHash <QString, SgfVariant::Type> SgfGame::createSgfTypeHash()
{
	QHash <QString, SgfVariant::Type> typeHash;
	// Move
	typeHash["B" ] = SgfVariant::Move;
	typeHash["KO"] = SgfVariant::None;
	typeHash["MN"] = SgfVariant::Number;
	typeHash["W" ] = SgfVariant::Move;
	// Setup
	typeHash["AB"] = SgfVariant::Move;
	typeHash["AE"] = SgfVariant::Move;
	typeHash["AW"] = SgfVariant::Move;
	typeHash["PL"] = SgfVariant::Color;
	// Node annotation
	typeHash["C" ] = SgfVariant::Text;
	typeHash["DM"] = SgfVariant::Double;
	typeHash["GB"] = SgfVariant::Double;
	typeHash["GW"] = SgfVariant::Double;
	typeHash["HO"] = SgfVariant::Double;
	typeHash["N" ] = SgfVariant::SimpleText;
	typeHash["UC"] = SgfVariant::Double;
	typeHash["V" ] = SgfVariant::Real;
	// Move annotation
	typeHash["BM"] = SgfVariant::Double;
	typeHash["DO"] = SgfVariant::None;
	typeHash["IT"] = SgfVariant::None;
	typeHash["TE"] = SgfVariant::Double;
	// Markup
	typeHash["AR"] = SgfVariant::Compose;
	typeHash["CR"] = SgfVariant::Move;
	typeHash["DD"] = SgfVariant::Move;
	typeHash["LB"] = SgfVariant::Compose;
	typeHash["LN"] = SgfVariant::Compose;
	typeHash["MA"] = SgfVariant::Move;
	typeHash["SL"] = SgfVariant::Move;
	typeHash["SQ"] = SgfVariant::Move;
	typeHash["TR"] = SgfVariant::Move;
	// Root
	typeHash["AP"] = SgfVariant::Compose;
	typeHash["CA"] = SgfVariant::SimpleText;
	typeHash["FF"] = SgfVariant::Number;
	typeHash["GM"] = SgfVariant::Number;
	typeHash["ST"] = SgfVariant::Number;
	typeHash["SZ"] = SgfVariant::Number;
	// Info
	typeHash["BR"] = SgfVariant::SimpleText;
	typeHash["BT"] = SgfVariant::SimpleText;
	typeHash["CP"] = SgfVariant::SimpleText;
	typeHash["DT"] = SgfVariant::SimpleText;
	typeHash["EV"] = SgfVariant::SimpleText;
	typeHash["GN"] = SgfVariant::SimpleText;
	typeHash["GC"] = SgfVariant::Text;
	typeHash["ON"] = SgfVariant::SimpleText;
	typeHash["OT"] = SgfVariant::SimpleText;
	typeHash["PB"] = SgfVariant::SimpleText;
	typeHash["PC"] = SgfVariant::SimpleText;
	typeHash["PW"] = SgfVariant::SimpleText;
	typeHash["RE"] = SgfVariant::SimpleText;
	typeHash["RO"] = SgfVariant::SimpleText;
	typeHash["RU"] = SgfVariant::SimpleText;
	typeHash["SO"] = SgfVariant::SimpleText;
	typeHash["TM"] = SgfVariant::Real;
	typeHash["US"] = SgfVariant::SimpleText;
	typeHash["WR"] = SgfVariant::SimpleText;
	typeHash["WT"] = SgfVariant::SimpleText;
	// Timing
	typeHash["BL"] = SgfVariant::Real;
	typeHash["OB"] = SgfVariant::Number;
	typeHash["OW"] = SgfVariant::Number;
	typeHash["WL"] = SgfVariant::Real;
	// Misc
	typeHash["FG"] = SgfVariant::Compose;
	typeHash["PM"] = SgfVariant::Number;
	typeHash["VW"] = SgfVariant::Move;
	// Go
	typeHash["HA"] = SgfVariant::Number;
	typeHash["KM"] = SgfVariant::Real;
	typeHash["TB"] = SgfVariant::Move;
	typeHash["TW"] = SgfVariant::Move;
	return typeHash;
}



QHash <SgfGame::MoveError, QString> SgfGame::createMoveErrorHash()
{
	QHash <SgfGame::MoveError, QString> hash;
	hash[MENo] = tr("No errors");
	hash[MESuicide] = tr("Suicide move");
	hash[MEKo] = tr("Ko");
	return hash;
}
