
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
#include "rpi-gpio.h"
#include "barrier.h"

#define V8

#ifdef V1 

#define RPMV_D0	(1 << 0)
#define MD00_PIN	0
#define RA08	(1 << 8)
#define RA09	(1 << 9)
#define RA10	(1 << 10)
#define RA11	(1 << 11)
#define RA12	(1 << 12)
#define RA13	(1 << 13)
#define RA14	(1 << 14)
#define RA15	(1 << 15)
#define LE_A	(1 << 16)
#define LE_B	(1 << 17)
#define LE_D	(1 << 26)
#define RCLK	(1 << 25)
#define RST0	(1 << 19)
#define RST		(1 << 23)
#define RINT	(1 << 20)
#define RBUSDIR (1 << 22)
#define RWAIT	(1 << 24)
#define RW		(1 << 19)

#define SLTSL	RA11
#define MREQ	RA10
#define IORQ	RA12
#define RD		RA08
#define WR		RA14
#define RESET	RA13
#define BUSDIR  0
#define INT		0
#define WAIT	RST

#elif defined (V2)

#define RA08	(1 << 8)
#define RA09	(1 << 9)
#define RA10	(1 << 10)
#define RA11	(1 << 11)
#define RA12	(1 << 12)
#define RA13	(1 << 13)
#define RA14	(1 << 14)
#define RA15	(1 << 15)
#define RC16	(1 << 16)
#define RC17	(1 << 17)
#define RC18	(1 << 18)
#define RC19	(1 << 19)
#define RC20	(1 << 20)
#define RC21	(1 << 21)
#define RC22	(1 << 22)
#define RC23	(1 << 23)
#define RC24	(1 << 24)
#define RC25	(1 << 25)
#define RC26	(1 << 26)
#define RC27	(1 << 27)

#define SLTSL	RC16
#define IORQ	RC17
#define RESET	RC18
#define RD		RC19
#define MREQ	RC20
#define WAIT	RC21
#define INT		RC22
#define BUSDIR	RC23
#define LE_A	RC27
#define RW		RC25
#define LE_B	RC24
#define DIR_A	RC26

#elif defined (V8)

#define RA08	(1 << 8)
#define RA09	(1 << 9)
#define RA10	(1 << 10)
#define RA11	(1 << 11)
#define RA12	(1 << 12)
#define RA13	(1 << 13)
#define RA14	(1 << 14)
#define RA15	(1 << 15)
#define RC16	(1 << 16)
#define RC17	(1 << 17)
#define RC18	(1 << 18)
#define RC19	(1 << 19)
#define RC20	(1 << 20)
#define RC21	(1 << 21)
#define RC22	(1 << 22)
#define RC23	(1 << 23)
#define RC24	(1 << 24)
#define RC25	(1 << 25)
#define RC26	(1 << 26)
#define RC27	(1 << 27)

#define RESET	RC16
#define DAT_EN	RC16
#define SLTSL	RC17
#define SNDOUT	RC18
#define IORQ	RC19
#define RD 		RC20
#define BUSDIR	RC21
#define ADDR	RC22
#define INT  	RC23
#define WAIT	RC24
#define DAT_DIR	RC25
#define MREQ	RC26
#define WR		RC27

#endif 
#define MMUTABLEBASE 0x00004000


static unsigned char ROM[] = {
//#include "Antarctic.data"
#include "Gradius.data"
//#include "Zemix30.data"
//#include "game126.data"
};

volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;
volatile unsigned int* gpio0 = (unsigned int*)(GPIO_BASE+GPIO_GPLEV0*4);
volatile unsigned int* gpio1 = (unsigned int*)(GPIO_BASE+GPIO_GPSET0*4);
volatile unsigned int* gpio2 = (unsigned int*)(GPIO_BASE+GPIO_GPCLR0*4);

void PUT32 ( unsigned int a, unsigned int b)
{
	*(unsigned int *)a = b;
}
unsigned int GET32 ( unsigned int a)
{
	return *(unsigned int *)a;
}

#if 0
unsigned int GPIO_GET(void)
{
	return gpio[GPIO_GPLEV0];
}

void GPIO_SET(unsigned int b)
{
//	*gpio1 = b;
	gpio[GPIO_GPSET0] = b;
}

void GPIO_CLR(unsigned int b)
{
	//*gpio2 = b;
	gpio[GPIO_GPCLR0] = b;
}
#else
#define GPIO_GET() gpio[GPIO_GPLEV0]
#define GPIO_SET(a) *gpio1 = a
#define GPIO_CLR(a) *gpio2 = a
#endif
void GPIO_PUT(unsigned int a, unsigned int b)
{
	gpio[GPIO_GPSET0] = a;
	gpio[GPIO_GPCLR0] = b;
}

extern "C" {
extern void start_mmu ( unsigned int, unsigned int );
extern void stop_mmu ( void );
extern void invalidate_tlbs ( void );
extern void invalidate_caches ( void );
extern void membarrier();
}
//-------------------------------------------------------------------
unsigned int mmu_section ( unsigned int vadd, unsigned int padd, unsigned int flags )
{
    unsigned int ra;
    unsigned int rb;  
    unsigned int rc;

    ra=vadd>>20;
    rb=MMUTABLEBASE|(ra<<2);
    rc=(padd&0xFFF00000)|0xC00|flags|2;
    PUT32(rb,rc);
    return(0);
}
//-------------------------------------------------------------------
unsigned int mmu_small ( unsigned int vadd, unsigned int padd, unsigned int flags, unsigned int mmubase )
{
    unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    ra=vadd>>20;
    rb=MMUTABLEBASE|(ra<<2);
    rc=(mmubase&0xFFFFFC00)/*|(domain<<5)*/|1;
    PUT32(rb,rc); //first level descriptor
    ra=(vadd>>12)&0xFF;
    rb=(mmubase&0xFFFFFC00)|(ra<<2);
    rc=(padd&0xFFFFF000)|(0xFF0)|flags|2;
    PUT32(rb,rc); //second level descriptor
    return(0);
}


