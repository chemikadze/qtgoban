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
			None = 0x0,
			Number,	// C++ int
			Real,	// C++ double
			Double,	// C++ qint8, parse as number
			Color,	// C++ StoneColor
		//	non-scalar types
			SimpleText,
			Text,
			Move,	// C++ QPair <qint8, qint8>
			Compose,// C++ QPair <SgfVariant,SgfVariant>
			List	// C++ QList <SgfVariant>
		};
private:
	Type m_type;
	void* m_data;
	void deleteData();
public:
	int toNumber()const;
	double toReal()const;
	qint8 toDouble()const;
	StoneColor toColor()const;
	QString toString()const;				// just data string, not features of Text
	QString toSgfRecordFormat()const;	// main save feature here
	QPair <qint8, qint8> toMove()const;
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
	SgfVariant(StoneColor t);
	SgfVariant(const QString& text, bool simple = true);
	SgfVariant(qint8 col, qint8 row);		// stone
	SgfVariant(const QPair<qint8,qint8>& t);// stone
	SgfVariant(const SgfVariant& first,		// compose
			   const SgfVariant& second);	//
	SgfVariant(const QPair<SgfVariant,SgfVariant>& t);
	SgfVariant(const QList<SgfVariant>& t);
	SgfVariant(const SgfVariant& t);

	~SgfVariant();
};

#endif // SGFVARIANT_H
