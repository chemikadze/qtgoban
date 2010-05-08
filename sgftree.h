#ifndef SGFTREE_H
#define SGFTREE_H

#include <QtCore/QVector>
#include <QtCore/QMultiHash>
#include <QtCore/QString>
#include <QtCore/QSet>
#include "sgfvariant.h"
#include "common.h"

class SgfTree
{
	QVector <SgfTree*> m_children;
	SgfTree *m_parent;
	QMultiHash <QString, SgfVariant> m_attr;
	quint16 m_moveIndex;
	QVector < QPair<Point,StoneColor> > m_killed;
	QSet < QPair<Point, StoneColor> > m_rewrites;

public:
	void addChild(SgfTree *child);
	void removeChild(SgfTree *child);
	inline SgfTree* parent() { return m_parent; }
	inline void setParent(SgfTree* newParent) { m_parent = newParent; }
	inline QVector <SgfTree*> children()const { return m_children; }
	SgfTree* child(int i);

	inline const QVector < QPair<Point,StoneColor> > killed() { return m_killed; }
	inline void addKilled(const Point &kill, StoneColor color) { m_killed.push_back(QPair<Point, StoneColor>(kill, color)); }

	inline void addRewrite(qint8 col, qint8 row, StoneColor color) { addRewrite(Point(col, row), color); }
	inline void addRewrite(Point p, StoneColor color) { m_rewrites.insert( QPair<Point, StoneColor>(p, color) ); }
	inline const QSet < QPair<Point, StoneColor> > & rewrites() { return m_rewrites; }
	inline void removeRewrite(QPair<Point, StoneColor> p) { m_rewrites.remove(p); }

	SgfVariant attrValue(const QString& attrname)const;
	QList<SgfVariant> attrValues(const QString& attrname);
	void setAttribute(const QString& attrname, SgfVariant val);
	void addAttribute(const QString& attrname, SgfVariant val);
	QMultiHash <QString, SgfVariant>& attributes();

	inline quint16 moveIndex() { return m_moveIndex; }
	inline void setMoveIndex(quint16 index) { m_moveIndex = index; }

	SgfTree(SgfTree *p = NULL);
	~SgfTree();
};

#endif // SGFTREE_H
