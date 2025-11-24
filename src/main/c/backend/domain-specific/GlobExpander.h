#ifndef GLOB_EXPANDER_HEADER
#define GLOB_EXPANDER_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdbool.h>

/** Initialize module's internal state. */
ModuleDestructor initializeGlobExpanderModule();

/**
 * Expands all glob patterns in the program.
 * Returns SUCCEEDED if all globs can be expanded, FAILED otherwise.
 */
CompilationStatus expandGlobPatterns(CompilerState * compilerState);

#endif

