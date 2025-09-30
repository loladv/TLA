#include "BisonActions.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;
static bool _semanticError = false;

// Simple variable table for var/use
typedef struct VarBinding {
    char * name;
    ItemList * value; // owned list
    struct VarBinding * next;
} VarBinding;
static VarBinding * _vars = NULL;

// Forward declarations
static void _freeVarTable(void);

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
    _freeVarTable();
    _compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
    _semanticError = false;
	return _shutdownBisonActionsModule;
}
bool HasSemanticError(void) {
    return _semanticError;
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

static char * _dup(const char * s) {
    size_t n = strlen(s);
    char * c = calloc(n + 1, 1);
    memcpy(c, s, n);
    return c;
}

static void _freeVarTable(void) {
    VarBinding * v = _vars;
    while (v) {
        VarBinding * nxt = v->next;
        if (v->name) free(v->name);
        destroyItemList(v->value);
        free(v);
        v = nxt;
    }
    _vars = NULL;
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
    // Expose AST to the rest of the compiler phases
    if (_compilerState != NULL) {
        _compilerState->abstractSyntaxtTree = program;
    }
    // Check at least one SRC_DECL with non-empty items
    bool hasSrc = false;
    for (DeclList* it = sections; it != NULL; it = it->next) {
        if (it->decl && it->decl->type == SRC_DECL) {
            if (it->decl->srcFiles != NULL) {
                hasSrc = true;
                break;
            }
        }
    }
    if (!hasSrc) {
        logError(_logger, "No source files declared (missing or empty src section).");
        _semanticError = true;
    }
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

Decl* MakeFlagsDecl(ItemList* flags){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Decl * decl = calloc(1, sizeof(Decl));
	decl->type = FLAGS_DECL;
	decl->flags = flags;
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

ItemList* ConcatItemLists(ItemList* xs, ItemList* ys){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    if (!xs) return ys;
    ItemList* cur = xs;
    while (cur->next) cur = cur->next;
    cur->next = ys;
    return xs;
}

Decl* MakeLibsDecl(ItemList* libs){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Decl * decl = calloc(1, sizeof(Decl));
    decl->type = LIBS_DECL;
    decl->libs = libs;
    return decl;
}

Decl* MakeCompilerDecl(char* compiler){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Decl * decl = calloc(1, sizeof(Decl));
    decl->type = COMPILER_DECL;
    decl->compilerName = compiler;
    return decl;
}

void VarAssign(char* name, ItemList* value){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    // overwrite if exists
    for (VarBinding* v = _vars; v; v = v->next) {
        if (strcmp(v->name, name) == 0) {
            destroyItemList(v->value);
            v->value = value;
            free(name);
            return;
        }
    }
    VarBinding* nb = calloc(1, sizeof(VarBinding));
    nb->name = name;
    nb->value = value;
    nb->next = _vars;
    _vars = nb;
}

ItemList* UseVar(char* name){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    for (VarBinding* v = _vars; v; v = v->next) {
        if (strcmp(v->name, name) == 0) {
            // deep copy list
            ItemList* head = NULL;
            ItemList* tail = NULL;
            for (ItemList* it = v->value; it; it = it->next) {
                ItemList* node = calloc(1, sizeof(ItemList));
                node->item = calloc(1, sizeof(Item));
                node->item->text = _dup(it->item->text);
                if (!head) head = node; else tail->next = node;
                tail = node;
            }
            free(name);
            return head;
        }
    }
    // undefined var -> semantic error
    logError(_logger, "Use of undefined variable '%s'", name);
    _semanticError = true;
    free(name);
    return NULL;
}
