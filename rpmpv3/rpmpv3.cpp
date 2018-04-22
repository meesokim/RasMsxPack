
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
#include "rpi-gpio.h"
#include "barrier.h"

#define V3

#ifdef V3

#define RA00    (0)
#define RD00    (0)
#define RW      (1 << 16)
#define IOMEM   (1 << 17)
#define SLTSL   (1 << 18)
#define RESET   (1 << 19)
#define ERROR   (1 << 20)
#define CMD     (3 << 21)
#define GCLK    (1 << 20)
#define CMDP     (22)
#define RATN    (1 << 26)
#define ATN     (1 << 27)

#endif

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

#endif 
#define MMUTABLEBASE 0x00004000

static unsigned char ROM[] = {
//#include "Antarctic.data"
//#include "Gradius.data"
//#include "Zemix30.data"
#include "game126.data"
};	

/** GPIO Register set */
volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;

void PUT32 ( unsigned int a, unsigned int b)
{
	*(unsigned int *)a = b;
}
unsigned int GET32 ( unsigned int a)
{
	return *(unsigned int *)a;
}

void GPIO_SET(unsigned int b)
{
	gpio[GPIO_GPSET0] = b;
}

void GPIO_CLR(unsigned int b)
{
	gpio[GPIO_GPCLR0] = b;
}

void GPIO_PUT(unsigned int a, unsigned int b)
{
	gpio[GPIO_GPSET0] = a;
	gpio[GPIO_GPCLR0] = b;
}

unsigned int GPIO_GET0()
{
	return gpio[GPIO_GPLEV0];
}

unsigned short GPIO_GETA()
{
	unsigned short a = GPIO_GET0();
	return (((a) & 0x00ff) << 8) | (((a) & 0xff00) >> 8);
}

void SET_CMD(int a)
{
    GPIO_CLR(CMD | RATN);
    GPIO_SET((a & 3) << CMD);
    GPIO_SET(RATN);
}

unsigned short GetAddress()
{
    unsigned short addr;
    SET_CMD(0);
    addr = GET_GPIO() & 0xffff;
    return addr;
}

unsigned char GetData()
{
    SET_CMD(1)
	return GPIO_GET0();
}

unsigned char SetData(unsigned char data)
{
    GPIO_CLR(0xff);
    GPIO_SET(data);
    SET_CMD(2);
}

void SetFlag(unsigned char flag)
{
    GPIO_CLR(0xff);
    GPIO_SET(flag);
    SET_CMD(3);
}
extern "C" {
extern void start_mmu ( unsigned int, unsigned int );
extern void stop_mmu ( void );
extern void invalidate_tlbs ( void );
extern void invalidate_caches ( void );
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
	//int loop;
    //unsigned int* counters;
	//unsigned char *addr;
	register unsigned short addr0;
	register unsigned char byte;
	unsigned int ra;
	//int i = 0;
	int page[8] = {0,0,0,1,2,3,4,5};
	int page2[4] = {0,0,1,2};
	int mapper = 0;
	int signal = 0;
	int pg = 0;
    gpio[GPIO_GPFSEL0] = 0x49249249;
	gpio[GPIO_GPFSEL1] = 0x49249249;
	gpio[GPIO_GPFSEL2] = 0x49249249;
	GPIO_PUT(LE_B | INT | BUSDIR | WAIT, LE_A | 0xffff | RW);

#if 1
    for(ra=0;;ra+=0x00100000)
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
    start_mmu(MMUTABLEBASE,0x00000001|0x1000|0x0005); //[23]=0 subpages enabled = legacy ARMv4,v5 and v6 
#endif
	
	if (sizeof(ROM) == 2 * 1024 * 1024)
		mapper = 2;
	else if (sizeof(ROM) > 32768)
		mapper = 1;
	if (!mapper)
	{
		while(1)
		{
			signal = ~GPIO_GET0();
			if (signal & (ATN | SLTSL))
			{
				if (signal & RW)
				{
					addr0 = GetAddress();
					pg = (addr0 & 0xe000)>>13;
                    SetData(ROM[page[pg] + (addr0 & 0x1fff)]);
				}
				else
					continue;
			}
		}
	} else if (mapper == 1)
	{
		while(1)
		{
			signal = ~GPIO_GET0();
			if (signal & (ATN | SLTSL))
			{
				if (signal & RW)
				{
					addr0 = GetAddress();
					pg = (addr0 & 0xe000)>>13;
					SetData(ROM[page[pg] * 0x2000 + (addr0 & 0x1fff)]);
				}
				else// if (signal & WR)
				{
					addr0 = GetAddress();
					byte = GetData();
					pg = (addr0 & 0xe000)>>13;
					if ((((addr0 & 0x1fff) == 0) && (pg > 2)) || (!(addr0 & 0xfff)))
						page[pg] = byte * 0x2000;
				}
			
			}			
		}
	} else if (mapper == 2)
	{
		while(1)
		{
			signal = ~GPIO_GET0();
			if (signal & (ATN | SLTSL))
			{
				if  (signal & RW)
				{
					addr0 = GetAddress();
					SetData(ROM[page2[addr0 >>14] + (addr0 & 0x3fff)]);
				}
				else
				{
					addr0 = GetAddress();
					byte = GetData();
					if (addr0 == 0x6000)
						page2[1] = byte * 0x4000;
					else if (addr0 == 0x7000)
						page2[2] = byte * 0x4000;
				}
			}
		}		
	}
	return 0;
}

void __attribute__((interrupt("FIQ"))) fiq_vector(void)
{
	while( 1 )
	{
		/* Do Nothing! */
	}
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
