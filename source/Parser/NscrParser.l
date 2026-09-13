%{
#import <Foundation/Foundation.h>

#import "TYDigitArgment.h"
#import "TYAliasArgment.h"
#import "TYConstStringArgment.h"
#import "TY-NSStringAddition.h"
#import "NscrParser.tab.h"
#import "TYScriptEngine.h"

#undef YY_INPUT
#define YY_INPUT(b,r,ms) (r=TY_yyinput(b,ms))

const char *TYParserInput;
int TYParserReadLength;
int TYParserStringLength;
BOOL TYParserSelCmdMode;
int TYParserPredictLine; // 先読みした行数
int TYParserReadLine; // 実際に構文解析で使用した行数

void TYParserSetReadBuffer(const char*);

%}

%s SEARCH_ARG
%s PRINTING
%%

[0-9]+ { yylval.obj = [ NSNumber argmentWithInt:atoi(yytext) ]; return DIGIT; }
\"[^"\0]*\x22 { yylval.obj = [ TYConstStringArgment argmentWithString:[ NSString stringWithCSJISString:yytext +1 length:yyleng -2 ] ]; return CONST_STR; }
fchk { return FUNC_FCHK; }
lchk { return FUNC_LCHK; }
<INITIAL>\*([a-z_][a-z_0-9]*|([\x80-\xFF].)+) { [ [ TYScriptEngine sharedEngine ] addLabelLog:[ NSString stringWithCSJISString:yytext ] ]; return 0; }
<INITIAL>~ { return 0; }
<INITIAL>if|notif { yylval.b = (tolower(yytext[0]) == 'i') ? YES : NO; BEGIN SEARCH_ARG; return CMD_IF; }
<INITIAL>select|selgosub|csel { yylval.obj = [ NSString stringWithCSJISString:yytext ]; BEGIN SEARCH_ARG; TYParserSelCmdMode=YES; return SEL_CMD; }
<INITIAL>[a-z_][a-z_0-9]* { yylval.obj = [ NSString stringWithCSJISString:yytext ]; BEGIN SEARCH_ARG;  return COMMAND; }
mod { return MOD; }
={1,2} { yylval.op = TYEqualOpType; return OP_CMP; }
!=|\<\> { yylval.op = TYNotEqualOpType; return OP_CMP;}
\<= { yylval.op = TYLessOrEqualOpType; return OP_CMP; }
\< { yylval.op = TYLessThanOpType; return OP_CMP; }
\>= { yylval.op = TYGreatOrEqualOpType; return OP_CMP; } 
\> { yylval.op = TYGreatThanOpTYpe; return OP_CMP; }
&{1,2} { return OP_AND; }
[a-z_][a-z_0-9]* { yylval.obj = [ NSString argmentWithCString:yytext ]; return ALNUM_; }
\*([a-z_][a-z_0-9]*|([\x80-\xFF].)+) { yylval.obj = [ NSString argmentWithCString:yytext ]; return ALNUM_; }
<SEARCH_ARG>\#[0-9a-f]{6} { yylval.obj = [ NSString argmentWithCString:yytext ]; return ALNUM_; }
[ \t]+ ;
[:;\r\n\0] {
        if(yytext[0] == ':')
            [ [ TYScriptEngine sharedEngine ] incRunColumn ];
        else 
            [ [ TYScriptEngine sharedEngine ] resetRunColumn ];
        
        return (TYParserSelCmdMode) ? CR_LF : 0;
    }
<INITIAL,PRINTING>\#[0-9a-f]{6}|![dsw][0-9]+|!sd|[_@\\/] { yylval.obj = [ NSString stringWithCSJISString:yytext ]; BEGIN PRINTING; return PRINT; }
<INITIAL,PRINTING>([\x80-\xFF].)+ { yylval.obj = [ NSString stringWithCSJISString:yytext ]; BEGIN PRINTING; return PRINT; }
<INITIAL>[$%] { BEGIN PRINTING; return yytext[0]; }
. { return (unsigned char)yytext[0]; }

%%

#if YYDEBUG == 1
extern int yydebug;
#endif
void TYParserSetCString(const char *ptr)
{
//yydebug=1;
    TYParserSetReadBuffer(ptr);
    yyrestart(NULL);
    BEGIN INITIAL;
    TYParserSelCmdMode = NO;
    TYParserPredictLine = 0;
}

void TYParserSetReadBuffer(const char *ptr)
{
    TYParserStringLength = strlen(ptr) +1;
    TYParserReadLength = 0;
    TYParserInput = ptr;
}

void TYParserBeginSelCmdMode(void)
{
    TYParserSelCmdMode = YES;
    TYParserPredictLine = 0;
    TYParserReadLine = 0;
}

void TYParserBEGIN_INITIAL(void)
{
    BEGIN INITIAL;
}

void TYParserFixPredictLine(void)
{
    int fix = TYParserPredictLine -TYParserReadLine;
    
    if(fix){
        [ [ TYScriptEngine sharedEngine ] moveRunLine:fix ];
    }
}

int TY_yyinput(char *buf, int max_size)
{
    int rest = TYParserStringLength -TYParserReadLength;
    int n = (max_size < rest) ? max_size : rest;
    
    if(n > 0){
        memcpy(buf,TYParserInput+TYParserReadLength,n);
        TYParserReadLength += n;
    }
    
    return n;
}

int yywrap(void)
{
    if(TYParserSelCmdMode){
        TYParserPredictLine++;
        TYParserSetReadBuffer([ [ TYScriptEngine sharedEngine ] nextLine ]);
        return 0;
    } else {
        return 1;
    }
}
