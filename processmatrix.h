#ifndef PROCESSMATRIX_H
#define PROCESSMATRIX_H

#include <sgfvariant.h>

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

#endif // PROCESSMATRIX_H