//void notmain( unsigned int r0, unsigned int r1, unsigned int atags )
int main (void)
{
	/** GPIO Register set */
    gpio[GPIO_GPFSEL0] = 0x49249249;
	gpio[GPIO_GPFSEL1] = 0x49249249;
	gpio[GPIO_GPFSEL2] = 0x49249249;
    asm ("cpsid f");
#if 0
    for(unsigned int ra=0;;ra+=0x00100000)
    {
        mmu_section(ra,ra,(1 << 16)|(1 << 15));
        if(ra==0xFFF00000) break;
    }	
    //peripherals	
#if RASPPI==1	
   mmu_section(0x20000000,0x20000000,(1 << 16)); //NOT CACHED!
   mmu_section(0x20200000,0x20200000,(1 << 16)); //NOT CACHED!	
#else	
    mmu_section(0x3F000000,0x3F000000,0x0); //NOT CACHED!
	mmu_section(0x3F200000,0x3F200000,0x0); //NOT CACHED!
#endif	
    start_mmu(MMUTABLEBASE,0x00000001|0x1000|0x0005);//[23]=0 subpages enabled = legacy ARMv4,v5 and v6 
#endif
	GPIO_CLR(0xffffffff);
	GPIO_SET(INT | WAIT | DAT_DIR | DAT_EN | 0xffff);
	register unsigned int g = 0;
	register unsigned short addr = 0;
	register int sltsl = 0;
	register int p = 0;
	register unsigned char  byte = 0;
	register int mapper = 0;
	
	if (sizeof(ROM) > 1 * 1024 * 1024)
		mapper = 2;
	else if (sizeof(ROM) > 32768)
		mapper = 1;
	if (!mapper)
	{
		while(1)
		{
			g = GPIO_GET();
            if (!(g & (SLTSL | RD)))
			{
				if (sltsl == 0)
				{
					GPIO_CLR(DAT_EN | DAT_DIR);
					GPIO_SET(ADDR);
				} 
				else if (sltsl == 1)
				{
					addr = g;
					p = addr - 0x4000;
					byte = ROM[p];
					GPIO_SET(byte);
				}
				sltsl++;
			}
			else if (g & SLTSL)
			{
				GPIO_SET(DAT_EN | DAT_DIR);
				GPIO_CLR(ADDR | 0xffff);
				sltsl = 0;
			}
		}
	} 
	else if (mapper == 2)
	{
		unsigned int page2[2] = {0,0};
		while(1)
		{
			g = GPIO_GET();
            if (!(g & (SLTSL | RD)))
			{
				if (sltsl == 0)
				{
					GPIO_CLR(DAT_EN | DAT_DIR);
				}
				else if (sltsl == 1)
				{
					addr = g;
					p = page2[(addr & 0x8000) > 0];
					byte = ROM[p | (addr & 0x3fff)];
					GPIO_SET(byte | ADDR);
				}
				sltsl++;
			}
			else if (!(g & (SLTSL | WR)))
			{
				if (sltsl == 0)
				{
					addr = g & 0x7000;
					GPIO_CLR(DAT_EN);
					GPIO_SET(ADDR);
				}
				else if (sltsl == 1)
				{
					if (addr == 0x6000)
					{
						byte = GPIO_GET();
						page2[0] = byte << 14;
						dmb();
						flushcache();
					}
					else if (addr == 0x7000)
					{
						byte = GPIO_GET();
						page2[1] = byte << 14;
						dmb();
						flushcache();
					}
					GPIO_SET(DAT_EN);
				}
				sltsl++;
			}
			else if (g & SLTSL)
			{
				GPIO_CLR(ADDR | 0xffff);
				GPIO_SET(DAT_EN | DAT_DIR);
				sltsl = 0;
			}
		}
	}
	else if (mapper == 1)
	{
		register unsigned char pageKonami4[8] = {0,0,0,1,2,3,4,5};
		register unsigned int g;
		register unsigned short addr = 0;
		register unsigned char byte;
		register int sltsl = 0;
		while(1)
		{
			g = GPIO_GET();
            if (!(g & (SLTSL | RD)))
			{
				if (sltsl == 0)
				{
					GPIO_CLR(DAT_EN | DAT_DIR);
				}
				else if (sltsl == 1)
				{
					addr = g;
					p = (pageKonami4[addr >> 13] << 13);
					byte = ROM[p | (addr & 0x1fff)];
					GPIO_SET(byte | ADDR);
				}
				sltsl++;
			}
			else if (!(g & (SLTSL | WR)))
			{
				if (sltsl == 0)
				{
					addr = g & 0xe000;
					GPIO_CLR(DAT_EN);
					GPIO_SET(ADDR);
				}
				else if (sltsl == 1)
				{
					if ((addr >= 0x6000) && (addr < 0xc000))
					{
						byte = GPIO_GET();
						pageKonami4[(addr >> 13)] = byte;
						dmb();
					}
					GPIO_SET(DAT_EN);
				}
				sltsl++;
			}
			else if (g & SLTSL)
			{
				GPIO_SET(DAT_EN | DAT_DIR);
				GPIO_CLR(ADDR | 0xffff);
				sltsl = 0;
			}
		}
	}
	return 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//
// Copyright (c) 2012 David Welch dwelch@dwelch.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-------------------------------------------------------------------------
