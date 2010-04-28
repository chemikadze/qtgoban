#ifndef SGFTREE_H
#define SGFTREE_H

#include <QtCore/QVector>
#include <QtCore/QMultiMap>
#include <QtCore/QString>
#include "sgfvariant.h"

class SgfTree
{
	QVector <SgfTree*> m_children;
	SgfTree *m_parent;
	QMultiMap <QString, SgfVariant> m_attr;
	quint16 m_moveIndex;

public:
	void addChild(SgfTree *child);
	void removeChild(SgfTree *child);
	inline SgfTree* parent() { return m_parent; }
	inline void setParent(SgfTree* newParent) { m_parent = newParent; }
	inline QVector <SgfTree*> children()const { return m_children; }
	SgfTree* child(int i);

/*
  multi-attr values are in ass, FFUU
  */
	SgfVariant attrValue(const QString& attrname)const;
	QList<SgfVariant> attrValues(const QString& attrname);
	void setAttribute(const QString& attrname, SgfVariant val);
	QMultiMap <QString, SgfVariant>& attributes();

	inline quint16 moveIndex() { return m_moveIndex; }
	inline void setMoveIndex(quint16 index) { m_moveIndex = index; }

	SgfTree(SgfTree *p = NULL);
	~SgfTree();
};

#endif // SGFTREE_H
