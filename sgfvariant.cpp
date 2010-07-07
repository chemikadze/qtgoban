#include "sgfvariant.h"

SgfVariant::SgfVariant()
{
	m_type = tNone;
	m_data = NULL;
}

SgfVariant::SgfVariant(int t)
{
	m_type = tNumber;
	m_data = new int(t);
}

SgfVariant::SgfVariant(double t)
{
	m_type = tReal;
	m_data = new double(t);
}

// SGF Double
SgfVariant::SgfVariant(bool t)
{
	m_type = tDouble;
	m_data = new qint8(1 + t);
}

// SGF Color
SgfVariant::SgfVariant(Color t)
{
	m_type = tColor;
	if (t < cVoid || t > cWhite)
		t = cVoid;
	m_data = new Color(t);
}

// SGF Text or Simple Text
SgfVariant::SgfVariant(const QString &text, bool simple /*=true*/)
{
	m_type = simple ? tSimpleText : tText;
	m_data = new QString(text);
}

// SGF Move (Go standard)
SgfVariant::SgfVariant(qint8 col, qint8 row)
{
	m_type = tPoint;
	m_data = new Point (col, row);
}

// SGF Move (Go Standard)
SgfVariant::SgfVariant(const Point t)
{
	m_type = tPoint;
	m_data = new Point(t);
}

// SGF Compose
SgfVariant::SgfVariant(const SgfVariant &first, const SgfVariant &second)
{
	m_type = tCompose;
	m_data = new QPair<SgfVariant,SgfVariant>(first, second);
}

// SGF Compose
SgfVariant::SgfVariant(const QPair<SgfVariant, SgfVariant> &t)
{
	m_type = tCompose;
	m_data = new QPair<SgfVariant, SgfVariant>(t.first, t.second);
}

// SGF List
SgfVariant::SgfVariant(const QList<SgfVariant> &t)
{
	m_type = tList;
	m_data = new QList<SgfVariant>(t);
}

SgfVariant::SgfVariant(const SgfVariant &t)
{
	m_type = tNone;
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
	case tNumber:
		m_data = new int( *(int*)t.m_data );
		break;
	case tReal:
		m_data = new double( *(double*)t.m_data );
		break;
	case tDouble:
		m_data = new qint8( *(qint8*)t.m_data );
		break;
	case tColor:
		m_data = new Color( *(Color*)t.m_data );
		break;
	case tSimpleText:
	case tText:
		m_data = new QString(*(QString*)t.m_data);
		break;
	case tPoint:
	{
			Point b = *(Point*)t.m_data;
		m_data = new Point( b.col, b.row );
		break;
	}
	case tCompose:
	{
		QPair <SgfVariant, SgfVariant> b = *(QPair <SgfVariant, SgfVariant>*)t.m_data;
		m_data = new QPair <SgfVariant, SgfVariant>( b.first, b.second );
		break;
	}
	case tList:
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
	case tNumber:
		delete (int*)m_data;
		break;
	case tReal:
		delete (double*)m_data;
		break;
	case tDouble:
		delete (qint8*)m_data;
		break;
	case tColor:
		delete (Color*)m_data;
		break;
	case tSimpleText:
	case tText:
		delete (QString*)m_data;
		break;
	case tPoint:
	{
		delete (Point*)m_data;
		break;
	}
	case tCompose:
	{
		delete (QPair <SgfVariant, SgfVariant>*)m_data;
		break;
	}
	case tList:
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
	if (m_type == tNumber)
		return *(int*)m_data;
	else if (m_type == tReal)
		return int(*(double*)m_data);
	return 0;
}

double SgfVariant::toReal()const
{
	if (m_type == tNumber)
		return double(*(int*)m_data);
	else if (m_type == tReal)
		return *(double*)m_data;
	return 0;
}

qint8 SgfVariant::toDouble()const
{
	if (m_type == tNumber)
		return *(int*)m_data ? 2 : 1;
	else if (m_type == tDouble)
		return *(qint8*)m_data;
	return 1;
}

QString SgfVariant::toString()const
{
	switch (m_type)
	{
	case tNumber:
		return QString::number(*(int*)m_data);
	case tReal:
		return QString::number(*(double*)m_data, 'f');
	case tDouble:
		return QString::number(*(qint8*)m_data);
	case tColor:
		switch (*(Color*)m_data)
		{
		case cBlack:
			return QString("B");
		case cWhite:
			return QString("W");
		default:
			return QString();
		}
	case tSimpleText:
	case tText:
		return *(QString*)m_data;
	case tPoint:
	{
		Point b = *(Point*)m_data;
		if (b.col < 0 || b.col > 52 ||
			b.row < 0 || b.row > 52)
			return QString();
		char s[3] = "aa";
		if (b.col > 26)
			s[0] = 'A' + b.col - 27;
		else
			s[0] += b.col;
		if (b.row > 26)
			s[1] = 'A' + b.row - 27;
		else
			s[1] += b.row;
		return QString(s);
	}
	case tCompose:
	{
		QPair <SgfVariant,SgfVariant> b = *(QPair <SgfVariant,SgfVariant>*)m_data;
		return QString("%1:%2").arg( b.first.toString(), b.second.toString() );
	}
	case tList:
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
	if (m_type == tText)
	{
		QString rec = *(QString*)m_data;
		rec.replace(QRegExp("(\\\\|\\])"), "\\\\1");
		return rec;
	}
	else if (m_type == tSimpleText)
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
		return SgfVariant(Point::pass());
	else
		return SgfVariant();

	return SgfVariant(col, row);
}

Point SgfVariant::toPoint()const
{
	if (m_type == tPoint)
		return *(Point*)m_data;
	else
		return Point();
}

Color SgfVariant::toColor()const
{
	if (m_type == tColor)
		return *(Color*)m_data;
	else
		return cVoid;
}

QPair <SgfVariant,SgfVariant> SgfVariant::toCompose()const
{
	if ( m_type == tCompose )
		return *(QPair <SgfVariant,SgfVariant>*)m_data;
	else
		return QPair <SgfVariant,SgfVariant>();
}

QList <SgfVariant> SgfVariant::toList()const
{
	if ( m_type == tList )
	{
		return *(QList <SgfVariant>*)m_data;
	}
	else if ( m_type == tCompose )
	{
		QList <SgfVariant> ret;
		ret.push_back( ((QPair <SgfVariant,SgfVariant>*)m_data)->first );
		ret.push_back( ((QPair <SgfVariant,SgfVariant>*)m_data)->second );
		return ret;
	}
	return QList<SgfVariant>();
}


bool SgfVariant::operator ==(const SgfVariant& t)const
{
	if (m_type != t.m_type)
		return false;
	switch (m_type)
	{
	case tNone:
		return true;
	case tNumber:
		return *(int*)m_data == *(int*)t.m_data;
	case tReal:
		return *(double*)m_data == *(double*)t.m_data;
	case tDouble:
		return *(qint8*)m_data == *(qint8*)t.m_data;
	case tColor:
		return *(Color*)m_data == *(Color*)t.m_data;
	case tSimpleText:
		return *(QString*)m_data == *(QString*)t.m_data;
	case tText:
		return *(QString*)m_data == *(QString*)t.m_data;
	case tPoint:
		return *(Point*)m_data == *(Point*)t.m_data;
	case tCompose:
		return *(QPair <SgfVariant,SgfVariant>*)m_data == *(QPair <SgfVariant,SgfVariant>*)t.m_data;
	case tList:
		return *(QList <SgfVariant>*)m_data == *(QList <SgfVariant>*)t.m_data;
	default:
		return false;
	}
}
