/* $Id$
   $Locker$	$Name$	$State$

	Declarations for the supplemental I/O functions.

	To overcome the difference between Micro-C and ANSI C:
	All identifiers starts with an uppercase 'F', the leading
	lowercase 'f' is stripped, if present.

	Functions that will automatically terminate the program with an
	appropriate error message if the action fails. They have an uppercase
	'E' instead of the 'F'. The Eopen(), Ecreate() and Eclose() functions
	use the Fxopen() and Fxclose() to save the extended attributes;
	they use the filename to issue with the error messages.

	Three functions support the "extended attributes per file" feature:
	Fxopen(): open a file and generate & store the extended attributes
	Fxclose(): close the file and destroy the extended attributes
	Fxinfo(): retrieve the extended attributes

	The implementation is such that Fxopen() returns a normal (FILE*),
	in order to be passed to the normal F*() functions, but also sets
	up an association between the (FILE*) pointer and the extended
	attributes.

	If the macro SUPPL_USE_EXTENDED_ATTRIBUTES is defined prior
	#include'ing this header file, Fxopen() and Fxclose() replace
	Fopen() and Fclose() by default.

   $Log$
   Revision 1.2  2000/07/09 22:19:22  skaus
   + Support for international strings without recompiling
   + Useage of TC++1

   Revision 1.10  2000/03/31 09:09:34  ska
   add: DBG_CLOSELOG, suppl_l_openmode
   add: SYSLOG_ALWAYS_FLUSHLOG
   add: fcommit(), Fcommit(), Fflush(), commit()
   add: suppl_log_flush(), DBG_FLUSHLOG, DBG_ALWAYS_FLUSHLOG
   fix: dfnsearch(): DBG_ARGUMENT()
   chg: F[gs]etpos() -> true replacements of f[gs]etpos(); removed
   	Fp[gs]etpos(); added FF[gs]etpos() using a non-pointer argument
   bugfix: secure string functions: memory functions don't accept length == 0
   add: MKSTRFCT.PL: generate DOC\SSTR.FCT
   fix: dfnsplit(): a pathname without path components returns the root
   	directory as path portion
   add: dfnsplit(): debug output of return values of found drive/path/name/ext
   fix: dfnsqueeze(): DBG_ENTER() contains wrong names
   fix: dfnsplit(): chkHeap in drive spec detection routine breaks if/else
   chg: moved NLS-depended stuff from DFN.H into NLS_F.H
   add: integer SUPPL error codes -- all functions returning (int) error codes
   	return unique codes used throughout all SUPPL, see ERRCODES.H

   Revision 1.9  1999/12/13 02:23:44  ska
   add: debug subsystem
   bugfix: Fposcmp(): If abs(pos1 - pos2) > 32767, the return value is random
   add: strend() --> returns the address of the NUL byte of a string
   chg: StrAppend() --> StrAppQStr()
   add: StrQuote(), StrUnquote()
   bugfix: Strspn(): if any parameter is NULL, returns NULL now
   bugfix: MemiCmp(): returned wrong sign
   bugfix: dfnstat(): Win95 LFN's entry never or invalidly detected
   add: dfndelim() & dfndelim2()
   chg: environ.h: mcb_toenv() --> macro mcb_env()
   fix: cfg_osen.c: cfg_ostkEnum(): If output stack empty --> random return value
   fix: strnum(): '0x' prefix detection fails
   chg: long*(): Except longcmp() all functions are void; for portable subsys
   bugfix: Erealloc(): if len == 0, the program is terminated erroreously
   add: Fcopyi() & Fcopyl() & Fcopybuf()
   chg: Fcopyto(): 'topos' must contain a valid position returned by Fgetpos()
   add: supplio.h: Fpos2dword() & Fppos2dword(): Extract (dword) pos from(fpos_t)
   chg: env_newsize() renamed into env_setsize()
   bugfix: env_check(): accepts no segm==0, as it is standard
   sub: dynstr.h: STR_SAVE_MODES, not used anymore (StrSaveTokens() removed)
   chg: dynstr.h: STR_SAVED_TOKENS is its own type now
   bugfix: StrTok*(): if(st) effects first assignment only
   bugfix: StrTail(): string is strdup()'ed two times
   bugfix: _fStriCmp(): temporary storage into (char) is unportable
   bugfix: StriCmp(): sign is inverted
   bugfix: StriCmp(): accepts no NULL parameters
   add: DFN_LFN to dfn.h and dfnstat()
   bugfix: UNC-aware dfnmatch2(): function name wrong
   sub: removed "Target compilers" note everywhere
   fix: dfnstat(): returns DFN_DIRECTORY on "X:" and "X:\"
   chg: No SUPPL function will invoke openlog() automatically

   Revision 1.8  1999/04/13 00:11:50  ska
   bugfix: dfnexpand(): fname == "" --> fname := "."
   bugfix: dfnsearch(): Searching root instead of '.' if no search path
   add: syslog subsystem
   fix: spelling in comments etc.
   bugfix: not all SUPPORT_UNC_PATH macros correct
   add: INI file random access subsystem
   add: Fmaxbuf()
   add: Fcopy(), Fcopyto()
   add: Fposcpy(), Fposcmp()
   add: memzero(): unportable memory zero'ing function

   Revision 1.7  1999/01/18 05:32:29  ska
   add: INI file writing functions
   fix: dfnstat() uses wrong register for result
   add: dfnfreplace(), dfnbakname()

   Revision 1.6  1999/01/13 03:26:03  ska
   chg: renamed *.inc into *.loc
   add: Fgetline() & Egetline()
   sub: cfgfile subsystem to be added later
   add: FFget()/FFput()
   add: appName subsystem

   Revision 1.5  1998/12/12 02:41:51  ska
   chg: F_tmpfile(): returns NULL if fnam == NULL (to simplify Ftmpfile())

   Revision 1.4  1998/12/10 03:43:24  ska
   add: Fxcloseall()
   fix: Ftmpfile(): to set the "istmp" attribute
   chg: Fxopen() renamed to F_xopen() and returns the extended attributes
   	added new Fxopen() to call F_xopen()
   fix: cfg_rspwr(): Eget() instead of Eput()
   fix: env_fetch(): ignoring segm==0, returning variable name as well
   chg: cfgSplitArg() & cfgSplitBuffer() independed on what string is
   	to be splitted
   fix: getopt(): longname == NULL
   fix: pushargv: wrong parameters in structure
   fix: getarg(): C(_stk) == NULL --> return NULL
   fix: getopt(): S(buf)[ ] = new char

   Revision 1.3  1998/12/08 04:04:11  ska
   add: dpeekc(), dpeekb(), dpeekw()
   add: Eresize()
   chg: env_change() to use env_ovrVarOffset() --> replaced variables
   	remain at the position they were before
   add: env_ovrVarOffset(), env_insVarOffset(), env_appVar()
   sub: env_addVar() [replaced by env_appVar()]
   bugfix: DOSalloc(): add FreeDOS compliant setting of flags prior call DOS

   Revision 1.2  1998/12/04 06:00:44  ska
   add: fpos_t/fsetpos()/fgetpos() to PAC
   bugfix: DOSalloc(): If called with length == 0, the allocation
   	strategy is not resetted
   bugfix: env_newsize(): Grow block, if fails completely, the old environment
   	was destroyed unneceesaryly
   add: getbootdisk()
   add: Ftmpfile(), Ftmpnam(), Etmpfile()
   add: dfnstat(), dfnmktmp()
   add: dfnwrdir(), dfnstat(), dfnmkfile()
   fix/chg: StrTrim(): returns "s" if realloc() fails
   fix: _getdcwd(): removed Carry()
   bugfix: dfnsqueeze(): In non-UNC mode, there was nothing actually
   	sequeezed, except the slashes were flipped
   add: env_fetch(): DOS-ish getenv()
   add: _fdupstr(): Duplicates far string into local heap

   Revision 1.1  1998/11/25 09:33:08  ska
   Initial revision

*/

