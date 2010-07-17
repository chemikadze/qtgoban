#include "common.h"

QHash <Markup, QString> getMarkupNames();
QHash <QString, Markup> getNamesMarkup();

const QHash <Markup, QString> markupNames = getMarkupNames();
extern const QHash <QString, Markup> namesMarkup = getNamesMarkup();

QHash <Markup, QString> getMarkupNames()
{
	QHash <Markup, QString> hash;

	hash[mCircle] = "CR";
	hash[mCross] = "MA";
	hash[mSquare] = "SQ";
	hash[mTriangle] = "TR";
	hash[mSelection] = "SL";

	return hash;
}

QHash <QString, Markup> getNamesMarkup()
{
	QHash <QString, Markup> hash;

	hash["CR"] = mCircle;
	hash["MA"] = mCross;
	hash["SQ"] = mSquare;
	hash["TR"] = mTriangle;
	hash["SL"] = mSelection;

	return hash;
}

QVector <Point> getUpDownLeftRight()
{
	return QVector<Point>() << Point(0, -1)
							<< Point(0, +1)
							<< Point(-1, 0)
							<< Point(+1, 0);
}
