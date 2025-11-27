#include "MakefileGenerator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

typedef struct {
	bool enabled;
	LogMode mode;
	const char * path;
} LoggingConfig;

/** Shutdown module's internal state. */
void _shutdownMakefileGeneratorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: MakefileGenerator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeMakefileGeneratorModule() {
	_logger = createLogger("MakefileGenerator");
	return _shutdownMakefileGeneratorModule;
}

/** PRIVATE FUNCTIONS */

/**
 * Finds a declaration of a specific type in the program sections.
 * Returns the first matching declaration, or NULL if not found.
 */
static Decl * _findDecl(Program * program, DeclType type) {
	if (program == NULL || program->sections == NULL) {
		return NULL;
	}
	
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl != NULL && it->decl->type == type) {
			return it->decl;
		}
	}
	
	return NULL;
}

/**
 * Returns the logging configuration for the program, if any.
 */
static LoggingConfig _getLoggingConfig(Program * program) {
	LoggingConfig config = { false, APPEND_MODE, NULL };
	Decl * logDecl = _findDecl(program, LOG_DECL);
	if (logDecl != NULL && logDecl->logPath != NULL) {
		config.enabled = true;
		config.mode = logDecl->logMode;
		config.path = logDecl->logPath;
	}
	return config;
}

/**
 * Prints an ItemList to the output file, joining items with spaces.
 */
static void _printItemList(ItemList * items, FILE * output) {
	if (items == NULL) {
		return;
	}
	
	bool first = true;
	for (ItemList * it = items; it != NULL; it = it->next) {
		if (it->item != NULL && it->item->text != NULL) {
			if (!first) {
				fprintf(output, " ");
			}
			fprintf(output, "%s", it->item->text);
			first = false;
		}
	}
}

/**
 * Writes a command line with optional logging redirection and terminator.
 */
static void _writeCommand(FILE * output, const char * indent, const char * commandLine, LoggingConfig logging, const char * terminator) {
	if (output == NULL || indent == NULL || commandLine == NULL || terminator == NULL) {
		return;
	}
	
	fprintf(output, "%s@%s", indent, commandLine);
	
	if (logging.enabled && logging.path != NULL) {
		if (logging.mode == APPEND_MODE) {
			fprintf(output, " >> $(LOG_FILE) 2>&1");
		} else {
			fprintf(output, " > $(LOG_FILE) 2>&1");
		}
	}
	
	fprintf(output, "%s", terminator);
}

/**
 * Generates Makefile variables section.
 */
