#include "SemanticAnalyzer.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownSemanticAnalyzerModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: SemanticAnalyzer...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeSemanticAnalyzerModule() {
	_logger = createLogger("SemanticAnalyzer");
	return _shutdownSemanticAnalyzerModule;
}

/** PRIVATE FUNCTIONS */

/**
 * Validates the program structure (exactly one PROJECT_DECL, at least one SRC_DECL with items).
 * Returns true if valid, false otherwise.
 */
static bool _validateProgramStructure(Program * program) {
	if (program == NULL) {
		logError(_logger, "Program is NULL.");
		return false;
	}
	
	// Validate exactly one PROJECT_DECL
	if (program->projectDecl == NULL) {
		logError(_logger, "Program must have exactly one project declaration.");
		return false;
	}
	
	if (program->projectDecl->type != PROJECT_DECL) {
		logError(_logger, "Project declaration has invalid type.");
		return false;
	}
	
	// Validate at least one SRC_DECL with items
	bool hasSources = false;
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl != NULL && it->decl->type == SRC_DECL) {
			if (it->decl->srcFiles != NULL) {
				// Check if the list has at least one item
				ItemList * items = it->decl->srcFiles;
				if (items != NULL && items->item != NULL) {
					hasSources = true;
					break;
				}
			}
		}
	}
	
	if (!hasSources) {
		logError(_logger, "Program must have at least one sources declaration with files.");
		return false;
	}
	
	return true;
}

/**
 * Validates a single command.
 * Returns true if the command is valid, false otherwise.
 */
static bool _validateCommand(Command * cmd) {
	if (cmd == NULL || cmd->commandLine == NULL) {
		return false;
	}
	
	char * line = cmd->commandLine;
	
	// Check if command starts with one of the allowed commands
	if (strncmp(line, "mkdir", 5) == 0) {
		return true;
	}
	if (strncmp(line, "rm", 2) == 0) {
		return true;
	}
	if (strncmp(line, "cp", 2) == 0) {
		return true;
	}
	if (strncmp(line, "mv", 2) == 0) {
		return true;
	}
	
	logError(_logger, "Unsupported command: %s", line);
	return false;
}

/**
 * Validates all commands in the program.
 * Returns true if all commands are valid, false otherwise.
 */
static bool _validateCommands(Program * program) {
	if (program == NULL) {
		return false;
	}
	
	// Validate commands in all sections that contain commands
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl == NULL) {
			continue;
		}
		
		CommandList * commands = NULL;
		
		switch (it->decl->type) {
			case PRE_BUILD_DECL:
				commands = it->decl->preBuildCommands.commands;
				break;
			case POST_BUILD_DECL:
				commands = it->decl->postBuildCommands.commands;
				break;
			case CUSTOM_PHASE_DECL:
				commands = it->decl->customPhase.commands;
				break;
			case CONDITIONAL_DECL:
				commands = it->decl->conditionalPhase.body;
				break;
			default:
				// No commands in this declaration type - this is expected and normal
				// Only log as warning if the type is truly unexpected (outside enum range)
				if (it->decl->type > CUSTOM_PHASE_DECL) {
					logWarning(_logger, "Unknown declaration type (%d) does not contain commands, skipping validation.", it->decl->type);
				}
				continue;
		}
		
		// Validate each command in the list
		for (CommandList * cmdList = commands; cmdList != NULL; cmdList = cmdList->next) {
			if (cmdList->command != NULL) {
				if (!_validateCommand(cmdList->command)) {
					return false;
				}
			}
		}
	}
	
	return true;
}

/**
 * Checks if a phase exists in the program.
 * Returns true if the phase exists, false otherwise.
 */
static bool _phaseExists(const char * phaseName, DeclList * sections) {
	if (phaseName == NULL) {
		return false;
	}
	
	for (DeclList * it = sections; it != NULL; it = it->next) {
		if (it->decl == NULL) {
			continue;
		}
		
		// Check pre_build
		if (it->decl->type == PRE_BUILD_DECL) {
			if (strcmp(phaseName, "pre_build") == 0) {
				return true;
			}
		}
		
		// Check post_build
		if (it->decl->type == POST_BUILD_DECL) {
			if (strcmp(phaseName, "post_build") == 0) {
				return true;
			}
		}
		
		// Check custom phases
		if (it->decl->type == CUSTOM_PHASE_DECL) {
			if (it->decl->customPhase.phaseName != NULL) {
				if (strcmp(phaseName, it->decl->customPhase.phaseName) == 0) {
					return true;
				}
			}
		}
	}
	
	return false;
}

