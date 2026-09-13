typedef union {
        BOOL b;
	id obj;
        TYOpType op;
} YYSTYPE;
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


extern YYSTYPE yylval;