#ifndef __SUPPLIO_H
#define __SUPPLIO_H

#include <stdio.h>
#include <supl_def.h>
#ifndef __PORTABLE_H
#include <portable.h>
#endif

#ifdef _PAC_NOCLIB_
/*
 *	Missing fpos_t and assoc. functions
 */

typedef long fpos_t;

//int fsetpos(FILE * const fp, const fpos_t * const pos);
#define fsetpos(stream,position) fseek((stream), *(position), 0)
int fgetpos(FILE * const fp, fpos_t * const pos);
/* fsetpos() is similar to fseek() and fgetpos() to ftell(), though,
	better portabel. Both return 0 (zero) on success.
*/
#endif

/*
 *	Macros passed into Fsetmode()
 */
#define FF_RD 1		/* read access */
#define FF_WR 2		/* write access */
#define FF_RW 3		/* read & write access */

/*
 *	Replacement of stdio functions
 *	All of these macros evaluate all parameter exactly once.
 */
#define Fflush fflush
#define Fyclose fclose
#define Frewind rewind
#define Fgetc(fp) getc((fp))
#define Fputc(c,fp) putc((c),(fp))
#define Ffgetc(fp) fgetc((fp))
#define Ffputc(c,fp) fputc((c),(fp))
#define Fgets fgets
#define Fputs fputs
#define Ferror ferror
#define Fclearerr clearerr
#ifndef _MICROC_
#define Fget(buf,size,fp) fread((buf), 1, (size), (fp))
#define Fput(buf,size,fp) fwrite((buf), 1, (size), (fp))
#define Fgetpos(fp,pos) fgetpos((fp), (pos))
#define Fsetpos(fp,pos) fsetpos((fp), (pos))
#define Fsetmode(fp,mode) 	// overcome Micro-C bug with R&W files
#define Fseeki(fp,ofs)	fseek((fp), (long)(ofs), SEEK_CUR)
#define Fseekc(fp,pos)	Fseeki((fp), (ofs))
#define Fpseekc(fp,pos)	Fseeki((fp), *(ofs))
#define Fseeke(fp)	fseek((fp), (long)0, SEEK_END)
#define Fyopen fopen
#define Fseek(fp,pos,whence)	fseek((fp), (long)(pos), (whence))
#define Fpseek(fp,pos,whence)	Fseek((fp), *(pos), (whence))
#define Fileno(fp) fileno((fp))
#define Fdopen fdopen
#define Feof feof
#else
#include <file.h>
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif
#define fpos_t dword
#define Fput fput
#define Fget fget
void Fsetmode(FILE *fp, int mode);
int Fgetpos(FILE *fp, fpos_t *pos);
#define Fsetpos(fp,pos)	Fseek((fp), (pos), SEEK_SET)
int Fseeki(FILE *fp, int ofs);
#define Fseekc(fp,pos)	Fseek((fp), (pos), SEEK_CUR)
#define Fpseekc(fp,pos)	Fseekc((fp),(pos))
#define Fseeke(fp)	fseek((fp), 0, 0, SEEK_END)
FILE *Fyopen(char *fnam, char *mode);
FILE *Fdopen(int fd, char *mode);
int Fseek(FILE *fp, dword *pos, int whence);
#define Fpseek Fseek
#define Fileno(fp)	(fp)->FILE_handle
int Feof(FILE *fp);
#endif
#define FFgetpos(fp,pos) Fgetpos((fp), aS(pos))
#define FFsetpos(fp,pos) Fsetpos((fp), aS(pos))

