#include "stdafx.h"														// for precompiled headers (has to be in first place)


typedef unsigned int uint32;

#include "tga.h"															// 
#include <fcntl.h>													  // filesize
#include <io.h>															  // filesize

/* PLTGA.C : Targa file handling routines for PICLAB.  The Targa file type
** is actually a collection of at least 50 different ways to store picture
** data which are all loosely related.	The lookup tables tgatbl1[] and
** tgatbl2[] are used to smooth out some of the differences in these formats.
**
** The third by of the header contains a file type code, and the 17th byte
** contains the size in bits of each pixel entry.  These are the two indices
** into tgatbl1[].	The number at that position is a file type code used for
** the remainder of the code.  Tgatbl2[] is indexed by this code and contains
** a byte of flags laid out as follows:
**
**		Bits 7-3	Not used
**		Bit 2		True if format is monochrome
**		Bit 1		True if format is run-length encoded
**		Bit 0		True if format is color-mapped
**
** This module contains handlers for the commands TLOAD, TSAVE.
*/


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>			// memset
#include <assert.h>			// assert

#define BUFSIZE (16384*16)
#define GET16(c,w) (w)=*((unsigned short*)(&(c)))  /* low order byte first   */
#define SET16(c,w) (*((unsigned short*)(&(c))))=(w)  /* low order byte first   */


static unsigned char tgatbl1[40][4] = 
{
	{ 0 },
	{	1,	2,	0,	0	},
	{	0,	3,	4,	5	},
	{	6,	0,	0,	0	},
	{0},{0},{0},{0},{0},
	{	7,	8,	0,	0	},
	{	0,	9, 10, 11	},
	{  12,	0,	0,	0	}
};

static unsigned char tgatbl2[13] = 
{
	0, 0x01, 0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x03, 0x02, 0x02, 0x02, 0x06	
};


// Handler for TLOAD command.  Only argument is filename to load.  All of
// the flags are set by info in the file header.


#define OUTB(c) (*outpix++ = (unsigned char)(c))
#define OUTDW(c) (*((uint32 * &)outpix )++ = (uint32)(c))




int write_block( unsigned char *ptr, int len, FILE *handle )
{
	return (int)fwrite(ptr,1,len,handle);
}



static uint32 IO_GetFileSize( const char *Name )
{
	FILE *fp;
  int size;

	fp = fopen(Name,"rb");

	if(fp==NULL)
    return(0);
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fclose(fp);

	return size;
}

bool PIX_LoadTGA32( const char* filename, unsigned int *outpix2, unsigned int &width, unsigned int &height, bool &AlphaIncluded )
{
	if(!outpix2)
	{
		int BytesPerPixel;

		bool bRet=PIX_LoadTGA(filename,(unsigned char *)outpix2,0,&width,&height,BytesPerPixel,AlphaIncluded);

		return bRet;
	}
	else
	{
		uint32 Palette[256];

		int BytesPerPixel;

		bool bRet=PIX_LoadTGA(filename,(unsigned char *)outpix2,Palette,&width,&height,BytesPerPixel,AlphaIncluded);

		if(!bRet)
			return bRet;

		switch(BytesPerPixel)
		{
			case 1:
				{
					uint32 *pOutpix=(uint32 *)outpix2;

					for(int y=height-1;y>=0;y--)
						for(int x=width-1;x>=0;x--)
						{
							unsigned char idx=((unsigned char *)outpix2)[y*width+x];

							pOutpix[y*width+x]=Palette[idx];
						}
				}
				break;

			case 4:
				return true;

			case 2:
				/*				{
				uint32 *pOutpix=(uint32 *)outpix2;

				for(int y=height-1;y>=0;y--)
				for(int x=width-1;x>=0;x--)
				{
				unsigned short RGB=(uint32)(((unsigned short *)outpix2)[y*width+x]);

				pOutpix[y*width+x]=RGB&0x;
				}
				}
				return true;
				*/

			default: return false;
		}

		return bRet;
	}
}

