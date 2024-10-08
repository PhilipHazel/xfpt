/*************************************************
*     xfpt - Simple ASCII->Docbook processor     *
*************************************************/

/* Copyright (c) University of Cambridge, 2024 */
/* Written by Philip Hazel, started in 2006 */

/* This module contains code for reading the input. */

#include "xfpt.h"



/*************************************************
*             Process macro line                 *
*************************************************/

/* This is the place where macro arguments are substituted. In a section
delimited by .eacharg/.endeach, the variable macro_argbase is set to the first
of the relative arguments. This function is also called from para.c in order to
handle inline macro calls. There will always be a zero at the end of the input
line, but we must allow for zeroes inside the line, so this cannot be used to
detect end of line.

Arguments:
  p         the macro input line
  b         where to put the result

Returns:    nothing
*/

void
read_process_macroline(uschar *p, uschar *b, int blen)
{
int optend = 0;

while (*p != 0)
  {
  int argn = 0;
  argstr *argbase, *arg;

  /* If we are including an optional substring, when we get to the terminator,
  just skip it. */

  if (*p == optend)
    {
    optend = 0;
    p++;
    continue;
    }

  /* Ensure at least 3 bytes left in the buffer because all cases except an
  argument substitution (which does its own test) add no more than two bytes,
  and the third is for the terminating zero. */

  if (blen < 3) error(33);   /* Hard error; does not return. */

  /* Until we hit a dollar, just copy verbatim */

  if (*p != '$') { *b++ = *p++; blen--; continue; }

  /* If dollar is at the end of the string, treat as literal. */

  if (p[1] == 0) { *b++ = '$'; break; }

  /* If the character after $ is another $, insert a literal $. */

  if (p[1] == '$') { p++; *b++ = *p++; blen--; continue; }

  /* If the character after $ is +, we are dealing with arguments
  relative to macro_arg0 in a ".eacharg" section. Otherwise, we are dealing
  with an absolute argument number. */

  if (p[1] == '+')
    {
    p++;
    if (macro_argbase == NULL)       /* Not in a .eacharg section */
      {
      error(18);
      *b++ = '$';
      *b++ = *p++;
      blen -= 2;
      continue;
      }
    argbase = macro_argbase;
    }
  else argbase = macrocurrent->args;

  /* $= introduces an optional substring */

  if (p[1] == '=')
    {
    p++;
    if (!isdigit(p[1]))
      {
      if (p[1] == 0 || p[1] == '\n') error(30, "end of line", "$=");
        else error(17, p[1], "$=");
      *b++ = '$';
      *b++ = *p++;
      blen -= 2;
      continue;
      }
    while (isdigit(*(++p))) argn = argn * 10 + *p - '0';

    if (*p == 0 || *p == '\n')
      {
      error(29, argn);
      break;
      }

    /* Pick up the delimiter and check for the argument. */

    optend = *p++;
    arg = argbase;
    for (int i = 1; i < argn; i++)
      {
      if (arg == NULL) break;
      arg = arg->next;
      }

    if (arg == NULL || arg->string[0] == 0)
      {
      while (*p != 0 && *p != optend) p++;
      if (*p != 0) p++;
      }

    continue;
    }

  /* Not '=' after $; this is an argument substitution */

  if (!isdigit(p[1]))
    {
    if (*p == 0 || *p == '\n') error(30, "end of line", "$");
      else error(17, p[1], "$");
    *b++ = *p++;
    blen--;
    continue;
    }
  while (isdigit(*(++p))) argn = argn * 10 + *p - '0';

  /* Handle $0 - currently no meaning */

  if (argn == 0)
    {
    continue;
    }

  /* Seek an argument in this invocation */

  arg = argbase;
  for (int i = 1; i < argn; i++)
    {
    if (arg == NULL) break;
    arg = arg->next;
    }

  /* If not found, seek a default argument for an absolute substitution, but
  not for a relative one. */

  if (arg == NULL && argbase == macrocurrent->args)
    {
    arg = macrocurrent->macro->args;
    for (int i = 1; i < argn; i++)
      {
      if (arg == NULL) break;
      arg = arg->next;
      }
    }

  /* If we have found an argument, substitute it. */

  if (arg != NULL)
    {
    blen -= arg->length;
    if (blen < 1) error(33);  /* Hard; does not return */
    b += sprintf(CS b, "%s", arg->string);
    }
  }

*b = 0;
}



/*************************************************
*          Read a line from a file               *
*************************************************/

/* Input is read by fgetc() rather than fgets() so that we can detect and
complain about binary zeroes, which have been thrown at xfpt by fuzzers.

Arguments:
  f           the input file
  buffer      where to read into
  blen        length of buffer

Returns:      number of chars read or -1 at EOF
*/

static int
get_nextline(FILE *f, uschar *buffer, int blen)
{
uschar *p = buffer;
uschar *pend = buffer + blen;

for (;;)
  {
  int c;
  if (p >= pend - 1) break;
  c = fgetc(f);
  if (c == EOF) return -1;
  if (c == 0)
    {
    error(31);
    continue;
    }
  *p++ = c;
  if (c == '\n') break;
  }
*p = 0;
return (int)(p - buffer);
}



/*************************************************
*         Get the next line of input             *
*************************************************/

