#ifndef MATH_H
#define MATH_H

#include <math.h>																// sqrt()
#include <stdlib.h>															// rand()

#include "types.h"									// uint32

#define PI	3.1415926535897932384626433832795
#define INV_PI	(1.0/3.1415926535897932384626433832795)

#define FLT_MAX 3.402823466e+38F


template <class T> T sqr(T a) { return(a*a); }


template <class T> T clamp( const T &x, const T &a, const T &b )
{ 
	if(x<=a)
		return a;

	if(x>=b)
		return b;

	return x; 
}

inline uint16 Float2Half( float x )
{
//	return D3DXFLOAT16(x);

	// float layout from: http://www.fluidstudios.com/pub/GeneralReferences/Pentium/flt2int.txt

	uint32 dwFloat = *((uint32 *)&x);
	uint32 dwMantissa = dwFloat & 0x7fffff;
	int iExp = (int)((dwFloat>>23) & 0xff) - (int)0x7f;
	uint32 dwSign = dwFloat>>31;

	if(iExp<-15)
		iExp=-15;

	if(iExp>16)
		iExp=16;


	// FP16 is s1e5m10
	uint16 ret = (uint16)( (dwSign<<15) | (((uint32)(iExp+0xf))<<10) | (dwMantissa>>13) );

	return ret;
}

inline uint16 Swizzle( const uint16 x )
{
	return (x<<8) | (x>>8);
}

/*
// cubic interpolation
template < class T > class CCubicData
{
public:

	CCubicData()
	{
	}

	CCubicData( T a, T b, T c, T d )
	{
		m_para[0]=a;
		m_para[1]=b;
		m_para[2]=c;
		m_para[3]=d; 
	}

	void SetData( T a, T b, T c, T d )
	{
		m_para[0]=a;
		m_para[1]=b;
		m_para[2]=c;
		m_para[3]=d; 
	}

	float GetNearLength( const float time )
	{
		T v1=CalcPos(0.0f);
		T v2=CalcPos(time*0.5f);
		T v3=CalcPos(time);

		return (v2-v1).Length()+(v3-v2).Length();
	}

	//! \param fak=0..1
	T CalcPos( const float fak ) const
	{
		return ((m_para[0]*fak + m_para[1])*fak + m_para[2])*fak + m_para[3];
	}

//private: // -----------------------------------------------------------

	T							m_para[4];					//!<
};
*/



template < class T > class CBezierData
{
public:

	CBezierData( T a, T b, T c, T d )
		:m_para0(a), m_para1(b), m_para2(c) ,m_para3(d)
	{
	}
/*
	//! \param fA=0..1
	T CalcGrad( const float fA, const float fH=0.01f ) const
	{
		T vDir = CalcPos(fA+fH) - CalcPos(fA-fH);

		float fSqr = vDir.Length();

		if(fSqr>0.001f)
			vDir = vDir * (1.0f/fSqr);

		return vDir;
	}
*/
	//! \param fA=0..1
	T CalcGrad( const float fA ) const
	{
		float fB = 1.0f-fA;

		T a = m_para0*fB + m_para1*fA;
		T b = m_para1*fB + m_para2*fA;
		T c = m_para2*fB + m_para3*fA;

		T d = a*fB + b*fA;
		T e = b*fB + c*fA;

		return e-d;
	}

	//! \param fA=0..1
	T CalcPos( const float fA ) const
	{
		float fB = 1.0f-fA;

		// todo: optimize

		T a = m_para0*fB + m_para1*fA;
		T b = m_para1*fB + m_para2*fA;
		T c = m_para2*fB + m_para3*fA;

		T d = a*fB + b*fA;
		T e = b*fB + c*fA;

		T f = d*fB + e*fA;

		return f;
	}

	float EstimateLength( const uint32 dwSteps ) const
	{
		assert(dwSteps);

		float fStep=1.0f/dwSteps;

		float fRet=0,fT=fStep;
		T old = m_para0;

		for(uint32 dwI=0;dwI<dwSteps;++dwI,fT+=fStep)
		{
			T here = CalcPos(fT);

			fRet+=(old-here).Length();

			old=here;
		}

		return fRet;
	}

	// --------------------------------------------------------------

	T							m_para0;					//!<
	T							m_para1;					//!<
	T							m_para2;					//!<
	T							m_para3;					//!<
};

// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************

// compute amount of bits needed to respresent the number
// e.g. bitsneeded(256)=9
// e.g. bitsneeded(254)=7
inline uint32 bitsneeded( uint32 dwValue )
{
	uint32 dwRet=0;

	while(dwValue>0)
	{
		dwValue>>=1;
		++dwRet;
	}

	return dwRet;
}




inline uint32 irand( uint32 max ){ return(((uint32)rand()*max)>>15); }

inline float frand( float max ){ return(rand()*(1.0f/32768.0f)*max); }
inline float frand2( float max ){ return(( rand()*(1.0f/16384.0f) - 1.0f )*max); }
inline void frandXY2( const float max, float &x, float &y)
{
	do
	{
		x=frand2(1.0f);
		y=frand2(1.0f);

	} while(x*x+y*y<=1.0f);

	x*=max;
	y*=max;
}
inline float fsqr( float a ){ return(a*a); }



template <class T> 
T Lerp( const T &a, const T &b, const float fac )
{
//	return a*(1-fac)+b*fac;
	return a+(b-a)*fac;					// slightly faster
}



#endif
