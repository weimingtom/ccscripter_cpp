%{
#include <vector>
#include <string>
#include "TYScriptEngine.h"

#include "TYIntVarArgment.h"
#include "TYStringVarArgment.h"
#include "TYArrayVarArgment.h"
#include "TYFchkStringArgment.h"

//FIXME:added
extern int yylex (void);
extern int yyerror(const char*);

const char *OUTPUT_FORMAT = "%@";

static NSMutableArray *arrayIfNotArray(void *);
static NSMutableArray *arrayWithAddObject(void *, void *);
static NSMutableArray *arrayOfForCmd(void *, TYOpType, void *, void *, void *, void *, void *);

extern int TYParserReadLine;
extern void TYParserFixPredictLine(void);
extern void TYParserBEGIN_INITIAL(void);
%}

%union {
        bool b;
	void *obj;
        TYOpType op;
}

%token <obj> DIGIT
%token MOD
%token <obj> ALNUM_
%token BLANK_LINE
%token <obj> COMMAND
%token <obj> CONST_STR
%token <b> CMD_IF
%token <op> OP_CMP
%token OP_AND
%token FUNC_FCHK
%token FUNC_LCHK
%token <obj> PRINT
%token <obj> SEL_CMD
%token CR_LF

%left '-' '+'
%left '*' '/' MOD
%nonassoc UMINUS

%type <obj> expression
%type <obj> number
%type <obj> argments
%type <obj> arg
%type <b> cond_state
%type <b> cond_statements
%type <obj> cmd_statement
%type <obj> selcmd_statement
%type <obj> array_index
%type <obj> cmd
%type <obj> for_argments
%type <obj> string
%type <obj> exec_statement
%type <obj> print_statement
%type <obj> print_chunk
%%

statement : exec_statement { if($1) TYScriptEngine::sharedEngine()->eval($1); }
        | exec_statement error { 
                if($1 != NULL) {
                    TYScriptEngine::sharedEngine()->incRunColumn();
                    TYScriptEngine::sharedEngine()->eval($1); 
                }
            }
        |
        ;

exec_statement: cmd_statement { $$ = $1; }
        | selcmd_statement { TYParserFixPredictLine(); TYScriptEngine::sharedEngine()->eval($1);  YYACCEPT; }
        | if_statement cmd_statement { $$ = $2; }
        | if_statement { $$ = NULL; }
        | print_statement { 
std::vector<void *> *kkk = new std::vector<void *>(); kkk->push_back((void*)"_print"); kkk->push_back((void*)$1);
$$ = kkk; }
        ;

print_statement : print_chunk 
        | print_statement print_chunk { $$ = (void *)new std::string(std::string((const char*)$1) + std::string((const char*)$2)); }
        ;

print_chunk : PRINT
        | '$' number { $$ = TYStringVarArgment::argmentWithVarID($2)->stringObj(); }
        | '%' number { $$ = TYIntVarArgment::argmentWithVarID($2)->intNumber()->stringValue(); }
        | ALNUM_
        ;

        
cmd_statement : cmd argments { ((NSMutableArray *)$2)->insertObject($1, 0); $$ = $2; }
        | cmd { $$ = arrayIfNotArray($1); }
        | cmd for_argments { 
                if(!((NSString *)$1)->isEqualToString("for")) {
                    YYERROR;
                } else {
                    TYScriptEngine::sharedEngine()->registLoopTargetID(  ((TYArgment *)((NSMutableArray *)$2)->objectAtIndex(0))->varID(), 
                                                               ((NSMutableArray *)$2)->objectAtIndex(1),
                                                               ((NSMutableArray *)$2)->objectAtIndex(2),
                                                               ((NSMutableArray *)$2)->objectAtIndex(3)); 
                    YYACCEPT;
                }
            }
        ;

cmd : COMMAND 
    | ALNUM_
    ;
        
selcmd_statement : SEL_CMD pre_blank argments CR_LF { ((NSMutableArray *)$3)->insertObject($1, 0);  TYParserReadLine++; $$ = $3; }
        ;

pre_blank : CR_LF pre_blank
        |
        ;

if_statement : CMD_IF cond_statements { if($1 != $2) { TYScriptEngine::sharedEngine()->resetRunColumn(); YYACCEPT; } } 
        ;

cond_statements : cond_state
        | cond_statements OP_AND cond_state { if($1 != $3) { TYScriptEngine::sharedEngine()->resetRunColumn(); YYACCEPT; } else $$ = $1; }
        ;

