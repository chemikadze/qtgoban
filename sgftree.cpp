#include "sgftree.h"

SgfTree::SgfTree(SgfTree *p/* = NULL*/)
{
	m_parent = p;
	if ( p )
		m_moveIndex = m_parent->moveIndex() + 1;
	else
		m_moveIndex = 0;
}

SgfTree::~SgfTree()
{
	foreach (SgfTree *subtree, m_children)
		delete subtree;
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

void SgfTree::removeChild(SgfTree *child)
{
	m_children.remove( m_children.indexOf(child) );
}

SgfVariant SgfTree::attrValue(const QString &attrname)const
{
	return m_attr.value(attrname);
}

QList<SgfVariant> SgfTree::attrValues(const QString &attrname)
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
	setAttribute("LN", SgfVariant( SgfVariant(from), SgfVariant(to) ));
}

void SgfTree::setArrow(Point from, Point to)
{
	setAttribute("AR", SgfVariant( SgfVariant(from), SgfVariant(to) ));
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

	setAttribute("LB", SgfVariant( SgfVariant(lbl.pos), SgfVariant(lbl.text) ));
}

void SgfTree::deleteLineElement(Point from, Point to)
{
	m_attr.remove("AR", SgfVariant(SgfVariant(from), SgfVariant(to)));
	m_attr.remove("LN", SgfVariant(SgfVariant(from), SgfVariant(to)));
}

void SgfTree::deleteLabel(Point pnt)
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
