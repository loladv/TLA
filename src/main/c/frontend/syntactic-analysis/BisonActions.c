#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

/*

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Program * ExpressionProgramSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->expression = expression;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

*/

//TODO: Completar con lo que falta

Program* MakeProgram(Decl* projectDecl, DeclList* sections){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->projectDecl = projectDecl;
	program->sections = sections;
	return program;
}

Decl* MakeProjectDecl(char* name){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Decl * decl = calloc(1, sizeof(Decl));
	decl->type = PROJECT_DECL;
	decl->projectName = name;
	return decl;
}

DeclList* MakeDeclListEmpty(void){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return NULL;
}

DeclList* AddSectionToList(DeclList* xs, Decl* d){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	DeclList * node = calloc(1, sizeof(DeclList));
	node->decl = d;
	node->next = NULL;
	if (!xs) return node;
	DeclList* cur = xs;
	while (cur->next) cur = cur->next;
	cur->next = node;
	return xs;
}

Decl* MakeSrcDecl(ItemList* items){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Decl * decl = calloc(1, sizeof(Decl));
	decl->type = SRC_DECL;
	decl->srcFiles = items;
	return decl;
}

Decl* MakeBuildDecl(void){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Decl * decl = calloc(1, sizeof(Decl));
	decl->type = BUILD_DECL;
	return decl;
}

Decl* MakeRunDecl(void){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Decl * decl = calloc(1, sizeof(Decl));
	decl->type = RUN_DECL;
	return decl;
}

ItemList* MakeArgListEmpty(void){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return NULL;
}

ItemList* MakeArgList(char* first){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ItemList * itemList = calloc(1, sizeof(ItemList));
	itemList->item = calloc(1, sizeof(Item));
	itemList->item->text = first;
	itemList->next = NULL;
	return itemList;
}

ItemList* AddArgToList(ItemList* xs, char* t){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ItemList * node = calloc(1, sizeof(ItemList));
    node->item = calloc(1, sizeof(Item));
    node->item->text = t;
    node->next = NULL;
    if (!xs) return node;
    ItemList* cur = xs;
    while (cur->next) cur = cur->next;
    cur->next = node;
    return xs;
}
