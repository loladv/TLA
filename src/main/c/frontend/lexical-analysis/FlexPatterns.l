%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */
%option yylineno

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

/* Patrones reusables */
whitespace          [[:space:]]+

/* Un átomo de texto sin espacios ni llaves/comas (flags, paths, args, libs, etc.) */
text_atom           [^[:space:]{},]+

%%

/* Palabras clave (MakeLite-C) */
"project"           { return SimpleLexemeAction(PROJECT); }
"compiler"          { return SimpleLexemeAction(COMPILER); }
"flags"             { return SimpleLexemeAction(FLAGS); }
"src"               { return SimpleLexemeAction(SRC); }
"headers"           { return SimpleLexemeAction(HEADERS); }
"libs"              { return SimpleLexemeAction(LIBS); }
"output"            { return SimpleLexemeAction(OUTPUT); }
"build"             { return SimpleLexemeAction(BUILD); }
"run"               { return SimpleLexemeAction(RUN); }
"var"               { return SimpleLexemeAction(VAR); }
"use"               { return SimpleLexemeAction(USE); }
"pre_build"         { return SimpleLexemeAction(PRE_BUILD); }
"post_build"        { return SimpleLexemeAction(POST_BUILD); }
"clean"             { return SimpleLexemeAction(CLEAN); }
"test"              { return SimpleLexemeAction(TEST); }
"target"            { return SimpleLexemeAction(TARGET); }

/* Símbolos */
"{"                 { return SimpleLexemeAction(OPEN_BRACE); }
"}"                 { return SimpleLexemeAction(CLOSE_BRACE); }
","                 { return SimpleLexemeAction(COMMA); }
"=>"                { return SimpleLexemeAction(ARROW); }
"="                 { return SimpleLexemeAction(ASSIGN); }

/* Identificadores (variables, nombres de target/binario) */
[[:alpha:]_][[:alnum:]_]*   { return TextualLexemeAction(IDENT); }

/* Texto genérico (flags/paths/args/libs) */
{text_atom}         { return TextualLexemeAction(TEXT); }

/* Comentarios multilínea (como el template) */
"/*"                        { return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"     { return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+    { return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.        { return IgnoredLexemeAction(); }

/* Espacios */
{whitespace}        { return IgnoredLexemeAction(); }

/* Desconocido */
.                   { return UnknownLexemeAction(); }

/* EOF */
<<EOF>>             { return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"