static void _generateVariables(Program * program, FILE * output) {
	logDebugging(_logger, "Generating variables section...");
	
	// Generate CC (compiler)
	Decl * compilerDecl = _findDecl(program, COMPILER_DECL);
	if (compilerDecl != NULL && compilerDecl->compilerName != NULL) {
		fprintf(output, "CC = %s\n", compilerDecl->compilerName);
	} else {
		// Default compiler
		fprintf(output, "CC = gcc\n");
	}
	
	// Generate CFLAGS
	Decl * flagsDecl = _findDecl(program, FLAGS_DECL);
	if (flagsDecl != NULL && flagsDecl->flags != NULL) {
		fprintf(output, "CFLAGS = ");
		_printItemList(flagsDecl->flags, output);
		fprintf(output, "\n");
	} else {
		fprintf(output, "CFLAGS = \n");
	}
	
	// Generate SOURCES
	Decl * srcDecl = _findDecl(program, SRC_DECL);
	if (srcDecl != NULL && srcDecl->srcFiles != NULL) {
		fprintf(output, "SOURCES = ");
		_printItemList(srcDecl->srcFiles, output);
		fprintf(output, "\n");
	} else {
		fprintf(output, "SOURCES = \n");
	}
	
	// Generate INCLUDES (from headers)
	Decl * headersDecl = _findDecl(program, HEADERS_DECL);
	if (headersDecl != NULL && headersDecl->headers != NULL) {
		fprintf(output, "INCLUDES = ");
		bool first = true;
		for (ItemList * it = headersDecl->headers; it != NULL; it = it->next) {
			if (it->item != NULL && it->item->text != NULL) {
				if (!first) {
					fprintf(output, " ");
				}
				fprintf(output, "-I%s", it->item->text);
				first = false;
			}
		}
		fprintf(output, "\n");
	} else {
		fprintf(output, "INCLUDES = \n");
	}
	
	// Generate LIBS (from libraries)
	Decl * libsDecl = _findDecl(program, LIBS_DECL);
	if (libsDecl != NULL && libsDecl->libs != NULL) {
		fprintf(output, "LIBS = ");
		bool first = true;
		for (ItemList * it = libsDecl->libs; it != NULL; it = it->next) {
			if (it->item != NULL && it->item->text != NULL) {
				if (!first) {
					fprintf(output, " ");
				}
				fprintf(output, "-l%s", it->item->text);
				first = false;
			}
		}
		fprintf(output, "\n");
	} else {
		fprintf(output, "LIBS = \n");
	}
	
	// Generate OUTPUT
	Decl * outputDecl = _findDecl(program, OUTPUT_DECL);
	if (outputDecl != NULL && outputDecl->outputName != NULL) {
		fprintf(output, "OUTPUT = %s\n", outputDecl->outputName);
	} else {
		// Default output name from project
		if (program->projectDecl != NULL && program->projectDecl->projectName != NULL) {
			fprintf(output, "OUTPUT = %s\n", program->projectDecl->projectName);
		} else {
			fprintf(output, "OUTPUT = a.out\n");
		}
	}
	
	// Generate LOG_FILE
	Decl * logDecl = _findDecl(program, LOG_DECL);
	if (logDecl != NULL && logDecl->logPath != NULL) {
		fprintf(output, "LOG_FILE = %s\n", logDecl->logPath);
	}
	
	fprintf(output, "\n");
}

/**
 * Generates commands for a CommandList.
 */
static void _generateCommands(CommandList * commands, FILE * output, LoggingConfig logging) {
	if (commands == NULL) {
		return;
	}
	
	for (CommandList * it = commands; it != NULL; it = it->next) {
		if (it->command != NULL && it->command->commandLine != NULL) {
			_writeCommand(output, "\t", it->command->commandLine, logging, "\n");
		}
	}
}

/**
 * Generates Makefile targets section.
 */
static void _generateTargets(Program * program, FILE * output) {
	logDebugging(_logger, "Generating targets section...");
	
	LoggingConfig logging = _getLoggingConfig(program);
	
	// Generate .PHONY declaration
	fprintf(output, ".PHONY: pre_build build post_build run\n\n");
	
	// Generate pre_build target
	Decl * preBuildDecl = _findDecl(program, PRE_BUILD_DECL);
	if (preBuildDecl != NULL && preBuildDecl->preBuildCommands.commands != NULL) {
		fprintf(output, "pre_build:\n");
		_generateCommands(preBuildDecl->preBuildCommands.commands, output, logging);
		fprintf(output, "\n");
	} else {
		fprintf(output, "pre_build:\n");
		_writeCommand(output, "\t", "echo \"Pre-build phase\"", logging, "\n\n");
	}
	
	// Generate build target
	bool hasBuild = false;
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl != NULL && it->decl->type == BUILD_DECL) {
			hasBuild = true;
			break;
		}
	}
	
	if (hasBuild) {
		fprintf(output, "build: pre_build\n");
		_writeCommand(output, "\t", "$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT) $(SOURCES) $(LIBS)", logging, "\n\n");
	}
	
	// Generate post_build target
	Decl * postBuildDecl = _findDecl(program, POST_BUILD_DECL);
	if (postBuildDecl != NULL && postBuildDecl->postBuildCommands.commands != NULL) {
		fprintf(output, "post_build: build\n");
		_generateCommands(postBuildDecl->postBuildCommands.commands, output, logging);
		fprintf(output, "\n");
	} else {
		fprintf(output, "post_build: build\n");
		_writeCommand(output, "\t", "echo \"Post-build phase\"", logging, "\n\n");
	}
	
	// Generate run target
	bool hasRun = false;
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl != NULL && it->decl->type == RUN_DECL) {
			hasRun = true;
			break;
		}
	}
	
	if (hasRun) {
		fprintf(output, "run: post_build\n");
		_writeCommand(output, "\t", "./$(OUTPUT)", logging, "\n\n");
	}
}

