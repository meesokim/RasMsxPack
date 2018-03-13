#ifndef RASMSXPACK
#define RASMSXPACK

#define V1

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

#endif