/* Read/write a buffer to the file.
 *	Is a function in order to evaluate all parameters only once.
 *	Return:
 *		0: on success
 */
int FFget(void * const buf, size_t len, FILE *f);
int FFput(const void * const buf, size_t len, FILE *f);

/*
 * Commit a file
 *
 *	Flushes the buffers cached by the C library, and
 *	Flushes the buffers cached by the system and also makes sure
 *	that the directory structure is updated.
 */
#define Fcommit fcommit
int fcommit(FILE *fp);
int commit(int fd);

/* Read a line into dynamical memory.
 *
 *	There is always an '\n' (newline) character at the end of the line,
 *	even at the end of file without one.
 *
 *	Return:
 *		NULL: on failure (out of memory)
 *		else: pointer to malloc()'ed buffer, must be free()'ed by the user
 *			if the returned buffer is a pointer to "", EOF or a read
 *			error was encountered.
 *			Note: The "" must also be free()'ed.
 */
char *Fgetline(FILE * const f);


FILE *Ftmpfile(void);
/* Create and open a temporary file in binary mode

	Should be closed by Fxclose() in order to remove the file.

	Return:
		NULL: on failure (e.g. malloc() failed, no writeble directory found)
		else: opened FILE* pointer to be closed with Fxclose()
*/

