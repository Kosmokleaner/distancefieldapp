
#pragma once

#include <assert.h>				// assert()
#include <string.h>				// strncpy_s()


#pragma warning(disable:4996)				// 'fopen' was declared deprecated

typedef unsigned char			uint8;
typedef unsigned short		uint16;
typedef unsigned int			uint32;
typedef unsigned __int64	uint64;

typedef char							sint8;
typedef short							sint16;
typedef int								sint32;
typedef __int64						sint64;

template <class T>
void swap( T &a, T &b )
{
	T c=a;	a=b;b=c;
}


template <int TSize>
struct SString
{
	//! constructor
	SString()
	{
		*dat=0;
	}

	//! constructor
	SString( const char *szSrc )
	{
		strncpy_s(dat,sizeof(dat),szSrc,TSize-1);
		dat[TSize-1]=0;
	}

	//! copy constructor
	SString( const SString<TSize> &rhs )
	{
		strcpy_s(dat,sizeof(dat),rhs.dat);
	}

	//! assignment operator
	SString<TSize> &operator=( const SString<TSize> &rhs )
	{
		strcpy_s(dat,sizeof(dat),rhs.dat);

		return *this;
	}

	SString<TSize> operator+( const char *rhs ) const
	{
		SString<TSize> ret = *this;

		strcat_s(ret.dat,sizeof(dat),rhs);

		return ret;
	}

	SString<TSize> operator+=( const char *rhs )
	{
		strcat_s(dat,sizeof(dat),rhs);

		return *this;
	}

	uint32 size() const
	{
		uint32 dwLen = (uint32)strlen(dat);

		assert(dwLen<sizeof(dat));
		return dwLen;
	}

	const char *c_str() const { return dat; }

	bool empty() const { return dat[0]==0; }

	char *c_str() { return dat; }

	operator char *() {	return dat; }

	operator const char *() const {	return dat; }

	SString<TSize> right( const uint32 dwPart ) const
	{
		uint32 dwLen = size();

		if(dwPart<dwLen)
			return &dat[dwLen-dwPart];

		return *this;
	}

	SString<TSize> left( const uint32 dwPart ) const
	{
		uint32 dwLen = size();

		if(dwPart<dwLen)
		{
			SString<TSize> ret;
			const char *pS=dat;
			char *pD=ret.dat;

			for(uint32 dwI=0;dwI<dwPart;++dwI)
				*pD++=*pS++;

			*pD=0;

			return ret;
		}

		return SString<TSize>();
	}

	// ----------------------------------------

	char		dat[TSize];		//!<
};

typedef SString<256> SString256;


namespace Path
{
	//! remove extension from given path
	//! e.g. "c:\test/test.ab.de" -> "c:\test/test"
	inline void RemoveExt( SString256 &v )
	{
		char *s = v.dat;

		while(*s!=0 && *s!='.')
			++s;

		*s=0;
	}

	//! get extension from given path (after last point)
	//! e.g. "c:\test/test.bmp" -> "bmp"
	//! e.g. "c:\test/test.ab.de" -> "de"
	//! e.g. "c:\test/test" -> ""
	inline const char *GetExt( const char *s )
	{
		const char *szRet = 0;

		while(*s)
		{
			if(*s=='.')
				szRet = s+1;

			++s;
		}

		if(!szRet)
			szRet=s;

		return szRet;
	}

	//! e.g. "c:\test/filename.ext" -> "filename.ext"
	inline void RemovePath( SString256 &v )
	{
		char *p=v.dat, *s=0, *d=v.dat;

		while(*p!=0)
		{
			if(*p=='/' || *p=='\\')
				s=p;

			++p;
		}

		if(!s)
			return;

		++s;

		while(*s!=0)
			*d++=*s++;

		*d=0;
	}
	
	//! e.g. "c:\test/filename.ext" -> "c:\test"
	inline void RemoveFilename( SString256 &v )
	{
		char *p=v.dat, *s=0;

		while(*p!=0)
		{
			if(*p=='/' || *p=='\\')
				s=p;

			++p;
		}

		if(!s)
			return;

		*s=0;
	}
};


// Key state masks for mouse and keyboard messages
const uint32 MIL_FLAGS_LBUTTON	= 0x01;	// like MK_ in windows
const uint32 MIL_FLAGS_RBUTTON	= 0x02;	// like MK_ in windows
const uint32 MIL_FLAGS_SHIFT		= 0x04;	// like MK_ in windows
const uint32 MIL_FLAGS_CONTROL	= 0x08;	// like MK_ in windows
const uint32 MIL_FLAGS_MBUTTON	= 0x10;	// like MK_ in windows
const uint32 MIL_FLAGS_ALT			= 0x20;	// extended over windows


