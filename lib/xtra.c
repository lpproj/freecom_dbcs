/*
 * XTRA.C
 * Extra function(s) for FreeCom(98) and/or FreeCom/V
 *
 * 2015-11-29 lpproj
 *
 */

#include <dos.h>
#include <io.h>
#include <stdio.h>
#include "../include/misc.h"

int mymachine = MYMACHINE_UNKNOWN;

#if defined(NEC98)

#define nec98_delay	mydelay
#define nec98_sound	mysound
#define nec98_nosound	mynosound

# define GDC_STATUS 0x60
# define GDC_FIFOREAD 0x62
# define GDC_COMMAND 0x62
# define GDC_FIFOWRITE 0x60

static char nec98_use_port5f;

void nec98_wait_gdc(void)
{
	/* tRCY,tWCY -> 4 clk (min) */
	/* 400ns per clock (for 2.5Mhz) * 4 = 1600ns */
	if (nec98_use_port5f) {
		outp(0x5f, 0); /* wait (longer than) 600ns */
		outp(0x5f, 0); /* wait (longer than) 600ns */
		outp(0x5f, 0); /* wait (longer than) 600ns */
	} else {
		/* put `software wait' here if you need... */
	}
}
void nec98_outp_gdc(int port, unsigned char value)
{
	nec98_wait_gdc();
	outp(port, value);
}
unsigned char nec98_inp_gdc(int port)
{
	nec98_wait_gdc();
	return inp(port);
}

#if 0
void nec98_setcursorblinkrate(int blink_rate)
{
	int show_cursor, do_blink;
	unsigned char cursor_rows, top, bottom;
	unsigned char f[3];

	show_cursor = (blink_rate >= 0);
	do_blink = (show_cursor && blink_rate <= 0x1f);
	blink_rate &= 0x1f; /* 0x0d */
	cursor_rows = *(unsigned char far *)MK_FP(0x0, 0x53b);
	top = 0;
	bottom = cursor_rows;
	f[0] = (cursor_rows & 0x1f) | (show_cursor ? 0x80 : 0x00);
	f[1] = ((blink_rate & 3) << 6) | (do_blink ? 0x00 : 0x20) | (top & 0x1f);
	f[2] = ((bottom & 0x1f) << 3) | ((blink_rate >> 2) & 7);
	/* wait for GDC ready (FIFO Empty) */
	while(1) {
		enable();
		nec98_wait_gdc();
		disable();
		if (nec98_inp_gdc(GDC_STATUS) & 0x04) break;
	}
	/* write command and parameters to GDC */
	nec98_outp_gdc(GDC_COMMAND, 0x4b);	/* CCHAR (CSRFORM) command */
	nec98_outp_gdc(GDC_FIFOWRITE, f[0]);
	nec98_outp_gdc(GDC_FIFOWRITE, f[1]);
	nec98_outp_gdc(GDC_FIFOWRITE, f[2]);

	nec98_wait_gdc();
	enable();
}
#endif

/* roughly delay (ticked by VSYNC) */
/*
    9801/9821 normal resolution
    resolution  hsync       vsync
    640x200     15.98kHz    61.47Hz
    640x400     24.83kHz    56.42Hz
    640x400     31.47kHz    70.10Hz
   (640x480     31.47kHz    59.94Hz)
*/
void nec98_delay(unsigned ms)
{
	unsigned long ms_x10000 = 10000UL * ms;
	unsigned long onetick, ticks;
	unsigned vsyncHz_x100;

	if ((peekb(0, 0x459) & 1))			/* 480line (guess 31.47kHz) */
		vsyncHz_x100 = 5994;
	else if (peekb(0, 0x54c) & 0x20)	/* hsync 31.47kHz */
		vsyncHz_x100 = 7010;
	else								/* 400/200line */
		vsyncHz_x100 = (peekb(0, 0x53c) & 0x80) ? 5642 : 6147;

	onetick = (10000UL * 1000UL * 100U) / vsyncHz_x100;

	enable();
	for(ticks = 0; ticks < ms_x10000; ticks += onetick) {
		/* you can abort by pressing the STOP key... */
		if (*(unsigned char far *)MK_FP(0, 0x536) & 1) break;
		while( !(nec98_inp_gdc(GDC_STATUS) & 0x20)) ;
		while( (nec98_inp_gdc(GDC_STATUS) & 0x20)) ;
	}
}

void nec98_sound(unsigned f)
{
	IREGS r;
	
	(void)f;	/* todo: support freq. */
	r.r_ax = 0x1700;
	intrpt(0x18, &r);	/* wake beep */
}

void nec98_nosound(void)
{
	IREGS r;
	
	r.r_ax = 0x1800;
	intrpt(0x18, &r);	/* beep quiet... */
}

#elif defined(IBMPC)

void mydelay(unsigned ms)
{
	delay(ms);
}

void mysound(unsigned f)
{
	sound(f);
}

void mynosound(void)
{
	nosound();
}

#endif

int init_mymachine(void)
{
#if defined(NEC98)
	mymachine = MYMACHINE_NEC98;
	nec98_use_port5f = 
# if 1
		((*(unsigned short far *)MK_FP(0, 0x500) & 0x1801) != 0);	/* not PC-9801(original)/E/F/M */
# else
		(*(unsigned char far *)MK_FP(0, 0x45b) & 0x80) || (*(unsigned char far *)MK_FP(0, 0x458) & 0x80);	/* time stamper or NESA machine */
# endif
#elif defined(IBMPC)
	mymachine = MYMACHINE_IBMPC;
#else
	mymachine = MYMACHINE_UNKNOWN;
#endif
	return 0;
}

