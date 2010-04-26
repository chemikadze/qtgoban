#include "sgfvariant.h"

SgfVariant::SgfVariant()
{
	m_type = None;
	m_data = NULL;
}

SgfVariant::SgfVariant(int t)
{
	m_type = Number;
	m_data = new int(t);
}

SgfVariant::SgfVariant(double t)
{
	m_type = Real;
	m_data = new double(t);
}

// SGF Double
SgfVariant::SgfVariant(bool t)
{
	m_type = Double;
	m_data = new qint8(1 + t);
}

// SGF Color
SgfVariant::SgfVariant(StoneColor t)
{
	m_type = Color;
	if (t < StoneVoid || t > StoneWhite)
		t = StoneVoid;
	m_data = new StoneColor(t);
}

// SGF Text or Simple Text
SgfVariant::SgfVariant(const QString &text, bool simple /*=true*/)
{
	m_type = simple ? SimpleText : Text;
	m_data = new QString(text);
}

// SGF Move (Go standard)
SgfVariant::SgfVariant(qint8 col, qint8 row)
{
	m_type = Move;
	m_data = new QPair<qint8,qint8> (col, row);
}

// SGF Move (Go Standard)
SgfVariant::SgfVariant(const QPair<qint8, qint8>& t)
{
	m_type = Move;
	m_data = new QPair<qint8,qint8>(t.first, t.second);
}

// SGF Compose
SgfVariant::SgfVariant(const SgfVariant &first, const SgfVariant &second)
{
	m_type = Compose;
	m_data = new QPair<SgfVariant,SgfVariant>(first, second);
}

// SGF Compose
SgfVariant::SgfVariant(const QPair<SgfVariant, SgfVariant> &t)
{
	m_type = Compose;
	m_data = new QPair<SgfVariant, SgfVariant>(t.first, t.second);
}

// SGF List
SgfVariant::SgfVariant(const QList<SgfVariant> &t)
{
	m_type = List;
	m_data = new QList<SgfVariant>(t);
}

SgfVariant::SgfVariant(const SgfVariant &t)
{
	m_type = None;
	m_data = NULL;
	*this = t;
}

SgfVariant& SgfVariant::operator= (const SgfVariant& t)
{
	if (t.m_data == this->m_data)
		return *this;

	deleteData();
	m_type = t.m_type;
	switch (m_type)
	{
	case Number:
		m_data = new int( *(int*)t.m_data );
		break;
	case Real:
		m_data = new double( *(double*)t.m_data );
		break;
	case Double:
		m_data = new qint8( *(qint8*)t.m_data );
		break;
	case Color:
		m_data = new StoneColor( *(StoneColor*)t.m_data );
		break;
	case SimpleText:
	case Text:
		m_data = new QString(*(QString*)t.m_data);
		break;
	case Move:
	{
		QPair <qint8, qint8> b = *(QPair <qint8, qint8>*)t.m_data;
		m_data = new QPair <qint8, qint8>( b.first, b.second );
		break;
	}
	case Compose:
	{
		QPair <SgfVariant, SgfVariant> b = *(QPair <SgfVariant, SgfVariant>*)t.m_data;
		m_data = new QPair <SgfVariant, SgfVariant>( b.first, b.second );
		break;
	}
	case List:
		m_data = new QList<SgfVariant>(*(QList <SgfVariant>*)t.m_data);
		break;
	default:
		m_data = NULL;
	}

	return *this;
}

void SgfVariant::deleteData()
{
	switch (m_type)
	{
	case Number:
		delete (int*)m_data;
		break;
	case Real:
		delete (double*)m_data;
		break;
	case Double:
		delete (qint8*)m_data;
		break;
	case Color:
		delete (StoneColor*)m_data;
		break;
	case SimpleText:
	case Text:
		delete (QString*)m_data;
		break;
	case Move:
	{
		delete (QPair <qint8, qint8>*)m_data;
		break;
	}
	case Compose:
	{
		delete (QPair <SgfVariant, SgfVariant>*)m_data;
		break;
	}
	case List:
		delete (QList <SgfVariant>*)m_data;
		break;
	default:
		free(m_data);
	}
}

SgfVariant::~SgfVariant()
{
	deleteData();
}


SgfVariant::Type SgfVariant::type()const
{
	return m_type;
}

