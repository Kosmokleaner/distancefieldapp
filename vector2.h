#ifndef VEC2_H
#define VEC2_H

#include <math.h>
#include <stdlib.h>		// rand()
#include <assert.h>		// assert()
#include <stdio.h>
#include <string.h>
#include "types.h"


#define PI	3.1415926535897932384626433832795



// T could be float, double or int
template < class T > class _Vector2dC
{
public:

	T x,y;

	// default constructor
	_Vector2dC();
	_Vector2dC( const T x, const T y );
	_Vector2dC( const _Vector2dC<T> &x );

	void Set( T x, T y );

	void Max( const _Vector2dC<T> &c );
	void Min( const _Vector2dC<T> &c );

	// unary -
	_Vector2dC<T> operator-() const
	{
		return _Vector2dC<T>(-x,-y);
	}

	_Vector2dC<T> operator-( const _Vector2dC<T> &other ) const;
	_Vector2dC<T> operator-=( const _Vector2dC<T> &other );
	_Vector2dC<T> operator+( const _Vector2dC<T> &other ) const;
	_Vector2dC<T> operator+=( const _Vector2dC<T> &other );
	T operator*( const _Vector2dC<T> &other ) const;
	_Vector2dC<T> operator*( const T fak ) const;
	_Vector2dC<T> operator*=( const T fak );
	_Vector2dC<T> operator/=( const T fak );

	// comparison operators ---------------------------------------------------

	bool operator==( const _Vector2dC<T> &other );
	bool operator!=( const _Vector2dC<T> &other );

	T Length() const;
	float LengthQuad() const;
	void Normalize();
	void NormalizeSafe();

	friend _Vector2dC<T>	operator*( const T t, const _Vector2dC<T> &a );
};








// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************

template <class T> 
_Vector2dC<T>::_Vector2dC()
{}


template <class T> 
_Vector2dC<T>::_Vector2dC( const T _x, const T _y )
{
    x=_x;y=_y;
}


template <class T> 
_Vector2dC<T>::_Vector2dC( const _Vector2dC<T> &p )
{
    x=p.x;y=p.y;
}

template <class T> 
void _Vector2dC<T>::Set( T _x, T _y )
{
    x=_x;y=_y;
}



template <class T> 
void _Vector2dC<T>::Max( const _Vector2dC<T> &c )
{
    x=(x>c.x)?x:c.x;
    y=(y>c.y)?y:c.y;
}


template <class T> 
void _Vector2dC<T>::Min( const _Vector2dC<T> &c )
{
    x=(x<c.x)?x:c.x;
    y=(y<c.y)?y:c.y;
}


template <class T> 
_Vector2dC<T> _Vector2dC<T>::operator-( const _Vector2dC<T> &b ) const
{
	return _Vector2dC<T>(x-b.x,y-b.y );
};

template <class T> 
_Vector2dC<T> _Vector2dC<T>::operator-=( const _Vector2dC<T> &b )
{
	x-=b.x;y-=b.y;

	return *this;
}

template <class T> 
_Vector2dC<T> _Vector2dC<T>::operator+( const _Vector2dC<T> &b ) const
{
	return _Vector2dC<T>(x+b.x,y+b.y );
}

template <class T> 
_Vector2dC<T> _Vector2dC<T>::operator+=( const _Vector2dC<T> &b )
{
	x+=b.x;y+=b.y;

	return *this;
}


template <class T> 
_Vector2dC<T> _Vector2dC<T>::operator*( const T f ) const
{
	return _Vector2dC<T>(x*f,y*f );
}

template <class T> 
T _Vector2dC<T>::operator*( const _Vector2dC<T> &b ) const
{
	return(x*b.x+y*b.y);
}


template <class T> 
_Vector2dC<T> _Vector2dC<T>::operator*=( const T f )
{
	x*=f;y*=f;

	return *this;
}

template <class T> 
_Vector2dC<T> _Vector2dC<T>::operator/=( const T f )
{
	// not optimized for float or double, for int it's better not to use the inverse
	x/=f;y/=f;

	return *this;
}
















template <class T> 
void _Vector2dC<T>::Normalize()
{
  T len=(T)(1.0)/Length();

  x*=len;y*=len;
}

template <class T> 
void _Vector2dC<T>::NormalizeSafe()
{
	T len = Length();

	if(len>0.0001f)
	{
		T invlen=(T)(1.0)/len;

		x*=invlen;y*=invlen;
	}
}


template <class T> 
T _Vector2dC<T>::Length() const
{
    return (T)sqrt(x*x+y*y) ;
}


template <class T> 
float _Vector2dC<T>::LengthQuad() const
{
    return x*x+y*y;
}



template <class T> 
_Vector2dC<T> operator*( const T t, const _Vector2dC<T> &a )
{
	return a*t;
}

template <class T> 
bool _Vector2dC<T>::operator==( const _Vector2dC<T> &other )
{
	return x==other.x && y==other.y;
}

template <class T> 
bool _Vector2dC<T>::operator!=( const _Vector2dC<T> &other )
{
	return !(*this==other);
}


// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************


typedef _Vector2dC<float> CVector2;
typedef _Vector2dC<int> CVector2I;






#endif	// VEC2
