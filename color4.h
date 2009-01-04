#pragma once

#include "types.h"						// uint32
#include <assert.h>												// assert()
#include "Color3.h"												// Color3
#include "ColorARGB.h"										// CColorARGB

class CColor4
{
public:

//	float a,r,g,b;
	float r,g,b,a;				// xyzw, order like DirectX preferes

	//! default constructor
	CColor4() {}

	//! constructor
	CColor4( const float fR, const float fG, const float fB ) 
		:r(fR), g(fG), b(fB), a(0) {}

	//! constructor
	CColor4( const float fR, const float fG, const float fB, const float fA ) 
		:r(fR), g(fG), b(fB), a(fA) {}

	//! copy constructor
	CColor4( const CColor4 &Ref ){ a=Ref.a;r=Ref.r;g=Ref.g;b=Ref.b; }

	//!
	CColor4 operator*( const CColor4 &rhs ) const
	{
		return CColor4(r*rhs.r,g*rhs.g,b*rhs.b,a*rhs.a);
	}

	//!
	CColor4 operator*( const float &fMul ) const
	{
		return CColor4(r*fMul,g*fMul,b*fMul,a*fMul);
	}

	//!
	CColor4 operator+( const CColor4 &rhs ) const
	{
		return CColor4(r+rhs.r,g+rhs.g,b+rhs.b,a+rhs.a);
	}

	//!
	CColor4 operator-( const CColor4 &rhs ) const
	{
		return CColor4(r-rhs.r,g-rhs.g,b-rhs.b,a-rhs.a);
	}

	//!
	CColor4 operator+=( const CColor4 &rhs )
	{
		a+=rhs.a;
		r+=rhs.r;
		g+=rhs.g;
		b+=rhs.b;
		
		return *this;
	}

	
	//!
	CColor4 operator*=( const float &fMul )
	{
		a*=fMul;
		r*=fMul;
		g*=fMul;
		b*=fMul;
		
		return *this;
	}
	
	//!
	CColor4 operator*=( const CColor4 &rhs )
	{
		a*=rhs.a;
		r*=rhs.r;
		g*=rhs.g;
		b*=rhs.b;

		return *this;
	}

	//!
	void Clamp()
	{
		if(a<0.0f)a=0.0f;
		if(a>1.0f)a=1.0f;

		if(r<0.0f)r=0.0f;
		if(r>1.0f)r=1.0f;
		
		if(g<0.0f)g=0.0f;
		if(g>1.0f)g=1.0f;
		
		if(b<0.0f)b=0.0f;
		if(b>1.0f)b=1.0f;
	}

	//!
	uint32 GetARGB() const
	{
		uint8 R=(uint8)(r*255.0f);
		uint8 G=(uint8)(g*255.0f);
		uint8 B=(uint8)(b*255.0f);
		uint8 A=(uint8)(a*255.0f);

		return	(((uint32)A)<<24) | (((uint32)R)<<16) | (((uint32)G)<<8) | ((uint32)B);
	}

	//! not clamped
	CColorARGB GetRGB( const float fMul=255.0f ) const
	{
		uint8 R=(uint8)(r*fMul);
		uint8 G=(uint8)(g*fMul);
		uint8 B=(uint8)(b*fMul);

		return (((uint32)R)<<16) | (((uint32)G)<<8) | ((uint32)B);
	}

	//! not clamped
	CColorARGB GetBGR( const float fMul=255.0f ) const
	{
		uint8 R=(uint8)(r*fMul);
		uint8 G=(uint8)(g*fMul);
		uint8 B=(uint8)(b*fMul);
		return (((uint32)B)<<16) | (((uint32)G)<<8) | ((uint32)R);

//		uint32 R=(uint32)(r*fMul);		// slower (1.7ms -> 2.3ms)
//		uint32 G=(uint32)(g*fMul);
//		uint32 B=(uint32)(b*fMul);
//		return (B<<16) | (G<<8) | R;
	}

	//!
	void operator=( const int Rhs )
	{
		assert(Rhs==0);

		a=r=g=b=(float)Rhs;
	}

	//!
	void SetARGB( const uint32 dwARGB )
	{
		const uint8 *uC=(const uint8 *)&dwARGB;

		a=uC[3]/255.0f;
		r=uC[2]/255.0f;
		g=uC[1]/255.0f;
		b=uC[0]/255.0f;
	}

	//!
	uint32 GetABGR() const
	{
		uint8 R=(uint8)(r*255.0f);
		uint8 G=(uint8)(g*255.0f);
		uint8 B=(uint8)(b*255.0f);
		uint8 A=(uint8)(a*255.0f);

		return	(((uint32)A)<<24) | (((uint32)B)<<16) | (((uint32)G)<<8) | ((uint32)R);
	}

	CColor3 GetColor3() const
	{
		return CColor3(r,g,b);
	}
};
