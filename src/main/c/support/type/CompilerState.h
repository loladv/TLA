#ifndef COMPILER_STATE_HEADER
#define COMPILER_STATE_HEADER

typedef struct Program Program;

/**
 * Global compiler state shared across phases.
 */
typedef struct {
	Program * abstractSyntaxtTree;
} CompilerState;

#endif
