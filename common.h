#ifndef COMMON_H
#define COMMON_H

#include <QtCore/QVector>
#include <QtCore/QSize>
#include <QtCore/QHash>

#define VERSION_STRING "QGoban:0.1"
#define VERSION 0.1

enum Color { cVoid = 0x0, cBlack = 0x1, cWhite = 0x2, cBoth = 0x3};
enum Markup { mVoid = 0x0,
			mCircle,
			mCross,
			mSquare,
			mTriangle,
			mSelection
};
enum Annotation {
			maNone = 0x0,
			maGoodForBlack,
			maGoodForWhite,
			maEven,				// DM
			maUnclear			// UC
};

enum LineStyle {
	lsLine = 0x1,
	lsArrow
};

// position at board
class Point {
public:
	qint8 col;
	qint8 row;
	inline const Point operator+(const Point p)const { return Point(col+p.col, row+p.row); }
	inline const Point operator-(const Point p)const { return Point(col-p.col, row-p.row); }
	inline bool operator ==(const Point second)const { return second.col == col && second.row == row; }
	inline bool isNull()const { return *this == null(); }
	inline bool isPass()const { return *this == pass(); }

	static inline Point null() { return Point(-2, -2); };
	static inline Point pass() { return Point(-1, -1); };

	inline Point(qint8 c, qint8 r) : col(c), row(r) { }
	inline Point() : col(-2), row(-2) { }
};

inline uint qHash(Point p) { return qHash(QPair<qint8,qint8>(p.col, p.row)); }


// color and position
class Stone {
public:

	Color color;
	Point point;

	inline bool operator ==(const Stone& s)const { return color == s.color && point == s.point; }
	inline bool isNull() { return *this == null(); }

	static inline Stone null() { return Stone(); }

	inline Stone() : color(cVoid), point() { }
	inline Stone(Color c, Point p) : color(c), point(p) { }
};

inline uint qHash(const Stone &p) { return qHash(QPair<Color,Point>(p.color, p.point)); }

// labels on board
class Mark {
public:
	Markup mark;
	Point pos;
	inline bool operator == (const Mark& mrk)const { return mark==mrk.mark && pos==mrk.pos; }
	inline Mark(): mark(), pos() {}
	inline Mark(Markup mrk, Point ps) : mark(mrk), pos(ps) { }
};

// labels on board
class Label {
public:
	QString text;
	Point pos;
	inline bool operator == (const Label& lbl)const { return text==lbl.text && pos==lbl.pos; }
	inline Label(): text(), pos() {}
	inline Label(QString txt, Point ps) : text(txt), pos(ps) { }
};

inline uint qHash(const Label &p) { return qHash(QPair<QString,Point>(p.text, p.pos)); }

// lines of markup
class Line {
public:
	Point from;
	Point to;
	LineStyle style;
	inline bool operator == (const Line& ln)const { return from==ln.from && to==ln.to && style==ln.style; }
	inline Line() : from(), to(), style(lsLine) {}
	inline Line(Point f, Point t, LineStyle ls) : from(f), to(t), style(ls) {}
};

inline uint qHash(const Line &p) { return qHash( QPair< QPair<Point,Point>, uint >(QPair<Point,Point>(p.from, p.to), uint(p.style)) ); }

template <typename T>
void resizeMatrix(QVector< QVector<T> > &v, QSize newSize, const T& defaultValue)
{
	QSize oldSize(v.size(),
				  v.size()>0 ? v[0].size() : 0);

	v.resize(newSize.height());
	for (int i=0; i < std::min(oldSize.height(), newSize.height()); ++i)
	{
		v[i].resize(newSize.width());
		for (int j=oldSize.width(); j<newSize.width(); ++j)
			v[i][j] = defaultValue;
	}

	for (int i=oldSize.height(); i<newSize.height(); ++i)
		v[i] = QVector <T> (newSize.width(), defaultValue);
}

inline Color invertColor(Color c)
{
	return c == cBlack ? cWhite :
				( c == cWhite ? cBlack : cVoid );
}

template <class T>
		void removeFromVector(QVector <T> &v, const T &val)
{
	typename QVector<T>::iterator it;
	it = qFind(v.begin(), v.end(), val);
	if (it != v.end())
	{
		std::swap(*it, v.last());
		v.pop_back();
	}
}

template <typename T>
		bool insert(QVector<T>& v, const T& val)

{
	if (v.contains(val))
		return false;
	else
	{
		v.push_back(val);
		return true;
	}
}

QVector <Point> getUpDownLeftRight();
extern const QHash <Markup, QString> markupNames;
extern const QHash <QString, Markup> namesMarkup;

#endif // COMMON_H