// palette and buffer is in uint32:00rrggbb or UBYTE:bbggrr00 format
bool PIX_LoadTGA( const char* filename, unsigned char *outpix2,
				  uint32 * pal, unsigned int* width, unsigned int* height, int &BytesPerPixel,
				  bool &AlphaIncluded )
{
	unsigned char *tbuf,*pMemory;
	int i, np, idsize, maptype, ftype, flipped,	psize, maplen, mapentsize, tflags;
	int tgawidth,tgaheight;
	unsigned short line, h, w, x, cv;

	AlphaIncluded=false;

	if(pal)																									// init palette for grayscale TGA
	{
		for( i = 0; i < 256; i++)
			pal[i]=(i<<16)+(i<<8)+(i);
	}

	if(*filename==0)
		return false;

	{
		FILE *handle=fopen(filename,"rb");

		if(handle==0)return false;
	
		uint32 filesize=IO_GetFileSize(filename);

		// alloc memory
		if((pMemory=(unsigned char *)malloc(filesize))==NULL)
		{ 
			fclose(handle);
			return false; 
		}

		// load
		if(fread(pMemory,1,filesize,handle)!=filesize)
		{
			free(pMemory);
			fclose(handle);
			return false;
		}

		fclose(handle);
	}

	tbuf=pMemory;

	GET16(tbuf[12],w);
	GET16(tbuf[14],h);
	flipped = ((tbuf[17] & 0x20) == 0);
	idsize = tbuf[0];
	maptype = tbuf[1];
	GET16(tbuf[5],maplen);
	mapentsize = tbuf[7] >> 3;
	psize = tbuf[16] >> 3;

	/*
		 tbuf[2]

		 0  -  No image data included.
     1  -  Uncompressed, color-mapped images.
     2  -  Uncompressed, RGB images.
     3  -  Uncompressed, black and white images.
     9  -  Runlength encoded color-mapped images.
    10  -  Runlength encoded RGB images.
    11  -  Runlength encoded, black and white images.
    32  -  Compressed color-mapped data, using Huffman, Delta, and runlength encoding.
    33  -  Compressed color-mapped data, using Huffman, Delta, and runlength encoding.  4-pass quadtree-type process.
*/

	if((ftype = tgatbl1[tbuf[2]][psize-1]) == 0)
	{ 
		free(pMemory);
		return false; 
	}

	tflags = tgatbl2[ftype];
	if(tflags & 0x04) np = 1; else np = 3;		// Anzahl der Planes


	tgawidth=(int)w;
	tgaheight=(int)h;

	if(width)*width = tgawidth;
	if(height)*height = tgaheight;

	BytesPerPixel=1;										// unknown
	if(ftype==3)	BytesPerPixel=2;			// 16Bit
	if(ftype==4)	BytesPerPixel=4;			// 24Bit
	if(ftype==10)	BytesPerPixel=4;			// 24Bit compressed
	if(ftype==6)	BytesPerPixel=1;			// 8bit	Grayscale
	if(ftype==5)												// 32Bit
	{
		AlphaIncluded=true;
		BytesPerPixel=4;			
	}

  if(outpix2==NULL)          // caller was only interested in the dimensions
	{
		free(pMemory);
		return true;
	}
	
	tbuf+=18;
	
	if(idsize) 
	{
		tbuf+=idsize;						// skip ID field
//		i = read_block(tbuf,idsize,handle); 
//		if (i != idsize) return(false);
	}

	// read palette
	if(maptype) 
	{
//		i = read_block(tbuf,(maplen * mapentsize),handle);
//		if (i != (maplen * mapentsize)) return(false);
		if(tflags & 0x01) 
		{
			unsigned char *fbuf=tbuf;

			if(pal)																	// used is interested in the palette
			{
				for(i=0;i<256;i++)
				{
					unsigned char r,g,b;

					b=*fbuf++;
					g=*fbuf++;
					r=*fbuf++;
					
					pal[i]=(r<<16)+(g<<8)+(b);
				}
			}    
		}
		tbuf+=maplen*mapentsize;
	}

	for (line = 0; line < h; ++line) 
	{
		unsigned char *outpix=&outpix2[line*(*width)*BytesPerPixel];			// damit man auch in eine größere Bitmap lesen kann (alignment)

		switch (ftype) 
		{
			case 1:

//				if (read_block(tbuf,w,handle) != w) return(false);
				memcpy(outpix,tbuf,w);
				tbuf+=w;
				break;

			case 2:
//				if (read_block(tbuf,2*w,handle) != 2*w) return(false);
				memcpy(outpix,tbuf,2*w);
				tbuf+=2*w;
				break;

			case 3:
//				if (read_block(tbuf,2*w,handle) != 2*w) return(false);
				for (x=0; x<w; ++x) 
				{
					cv = GET16(*tbuf,cv);				// compressed
					tbuf += 2;
					OUTB(cv>>8);OUTB(cv&0xff);	// evtl. anderst herum
				}
				break;

			case 4:
//				if (read_block(tbuf,3*w,handle) != 3*w) return(false);
				for (x=0; x<w; ++x) 
				{
					OUTB(*tbuf++);
					OUTB(*tbuf++);
					OUTB(*tbuf++);
					OUTB(0);
				}
				break;


			case 10:
					// Runlength encoded RGB images. (e.g. ACDSee runlength compressed)
/*				The first field (1 byte) of each packet is called the Repetition Count field. The second field is called
				the Pixel Value field. For Run-length Packets, the Pixel Value field contains a single pixel value.
				For Raw Packets, the field is a variable number of pixel values.
				The highest order bit of the Repetition Count indicates whether the packet is a Raw Packet or a Runlength
				Packet. If bit 7 of the Repetition Count is set to 1, then the packet is a Run-length Packet. If
				bit 7 is set to zero, then the packet is a Raw Packet.
				The lower 7 bits of the Repetition Count specify how many pixel values are represented by the
				packet. In the case of a Run-length packet, this count indicates how many successive pixels have the
				pixel value specified by the Pixel Value field. For Raw Packets, the Repetition Count specifies how
				many pixel values are actually contained in the next field. This 7 bit value is actually encoded as 1
				less than the number of pixels in the packet (a value of 0 implies 1 pixel while a value of 0x7F
				implies 128 pixels).
				Run-length Packets should never encode pixels from more than one scan line. Even if the end of one
				scan line and the beginning of the next contain pixels of the same value, the two should be encoded
				as separate packets. In other words, Run-length Packets should not wrap from one line to another.
				This scheme allows software to create and use a scan line table for rapid, random access of
				individual lines. Scan line tables are discussed in further detail in the Extension Area section of this
				document.
*/
				for(x=0;x<w;)
				{
					unsigned char pack=*tbuf++;
					int length=(pack&0x7f)+1;

					if(pack&0x80)		// compressed packet
					{
						uint32 r=*tbuf++,g=*tbuf++,b=*tbuf++;
						uint32 dat=(b<<16) | (g<<8) | (r);

						for(int i=0;i<length;i++)
							OUTDW(dat);
					}
					else						// raw packet
					{
						for(int i=0;i<length;i++)
						{
							OUTB(*tbuf++);
							OUTB(*tbuf++);
							OUTB(*tbuf++);
							OUTB(0);
						}
					}
					x+=length;
				}
				break;

			case 5:
//				if (read_block(tbuf,4*w,handle) != 4*w) return(false);
				for (x=0; x<w; ++x) 
				{
					OUTB(*tbuf++);
					OUTB(*tbuf++);
					OUTB(*tbuf++);
					OUTB(*tbuf++);
//					++fbuf; 				/* Skip overlay byte */
				}
				break;

			case 6:
//				if(read_block(tbuf,w,handle) != w) return(false);
				for(x=0;x<w;++x)OUTB(*tbuf++);
				break;

			case 7:
			case 8:
			case 9:
			case 11:
			case 12:
			default: 	
				{
					free(pMemory);
					return(false);
				}
		}
	}



	if(flipped)
	{
		assert((((*width)*BytesPerPixel)%4)==0);			// sonst geht die Routine darunter nicht
		uint32 linsize=(*width)*BytesPerPixel/4;	
		uint32 a=0,b=(h-1)*linsize;
		uint32 *mema=(uint32 *)outpix2;
		uint32 *memb=&mema[b];

		for(uint32 i=0;i<(uint32)h/2;i++)
		{	
			for(uint32 f=0;f<linsize;f++)
			{
				uint32 help=*mema;

				*mema++=*memb;*memb++=help;
			}
			memb-=2*linsize;
		}
	}

	free(pMemory);
	return true;
}


