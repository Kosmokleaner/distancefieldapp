#ifndef MAT44_H
#define MAT44_H

#include <math.h>
#include <d3dx9math.h>					// D3DXMatrixReflect
#include <stdlib.h>							// rand()
#include <assert.h>							// assert()
#include <stdio.h>
#include <string.h>
#include "types.h"

#include "Vector3.h"						// CVector3
#include "Plane3.h"							// CPlane3


// todo: Matrix4x4 inversion
//
// Version																								Cycles
// ==============================================================
// C code with Gaussian elimination												1074
// C code with Cramer's rule															846
// C code with Cramer's rule & Streaming SIMD Extensions	210
// 
// http://www.intel.com/design/pentiumiii/sml

struct D3DXMATRIX;

#pragma comment( lib, "d3d9" )
#pragma comment( lib, "d3dx9" )



class CMatrix44
{
public:

	union 
	{
    struct
		{
        float   _11, _12, _13, _14;
        float   _21, _22, _23, _24;
        float   _31, _32, _33, _34;
        float   _41, _42, _43, _44;
    };
		// -----------------------------
    float m[4][4];
		// -----------------------------
	  float p[16];
  };

	// -------------------------------

	//! default constructor
	__forceinline CMatrix44();
	//! copy constructor
	__forceinline CMatrix44( const CMatrix44 &x );

	//! constructor for DirectX matrix class
	CMatrix44( const D3DXMATRIX &inValue );


  //! access grants
	__forceinline float & operator()( const DWORD Row, const DWORD Col );
	__forceinline float operator()( const DWORD Row, const DWORD Col ) const;

    // casting operators
//__forceinline operator float* ();
//__forceinline operator const float* () const;

	__forceinline void SetIdentity();
	__forceinline void ScaleBase33( const float fFactor );

    // assignment operators
	__forceinline CMatrix44 &	operator*=( const CMatrix44 &inrRightSide );
	__forceinline CMatrix44 &	operator+=( const CMatrix44 &inrRightSide );
	__forceinline CMatrix44 &	operator-=( const CMatrix44 &inrRightSide );
	__forceinline CMatrix44 &	operator*=( float infFactor );
	__forceinline CMatrix44 &	operator/=( float infFactor );

    // binary operators
	__forceinline CMatrix44	operator*( const CMatrix44 &inrRightSide ) const;
	__forceinline CMatrix44	operator+( const CMatrix44 &inrRightSide ) const;
	__forceinline CMatrix44	operator-( const CMatrix44 &inrRightSide ) const;
	__forceinline CMatrix44	operator*( float infFactor ) const;
	__forceinline CMatrix44	operator/( float infFactor ) const;

	friend __forceinline CMatrix44 operator*( float infFactor, const CMatrix44 &inrRightSide );

	// better use TransformPos() or TransformDir()
//	friend __forceinline CVector3 operator*( const CMatrix44 &inLhs, const CVector3 &inrRhs );

	//! comparison
	bool operator==( const CMatrix44 &rhs ) const;
	//! comparison
	bool operator!=( const CMatrix44 &rhs ) const;

	//!
	inline CMatrix44 GetInverse() const;

	//!
	inline CMatrix44 GetTranspose() const;

	inline void SetInvalid() { _11=_21=_31=0; }

	inline bool IsValid() const { return _11!=0 || _21!=0 || _31!=0; }

	inline void Debug( const char *inTxt ) const;

	//!
//	inline CMatrix44 GetReflectionMatrix( const float infX, const float infY, const float infZ, const float infT ) const;

//	friend inline CMatrix44 GetYawPitchRollMatrix( const float infX, const float infY, const float infZ );

//	friend inline CMatrix44 GetReflectionMatrix( const float infX, const float infY, const float infZ, const float infT );

//	friend inline CMatrix44 GetScaleMatrix( const float infX, const float infY, const float infZ );

//	friend inline CMatrix44 GetTransMatrix( const float infX, const float infY, const float infZ );


