#include "sgfgame.h"

const QMap <QString, SgfVariant::Type> SgfGame::m_typeMap = SgfGame::createSgfTypeMap();
const QMap <SgfGame::Error, QString> SgfGame::m_errorStrings = SgfGame::createErrorStringsMap();

SgfGame::SgfGame(int size /*=19*/)
{
	m_board.resize(size);
	for (int i=0; i<size; i++)
		m_board[i].resize(size);
	m_io = new QFile();
	codec = NULL;
	setEncoding("UTF-8");
	m_tree = new SgfTree();
	m_st = 0;
	m_error = ENo;
}

SgfGame::~SgfGame()
{
	delete m_io;
}

/*
   @i - column
   @j - row
*/
StoneColor SgfGame::stone(char i, char j)
{
	return m_board[i][j];
}

const QVector < QVector<StoneColor> > &  SgfGame::board()
{
	return m_board;
}

SgfTree* SgfGame::tree()
{
	return m_tree;
}

SgfTree* SgfGame::currentMove()
{
	return m_current;
}

/*
  TODO: maybe create some pointers for comments/marks/etc?
*/

bool SgfGame::makeMove(qint8 col, qint8 row, StoneColor color/* =Void  */)
{
	if (color == Void)
		color = m_turn;
	SgfTree* newNode = new SgfTree(m_current);
	m_current->addChild(newNode);
	m_current = newNode;
	if (moveIsCorrect(col, row))
	{
		if (m_turn == Black)
			m_current->setAttribute("B", SgfVariant(col, row));
		else
			m_current->setAttribute("W", SgfVariant(col, row));
		return true;
	}
	else
	{
		return false;
	}
}

StoneColor SgfGame::turn()
{
	return m_turn;
}


