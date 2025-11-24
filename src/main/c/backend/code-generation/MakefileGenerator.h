#ifndef MAKEFILE_GENERATOR_HEADER
#define MAKEFILE_GENERATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdbool.h>

/** Initialize module's internal state. */
ModuleDestructor initializeMakefileGeneratorModule();

/**
 * Generates a Makefile from the program in the compiler state.
 * Returns SUCCEEDED if generation succeeds, FAILED otherwise.
 */
CompilationStatus executeMakefileGenerator(CompilerState * compilerState);

#endif

