/****************************************************************/
/*								*/
/*	ic_input.c						*/
/*								*/
/*		Character input routines for the		*/
/*		"Integer Calculator".				*/
/*								*/
/****************************************************************/
/*  origination          1988-Apr-7         Terrence W. Holm	*/
/*  added cmd line args  1988-May-13	    Terrence W. Holm	*/
/****************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <sgtty.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "ic.h"

#ifndef NO_TERM
static struct sgttyb saved_mode;
static struct tchars saved_chars;
#endif

/****************************************************************/
/*								*/
/*	Save_Term()						*/
/*								*/
/*		Save the current terminal characteristics.	*/
/*								*/
/****************************************************************/
void Save_Term()
{
#ifndef NO_TERM
  ioctl(0, TIOCGETP, &saved_mode);
  ioctl(0, TIOCGETC, (struct sgttyb *) & saved_chars);
#endif
}

/****************************************************************/
/*								*/
/*	Set_Term()						*/
/*								*/
/*		Set up the terminal characteristics for ic.	*/
/*								*/
/****************************************************************/
void Set_Term()
{
#ifndef NO_TERM
  struct sgttyb ic_mode;
  struct tchars ic_chars;

  ic_mode = saved_mode;
  ic_chars = saved_chars;

  /* No tab expansion, no echo, cbreak mode			 */
  ic_mode.sg_flags = (ic_mode.sg_flags & ~XTABS & ~ECHO) | CBREAK;

  /* Change the interrupt character to ^C, ignore ^S & ^Q 	 */
  ic_chars.t_intrc = '\003';
  ic_chars.t_start = '\377';
  ic_chars.t_stop  = '\377';

  ioctl(0, TIOCSETP, &ic_mode);
  ioctl(0, TIOCSETC, (struct sgttyb *) & ic_chars);
#endif
}

/****************************************************************/
/*								*/
/*	Reset_Term()						*/
/*								*/
/*		Restore the terminal characteristics.		*/
/*								*/
/****************************************************************/
void Reset_Term()
{
#ifndef NO_TERM
  ioctl(0, TIOCSETP, &saved_mode);
  ioctl(0, TIOCSETC, (struct sgttyb *) & saved_chars);
#endif
}

/****************************************************************/
/*								*/
/*	Get_Char()						*/
/*								*/
/*		Return the next input character. Upper case	*/
/*		is mapped to lower case. Escape sequences	*/
/*		are mapped to special codes (msb set).		*/
/*								*/
/****************************************************************/
int Get_Char()
{
  int c;

  /* Fflush() used because Minix does not automatically	flush the output. */
  fflush(stdout);
#if defined(MSX_UZIX_TARGET) || defined(PC_UZIX_TARGET)
/*  fflush(stdin);*/
#endif
  if ((c = Getc()) == EOF) {
  	return(EOF);
  }
  c &= 0x7f;
  if (isupper(c)) {
  	return(tolower(c));
  }

#ifdef PC
  if (c == ESCAPE)
	if ((c = Getc()) != '[') {
		ungetc(c, stdin);
		return(ESCAPE);
	} else {
		c = Getc() | 0x80;
#else
  if ((c<32 || c>127) && c!='\n' && c!='\b') {
#endif

		if (c == ESC_HOME || c == ESC_UP || c == ESC_PGUP ||
		    c == ESC_LEFT || c == ESC_5 || c == ESC_RIGHT ||
		    c == ESC_END || c == ESC_DOWN || c == ESC_PGDN ||
		    c == ESC_PLUS || c == ESC_MINUS)
			return(c);
		else
			return(ESCAPE);
	}

  return(c);
}

/****************************************************************/
/*								*/
/*	Init_Getc( argc, argv )					*/
/*								*/
/*		Give Getc() references to the command line	*/
/*		arguments.					*/
/*								*/
/****************************************************************/
static int args_remaining;
static char **args_pointer;
static int args_index;

void Init_Getc(argc, argv)
int argc;
char *argv[];
{
  args_remaining = argc - 1;
  args_pointer = &argv[1];
  args_index = 0;
}

/****************************************************************/
/*								*/
/*	Getc()							*/
/*								*/
/*		Get the next input character from the command	*/
/*		line if there is some more, else from stdin.	*/
/*								*/
/****************************************************************/
int Getc()
{
#if defined(MSX_UZIX_TARGET) || defined(PC_UZIX_TARGET)
  char cc;
#endif
  int c;

  if (args_remaining > 0)
	if ((c = args_pointer[0][args_index++]) == '\0') {
		--args_remaining;
		++args_pointer;
		args_index = 0;

		if (args_remaining > 0) return('\n');
	} else
		return(c);
#if defined(MSX_UZIX_TARGET) || defined(PC_UZIX_TARGET)
  ioctl(fileno(stdin), TTY_RAW);
  for (;;) if (read(fileno(stdin), &cc, 1) == 1) break;
  ioctl(fileno(stdin), TTY_COOKED);
  c = cc;  
#else
  c=getchar();
#endif
  if (c == '\r') c = '\n';
  return c;
}

/****************************************************************/
/*								*/
/*	Get_Base( character )					*/
/*								*/
/*		Return an appropriate base number for the	*/
/*		given character code. Used by 'i' and 'o'.	*/
/*								*/
/****************************************************************/
int Get_Base(acode)
char acode;
{
  switch (acode) {
      case 'h':
	return(HEXADECIMAL);

      case 'd':
	return(DECIMAL);

      case 'o':
	return(OCTAL);

      case 'b':
	return(BINARY);

      case 'a':
	return(ASCII);

      default:	return(ERROR);
  }
}
