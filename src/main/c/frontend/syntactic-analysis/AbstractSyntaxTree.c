#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */
/*
void destroyConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void destroyExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case FACTOR:
				destroyFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void destroyFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				destroyConstant(factor->constant);
				break;
			case EXPRESSION:
				destroyExpression(factor->expression);
				break;
		}
		free(factor);
	}
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroyExpression(program->expression);
		free(program);
	}
}
*/

void destroyItem(Item * item) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (item != NULL) {
        if (item->text != NULL) {
            free(item->text);
        }
        free(item);
    }
}

void destroyItemList(ItemList * itemList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (itemList != NULL) {
        if (itemList->item != NULL) {
            destroyItem(itemList->item);
        }
        if (itemList->next != NULL) {
            destroyItemList(itemList->next);
        }
        free(itemList);
    }
}

void destroyCommand(Command *command) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (command != NULL) {
        if (command->commandLine != NULL) {
            free(command->commandLine);
        }
        free(command);
    }
}

void destroyCommandList(CommandList *commandList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (commandList != NULL) {
        if (commandList->command != NULL) {
            destroyCommand(commandList->command);
        }
        if (commandList->next != NULL) {
            destroyCommandList(commandList->next);
        }
        free(commandList);
    }
}

void destroyCondition(Condition* condition) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (condition != NULL) {
        if (condition->phaseName != NULL) {
            free(condition->phaseName);
        }
        free(condition);
    }
}

void destroyDecl(Decl * decl) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (decl != NULL) {
        switch (decl->type) {
            case PROJECT_DECL:
                if (decl->projectName != NULL) {
                    free(decl->projectName);
                }
                break;
            case SRC_DECL:
                destroyItemList(decl->srcFiles);
                break;
            case COMPILER_DECL:
                if (decl->compilerName != NULL) {
                    free(decl->compilerName);
                }
                break;
            case LIBS_DECL:
                destroyItemList(decl->libs);
                break;
            case HEADERS_DECL:
                destroyItemList(decl->headers);
                break;
            case OUTPUT_DECL:
                if (decl->outputName != NULL) {
                    free(decl->outputName);
                }
                break;
            case FLAGS_DECL:
                destroyItemList(decl->flags);
                break;
            case PRE_BUILD_DECL:
                destroyCommandList(decl->preBuildCommands.commands); 
                break;
            case POST_BUILD_DECL:
                destroyCommandList(decl->postBuildCommands.commands); 
                break;
            case BUILD_DECL:
            case RUN_DECL:
                // No hay datos adicionales que liberar
                break;
            case LOG_DECL:
                if (decl->logPath != NULL) {
                    free(decl->logPath);
                }
                // logMode is an enum, no need to free
                break;
            case CONDITIONAL_DECL:
                destroyCondition(decl->conditionalPhase.condition);
                destroyCommandList(decl->conditionalPhase.body);
                break;
            case CUSTOM_PHASE_DECL:
                if (decl->customPhase.phaseName != NULL) {
                    free(decl->customPhase.phaseName);
                }
                destroyCommandList(decl->customPhase.commands);
                break;
        }
        free(decl);
    }
}

void destroyDeclList(DeclList * declList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (declList != NULL) {
        if (declList->decl != NULL) {
            destroyDecl(declList->decl);
        }
        if (declList->next != NULL) {
            destroyDeclList(declList->next);
        }
        free(declList);
    }
}

void destroyProgram(Program * program) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (program != NULL) {
        if (program->projectDecl != NULL) {
            destroyDecl(program->projectDecl);
        }
        if (program->sections != NULL) {
            destroyDeclList(program->sections);
        }
        free(program);
    }
}