	//!
	void Transpose();


/*
__forceinline void				SetWxyz				( float wx, float wy, float wz );
__forceinline void				SetVec				( float x, float y, float z );
__forceinline void				SetVec				( const CVector3 w );					
__forceinline void				SetBase				( CVector3 &a,CVector3 &b );		// nur Kreuzprodukt
__forceinline bool				SetNormBase			( const CVector3 &a, const CVector3 &b );	// senkrecht, normieren, Kreuzprodukt, false wenn keine Basis
__forceinline bool				SetOriented			( CVector3 &a );						// vorne ist Vectorrichtung, Drehung darum ist nicht gegeben

inline CVector3 &		GetVec				() const;					// Translation
inline CVector3		GetWxVec			() const;					// X-Achse (horizontal)
inline CVector3		GetWyVec			() const;					// Z-Achse (Tiefe)
inline CVector3		GetWzVec			() const;					// Y-Achse (vertikal)

inline    void				ApplyWxyz			( double wx, double wy, double wz );
inline    void				ApplyVec			( float x, float y, float z );
inline    void				ApplyVec			( const CVector3 &w );
inline    void				ApplyMat			( const CMatrix44 &x );

inline    void				Mul					( const CMatrix44 &a, const CMatrix44 &b );
*/

	//! transform (x,y,z,0)
	//! \param inDir (x,y,z)
	CVector3 TransformDir( const CVector3 &inDir ) const;

	//! out.xyz = Mat * in.xyz1
	inline CVector3 TransformPos( const CVector3 &inPos ) const;

	//!
	inline CPlane3 TransformPlane( const CPlane3 &inPlane ) const;

	//! temp.xyzw = Mat * in.xyz1; out.xyz = temp.xyz / temp.w
	inline CVector3 ProjectPos( const CVector3 &inPos ) const;

	//! assignment operator
	const CMatrix44	operator=( const CMatrix44 &mat );

	
	//!
  const float *GetPtr() const
	{
		return p;
	}

	//!
	float *GetPtr()
	{
		return p;
	}
};



// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************




// constructor
inline CMatrix44::CMatrix44()
{
}


// constructor for DirectX matrix class
inline CMatrix44::CMatrix44( const D3DXMATRIX &inValue )
{
	for(int i=0;i<16;i++)
		p[i]=inValue[i];
}


// copy constructor
inline CMatrix44::CMatrix44( const CMatrix44 &x )
{
	memcpy(this,&x,sizeof(CMatrix44));
}

	//
__forceinline void CMatrix44::SetIdentity()
{
	p[1]=p[2]=p[3]=p[4]=p[6]=p[7]=p[8]=p[9]=p[11]=p[12]=p[13]=p[14]=0;
	p[0]=p[5]=p[10]=p[15]=1;
}


//
__forceinline void CMatrix44::ScaleBase33( const float fFactor )
{
	p[0]*=fFactor; p[1]*=fFactor; p[2]*=fFactor;
	p[4]*=fFactor; p[5]*=fFactor; p[6]*=fFactor;
	p[8]*=fFactor; p[9]*=fFactor; p[10]*=fFactor;
}

__forceinline void CMatrix44::Debug( const char *inTxt ) const
{
	assert(inTxt);

	{
		char str[256];

		sprintf_s(str,sizeof(str),"%s=\n",inTxt);

		OutputDebugStringA(str);
	}
	for(int y=0;y<4;y++)
	{
		char str[256];

		sprintf_s(str,sizeof(str),"  %.2f %.2f %.2f %.2f\n",m[y][0],m[y][1],m[y][2],m[y][3]);

		OutputDebugStringA(str);
	}
	OutputDebugStringA("\n");
}


inline bool CMatrix44::operator==( const CMatrix44 &rhs ) const
{
	return memcmp(this,&rhs,sizeof(CMatrix44))==0;
}


inline bool CMatrix44::operator!=( const CMatrix44 &rhs ) const
{
	return !(*this==rhs);
}


//
inline CMatrix44 GetScaleMatrix( const float infX, const float infY, const float infZ )
{
	CMatrix44 ret;

	ret.p[1]=ret.p[2]=ret.p[3]=ret.p[4]=ret.p[6]=ret.p[7]=ret.p[8]=ret.p[9]=ret.p[11]=ret.p[12]=ret.p[13]=ret.p[14]=0;
	ret.p[0]=infX;
	ret.p[5]=infY;
	ret.p[10]=infZ;
	ret.p[15]=1;

	return ret;
}


