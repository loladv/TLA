%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%option yylineno

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

/* Acciones existentes del template */
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus LeaveMultilineCommentLexemeAction();

/* Acciones NUEVAS genéricas para MakeLite-C (manteniendo el estándar del template) */
extern CompilationStatus SimpleLexemeAction(TokenLabel label);   /* keywords y símbolos */
extern CompilationStatus TextualLexemeAction(TokenLabel label);  /* IDENT, TEXT */

%}

/* Contexto para comentarios multilínea (como el template) */
%x MULTILINE_COMMENT
%x COMMANDS

/* Patrones reusables */
whitespace          [[:space:]]+

/* Un átomo de texto sin espacios ni llaves/comas (flags, paths, args, libs, etc.) */
text_atom           [^[:space:]{},]+

/* Palabras clave (MakeLite-C) */
%%

"project"           { return SimpleLexemeAction(PROJECT); }
"compiler"          { return SimpleLexemeAction(COMPILER); }
"flags"             { return SimpleLexemeAction(FLAGS); }
"sources"           { return SimpleLexemeAction(SRC); }
"headers"           { return SimpleLexemeAction(HEADERS); }
"libraries"         { return SimpleLexemeAction(LIBS); }
"output"            { return SimpleLexemeAction(OUTPUT); }
"build"             { return SimpleLexemeAction(BUILD); }
"run"               { return SimpleLexemeAction(RUN); }
"var"               { return SimpleLexemeAction(VAR); }
"use"               { return SimpleLexemeAction(USE); }
"pre_build"         { BEGIN(COMMANDS); return SimpleLexemeAction(PRE_BUILD); }
"post_build"        { BEGIN(COMMANDS); return SimpleLexemeAction(POST_BUILD); }
"clean"             { return SimpleLexemeAction(CLEAN); }
"test"              { return SimpleLexemeAction(TEST); }
"target"            { return SimpleLexemeAction(TARGET); }

<COMMANDS>"{"       { return SimpleLexemeAction(OPEN_BRACE); }
<COMMANDS>"}"       { BEGIN(INITIAL); return SimpleLexemeAction(CLOSE_BRACE); }
"{"                 { return SimpleLexemeAction(OPEN_BRACE); }
"}"                 { return SimpleLexemeAction(CLOSE_BRACE); }
","                 { return SimpleLexemeAction(COMMA); }
"=>"                { return SimpleLexemeAction(ARROW); }
"="                 { return SimpleLexemeAction(ASSIGN); }

[[:alpha:]_][[:alnum:]_]*   { return TextualLexemeAction(IDENT); }

"//[^\n]*"                         { return IgnoredLexemeAction(); }
<COMMANDS>\n                        { return IgnoredLexemeAction(); }
<COMMANDS>{whitespace}           { return IgnoredLexemeAction(); }

"/*"                        { return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"     { return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+    { return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.        { return IgnoredLexemeAction(); }

{whitespace}        { return IgnoredLexemeAction(); }

<COMMANDS>[^\n{}[:space:]]+   { return TextualLexemeAction(TEXT); }
{text_atom}         { return TextualLexemeAction(TEXT); }

.                   { return UnknownLexemeAction(); }

<<EOF>>             { return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"