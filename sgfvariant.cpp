#include "sgfvariant.h"

SgfVariant::SgfVariant()
{
	d = 0;
}

SgfVariant::SgfVariant(int t)
{
	d = new Data;
	d->refs = 1;
	d->m_type = tNumber;
	d->m_data = new int(t);
}

SgfVariant::SgfVariant(double t)
{
	d = new Data;
	d->refs = 1;
	d->m_type = tReal;
	d->m_data = new double(t);
}

// SGF Double
SgfVariant::SgfVariant(bool t)
{
	d = new Data;
	d->refs = 1;
	d->m_type = tDouble;
	d->m_data = new qint8(1 + t);
}

// SGF Color
SgfVariant::SgfVariant(Color t)
{
	d = new Data;
	d->refs = 1;
	d->m_type = tColor;
	if (t < cVoid || t > cWhite)
		t = cVoid;
	d->m_data = new Color(t);
}

// TODO: checkup linebreaks
// SGF Text or Simple Text
SgfVariant::SgfVariant(const QString &text, bool simple /*=true*/)
{
	d = new Data;
	d->refs = 1;
	d->m_type = simple ? tSimpleText : tText;
	QString *data = new QString(text);
	data->replace(QRegExp("(\\\\n\\r|\\\\r\\n|\\\\r|\\\\n)"), "");
	d->m_data = data;
}

// SGF Move (Go standard)
SgfVariant::SgfVariant(qint8 col, qint8 row)
{
	d = new Data;
	d->refs = 1;
	d->m_type = tPoint;
	d->m_data = new Point (col, row);
}

// SGF Move (Go Standard)
SgfVariant::SgfVariant(const Point t)
{
	d = new Data;
	d->refs = 1;
	d->m_type = tPoint;
	d->m_data = new Point(t);
}

// SGF Compose
SgfVariant::SgfVariant(const SgfVariant &first, const SgfVariant &second)
{
	d = new Data;
	d->refs = 1;
	d->m_type = tCompose;
	d->m_data = new QPair<SgfVariant,SgfVariant>(first, second);
}

// SGF Compose
SgfVariant::SgfVariant(const QPair<SgfVariant, SgfVariant> &t)
{
	d = new Data;
	d->refs = 1;
	d->m_type = tCompose;
	d->m_data = new QPair<SgfVariant, SgfVariant>(t.first, t.second);
}

// SGF List
SgfVariant::SgfVariant(const QList<SgfVariant> &t)
{
	d = new Data;
	d->refs = 1;
	d->m_type = tList;
	d->m_data = new QList<SgfVariant>(t);
}

SgfVariant::SgfVariant(const SgfVariant &t)
{
	d = t.d;
	if (d)
		++d->refs;
}

SgfVariant& SgfVariant::operator= (const SgfVariant& t)
{
	deleteData();
	d = t.d;
	if (d)
		++d->refs;

	return *this;
}

void SgfVariant::deleteData()
{
	if (!d)
		return;

	// if it is in use
	if ( --(d->refs) )
	{
		d = 0;
		return;
	}

	switch (d->m_type)
	{
	case tNumber:
		delete (int*)d->m_data;
		break;
	case tReal:
		delete (double*)d->m_data;
		break;
	case tDouble:
		delete (qint8*)d->m_data;
		break;
	case tColor:
		delete (Color*)d->m_data;
		break;
	case tSimpleText:
	case tText:
		delete (QString*)d->m_data;
		break;
	case tPoint:
	{
		delete (Point*)d->m_data;
		break;
	}
	case tCompose:
	{
		delete (QPair <SgfVariant, SgfVariant>*)d->m_data;
		break;
	}
	case tList:
		delete (QList <SgfVariant>*)d->m_data;
		break;
	default:
		free(d->m_data);
	}
	delete d;
	d = 0;
}

SgfVariant::~SgfVariant()
{
	deleteData();
}

