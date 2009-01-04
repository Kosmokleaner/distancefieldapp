#ifndef VEC3_H
#define VEC3_H

#include <assert.h>												// assert()
#include <d3dx9math.h>										// D3DXVECTOR3
#include "MathLib.h"
#include "vector2.h"											// CVector2


template <class T, class T2> class _Matrix34C;
template <class T> class _Vector2dC;


class CVector3
{
public:

	float x,y,z;

	//! default constructor (no initialization for performace reasons)
	CVector3();
	//! constructor
	CVector3( float x, float y, float z );
	//! constructor
	CVector3( const CVector2 p, float _z ) :x(p.x), y(p.y), z(_z)
	{
	}	
	//! copy constructor
	CVector3( const CVector3 &x );


	//! max per component
	void Max( const CVector3 &c );
	//! min per component
	void Min( const CVector3 &c );							

	void GetOtherBaseVec( CVector3 &a, CVector3 &b ) const;

	//! unary negation
	const CVector3 operator-() const;

	CVector3 operator-( const CVector3 &other ) const;
	CVector3 operator-=( const CVector3 &other );	
	CVector3 operator+( const CVector3 &other ) const;
	CVector3 operator+=( const CVector3 &other );
	float operator*( const CVector3 &other ) const;
	CVector3	operator*( const float fak ) const;
	CVector3 operator*=( const float fak );
	CVector3 operator/=( const float fak );
	bool operator==( const CVector3 &outher ) const;	
	bool operator!=( const CVector3 &outher ) const;	

  float Length() const;
  float Length2() const;
  void Normalize();
	
  CVector3 GetNormalized() const;

	//! \param iniIndex [0..2]
	const float &operator[]( const int iniIndex ) const
	{
		assert(iniIndex>=0);
		assert(iniIndex<3);
		return ((const float *)(this))[iniIndex];
	}

	//! \param iniIndex [0..2]
	float &operator[]( const int iniIndex )
	{
		assert(iniIndex>=0);
		assert(iniIndex<3);
		return ((float *)(this))[iniIndex];
	}

	//! \return true if the vector has unit length and is axis aligned
	inline bool IsAxisAlignedAndUnitLength() const;

	//! \return true=vector length is about 1 unit, false othwerwise
	bool IsNormalized() const;

	bool IsNAN() const;

	//! \return true=internal representation is consistant, false oterwise 
	bool Check() const;

	friend CVector3 operator*( const float t, const CVector3 &a );

	//! return position vector in homogenous coordinates
	const D3DXVECTOR4 GetPos() const;

	//! return direction vector in homogenous coordinates
  const D3DXVECTOR4 GetDir() const;

	//!
  D3DXVECTOR3 GetD3D() const
	{
		return *((D3DXVECTOR3 *)this);
	}
};


inline const D3DXVECTOR4 CVector3::GetPos() const
{
	return D3DXVECTOR4(x,y,z,1);
}


inline const D3DXVECTOR4 CVector3::GetDir() const
{
	return D3DXVECTOR4(x,y,z,0);
}


inline CVector3::CVector3()
{
	// no initialization for performace reasons
}

inline CVector3::CVector3( const float _x, const float _y, const float _z )
{
	x=_x;y=_y;z=_z;
}




inline CVector3::CVector3( const CVector3 &v )
{
  x=v.x;y=v.y;z=v.z;
}






inline void CVector3::Max( const CVector3 &c )
{
	x=(x>c.x)?x:c.x;
	y=(y>c.y)?y:c.y;
	z=(z>c.z)?z:c.z;
}


inline void CVector3::Min( const CVector3 &c )
{
  x=(x<c.x)?x:c.x;
  y=(y<c.y)?y:c.y;
  z=(z<c.z)?z:c.z;
}


inline CVector3 CVector3::operator-( const CVector3 &b ) const
{
	return CVector3(x-b.x,y-b.y,z-b.z );
}


inline CVector3 CVector3::operator-=( const CVector3 &b )
{
	x-=b.x;y-=b.y;z-=b.z;

	return *this;
}


