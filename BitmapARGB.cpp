#include "StdAfx.h"
#include "BitmapARGB.h"

CBitmapARGB::CBitmapARGB( const uint32 dwWidth, const uint32 dwHeight ) :CRBBitmap(dwWidth,dwHeight)
{
}


void CBitmapARGB::SetPixel( const CVector2I vPos, const CColorARGB value )
{
	Set(vPos.x,vPos.y,value);
}

CColorARGB CBitmapARGB::GetPixel( const CVector2I vPos ) const
{
	CColorARGB ret;

	bool bOk = Get(vPos.x,vPos.y,ret);			assert(bOk);

	return ret;
}




void CBitmapARGB::Blit( HDC hdc, const uint32 dwX, const uint32 dwY ) const
{
	uint32 mem[sizeof(BITMAPINFOHEADER)/4+3];	//3 uint32 for the RGB masks
	BITMAPINFO *lpbmi=(BITMAPINFO *)mem; 	

	const CColorARGB *pMem = m_pData;

	lpbmi->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
	lpbmi->bmiHeader.biWidth  = GetWidth();
	lpbmi->bmiHeader.biHeight = -(int)GetHeight();
	lpbmi->bmiHeader.biPlanes = 1;
	lpbmi->bmiHeader.biBitCount=32;
	lpbmi->bmiHeader.biCompression=BI_BITFIELDS;
	lpbmi->bmiHeader.biSizeImage=0;
//		lpbmi->bmiHeader.biSizeImage=lpbmi->bmiHeader.biWidth*lpbmi->bmiHeader.biHeight+4*sizeof(uint32)+sizeof(BITMAPINFOHEADER);
	lpbmi->bmiHeader.biXPelsPerMeter=1;
	lpbmi->bmiHeader.biYPelsPerMeter=1;
	lpbmi->bmiHeader.biClrUsed=0;
	lpbmi->bmiHeader.biClrImportant=0;

	mem[sizeof(BITMAPINFOHEADER)/4+0]=0x00ff0000;
	mem[sizeof(BITMAPINFOHEADER)/4+1]=0x0000ff00;
	mem[sizeof(BITMAPINFOHEADER)/4+2]=0x000000ff;

	int zeilen = SetDIBitsToDevice(hdc,
                   dwX,dwY,
                   lpbmi->bmiHeader.biWidth,
                     -lpbmi->bmiHeader.biHeight,
								   0,0,
								   0,-lpbmi->bmiHeader.biHeight,
								   pMem,
								   lpbmi,
								   DIB_RGB_COLORS);
}



bool CBitmapARGB::LoadTGA( const char *szFilename )
{
	uint32 w,h;
	bool alpha;

	if(!PIX_LoadTGA32(szFilename,0,w,h,alpha))
		return false;

	Alloc(w,h);

	if(!PIX_LoadTGA32(szFilename,(uint32 *)m_pData,w,h,alpha))
		return false;
/*
	// todo: check if this is the right way to fix the problem, optimize
	{
		CColorARGB *pPtr=m_pData;

		for(uint32 y=0;y<m_dwHeight;y++)
		for(uint32 x=0;x<m_dwWidth;x++)
		{
			CColorARGB c = *pPtr; 

			*pPtr++ = CColorARGB(c.GetA(),c.GetB(),c.GetG(),c.GetR());
		}
	}
*/
	return true;
}