//! create translation matrix
//! e.g. out=Mat*in
__forceinline CMatrix44 GetTransMatrix( const float inX, const float inY, const float inZ )
{
	CMatrix44 ret;

	ret.p[0]=1;		ret.p[1]=0;		ret.p[2]=0;		ret.p[3]=inX;
	ret.p[4]=0;		ret.p[5]=1;		ret.p[6]=0;		ret.p[7]=inY;
	ret.p[8]=0;		ret.p[9]=0;		ret.p[10]=1;	ret.p[11]=inZ;
	ret.p[12]=0;	ret.p[13]=0;	ret.p[14]=0;	ret.p[15]=1;

	return ret;
}


//! from DirectX SDK
//! The order of transformations is roll first, then pitch, then yaw.
//! Relative to the object's local coordinate axis, this is equivalent
//! to rotation around the z-axis, followed by rotation around the x-axis,
//! followed by rotation around the y-axis. 
// \param infX usually in the range 0..2*PI, can be negative
// \param infY usually in the range 0..2*PI, can be negative
// \param infY usually in the range 0..2*PI, can be negative
inline CMatrix44 GetYawPitchRollMatrix( const float infX, const float infY, const float infZ )
{
	CMatrix44 ret;

	D3DXMatrixRotationYawPitchRoll((D3DXMATRIX *)&ret,infX,infY,infZ);
	ret.Transpose();

	return ret;
}


// out.xyz = Mat * in.xyz1
inline CVector3 CMatrix44::TransformPos( const CVector3 &inPos ) const
{
	return CVector3(inPos.x*_11 + inPos.y*_12 + inPos.z*_13 + _14,
									inPos.x*_21 + inPos.y*_22 + inPos.z*_23 + _24,
									inPos.x*_31 + inPos.y*_32 + inPos.z*_33 + _34);
}

inline CPlane3 CMatrix44::TransformPlane( const CPlane3 &inPlane ) const
{
	CVector3 vNormal = TransformDir(inPlane.m_vNormal);

	vNormal.Normalize();

	// can be optimized
	CPlane3 ret( vNormal, TransformPos(inPlane.GetPlanePoint()) );
	
	return ret;
}


// temp.xyzw = Mat * in.xyz1; out.xyz = temp.xyz / temp.w
inline CVector3 CMatrix44::ProjectPos( const CVector3 &inPos ) const
{
	CVector3 inter(inPos.x*_11 + inPos.y*_12 + inPos.z*_13 + _14,
								 inPos.x*_21 + inPos.y*_22 + inPos.z*_23 + _24,
								 inPos.x*_31 + inPos.y*_32 + inPos.z*_33 + _34);

	float fInv = 1.0f/(inPos.x*_41 + inPos.y*_42 + inPos.z*_43 + _44);

	return CVector3(inter.x,inter.y,inter.z)*fInv;
}


//
__forceinline void CMatrix44::Transpose()
{
	for(int y=0;y<4;y++)
		for(int x=y+1;x<4;x++)
		{
			float help=m[x][y];
			m[x][y]=m[y][x];
			m[y][x]=help;
		}
}


inline CMatrix44 CMatrix44::GetTranspose() const
{
	CMatrix44 ret = *this;

	ret.Transpose();

	return ret;
}


inline CMatrix44 CMatrix44::GetInverse() const
{
	CMatrix44 ret;
	CMatrix44 in=*this;

	in.Transpose();
	D3DXMatrixInverse((D3DXMATRIX *)(&ret),0,(const D3DXMATRIX *)&in);
	ret.Transpose();

	return ret;
}

inline CMatrix44 GetReflectionMatrix( const float infX, const float infY, const float infZ, const float infT ) 
{
	CMatrix44 ret;

	D3DXPLANE plane(infX,infY,infZ,infT);

	D3DXMatrixReflect((D3DXMATRIX *)&ret,&plane);
	ret.Transpose();

	return ret;
}

