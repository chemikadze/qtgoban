#ifndef SGFVARIANT_H
#define SGFVARIANT_H

#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QPair>
#include <QtCore/QList>
#include <cstdlib>
#include "common.h"

#ifdef DEBUG
#include <QtCore/QDebug>
#endif

class SgfVariant
{
public:
	enum Type {
		// scalar types
			tNone = 0x0,
			tNumber,	// C++ int
			tReal,	// C++ double
			tDouble,	// C++ qint8, parse as number
			tColor,	// C++ StoneColor
		//	non-scalar types
			tSimpleText,
			tText,
			tPoint,	// C++ Point
			tCompose,// C++ QPair <SgfVariant,SgfVariant>
			tList	// C++ QList <SgfVariant>
		};
private:
	Type m_type;
	void* m_data;
	void deleteData();
public:
	int toNumber()const;
	double toReal()const;
	qint8 toDouble()const;
	Color toColor()const;
	QString toString()const;				// just data string, not features of Text
	QString toSgfRecordFormat()const;	// main save feature here
	Point toPoint()const;
	QPair <SgfVariant,SgfVariant> toCompose()const;
	QList <SgfVariant> toList()const;

	static SgfVariant strToMove(const QString& s);

	Type type()const;

	SgfVariant& operator= (const SgfVariant& t);
	bool operator== (const SgfVariant& t)const;

    SgfVariant();
	SgfVariant(int t);
	SgfVariant(double t);
	SgfVariant(bool t); // double
	SgfVariant(Color t);
	SgfVariant(const QString& text, bool simple = true);
	SgfVariant(qint8 col, qint8 row);		// stone
	SgfVariant(const Point t);// stone
	SgfVariant(const SgfVariant& first,		// compose
			   const SgfVariant& second);	//
	SgfVariant(const QPair<SgfVariant,SgfVariant>& t);
	SgfVariant(const QList<SgfVariant>& t);
	SgfVariant(const SgfVariant& t);

	~SgfVariant();
};

#endif // SGFVARIANT_H