int SgfVariant::toNumber()const
{
	if (m_type == Number)
		return *(int*)m_data;
	else if (m_type == Real)
		return int(*(double*)m_data);
	return 0;
}

double SgfVariant::toReal()const
{
	if (m_type == Number)
		return double(*(int*)m_data);
	else if (m_type == Real)
		return *(double*)m_data;
	return 0;
}

qint8 SgfVariant::toDouble()const
{
	if (m_type == Number)
		return *(int*)m_data ? 2 : 1;
	else if (m_type == Double)
		return *(qint8*)m_data;
	return 1;
}

QString SgfVariant::toString()const
{
	switch (m_type)
	{
	case Number:
		return QString::number(*(int*)m_data);
	case Real:
		return QString::number(*(double*)m_data, 'f');
	case Double:
		return QString::number(*(qint8*)m_data);
	case Color:
		switch (*(StoneColor*)m_data)
		{
		case StoneBlack:
			return QString("B");
		case StoneWhite:
			return QString("W");
		default:
			return QString();
		}
	case SimpleText:
	case Text:
		return *(QString*)m_data;
	case Move:
	{
		QPair <qint8,qint8> b = *(QPair <qint8,qint8>*)m_data;
		if (b.first < 0 || b.first > 52 ||
			b.second < 0 || b.second > 52)
			return QString();
		char s[3] = "aa";
		if (b.first > 26)
			s[0] = 'A' + b.first - 27;
		else
			s[0] += b.first;
		if (b.second > 26)
			s[1] = 'A' + b.second - 27;
		else
			s[1] += b.second;
		return QString(s);
	}
	case Compose:
	{
		QPair <SgfVariant,SgfVariant> b = *(QPair <SgfVariant,SgfVariant>*)m_data;
		return QString("%1:%2").arg( b.first.toString(), b.second.toString() );
	}
	case List:
	{
		QString res, pattern("[%1]");
		QList <SgfVariant> list = *(QList <SgfVariant>*)m_data;
		for (int i=0; i<list.size(); i++)
			res += pattern.arg(list[i].toString());
		return res;
	}
	default:
		return QString();
	}
}

QString SgfVariant::toSgfRecordFormat()const
{
	if (m_type == Text)
	{
		QString rec = *(QString*)m_data;
		rec.replace(QRegExp("(\\\\|\\])"), "\\\\1");
		return rec;
	}
	else if (m_type == SimpleText)
	{
		QString rec = *(QString*)m_data;
		rec.replace(QRegExp("(\\:|\\\\|\\])"), "\\\\1");
		return rec;
	}

	return toString();
}

SgfVariant SgfVariant::strToMove(const QString& s)
{
	static const QChar l_a('a'), l_z('z'), l_A('A'), l_Z('Z');
	qint8 col, row;
	if (s.size() == 2)
	{
		if (s[0] >= l_a && s[0] <=l_z)
			col = s[0].toLatin1() - 'a';
		else if (s[0] >= l_A && s[0] <= l_Z)
			col = s[0].toLatin1() - 'A' + 27;
		else
			return SgfVariant();

		if (s[1] >= l_a && s[1] <=l_z)
			row = s[1].toLatin1() - 'a';
		else if (s[1] >= l_A && s[0] <= l_Z)
			row = s[1].toLatin1() - 'A' + 27;
		else
			return SgfVariant();
	}
	else if (s.size() == 0)
		col = row = -1;
	else
		return SgfVariant();

	return SgfVariant(col, row);
}

QPair <qint8, qint8> SgfVariant::toMove()const
{
	if (m_type == Move)
		return *(QPair <qint8, qint8>*)m_data;
	else
		return QPair<qint8,qint8>(0,0);
}

QPair <SgfVariant,SgfVariant> SgfVariant::toCompose()const
{
	if ( m_type == Compose )
		return *(QPair <SgfVariant,SgfVariant>*)m_data;
	else
		return QPair <SgfVariant,SgfVariant>();
}

QList <SgfVariant> SgfVariant::toList()const
{
	if ( m_type == List )
	{
		return *(QList <SgfVariant>*)m_data;
	}
	else if ( m_type == Compose )
	{
		QList <SgfVariant> ret;
		ret.push_back( ((QPair <SgfVariant,SgfVariant>*)m_data)->first );
		ret.push_back( ((QPair <SgfVariant,SgfVariant>*)m_data)->second );
		return ret;
	}
	return QList<SgfVariant>();
}
