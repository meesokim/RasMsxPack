//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2016  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "kernel.h"
#include <circle/gpiopin.h>
#include <circle/timer.h>
#include "rpi-gpio.h"
#include "RasMsxPack.h"

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

unsigned short GetAddress()
{
	GPIO_PUT(LE_A, 0xff | LE_B); 
	asm volatile ("nop;nop;"); 
#ifdef V1
	return GPIO_GET0() & 0xffff;
#elif defined V2
	return GPIO_GETA();
#endif	
}

unsigned char GetData()
{
	GPIO_PUT(LE_B, LE_A);
	asm volatile ("nop;nop;"); 
	return GPIO_GET0() & 0xff;
}

static unsigned char ROM[] = {
//#include "Antarctic.data"
//#include "Gradius.data"
//#include "Zemix30.data"
#include "game126.data"
};	

boolean CKernel::Initialize (void)
{
    gpio[GPIO_GPFSEL0] = 0x49249249;
	gpio[GPIO_GPFSEL1] = 0x49249249;
	gpio[GPIO_GPFSEL2] = 0x49249249;
	GPIO_PUT(LE_B | INT | BUSDIR | WAIT, LE_A | 0xffff);	
	return TRUE;
}

TShutdownMode CKernel::Run (void)
{
	unsigned int addr0;
	unsigned char byte;
	int page[8] = {0,0,0,1,2,3,4,5};
	int page2[4] = {0,0,1,2};
	int mapper = 0;
	int signal = 0;
	int pg = 0;
	if (sizeof(ROM) == 2 * 1024 * 1024)
		mapper = 2;
	else if (sizeof(ROM) > 32768)
		mapper = 1;
	if (!mapper)
	{
		while(1)
		{
			signal = ~GPIO_GET0();
			if (signal & SLTSL)
			{
				if (signal & RD)
				{
					GPIO_PUT(LE_A, 0xff | LE_B); 
					addr0 = GPIO_GET0() & 0xffff;
					pg = (addr0 & 0xe000)>>13;
					byte = ROM[page[pg] * 0x2000 + (addr0 & 0x1fff)];
					GPIO_PUT(LE_B | byte | RW, LE_A);
					while(!(gpio[GPIO_GPLEV0] & SLTSL));
					GPIO_PUT(0, RW);
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
			if (signal & SLTSL)
			{
				if (signal & RD)
				{
					addr0 = GetAddress();
					pg = (addr0 & 0xe000)>>13;
					byte = ROM[page[pg] * 0x2000 + (addr0 & 0x1fff)];
					GPIO_PUT(LE_B | byte | RW, LE_A);
					while(!(gpio[GPIO_GPLEV0] & SLTSL));
					GPIO_PUT(0, RW);
				}
				else// if (signal & WR)
				{
					addr0 = GetAddress();
					pg = (addr0 & 0xe000)>>13;
					byte = GetData();
					if ((((addr0 & 0x1fff) == 0) && (pg > 2)) || (!(addr0 & 0xfff)))
						page[pg] = byte;
					while(!(gpio[GPIO_GPLEV0] & SLTSL));
				}
			
			}			
		}
	} else if (mapper == 2)
	{
		while(1)
		{
			signal = ~GPIO_GET0();
			if (signal & SLTSL)
			{
				if (signal & RD)
				{
					addr0 = GetAddress();
					pg = (addr0 & 0xc000)>>14;
					byte = ROM[page2[pg] * 0x4000 + (addr0 & 0x3fff)];
					GPIO_PUT(LE_B | byte | RW, LE_A);
					while(!(gpio[GPIO_GPLEV0] & SLTSL));
					GPIO_PUT(0, RW);
				}
				else 
				{
					addr0 = GetAddress();
					byte = GetData();
					if (addr0 == 0x6000)
						page2[1] = byte;
					else if (addr0 == 0x7000)
						page2[2] = byte;
					while(!(gpio[GPIO_GPLEV0] & SLTSL)); 
				}
			}
		}		
	}	
	return ShutdownReboot;
}


CKernel::CKernel (void)
{
}

CKernel::~CKernel (void)
{
}
