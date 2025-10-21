%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */
	signed int integer; //Lo usan algunas de las acciones que vinieron en el template de la calculadora
	char * text;
	TokenLabel token;

	/** Non-terminals. */

	Decl* decl;
	DeclList* decls;
	Item* item;
	ItemList* items;
	Program * program;

  Command* command;
  CommandList* commandList;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */

%destructor { destroyCommand($$); } <command>
%destructor { destroyCommandList($$); } <commandList> 
%destructor { destroyDecl($$); } <decl>
%destructor { destroyDeclList($$); } <decls>
%destructor { destroyItemList($$); } <items>



/** Terminals. */
%token <integer> INTEGER
%token <token> PROJECT
%token <token> COMPILER
%token <token> FLAGS
%token <token> SRC
%token <token> HEADERS
%token <token> LIBS
%token <token> OUTPUT
%token <token> BUILD
%token <token> RUN
%token <token> VAR
%token <token> USE
%token <token> PRE_BUILD
%token <token> POST_BUILD
%token <token> CLEAN
%token <token> TEST
%token <token> TARGET
%token <token> LOG
%token <token> APPEND
%token <token> OVERWRITE

%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> OPEN_COMMENT
%token <token> CLOSE_COMMENT
%token <token> COMMA
%token <token> SEMICOLON
%token <token> ARROW
%token <token> ASSIGN

%token <text> IDENT
%token <text> TEXT

%token <token> IGNORED
%token <token> UNKNOWN

%token <token> MKDIR
%token <token> RM
%token <token> CP
%token <token> MV

/** Non-terminals. */
/*
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program
*/

//TODO: Completar con lo que falta
%type  <command> command
%type  <commandList> command_list command_list_opt pre_build_decl post_build_decl 
%type  <program> program
%type  <decl>    project_decl section_decl var_decl
%type  <text>    compiler_decl output_decl
%type  <decls>   section_list_opt
%type  <items>   sources_decl flags_decl libraries_decl headers_decl arg_list_opt arg_list use_items
%type  <decl>    log_decl
%type  <integer> log_mode

//Lo dejo comentado para ver como se usa si lo necesitaramos
/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
/*
%left ADD SUB
%left MUL DIV
*/

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.
/*

program: expression											{ $$ = ExpressionProgramSemanticAction($1); }
	;

expression: expression[left] ADD expression[right]			{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| factor												{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS		{ $$ = ExpressionFactorSemanticAction($2); }
	| constant												{ $$ = ConstantFactorSemanticAction($1); }
	;

constant: INTEGER											{ $$ = IntegerConstantSemanticAction($1); }
	;

*/

//TODO: Completar, arriba esta el ejemplo de la calculadora

program
  : var_decl_list_opt project_decl section_list_opt  { $$ = MakeProgram($2, $3); }
  ;

var_decl_list_opt
  : %empty
  | var_decl_list_opt var_decl                   { /* Process var_decl but don't return it */ }
  ;

project_decl
  : PROJECT IDENT                                 { $$ = MakeProjectDecl($2); }
  ;

section_list_opt
  : %empty									{ $$ = MakeDeclListEmpty(); }
  | section_list_opt section_decl               { $$ = AddSectionToList($1, $2); }
  ;

section_decl
  : sources_decl                                 { $$ = MakeSrcDecl($1); }
  | libraries_decl                               { $$ = MakeLibsDecl($1); }
  | compiler_decl                                { $$ = MakeCompilerDecl($1); }
  | headers_decl                                 { $$ = MakeHeadersDecl($1); }
  | output_decl                                  { $$ = MakeOutputDecl($1); }
  | flags_decl                                   { $$ = MakeFlagsDecl($1); }
  | pre_build_decl                               { $$ = MakePreBuildDecl($1); }
  | post_build_decl                              { $$ = MakePostBuildDecl($1); }
  | build_decl                                   { $$ = MakeBuildDecl(); }
  | run_decl                                     { $$ = MakeRunDecl(); }
  | log_decl                                     { $$ = $1; }
  ;

sources_decl
  : SRC OPEN_BRACE arg_list_opt CLOSE_BRACE     { $$ = $3; }
  ;

flags_decl
  : FLAGS OPEN_BRACE arg_list_opt CLOSE_BRACE   { $$ = $3; }
  ;

libraries_decl
  : LIBS OPEN_BRACE arg_list_opt CLOSE_BRACE    { $$ = $3; }
  ;
headers_decl
  : HEADERS OPEN_BRACE arg_list_opt CLOSE_BRACE { $$ = $3; }
  ;

output_decl
  : OUTPUT IDENT                                 { $$ = $2; }
  | OUTPUT TEXT                                  { $$ = $2; }
  ;

pre_build_decl
  : PRE_BUILD OPEN_BRACE command_list_opt CLOSE_BRACE { $$ = $3; } 
  ;

post_build_decl
  : POST_BUILD OPEN_BRACE command_list_opt CLOSE_BRACE { $$ = $3; } 
  ;

compiler_decl
  : COMPILER IDENT                               { $$ = $2; }
  | COMPILER TEXT                                { $$ = $2; }
  ;

build_decl : BUILD ;

run_decl   : RUN   ;

command_list_opt
  : %empty                  { $$ = NULL; } 
  | command_list            { $$ = $1; }
  ;

  command_list
  : command                 { $$ = MakeCommandList($1, NULL); } 
  | command_list command    { $$ = AddCommandToList($1, $2); }  
  ;

command
  : TEXT { $$ = MakeCommandFromLine($1); }
  ;

arg_list_opt
  : arg_list                                     { $$ = $1; }
  | %empty                                       { $$ = MakeArgListEmpty(); }
  ;

arg_list
  : arg_list IDENT                               { $$ = AddArgToList($1, $2); }
  | arg_list TEXT                                { $$ = AddArgToList($1, $2); }
  | arg_list use_items                           { $$ = ConcatItemLists($1, $2); }
  | arg_list COMMA IDENT                         { $$ = AddArgToList($1, $3); }
  | arg_list COMMA TEXT                          { $$ = AddArgToList($1, $3); }
  | arg_list COMMA use_items                     { $$ = ConcatItemLists($1, $3); }
  | IDENT                                        { $$ = MakeArgList($1); }
  | TEXT                                         { $$ = MakeArgList($1); }
  | use_items                                    { $$ = $1; }
  ;

use_items
  : USE IDENT                                    { $$ = UseVar($2); }
  ;

var_decl
  : VAR IDENT ASSIGN arg_list_opt                { VarAssign($2, $4); $$ = NULL; }
  ;

log_decl
  : LOG OPEN_BRACE TEXT log_mode CLOSE_BRACE     { $$ = MakeLogDecl($3, $4); }
  ;

log_mode
  : APPEND                                      { $$ = MakeLogMode(APPEND_MODE); }
  | OVERWRITE                                   { $$ = MakeLogMode(OVERWRITE_MODE); }
  ;

%%