inline CVector3 CVector3::operator+( const CVector3 &b ) const
{
	return CVector3(x+b.x,y+b.y,z+b.z );
}


inline CVector3 CVector3::operator+=( const CVector3 &b )
{
	x+=b.x;y+=b.y;z+=b.z;

	return *this;
}


inline CVector3 CVector3::operator*( const float f ) const
{
	return CVector3(x*f,y*f,z*f );
}


inline float CVector3::operator*( const CVector3 &b ) const
{
	return x*b.x+y*b.y+z*b.z;
}

inline bool CVector3::operator!=( const CVector3 &other ) const
{
	return !(*this == other);
}

inline bool CVector3::operator==( const CVector3 &other ) const
{
	if(x!=other.x)
		return false;
	if(y!=other.y)
		return false;
	if(z!=other.z)
		return false;

	return true;
}


inline CVector3 CVector3::operator*=( const float f )
{
	x*=f;y*=f;z*=f;

	return *this;
}



inline CVector3 CVector3::operator/=( const float f )
{
	float fInv=1.0f/f;

	x*=fInv;y*=fInv;z*=fInv;

	return *this;
}



inline CVector3 CrossProd( const CVector3 &a, const CVector3 &b )
{
	CVector3 ret;

  ret.x=a.y*b.z - a.z*b.y;
  ret.y=a.z*b.x - a.x*b.z;
  ret.z=a.x*b.y - a.y*b.x;

	return ret;
}











inline void CVector3::Normalize()
{
  float fInvLen=1.0f/Length();

  x*=fInvLen;y*=fInvLen;z*=fInvLen;
}

inline CVector3 CVector3::GetNormalized() const
{
	CVector3 v=*this;

	v.Normalize();
  return v;
}

inline float CVector3::Length() const
{
  return sqrtf(x*x+y*y+z*z);
}


inline float CVector3::Length2() const
{
  return x*x+y*y+z*z;
}

inline bool CVector3::Check() const
{
	// todo
	return true;
}

inline bool CVector3::IsNormalized() const
{
	float fLen=Length();

	return fLen>0.999f && fLen<1.01f;
}


inline bool CVector3::IsNAN() const
{
	return _isnan(x) || _isnan(y) || _isnan(z);
}


// unary negation
inline const CVector3 CVector3::operator-() const
{
	return CVector3(-x,-y,-z);
}


inline CVector3 operator*( const float t, const CVector3 &a )
{
	return a*t;
}



// input vectors don't have to be normalized
inline void CVector3::GetOtherBaseVec( CVector3 &a, CVector3 &b ) const
{
	if(z<-0.5f || z>0.5f)
	{
		a.x=z;
		a.y=y;
		a.z=-x;
	}
	else
	{
		a.x=y;
		a.y=-x;
		a.z=z;
	}

	b=CrossProd(*this,a);
	b.Normalize();
	a=CrossProd(b,*this);
	a.Normalize();

	assert(fabs(a*b)<0.01f);
	assert(fabs((*this)*a)<0.01f);
	assert(fabs((*this)*b)<0.01f);
}

inline CVector3 normalize( const CVector3 &a )
{
	CVector3	vRet=a;

	vRet.Normalize();

	return vRet;
}


//! \param fBlend any float value, 0=point a, 1=point b
inline CVector3 lerp( const CVector3 &a, const CVector3 &b, const float fBlend )
{
	return a*(1.0f-fBlend) + b*fBlend;
}


inline bool CVector3::IsAxisAlignedAndUnitLength() const
{
	if(x==1.0f || x==-1.0f)
	if(y==0.0f && z==0.0f)
		return true;

	if(y==1.0f || y==-1.0f)
	if(x==0.0f && z==0.0f)
		return true;

	if(z==1.0f || z==-1.0f)
	if(x==0.0f && y==0.0f)
		return true;

	return false;
}


// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************
// ******************************************************************************************************



inline CVector3 VectorRndCube( const float max=1.0f )
{
	return CVector3(frand2(max),frand2(max),frand2(max));
}