/* There may be a saved line already in the buffer, following the reading of a
paragraph or a .nonl directive. Otherwise, take the next line from one of three
sources:

  (1) If popto is not negative, get an appropropriate line off the stack.
  (2) If we are in a macro, get the next macro line.
  (3) If we are in a file, read a new line from a file and handle any
      continuations.

There can be arbitrary nesting of macros and files, because a .include
directive may appear inside a macro. The current from_type vector is used to
keep track of what is current.

Arguments:  none
Returns:    pointer to the next line or NULL
*/

uschar *
read_nextline(void)
{
int len;
uschar *p, *q;

/* Handle a dot line that terminated a paragraph, or a .nonl line */

if (next_line != NULL)
  {
  uschar *yield = next_line;
  next_line = NULL;
  return yield;
  }

/* Handle a line off the stack */

if (popto == 0)
  {
  pushstr *ps = pushed;
  if (ps == NULL) error(12); else
    {
    popto = -1;
    (void)sprintf(CS inbuffer, "%s\n", ps->string);
    pushed = ps->next;
    free(ps);
    return inbuffer;
    }
  }

/* Handle a line off the stack when there is a matching line at the top or
below for the given letter. When we reach the matching line, stop popping. The
value of popto is set greater than zero only when it is known that there's a
matching line. */

if (popto > 0)
  {
  pushstr *ps = pushed;
  if (ps->letter == popto) popto = -1;  /* Last one */
  (void)sprintf(CS inbuffer, "%s\n", ps->string);
  pushed = ps->next;
  free(ps);
  return inbuffer;
  }

/* Get the next line from the current macro or the current file. We need a loop
for handling the ends of macros and files. First check for having previously
reached the end of the input. */

if (from_type_ptr < 0) return NULL;

for (;;)
  {
  if (from_type[from_type_ptr] == FROM_MACRO)
    {
    if (macrocurrent->nextline == NULL)
      {
      macroexe *temp = macrocurrent;
      macrocurrent = macrocurrent->prev;
      macro_free(temp);
      }
    else
      {
      read_process_macroline(macrocurrent->nextline->string, inbuffer,
        INBUFFSIZE);
      macrocurrent->nextline = macrocurrent->nextline->next;
      break;
      }
    }

  /* When reading from a file, handle continuation lines, but only within the
  single file. */

  else
    {
    len = get_nextline(istack->file, inbuffer, INBUFFSIZE);
    if (len < 0)
      {
      istackstr *prev = istack->prev;
      fclose(istack->file);
      free(istack);
      istack = prev;
      }
    else
      {
      istack->linenumber++;
      q = inbuffer;

      for (;;)
        {
        p = q + len;
        while (p > q && isspace(p[-1])) p--;

        if (p - q < 3 || Ustrncmp(p - 3, "&&&", 3) != 0) break;

        q = p - 3;
        *q = 0;

        if (istack == NULL) break;
        len = get_nextline(istack->file, q, INBUFFSIZE - (q - inbuffer));
        if (len < 0) break;

        istack->linenumber++;
        p = q;
        while (*p == ' ' || *p == '\t') { p++; len--; }
        if (p > q) memmove(q, p, len + 1);
        }

      break;
      }
    }

  /* We get here if the end of a macro or a file was reached. The appropriate
  chain has been popped. Back up the stack of input types before the loop
  repeats. When we reach the end of the stack, we have reached the end of all
  the input. */

  if (--from_type_ptr < 0) return NULL;
  }

return inbuffer;
}



/*************************************************
*        Complete the reading of a paragraph     *
*************************************************/

/* This function is called after a line has been identified as the start of a
paragraph. We need to read the rest so that flags can be matched across the
entire paragraph. (If there is nested material such as a footnote, this applies
only to the separate parts, not across the nesting.) The text is copied into
the paragraph buffer. Directives that are encountered in the paragraph are
processed, with two exceptions.

(1) For .literal, we set next_line so it is processed next, and exit. This is
the end of the paragraph.

(2) For .nest, we set *nest_info, according to whether it is the start or
end of a nested section, and exit.

Arguments:
  p           the first line
  nest_info   returns NEST_NO, NEST_START, or NEST_END

Returns:      the paragraph
*/

uschar *
read_paragraph(uschar *p, int *nest_info)
{
uschar *q = parabuffer;
int length = Ustrlen(p);

memcpy(q, p, length);
q += length;

*nest_info = NEST_NO;    /* Not hit .nest */

for (;;)
  {
  uschar *s;

  if ((p = read_nextline()) == NULL) break;

  if (Ustrncmp(p, ".literal ", 9) == 0)
    {
    next_line = p;
    break;
    }

  if (Ustrncmp(p, ".nest ", 6) == 0)
    {
    p += 6;
    while (isspace(*p)) p++;
    s = p + Ustrlen(p);
    while (s > p && isspace(s[-1])) s--;
    *s = 0;
    if (Ustrcmp(p, "begin") == 0) *nest_info = NEST_BEGIN;
    else if (Ustrcmp(p, "end") == 0) *nest_info = NEST_END;
    else error(26, p);
    break;
    }

  else if (*p == '.')
    {
    dot_process(p);
    continue;
    }

  /* End paragraph on encountering a completely blank line */

  for (s = p;  *s == ' ' || *s == '\t'; s++);
  if (*s == '\n') break;

  length = Ustrlen(p);
  memcpy(q, p, length);
  q += length;
  }

*q = 0;
return parabuffer;
}

/* End of read.c */
