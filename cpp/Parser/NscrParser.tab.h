#ifndef NSCR_PARSER_TAB_H
#define NSCR_PARSER_TAB_H

#include <stdbool.h>
#include <stdint.h>

// Type definitions - matching original YYSTYPE
union YYSTYPE {
    bool b;
    void* obj;
    TYOpType op;
};

extern YYSTYPE yylval;

// Token definitions
#define DIGIT       257
#define MOD         258
#define ALNUM_      259
#define BLANK_LINE  260
#define COMMAND     261
#define CONST_STR   262
#define CMD_IF      263
#define OP_CMP      264
#define OP_AND      265
#define FUNC_FCHK   266
#define FUNC_LCHK   267
#define PRINT       268
#define SEL_CMD     269
#define CR_LF       270
#define UMINUS      271

// Forward declarations
class TYScriptEngine;
class TYArgment;
class TYIntVarArgment;
class TYStringVarArgment;
class TYArrayVarArgment;
class TYFchkStringArgment;

class NscrParser {
public:
    NscrParser();
    ~NscrParser();

    int parse();

    // External variables from lexer
    static int TYParserReadLine;
    static void TYParserFixPredictLine();
    static void TYParserBEGIN_INITIAL();

    // Helper functions
    static std::vector<TYArgment*>* arrayIfNotArray(TYArgment* receiver);
    static std::vector<TYArgment*>* arrayWithAddObject(TYArgment* receiver, TYArgment* aObject);
    static std::vector<TYArgment*>* arrayOfForCmd(TYArgment* bindVar,
                                                   TYOpType op,
                                                   TYArgment* start,
                                                   TYArgment* to_token,
                                                   TYArgment* end,
                                                   TYArgment* step_token,
                                                   TYArgment* step);

private:
    // Parser state
    int yychar;
    int yynerrs;
    bool yydebug;

    // Internal parser implementation
    int yyparse_internal();
    void yyerror(const char* msg);
};

#endif // NSCR_PARSER_TAB_H
