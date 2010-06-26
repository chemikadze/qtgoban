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

template <typename T, template <typename _T> class C1,  template <typename _T> class ProcessFunctor>
void processMatrix(C1 < C1 <T> > &matrix, const SgfVariant& variant, ProcessFunctor <T> f)
{
	if (variant.type() == SgfVariant::Move)
	{
		Point pnt = variant.toMove();
		f(matrix[ pnt.second ][ pnt.second ]);
	}
	else if (variant.type() == SgfVariant::Compose)
	{
		QPair <SgfVariant, SgfVariant> compose = variant.toCompose();
		if (compose.first.type() == SgfVariant::Move && compose.second.type() == SgfVariant::Move)
		{
			Point from = compose.first.toMove();
			Point to = compose.second.toMove();
			for (int col=from.first; col<=to.first; ++col)
				for (int row=from.second; row<=to.second; ++row)
					f(matrix[ row ][ col ]);
		}
	}
}

#endif // PROCESSMATRIX_H
