/* $Id$
 *  CMDINPUT.C - handles command input (tab completion, history, etc.)
 */

#include "../config.h"

#include <assert.h>
#include <dos.h>
#include <stdio.h>
#include <io.h>
#include <string.h>

#include "../include/command.h"
#include "../include/keys.h"
#include "../include/misc.h"

static unsigned orgx, orgy;		/* start of current line */

#if defined(NEC98)
unsigned mywherex (void) {
    return 1U + *(unsigned char far *)MK_FP(0x60, 0x11c);
}
unsigned mywherey (void) {
    return 1U + *(unsigned char far *)MK_FP(0x60, 0x110);
}
#elif defined(IBMPC)
#define MK_PTR(type,seg,ofs) ((type FAR*) MK_FP (seg, ofs))
/* safer edition of MK_FP (Arkady) */
typedef struct { unsigned char col, row; } SCRPOS;
#define _scr_page      (* MK_PTR (volatile const unsigned char, 0, 0x462))
#define _scr_pos_array    MK_PTR (volatile const SCRPOS, 0, 0x450)
unsigned mywherex (void) {
    return _scr_pos_array [_scr_page].col + 1;
}

unsigned mywherey (void) {
    return _scr_pos_array [_scr_page].row + 1;
}
#endif

/* set cursor state for insert/overwrite mode */
#if defined(NEC98)

#define GDC_STATUS 0x60
#define GDC_FIFOREAD 0x62
#define GDC_COMMAND 0x62
#define GDC_FIFOWRITE 0x60

static int use_tstamp_wait;

static void wait_for_gdc(void)
{
	/* tRCY,tWCY -> 4 clk (min) */
	/* 400ns per clock (for 2.5Mhz) * 4 = 1600ns */
	if (use_tstamp_wait) {
		outp(0x5f, 0); /* wait (longer than) 600ns */
		outp(0x5f, 0); /* wait (longer than) 600ns */
		outp(0x5f, 0); /* wait (longer than) 600ns */
	} else {
		/* put `software wait' here if you need... */
	}
}
static void outp_gdc(int port, unsigned char value)
{
	wait_for_gdc();
	outp(port, value);
}

static void setcursorstate_nec98(int insert)
{
	int show_cursor;
	unsigned char cursor_rows, top, bottom, blink_rate;
	unsigned char f[3];

	/* check port 5fh is capable of I/O wait... */
	use_tstamp_wait =
# if 1
		((*(unsigned short far *)MK_FP(0, 0x500) & 0x1801) != 0);	/* not PC-9801(original)/E/F/M */
# else
		(*(unsigned char far *)MK_FP(0, 0x45b) & 0x80) || (*(unsigned char far *)MK_FP(0, 0x458) & 0x80);	/* time stamper or NESA machine */
# endif

	/* for NEC98: insert -> do not blink, not insert -> blink */
	show_cursor = 1;
	blink_rate = 0x0d;
	cursor_rows = *(unsigned char far *)MK_FP(0x0, 0x53b);
	top = 0;
	bottom = cursor_rows;
	f[0] = (cursor_rows & 0x1f) | (show_cursor ? 0x80 : 0x00);
	f[1] = ((blink_rate & 3) << 6) | (insert ? 0x20 : 0x00) | (top & 0x1f);
	f[2] = ((bottom & 0x1f) << 3) | ((blink_rate >> 2) & 7);
	
	/* wait until GDC is ready (FIFO Empty) */
	while(1) {
		enable();
		wait_for_gdc();
		disable();
		if (inp(GDC_STATUS) & 0x04) break;
	}
	/* write command and parameters to GDC */
	outp_gdc(GDC_COMMAND, 0x4b);	/* CCHAR (CSRFORM) command */
	outp_gdc(GDC_FIFOWRITE, f[0]);
	outp_gdc(GDC_FIFOWRITE, f[1]);
	outp_gdc(GDC_FIFOWRITE, f[2]);
	
	wait_for_gdc();
	enable();
	goxy(mywherex(), mywherey());
}

# define setcursorstate setcursorstate_nec98

#elif defined(IBMPC)

static void setcursorstate_ibmpc(int insert)
{
	IREGS r;
	unsigned char cur_mode = *(unsigned char far *)MK_FP(0x40, 0x49);
	
	/* for IBMPC: insert -> _NORMALCURSOR, not insert -> _SOLIDCURSOR */
	r.r_ax = 0x0100;
	if (cur_mode == 7)
		r.r_cx = insert ? 0x0b0c : 0x010c;
	else
		r.r_cx = insert ? 0x0607 : 0x0107;
	intrpt(0x10, &r);
}

