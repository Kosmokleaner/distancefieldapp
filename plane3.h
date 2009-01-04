#ifndef MATHLIB_PLANE3_H
#define MATHLIB_PLANE3_H

#include <assert.h>												// assert()
#include "Math.h"
#include <float.h>												// FLT_MAX

#include "Vector3.h"											// CVector3


// m_vNormal*v + m_fDist=0
class CPlane3
{
public:

	CVector3			m_vNormal;				//!< has to be normalized
	float					m_fDist;					//!< distance to (0,0,0)


	//! default constructor (no initialization for performace reasons)
	CPlane3()
	{
	}

	//! constructor get plane from 3 points
	CPlane3( const CVector3 &vNormal, const float fDist )
		:m_vNormal(vNormal), m_fDist(fDist)
	{
		assert(vNormal.IsNormalized());
	}

	//! constructor get plane from normal and a point on the plane
	CPlane3( const CVector3 &vNormal, const CVector3 &vPointOnPlane )
		:m_vNormal(vNormal)
	{
		m_fDist = -vPointOnPlane*m_vNormal;

		assert(vNormal.IsNormalized());
	}

	//! constructor get plane from 3 points
	CPlane3( const CVector3 &p1, const CVector3 &p2, const CVector3 &p3 )
	{
		CVector3 vA=p2-p1;
		CVector3 vB=p3-p1;

		m_vNormal = normalize( CrossProd(vA,vB) );

		assert(m_vNormal.IsNormalized());

		m_fDist = -p1*m_vNormal;
	}

	float GetPointDistance( const CVector3 &vPoint ) const
	{
		return vPoint*m_vNormal+m_fDist;
	}

	//! get point nearest to (0,0,0)
	CVector3 GetPlanePoint() const
	{
		return -m_vNormal*m_fDist;
	}

	void Negate()
	{
		m_vNormal=-m_vNormal;
		m_fDist=-m_fDist;
	}

	void SetInvalid()
	{
		m_vNormal=CVector3(0,0,0);m_fDist=FLT_MAX;
	}

	bool IsValid() const
	{
		return m_vNormal==CVector3(0,0,0) || m_fDist==FLT_MAX;
	}
};


inline bool IntersectRayPlane( const CVector3 vPos, const CVector3 vDir, const CPlane3 p, float &t, CVector3 &q )
{
	float fDiv = p.m_vNormal*vDir;

	if(fDiv==0.0f)
		return false;

	t = (p.m_fDist + p.m_vNormal*vPos) / fDiv;
	q = vPos - t * vDir;
	return true;
}






#endif // MATHLIB_PLANE3_H