cond_state : arg OP_CMP arg { $$ = TYCompareArgment($1, $3, $2); }
        | FUNC_FCHK arg { $$ = TYScriptEngine::sharedEngine()->judgefchk($2); }
        | FUNC_LCHK arg { $$ = TYScriptEngine::sharedEngine()->judgelchk($2); }
        ;

for_argments : '%' number OP_CMP expression ALNUM_ expression {
                $$ = arrayOfForCmd(TYIntVarArgment::argmentWithVarID($2),
                                   $3,
                                   $4,
                                   $5,
                                   $6,
                                   NULL, NULL);
            }
        | '%' number OP_CMP expression ALNUM_ expression ALNUM_ expression {
                $$ = arrayOfForCmd(TYIntVarArgment::argmentWithVarID($2),
                                   $3,
                                   $4,
                                   $5,
                                   $6,
                                   $7,
                                   $8);
            }
        ;

argments : arg { $$ = arrayIfNotArray($1); }
        | argments ',' arg { $$ = arrayWithAddObject($1, $3); }
        | argments ',' CR_LF arg { $$ = arrayWithAddObject($1, $4); TYParserReadLine++; }
        ;

arg : expression
    | string
    ;

string : '$' number  { $$ = TYStringVarArgment::argmentWithVarID($2); }
    | CONST_STR
    | '(' string ')' string string { 
            $$ = TYFchkStringArgment::argmentWithFileName($2, $4, $5);
        }
    | ALNUM_
    ;
	
expression: expression '+' expression { $$ = NSNumber::numberWithInt(((TYArgment *)$1)->intValue() + ((TYArgment *)$3)->intValue()); }
	|   expression '-' expression { $$ = NSNumber::numberWithInt(((TYArgment *)$1)->intValue() - ((TYArgment *)$3)->intValue()); }
	|   expression '*' expression { $$ = NSNumber::numberWithInt(((TYArgment *)$1)->intValue() * ((TYArgment *)$3)->intValue()); }
	|   expression '/' expression { $$ = NSNumber::numberWithInt(((TYArgment *)$1)->intValue() / ((TYArgment *)$3)->intValue()); }
	|   expression MOD expression { $$ = NSNumber::numberWithInt(((TYArgment *)$1)->intValue() % ((TYArgment *)$3)->intValue()); }
	|   '-' expression  %prec UMINUS { $$ = NSNumber::numberWithInt(((TYArgment *)$2)->intValue() * (-1)); }
	|   '(' expression  ')' { $$ = $2; }
	|   number { $$ = $1 ; }
	;

number : DIGIT { $$ = $1; }
        | ALNUM_ { $$ = ((TYArgment *)$1)->intNumber(); }
        | '%' number { $$ = TYIntVarArgment::argmentWithVarID($2); }
        | '?' number array_index { $$ = TYArrayVarArgment::argmentWithVarID($2, (NSArray*)$3); }
        ;
        
array_index : '[' expression ']' { $$ = arrayIfNotArray($2); }
        | array_index array_index { $$ = arrayWithAddObject($1, $2); }
        ;
%%

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

static NSMutableArray *arrayIfNotArray(void *receiver)
{
//    if(![ receiver respondsToSelector:@selector(addObject:)]){
//        return [ NSMutableArray arrayWithObject:receiver ];
//    } else {
//        return receiver;
//    }
throw "not implemented";
}

static NSMutableArray *arrayWithAddObject(void *receiver, void *aObject)
{    
/*
    if(![ receiver respondsToSelector:@selector(addObject:)]){
        receiver = [ NSMutableArray arrayWithObject:receiver ];
    }
    
    if([ aObject respondsToSelector:@selector(addObject:)]){
        [ receiver addObjectsFromArray:aObject ];
    } else {
        [ receiver addObject:aObject ];
    }
    
    return receiver;
*/
	throw "not implemented";
}

static NSMutableArray *arrayOfForCmd(void *bindVar,
                                     TYOpType op,
                                     void *start,
                                     void *to_token,
                                     void *end,
                                     void *step_token,
                                     void *step)
{
/*
    if(op != TYEqualOpType){
        return NULL;
    }
    if(![ to_token isEqualToString:@"to" ])
        return NULL;
    if(step_token && ![ step_token isEqualToString:@"step" ])
        return NULL;
    
    if(!step)
        step = [ NSNumber numberWithInt:1 ];
        
    return [ NSMutableArray arrayWithObjects:bindVar,start,end,step,nil ];
*/
	throw "not implemented";
}




