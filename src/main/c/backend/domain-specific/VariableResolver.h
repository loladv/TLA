#ifndef VARIABLE_RESOLVER_HEADER
#define VARIABLE_RESOLVER_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdbool.h>

/** Initialize module's internal state. */
ModuleDestructor initializeVariableResolverModule();

/**
 * Resolves all variables in the program, detects cycles, and expands references.
 * Returns SUCCEEDED if all variables can be resolved, FAILED otherwise.
 */
CompilationStatus resolveVariables(CompilerState * compilerState);

#endif