bool SgfGame::moveIsCorrect(qint8 col, qint8 row)
{
	// Ko, suicide
	if (m_board[col][row] == Void)
		return true;
	else
		return false;
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
		return true;
	else
	{
		qWarning("SgfGame: can not load game from buffer with error %d (%s)", m_error, errorToString(m_error).toLatin1().data());
		m_tree = new SgfTree();
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
					if ( value.type() == NULL && value.type() != m_typeMap.value(attrName) )
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
							node->setAttribute(attrName, value);
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
	QMultiMap<QString,SgfVariant>::iterator i;
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
		m_buffer.append(";");
		activeKey.clear();
		for (i = node->attributes().begin(); i!=node->attributes().end(); ++i)
		{
			// seed hack to use Qt copy-on-write container
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
	SgfVariant::Type type = m_typeMap.value(attr);
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
				ret = SgfVariant(Black);
			else if (data[0] == 'W')
				ret = SgfVariant(White);
			else if (data[0] == '1')
				ret = SgfVariant(White);
			else if (data[0] == '2')
				ret = SgfVariant(Black);
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
		ret = SgfVariant::strToMove(data);
		ok = ret.type() == SgfVariant::Move;
		break;

	case SgfVariant::Compose:
	{
		QPair <QString,QString> dataPair = splitCompose(data);
		QPair <SgfVariant,SgfVariant> val;
		if (attr == "AR" || attr == "LN")
		{
			val.first = SgfVariant::strToMove(dataPair.first);
			val.second = SgfVariant::strToMove(dataPair.second);
			ok = val.first.type() == SgfVariant::None &&
				 val.second.type() == SgfVariant::None;
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
	// TODO: maybe use QSet here?
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
	else if (attr == "CA")
	{// already read to this moment
		/* maybe this will be useful
		m_encoding = data.toString();
		codec = QTextCodec::codecForName(m_encoding.toLatin1());
		if (!codec)
		{
			m_encoding = "UTF-8";
			codec = QTextCodec::codecForName("UTF-8");
		} */
	}
	else if (attr == "ST")
		m_st = data.toNumber();
	//else
	//	qDebug("sgfgame.cpp:%d Oops", __LINE__);
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
	m_board.resize(s.height());
	for (int i=0; i<s.height(); ++i)
		m_board[i].resize(s.width());
}

QSize SgfGame::size()const
{
	return m_size;
}

const QString& SgfGame::encoding()const
{
	return m_encoding;
}

void SgfGame::emitError(Error errorcode)
{
	m_error = errorcode;
	emit errorOccured(errorcode);
}

QString SgfGame::errorToString(Error errorcode)
{
	return m_errorStrings.value(errorcode);
}

QMap <SgfGame::Error, QString> SgfGame::createErrorStringsMap()
{
	QMap <Error, QString> map;
	map[ENo] = tr("No errors");
	map[EBadAttrName] = tr("Bad attribute name");
	map[EBadSyntax] = tr("Bad syntax");
	map[EWrongGM] = tr("This is not a Go GSF file, GM attribute must have value \'1\'");
	map[EUnknownEncoding] = tr("This file has no charset information or CA value is wrong");
	return map;
}

// Yes, i can write that as const char[][] and some cycles, but that is more useful
QMap <QString, SgfVariant::Type> SgfGame::createSgfTypeMap()
{
	QMap <QString, SgfVariant::Type> typeMap;
	// Move
	typeMap["B" ] = SgfVariant::Move;
	typeMap["KO"] = SgfVariant::None;
	typeMap["MN"] = SgfVariant::Move;
	typeMap["W" ] = SgfVariant::Move;
	// Setup
	typeMap["AB"] = SgfVariant::Move;
	typeMap["AE"] = SgfVariant::Move;
	typeMap["AW"] = SgfVariant::Move;
	typeMap["PL"] = SgfVariant::Color;
	// Node annotation
	typeMap["C" ] = SgfVariant::Text;
	typeMap["DM"] = SgfVariant::Double;
	typeMap["GB"] = SgfVariant::Double;
	typeMap["GW"] = SgfVariant::Double;
	typeMap["HO"] = SgfVariant::Double;
	typeMap["N" ] = SgfVariant::SimpleText;
	typeMap["UC"] = SgfVariant::Double;
	typeMap["V" ] = SgfVariant::Real;
	// Move annotation
	typeMap["BM"] = SgfVariant::Double;
	typeMap["DO"] = SgfVariant::None;
	typeMap["IT"] = SgfVariant::None;
	typeMap["TE"] = SgfVariant::Double;
	// Markup
	typeMap["AR"] = SgfVariant::Compose;
	typeMap["CR"] = SgfVariant::Move;
	typeMap["DD"] = SgfVariant::Move;
	typeMap["LB"] = SgfVariant::Compose;
	typeMap["LN"] = SgfVariant::Compose;
	typeMap["MA"] = SgfVariant::Move;
	typeMap["SL"] = SgfVariant::Move;
	typeMap["SQ"] = SgfVariant::Move;
	typeMap["TR"] = SgfVariant::Move;
	// Root
	typeMap["AP"] = SgfVariant::Compose;
	typeMap["CA"] = SgfVariant::SimpleText;
	typeMap["FF"] = SgfVariant::Number;
	typeMap["GM"] = SgfVariant::Number;
	typeMap["ST"] = SgfVariant::Number;
	typeMap["SZ"] = SgfVariant::Number;
	// Info
	typeMap["BR"] = SgfVariant::SimpleText;
	typeMap["BT"] = SgfVariant::SimpleText;
	typeMap["CP"] = SgfVariant::SimpleText;
	typeMap["DT"] = SgfVariant::SimpleText;
	typeMap["EV"] = SgfVariant::SimpleText;
	typeMap["GN"] = SgfVariant::SimpleText;
	typeMap["GC"] = SgfVariant::Text;
	typeMap["ON"] = SgfVariant::SimpleText;
	typeMap["OT"] = SgfVariant::SimpleText;
	typeMap["PB"] = SgfVariant::SimpleText;
	typeMap["PC"] = SgfVariant::SimpleText;
	typeMap["PW"] = SgfVariant::SimpleText;
	typeMap["RE"] = SgfVariant::SimpleText;
	typeMap["RO"] = SgfVariant::SimpleText;
	typeMap["RU"] = SgfVariant::SimpleText;
	typeMap["SO"] = SgfVariant::SimpleText;
	typeMap["TM"] = SgfVariant::Real;
	typeMap["US"] = SgfVariant::SimpleText;
	typeMap["WR"] = SgfVariant::SimpleText;
	typeMap["WT"] = SgfVariant::SimpleText;
	// Timing
	typeMap["BL"] = SgfVariant::Real;
	typeMap["OB"] = SgfVariant::Number;
	typeMap["OW"] = SgfVariant::Number;
	typeMap["WL"] = SgfVariant::Real;
	// Misc
	typeMap["FG"] = SgfVariant::Compose;
	typeMap["PM"] = SgfVariant::Number;
	typeMap["VW"] = SgfVariant::Move;
	// Go
	typeMap["HA"] = SgfVariant::Number;
	typeMap["KM"] = SgfVariant::Real;
	typeMap["TB"] = SgfVariant::Move;
	typeMap["TW"] = SgfVariant::Move;
	return typeMap;
}


