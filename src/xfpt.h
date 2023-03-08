/*************************************************
*     xfpt - Simple ASCII->Docbook processor     *
*************************************************/

/* Copyright (c) University of Cambridge, 2023 */
/* Written by Philip Hazel, started in 2006 */

/* This is the header file for all code modules. */

#ifndef INCLUDED_xfpt_H
#define INCLUDED_xfpt_H

/* C headers */

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* Local headers */

#include "mytypes.h"
#include "structs.h"
#include "globals.h"
#include "functions.h"

/* Manifest values and enumerations */

#define INBUFFSIZE          1024
#define PARABUFFSIZE       10000
#define FLAGSTACKSIZE         40
#define MAXNEST                3
#define FROM_TYPE_STACKSIZE   20

/* Type of current input */

enum { FROM_FILE, FROM_MACRO };

/* Nested block indicators for read_paragraph() */

enum { NEST_NO, NEST_BEGIN, NEST_END };

/* The literal states */

enum { LITERAL_OFF, LITERAL_LAYOUT, LITERAL_TEXT, LITERAL_XML };

#endif   /* INCLUDED_xfpt_H */

/* End of xfpt.h */
