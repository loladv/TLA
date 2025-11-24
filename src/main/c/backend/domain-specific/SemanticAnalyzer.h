#ifndef SEMANTIC_ANALYZER_HEADER
#define SEMANTIC_ANALYZER_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdbool.h>

/** Initialize module's internal state. */
ModuleDestructor initializeSemanticAnalyzerModule();

/**
 * Executes semantic analysis on the program in the compiler state.
 * Returns SUCCEEDED if all validations pass, FAILED otherwise.
 */
CompilationStatus executeSemanticAnalysis(CompilerState * compilerState);

#endif

