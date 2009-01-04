#include "stdafx.h"
#include "DistanceFieldApp.h"
#include "BitmapARGB.h"														// CBitmapARGB
#include "ProcessorClock.h"		// CProcessorClock
#include "MathLib.h"					// Lerp<>()
#include "Vector2.h"					// CVector2
#include "Vector3.h"					// CVector3
#include "Color4.h"						// CColor4
#include "Renderer.h"															// CRenderer
#include "Matrix44.h"					// CMatrix44
#include "perlin.h"						// PerlinNoise3D()




TCHAR							szWindowClass[100];

HWND									g_hWnd=0;
CTimer								g_Timer;
bool									g_bQuit=false;
const float						g_fMinStep=1.0f;
const float						g_fGradientStep=2.0f;
CVector3							g_vLightDir0(-1,-10,-1);								 // normalized after calling Init()
CColor3								g_cLightCol0(0.6f,0.6f,0.3f);
CColor3								g_cDiffMaterial(1.0f,0.1f,0.1f);
CColor3								g_cAmbient(0.1f,0.1f,0.4f);





void Rotate( CVector3 &vP, const float fS, const float fC )
{
	float x = vP.x*fC + vP.y*fS;
	float y = vP.x*fS - vP.y*fC;

	vP.x=x;
	vP.y=y;
}


CVector3 Project( const CVector3 vPos )
{
	CVector3 p;

	float depth = 4+vPos.y;

	p.x = vPos.x/depth*800+250;
	p.y = -vPos.z/depth*800+220;
	p.z = 1000+vPos.y*1000;

	return p;
}


CVector2 Lerp( const CVector2 c0, const CVector2 c1, const CVector2 c2, const float Alpha, const float Beta )
{
	float Gamma = 1.0f-Alpha-Beta;

	return c0*Alpha + c1*Beta + c2*Gamma;
}

// DF = signed distance field function

float DF_Sphere( const CVector3 &vPos, const CVector3 &vMid, const float fRad )
{
	return (vPos-vMid).Length()-fRad;
}


float DF_TwoSpheres( const CVector3 &vPos )
{
	float fRet=min(
			DF_Sphere(vPos,CVector3(-00,-100,-20),50),
			DF_Sphere(vPos,CVector3(30,40,0),100)
		);

	return fRet;
}

float DF_Twist( const CVector3 &vPos )
{
	CVector3 vP = vPos;

//	float fS = sinf(vPos.z*0.1f);
//	float fC = cosf(vPos.z*0.1f);

//	Rotate(vP,fS,fC);



	float fRet = DF_TwoSpheres(vP);

//	if(fRet>g_fMinStep*1.0f)
//		fRet=g_fMinStep*1.0f;

	return fRet;
}

float frac( const float x )
{
	return x-floor(x);
}

float DF( const CVector3 &vPos )
{
	CVector3 vP=vPos;
	const float fD=250.0f;

	vP.x=frac(vP.x/fD)*fD;

	float fRet=DF_Twist(vP);

//	fRet=min(fRet,DF_Twist(vP+CVector3(fD,0,0)));
	fRet=min(fRet,DF_Twist(vP-CVector3(fD,0,0)));


	float fPScale = 0.02f;
	fRet += 10.0f*(float)PerlinNoise3D((double)(vPos.x*fPScale),(double)(vPos.y*fPScale),(double)(vPos.z*fPScale),2.0,2.0f,8);


	return fRet;
}

// based on DF()
CVector3 Normal( const CVector3 &vPos )
{
	float fR=g_fGradientStep;
	float fX0=DF(vPos+CVector3(-fR,0,0));
	float fX1=DF(vPos+CVector3(fR,0,0));
	float fY0=DF(vPos+CVector3(0,-fR,0));
	float fY1=DF(vPos+CVector3(0,fR,0));
	float fZ0=DF(vPos+CVector3(0,0,-fR));
	float fZ1=DF(vPos+CVector3(0,0,fR));

	CVector3 vRet(fX1-fX0,fY1-fY0,fZ1-fZ0);

	vRet.Normalize();

	return vRet;
}


//! \param vDirection must be normalized
//! \return distance, FLT_MAX if not hit
float Raycast( const CVector3 &vStart, const CVector3 &vDirection )
{
	float fRet=0;

	for(uint32 dwI=0;dwI<190;++dwI)
	{
		CVector3 vPos = vStart + vDirection*fRet;
		float fDist = DF(vPos);

		if(fDist<g_fMinStep)
			return fRet;

		fRet += fDist;

		if(fRet>500.0f)
			return FLT_MAX;
	}

//	return FLT_MAX;
	return fRet;
}

CVector3 reflect( const CVector3 &vDir, const CVector3 &vNormal )
{
	return vDir - 2.0f*vNormal*(vDir*vNormal);
}


CColor3 Shading( const CVector3 &vPos, const CVector3 &vNormal, const CVector3 &vEyePos )
{
	CColor3 cRet = g_cAmbient*g_cDiffMaterial;
	CVector3 vNormEyeDir = (vEyePos-vPos).GetNormalized();

	float fLambert = vNormal*g_vLightDir0;		if(fLambert<0)fLambert=0;

	// hard shadows
	float fShadowDist = Raycast(vPos+vNormal*5.0f,g_vLightDir0);
	if(fShadowDist!=FLT_MAX)
		fLambert=0;

	cRet += g_cLightCol0*g_cDiffMaterial*fLambert;		// diffuse

	float fSpec = powf(reflect(vEyePos-vPos,vNormal).GetNormalized()*g_vLightDir0,32.0f) * fLambert;

	cRet += g_cLightCol0*fSpec;		// specular

	return cRet;
}