inline CVector3 VectorRndSphere( const float max=1.0f )
{
	CVector3 a;

	do
	{
		a.x=frand2(1);
		a.y=frand2(1);
		a.z=frand2(1);

	} while(a.Length2()>1.0f);

	return a*max;
}


// From the GNU Scientific Library, src/randist/gauss.c

/* Of the two methods provided below, I think the Polar method is more
 * efficient, but only when you are actually producing two random
 * deviates.  We don't produce two, because then we'd have to save one
 * in a static variable for the next call, and that would screws up
 * re-entrant or threaded code, so we only produce one.  This makes
 * the Ratio method suddenly more appealing.  There are further tests
 * one can make if the log() is slow.  See Knuth for details */

/* Both methods pass the statistical tests; but the polar method
 * seems to be a touch faster on my home Pentium, EVEN though we
 * are only using half of the available random deviates!
 */

/* Polar (Box-Mueller) method; See Knuth v2, 3rd ed, p122 */

inline CVector3 VectorRndGaussSphere( const float max=1.0f )
{
	CVector3 vRet;

	float lenQ; 

	do
	{
		vRet.x=frand2(1);
		vRet.y=frand2(1);
		vRet.z=frand2(1);

		lenQ=vRet.Length2();

	} while(lenQ>1 && lenQ!=0);

  // Box-Muller transform
	float w = max * sqrtf( (-2.0f * logf( lenQ ) ) / lenQ );

	vRet.x*=w;
	vRet.y*=w;
	vRet.z*=w;

	return vRet;
}

/*

// The polar form of the Box-Muller transformation 
inline CVector3 VectorRndGaussSphere( const float max=1.0f )
{
	CVector3 a;

	double lenQ; 

	do
	{
		a.x=frand2(1);
		a.y=frand2(1);
		a.z=frand2(1);

		lenQ=a.LengthQuad();

	} while(lenQ>1.0f);


	double ln=log()/log();

	// Box-Muller transform
  return sigma * y * sqrt (-2.0 * log (r2) / r2);


	double w = sqrt( (-2.0 * ln( w ) ) / w );

	log
	a.x*=w;
	a.y*=w;
	a.z*=w;

	return(a*max);
}
*/



inline CVector3 VectorRndSphereHull( const float max=1.0f )
{
	CVector3 a;

	do
	{
		do
		{
			a.x=frand2(1);
			a.y=frand2(1);
			a.z=frand2(1);

		} while(a.Length2()>1.0f);
	} while(a.Length2()<0.0001f);

	return a*(max/a.Length());
}

inline void VectorRndBase( CVector3 &outA, CVector3 &outB, CVector3 &outC )
{
	outA=VectorRndSphereHull();

	CVector3 b,c;

	outA.GetOtherBaseVec(b,c);	

	float fAngle=frand((float)PI*2);
	float fSin=sinf(fAngle);
	float fCos=cosf(fAngle);

	assert(fabs(outA*b)<0.01f);
	assert(fabs(outA*c)<0.01f);
	assert(fabs(b*c)<0.01f);

	outB=b*fSin + c*fCos;
	outC=b*fCos - c*fSin;

	assert(fabs(outA*outB)<0.01f);
	assert(fabs(outB*outC)<0.01f);
	assert(fabs(outC*outA)<0.01f);
	assert(fabs(outC*outB)<0.01f);
}




/*
// from the book "Real Time Collision Detection" Author: Christer Ericson
bool IntersectSegmentPlane( CVector3 a, CVector3 b, CPlane3 p, float &t, CVector3 &q )
{
	// Compute the t value for the directed line ab intersecting the plane
	CVector3 ab = b - a;
	t = (p.m_fDist - p.m_vNormal*a) / p.m_vNormal*ab;

	// If t in [0..1] compute and return intersection point
	if(t>=0.0f && t<=1.0f) 
	{
		q = a + t * ab;
		return true;
	}
	return false;    // Else no intersection
}
*/


