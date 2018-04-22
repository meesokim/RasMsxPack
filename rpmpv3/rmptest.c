#define RPMP_V3
 
#define RA00    (0)
#define RD00    (0)
#define RWAIT   (1 << 15)
#define RINT    (1 << 14)
#define RREADY  (1 << 13)
#define RTEST   (1 << 11)
#define RW      (1 << 16)
#define IOMEM   (1 << 17)
#define RESET   (1 << 19)
#define SLTSL   (1 << 18)
#define ERROR   (1 << 20)
#define CMD     (3 << 21)
#define CMD0    0
#define CMD1    (1 << 21)
#define CMD2    (2 << 21)
#define CMD3    (CMD)
#define CMDP    (21)
#define GCLK    (1 << 20)
#define ACK     (1 << 25)
#define RATN    (1 << 26)
#define ATN     (1 << 27)

// Access from ARM Running Linux
 
#define BCM2708_PERI_BASE        0x3F000000 // Raspberry Pi 2
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define GPCLK_BASE               (BCM2708_PERI_BASE + 0x00101000) /* GPCLK controller */

#include <stdio.h>
#include <stdlib.h>  
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <bcm2835.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <pthread.h>
#include <pigpio.h>


void usleep(unsigned long);

int  mem_fd;
void *gpio_map;
 
// I/O access
volatile unsigned *gpio;
volatile unsigned *gpio10;
volatile unsigned *gpio7;
volatile unsigned *gpio13;
volatile unsigned *gpio1;

#define GPIO_GPSET0 7
#define GPIO_GPCLR0 10
#define GPIO_GPLEV0 13

#define CLK_BUSY    (1 << 7)
#define GPIO_CLK0_CTL *(gpclk + 28)
#define GPIO_CLK0_DIV *(gpclk + 29)

void GPIO_SET(unsigned int b)
{
	gpio[GPIO_GPSET0] = b;
}

void GPIO_CLR(unsigned int b)
{
	gpio[GPIO_GPCLR0] = b;
}

unsigned int GPIO_GET()
{
	return gpio[GPIO_GPLEV0];
}

void SET_CMD(int a)
{
    GPIO_CLR(CMD | RATN);
    GPIO_SET((a & 3) << CMDP);
    GPIO_SET(RATN);
}
 
 #define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)

#define MSX_SET_OUTPUT(g) {INP_GPIO(g); OUT_GPIO(g);};
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
 
//#define GPIO_SET(x) *(gpio7)=x  // sets   bits which are 1 ignores bits which are 0
//#define GPIO_CLR(x) *(gpio10)=x // clears bits which are 1 ignores bits which are 0
 
#define GET_GPIO(g) (*(gpio13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
//#define GPIO_GET() (*(gpio13))
 
#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

//
// Set up a memory regions to access GPIO
//
//
// Set up a memory regions to access GPIO
//
volatile unsigned int *setup_io(int reg_base)
{
    int  mem_fd;
    void *io_map;

    // open /dev/mem
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
    {
        printf("can't open /dev/mem \n");
        exit(-1);
    }
    // mmap IO
    io_map = mmap(NULL,             //Any adddress in our space will do
		  PAGE_SIZE,       //Map length
		  PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
		  MAP_SHARED,       //Shared with other processes
		  mem_fd,           //File to map
		  reg_base);        //Offset to peripheral
    close(mem_fd); //No need to keep mem_fd open after mmap
    if (io_map == MAP_FAILED)
    {
        printf("mmap error %d\n", (int)io_map);//errno also set!
        exit(-1);
    }
    return (volatile unsigned *)io_map;
}

int clock_ena(volatile unsigned *gpclk, int speed_id, int divisor, int divf) {
  if (divisor < 2) {
    printf("clock_ena: Minimum divisor value is 2.\n");
    exit(-1);
  }
  if (divisor > 0xfff) {
    printf("clock_ena: Maximum divisor value is %d.\n", 0xfff);
    exit(-1);
  }
  usleep(1000L);
  GPIO_CLK0_CTL = 0x5A000000 | speed_id;    // GPCLK0 off
  while (GPIO_CLK0_CTL & CLK_BUSY) {}    // Wait for BUSY low
  GPIO_CLK0_DIV = 0x5A002000 | (divisor << 12) | (divf); // set DIVI and DIVF (<=freq / (DIVI + DIVF/1024))
  GPIO_CLK0_CTL = 0x5A000010 | speed_id;    // GPCLK0 on
  return 0;
  
}
void setup_gpio()
{
   // Always use volatile pointer!
   volatile unsigned *gpclk;
   gpio =  setup_io(GPIO_BASE);
   gpio10 = gpio+10;
   gpio7 = gpio+7;
   gpio13 = gpio+13;
   gpio1 = gpio+1;
   gpclk = setup_io(GPCLK_BASE);
   INP_GPIO(20);
   SET_GPIO_ALT(20,5);
   if (gpclk != 0)
    clock_ena(gpclk, 6, 2, 2048); // 1st=clock control address, 2nd=port number, 500Mhz -> 100Mhz
   else
    exit(0);
} // setup_io

