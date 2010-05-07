#ifndef COMMON_H
#define COMMON_H

#include <QtCore/QVector>
#include <QtCore/QSize>

#define VERSION_STRING "QGoban:0.1"
#define VERSION 0.1

enum StoneColor { StoneVoid = 0x0, StoneBlack = 0x1, StoneWhite = 0x2, StoneBoth = 0x3};
enum Markup { MVoid = 0x0, MCircle, MCross, MSquare, MTriangle, MSelection };

typedef QPair<qint8, qint8> Point;

template <typename T>
void resizeMatrix(QVector< QVector<T> > &v, QSize newSize, T defaultValue)
{
	QSize oldSize(v.size(),
				  v.size()>0 ? v[0].size() : 0);

	v.resize(newSize.height());
	for (int i=0; i<oldSize.height(); ++i)
	{
		v[i].resize(newSize.width());
		for (int j=oldSize.width(); j<newSize.width(); ++j)
			v[i][j] = defaultValue;
	}

	for (int i=oldSize.height(); i<newSize.height(); ++i)
		v[i] = QVector <T> (newSize.width(), defaultValue);
}


#endif // COMMON_H
