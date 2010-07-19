#include "sgftree.h"

static int i = 0;

SgfTree::SgfTree(SgfTree *p/* = NULL*/)
{
	m_parent = p;
	if ( p )
		m_moveIndex = m_parent->moveIndex() + 1;
	else
		m_moveIndex = 0;
	qDebug() << ++i;
}

SgfTree::~SgfTree()
{
	foreach (SgfTree *subtree, m_children)
		delete subtree;
	qDebug() << --i;
}

SgfTree* SgfTree::child(int i)
{
	return m_children.value(i, NULL);
}

void SgfTree::addChild(SgfTree *child)
{
	m_children.push_back(child);
	child->setParent(this);
}

bool SgfTree::removeChild(SgfTree *child)
{
	int idx = m_children.indexOf(child);
	if (idx>=0)
	{
		m_children.remove( idx );
		return true;
	}
	else
	{
		return false;
	}
}

SgfVariant SgfTree::attrValue(const QString &attrname)const
{
	return m_attr.value(attrname);
}

QList<SgfVariant> SgfTree::attrValues(const QString &attrname)const
{
	return m_attr.values(attrname);
}

void SgfTree::setAttribute(const QString &attrname, SgfVariant val)
{
	m_attr.replace(attrname, val);
}

void SgfTree::addAttribute(const QString &attrname, SgfVariant val)
{
	m_attr.insert(attrname, val);
}

QMultiHash <QString, SgfVariant>& SgfTree::attributes()
{
	return m_attr;
}

Stone SgfTree::move()
{
	Point p = attrValue("B").toPoint();
	if ( ! p.isNull())
	{
		return Stone(cBlack, p);
	}
	else
	{
		p = attrValue("W").toPoint();
		if ( ! p.isNull() )
		{
			return Stone(cWhite, p);
		}
		else
		{
			return Stone::null();
		}
	}
}

void SgfTree::setLine(Point from, Point to)
{
	addAttribute("LN", SgfVariant( SgfVariant(from), SgfVariant(to) ));
}

void SgfTree::removeLine(Point p)
{
	foreach(SgfVariant v, attrValues("LN"))
	{
		if ((v.toCompose().first.toPoint() == p) || (v.toCompose().second.toPoint() == p))
			m_attr.remove("LN", v);
	}
	foreach(SgfVariant v, attrValues("AR"))
	{
		if (v.toCompose().first.toPoint() == p || v.toCompose().second.toPoint() == p)
			m_attr.remove("LN", v);
	}
}

void SgfTree::setArrow(Point from, Point to)
{
	addAttribute("AR", SgfVariant( SgfVariant(from), SgfVariant(to) ));
}

void SgfTree::setLabel(Label lbl)
{
	foreach (SgfVariant var, m_attr.values("LB"))
	{
		if (var.toCompose().first == lbl.pos)
		{
			m_attr.remove("LB", var);
			break;
		}
	}

	addAttribute("LB", SgfVariant( SgfVariant(lbl.pos), SgfVariant(lbl.text) ));
}

void SgfTree::removeLineElement(Point from, Point to)
{
	m_attr.remove("AR", SgfVariant(SgfVariant(from), SgfVariant(to)));
	m_attr.remove("LN", SgfVariant(SgfVariant(from), SgfVariant(to)));
}

void SgfTree::removeLabel(Point pnt)
{
	foreach (SgfVariant var, m_attr.values("LB"))
	{
		if (var.toCompose().first == pnt)
		{
			m_attr.remove("LB", var);
			break;
		}
	}
}

void SgfTree::setStone(Stone s)
{
	if (!(m_attr.count("B", s.point) ||
		  m_attr.count("W", s.point)))
	{
		m_attr.remove("AE", s.point);
		m_attr.remove("AB", s.point);
		m_attr.remove("AW", s.point);
		if (s.color == cBlack)
		{
			addAttribute("AB", s.point);
		}
		else if (s.color == cWhite)
		{
			addAttribute("AW", s.point);
		}
		else
		{
			addAttribute("AE", s.point);
		}
	}
}