# define setcursorstate setcursorstate_ibmpc

#else

static void setcursorstate_ibmpc(int insert)
{
# error need platform-specific setcursorstate()
}

#endif

#define wherex mywherex
#define wherey mywherey
#define _setcursortype my_setcursortype

static int isworddelimiter(unsigned c)
{
	return c == ' ' || c == '\t';
}

/* Print a character to current cursor position
	Updates cursor postion
 */
void outc(char c)
{
	if (c == '\n') dos_write(1, "\r\n", 2); else dos_write(1, &c, 1);
}
/* Print a blank to current cursor postion
	Updates cursor position
 */
static void outblank(void)
{	outc(' ');
}

/* Print string to current cursor position
	Updates cursor position
 */
void outs(const char * const s)
{	assert(s);
	printf("%s", s);
}
/* Print string to cursor position and append one blank
	Updates cursor postion
 */
static void outsblank(const char * const s)
{	outs(s);
	outblank();
}

/* read in a command line */
void readcommandEnhanced(char * const str, const int maxlen)
{
	unsigned char insert = 1;
	unsigned ch;
#ifdef FEATURE_FILENAME_COMPLETION
	unsigned lastch = 0;
#endif
#ifdef FEATURE_HISTORY
	int histLevel = 0;
	char prvLine[MAX_INTERNAL_COMMAND_SIZE];
#endif
	unsigned curx;
	unsigned cury;
	int count;
	unsigned current = 0;
	unsigned charcount = 0;

	assert(str);
	assert(maxlen <= MAX_INTERNAL_COMMAND_SIZE);

	/* if echo off, don't print prompt */
	if(echo)
		printprompt();

	orgx = wherex();
	orgy = wherey();
	memset(str, 0, maxlen);

	setcursorstate(insert);

#ifdef FEATURE_HISTORY
	histGet(histLevel - 1, prvLine, sizeof(prvLine));
#endif

	do {
		ch = cgetchar();

		if(cbreak)
			ch = KEY_CTL_C;

		switch(ch) {
		case KEY_BS:               /* delete character to left of cursor */

			if(current > 0 && charcount > 0) {
			  if(current == charcount) {     /* if at end of line */
				str[current - 1] = 0;
				if (wherex() != 1)
				  outs("\b \b");
				else
				{
				  goxy(MAX_X, wherey() - 1);
				  outblank();
				  goxy(MAX_X, wherey() - 1);
				}
			  }
			  else
			  {
				for (count = current - 1; count < charcount; count++)
				  str[count] = str[count + 1];
				if (wherex() != 1)
				  goxy(wherex() - 1, wherey());
				else
				  goxy(MAX_X, wherey() - 1);
				curx = wherex();
				cury = wherey();
				outsblank(&str[current - 1]);
				goxy(curx, cury);
			  }
			  charcount--;
			  current--;
			}
			break;

		case KEY_INSERT:           /* toggle insert/overstrike mode */
			insert ^= 1;
				setcursorstate(insert);
			break;

		case KEY_DELETE:           /* delete character under cursor */

			if (current != charcount && charcount > 0)
			{
			  for (count = current; count < charcount; count++)
				str[count] = str[count + 1];
			  charcount--;
			  curx = wherex();
			  cury = wherey();
			  outsblank(&str[current]);
			  goxy(curx, cury);
			}
			break;

		case KEY_HOME:             /* goto beginning of string */

			if (current != 0)
			{
			  goxy(orgx, orgy);
			  current = 0;
			}
			break;

		case KEY_END:              /* goto end of string */

			if (current != charcount)
			{
			  goxy(orgx, orgy);
			  outs(str);
			  current = charcount;
			}
			break;

#ifdef FEATURE_FILENAME_COMPLETION
		case KEY_TAB:		 /* expand current file name */
			if(current == charcount) {      /* only works at end of line */
			  if(lastch != KEY_TAB) { /* if first TAB, complete filename */
				complete_filename(str, charcount);
				charcount = strlen(str);
				current = charcount;

				goxy(orgx, orgy);
				outs(str);
				if ((strlen(str) > (MAX_X - orgx)) && (orgy == MAX_Y + 1))
				  orgy--;
			  } else {                 /* if second TAB, list matches */
				if (show_completion_matches(str, charcount))
				{
				  printprompt();
				  orgx = wherex();
				  orgy = wherey();
				  outs(str);
				}
			  }
			}
			else
			  beep();
			break;
#endif

		case KEY_ENTER:            /* end input, return to main */

#ifdef FEATURE_HISTORY
			if(str[0])
			  histSet(0, str);      /* add to the history */
#endif

			outc('\n');
			break;

		case KEY_CTL_C:       		/* ^C */
		case KEY_ESC:              /* clear str  Make this callable! */

			clrcmdline(str, maxlen);
			current = charcount = 0;

			if(ch == KEY_CTL_C && !echo) {
			  /* enable echo to let user know that's this
				is the command line */
			  echo = 1;
			  printprompt();
			}
			break;

		case KEY_RIGHT:            /* move cursor right */

			if (current != charcount)
			{
			  current++;
			  if (wherex() == MAX_X)
				goxy(1, wherey() + 1);
			  else
				goxy(wherex() + 1, wherey());
				break;
			}
			/* cursor-right at end of string grabs the next character
				from the previous line */
			/* FALL THROUGH */

#ifndef FEATURE_HISTORY
			break;
#else
		case KEY_F1:       /* get character from last command buffer */
			  if (current < strlen(prvLine)) {
				 outc(str[current] = prvLine[current]);
				 charcount = ++current;
			  }
			  break;
			  
		case KEY_F3:               /* get previous command from buffer */
			if(charcount < strlen(prvLine)) {
				outs(strcpy(&str[charcount], &prvLine[charcount]));
			   current = charcount = strlen(str);
		   }
		   break;

		case KEY_UP:               /* get previous command from buffer */
			if(!histGet(--histLevel, prvLine, sizeof(prvLine)))
				++histLevel;		/* failed -> keep current command line */
			else {
				clrcmdline(str, maxlen);
				strcpy(str, prvLine);
				current = charcount = strlen(str);
				outs(str);
				histGet(histLevel - 1, prvLine, sizeof(prvLine));
			}
			break;

		case KEY_DOWN:             /* get next command from buffer */
			if(histLevel) {
				clrcmdline(str, maxlen);
				strcpy(prvLine, str);
				histGet(++histLevel, str, maxlen);
				current = charcount = strlen(str);
				outs(str);
			}
			break;

		case KEY_F5: /* keep cmdline in F3/UP buffer and move to next line */
			strcpy(prvLine, str);
			clrcmdline(str, maxlen);
			outc('@');
			if(orgy >= MAX_Y) {
				outc('\n');			/* Force scroll */
				orgy = MAX_Y;
			} else {
				++orgy;
			}
			goxy(orgx, orgy);
			current = charcount = 0;

			break;

#endif

		case KEY_LEFT:             /* move cursor left */
			if(current > 0) {
			  current--;
			  if (wherex() == 1)
				goxy(MAX_X, wherey() - 1);
			  else
				goxy(wherex() - 1, wherey());
			}
			break;

#if defined(KEY_CTRL_LEFT)
		case KEY_CTRL_LEFT:	/* move cursor left to begin of word */
			while(current > 0) {
			  current--;
			  if (wherex() == 1)
				goxy(MAX_X, wherey() - 1);
			  else
				goxy(wherex() - 1, wherey());

			  if(isworddelimiter(str[current-1])	/* ignore current == 0 */
			   && !isworddelimiter(str[current]))
			     break;
			}
			break;
#endif

#if defined(KEY_CTRL_RIGHT)
		case KEY_CTRL_RIGHT:	/* move cursor right to begin of word */
			while(current < charcount) {
			  current++;
			  if (wherex() == MAX_X)
				goxy(1, wherey() + 1);
			  else
				goxy(wherex() + 1, wherey());

			  if(isworddelimiter(str[current-1])
			   && !isworddelimiter(str[current]))
			     break;
			}
			break;
#endif

		default:                 /* insert character into string... */

			if ((ch >= 32 && ch <= 255) && (charcount != (maxlen - 2)))
			{
			  if (insert && current != charcount)
			  {
				for (count = charcount; count > current; count--)
				  str[count] = str[count - 1];
				str[current++] = ch;
				curx = wherex() + 1;
				cury = wherey();
				outs(&str[current - 1]);
				if ((strlen(str) > (MAX_X - orgx)) && (orgy == MAX_Y + 1))
				  cury--;
				goxy(curx, cury);
				charcount++;
			  }
			  else
			  {
				if (current == charcount)
				  charcount++;
				str[current++] = ch;
				outc(ch);
			  }
			  if ((strlen(str) > (MAX_X - orgx)) && (orgy == MAX_Y + 1))
				orgy--;
			}
			else
			  beep();
			break;
		}
#ifdef FEATURE_FILENAME_COMPLETION
		lastch = ch;
#endif
	} while(ch != KEY_ENTER);

	setcursorstate(insert);
}
