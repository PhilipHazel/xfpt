/*************************************************
*     xfpt - Simple ASCII->Docbook processor     *
*************************************************/

/* Copyright (c) University of Cambridge, 2012 */
/* Written by Philip Hazel, started in 2006 */

/* Header file for all the global variables */


/*************************************************
*           General global variables             *
*************************************************/

extern uschar     *xfpt_share;
extern uschar     *xfpt_version;

extern tree_node  *entities;

extern flagstr    *flaglist;
extern int         from_type[];
extern int         from_type_ptr;

extern uschar     *inbuffer;
extern istackstr  *istack;
extern istackstr  *istackbase;

extern int         literal_state;

extern int         nest_level;
extern int         nest_literal_stack[];
extern uschar     *next_line;

extern macroexe   *macrocurrent;
extern macrodef   *macrolist;

extern argstr     *macro_argbase;
extern argstr     *macro_starteach;

extern FILE       *outfile;

extern int         para_inline_macro;
extern uschar     *parabuffer;
extern int         popto;
extern pushstr    *pushed;

extern int         return_code;
extern uschar     *revision;

extern BOOL        suppress_warnings;

/* End of globals.h */
