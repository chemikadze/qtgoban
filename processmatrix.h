#ifndef PROCESSMATRIX_H
#define PROCESSMATRIX_H

#include <sgfvariant.h>
#include <QQueue>
#include "common.h"

/* functor looks like:

template <typename T>
class ProcessFunctor
{
public:
	void operator() (T& source);
};

*/

template <typename T>
class assignment
{
	T val;
public:
	inline assignment(const T assign)
	{
		val = assign;
	}

	inline void operator() (T& source)
	{
		source = val;
	}
};

template <typename T, template <typename T> class C,  template <typename T> class ProcessFunctor>
void processMatrix(C<C <T> > &matrix, const SgfVariant& variant, ProcessFunctor <T> f)
{
	if (variant.type() == SgfVariant::tPoint)
	{
		Point pnt = variant.toPoint();
		f(matrix[ pnt.row ][ pnt.col ]);
	}
	else if (variant.type() == SgfVariant::tCompose)
	{
		QPair <SgfVariant, SgfVariant> compose = variant.toCompose();
		if (compose.first.type() == SgfVariant::tPoint && compose.second.type() == SgfVariant::tPoint)
		{
			Point from = compose.first.toPoint();
			Point to = compose.second.toPoint();
			for (int col=from.col; col<=to.col; ++col)
				for (int row=from.row; row<=to.row; ++row)
					f(matrix[ row ][ col ]);
		}
	}
}

template <typename T, template <typename T> class C,  template <typename T> class ProcessFunctor>
void processMatrix(C<C<T> > &matrix, ProcessFunctor<T>f)
{
	for (typename C<C<T> >::iterator i = matrix.begin(); i != matrix.end(); ++i)
	{
		for (typename C<T>::iterator j = i->begin(); j != i->end(); ++j)
			f(*j);
	}
}

template <typename T, template <typename T> class C,  template <typename T> class ProcessFunctor>
int dfsMatrix(C<C<T> > &matrix, Point p, ProcessFunctor<T>f)
{
	int res = 0;
	QQueue < Point > q;
	QVector < QVector<qint8> > w_matrix;
	Color color = matrix[p.row][p.col];
	QSize size(matrix[0].size(), matrix.size());
#define vp(p, s) ( (p.col>=0) && (p.row>=0) && (p.col<s.width()) && (p.row<s.height()) )

	resizeMatrix(w_matrix, size, qint8(0));
	w_matrix[p.row][p.col] = 1;
	q.append(p);

	while (!q.isEmpty())
	{
		static QVector <Point> deltas = getUpDownLeftRight();

		foreach (Point point, deltas)
		{
			point = q.first() + point;
			if ( vp(point,size) && !w_matrix[point.row][point.col] )
			{
				if (matrix[point.row][point.col] == color)
					q.enqueue( point );
				w_matrix[point.row][point.col] = 1;
			}
		}
		f(matrix[q.first().row][q.first().col]);
		++res;
		q.dequeue();
	}
	return res;
#undef vp
}

#endif // PROCESSMATRIX_H
