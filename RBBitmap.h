#pragma once

// memory block used as bitmap

template <class RasterElement>
class CRBBitmap
{
public:

	//! default constructor
	CRBBitmap( const uint32 dwWidth=0, const uint32 dwHeight=0 ) :m_pData(0), m_dwWidth(0), m_dwHeight(0), m_bFreeData(true)
	{
		Alloc(dwWidth,dwHeight);
	}
	
	//! memory is not freed
	//! \param pMem must not be 0
	void SetUserMemory( void *pMem, const uint32 dwWidth, const uint32 dwHeight )
	{
		assert(pMem);

		FreeData();

		m_pData=(RasterElement *)pMem;
		m_dwWidth=dwWidth;
		m_dwHeight=dwHeight;		
		m_bFreeData=false;
	}

	//! destructor
	~CRBBitmap()
	{
		FreeData();
	}

	//! free all the memory resources
	void FreeData()
	{
		if(m_bFreeData)
			delete [] m_pData;

		m_pData=0;
		m_dwWidth=0;
		m_dwHeight=0;
		m_bFreeData=true;
	}

	//! use with or height of 0 to dealloc
	//! /param inWidth 0..
	//! /param inHeight 0..
	//! /return true=operator was successful, 0 otherwise (low memory)
	bool Alloc( const uint32 indwWidth, const uint32 indwHeight, const RasterElement *inpInitial=0 )
	{
		FreeData();

		uint32 dwSize = indwWidth*indwHeight;

		if(dwSize==0)
			return true;

		m_pData=new RasterElement[dwSize];
		if(!m_pData)return(false);

		m_dwWidth=indwWidth;
		m_dwHeight=indwHeight;

		if(inpInitial)
			Clear(*inpInitial);		// could be done better (copy constructor calls)

		return true;
	}

	//! works only within the Bitmap
	//! /param inX 0..m_dwWidth-1 or the method returns false
	//! /param inY 0..m_dwHeight-1 or the method returns false
	//! /param outValue
	//! /return true=position was in the bitmap, false otherwise
	bool Get( const uint32 inX, const uint32 inY, RasterElement &outValue ) const
	{
		assert(m_pData);
		
		if(inX>=m_dwWidth || inY>=m_dwHeight)
			return false;

		outValue=m_pData[inY*m_dwWidth+inX];
		return true;
	}

	//! works only within the Bitmap
	//! /param inX 0..m_dwWidth-1 or the method returns false
	//! /param inY 0..m_dwHeight-1 or the method returns false
	RasterElement &GetRef( const uint32 inX, const uint32 inY )
	{
		assert(m_pData);
		assert(inX<m_dwWidth && inY<m_dwHeight);

		return(m_pData[inY*m_dwWidth+inX]);
	}

	//! works only within the Bitmap
	//! /param inX 0..m_dwWidth-1 or the method returns false
	//! /param inY 0..m_dwHeight-1 or the method returns false
	//! /param inValue
	bool Set( const uint32 inX, const uint32 inY, const RasterElement &inValue )
	{
		assert(m_pData);
		
		if(inX>=m_dwWidth || inY>=m_dwHeight)
			return false;

		assert(m_pData);

		m_pData[inY*m_dwWidth+inX]=inValue;
		return true;
	}

	uint32 GetWidth() const
	{
		return m_dwWidth;
	}

	uint32 GetPitch() const
	{
		return m_dwWidth * sizeof(RasterElement);
	}

	uint32 GetHeight() const
	{
		return m_dwHeight;
	}

	// /return could be 0 if point is outside the bitmap
	RasterElement *GetPointer( const uint32 inX=0, const uint32 inY=0 )
	{
		if(inX>=m_dwWidth || inY>=m_dwHeight)
			return 0;

		return &m_pData[inY*m_dwWidth+inX];
	}

	void Clear( const RasterElement &inValue )
	{
		RasterElement *pPtr=m_pData;

		for(uint32 y=0;y<m_dwHeight;y++)
			for(uint32 x=0;x<m_dwWidth;x++)
				*pPtr++=inValue;
	}

protected: // ------------------------------------------------

	RasterElement *		m_pData;			//!< [m_dwWidth*m_dwHeight]
	uint32						m_dwWidth;		//!< width of m_pData
	uint32						m_dwHeight;		//!< height of m_pData
	bool							m_bFreeData;	//!< 
};