FILE *F_tmpfile(const char * const fnam);
/* Create and open a temporary file in binary mode

	Should be closed by Fxclose() in order to remove the file.

	Return:
		NULL: on failure (e.g. malloc() failed, or fnam == NULL)
			removes external file 'fnam' if it exists
		else: opened FILE* pointer to be closed with Fxclose()
*/

char *Ftmpnam(const char * const ext);
/* Create a temporary filename, create it as a zero-length file and
	return its name.

	The name is overwritten the next time Ftmpnam(), Ftmpdir(),
	Etmpnam() or Etmpdir() is called.

	Return:
		NULL: on failure, e.g. malloc() failed, no writeble directory found
*/

char *Ftmpdir(void);
/* Return the current writeble temporary directory with a trailing
	backslash.

	The user must free() the returned string manually.
	This function overwrites the string returned by Ftmpnam() or
	Etmpnam().

	Return:
		NULL: on failure, e.g. malloc() failed, no directory found
		else: pointer to pathname
*/

//void Fposcpy(fpos_t * const dst, const fpos_t * const src);
#ifdef _MICROC_
#define Fposcpy(dst,src)	longcpy(dst, src)
#else
#define Fposcpy(dst,src)	memcpy((dst), (src), sizeof(fpos_t))
#endif
/* Copy the value of one position to another location
*/

	/* if debug enabled --> use the function and provide the test
		if fpos_t==long */
int Fposcmp(const fpos_t * const pos1, const fpos_t * const pos2);

#ifdef NDEBUG
#define Fposcmp(pos1,pos2)	\
	longcmp1(nM(*(unsigned long*))pos1, nM(*(unsigned long*))pos2)
#endif
/* Compare both positions
	Return:
		<0: if pos1 < pos2
		=0: if pos1 = pos2
		>0: if pos1 > pos2
*/

// void Fppos2dword(const fpos_t const *fpos, dword * const longPosp)
#ifdef _MICROC_
#define Fpos2dword(fpos,longPos) Fposcpy(fpos, longPos)
#define Fppos2dword(fpos,longPos) Fposcpy(fpos, longPos)
#else
#define Fpos2dword(fpos,longPos) (longPos) = (dword)(fpos);
#define Fppos2dword(fpos,longPos) *(longPos) = (dword)(fpos);
#endif
/* Extract the file position from a (fpos_t) type

	Note: There is _NO_ reverse function.

	Also note: (fpos_t) is known to contain more than just the
	file position, when special text handling code is enabled, such
	as multi-byte characters.
*/

int Fmaxbuf(byte ** const buf, size_t * const len);
/* Allocate are rather large amount of memory, usually a power of 2
	Return: 0 on success
 */