//! left handed
//! like D3DXMatrixPerspectiveFovLH
//! \param fViewScaleWidth = fViewScaleHeight / Aspect.
//! \param fViewScaleHeight = cot(fovY/2);
//! \param fZNear
//! \param fZFar
inline CMatrix44 GetPerspectiveMatrixLH( const float fViewScaleWidth, const float fViewScaleHeight, const float fZNear, const float fZFar )
{
	float fDiv = fZFar/(fZFar-fZNear);

	CMatrix44 ret;

	ret._11=fViewScaleWidth;	ret._12=0;								ret._13=0;							ret._14=0;
	ret._21=0;								ret._22=fViewScaleHeight;	ret._23=0;							ret._24=0;
	ret._31=0;								ret._32=0;								ret._33=fDiv;						ret._34=1;
	ret._41=0;								ret._42=0;								ret._43=-fZNear*fDiv;		ret._44=0;

	return ret;
}


//! left handed
//! like D3DXMatrixPerspectiveFovLH
//! \param fFovY
inline CMatrix44 GetMatrixPerspectiveFovLH( const float fFovY, const float Aspect, const float fZNear, const float fZFar )
{
	CMatrix44 ret;

	float fAngle = fFovY*0.5f;

	float fViewScaleHeight = (float)(sin(fAngle)/cos(fAngle));
	float fViewScaleWidth = Aspect/fViewScaleHeight;

	return GetPerspectiveMatrixLH(fViewScaleWidth,fViewScaleHeight,fZNear,fZFar);
}



/*
2*zn/(r-l)   0            0              0
0            2*zn/(t-b)   0              0
(l+r)/(l-r)  (t+b)/(b-t)  zf/(zf-zn)     1
0            0            zn*zf/(zn-zf)  0
*/
//!
//! \param fViewScaleWidth = fViewScaleHeight / Aspect.
//! \param fViewScaleHeight = cot(fovY/2);
inline CMatrix44 GetPerspectiveMatrixLH( const float fViewScaleWidth, const float fViewScaleHeight, const float fZNear, const float fZFar,
	const float l, const float r, const float b, const float t )
{
	CMatrix44 ret;

	float fDiv = fZFar/(fZFar-fZNear);

	ret._11=0.5f*(r-l)/fViewScaleWidth;	ret._12=0;														ret._13=0;							ret._14=0;
	ret._21=0;													ret._22=0.5f*(t-b)/fViewScaleHeight;	ret._23=0;							ret._24=0;
	ret._31=(l+r)/2/(r-l)*fDiv;					ret._32=(t+b)/2/(b-t)*fDiv;						ret._33=fDiv;						ret._34=1;
	ret._41=0;													ret._42=0;														ret._43=-fZNear*fDiv;		ret._44=0;

	return ret;
}


/*
2/(r-l)      0            0           0
0            2/(t-b)      0           0
0            0            1/(zf-zn)   0
(l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  l
*/
//!
//! \param fViewScaleWidth = fViewScaleHeight / Aspect.
//! \param fViewScaleHeight
inline CMatrix44 GetParallelMatrixLH( const float fViewScaleWidth, const float fViewScaleHeight, const float fZNear, const float fZFar,
	const float l, const float r, const float b, const float t )
{
	CMatrix44 ret;

	float fDiv = 1.0f/(fZFar-fZNear);

	ret._11=0.5f*(r-l)/fViewScaleWidth;	ret._12=0;														ret._13=0;						ret._14=0;
	ret._21=0;													ret._22=0.5f*(t-b)/fViewScaleHeight;	ret._23=0;						ret._24=0;
	ret._31=0;													ret._32=0;														ret._33=fDiv;					ret._34=0;
	ret._41=(l+r)/2/(r-l)*fDiv;					ret._42=(t+b)/2/(b-t)*fDiv;						ret._43=-fZNear*fDiv;	ret._44=1;

	return ret;
}


//! like D3DXMatrixPerspectiveFovLH
inline CMatrix44 GetMatrixPerspectiveFovLH( const float fovY, const float Aspect, const float fZNear, const float fZFar,
	const float l, const float r, const float b, const float t )
{
	CMatrix44 ret;

	float fAngle = fovY/2;
	float fViewScaleHeight = (float)(cos(fAngle)/sin(fAngle));
	float fViewScaleWidth = fViewScaleHeight / Aspect;

	return GetPerspectiveMatrixLH(fViewScaleWidth,fViewScaleHeight,fZNear,fZFar,l,r,b,t);
}

