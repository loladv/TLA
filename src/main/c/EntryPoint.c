#include "backend/domain-specific/SemanticAnalyzer.h"
#include "backend/domain-specific/VariableResolver.h"
#include "backend/domain-specific/GlobExpander.h"
#include "backend/code-generation/MakefileGenerator.h"

#include "frontend/Frontend.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "support/logging/Logger.h"
#include "support/type/CompilationStatus.h"
#include "support/type/CompilerState.h"
#include "support/type/ModuleDestructor.h"

/** Main entry point of the compiler. */
const int main(const int length, const char ** arguments) {
	LexicalAnalyzer * lexicalAnalyzer = createLexicalAnalyzer();
	Logger * logger = createLogger("EntryPoint");
	for (int k = 0; k < length; ++k) {
		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}
	CompilerState compilerState = {
		.abstractSyntaxtTree = NULL
	};
	ModuleDestructor moduleDestructors[] = {
		initializeAbstractSyntaxTreeModule(),
		initializeFlexActionsModule(lexicalAnalyzer),
		initializeBisonActionsModule(&compilerState),
		initializeFrontendModule(lexicalAnalyzer),
		initializeSemanticAnalyzerModule(),
		initializeVariableResolverModule(),
		initializeGlobExpanderModule(),
		initializeMakefileGeneratorModule()
	};
	CompilationStatus compilationStatus = executeSyntacticAnalysis();
	Program * program = compilerState.abstractSyntaxtTree;
	if (compilationStatus == SUCCEEDED) {
		logDebugging(logger, "Starting semantic analysis...");
		compilationStatus = executeSemanticAnalysis(&compilerState);
		
		if (compilationStatus == SUCCEEDED) {
			logDebugging(logger, "Resolving variables...");
			compilationStatus = resolveVariables(&compilerState);
			
			if (compilationStatus == SUCCEEDED) {
				logDebugging(logger, "Expanding glob patterns...");
				compilationStatus = expandGlobPatterns(&compilerState);
				
				if (compilationStatus == SUCCEEDED) {
					logDebugging(logger, "Generating Makefile...");
					CompilationStatus genStatus = executeMakefileGenerator(&compilerState);
					if (genStatus != SUCCEEDED) {
						logError(logger, "Makefile generation failed.");
						compilationStatus = FAILED;
					}
				} else {
					logError(logger, "Glob pattern expansion failed.");
				}
			} else {
				logError(logger, "Variable resolution failed.");
			}
		} else {
			logError(logger, "Semantic analysis failed.");
		}
	}
	else {
		logError(logger, "The syntactic-analysis phase rejects the input program.");
		compilationStatus = FAILED;
	}
	logDebugging(logger, "Releasing AST resources...");
	destroyProgram(program);
	for (int k = (sizeof(moduleDestructors)/sizeof(ModuleDestructor)) - 1; 0 <= k; --k) {
		moduleDestructors[k]();
	}
	logDebugging(logger, "Compilation is done.");
	destroyLogger(logger);
	destroyLexicalAnalyzer(lexicalAnalyzer);
	return compilationStatus;
}