int Fcopyto(FILE * const fdst, FILE * const fsrc, const fpos_t * const pos);
/* Copy (a part of) a file to another.
	Both files must be already opened with the proper permissions and
	file modes (e.g. both either binary or text, fdst has write permission,
	fsrc has read permission).
	Any bytes from fsrc are copied to fdst upto, but excluding to the
	position 'pos'.
	If the soruce file is in text mode, it is possible that 'pos' points
	to an invalid file position. In this case the behaviour is undefined.

	Return:
		0: OK
		else: on failure
*/

int Fcopy(FILE * const fdst, FILE * const fsrc);
/* Copy any byte from fsrc into fdst
	Return: 0 on success
*/

int Fcopyi(FILE * const fdst, FILE * const fsrc, size_t len);
/* Copy len byte from fsrc into fdst
	Return: 0 on success
*/

int Fcopyl(FILE * const fdst, FILE * const fsrc, dword iM(*) len);
/* Copy len byte from fsrc into fdst
	Return: 0 on success
*/

int Fcopybuf(FILE * const fdst, FILE * const fsrc, byte * const buf
	, const size_t len, size_t * const num);
/* Copies one block from one stream into another one.
	Return: 0 on success
*/

/*
 *	Replace for the F*() functions to automatically terminate the
 *	program if the action fails.
 *
 *	If the FILE* pointer is associated with an extended attribute,
 *	the message contains the name of the file.
 */

FILE *Eopen(const char * const fnam, const char * const mode);
/* Open file as Fopen(), on failure return: "Cannot open file"
*/

FILE *Ecreate(const char * const fnam, const char * const mode);
/* Open file as Fopen(), on failure return: "Cannot create file"
*/

void Eclose(FILE *f);
/* Close the file and free all allocated resources. If the error flag
	is true, the program terminates with: "Error to access file"
*/

#define Erewind	Frewind
#define Egetc Fgetc
#define Efgetc Ffgetc
#define Efputc Ffputc
#define Eputc Fputc
#define Esetmode	Fsetmode
#define Efileno Fileno
/* Definitions that don't generate error states or, if they would be
	checked, it could be drastically performance drop */


char *Egets(char * const str, size_t size, FILE * const fp);
/* Inputs a line from the stream; on failure the program
	is terminated with: "Cannot read from file"
	Return: NULL: if EOF
			else: str
*/

char *Eputs(const char * const str, FILE * const fp);
/* Outputs a line into the stream; on failure the program
	is terminated with: "Cannot write into file"
	Return: str
*/

void *Eget(void * const buf, size_t size, FILE * const fp);
/* Read size bytes from the stream into the buffer; on failure
	terminate the program with: "Cannot read from file"
	Return: buf
*/

void Eput(const void * const buf, size_t size, FILE * const fp);
/* Write size bytes from the buffer into the stream; on failure
	terminate the program with: "Cannot write into file"
*/

#define EEgetpos(fp,pos) Egetpos((fp), aS(pos))
void Egetpos(FILE * const fp, fpos_t * const pos);
/* Retreive the file position from the stream; on failure terminate
	the program with: "Cannot access file"
*/

#define EEsetpos(fp,pos) Esetpos((fp), aS(pos))
void Esetpos(FILE * const fp, const fpos_t * const pos);
/* Seek to the position saved in *pos; on failure terminate the
	program with: "Cannot access file"
*/

#define Eclearerr Fclearerr
void Eerror(const FILE * const fp);
/* Terminate the program with "Cannot access file" if the ferror()
	statue is true.
*/

void Ereaderr(const FILE * const fp);
void Ewriteerr(const FILE * const fp);
/* Terminate the program with "Cannot read from file" or "Cannot
	write to file" if the ferror() status is true
*/