bool PIX_SaveTGA8( const char* filename, unsigned char *inpix2, int w, int h )
{
	unsigned char *fbuf;
	int line, x;

	FILE *handle=fopen(filename,"wb");

	if(handle==0)
		return false;

	if((fbuf=(unsigned char *)malloc(BUFSIZE + 6144))==NULL)
	{
		fclose(handle);
		return false;
	}

	fbuf[0]=0;									// idsize
	fbuf[1]=0;									// maptype  (palette or not)
	fbuf[2]=2;									// ftype, not compressed
	fbuf[3]=0;									// ?
	fbuf[4]=0;									// ?
	SET16(fbuf[5],0);						// maplen (palette)
	fbuf[7]=0;									// mapentsize (palette)
	fbuf[8]=0;									// ?
	fbuf[9]=0;									// ?
	fbuf[10]=0;									// ?
	fbuf[11]=0;									// ?
	SET16(fbuf[12],w);					// width
	SET16(fbuf[14],h);					// height
	
	{
		fbuf[16]=0x18;					// ?????
		fbuf[17]=0x20;					// flag 0x20 means not flipped
	}

 	if(write_block(fbuf,18,handle)!=18)
	{
		free(fbuf);
		fclose(handle);
		return false;
	}

	for(line=0;line<h;++line) 
	{
		unsigned char *gbuf=fbuf;
		for (x=0; x<w; ++x) 
		{
			*gbuf++=*inpix2;
			*gbuf++=*inpix2;
			*gbuf++=*inpix2;
			inpix2++;
		}

		if(write_block(fbuf,3*w,handle) != 3*w)
		{
			free(fbuf);
			fclose(handle);
			return false;
		}
	}

	free(fbuf);
	fclose(handle);
	return true;
}


