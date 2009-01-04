#pragma once

#include <windows.h>												// HDC
#include "Vector2.h"		// CVector2
#include "ColorARGB.h"	// CColorARGB
#include "tga.h"					// PIX_LoadTGA32
#include "RBBitmap.h"												// CRBBitmap<>

class CBitmapARGB : public CRBBitmap<CColorARGB>
{
public:
	CBitmapARGB( const uint32 dwWidth=0, const uint32 dwHeight=0 );

	void Blit( HDC hdc, const uint32 dwX, const uint32 dwY ) const;


	//!< clipped
	void SetPixel( const CVector2I vPos, const CColorARGB value );

	CColorARGB GetPixel( const CVector2I vPos ) const;

	bool LoadTGA( const char *szFilename );
};