// access grants
__forceinline float &CMatrix44::operator() ( const DWORD Row, const DWORD Col )
{
	assert(Row<4);
	assert(Col<4);
	return p[Col+4*Row];
}

// access grants
__forceinline float CMatrix44::operator() ( const DWORD Row, const DWORD Col ) const
{
	assert(Row<4);
	assert(Col<4);
	return p[Col+4*Row];
}


__forceinline CVector3 CMatrix44::TransformDir( const CVector3 &inDir ) const
{
	return CVector3(inDir.x*p[0] + inDir.y*p[1] + inDir.z*p[2],
									inDir.x*p[4] + inDir.y*p[5] + inDir.z*p[6],
									inDir.x*p[8] + inDir.y*p[9] + inDir.z*p[10]);
}




__forceinline CMatrix44 &	CMatrix44::operator *= ( const CMatrix44 &b )
{
	assert(this!=&b);

	CMatrix44 temp=*this;

	*this=temp*b;

	return *this;
}

__forceinline CMatrix44 &	CMatrix44::operator += ( const CMatrix44 &inrRightSide )
{
  for(int i=0;i<16;i++)
		p[i] += inrRightSide.p[i];

	return *this;
}

__forceinline CMatrix44 &	CMatrix44::operator -= ( const CMatrix44 &inrRightSide )
{
  for(int i=0;i<16;i++)
		p[i] -= inrRightSide.p[i];

	return *this;
}

__forceinline CMatrix44 &	CMatrix44::operator *= ( float infFactor )
{
  for(int i=0;i<16;i++) 
		p[i] *= infFactor;

	return *this;
}

__forceinline CMatrix44 &	CMatrix44::operator /= ( float infFactor )
{
	return (*this)*=1/infFactor;
}




__forceinline CMatrix44 CMatrix44::operator *( const CMatrix44 &b ) const
{
	CMatrix44 ret;

  for(int i=0;i<4;i++)
    for(int e=0;e<4;e++)
      ret(i,e) =  (*this)(i,0)*b(0,e) 
								+ (*this)(i,1)*b(1,e)
								+ (*this)(i,2)*b(2,e)
								+ (*this)(i,3)*b(3,e);

	return ret;
}

__forceinline CMatrix44 CMatrix44::operator + ( const CMatrix44 &inrRightSide ) const
{
	CMatrix44 ret;
	
	for(int i=0;i<16;i++) ret.p[i] = p[i] + inrRightSide.p[i];

	return ret;
}


__forceinline CMatrix44 CMatrix44::operator - ( const CMatrix44 &inrRightSide ) const
{
	CMatrix44 ret;
	
	for(int i=0;i<16;i++) ret.p[i] = p[i] - inrRightSide.p[i];

	return ret;
}


__forceinline CMatrix44 CMatrix44::operator * ( float infFactor ) const
{
	CMatrix44 ret;
	
	for(int i=0;i<16;i++) ret.p[i] = p[i] * infFactor;

	return ret;
}


__forceinline CMatrix44 CMatrix44::operator / ( float infFactor ) const
{
	return (*this) * (1/infFactor);
}


// assignment operator
__forceinline const CMatrix44 CMatrix44::operator=( const CMatrix44 &mat )
{
	if(this==&mat)return(*this);

	memcpy(this,&mat,sizeof(CMatrix44));

	return *this;
}

__forceinline CMatrix44 operator*( float infFactor, const CMatrix44 &inrRightSide )
{
	return inrRightSide*infFactor;
}

/*
inline CVector3	operator*( const CMatrix44 &inLhs, const CVector3 &inrRhs )
{
	float dat[4];

	for(int y=0;y<4;y++)
	{
		dat[y]=inLhs.p[y*4]*inrRhs.p[0];
	
		for(int x=1;x<4;x++)
			dat[y]+=inLhs.p[y*4+x]*inrRhs.p[x];
	}

	return CVector3(dat[0],dat[1],dat[2]);
//	return CVector3(dat[0]/dat[3],dat[1]/dat[3],dat[2]/dat[3]);
}
*/







#endif	// MAT44_H