// buffer is in uint32:00rrggbb or UBYTE:bbggrr00 format
bool PIX_SaveTGA32( const char* filename, unsigned char *inpix2, int w, int h, bool AlphaInclude, bool inbCompress )
{
	unsigned char *fbuf;
	int line, x;

	if(AlphaInclude && inbCompress)
	{
		assert(0);
		return false;		// currently not supported
	}

	FILE *handle=fopen(filename,"wb");

	if(handle==0)
		return false;

	if((fbuf=(unsigned char *)malloc(BUFSIZE + 6144))==NULL)
	{
		fclose(handle);
		return false;
	}

	fbuf[0]=0;									// idsize
	fbuf[1]=0;									// maptype  (palette or not)
	fbuf[2]=inbCompress?10:2;		// ftype
	fbuf[3]=0;									// ?
	fbuf[4]=0;									// ?
	SET16(fbuf[5],0);						// maplen (palette)
	fbuf[7]=0;									// mapentsize (palette)
	fbuf[8]=0;									// ?
	fbuf[9]=0;									// ?
	fbuf[10]=0;									// ?
	fbuf[11]=0;									// ?
	SET16(fbuf[12],w);					// width
	SET16(fbuf[14],h);					// height
	
	if(AlphaInclude)
	{
		fbuf[16]=0x20;					// ?????
		fbuf[17]=0x28;					// flag 0x20 means not flipped
	}
	else
	{
		fbuf[16]=0x18;					// ?????
		fbuf[17]=0x20;					// flag 0x20 means not flipped
	}

 	if(write_block(fbuf,18,handle)!=18)
	{
		free(fbuf);
		fclose(handle);
		return false;
	}
	
	for(line=0;line<h;++line) 
	{
		if(AlphaInclude)
		{
			unsigned char *gbuf=fbuf;
			for (x=0; x<w; ++x) 
			{
				*gbuf++=*inpix2++;				// could be optimized
				*gbuf++=*inpix2++;
				*gbuf++=*inpix2++;
				*gbuf++=*inpix2++;
			}

			if(write_block(fbuf,4*w,handle) != 4*w)
			{
				free(fbuf);
				fclose(handle);
				return false;
			}
		}
		else
		{
			if(!inbCompress)
			{
				unsigned char *gbuf=fbuf;
				for (x=0; x<w; ++x) 
				{
					*gbuf++=*inpix2++;
					*gbuf++=*inpix2++;
					*gbuf++=*inpix2++;
					inpix2++;
				}

				if(write_block(fbuf,3*w,handle) != 3*w)
				{
					free(fbuf);
					fclose(handle);
					return false;
				}
			}
			else	// compressed
			{
				uint32 RawCount=0;
				unsigned char *RawPtr=inpix2;

				for(x=0;x<w;)
				{
					if(RawCount==0)RawPtr=inpix2;

					uint32 r=*inpix2++,g=*inpix2++,b=*inpix2++;inpix2++;
					uint32 dat=(b<<16) | (g<<8) | (r);
          x++;

					uint32 CompCount=1;

          while(x<w && CompCount+RawCount<128)
          {
						uint32 r=*inpix2++,g=*inpix2++,b=*inpix2++;inpix2++;
						uint32 here=(b<<16) | (g<<8) | (r);
						x++;	

						if(here!=dat)
						{
							inpix2-=4;x--;break;
						}

            CompCount++;
          }

					if(RawCount>127)											// uncompressed
					{
						putc(RawCount-1,handle);

						for(uint32 i=0;i<RawCount;i++)
						{
							putc(*RawPtr++,handle);
							putc(*RawPtr++,handle);
							putc(*RawPtr++,handle);
							RawPtr++;
						}

						RawCount=0;
					}

					if(CompCount>2)
          {
						if(RawCount)												// uncompressed
						{
							putc(RawCount-1,handle);

							for(uint32 i=0;i<RawCount;i++)
							{
								putc(*RawPtr++,handle);
								putc(*RawPtr++,handle);
								putc(*RawPtr++,handle);
								RawPtr++;
							}

							RawCount=0;
						}

            putc(0x80|(CompCount-1),handle);		// compressed

            putc(r,handle);putc(g,handle);putc(b,handle);
          }
					else
					{
						RawCount+=CompCount;
					}

				}

				if(RawCount)
				{
					putc(RawCount-1,handle);					// uncompressed

					for(uint32 i=0;i<RawCount;i++)
					{
						putc(*RawPtr++,handle);
						putc(*RawPtr++,handle);
						putc(*RawPtr++,handle);
						RawPtr++;
					}
				}
			}
		}
	}

	free(fbuf);
	fclose(handle);
	return true;
}