#ifndef _MICROC_
#define Eseeki(fp,ofs)	Eseek((fp), (long)(ofs), SEEK_CUR)
#define Eseekc(fp,pos)	Eseeki((fp), (ofs))
#define Epseekc(fp,pos)	Eseeki((fp), *(ofs))
#define Eseeke(fp)	Eseek((fp), (long)0, SEEK_END)
#define Epseek(fp,pos,whence)	Eseek((fp), *(pos), (whence))
void Eseek(FILE * const fp, dword pos, int whence);
#else
#define Eseeki(fp,ofs)	Efseek((fp), (ofs), SEEK_CUR)
#define Eseeke(fp)	Efseek((fp), 0, SEEK_END)
void Efseek(FILE *fp, int pos, int whence);
#define Eseekc(fp,pos)	Eseek((fp), (ofs), SEEK_CUR)
#define Epseekc(fp,pos)	Eseek((fp), (ofs), SEEK_CUR)
#define Epseek Eseek
void Eseek(FILE * fp, dword *pos, int whence);
#endif
/* On failure terminate with: "Cannot access file"
*/

/* Read a line into dynamical memory.
 *
 *	On error, an appropriate message is issued and the program
 *	is terminated.
 *
 *	There is always an '\n' (newline) character at the end of the line,
 *	even at the end of file without one.
 *
 *	Return:
 *		NULL: EOF (or read error)
 *		else: pointer to malloc()'ed buffer, must be free()'ed by the user
 */
char *Egetline(FILE * const f);

FILE *Etmpfile(void);
/* Create and open a binary temporary file. On failure the program
	is terminated with: Cannot create temporary file
	The file is create with mode "w+b"
*/

char *Etmpnam(const char * const ext);
/* Create a temporary filename with the extension ext. On failure
	terminate the program.

	The returned string is overwritten by the next call to Ftmpnam(),
	Ftmpdir(), Etmpnam() or Etmpdir().
*/

char *Etmpdir(void);
/* Return the current writeble temporary directory with a trailing backslash.
	On failure the program is terminated
	The returned string must be free()'ed by the user of this function.
	This function overwrites the string returned by Ftmpnam() or Etmpnam().
*/


/*
 *	Extended attributes
 */
struct Suppl_ExtAttr_ {	/* extended attributes */
	struct Suppl_ExtAttr_ *sea_nxt;
	FILE *sea_fp;		/* associated FILE* pointer; if NULL this is the
							default entry */
	char *sea_fnam;		/* absolute filename; dflt: "<<unknown filename>>" */
	FLAG sea_istmp;		/* is temporary file */
};
#ifdef _MICROC_
#define Suppl_ExtAttr struct Suppl_ExtAttr_
#else
typedef struct Suppl_ExtAttr_ Suppl_ExtAttr;
#endif

extern Suppl_ExtAttr suppl_dfltExtAttr;	/* Returned by Fxinfo() on failure */

Suppl_ExtAttr *F_xopen(const char * const fnam, const char * const mode);
FILE *Fxopen(const char * const fnam, const char * const mode);
FILE *Exopen(const char * const fnam, const char * const mode);
void Fxclose(FILE *fp);
void Exclose(FILE *fp);
Suppl_ExtAttr *F_xinfo(const FILE * const fp, Suppl_ExtAttr ** const prev);
Suppl_ExtAttr *Fxinfo(const FILE * const fp);
#define E_xinfo F_xinfo
#define Exinfo Fxinfo
/* Return the extended attributes associated with the FILE* pointer fp;

	If fp is not associated, a default entry is returned, which 'fp' is
	NULL, but all other members contain some sort of useful data

	Note: F_xinfo() is for internal puprose only.
*/

void Fxcloseall(void);
/* Close all functions opened by Fxopen()
 */

#ifdef SUPPL_USE_EXTENDED_ATTRIBUTES
#define Fopen Fxopen
#define Fclose Fxclose
#define Eopen Exopen
#define Eclose Exclose
#else
#define Fopen Fyopen
#define Fclose Fyclose
#endif

#endif
