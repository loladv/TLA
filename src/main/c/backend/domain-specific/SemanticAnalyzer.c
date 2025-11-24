#include "SemanticAnalyzer.h"

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
	// TODO: Implement validation logic (Fase 1.2)
	return true;
}

/**
 * Validates all commands in the program.
 * Returns true if all commands are valid, false otherwise.
 */
static bool _validateCommands(Program * program) {
	// TODO: Implement validation logic (Fase 1.3)
	return true;
}

/**
 * Validates phases and conditionals.
 * Returns true if all phases and conditionals are valid, false otherwise.
 */
static bool _validatePhases(Program * program) {
	// TODO: Implement validation logic (Fase 1.4)
	return true;
}

/**
 * Validates logging configuration.
 * Returns true if logging is valid (if present), false otherwise.
 */
static bool _validateLogging(Program * program) {
	// TODO: Implement validation logic (Fase 1.5)
	return true;
}

/**
 * Validates actions (build/run declarations).
 * Returns true if actions are valid, false otherwise.
 */
static bool _validateActions(Program * program) {
	// TODO: Implement validation logic (Fase 1.5)
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

