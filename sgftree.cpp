#include "sgftree.h"

SgfTree::SgfTree(SgfTree *p/* = NULL*/) : m_parent(p)
{
	if ( NULL != p )
		m_moveIndex = m_parent->moveIndex() + 1;
	else
		m_moveIndex = 0;
}

SgfTree::~SgfTree()
{
	foreach (SgfTree *subtree, m_children)
		delete subtree;
}


void SgfTree::addChild(SgfTree *child)
{
	m_children.push_back(child);
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
	m_attr.insert(attrname, val);
}

QVector <SgfTree*> & SgfTree::children()
{
	return m_children;
}

SgfTree *SgfTree::child(int i)
{
	return m_children.value(i, NULL);
}

QMultiMap <QString, SgfVariant>& SgfTree::attributes()
{
	return m_attr;
}

quint16 SgfTree::moveIndex()
{
	return m_moveIndex;
}

void SgfTree::setMoveIndex(quint16 index)
{
	m_moveIndex = index;
}
