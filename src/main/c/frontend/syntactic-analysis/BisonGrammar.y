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
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */

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

%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> OPEN_COMMENT
%token <token> CLOSE_COMMENT
%token <token> COMMA
%token <token> ARROW
%token <token> ASSIGN

%token <text> IDENT
%token <text> TEXT

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
/*
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program
*/

//TODO: Completar con lo que falta
%type  <program> program
%type  <decl>    project_decl section_decl
%type  <decls>   section_list_opt
%type  <items>   src_decl flags_decl arg_list_opt arg_list

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
  : project_decl section_list_opt                { $$ = MakeProgram($1, $2); }
  ;

project_decl
  : PROJECT IDENT                                 { $$ = MakeProjectDecl($2); }
  ;

section_list_opt
  : /* empty */									{ $$ = MakeDeclListEmpty(); }
  | section_list_opt section_decl               { $$ = AddSectionToList($1, $2); }
  ;

section_decl
  : src_decl                                     { $$ = MakeSrcDecl($1); }
  | flags_decl                                   { $$ = MakeFlagsDecl($1); }
  | build_decl                                   { $$ = MakeBuildDecl(); }
  | run_decl                                     { $$ = MakeRunDecl(); }
  ;

src_decl
  : SRC OPEN_BRACE arg_list_opt CLOSE_BRACE     { $$ = $3; }
  ;

flags_decl
  : FLAGS OPEN_BRACE arg_list_opt CLOSE_BRACE   { $$ = $3; }
  ;

build_decl : BUILD ;

run_decl   : RUN   ;

arg_list_opt
  : arg_list                                     { $$ = $1; }
  | /* empty */                                  { $$ = MakeArgListEmpty(); }
  ;

arg_list
  : arg_list IDENT                               { $$ = AddArgToList($1, $2); }
  | arg_list TEXT                                { $$ = AddArgToList($1, $2); }
  | arg_list COMMA IDENT                         { $$ = AddArgToList($1, $3); }
  | arg_list COMMA TEXT                          { $$ = AddArgToList($1, $3); }
  | IDENT                                        { $$ = MakeArgList($1); }
  | TEXT                                         { $$ = MakeArgList($1); }
  ;

%%
