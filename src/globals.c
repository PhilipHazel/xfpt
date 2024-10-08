/*************************************************
*     xfpt - Simple ASCII->Docbook processor     *
*************************************************/

/* Copyright (c) University of Cambridge, 2024 */
/* Written by Philip Hazel, started in 2006 */

/* Allocate storage and initialize global variables */

#include "xfpt.h"


uschar    *xfpt_share                = US DATADIR;
uschar    *xfpt_version              = US "1.01-DEV 10-August-2024";

tree_node *entities                  = NULL;

flagstr   *flaglist                  = NULL;
int        from_type[FROM_TYPE_STACKSIZE];
int        from_type_ptr             = 0;

uschar    *inbuffer                  = NULL;
istackstr *istack                    = NULL;
istackstr *istackbase                = NULL;

int        literal_state             = LITERAL_OFF;

int        nest_level                = 0;
int        nest_literal_stack[MAXNEST+1];
uschar    *next_line                 = NULL;

macroexe  *macrocurrent              = NULL;
macrodef  *macrolist                 = NULL;

argstr    *macro_argbase             = NULL;
argstr    *macro_starteach           = NULL;

FILE      *outfile                   = NULL;

int        para_inline_macro         = 0;
uschar    *parabuffer                = NULL;
int        popto                     = -1;
pushstr   *pushed                    = NULL;

int        return_code               = 0;
uschar    *revision                  = NULL;

BOOL       suppress_warnings         = FALSE;

/* End of globals.c */
