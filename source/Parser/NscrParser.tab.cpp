
/*  A Bison parser, made from NscrParser.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	DIGIT	257
#define	MOD	258
#define	ALNUM_	259
#define	BLANK_LINE	260
#define	COMMAND	261
#define	CONST_STR	262
#define	CMD_IF	263
#define	OP_CMP	264
#define	OP_AND	265
#define	FUNC_FCHK	266
#define	FUNC_LCHK	267
#define	PRINT	268
#define	SEL_CMD	269
#define	CR_LF	270
#define	UMINUS	271

#line 1 "NscrParser.y"

#import <Foundation/Foundation.h>
#import "TYScriptEngine.h"

#import "TYIntVarArgment.h"
#import "TYStringVarArgment.h"
#import "TYArrayVarArgment.h"
#import "TYFchkStringArgment.h"

NSString * const OUTPUT_FORMAT = @"%@";

static NSMutableArray *arrayIfNotArray(id);
static NSMutableArray *arrayWithAddObject(id,id);
static NSMutableArray *arrayOfForCmd(id,TYOpType,id,id,id,id,id);

extern int TYParserReadLine;
extern void TYParserFixPredictLine(void);
extern void TYParserBEGIN_INITIAL(void);

#line 21 "NscrParser.y"
typedef union {
        BOOL b;
	id obj;
        TYOpType op;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		95
#define	YYFLAG		-32768
#define	YYNTBASE	30

#define YYTRANSLATE(x) ((unsigned)(x) <= 271 ? yytranslate[x] : 48)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,    22,    23,     2,     2,    25,
    26,    19,    18,    24,    17,     2,    20,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    27,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    28,     2,    29,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    21
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     6,     8,    10,    13,    15,    17,    19,
    22,    24,    27,    30,    32,    35,    37,    40,    42,    44,
    49,    52,    53,    56,    58,    62,    66,    69,    72,    79,
    88,    90,    94,    99,   101,   103,   106,   108,   114,   116,
   120,   124,   128,   132,   136,   139,   143,   145,   147,   149,
   152,   156,   160
};

static const short yyrhs[] = {    31,
     0,    31,     1,     0,     0,    34,     0,    36,     0,    38,
    34,     0,    38,     0,    32,     0,    33,     0,    32,    33,
     0,    14,     0,    22,    46,     0,    23,    46,     0,     5,
     0,    35,    42,     0,    35,     0,    35,    41,     0,     7,
     0,     5,     0,    15,    37,    42,    16,     0,    16,    37,
     0,     0,     9,    39,     0,    40,     0,    39,    11,    40,
     0,    43,    10,    43,     0,    12,    43,     0,    13,    43,
     0,    23,    46,    10,    45,     5,    45,     0,    23,    46,
    10,    45,     5,    45,     5,    45,     0,    43,     0,    42,
    24,    43,     0,    42,    24,    16,    43,     0,    45,     0,
    44,     0,    22,    46,     0,     8,     0,    25,    44,    26,
    44,    44,     0,     5,     0,    45,    18,    45,     0,    45,
    17,    45,     0,    45,    19,    45,     0,    45,    20,    45,
     0,    45,     4,    45,     0,    17,    45,     0,    25,    45,
    26,     0,    46,     0,     3,     0,     5,     0,    23,    46,
     0,    27,    46,    47,     0,    28,    45,    29,     0,    47,
    47,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    63,    64,    70,    73,    74,    75,    76,    77,    80,    81,
    84,    85,    86,    87,    91,    92,    93,   106,   107,   110,
   113,   114,   117,   120,   121,   124,   125,   126,   129,   137,
   148,   149,   150,   153,   154,   157,   158,   159,   164,   167,
   168,   169,   170,   171,   172,   173,   174,   177,   178,   179,
   180,   183,   184
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","DIGIT",
"MOD","ALNUM_","BLANK_LINE","COMMAND","CONST_STR","CMD_IF","OP_CMP","OP_AND",
"FUNC_FCHK","FUNC_LCHK","PRINT","SEL_CMD","CR_LF","'-'","'+'","'*'","'/'","UMINUS",
"'$'","'%'","','","'('","')'","'?'","'['","']'","statement","exec_statement",
"print_statement","print_chunk","cmd_statement","cmd","selcmd_statement","pre_blank",
"if_statement","cond_statements","cond_state","for_argments","argments","arg",
"string","expression","number","array_index", NULL
};
#endif

static const short yyr1[] = {     0,
    30,    30,    30,    31,    31,    31,    31,    31,    32,    32,
    33,    33,    33,    33,    34,    34,    34,    35,    35,    36,
    37,    37,    38,    39,    39,    40,    40,    40,    41,    41,
    42,    42,    42,    43,    43,    44,    44,    44,    44,    45,
    45,    45,    45,    45,    45,    45,    45,    46,    46,    46,
    46,    47,    47
};

static const short yyr2[] = {     0,
     1,     2,     0,     1,     1,     2,     1,     1,     1,     2,
     1,     2,     2,     1,     2,     1,     2,     1,     1,     4,
     2,     0,     2,     1,     3,     3,     2,     2,     6,     8,
     1,     3,     4,     1,     1,     2,     1,     5,     1,     3,
     3,     3,     3,     3,     2,     3,     1,     1,     1,     2,
     3,     3,     2
};

static const short yydefact[] = {     3,
    14,    18,     0,    11,    22,     0,     0,     0,     8,     9,
     4,    16,     5,     7,    48,    39,    37,     0,     0,     0,
     0,     0,     0,     0,    23,    24,     0,    35,    34,    47,
    22,     0,    49,    12,    13,     2,    14,    10,     0,    17,
    15,    31,    19,     6,    27,    28,     0,    45,    36,    50,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    21,     0,    50,     0,     0,    46,     0,    51,    25,    26,
    44,    41,    40,    42,    43,    20,     0,     0,    32,    39,
     0,     0,     0,    53,     0,    33,    38,    52,     0,    29,
     0,    30,     0,     0,     0
};

static const short yydefgoto[] = {    93,
     8,     9,    10,    11,    12,    13,    32,    14,    25,    26,
    40,    41,    27,    28,    29,    30,    84
};

static const short yypact[] = {   130,
   103,-32768,    53,-32768,    26,   102,   102,    54,     1,-32768,
-32768,    76,-32768,     7,-32768,    31,-32768,    92,    92,    85,
   102,   102,    92,   102,     2,-32768,    10,-32768,   146,-32768,
    26,    92,-32768,-32768,-32768,-32768,-32768,-32768,   102,-32768,
    21,-32768,-32768,-32768,-32768,-32768,    85,-32768,-32768,-32768,
    33,   114,    24,    53,    92,    85,    85,    85,    85,    85,
-32768,    -7,    50,    69,    -3,-32768,    85,    24,-32768,-32768,
-32768,    14,    14,-32768,-32768,-32768,    85,    92,-32768,-32768,
    -3,    -3,    12,    24,   137,-32768,-32768,-32768,    85,   142,
    85,   146,    64,    68,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    73,    59,-32768,-32768,    52,-32768,-32768,    35,
-32768,    55,   -11,   -19,   -20,     4,    51
};


#define	YYLAST		166


static const short yytable[] = {    48,
    42,    80,    52,    51,    17,    37,    45,    46,    76,    34,
    35,    43,    54,     2,     4,    56,    64,    56,    21,    55,
    42,    81,     6,     7,    49,    50,    52,    53,    57,    58,
    59,    60,    59,    60,   -49,    71,    72,    73,    74,    75,
    88,    31,    63,    70,    64,    82,    83,   -49,   -49,   -49,
   -49,    67,    79,    -1,    36,    15,    85,    16,    65,    77,
    17,    51,    87,    94,    18,    19,    86,    95,    90,    20,
    92,    15,    44,    16,    21,    22,    17,    23,    15,    24,
    16,    38,    61,    17,    78,    20,    62,    15,    69,    33,
    21,    22,    20,    23,    15,    24,    16,    21,    39,    17,
    23,    20,    24,    68,    15,   -19,    33,    22,    20,    47,
   -19,    24,     0,    21,    22,     0,    23,    56,    24,   -19,
     0,     0,     0,     0,    22,     0,     0,   -19,    24,   -19,
    57,    58,    59,    60,     1,     0,     2,     0,     3,    66,
    56,    89,     0,     4,     5,    56,    91,     0,     0,    56,
     0,     6,     7,    57,    58,    59,    60,     0,    57,    58,
    59,    60,    57,    58,    59,    60
};

static const short yycheck[] = {    20,
    12,     5,    23,    23,     8,     5,    18,    19,    16,     6,
     7,     5,    11,     7,    14,     4,    24,     4,    22,    10,
    32,    25,    22,    23,    21,    22,    47,    24,    17,    18,
    19,    20,    19,    20,     4,    56,    57,    58,    59,    60,
    29,    16,    39,    55,    24,    65,    67,    17,    18,    19,
    20,    28,    64,     0,     1,     3,    77,     5,    26,    10,
     8,    81,    82,     0,    12,    13,    78,     0,    89,    17,
    91,     3,    14,     5,    22,    23,     8,    25,     3,    27,
     5,     9,    31,     8,    16,    17,    32,     3,    54,     5,
    22,    23,    17,    25,     3,    27,     5,    22,    23,     8,
    25,    17,    27,    53,     3,     3,     5,    23,    17,    25,
     8,    27,    -1,    22,    23,    -1,    25,     4,    27,    17,
    -1,    -1,    -1,    -1,    23,    -1,    -1,    25,    27,    27,
    17,    18,    19,    20,     5,    -1,     7,    -1,     9,    26,
     4,     5,    -1,    14,    15,     4,     5,    -1,    -1,     4,
    -1,    22,    23,    17,    18,    19,    20,    -1,    17,    18,
    19,    20,    17,    18,    19,    20
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 63 "NscrParser.y"
{ if(yyvsp[0].obj) [ [ TYScriptEngine sharedEngine ] eval:yyvsp[0].obj ]; ;
    break;}
case 2:
#line 64 "NscrParser.y"
{ 
                if(yyvsp[-1].obj != nil) {
                    [ [ TYScriptEngine sharedEngine ] incRunColumn ];
                    [ [ TYScriptEngine sharedEngine ] eval:yyvsp[-1].obj ]; 
                }
            ;
    break;}
case 4:
#line 73 "NscrParser.y"
{ yyval.obj = yyvsp[0].obj; ;
    break;}
case 5:
#line 74 "NscrParser.y"
{ TYParserFixPredictLine(); [ [ TYScriptEngine sharedEngine ] eval:yyvsp[0].obj ];  YYACCEPT; ;
    break;}
case 6:
#line 75 "NscrParser.y"
{ yyval.obj = yyvsp[0].obj; ;
    break;}
case 7:
#line 76 "NscrParser.y"
{ yyval.obj = nil; ;
    break;}
case 8:
#line 77 "NscrParser.y"
{ yyval.obj = [ NSMutableArray arrayWithObjects:CFSTR("_print"),yyvsp[0].obj,nil ]; ;
    break;}
case 10:
#line 81 "NscrParser.y"
{ yyval.obj = [ yyvsp[-1].obj stringByAppendingString:yyvsp[0].obj ]; ;
    break;}
case 12:
#line 85 "NscrParser.y"
{ yyval.obj = [ [ TYStringVarArgment argmentWithVarID:yyvsp[0].obj ] stringObj ]; ;
    break;}
case 13:
#line 86 "NscrParser.y"
{ yyval.obj = [ [ [ TYIntVarArgment argmentWithVarID:yyvsp[0].obj ] intNumber ] stringValue ]; ;
    break;}
case 15:
#line 91 "NscrParser.y"
{ [ yyvsp[0].obj insertObject:yyvsp[-1].obj atIndex:0 ]; yyval.obj = yyvsp[0].obj; ;
    break;}
case 16:
#line 92 "NscrParser.y"
{ yyval.obj = arrayIfNotArray(yyvsp[0].obj); ;
    break;}
case 17:
#line 93 "NscrParser.y"
{ 
                if(![ yyvsp[-1].obj isEqualToString:CFSTR("for") ]) {
                    YYERROR;
                } else {
                    [ [ TYScriptEngine sharedEngine ] registLoopTargetID:[ [ yyvsp[0].obj objectAtIndex:0 ] varID ]
                                                               initValue:[ yyvsp[0].obj objectAtIndex:1 ]
                                                              limitValue:[ yyvsp[0].obj objectAtIndex:2 ]
                                                                    step:[ yyvsp[0].obj objectAtIndex:3 ] ]; 
                    YYACCEPT;
                }
            ;
    break;}
case 20:
#line 110 "NscrParser.y"
{ [ yyvsp[-1].obj insertObject:yyvsp[-3].obj atIndex:0 ];  TYParserReadLine++; yyval.obj = yyvsp[-1].obj; ;
    break;}
case 23:
#line 117 "NscrParser.y"
{ if(yyvsp[-1].b != yyvsp[0].b) { [ [ TYScriptEngine sharedEngine ] resetRunColumn ]; YYACCEPT; } ;
    break;}
case 25:
#line 121 "NscrParser.y"
{ if(yyvsp[-2].b != yyvsp[0].b) { [ [ TYScriptEngine sharedEngine ] resetRunColumn ]; YYACCEPT; } else yyval.b = yyvsp[-2].b; ;
    break;}
case 26:
#line 124 "NscrParser.y"
{ yyval.b = TYCompareArgment(yyvsp[-2].obj,yyvsp[0].obj,yyvsp[-1].op); ;
    break;}
case 27:
#line 125 "NscrParser.y"
{ yyval.b = [ [ TYScriptEngine sharedEngine ] judgefchk:yyvsp[0].obj ]; ;
    break;}
case 28:
#line 126 "NscrParser.y"
{ yyval.b = [ [ TYScriptEngine sharedEngine ] judgelchk:yyvsp[0].obj ]; ;
    break;}
case 29:
#line 129 "NscrParser.y"
{
                yyval.obj = arrayOfForCmd([ TYIntVarArgment argmentWithVarID:yyvsp[-4].obj ],
                                   yyvsp[-3].op,
                                   yyvsp[-2].obj,
                                   yyvsp[-1].obj,
                                   yyvsp[0].obj,
                                   nil,nil);
            ;
    break;}
case 30:
#line 137 "NscrParser.y"
{
                yyval.obj = arrayOfForCmd([ TYIntVarArgment argmentWithVarID:yyvsp[-6].obj ],
                                   yyvsp[-5].op,
                                   yyvsp[-4].obj,
                                   yyvsp[-3].obj,
                                   yyvsp[-2].obj,
                                   yyvsp[-1].obj,
                                   yyvsp[0].obj);
            ;
    break;}
case 31:
#line 148 "NscrParser.y"
{ yyval.obj = arrayIfNotArray(yyvsp[0].obj); ;
    break;}
case 32:
#line 149 "NscrParser.y"
{ yyval.obj = arrayWithAddObject(yyvsp[-2].obj,yyvsp[0].obj); ;
    break;}
case 33:
#line 150 "NscrParser.y"
{ yyval.obj = arrayWithAddObject(yyvsp[-3].obj,yyvsp[0].obj); TYParserReadLine++; ;
    break;}
case 36:
#line 157 "NscrParser.y"
{ yyval.obj = [ TYStringVarArgment argmentWithVarID:yyvsp[0].obj ]; ;
    break;}
case 38:
#line 159 "NscrParser.y"
{ 
            yyval.obj = [ TYFchkStringArgment argmentWithFileName:yyvsp[-3].obj
                                                 trueValue:yyvsp[-1].obj
                                                falseValue:yyvsp[0].obj ];
        ;
    break;}
case 40:
#line 167 "NscrParser.y"
{ yyval.obj = [ NSNumber numberWithInt:[ yyvsp[-2].obj intValue ] + [ yyvsp[0].obj intValue ] ]; ;
    break;}
case 41:
#line 168 "NscrParser.y"
{ yyval.obj = [ NSNumber numberWithInt:[ yyvsp[-2].obj intValue ] - [ yyvsp[0].obj intValue ] ]; ;
    break;}
case 42:
#line 169 "NscrParser.y"
{ yyval.obj = [ NSNumber numberWithInt:[ yyvsp[-2].obj intValue ] * [ yyvsp[0].obj intValue ] ]; ;
    break;}
case 43:
#line 170 "NscrParser.y"
{ yyval.obj = [ NSNumber numberWithInt:[ yyvsp[-2].obj intValue ] / [ yyvsp[0].obj intValue ] ]; ;
    break;}
case 44:
#line 171 "NscrParser.y"
{ yyval.obj = [ NSNumber numberWithInt:[ yyvsp[-2].obj intValue ] % [ yyvsp[0].obj intValue ] ]; ;
    break;}
case 45:
#line 172 "NscrParser.y"
{ yyval.obj = [ NSNumber numberWithInt:[ yyvsp[0].obj intValue ] * -1 ]; ;
    break;}
case 46:
#line 173 "NscrParser.y"
{ yyval.obj = yyvsp[-1].obj; ;
    break;}
case 47:
#line 174 "NscrParser.y"
{ yyval.obj = yyvsp[0].obj ; ;
    break;}
case 48:
#line 177 "NscrParser.y"
{ yyval.obj = yyvsp[0].obj; ;
    break;}
case 49:
#line 178 "NscrParser.y"
{ yyval.obj = [ yyvsp[0].obj intNumber ]; ;
    break;}
case 50:
#line 179 "NscrParser.y"
{ yyval.obj = [ TYIntVarArgment argmentWithVarID:yyvsp[0].obj ]; ;
    break;}
case 51:
#line 180 "NscrParser.y"
{ yyval.obj = [ TYArrayVarArgment argmentWithVarID:yyvsp[-1].obj arrayIndex:yyvsp[0].obj ]; ;
    break;}
case 52:
#line 183 "NscrParser.y"
{ yyval.obj = arrayIfNotArray(yyvsp[-1].obj); ;
    break;}
case 53:
#line 184 "NscrParser.y"
{ yyval.obj = arrayWithAddObject(yyvsp[-1].obj,yyvsp[0].obj); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 186 "NscrParser.y"


/*
int main(void)
{
	NSAutoreleasePool *pool = [ [ NSAutoreleasePool alloc ] init ];
	
	yyparse();
	
	[ pool release ];
}
*/

