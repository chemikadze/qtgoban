#include "common.h"

QHash <Markup, QString> getMarkupNames();

const QHash <Markup, QString> markupNames = getMarkupNames();


QHash <Markup, QString> getMarkupNames()
{
	QHash <Markup, QString> hash;

	hash[MCircle] = "CR";
	hash[MCross] = "MA";
	hash[MSquare] = "SQ";
	hash[MTriangle] = "TR";
	hash[MSelection] = "SL";
	hash[MTerrBlack] = "TB";
	hash[MTerrWhite] = "TW";

	return hash;
}