/**
 * Generates Makefile conditionals section.
 */
static void _generateConditionals(Program * program, FILE * output) {
	logDebugging(_logger, "Generating conditionals section...");
	
	if (program == NULL || output == NULL) {
		return;
	}
	
	LoggingConfig logging = _getLoggingConfig(program);
	
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl == NULL || it->decl->type != CONDITIONAL_DECL) {
			continue;
		}
		
		Condition * condition = it->decl->conditionalPhase.condition;
		CommandList * body = it->decl->conditionalPhase.body;
		
		if (condition == NULL || condition->phaseName == NULL) {
			continue;
		}
		
		const char * phaseName = condition->phaseName;
		const char * prefix = (condition->type == CONDITION_FAIL) ? "if_fail" : "if_success";
		const char * comparison = (condition->type == CONDITION_FAIL) ? "-ne" : "-eq";
		
		fprintf(output, ".PHONY: %s_%s\n", prefix, phaseName);
		fprintf(output, "%s_%s:\n", prefix, phaseName);
		fprintf(output, "\t@if [ $$? %s 0 ]; then \\\n", comparison);
		
		bool wroteBody = false;
		for (CommandList * cmdIt = body; cmdIt != NULL; cmdIt = cmdIt->next) {
			if (cmdIt->command == NULL || cmdIt->command->commandLine == NULL) {
				continue;
			}
			_writeCommand(output, "\t\t", cmdIt->command->commandLine, logging, "; \\\n");
			wroteBody = true;
		}
		
		if (!wroteBody) {
			char buffer[256];
			snprintf(buffer, sizeof(buffer), "echo \"Conditional %s_%s has no commands\"", prefix, phaseName);
			_writeCommand(output, "\t\t", buffer, logging, "; \\\n");
		}
		
		fprintf(output, "\tfi\n\n");
	}
}

/**
 * Generates Makefile custom phases section.
 */
static void _generateCustomPhases(Program * program, FILE * output) {
	logDebugging(_logger, "Generating custom phases section...");
	
	if (program == NULL || output == NULL) {
		return;
	}
	
	LoggingConfig logging = _getLoggingConfig(program);
	
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl == NULL || it->decl->type != CUSTOM_PHASE_DECL) {
			continue;
		}
		
		const char * phaseName = it->decl->customPhase.phaseName;
		if (phaseName == NULL) {
			continue;
		}
		
		fprintf(output, ".PHONY: %s\n", phaseName);
		fprintf(output, "%s:\n", phaseName);
		
		if (it->decl->customPhase.commands != NULL) {
			_generateCommands(it->decl->customPhase.commands, output, logging);
		} else {
			char buffer[256];
			snprintf(buffer, sizeof(buffer), "echo \"Custom phase %s has no commands\"", phaseName);
			_writeCommand(output, "\t", buffer, logging, "\n");
		}
		
		fprintf(output, "\n");
	}
}

/** PUBLIC FUNCTIONS */

CompilationStatus executeMakefileGenerator(CompilerState * compilerState) {
	logDebugging(_logger, "Starting Makefile generation...");
	
	Program * program = compilerState->abstractSyntaxtTree;
	if (program == NULL) {
		logError(_logger, "Program is NULL, cannot generate Makefile.");
		return FAILED;
	}
	
	// Open Makefile for writing
	FILE * makefile = fopen("Makefile", "w");
	if (makefile == NULL) {
		logError(_logger, "Cannot open Makefile for writing.");
		return FAILED;
	}
	
	// Generate sections
	_generateVariables(program, makefile);
	_generateTargets(program, makefile);
	_generateConditionals(program, makefile);
	_generateCustomPhases(program, makefile);
	
	fclose(makefile);
	logDebugging(_logger, "Makefile generation completed successfully.");
	return SUCCEEDED;
}

