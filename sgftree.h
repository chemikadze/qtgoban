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
	quint16 m_moveIndex;	// MN
	QVector <SgfTree*> m_children;
	SgfTree *m_parent;
	QMultiHash <QString, SgfVariant> m_attr;
	Annotation m_annot;	// DB GB GW

// start new
//	Stone m_move;
#ifdef FULL_MVC
	bool m_force; // KO
//	QVector <Stone> m_setting;

	Color m_turn;
	QString m_comment;
	QString m_name; // N
	//Annotation m_annot;
	bool m_hotspot;	// HO
	double m_value; // V

	bool m_bad;	// BM
	bool m_doubtfool; // DO
	bool m_interesting; // IT
	bool m_tesuji;	// TE

	QVector <Mark> m_marks;
	QVector <Point> m_dimm;
	QVector <Line> m_lines;
#endif
// end new

public:

	inline /* orly? */ SgfVariant getMoveVariant()
	{
		SgfVariant var = m_attr.value("B");
		if (var.type() == SgfVariant::tPoint)
			return var;
		else
		{
			var = m_attr.value("W");
			if (var.type() == SgfVariant::tPoint)
				return var;
			else
				return SgfVariant();
		}
	}

	SgfVariant attrValue(const QString& attrname)const;
	QList<SgfVariant> attrValues(const QString& attrname);
	void setAttribute(const QString& attrname, SgfVariant val);
	void addAttribute(const QString& attrname, SgfVariant val);
	QMultiHash <QString, SgfVariant>& attributes();

	// getters and setters
#ifdef FULL_MVC
	inline void setAnnotation(Annotation annot) { m_annot = annot; }
	inline Annotation annotation() { return m_annot; }
#endif

	Stone move();

	inline quint16 moveIndex() { return m_moveIndex; }
	inline void setMoveIndex(quint16 index) { m_moveIndex = index; }

	inline QString moveName() { return attrValue("N").toString(); }

	void addChild(SgfTree *child);
	void removeChild(SgfTree *child);
	inline SgfTree* parent() { return m_parent; }
	inline void setParent(SgfTree* newParent) { m_parent = newParent; }
	inline QVector <SgfTree*> children()const { return m_children; }
	SgfTree* child(int i);

	void setLine(Point from, Point to);
	void setArrow(Point from, Point to);
	void deleteLineElement(Point from, Point to);

	void setLabel(Label lbl);
	void deleteLabel(Point pnt);

	SgfTree(SgfTree *p = NULL);
	~SgfTree();
};

#endif // SGFTREE_H
