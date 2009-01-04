#pragma once

#include "types.h"				// uint32
#include <assert.h>										// assert()
#include <math.h>											// log()

class CColor3
{
public:

	float r,g,b;

	//! default constructor
	CColor3() {}

	//! constructor
	CColor3( const float fR, const float fG, const float fB ) 
		:r(fR), g(fG), b(fB) {}

	//! constructor
	CColor3( const CColor3 &Ref ){ r=Ref.r;g=Ref.g;b=Ref.b; }

	//!
	CColor3 operator*( const CColor3 &rhs ) const
	{
		return CColor3(r*rhs.r,g*rhs.g,b*rhs.b);
	}

	//! comparison
	bool operator==( const CColor3 &rhs ) const
	{
		return r==rhs.r && g==rhs.g && b==rhs.b;
	}

	//! comparison
	bool operator!=( const CColor3 &rhs ) const
	{
		return !(*this==rhs);
	}

	//!
	CColor3 operator+( const CColor3 &rhs ) const
	{
		return CColor3(r+rhs.r,g+rhs.g,b+rhs.b);
	}

	//!
	CColor3 operator-( const CColor3 &rhs ) const
	{
		return CColor3(r-rhs.r,g-rhs.g,b-rhs.b);
	}

	//!
	void operator=( const int Rhs )
	{
		assert(Rhs==0);

		r=g=b=(float)Rhs;
	}

	//!
	CColor3 operator*( const float &fMul ) const
	{
		return CColor3(r*fMul,g*fMul,b*fMul);
	}

	//!
	CColor3 operator+=( const CColor3 &rhs )
	{
		r+=rhs.r;
		g+=rhs.g;
		b+=rhs.b;
		
		return *this;
	}

	
	//!
	CColor3 operator*=( const float &fMul )
	{
		r*=fMul;
		g*=fMul;
		b*=fMul;
		
		return *this;
	}

	//!
	void Clamp()
	{
		if(r<0.0f)r=0.0f;
		if(r>1.0f)r=1.0f;
		
		if(g<0.0f)g=0.0f;
		if(g>1.0f)g=1.0f;
		
		if(b<0.0f)b=0.0f;
		if(b>1.0f)b=1.0f;
	}

	//!
	uint32 GetRGB() const
	{
		unsigned char R=(uint8)(r*255.0f);
		unsigned char G=(uint8)(g*255.0f);
		unsigned char B=(uint8)(b*255.0f);

		return (((uint32)R)<<16) | (((uint32)G)<<8) | ((uint32)B);
	}

	float GetMaxComponent() const
	{
		if(r>g)
		{
			if(r>b)
				return r;
		}
		else
		{
			if(g>b)
				return g;
		}

		return b;
	}

	//!
	uint32 GetARGB_HDRLog() const
	{
		float fLum=GetMaxComponent();

		CColor3 vColNormalized= (*this) * (1.0f/fLum);

		int iLum = (int)(log(fLum)*16.0f);

		if(iLum<0)
			iLum=0;
		if(iLum>255)
			iLum=255;

		unsigned char cLum = (unsigned char)iLum;

		return (((uint32)cLum)<<24) | vColNormalized.GetBGR();
	}

	//!
	uint32 GetARGB_HDRk() const
	{
		float fLum=GetMaxComponent();

		CColor3 vColNormalized= (*this) * (1.0f/fLum);

		int iLum = (int)(fLum*16.0f);

		if(iLum<0)
			iLum=0;
		if(iLum>255)
			iLum=255;

		unsigned char cLum = (unsigned char)iLum;

		return (((uint32)cLum)<<24) | vColNormalized.GetRGB();
	}


	//!
	void SetRGB( const uint32 dwARGB )
	{
		const unsigned char *uC=(const uint8 *)&dwARGB;

		r=uC[2]/255.0f;
		g=uC[1]/255.0f;
		b=uC[0]/255.0f;
	}

	//! need clamped colors 0..1 (you can call Clamp() before)
	uint32 GetBGR() const
	{
		assert(r>=0.0f);
		assert(r<=1.0f);
		assert(g>=0.0f);
		assert(g<=1.0f);
		assert(b>=0.0f);
		assert(b<=1.0f);

		uint8 R=(uint8)(r*255.0f);
		uint8 G=(uint8)(g*255.0f);
		uint8 B=(uint8)(b*255.0f);

		return (((uint32)B)<<16) | (((uint32)G)<<8) | ((uint32)R);
	}
};
