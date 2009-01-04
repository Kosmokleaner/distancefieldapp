#pragma once
#include "BitmapARGB.h"				// CBitmapARGB

// commend this line to get the alternative blit method (http://www.stereopsis.com/blttest/) 4ms
// otherwise directx is used 0.7ms
#define USE_DIRECTX_BACKBUFFER


#ifdef USE_DIRECTX_BACKBUFFER
struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DSurface9;
#endif // USE_DIRECTX_BACKBUFFER




class CRenderer
{
public:
	typedef uint32 ZBufferType;

	CRenderer( const uint32 dwWidth, const uint32 dwHeight );
	~CRenderer();

	//!
	bool Init( HWND hWnd );

	//!
	void Present();

	//!
	CBitmapARGB &GetFramebuffer() { return m_Framebuffer; }
	//!
	CRBBitmap<CRenderer::ZBufferType> &GetZBuffer() { return m_ZBuffer; }

	//!
	void DrawFramebuffer();

private: // ------------------------------------------------------------

	HWND																m_hWnd;								//!<
	uint32															m_dwWidth;						//!<
	uint32															m_dwHeight;						//!<
	
	CBitmapARGB													m_Framebuffer;				//!<
	CRBBitmap<CRenderer::ZBufferType>		m_ZBuffer;						//!<

#ifdef USE_DIRECTX_BACKBUFFER
	IDirect3D9 *												m_pD3D;								//!< direct3d interface
	IDirect3DDevice9 *									m_pd3dDevice;					//!< direct3ddevice interface
	IDirect3DSurface9 *									m_pSystemMem;					//!<
#else USE_DIRECTX_BACKBUFFER
	HDC																	m_hHDC;								//!<
	HBITMAP															m_hBitmap;						//!<
#endif // USE_DIRECTX_BACKBUFFER
 };

extern CRenderer g_Renderer;
