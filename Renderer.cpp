#include "StdAfx.h"
#include "Renderer.h"
#include "assert.h"
#include "BitmapARGB.h"					// CBitmapARGB

#ifdef USE_DIRECTX_BACKBUFFER
	#include <d3d9.h>								// IDirect3D9
	#include <D3dx9math.h>					// 
#endif // USE_DIRECTX_BACKBUFFER




CRenderer g_Renderer(640,480);
//CRenderer g_Renderer(640/4,480/4);
//CRenderer g_Renderer(640/2,480/2);
//CRenderer g_Renderer(128,128);


CRenderer::CRenderer( const uint32 dwWidth, const uint32 dwHeight ) :m_hWnd(0), m_dwWidth(dwWidth), m_dwHeight(dwHeight),
#ifdef USE_DIRECTX_BACKBUFFER
	m_pSystemMem(0), m_pD3D(0), m_pd3dDevice(0)
#else // USE_DIRECTX_BACKBUFFER
	m_hHDC(0), m_hBitmap(0)
#endif // USE_DIRECTX_BACKBUFFER
{
}



CRenderer::~CRenderer()
{
	// todo
}


bool CRenderer::Init( HWND hWnd )
{
	m_hWnd = hWnd;

	m_ZBuffer.Alloc(m_dwWidth,m_dwHeight);
	
#ifdef USE_DIRECTX_BACKBUFFER

	m_Framebuffer.Alloc(m_dwWidth,m_dwHeight);

	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;

	D3DDISPLAYMODE d3ddm;

	if( FAILED( m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
		return false;

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );

	// ~same speed
	d3dpp.Windowed   = TRUE;
//	d3dpp.Windowed   = FALSE;

	// ~same speed
//	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;	//d3ddm.Format;
//	d3dpp.BackBufferCount=2;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
//	d3dpp.Flags=D3DMULTISAMPLE_NONE;//D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.Flags=D3DMULTISAMPLE_NONE | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
/*
	if(!d3dpp.Windowed)
	{
		// ~same speed
		d3dpp.BackBufferWidth=d3ddm.Width;d3dpp.BackBufferHeight=d3ddm.Height;
	}
*/
	d3dpp.BackBufferWidth=m_dwWidth;d3dpp.BackBufferHeight=m_dwHeight;

	// try hardware vertexshader, if this failes, try software vertex shader
	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_HARDWARE_VERTEXPROCESSING,&d3dpp,&m_pd3dDevice)))
	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&m_pd3dDevice)))
		return false;

	HRESULT hRes;

//	hRes=m_pd3dDevice->CreateOffscreenPlainSurface(d3dpp.BackBufferWidth,d3dpp.BackBufferHeight,D3DFMT_X8R8G8B8,D3DPOOL_SYSTEMMEM,&m_pSystemMem,0);
//	if(FAILED(hRes))
//		assert(0);

	// Clear the back buffer
	hRes=m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0x30,0x30,0xdd), 1.0f, 0 );
	if(FAILED(hRes))
		assert(0);

	Present();

#else // USE_DIRECTX_BACKBUFFER

	// Create a bitmap
	{
		BITMAPINFO bmi;
		int bmihsize = sizeof(BITMAPINFOHEADER);
		memset(&bmi, 0, bmihsize);

		BITMAPINFOHEADER &h = bmi.bmiHeader;

		h.biSize		= bmihsize;
		h.biWidth		= m_dwWidth;
		h.biHeight		= -(int)m_dwHeight;
		h.biPlanes		= 1;
		h.biBitCount	= 32;
		h.biCompression	= BI_RGB;

		unsigned long *rawBits=0;

		m_hHDC = CreateCompatibleDC(NULL);
		m_hBitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&rawBits, NULL, 0);

		m_Framebuffer.SetUserMemory(rawBits,m_dwWidth,m_dwHeight);

		if (!m_hHDC || !m_hBitmap) 
			return false;
	}

#endif // USE_DIRECTX_BACKBUFFER

	return true;
}


void CRenderer::DrawFramebuffer()
{
#ifdef USE_DIRECTX_BACKBUFFER

	HRESULT hRes;
/*
	// Clear the back buffer
	hRes = m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0x30,0x30,0xdd), 1.0f, 0 );
	if(FAILED(hRes))
		assert(0);
*/

	IDirect3DSurface9 *pBackbuffer=0;

	hRes = m_pd3dDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackbuffer);
	if(FAILED(hRes))
		assert(0);

//	D3DSURFACE_DESC desc;

//	pBackbuffer->GetDesc(&desc);

	// 1ms for 480 lines (0.58ms for 1 line)
	{
		D3DLOCKED_RECT lock;

		hRes = pBackbuffer->LockRect(&lock,0,D3DLOCK_DISCARD|D3DLOCK_NOSYSLOCK|D3DLOCK_NO_DIRTY_UPDATE);
		if(FAILED(hRes))
			assert(0);

		uint8 *pSrc = (uint8 *)m_Framebuffer.GetPointer();
		uint32 dwSrcPitch = m_Framebuffer.GetPitch();
		uint8 *pDst = (uint8 *)lock.pBits;
		uint32 dwHeight = m_Framebuffer.GetHeight();

		for(uint32 dwY=0;dwY<dwHeight;++dwY)
		{
			memcpy(pDst,pSrc,dwSrcPitch);
			pSrc+=dwSrcPitch;
			pDst+=lock.Pitch;
		}

		hRes = pBackbuffer->UnlockRect();
		if(FAILED(hRes))
			assert(0);
	}

	pBackbuffer->Release();

#else // USE_DIRECTX_BACKBUFFER

	
	HDC dc = (HDC)GetDC(m_hWnd);
	HBITMAP oldbitmap = (HBITMAP)SelectObject((HDC)m_hHDC, m_hBitmap);
	::BitBlt(dc, 0, 0, m_dwWidth, m_dwHeight, (HDC)m_hHDC, 0, 0, SRCCOPY);
	SelectObject((HDC)m_hHDC, oldbitmap);
	DeleteDC(dc);
	::GdiFlush();
	


#endif // USE_DIRECTX_BACKBUFFER

/*
	// 1000ms
	{
		RECT rect;

		rect.left=0;
		rect.top=0;
		rect.right=rARGB.GetWidth();
		rect.bottom=rARGB.GetHeight();

		hRes = D3DXLoadSurfaceFromMemory(pBackbuffer,0,0,rARGB.GetPointer(),D3DFMT_X8R8G8B8,rARGB.GetPitch(),0,&rect,D3DX_FILTER_NONE,0);
		if(FAILED(hRes))
			assert(0);
	}

	// 6ms
	{
		hRes = m_pd3dDevice->UpdateSurface(m_pSystemMem,0,pBackbuffer,0);
		if(FAILED(hRes))
			assert(0);
	}
*/
}


void CRenderer::Present()
{
#ifdef USE_DIRECTX_BACKBUFFER

	// Blit or DoubleBuffer switch
	HRESULT hRes=m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	if(FAILED(hRes))
	{
		assert(0);
	}

#endif // USE_DIRECTX_BACKBUFFER
}



