#include "GlobExpander.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownGlobExpanderModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: GlobExpander...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGlobExpanderModule() {
	_logger = createLogger("GlobExpander");
	return _shutdownGlobExpanderModule;
}

/** PRIVATE FUNCTIONS */

/**
 * Checks if a text string is a glob pattern.
 * Returns true if it contains glob characters (*, ?, [).
 */
static bool _isGlobPattern(const char * text) {
	// TODO: Implement glob pattern detection (Fase 3.2)
	return false;
}

/**
 * Expands a single glob pattern to a list of items.
 * Returns the expanded item list, or NULL on error.
 */
static ItemList * _expandGlobPattern(const char * pattern) {
	// TODO: Implement glob expansion (Fase 3.3)
	return NULL;
}

/**
 * Expands all glob patterns in the program AST.
 * Returns true if expansion succeeds, false otherwise.
 */
static bool _expandGlobPatternsInProgram(Program * program) {
	// TODO: Implement glob expansion in AST (Fase 3.4)
	return true;
}

/** PUBLIC FUNCTIONS */

CompilationStatus expandGlobPatterns(CompilerState * compilerState) {
	logDebugging(_logger, "Starting glob pattern expansion...");
	
	Program * program = compilerState->abstractSyntaxtTree;
	if (program == NULL) {
		logError(_logger, "Program is NULL, cannot expand glob patterns.");
		return FAILED;
	}
	
	if (!_expandGlobPatternsInProgram(program)) {
		logError(_logger, "Glob pattern expansion failed.");
		return FAILED;
	}
	
	logDebugging(_logger, "Glob pattern expansion completed successfully.");
	return SUCCEEDED;
}