void SgfTree::setMark(Mark mark)
{
	// remove all old marks in this point
	QHash <Markup,QString>::const_iterator i;
	for (i=markupNames.constBegin(); i!=markupNames.constEnd(); ++i)
	{
		m_attr.remove(i.value(), SgfVariant(mark.pos));
	}
	if (mark.mark!=mVoid)
	{
		// add mark
		m_attr.insertMulti( markupNames.value(mark.mark), SgfVariant(mark.pos) );
	}
}

QList <Mark> SgfTree::marks()const
{
	QList <Mark> l;

	for (QHash<QString, Markup>::const_iterator i = namesMarkup.constBegin();i != namesMarkup.constEnd(); ++i)
	{
		foreach (SgfVariant variant, attrValues(i.key()))
		{
			if (variant.type() == SgfVariant::tPoint)
			{
				l.push_back(Mark(i.value(), variant.toPoint()));
			}
			else if (variant.type() == SgfVariant::tCompose)
			{
				QPair <SgfVariant, SgfVariant> compose = variant.toCompose();
				if (compose.first.type() == SgfVariant::tPoint && compose.second.type() == SgfVariant::tPoint)
				{
					Point from = compose.first.toPoint();
					Point to = compose.second.toPoint();
					for (int col=from.col; col<=to.col; ++col)
						for (int row=from.row; row<=to.row; ++row)
							l.push_back(Mark(i.value(), Point(col, row)));
				}
			}
		}
	}

	return l;
}

QList <Point> SgfTree::terrBlack()const
{
	QList <Point> l;

	foreach (SgfVariant variant, attrValues("TB"))
	{
		if (variant.type() == SgfVariant::tPoint)
		{
			l.push_back(variant.toPoint());
		}
		else if (variant.type() == SgfVariant::tCompose)
		{
			QPair <SgfVariant, SgfVariant> compose = variant.toCompose();
			if (compose.first.type() == SgfVariant::tPoint && compose.second.type() == SgfVariant::tPoint)
			{
				Point from = compose.first.toPoint();
				Point to = compose.second.toPoint();
				for (int col=from.col; col<=to.col; ++col)
					for (int row=from.row; row<=to.row; ++row)
						l.push_back(Point(col, row));
			}
		}
	}

	return l;
}

QList <Point> SgfTree::terrWhite()const
{
	QList <Point> l;

	foreach (SgfVariant variant, attrValues("TW"))
	{
		if (variant.type() == SgfVariant::tPoint)
		{
			l.push_back(variant.toPoint());
		}
		else if (variant.type() == SgfVariant::tCompose)
		{
			QPair <SgfVariant, SgfVariant> compose = variant.toCompose();
			if (compose.first.type() == SgfVariant::tPoint && compose.second.type() == SgfVariant::tPoint)
			{
				Point from = compose.first.toPoint();
				Point to = compose.second.toPoint();
				for (int col=from.col; col<=to.col; ++col)
					for (int row=from.row; row<=to.row; ++row)
						l.push_back(Point(col, row));
			}
		}
	}

	return l;
}

void SgfTree::setTerritory(Stone s)
{
	m_attr.remove("TB", s.point);
	m_attr.remove("TW", s.point);
	if (s.color == cBlack)
	{
		addAttribute("TB", s.point);
	}
	else if (s.color == cWhite)
	{
		addAttribute("TW", s.point);
	}
}

SgfTree::NodeAnnot SgfTree::nodeAnnot()
{
	if (m_attr.contains("GB"))
		return GoodForBlack;
	else if (m_attr.contains("GW"))
		return GoodForWhite;
	else if (m_attr.contains("DM"))
		return Even;
	else if (m_attr.contains("UC"))
		return Unclear;
	return naNone;
}

SgfTree::MoveAnnot SgfTree::moveAnnot()
{
	if (m_attr.contains("BM"))
		return Bad;
	else if (m_attr.contains("DO"))
		return Doubtful;
	else if (m_attr.contains("IT"))
		return Interesting;
	if (m_attr.contains("TE"))
		return Tesuji;
	return maNone;
}