int SgfVariant::toNumber()const
{
	if ( type() == tNumber)
		return *(int*)d->m_data;
	else if ( type() == tReal)
		return int(*(double*)d->m_data);
	return 0;
}

double SgfVariant::toReal()const
{
	if ( type() == tNumber)
		return double(*(int*)d->m_data);
	else if ( type() == tReal)
		return *(double*)d->m_data;
	return 0;
}

qint8 SgfVariant::toDouble()const
{
	if ( type() == tNumber)
		return *(int*)d->m_data ? 2 : 1;
	else if ( type() == tDouble)
		return *(qint8*)d->m_data;
	return 1;
}

QString SgfVariant::toString()const
{
	if (!d)
		return QString();
	switch (d->m_type)
	{
	case tNumber:
		return QString::number(*(int*)d->m_data);
	case tReal:
		return QString::number(*(double*)d->m_data, 'f');
	case tDouble:
		return QString::number(*(qint8*)d->m_data);
	case tColor:
		switch (*(Color*)d->m_data)
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
		return *(QString*)d->m_data;
	case tPoint:
	{
		Point b = *(Point*)d->m_data;
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
		QPair <SgfVariant,SgfVariant> b = *(QPair <SgfVariant,SgfVariant>*)d->m_data;
		return QString("%1:%2").arg( b.first.toString(), b.second.toString() );
	}
	case tList:
	{
		QString res, pattern("[%1]");
		QList <SgfVariant> list = *(QList <SgfVariant>*)d->m_data;
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
	if ( type() == tText)
	{
		QString rec = *(QString*)d->m_data;
		rec.replace(QRegExp("(\\\\|\\])"), "\\\\1");
		return rec;
	}
	else if ( type() == tSimpleText)
	{
		QString rec = *(QString*)d->m_data;
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
	if ( type() == tPoint)
		return *(Point*)d->m_data;
	else
		return Point();
}

Color SgfVariant::toColor()const
{
	if ( type() == tColor)
		return *(Color*)d->m_data;
	else
		return cVoid;
}

QPair <SgfVariant,SgfVariant> SgfVariant::toCompose()const
{
	if ( type() == tCompose )
		return *(QPair <SgfVariant,SgfVariant>*)d->m_data;
	else
		return QPair <SgfVariant,SgfVariant>();
}

QList <SgfVariant> SgfVariant::toList()const
{
	if ( type() == tList )
	{
		return *(QList <SgfVariant>*)d->m_data;
	}
	else if (type() == tCompose )
	{
		QList <SgfVariant> ret;
		ret.push_back( ((QPair <SgfVariant,SgfVariant>*)d->m_data)->first );
		ret.push_back( ((QPair <SgfVariant,SgfVariant>*)d->m_data)->second );
		return ret;
	}
	return QList<SgfVariant>();
}


bool SgfVariant::operator ==(const SgfVariant& t)const
{
	if (d == t.d)
		return true;
	if ( (d && !t.d) || (!d && t.d) )
		return false;
	if (d->m_type != t.d->m_type)
		return false;
	switch (type())
	{
	case tNone:
		return true;
	case tNumber:
		return *(int*)d->m_data == *(int*)t.d->m_data;
	case tReal:
		return *(double*)d->m_data == *(double*)t.d->m_data;
	case tDouble:
		return *(qint8*)d->m_data == *(qint8*)t.d->m_data;
	case tColor:
		return *(Color*)d->m_data == *(Color*)t.d->m_data;
	case tSimpleText:
		return *(QString*)d->m_data == *(QString*)t.d->m_data;
	case tText:
		return *(QString*)d->m_data == *(QString*)t.d->m_data;
	case tPoint:
		return *(Point*)d->m_data == *(Point*)t.d->m_data;
	case tCompose:
		return *(QPair <SgfVariant,SgfVariant>*)d->m_data == *(QPair <SgfVariant,SgfVariant>*)t.d->m_data;
	case tList:
		return *(QList <SgfVariant>*)d->m_data == *(QList <SgfVariant>*)t.d->m_data;
	default:
		return false;
	}
}