//! \param vDirection must be normalized
CColorARGB Raytrace( const CVector3 &vStart, const CVector3 &vDirection )
{
	float fDist = Raycast(vStart,vDirection);

	CColor3 rgb;

	if(fDist==FLT_MAX)
		rgb=g_cAmbient;
	else
	{
		CVector3 vPos = vStart+vDirection*fDist;

		CVector3 vNormal=Normal(vPos);
		
		rgb = Shading(vPos,vNormal,vStart);			// todo: fix eyepos for raytracing
	}

	rgb.Clamp();

	rgb.r = powf(rgb.r,1.0f/2.2f);
	rgb.g = powf(rgb.g,1.0f/2.2f);
	rgb.b = powf(rgb.b,1.0f/2.2f);

	return rgb.GetRGB();
}



void Draw( HDC hdc )
{
	static float fTime=5.3f;

	CBitmapARGB &rFramebuffer = g_Renderer.GetFramebuffer();

 	rFramebuffer.Clear(CColorARGB(0x808080));			// 0.8/0.9ms
//	memset(rFramebuffer.GetPointer(),0x80,rFramebuffer.GetPitch()*rFramebuffer.GetHeight());		// 0.7ms
	g_Renderer.GetZBuffer().Clear(0xffffff);

	uint32 dwWidth=rFramebuffer.GetWidth(), dwHeight=rFramebuffer.GetHeight();

	float fZNear=1.0f;
	float fZFar=1000.0f;

	CMatrix44 Mat;

	Mat.SetIdentity();
	
	Mat *= GetYawPitchRollMatrix(fTime,0,0);
	Mat *= GetPerspectiveMatrixLH((float)dwWidth/(float)dwHeight,1.0f,fZNear,fZFar).GetTranspose();
	Mat *= GetTransMatrix(0,0,-290);

	CMatrix44 invMat = Mat.GetInverse();

	for(uint32 dwY=0;dwY<dwHeight;++dwY)
	{
		float fY=dwY/(float)dwHeight*2.0f-1.0f;
		for(uint32 dwX=0;dwX<dwWidth;++dwX)
		{
			float fX=dwX/(float)dwWidth*2.0f-1.0f;

//			CVector3 vStart(0,0,-200);
//			CVector3 vDirection = CVector3(fX*50.0f*4/3,fY*50.0f,0)-vStart;
			CVector3 vStart = Mat.TransformPos(CVector3(fZNear*fX,fZNear*fY,fZNear));
			CVector3 vEnd = Mat.TransformPos(CVector3(fZFar*fX,fZFar*fY,fZFar));
			CVector3 vDirection = (vEnd-vStart).GetNormalized();

			CColorARGB Col=Raytrace(vStart,vDirection);

			rFramebuffer.Set(dwX,dwY,Col);
		}
	}
//	todo

	g_Renderer.DrawFramebuffer();

	fTime += g_Timer.GetFrameTime()*0.15f;
}









BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_vLightDir0.Normalize();

	{
		char strPath[MAX_PATH];

		GetCurrentDirectory(MAX_PATH,strPath);

		OutputDebugString("CurrentDir: ");
		OutputDebugString(strPath);
		OutputDebugString("\n");
	}






	g_Renderer.GetFramebuffer().Clear(CColorARGB(0x808080));			// 1.7ms

	g_hWnd = CreateWindowEx(WS_EX_TOOLWINDOW,szWindowClass, "DistanceFieldApp", WS_POPUP | WS_SYSMENU | WS_CAPTION,
		0,0, 640+2*GetSystemMetrics(SM_CXFIXEDFRAME), 480+2*GetSystemMetrics(SM_CYFIXEDFRAME)+GetSystemMetrics(SM_CYSMCAPTION), 0, NULL, hInstance, NULL);

//	g_hWnd = CreateWindowEx(WS_EX_TOPMOST,szWindowClass, "", WS_POPUP | WS_EX_TOOLWINDOW,
//		256, 16, 640,480, 0, NULL, hInstance, NULL);


	if(!g_hWnd)
		return FALSE;

	g_Renderer.Init(g_hWnd);

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	return TRUE;
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		
		case WM_ERASEBKGND:
			return 0;
			break;
/*
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			Draw(hdc);
			EndPaint(hWnd, &ps);
			InvalidateRect(hWnd,0,false);
		}
		break;
*/
	case WM_DESTROY:
		PostQuitMessage(0);g_bQuit=true;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}






int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDC_REYES, szWindowClass, 100);
	
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= 0;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REYES));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;	//MAKEINTRESOURCE(IDC_REYES);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REYES));

	while(!g_bQuit) 
	{
		if(PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE))
		{
 			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}


		HDC hdc = GetDC(g_hWnd);
		Draw(hdc);

		// 1ms - fullscreen or non fullscreen
		{
			g_Renderer.DrawFramebuffer();
			g_Renderer.Present();
		}

		char str[80];
		sprintf_s(str,sizeof(str)," %.3f ms  \n",g_Timer.GetAverageFrameTime()*1000.0f);
		TextOut(hdc,540,0,str,(int)strlen(str));
//		OutputDebugString(str);
		sprintf_s(str,sizeof(str)," %.3f fps  ",1.0f/g_Timer.GetAverageFrameTime());
		TextOut(hdc,540,20,str,(int)strlen(str));

		g_Timer.UpdateTimer();

		DeleteDC(hdc);
	}

//	char str[80];
//	sprintf_s(str,sizeof(str)," %.3f ms  \n",g_Timer.GetAverageFrameTime()*1000.0f);
//	OutputDebugString(str);

	return (int) msg.wParam;
}