// from the book "Real Time Collision Detection" Author: Christer Ericson
// Computes closest points C1 and C2 of S1(s)=P1+s*(Q1-P1) and
// S2(t)=P2+t*(Q2-P2), returning s and t. Function result is squared
// distance between between S1(s) and S2(t)
inline float ClosestPtSegmentSegment( CVector3 p1, CVector3 q1, CVector3 p2, CVector3 q2,
                              float &s, float &t, CVector3 &c1, CVector3 &c2, const float EPSILON=0.000001f )
{
	CVector3 d1 = q1 - p1; // Direction CVector3 of segment S1
	CVector3 d2 = q2 - p2; // Direction CVector3 of segment S2
	CVector3 r = p1 - p2;

	float a = d1*d1; // Squared length of segment S1, always nonnegative
	float e = d2*d2; // Squared length of segment S2, always nonnegative
	float f = d2*r;

	// Check if either or both segments degenerate into points
	if (a <= EPSILON && e <= EPSILON) 
	{
		// Both segments degenerate into points
		s = t = 0.0f;
		c1 = p1;
		c2 = p2;
		return (c1-c2)*(c1-c2);
	}

	if (a <= EPSILON) 
	{
		// First segment degenerates into a point
		s = 0.0f;
		t = f / e; // s = 0 => t = (b*s + f) / e = f / e
		t = clamp(t, 0.0f, 1.0f);
	} 
	else 
	{
		float c = d1*r;
		if (e <= EPSILON) 
		{
				// Second segment degenerates into a point
				t = 0.0f;
				s = clamp(-c / a, 0.0f, 1.0f); // t = 0 => s = (b*t - c) / a = -c / a
		} 
		else 
		{
			// The general nondegenerate case starts here
			float b = d1*d2;
			float denom = a*e-b*b; // Always nonnegative

			// If segments not parallel, compute closest point on L1 to L2, and
			// clamp to segment S1. Else pick arbitrary s (here 0)
			if (denom != 0.0f) 
			{
				s = clamp((b*f - c*e) / denom, 0.0f, 1.0f);
			} 
			else s = 0.0f;

			// Compute point on L2 closest to S1(s) using
			// t = Dot((P1+D1*s)-P2,D2) / Dot(D2,D2) = (b*s + f) / e
			t = (b*s + f) / e;

			// If t in [0,1] done. Else clamp t, recompute s for the new value
			// of t using s = Dot((P2+D2*t)-P1,D1) / Dot(D1,D1)= (t*b - c) / a
			// and clamp s to [0, 1]
			if (t < 0.0f) 
			{
				t = 0.0f;
				s = clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f) 
			{
				t = 1.0f;
				s = clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;
	return (c1-c2)*(c1-c2);
}


inline bool PointProjectToLine( const CVector3 vPos1, const CVector3 vDir1, const CVector3 vPos2, CVector3 vDir2, float &t, CVector3 &vRet )
{
	CVector3 vRelPos = vPos1 - vPos2;

	float a = vDir1*vDir1;		// Squared length of segment S1, always nonnegative
	float e = vDir2*vDir2;		// Squared length of segment S2, always nonnegative
	float f = vDir2*vRelPos;
	float c = vDir1*vRelPos;
	float b = vDir1*vDir2;
	float denom = a*e-b*b;		// Always nonnegative

	// If segments not parallel
	if(denom == 0.0f) 
		return false;

	t = (b*f - c*e) / denom;
	vRet = vPos1 + vDir1 * t;

	return true;
}


//! \param invA does not have to be normalized
//! \param invB does not have to be normalized
__inline const float CalcAngleBetween( const CVector3 &invA, const CVector3 &invB )
{
	float LengthQ = invA.Length()*invB.Length();

	if(LengthQ<0.0001f)LengthQ=0.0001f;			// to prevent division by zero

	float f=(invA*invB)/LengthQ;

	if(f>1.0f)f=1.0f;												// acosf need input in the range [-1..1]
		else if(f<-1.0f)f=-1.0f;							//

	float fRet=acosf(f);	
	
	return(fRet);
}



#endif	// VEC3_H

