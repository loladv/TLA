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
static bool _contains(const char * s, const char * needle);

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

static bool _contains(const char * s, const char * needle) {
    return (s != NULL && needle != NULL && strstr(s, needle) != NULL);
}

static bool _phaseExists(const char* phaseName, DeclList* sections) {
    if (phaseName == NULL) return false;
    for (DeclList* it = sections; it != NULL; it = it->next) {
        if (it->decl == NULL) continue;
        if (it->decl->type == PRE_BUILD_DECL && strcmp(phaseName, "pre_build") == 0) {
            return true;
        }
        if (it->decl->type == POST_BUILD_DECL && strcmp(phaseName, "post_build") == 0) {
            return true;
        }
        if (it->decl->type == CUSTOM_PHASE_DECL && it->decl->customPhase.phaseName != NULL &&
            strcmp(it->decl->customPhase.phaseName, phaseName) == 0) {
            return true;
        }
    }
    return false;
}

bool IsUnsupportedGlob(const char * text) {
    if (text == NULL) return false;
    /* Ahora permitimos todos los patrones GLOB avanzados */
    return false;
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
    
    // Validate conditional phase references
    for (DeclList* it = sections; it != NULL; it = it->next) {
        if (it->decl && it->decl->type == CONDITIONAL_DECL) {
            if (it->decl->conditionalPhase.condition) {
                const char* phaseName = it->decl->conditionalPhase.condition->phaseName;
                if (!_phaseExists(phaseName, sections)) {
                    logError(_logger, "Conditional references unknown phase '%s'", phaseName);
                    _semanticError = true;
                }
            }
        }
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
    if (IsUnsupportedGlob(first)) {
        logError(_logger, "Unsupported glob pattern: %s", first);
        _semanticError = true;
    }
	ItemList * itemList = calloc(1, sizeof(ItemList));
	itemList->item = calloc(1, sizeof(Item));
	itemList->item->text = first;
	itemList->next = NULL;
	return itemList;
}

ItemList* AddArgToList(ItemList* xs, char* t){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    if (IsUnsupportedGlob(t)) {
        logError(_logger, "Unsupported glob pattern: %s", t);
        _semanticError = true;
    }
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

Decl* MakeHeadersDecl(ItemList* headers){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Decl * decl = calloc(1, sizeof(Decl));
    decl->type = HEADERS_DECL;
    decl->headers = headers;
    return decl;
}

Decl* MakeOutputDecl(char* outputName){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Decl * decl = calloc(1, sizeof(Decl));
    decl->type = OUTPUT_DECL;
    decl->outputName = outputName;
    return decl;
}

Command* MakeCommand(CommandType type, ItemList* args) {
    _logSyntacticAnalyzerAction(__FUNCTION__);

    // Extraer el texto completo del primer (y único) item
    char* commandLine = NULL;
    if (args && args->item && args->item->text) {
        commandLine = strdup(args->item->text);
    } else {
        commandLine = strdup("");
    }
    
    // Liberar la lista temporal de args ya que ya extrajimos el string
    destroyItemList(args);
    
    // Validaciones básicas para comandos que requieren argumentos
    bool error = false;
    if (strlen(commandLine) == 0) {
        switch (type) {
            case MKDIR_CMD:
            case RM_CMD:
            case CP_CMD:
            case MV_CMD:
                logError(_logger, "Command '%s' requires arguments.", 
                    (type == MKDIR_CMD ? "mkdir" : 
                     type == RM_CMD ? "rm" : 
                     type == CP_CMD ? "cp" : "mv"));
                error = true;
                break;
            default:
                logError(_logger, "Unknown command type %d.", type);
                error = true;
                break;
        }
    }

    if (error) {
        _semanticError = true;
        free(commandLine);
        return NULL;         
    }
    
    Command *cmd = calloc(1, sizeof(Command));
    cmd->type = type;
    cmd->commandLine = commandLine; 
    return cmd;
}

Command* MakeCommandFromLine(char* commandLine) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    
    if (commandLine == NULL || strlen(commandLine) == 0) {
        logError(_logger, "Empty command line.");
        _semanticError = true;
        return NULL;
    }
    
    // Determinar el tipo de comando basado en el primer token
    CommandType type;
    if (strncmp(commandLine, "mkdir", 5) == 0) {
        type = MKDIR_CMD;
    } else if (strncmp(commandLine, "rm", 2) == 0) {
        type = RM_CMD;
    } else if (strncmp(commandLine, "cp", 2) == 0) {
        type = CP_CMD;
    } else if (strncmp(commandLine, "mv", 2) == 0) {
        type = MV_CMD;
    } else {
        logError(_logger, "Unknown command: %s", commandLine);
        _semanticError = true;
        free(commandLine);
        return NULL;
    }
    
    // Validar que el comando tenga argumentos si es necesario
    char* firstSpace = strchr(commandLine, ' ');
    if (firstSpace == NULL) {
        // Comando sin argumentos
        switch (type) {
            case MKDIR_CMD:
            case RM_CMD:
            case CP_CMD:
            case MV_CMD:
                logError(_logger, "Command '%s' requires arguments.", 
                    (type == MKDIR_CMD ? "mkdir" : 
                     type == RM_CMD ? "rm" : 
                     type == CP_CMD ? "cp" : "mv"));
                _semanticError = true;
                free(commandLine);
                return NULL;
            default:
                logError(_logger, "Unknown command type %d.", type);
                _semanticError = true;
                free(commandLine);
                return NULL;
        }
    }
    
    Command *cmd = calloc(1, sizeof(Command));
    cmd->type = type;
    cmd->commandLine = commandLine; // Usar directamente el string del token (ya es una copia)
    return cmd;
}


CommandList* MakeCommandList(Command* firstCommand, CommandList* next) {
    
     if (firstCommand == NULL) return next; 
     _logSyntacticAnalyzerAction(__FUNCTION__);
     CommandList *list = calloc(1, sizeof(CommandList));
     list->command = firstCommand;
     list->next = next;
     return list;
}

CommandList* AddCommandToList(CommandList* list, Command* newCommand) {

    if (newCommand == NULL) return list; 
    _logSyntacticAnalyzerAction(__FUNCTION__);
    CommandList *newNode = calloc(1, sizeof(CommandList));
    newNode->command = newCommand;
    newNode->next = NULL;

    if (list == NULL) {
        return newNode;
    } else {
        CommandList *current = list;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
        return list;
    }
}

Decl* MakePreBuildDecl(CommandList* commands){ 
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Decl * decl = calloc(1, sizeof(Decl));
    decl->type = PRE_BUILD_DECL;
    // Assign to the correct union member defined in AbstractSyntaxTree.h
    decl->preBuildCommands.commands = commands;
    return decl;
}


Decl* MakePostBuildDecl(CommandList* commands){ 
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Decl * decl = calloc(1, sizeof(Decl));
    decl->type = POST_BUILD_DECL;
    // Assign to the correct union member defined in AbstractSyntaxTree.h
    decl->postBuildCommands.commands = commands;
    return decl;
}

Decl* MakeLogDecl(char* path, int mode){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Decl * decl = calloc(1, sizeof(Decl));
    decl->type = LOG_DECL;
    decl->logPath = path;
    decl->logMode = (LogMode)mode;
    return decl;
}

int MakeLogMode(int mode){
    _logSyntacticAnalyzerAction(__FUNCTION__);
    return mode;
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

Condition* MakeCondition(ConditionType type, char* phaseName) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Condition* c = calloc(1, sizeof(Condition));
    c->type = type;
    c->phaseName = phaseName;
    return c;
}

Decl* MakeConditionalDecl(Condition* cond, CommandList* cmds) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Decl* d = calloc(1, sizeof(Decl));
    d->type = CONDITIONAL_DECL;
    d->conditionalPhase.condition = cond;
    d->conditionalPhase.body = cmds;
    return d;
}

Decl* MakeCustomPhaseDecl(char* phaseName, CommandList* commands) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Decl* d = calloc(1, sizeof(Decl));
    d->type = CUSTOM_PHASE_DECL;
    d->customPhase.phaseName = phaseName;
    d->customPhase.commands = commands;
    return d;
}