/**
 * Validates phases and conditionals.
 * Returns true if all phases and conditionals are valid, false otherwise.
 */
static bool _validatePhases(Program * program) {
	if (program == NULL) {
		return false;
	}
	
	// Validate all conditionals reference existing phases
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl == NULL) {
			continue;
		}
		
		if (it->decl->type == CONDITIONAL_DECL) {
			if (it->decl->conditionalPhase.condition != NULL) {
				const char * phaseName = it->decl->conditionalPhase.condition->phaseName;
				
				// Validate that build and run cannot be referenced as phases
				if (phaseName != NULL) {
					if (strcmp(phaseName, "build") == 0 || strcmp(phaseName, "run") == 0) {
						logError(_logger, "Conditional cannot reference 'build' or 'run' as phases.");
						return false;
					}
				}
				
				// Validate that the phase exists
				if (!_phaseExists(phaseName, program->sections)) {
					logError(_logger, "Conditional references unknown phase '%s'.", phaseName);
					return false;
				}
			}
		}
	}
	
	return true;
}

/**
 * Validates logging configuration.
 * Returns true if logging is valid (if present), false otherwise.
 */
static bool _validateLogging(Program * program) {
	if (program == NULL) {
		return false;
	}
	
	// Find LOG_DECL in sections
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl == NULL) {
			continue;
		}
		
		if (it->decl->type == LOG_DECL) {
			// Validate mode is APPEND_MODE or OVERWRITE_MODE
			if (it->decl->logMode != APPEND_MODE && it->decl->logMode != OVERWRITE_MODE) {
				logError(_logger, "Log mode must be 'append' or 'overwrite'.");
				return false;
			}
			
			// Validate path is not NULL and not empty
			if (it->decl->logPath == NULL || strlen(it->decl->logPath) == 0) {
				logError(_logger, "Log path must be a non-empty string.");
				return false;
			}
		}
	}
	
	// If no LOG_DECL is present, that's fine (logging is optional)
	return true;
}

/**
 * Validates actions (build/run declarations).
 * Returns true if actions are valid, false otherwise.
 */
static bool _validateActions(Program * program) {
	if (program == NULL) {
		return false;
	}
	
	// Validate that at least one BUILD_DECL exists
	bool hasBuild = false;
	
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl == NULL) {
			continue;
		}
		
		if (it->decl->type == BUILD_DECL) {
			hasBuild = true;
			break;
		}
	}
	
	if (!hasBuild) {
		logError(_logger, "Program must have at least one build declaration.");
		return false;
	}
	
	// RUN_DECL is optional according to the specification, so we don't require it
	
	return true;
}

/** PUBLIC FUNCTIONS */

CompilationStatus executeSemanticAnalysis(CompilerState * compilerState) {
	logDebugging(_logger, "Starting semantic analysis...");
	
	Program * program = compilerState->abstractSyntaxtTree;
	if (program == NULL) {
		logError(_logger, "Program is NULL, cannot perform semantic analysis.");
		return FAILED;
	}
	
	// Validate program structure
	if (!_validateProgramStructure(program)) {
		logError(_logger, "Program structure validation failed.");
		return FAILED;
	}
	
	// Validate commands
	if (!_validateCommands(program)) {
		logError(_logger, "Command validation failed.");
		return FAILED;
	}
	
	// Validate phases
	if (!_validatePhases(program)) {
		logError(_logger, "Phase validation failed.");
		return FAILED;
	}
	
	// Validate logging
	if (!_validateLogging(program)) {
		logError(_logger, "Logging validation failed.");
		return FAILED;
	}
	
	// Validate actions
	if (!_validateActions(program)) {
		logError(_logger, "Action validation failed.");
		return FAILED;
	}
	
	logDebugging(_logger, "Semantic analysis completed successfully.");
	return SUCCEEDED;
}

