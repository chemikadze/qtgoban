#include <QtCore/QQueue>
#include <QtCore/QHash>
#include "sgfgame.h"
#include "processmatrix.h"

const QHash <QString, SgfVariant::Type> SgfGame::m_typeHash = SgfGame::createSgfTypeHash();
const QHash <SgfGame::Error, QString> SgfGame::m_errorStrings = SgfGame::createErrorStringsHash();
const QHash <SgfGame::MoveError, QString> SgfGame::m_moveErrorStrings = SgfGame::createMoveErrorHash();

template <typename T>
class deleteBits
{
	T val;
public:
	inline deleteBits(const T mask)
	{
		val = mask;
	}

	inline void operator() (T& source)
	{
		source &= ~val;
	}
};

template <typename T>
class addBits
{
	T val;
public:
	inline addBits(const T mask)
	{
		val = mask;
	}

	inline void operator() (T& source)
	{
		source |= val;
	}
};

SgfGame::SgfGame(QObject *p/*=0*/, QSize size /* =QSize(19, 19) */, Rules rls/* = JapaneseRules */) : QObject(p), m_killed(3, 0), m_square(3, 0), m_rules(rls), m_size(0, 0)
{
	resize(size);
	m_io = new QFile();
	codec = NULL;
	setEncoding("UTF-8");
	m_tree = new SgfTree();
	m_current = m_tree;
	m_st = 0;
	m_error = ENo;
	m_turn = cBlack;

	m_viewStack.push_back( QList<SgfVariant>() << SgfVariant(SgfVariant(0, 0), SgfVariant(size.width()-1, size.height()-1)) );
	connect(this, SIGNAL(currentNodeChanged(SgfTree*)), this, SIGNAL(boardChanged()));
	connect(this, SIGNAL(boardChanged()), this, SIGNAL(nodeAttributesChanged()));
}

SgfGame::~SgfGame()
{
	delete m_io;
	delete m_tree;
}

// TODO: wtf is japaneese and chineese rules?

