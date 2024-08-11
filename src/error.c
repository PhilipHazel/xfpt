/*************************************************
*     xfpt - Simple ASCII->Docbook processor     *
*************************************************/

/* Copyright (c) University of Cambridge, 2024 */
/* Written by Philip Hazel, started in 2006 */

/* Error handling routines */

#include "xfpt.h"


/* Error codes */

#define ec_noerror   0
#define ec_warning   1
#define ec_serious   2
#define ec_failed    3
#define ec_disaster  4



/*************************************************
*             Static variables                   *
*************************************************/

static int  error_count = 0;
static int  warning_count = 0;



/*************************************************
*            Texts and return codes              *
*************************************************/

typedef struct {
  char ec;
  const char *text;
} error_struct;


static error_struct error_data[] = {

/* 0-4 */
{ ec_disaster, "failed to open %s: %s" },
{ ec_disaster, "malloc failed: requested %d bytes" },
{ ec_serious,  "unknown directive line: %s" },
{ ec_serious,  "missing semicolon after \"&%.*s\"" },
{ ec_serious,  "unexpected character \"%c\" after \"&#\"" },
/* 5-9 */
{ ec_serious,  "\"layout\", \"text\", \"xml\", or \"off\" expected, but \"%s\" found" },
{ ec_serious,  "unknown flag \"&%c\"" },
{ ec_serious,  "missing closing flag %s" },
{ ec_serious,  "flag nesting error: \"%s\" expected before \"%s\"" },
{ ec_serious,  "a flag must begin with \"&\"" },
/* 10-14 */
{ ec_serious,  "a flag must contain more than just \"&\"" },
{ ec_serious,  "malformed directive\n   %s" },
{ ec_serious,  "line stack is empty" },
{ ec_serious,  "missing %s" },
{ ec_serious,  "a macro must be given a name" },
/* 15-19 */
{ ec_serious,  "%s is permitted only inside a macro" },
{ ec_serious,  "unexpected %s" },
{ ec_serious,  "bad macro argument substitution: \"%c\" follows \"%s\"" },
{ ec_serious,  "relative macro argument not in \"eacharg\" section" },
{ ec_warning,  "extra characters at end of directive\n"
               "   %s %s\n   %.*s%.*s" },
/* 20-24 */
{ ec_disaster, "string too long for internal buffer (%d > %d)" },
{ ec_serious,  "entity \"%s\" has already been defined" },
{ ec_serious,  "\"%s\" is not permitted in an inline macro call" },
{ ec_serious,  "unknown macro \"%.*s\" in inline macro call" },
{ ec_serious,  "missing closing parenthesis in inline macro call:\n   %s" },
/* 25-29 */
{ ec_serious,  "ampersand found at end of line or string - ignored" },
{ ec_serious,  "\"begin\" or \"end\" expected, but \"%s\" found" },
{ ec_serious,  "\".nest begin\" too deeply nested" },
{ ec_serious,  "\".nest end\" incorrectly nested" },
{ ec_serious,  "missing delimiter after $=%d at end of line" },
/* 30-34 */
{ ec_serious,  "bad macro argument substitution: %s follows \"%s\"" },
{ ec_serious,  "binary zero in input ignored" },
{ ec_disaster, "input sources too deeply nested" },
{ ec_disaster, "maximum line length exceeded during macro substitution" }
};

#define error_maxerror (int)(sizeof(error_data)/sizeof(error_struct))



/*************************************************
*              Error message generator           *
*************************************************/

/* This function outputs an error or warning message, and may abandon the
run if the error is sufficiently serious, or if there have been too many less
serious errors. If there are too many warnings, subsequent ones are suppressed.

Arguments:
  n           error number
  ...         arguments to fill into message

Returns:      nothing, but some errors do not return
*/

void
error(int n, ...)
{
int ec;
macroexe *me;
istackstr *fe;
va_list ap;
va_start(ap, n);

if (n > error_maxerror)
  {
  /* LCOV_EXCL_START */
  (void)fprintf(stderr, "** Unknown error number %d\n", n);
  ec = ec_disaster;
  /* LCOV_EXCL_STOP */
  }
else
  {
  ec = error_data[n].ec;
  if (ec == ec_warning)
    {
    if (suppress_warnings) return;
    (void)fprintf(stderr, "** Warning: ");
    }
  else if (ec > ec_warning)
    (void)fprintf(stderr, "** Error: ");
  (void)vfprintf(stderr, error_data[n].text, ap);
  (void)fprintf(stderr, "\n");
  }

va_end(ap);

me = macrocurrent;
fe = istack;

if (from_type_ptr < 0) (void)fprintf(stderr, "   Detected near end of file\n");
else for (int i = from_type_ptr; i >= 0; i--)
  {
  if (from_type[i] == FROM_MACRO)
    {
    const char *eof = (i == 0)? " at end of input" : "";
    (void)fprintf(stderr, "   Processing macro %s%s\n", me->macro->name, eof);
    me = me->prev;
    }
  else
    {
    if (fe != NULL)
      {
      if (fe->linenumber > 0)
        (void)fprintf(stderr, "   Detected near line %d of %s\n",
          fe->linenumber, fe->filename);
      fe = fe->prev;
      }
    else
      {
      (void)fprintf(stderr, "   Detected near end of file\n"); /*LCOV_EXCL_LINE */
      }
    }
  }

if (ec == ec_warning)
  {
  warning_count++;
  if (warning_count > 40)
    {
    /* LCOV_EXCL_START */
    (void)fprintf(stderr, "** Too many warnings - subsequent ones suppressed\n");
    suppress_warnings = TRUE;
    /* LCOV_EXCL_STOP */
    }
  }

else if (ec > ec_warning)
  {
  return_code = EXIT_FAILURE;
  error_count++;
  if (error_count > 40)
    {
    /* LCOV_EXCL_START */
    (void)fprintf(stderr, "** Too many errors\n");
    ec = ec_failed;
    /* LCOV_EXCL_STOP */
    }
  }

if (ec >= ec_failed)
  {
  (void)fprintf(stderr, "** xfpt abandoned\n");
  suppress_warnings = TRUE;  /* Avoid unclosed warnings */
  exit(EXIT_FAILURE);
  }

(void)fprintf(stderr, "\n");   /* blank before next output */
}

/* End of error.c */