/*
int yyerror(const char* msg)
{
    [ NSException raise:@"TYParseErrorException" format:[ NSString stringWithCString:msg ] ];
    
    return 0;
}
*/

static NSMutableArray *arrayIfNotArray(id receiver)
{
    if(![ receiver respondsToSelector:@selector(addObject:)]){
        return [ NSMutableArray arrayWithObject:receiver ];
    } else {
        return receiver;
    }
}

static NSMutableArray *arrayWithAddObject(id receiver,id aObject)
{    
    if(![ receiver respondsToSelector:@selector(addObject:)]){
        receiver = [ NSMutableArray arrayWithObject:receiver ];
    }
    
    if([ aObject respondsToSelector:@selector(addObject:)]){
        [ receiver addObjectsFromArray:aObject ];
    } else {
        [ receiver addObject:aObject ];
    }
    
    return receiver;
}

static NSMutableArray *arrayOfForCmd(id bindVar,
                                     TYOpType op,
                                     id start,
                                     id to_token,
                                     id end,
                                     id step_token,
                                     id step)
{
    if(op != TYEqualOpType){
        return nil;
    }
    if(![ to_token isEqualToString:@"to" ])
        return nil;
    if(step_token && ![ step_token isEqualToString:@"step" ])
        return nil;
    
    if(!step)
        step = [ NSNumber numberWithInt:1 ];
        
    return [ NSMutableArray arrayWithObjects:bindVar,start,end,step,nil ];
}




