#pragma once

class CColorARGB
{
public:

	//! fast default constructor
	CColorARGB(){}

	//! constructor
	CColorARGB( const uint32 dwARGB ) :m_dwARGB(dwARGB) {}

	//! constructor
	CColorARGB( const uint8 A, const uint8 R, const uint8 G, const uint8 B ) :m_dwARGB(((uint32)A<<24)+((uint32)R<<16)+((uint32)G<<8)+(uint32)B) {}

  uint8 GetA() const { return (uint8)(m_dwARGB>>24); }
  uint8 GetR() const { return (uint8)(m_dwARGB>>16); }
  uint8 GetG() const { return (uint8)(m_dwARGB>>8); }
  uint8 GetB() const { return (uint8)m_dwARGB; }

  void SetA( const uint8 v ) { m_dwARGB=(m_dwARGB&0x00ffffff) | ((uint32)v<<24); }
  void SetR( const uint8 v ) { m_dwARGB=(m_dwARGB&0xff00ffff) | ((uint32)v<<16); }
  void SetG( const uint8 v ) { m_dwARGB=(m_dwARGB&0xffff00ff) | ((uint32)v<<8); }
  void SetB( const uint8 v ) { m_dwARGB=(m_dwARGB&0xffffff00) | ((uint32)v); }

	// can be optimized
	__forceinline CColorARGB operator+( const CColorARGB &rhs ) const
	{
		int iA = (int)GetA()+(int)rhs.GetA();					if(iA>255)iA=255;
		int iR = (int)GetR()+(int)rhs.GetR();					if(iR>255)iR=255;
		int iG = (int)GetG()+(int)rhs.GetG();					if(iG>255)iG=255;
		int iB = (int)GetB()+(int)rhs.GetB();					if(iB>255)iB=255;

		return CColorARGB( (uint8)iA, (uint8)iR, (uint8)iG, (uint8)iB );
	}

	// can be optimized
	__forceinline CColorARGB operator-( const CColorARGB &rhs ) const
	{
		int iA = (int)GetA()-(int)rhs.GetA();					if(iA<0)iA=0;
		int iR = (int)GetR()-(int)rhs.GetR();					if(iR<0)iR=0;
		int iG = (int)GetG()-(int)rhs.GetG();					if(iG<0)iG=0;
		int iB = (int)GetB()-(int)rhs.GetB();					if(iB<0)iB=0;

		return CColorARGB( (uint8)iA, (uint8)iR, (uint8)iG, (uint8)iB );
	}

//private: // -------------------------------------

	uint32					m_dwARGB;			//!<
};
