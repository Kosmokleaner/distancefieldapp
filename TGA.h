
#ifndef _TGA_LOADER_INCLUDED		// this is used, don't change
#define _TGA_LOADER_INCLUDED		// this is used, don't change

// from TGA.cpp (optimizable)
// stores in 32Bit, 16Bit or 8Bit with palette
// palette and buffer is in DWORD:00rrggbb or UBYTE:bbggrr00 format
bool PIX_LoadTGA( const char* filename, unsigned char *outpix2, unsigned int *pal, unsigned int* width, unsigned int* height, int &BytesPerPixel, bool &AlphaIncluded );

// from TGA.cpp (optimizable)
// greyscale and palettized images are converted to 32bit
bool PIX_LoadTGA32( const char* filename, unsigned int *outpix2, unsigned int &width, unsigned int &height, bool &AlphaIncluded );

// from TGA.cpp (optimizable), compressed alpha is currently not supported
// buffer is in DWORD:00rrggbb or UBYTE:bbggrr00 format
bool PIX_SaveTGA32( const char* filename, unsigned char *inpix2, int w, int h, bool AlphaInclude, bool inbCompress=false );

bool PIX_SaveTGA8( const char* filename, unsigned char *inpix2, int w, int h );

#endif // _TGA_LOADER_INCLUDED