#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState);
bool HasSemanticError(void);
/* Glob validation */
bool IsUnsupportedGlob(const char * text);

/**
 * Bison semantic actions.
 */

 /*

Constant * IntegerConstantSemanticAction(const int value);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);
Program * ExpressionProgramSemanticAction(Expression * expression);

*/

//TODO: Completar con lo que falta
/* programa y secciones */
Program*  MakeProgram(Decl* projectDecl, DeclList* sections);
Decl*     MakeProjectDecl(char* name);
DeclList* MakeDeclListEmpty(void);
DeclList* AddSectionToList(DeclList* xs, Decl* d);
Command* MakeCommand(CommandType type, ItemList* args); // Generic command builder

CommandList* MakeCommandList(Command* firstCommand, CommandList* next); 
CommandList* AddCommandToList(CommandList* list, Command* newCommand); 

/* src / flags / libs / compiler / build / run */
Decl*     MakeSrcDecl(ItemList* items);
Decl*     MakeFlagsDecl(ItemList* flags);
Decl*     MakeLibsDecl(ItemList* libs);
Decl*     MakeCompilerDecl(char* compiler);
Decl*     MakeHeadersDecl(ItemList* headers);
Decl*     MakeOutputDecl(char* outputName);
Decl*     MakePreBuildDecl(CommandList* commands);
Decl*     MakePostBuildDecl(CommandList* commands);
Decl*     MakeBuildDecl(void);
Decl*     MakeRunDecl(void);
Decl*     MakeLogDecl(char* path, int mode);
int       MakeLogMode(int mode);

/* args de src/flags/libs and var expansion */
ItemList* MakeArgListEmpty(void);
ItemList* MakeArgList(char* first);            /* crea lista con 1 item */
ItemList* AddArgToList(ItemList* xs, char* t); /* agrega item al final */
ItemList* ConcatItemLists(ItemList* xs, ItemList* ys); /* concatena dos listas */

/* variables */
void      VarAssign(char* name, ItemList* value);
ItemList* UseVar(char* name);


#endif