void *clk_map;
volatile unsigned * clk;

static unsigned char ROM[] = {
#include "Antarctic.data"
//#include "Gradius.data"
//#include "Zemix30.data"
//#include "game126.data"
};	

int iocount[256];
int memcount[256*256];
void signalfunc(int signal)
{
    int addrline[16];
    int count = 0;
    memset(addrline, 0, sizeof(addrline));
    if (signal == 2)
    {
        for(int i=0; i < 256; i++)
        {
            if (iocount[i] > 0)
                printf("IOx%02x (%d)\n", i, iocount[i]);
        }
#if 1        
        for(int i=0; i < 256*256; i++)
        {
            if (memcount[i] > 0)
            {
                count++;
                for(int j=0; j < 16; j++)
                    if (i & (1<<j))
                        addrline[j]++;
                //printf("%04x (%d)\n", i, memcount[i]);
            }
        }
        for(int i=0; i < 16;i++)
            printf("%d-%d\n", i, addrline[i]);
        printf("count=%d\n", count);
#endif        
        exit(-1);
    }
    return;
}
int main(int argc, char **argv)
{
    // Set up gpi pointer for direct register access
    int a, b;
    int byte;
    unsigned char RAM[0x10000];
   // setup_gpio();
    gpioInitialise();
    gpioSetSignalFunc(11, signalfunc);
    gpioSetSignalFunc(2, signalfunc);
//    gpioCfgSetInternals(PI_CFG_STATS);
    for (int i = 0; i < 28; i++)
        if (i != 20)
            gpioSetMode(i, PI_OUTPUT);
    gpioSetMode(25, PI_INPUT);
    gpioSetMode(27, PI_INPUT);
    gpioHardwareClock(20, 50000000);
    printf("ROM size:%d\n", sizeof(ROM));
    memcpy(RAM+0x4000, ROM, sizeof(ROM));
//  printf ("%07x\n", gpioRead_Bits_0_31() & 0xfffffff);
//  while(!(gpioRead_Bits_0_31() & ACK));
//    for (int i = 0; i < 100; i ++)
    while(1)
    {
        b = gpioRead_Bits_0_31();
        if (b & (ATN | RESET))
        {
            b = gpioRead_Bits_0_31();
            a = b & 0xffff;
            if (b & RW)
            {
                gpioWrite_Bits_0_31_Clear(RATN | CMD);
                gpioWrite_Bits_0_31_Set(RATN | CMD1);
//                while(!(gpioRead_Bits_0_31() & ACK));
                byte = gpioRead_Bits_0_31() & 0xff;
            }
            else if (!(b & (IOMEM | SLTSL)))
            {
                gpioWrite_Bits_0_31_Clear(RATN | CMD | 0xff);
                gpioWrite_Bits_0_31_Set(RATN | CMD2 | (byte = RAM[a]));
//                while(!(gpioRead_Bits_0_31() & ACK));
            }
            if (b & IOMEM)
            {
//                printf("IO0x%02x-%02x(%c)\n", a & 0xff, byte, b & RW ? 'w' : 'r');
            }
            else
  //          if (a == 0x4000 || a == 0x4001)
                printf("0x%04x-%02x(%c)%c\n", a, byte, b & RW ? 'w' : 'r', b & SLTSL ? 'm' : 's');
            if (b & IOMEM)
                iocount[a & 0xff]++;
            else
                memcount[a]++;
//            gpioWrite(26, 0);
        }
        else
            printf("0x%08x   %c%c%c%c\r", b, b & ATN ? 'A' : ' ', b & RW ? 'W' : 'R', b & IOMEM ? 'I' : 'M', b & RESET ? 'N' : 'R');
    }
}