bool SgfGame::makeMove(qint8 col, qint8 row)
{
	SgfTree* newNode = NULL;
	foreach (SgfTree* node, m_current->children())
	{
		SgfVariant v = node->attrValue("B");
		if (m_turn == cBlack && v.type() == SgfVariant::tPoint && v.toPoint() == Point(col, row))
		{
			newNode = node;
			break;
		}
		v = node->attrValue("W");
		if (m_turn == cWhite && v.type() == SgfVariant::tPoint && v.toPoint() == Point(col, row))
		{
			newNode = node;
			break;
		}
	}

	if (!newNode)
	{
		if (m_board[row][col] == cVoid)
		{
			newNode = new SgfTree(m_current);

			if (m_turn == cBlack)
				newNode->setAttribute("B", SgfVariant(col, row));
			else
				newNode->setAttribute("W", SgfVariant(col, row));

			m_board[row][col] = m_turn;
			setKills(newNode);
			// suicide
			if (!m_killStack.isEmpty() && m_killStack.last().first == newNode->moveIndex() &&
				m_killStack.last().second.contains(Stone(m_turn, Point(col,row))))
			{
				m_board[row][col] = cVoid;
				emitMoveError(MESuicide);
				delete newNode;
				return  false;
			}
			m_board[row][col] = cVoid;

			m_current->addChild(newNode);

			// ko check
			SgfTree *current = m_current;
			stepForward(newNode);
			QVector <QVector <Color> > curr_state = m_board;
			while (m_current != m_tree)
			{
				stepBackward();
				if (m_board == curr_state)
				{
					setCurrentMove(current);
					m_current->removeChild(newNode);
					emitMoveError(MEKo);
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}
	setCurrentMove(newNode);
	emit boardChanged();

	return true;
}

bool SgfGame::validatePoint(qint8 col, qint8 row)
{
	return col < m_size.width() && row < m_size.width() && col >=0 && row >= 0;
}

bool SgfGame::validatePoint(Point point)
{
	return validatePoint(point.col, point.row);
}

void SgfGame::validateAndAddKilled(SgfTree *node, qint8 col, qint8 row, const Color killedColor)
{
	if ( validatePoint(col, row) && m_board[row][col]==killedColor && isDead(col, row))
	{
		if ( !m_killStack.size() || m_killStack.last().first != node->moveIndex())
		{
			m_killStack.push_back(QPair <qint16, QSet< Stone > >(node->moveIndex(),
								  QSet<Stone>()));
		}
		m_killStack.last().second.insert(Stone(killedColor, Point(col, row)));
	}
}

void SgfGame::setKills(SgfTree *node)
{
	SgfVariant var;
	var = node->attrValue("B");
	Color killColor = cWhite;
	if (var.type() != SgfVariant::tPoint)
	{
		var = node->attrValue("W");
		killColor = cBlack;
		if (var.type() != SgfVariant::tPoint)
			return;
	}

	Point move = var.toPoint();

	// right
	validateAndAddKilled(node, move.col+1, move.row, killColor);
	//left
	validateAndAddKilled(node, move.col-1, move.row, killColor);
	//top
	validateAndAddKilled(node, move.col, move.row-1, killColor);
	//bottom
	validateAndAddKilled(node, move.col, move.row+1, killColor);
	// check suicide
	if ((m_killStack.isEmpty() || m_killStack.last().first != node->moveIndex()) &&
		isDead(move.col, move.row))
	{
		validateAndAddKilled(node, move.col, move.row, invertColor(killColor));
	}
}

bool SgfGame::isDead(qint8 col, qint8 row)
{
	Color color = m_board[row][col];
	if (color == cVoid)
		return false;

	QQueue < Point > q;
	QVector < QVector<qint8> > matrix;

	resizeMatrix(matrix, m_size, qint8(0));
	matrix[row][col] = 1;
	q.append( Point(col, row) );

	while (!q.isEmpty())
	{
		static const QVector <Point> deltas = getUpDownLeftRight();

		foreach (Point point, deltas)
		{
			point = q.first() + point;
			if ( validatePoint(point) && !matrix[point.row][point.col] )
			{
				if (m_board[point.row][point.col] == color)
					q.enqueue( point );
				else if (m_board[point.row][point.col] == cVoid)
					return false;
				matrix[point.row][point.col] = 1;
			}
		}
		q.dequeue();
	}
	return true;
}

int SgfGame::fillGroup(qint8 col, qint8 row, Color fillColor)
{
	return dfsMatrix(m_board, Point(col, row), assignment<Color>(fillColor));
}

void SgfGame::stepForward(SgfTree *next)
{
	m_current = next;

	Color color = cBlack;
	SgfVariant val = next->attrValue("B");
	if (val.type() != SgfVariant::tPoint)
	{
		val = next->attrValue("W");
		color = cWhite;
	}

	if (val.type() == SgfVariant::tPoint)
	{
		// make move
		Point p = val.toPoint();
		if (validatePoint(p)) // pass?
		{
			m_board[p.row][p.col] = color;

			setKills(next);

			if (m_killStack.size() && m_killStack.last().first == m_current->moveIndex())
				foreach (Stone killedStone, m_killStack.last().second)
				{
					Color scoredColor = invertColor(killedStone.color);
					int square = fillGroup(killedStone.point.col, killedStone.point.row, cVoid);
					m_killed[scoredColor] += square;
				}
		}

		//m_turn = invertColor(color);
	}
	else
	{
		static QHash <QString, Color> w;
		if (w.isEmpty())
		{
			w.insert("AW", cWhite);
			w.insert("AB", cBlack);
			w.insert("AE", cVoid);
		}

		for (QHash<QString, Color>::const_iterator it = w.constBegin(); it != w.constEnd(); ++it)
		{
			QList <SgfVariant> vals = next->attrValues(it.key());
			foreach (val, vals)
			{
				if (val.type() == SgfVariant::tPoint)
					setStone(val.toPoint(), it.value());
				else if (val.type() == SgfVariant::tCompose &&
						 val.toCompose().first.type() == SgfVariant::tPoint &&
						 val.toCompose().second.type() == SgfVariant::tPoint )
					{
						Point leftUp, rightDn;
						leftUp = val.toCompose().first.toPoint();
						rightDn = val.toCompose().second.toPoint();
						for (int x = leftUp.col; x<=rightDn.col; ++x)
							for (int y = leftUp.row; y<=rightDn.row; ++y)
								setStone(x, y, it.value());
					}
			}
		}
	}
	m_turn = invertColor(m_turn);

	QList <SgfVariant> vals = next->attrValues("DD"); // dimm
	for (int i=0; i<vals.size(); ++i)
	{
		processMatrix(m_cellVisible, vals[i], addBits<qint8>(CMDimm));
	}

	if (m_current->turn())
		m_turn = m_current->turn();
	
	vals = next->attrValues("VW"); // view region
	if (!vals.isEmpty())
	{
		setView(vals);
		m_viewStack.push_back(vals);
	}

	setTerritories(m_current);
	setLabels(m_current);
	setMarks(m_current);
	setLines(m_current);
}

void SgfGame::stepBackward()
{
	Color color = cWhite;
	SgfVariant val = m_current->attrValue("B");
	if (val.type() != SgfVariant::tPoint)
	{
		val = m_current->attrValue("W");
		color = cBlack;
	}

	if (val.type() == SgfVariant::tPoint)
	{
		// delete move
		if (validatePoint(val.toPoint())) // pass?
		{
			if (m_killStack.size() && m_killStack.last().first == m_current->moveIndex())
			{
				foreach (Stone killedStone, m_killStack.last().second)
			{
				Color scoredColor = invertColor(killedStone.color);
				int square = fillGroup(killedStone.point.col, killedStone.point.row, killedStone.color);
				m_killed[scoredColor] -= square;
			}
			m_killStack.pop_back();
			}

			m_board[val.toPoint().row][val.toPoint().col] = cVoid;
		}

		//m_turn = invertColor(killColor);
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
				if (val.type() == SgfVariant::tPoint)
					setStone(val.toPoint(), cVoid, true);
				else if (val.type() == SgfVariant::tCompose &&
						 val.toCompose().first.type() == SgfVariant::tPoint &&
						 val.toCompose().second.type() == SgfVariant::tPoint )
					{
						Point leftUp, rightDn;
						leftUp = val.toCompose().first.toPoint();
						rightDn = val.toCompose().second.toPoint();
						for (int x = leftUp.col; x<=rightDn.col; ++x)
							for (int y = leftUp.row; y<=rightDn.row; ++y)
								setStone(x, y, cVoid, true);
					}
			}
		}
		// repair moves
		if (m_rewriteStack.size() && m_rewriteStack.last().first == m_current->moveIndex())
		{
			foreach (Stone pnt, m_rewriteStack.last().second)
			{
				setStone(pnt.point, pnt.color, true); // we don't need remember rewrites
			}
			m_rewriteStack.pop_back();
		}
	}

	m_turn = invertColor(m_turn);

	QList <SgfVariant> vals = m_current->attrValues("DD"); // dimm
	for (int i=0; i<vals.size(); ++i)
	{
		processMatrix(m_cellVisible, vals[i], deleteBits<qint8>(CMDimm));
	}

	
	vals = m_current->attrValues("VW"); // view region
	if (!vals.isEmpty())
	{
		m_viewStack.pop_back();
		setView(m_viewStack.last());
	}

// now moved to parent node
	m_current = m_current->parent();

	if (m_current->turn())
		m_turn = m_current->turn();

	setMarks(m_current);
	setLabels(m_current);
	setLines(m_current);
	setTerritories(m_current);
}

void SgfGame::setLabels(SgfTree *node)
{
	m_labels.clear();
	foreach (SgfVariant var, node->attrValues("LB"))
	{
		if (var.type() == SgfVariant::tCompose)
		{
			QPair <SgfVariant, SgfVariant> pairValues = var.toCompose();
			if (pairValues.first.type() == SgfVariant::tPoint && pairValues.second.type() == SgfVariant::tSimpleText)
			{
				insert(m_labels, Label(pairValues.second.toString(),
									   Point(pairValues.first.toPoint().col,
											 pairValues.first.toPoint().row)));
			}
		}
	}
}

void SgfGame::setMarks(SgfTree *node)
{
	m_marks = node->marks();
}

void SgfGame::setLines(SgfTree *node)
{
	m_lines.clear();
	foreach (SgfVariant var, node->attrValues("AR"))
	{
		insert(m_lines, Line(var.toCompose().first.toPoint(),
							 var.toCompose().second.toPoint(),
							 lsArrow));
	}
	foreach (SgfVariant var, node->attrValues("LN"))
	{
		insert(m_lines, Line(var.toCompose().first.toPoint(),
							 var.toCompose().second.toPoint(),
							 lsLine));
	}
}

void SgfGame::setView(QList<SgfVariant> regionList)
{
	for (int i=0; i<m_size.height(); ++i)
		for (int j=0; j<m_size.width(); ++j)
			m_cellVisible[i][j] |= CMInvisible;
	for (int i=0; i<regionList.size(); ++i)
	{
		processMatrix(m_cellVisible, regionList[i], deleteBits<qint8>(CMInvisible));
	}
}

void SgfGame::setTerritories(SgfTree *node)
{
	m_square.fill(0, 3);
	processMatrix(m_territory, assignment<Color>(cVoid));
	foreach (Point p, node->terrBlack())
	{
		m_territory[p.row][p.col] = cBlack;
		++m_square[cBlack];
	}
	foreach (Point p, node->terrWhite())
	{
		m_territory[p.row][p.col] = cWhite;
		++m_square[cWhite];
	}
}

bool SgfGame::setStone(Point p, Color color, bool force /* = false*/)
{
	return setStone(p.col, p.row, color, force);
}

bool SgfGame::setStone(qint8 col, qint8 row, Color color, bool force /* = false*/)
{
	if (validatePoint(col, row))
	{
		if (m_board[row][col] != cVoid && !force)
		{
			if (m_rewriteStack.isEmpty() || m_rewriteStack.last().first != m_current->moveIndex())
				m_rewriteStack.push_back( QPair<qint16, QSet< Stone > >(m_current->moveIndex(), QSet< Stone >()) );
			m_rewriteStack.last().second.insert( Stone(m_board[row][col], Point(col, row)));
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

bool SgfGame::addStone(qint8 col, qint8 row, Color color)
{
	if (!validatePoint(col, row))
		return false;
	if (m_board[row][col] == color)
		return true;
	SgfTree *child = 0;
	if (m_current->move().color == cBlack || m_current->move().color == cWhite)
	{
		child = new SgfTree(m_current);
		m_current->addChild(child);
		stepForward(child);
	}

	if (setStone(col, row, color))
	{
		m_current->setStone(Stone(color, Point(col, row)));
		if (child)
		{
			emit currentNodeChanged(m_current);
		}
		emit boardChanged();
		return true;
	}
	else
	{
		if (child)
		{
			stepBackward();
			delete child;
		}
		return false;
	}
}

bool SgfGame::addStone(Stone stone)
{
	return addStone(stone.point.col, stone.point.row, stone.color);
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
	emit currentNodeChanged(m_current);
	emit boardChanged();
	emit turnChanged(turn());
	return true; // false if hasn't found
}

void SgfGame::addMark(qint8 col, qint8 row, Markup m)
{
	m_current->setMark(Mark(m, Point(col, row)));
	setMarks(m_current);
	emit boardChanged();
}

void SgfGame::addLine(Line ln)
{
	if (! lines().contains(ln))
	{
		m_lines.append(ln);

		if (ln.style == lsLine)
			m_current->setLine(ln.from, ln.to);
		else
			m_current->setArrow(ln.from, ln.to);
		emit boardChanged();
	}
}

void SgfGame::removeLine(Point p)
{
	m_current->removeLine(p);
	setLines(m_current);
	emit boardChanged();
}

void SgfGame::addLabel(Label lbl)
{
	m_current->setLabel(lbl);
	setLabels(m_current);
	emit boardChanged();
}

QString SgfGame::labelAt(Point p)
{
	foreach (Label lbl, labels())
	{
		if (lbl.pos == p)
			return lbl.text;
	}
	return QString();
}

void SgfGame::removeLabel(Point p)
{
	m_current->removeLabel(p);
	setLabels(m_current);
	emit boardChanged();
}

bool SgfGame::canMove(qint8 col, qint8 row)
{
	return m_board[row][col] == cVoid;
}

QFile::FileError SgfGame::readBufferFromFile(const QString& filename)
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

/*
  TODO: need clear old state data
		need read board size
  */
bool SgfGame::readGameFromBuffer()
{
	m_encodedBuffer = codec->toUnicode(m_buffer);

	delete m_tree;
	m_tree = readNodeFromBuffer();
	if (m_tree)
	{
		clearState();
		m_current = m_tree;
		emit gameTreeChanged(m_tree);
		emit currentNodeChanged(m_current);
		return true;
	}
	else
	{
		qWarning("SgfGame: can not load game from buffer with error %d (%s)", m_error, errorToString(m_error).toLatin1().data());
		m_tree = new SgfTree();
		m_current = m_tree;
		resize(19, 19);
		emit gameTreeChanged(m_tree);
		emit currentNodeChanged(m_current);
		return false;
	}
}

void SgfGame::clearState()
{
	processMatrix(m_board, assignment<Color>(cVoid));
	processMatrix(m_cellVisible, assignment<qint8>(qint8(CMNone)));
	m_viewStack.clear();
	m_rewriteStack.clear();
	m_killed.fill(0, 3);
	m_square.fill(0, 3);
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
					if ( value.type() == SgfVariant::tNone && value.type() != m_typeHash.value(attrName) )
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
	m_buffer.clear();
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
		// converting to FF4, we are not necrofils
		QString s_size, data;
		if (m_size.height() == m_size.width())
			s_size = QString::number(m_size.width());
		else
			s_size = QString("%1:%2").arg(m_size.width()).arg(m_size.height());
		// TODO: rewrite root properties saving correct
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
	if (type == SgfVariant::tNumber && attr == "SZ")
	{
		data.toInt(&ok);
		if (!ok)
			type = SgfVariant::tCompose;
	}
	switch (type)
	{
	case SgfVariant::tNumber:
		ret = SgfVariant(data.toInt(&ok));
		break;

	case SgfVariant::tReal:
		ret = SgfVariant(data.toDouble(&ok));
		break;

	case SgfVariant::tDouble:
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

	case SgfVariant::tColor:
		if (data.size() == 1)
		{
			if (data[0] == 'B')
				ret = SgfVariant(cBlack);
			else if (data[0] == 'W')
				ret = SgfVariant(cWhite);
			else if (data[0] == '1')
				ret = SgfVariant(cWhite);
			else if (data[0] == '2')
				ret = SgfVariant(cBlack);
			else
				ok = false;
		}
		break;

	case SgfVariant::tSimpleText:
	{
		QString p_data = data;
		p_data.replace('\n', "").replace('\r', "").replace(QRegExp("\\(\\:|\\\\|\\])"), "\\1");
		ret = SgfVariant(p_data, true);
		break;
	}
	case SgfVariant::tText:
	{
		QString p_data = data;
		p_data.replace(QRegExp("\\(\\:|\\\\|\\])"), "\\1");
		ret = SgfVariant(p_data, false);
		break;
	}

	case SgfVariant::tPoint:
	{
		ret = SgfVariant::strToMove(data);
		Point move = ret.toPoint();
		if ( m_size.width() <= move.col || m_size.height() <= move.row )
			ret = SgfVariant(Point::pass());
		ok = ret.type() == SgfVariant::tPoint;
		if (ok)
			break;
	}

	case SgfVariant::tCompose:
	{
		QPair <QString,QString> dataPair = splitCompose(data);
		QPair <SgfVariant,SgfVariant> val;
		if (attr == "AR" || attr == "LN" || attr == "AB" || attr == "AW" || attr == "AE" ||
			markupNames.key(attr, mVoid)!=mVoid || attr == "VW" || attr == "TW" || attr == "TB" || attr == "DD")
		{
			val.first = SgfVariant::strToMove(dataPair.first);
			val.second = SgfVariant::strToMove(dataPair.second);
			ok = val.first.type() == SgfVariant::tPoint &&
				 val.second.type() == SgfVariant::tPoint &&
				 validatePoint(val.first.toPoint()) &&
				 validatePoint(val.second.toPoint());
			if (ok)
				ret = SgfVariant(val);
		}
		else if (attr == "LB")
		{
			val.first = SgfVariant::strToMove(dataPair.first);
			ok = val.first.type() == SgfVariant::tPoint && validatePoint(val.first.toPoint());
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
		if (data.type() == SgfVariant::tNumber)
			resize(data.toNumber());
		else if (data.type() == SgfVariant::tCompose)
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
	resizeMatrix(m_board, s, cVoid);
	resizeMatrix(m_cellVisible, s, qint8(0x0));
	resizeMatrix(m_territory, s, cVoid);
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

QHash <SgfGame::Error, QString> SgfGame::createErrorStringsHash()
{
	QHash <Error, QString> hash;
	hash[ENo] = tr("No errors");
	hash[EBadAttrName] = tr("Bad attribute name");
	hash[EBadSyntax] = tr("Bad syntax");
	hash[EWrongGM] = tr("This is not a Go GSF file, GM attribute must have value \'1\'");
	hash[EUnknownEncoding] = tr("This file has no charset information or CA value is wrong");
	hash[EInvalidPoint] = tr("Point is out of bounds");
	hash[EBadAttrValue] = tr("Bad attribute value");
	return hash;
}

// Yes, i can write that as const char[][] and some cycles, but that is more useful and readable
QHash <QString, SgfVariant::Type> SgfGame::createSgfTypeHash()
{
	QHash <QString, SgfVariant::Type> typeHash;
	// Move
	typeHash["B" ] = SgfVariant::tPoint;
	typeHash["KO"] = SgfVariant::tNone;
	typeHash["MN"] = SgfVariant::tNumber;
	typeHash["W" ] = SgfVariant::tPoint;
	// Setup
	typeHash["AB"] = SgfVariant::tPoint;
	typeHash["AE"] = SgfVariant::tPoint;
	typeHash["AW"] = SgfVariant::tPoint;
	typeHash["PL"] = SgfVariant::tColor;
	// Node annotation
	typeHash["C" ] = SgfVariant::tText;
	typeHash["DM"] = SgfVariant::tDouble;
	typeHash["GB"] = SgfVariant::tDouble;
	typeHash["GW"] = SgfVariant::tDouble;
	typeHash["HO"] = SgfVariant::tDouble;
	typeHash["N" ] = SgfVariant::tSimpleText;
	typeHash["UC"] = SgfVariant::tDouble;
	typeHash["V" ] = SgfVariant::tReal;
	// Move annotation
	typeHash["BM"] = SgfVariant::tDouble;
	typeHash["DO"] = SgfVariant::tNone;
	typeHash["IT"] = SgfVariant::tNone;
	typeHash["TE"] = SgfVariant::tDouble;
	// Markup
	typeHash["AR"] = SgfVariant::tCompose;
	typeHash["CR"] = SgfVariant::tPoint;
	typeHash["DD"] = SgfVariant::tPoint;
	typeHash["LB"] = SgfVariant::tCompose;
	typeHash["LN"] = SgfVariant::tCompose;
	typeHash["MA"] = SgfVariant::tPoint;
	typeHash["SL"] = SgfVariant::tPoint;
	typeHash["SQ"] = SgfVariant::tPoint;
	typeHash["TR"] = SgfVariant::tPoint;
	// Root
	typeHash["AP"] = SgfVariant::tCompose;
	typeHash["CA"] = SgfVariant::tSimpleText;
	typeHash["FF"] = SgfVariant::tNumber;
	typeHash["GM"] = SgfVariant::tNumber;
	typeHash["ST"] = SgfVariant::tNumber;
	typeHash["SZ"] = SgfVariant::tNumber;
	// Info
	typeHash["BR"] = SgfVariant::tSimpleText;
	typeHash["BT"] = SgfVariant::tSimpleText;
	typeHash["CP"] = SgfVariant::tSimpleText;
	typeHash["DT"] = SgfVariant::tSimpleText;
	typeHash["EV"] = SgfVariant::tSimpleText;
	typeHash["GN"] = SgfVariant::tSimpleText;
	typeHash["GC"] = SgfVariant::tText;
	typeHash["ON"] = SgfVariant::tSimpleText;
	typeHash["OT"] = SgfVariant::tSimpleText;
	typeHash["PB"] = SgfVariant::tSimpleText;
	typeHash["PC"] = SgfVariant::tSimpleText;
	typeHash["PW"] = SgfVariant::tSimpleText;
	typeHash["RE"] = SgfVariant::tSimpleText;
	typeHash["RO"] = SgfVariant::tSimpleText;
	typeHash["RU"] = SgfVariant::tSimpleText;
	typeHash["SO"] = SgfVariant::tSimpleText;
	typeHash["TM"] = SgfVariant::tReal;
	typeHash["US"] = SgfVariant::tSimpleText;
	typeHash["WR"] = SgfVariant::tSimpleText;
	typeHash["WT"] = SgfVariant::tSimpleText;
	// Timing
	typeHash["BL"] = SgfVariant::tReal;
	typeHash["OB"] = SgfVariant::tNumber;
	typeHash["OW"] = SgfVariant::tNumber;
	typeHash["WL"] = SgfVariant::tReal;
	// Misc
	typeHash["FG"] = SgfVariant::tCompose;
	typeHash["PM"] = SgfVariant::tNumber;
	typeHash["VW"] = SgfVariant::tPoint;
	// Go
	typeHash["HA"] = SgfVariant::tNumber;
	typeHash["KM"] = SgfVariant::tReal;
	typeHash["TB"] = SgfVariant::tPoint;
	typeHash["TW"] = SgfVariant::tPoint;
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

void SgfGame::setTurn(Color turn)
{
	if ((turn == cBlack || turn == cWhite) && turn != m_turn)
	{
		m_turn = turn;
		if (m_current->turn() != m_turn && ! (m_current->parent() && m_current->parent()->turn() == invertColor(m_turn)) )
			m_current->setTurn(turn);
		emit turnChanged(turn);
	}
}

void SgfGame::removeNode(SgfTree *nodeToDelete)
{
	if (nodeToDelete->moveIndex() <= m_current->moveIndex())
	{
		SgfTree *node = m_current;
		bool ok = true;
		while (node->parent() && nodeToDelete->moveIndex() <= node->moveIndex())
		{
			ok = ok && node != nodeToDelete;
			node = node->parent();
		}
		if (!ok)
			setCurrentMove(node);
	}
	if (nodeToDelete->parent()->removeChild(nodeToDelete))
	{
		emit nodeRemoved(nodeToDelete);
		delete nodeToDelete;
	}
	emit boardChanged();
	emit nodeAttributesChanged();
}

Color SgfGame::borderColors(Point p)
{
	QQueue < Point > q;
	QVector < QVector<qint8> > w_matrix;
	Color color = m_board[p.row][p.col];
	Color returnColor(cVoid);

	resizeMatrix(w_matrix, m_size, qint8(0));
	w_matrix[p.row][p.col] = 1;
	q.append(p);
	while (!q.isEmpty())
	{
		static QVector <Point> deltas = getUpDownLeftRight();

		foreach (Point point, deltas)
		{
			point = q.first() + point;
			if ( validatePoint(point) )
			{
				if (!w_matrix[point.row][point.col])
				{
					if (m_board[point.row][point.col] == color)
					{
						q.enqueue( point );
					}
					else
					{
						returnColor = Color(returnColor | m_board[point.row][point.col]);
					}
					w_matrix[point.row][point.col] = 1;
				}
			}
		}
		q.dequeue();
	}
	return returnColor;
}

void SgfGame::markTerritory()
{
	QVector < QVector<qint8> > w_matrix;
	resizeMatrix(w_matrix, m_size, qint8(0));

	if (!(m_current->terrBlack().count() || m_current->terrWhite().count()))
		m_current->clearTerritory();
	for (int col=0; col<m_size.width(); ++col)
	{
		for (int row=0; row<m_size.height(); ++row)
		{
			if (!w_matrix[row][col])
			{
				if (!m_board[row][col])
				{
					Point p(col, row);
					Color clr = borderColors(Point(col, row));
					if (clr == cBlack || clr == cWhite)
					{
						// filling of m_territory
						QQueue < Point > q;

						processMatrix(w_matrix, assignment<qint8>(0));
						w_matrix[p.row][p.col] = 1;
						q.append(p);

						while (!q.isEmpty())
						{
							static QVector <Point> deltas = getUpDownLeftRight();

							foreach (Point point, deltas)
							{
								point = q.first() + point;
								if ( validatePoint(point) && !w_matrix[point.row][point.col] )
								{
									if (m_board[point.row][point.col] == cVoid)
										q.enqueue( point );
									else if (m_rules == ChineseRules)
									{
										setTerritory(Stone(m_board[point.row][point.col], Point(point.col, point.row)));
									}
									w_matrix[point.row][point.col] = 1;
								}
							}
							setTerritory(Stone(clr, q.first()));
							q.dequeue();
						}
					}
				}
				else if (m_rules == ChineseRules)
				{
					setTerritory(Stone(m_board[row][col], Point(col, row)));
				}
				w_matrix[row][col] = 1;
			}
		}
	}
	emit boardChanged();
	emit nodeAttributesChanged();
}

void SgfGame::markTerritory(Point p)
{
	if (validatePoint(p))
	{
		QVector < QVector<qint8> > w_matrix;
		resizeMatrix(w_matrix, m_size, qint8(0));
		Color terrOwner = invertColor(m_board[p.row][p.col]);
		if (!terrOwner)
		{
			terrOwner = borderColors(p);
			if ((!terrOwner) || terrOwner == cBoth)
				return;
		}

		QQueue < Point > q;

		processMatrix(w_matrix, assignment<qint8>(0));
		w_matrix[p.row][p.col] = 1;
		q.append(p);

		while (!q.isEmpty())
		{
			static QVector <Point> deltas = getUpDownLeftRight();

			foreach (Point point, deltas)
			{
				point = q.first() + point;
				if ( validatePoint(point) && !w_matrix[point.row][point.col] )
				{
					if (m_board[point.row][point.col] == cVoid ||
						m_board[point.row][point.col] == m_board[p.row][p.col])
						q.enqueue( point );
					w_matrix[point.row][point.col] = 1;
				}
			}
			setTerritory(Stone(terrOwner, q.first()));
			q.dequeue();
		}
		// due to bug in setTerritory
		setTerritories(m_current);
		emit boardChanged();
		emit nodeAttributesChanged();
	}
}

void SgfGame::setTerritory(Stone s)
{
	if (m_territory[s.point.row][s.point.col])
		--m_square[ m_territory[s.point.row][s.point.col] ];
	m_territory[s.point.row][s.point.col] = s.color;
	// buggy overwiriting
	m_current->setTerritory(s);
	++m_square[ s.color ];
}

void SgfGame::unmarkTerritory(Point p)
{
	if (validatePoint(p) && m_territory[p.row][p.col])
	{
		QQueue < Point > q;
		QVector < QVector<qint8> > w_matrix;
		Color clr = m_territory[p.row][p.col];

		resizeMatrix(w_matrix, m_size, qint8(0));
		w_matrix[p.row][p.col] = 1;
		q.append(p);

		while (!q.isEmpty())
		{
			static QVector <Point> deltas = getUpDownLeftRight();

			foreach (Point point, deltas)
			{
				point = q.first() + point;
				if ( validatePoint(point) && !w_matrix[point.row][point.col] )
				{
					if (m_territory[point.row][point.col] == clr)
						q.enqueue( point );
					w_matrix[point.row][point.col] = 1;
				}
			}
			setTerritory(Stone(cVoid, q.first()));
			q.dequeue();
		}
		// due to bug in setTerritory
		// todo - dfs for unmarking territory from black
		setTerritories(m_current);
		emit boardChanged();
		emit nodeAttributesChanged();
	}
}
