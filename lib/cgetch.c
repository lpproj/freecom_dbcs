/*	$Id$

	Get a character out-of-band and honor Ctrl-Break characters
	Invisible.

	This file bases on MISC.C of FreeCOM v0.81 beta 1.

	$Log$
	Revision 1.4  2006/06/12 04:55:42  blairdude
	All putchar's now use outc which first flushes stdout and then uses write to write the character to the console.  Some potential bugs have been fixed ( Special thanks to Arkady for noticing them :-) ).  All CONIO dependencies have now been removed and replaced with size-optimized functions (for example, mycprintf, simply opens "CON" and directly writes to the console that way, and mywherex and mywherey use MK_FP to access memory and find the cursor position).  FreeCOM is now
	significantly smaller.

	Revision 1.3  2006/06/11 06:50:49  blairdude
	Removed usage of Turbo C's getch to remove idle ...getch... object modules being compiled in.
	
	Revision 1.2  2004/02/01 13:52:17  skaus
	add/upd: CVS $id$ keywords to/of files
	
	Revision 1.1  2001/04/12 00:33:52  skaus
	chg: new structure
	chg: If DEBUG enabled, no available commands are displayed on startup
	fix: PTCHSIZE also patches min extra size to force to have this amount
	   of memory available on start
	bugfix: CALL doesn't reset options
	add: PTCHSIZE to patch heap size
	add: VSPAWN, /SWAP switch, .SWP resource handling
	bugfix: COMMAND.COM A:\
	bugfix: CALL: if swapOnExec == ERROR, no change of swapOnExec allowed
	add: command MEMORY
	bugfix: runExtension(): destroys command[-2]
	add: clean.bat
	add: localized CRITER strings
	chg: use LNG files for hard-coded strings (hangForEver(), init.c)
		via STRINGS.LIB
	add: DEL.C, COPY.C, CBREAK.C: STRINGS-based prompts
	add: fixstrs.c: prompts & symbolic keys
	add: fixstrs.c: backslash escape sequences
	add: version IDs to DEFAULT.LNG and validation to FIXSTRS.C
	chg: splitted code apart into LIB\*.c and CMD\*.c
	bugfix: IF is now using error system & STRINGS to report errors
	add: CALL: /N
	
 */

#include "../config.h"

#include <dos.h>

#include "../include/command.h"
#include "../include/keys.h"
#include "../include/misc.h"


#if defined(NEC98)

#define kb_ky_sts(index,mask) \
	(*(unsigned char far *)MK_FP(0,0x52a + (index)) & (mask))
#define kb_shft_sts	*(unsigned char far *)MK_FP(0,0x53a)
#define isCtrlPressed	(kb_shft_sts & 0x10)

static int key_to_scan(int k)
{
	struct { int key, scancode; } keytbl[] = {
		{ 0x1b53, KEY_F1 },
		{ 0x1b54, KEY_F2 },
		{ 0x1b55, KEY_F3 },
		{ 0x1b56, KEY_F4 },
		{ 0x1b57, KEY_F5 },
		{ 0x1b45, KEY_F6 },
		{ 0x1b4a, KEY_F7 },
		{ 0x1b50, KEY_INS /* KEY_F8 */ },
		{ 0x1b51, KEY_F9 },
		{ 0x1b5a, KEY_F10 },
		{ 0x000b, KEY_UP },
		{ 0x000a, KEY_DOWN },
		{ 0x1b44, KEY_DEL },
		{ 0x001a, KEY_HOME },
		{ 0, 0 }
	};
	int n = 0;
	while(keytbl[n].key) {
		if (k == keytbl[n].key)
			return keytbl[n].scancode;
		++n;
	}
	return k;
}

static int mykbhit_and_getch(IREGS *r)
{
	int loop_doskbhit = 2;
	while(loop_doskbhit--) {
		r->r_ax = 0x0600;
		r->r_dx = 0x00ff;
		intrpt(0x21, r);
		if (!(r->r_flags & 0x40)) return r->r_ax;
	}
	return 0;
}

int cgetchar(void)
{
	IREGS r;
	int c = 0;
	
	do {
		int doskbhit = mykbhit_and_getch(&r);
		if (doskbhit) {
			c = doskbhit & 0xff;
			switch(c) {
			case 0x1b:	/* ESC (and prefix for extra key) */
				if (!kb_ky_sts(0, 0x01)) {
					/* if not real ESC key was pressed, get next stroke */
					c = mykbhit_and_getch(&r);
					c = c ? (0x1b00 | (c & 0xff)) : 0x001b;
				}
				break;
			case 0x08:	/* LEFT (and BackSpace) */
				if (kb_ky_sts(7, 0x08))
					c = isCtrlPressed ? KEY_CTRL_LEFT : KEY_LEFT;
				break;
			case 0x0c:	/* RIGHT */
				if (kb_ky_sts(7, 0x10))
					c = isCtrlPressed ? KEY_CTRL_RIGHT : KEY_RIGHT;
				break;
			}
			c = key_to_scan(c);
			if (c) break;
			break;
		}
		/* intrpt(0x28, &r); */
	}while(c == 0);
	
	return c;
}


#else

static int mygetch( void )
{
    IREGS regs;
    regs.r_ax = 0x0700;
    intrpt(0x21, &regs);
    return (regs.r_ax)&0xFF;
}

#define getch mygetch

int cgetchar(void)
{	int c;

	if((c = getch()) == 0)
		c = SCANCODE(getch());		/* Scan code */

	if(c == KEY_CTL_C) {
		ctrlBreak = 1;
	}

	return c;
}

#endif
