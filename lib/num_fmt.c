/*	$Id$

 * convert
 *
 * insert commas into a number

	This file bases on DIR.C of FreeCOM v0.81 beta 1.

	$Log$
	Revision 1.3  2004/08/02 20:38:56  skaus
	bugfix: use of the nls thousands separator in convert() [Eduardo Casino]

	Revision 1.2  2004/02/01 13:52:17  skaus
	add/upd: CVS $id$ keywords to/of files
	
	Revision 1.1  2001/04/12 00:33:53  skaus
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

#include <assert.h>
#include <string.h>

#include "../include/misc.h"
#include "../include/nls.h"

void convert(unsigned long num, unsigned int billions, char * const des)
{
  char temp[sizeof(num) * 8];
  int c = 0;

  assert(des);

  if(num == 0) {
    des[0] = '0';
    des[1] = 0;
  } else {
    refreshNLS();
    assert(nlsBuf);
    temp[31] = 0;
   	do { 
      if (((c + 1) % 4) == 0)
        temp[30 - c++] = nlsBuf->thousendsSep[0];
      temp[30 - c++] = (char)(num % 10) + '0';
      num /= 10;
      if (c == 11 && num == 0) {
        num = billions;
        billions = 0;
      }
    } while(num != 0 || billions != 0);
    strcpy(des, &temp[31 - c]);
  }
}
