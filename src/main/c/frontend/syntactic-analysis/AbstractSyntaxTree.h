#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * This type definitions allows self-referencing types (e.g., an expression
 * that is made of another expressions, such as talking about you in 3rd
 * person, but without the madness).
 */

 /*

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Program Program;

/*

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

 /*
enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

struct Program {
	Expression * expression;
};

*/

/**
 * Node recursive super-duper-trambolik-destructors.
 */

 /*
void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyProgram(Program * program);
*/

//TODO: Completar con lo que falta

// Declaraciones forward para tipos recursivos
typedef enum DeclType DeclType;
typedef enum ItemType ItemType;
typedef enum LogMode LogMode;
typedef enum CommandType CommandType;


typedef struct Decl Decl;
typedef struct DeclList DeclList;
typedef struct Item Item;
typedef struct ItemList ItemList;
typedef struct Program Program;
typedef struct Command Command;
typedef struct CommandList CommandList;

// Tipos de declaraciones
enum DeclType {
    PROJECT_DECL,    // project helloWorld
    SRC_DECL,        // sources { main.c }
    COMPILER_DECL,   // compiler clang
    LIBS_DECL,       // libraries { m pthread }
    HEADERS_DECL,    // headers { include/ }
    OUTPUT_DECL,     // output myapp
    PRE_BUILD_DECL,  // pre_build { cmds }
    POST_BUILD_DECL, // post_build { cmds }
    FLAGS_DECL,      // flags { -O2, -Wall }
    BUILD_DECL,      // build
    RUN_DECL,        // run
    LOG_DECL         // log { path "file.log"; mode append }
};

// Tipos de items 
enum ItemType {
    TEXT_ITEM        // Para archivos como "main.c"
};

// Tipos de modo de log
enum LogMode {
    APPEND_MODE,     // append
    OVERWRITE_MODE   // overwrite
};

enum CommandType {
    MKDIR_CMD,
    RM_CMD,
    CP_CMD,
    MV_CMD
};

// Estructura para declaraciones individuales
struct Decl {
    DeclType type;
    union {
        struct {
            char * projectName;        // PROJECT: "helloWorld"
        };
        struct {
            ItemList * srcFiles;      // SOURCES: { main.c }
        };
        struct {
            char * compilerName;      // COMPILER: clang/gcc
        };
        struct {
            ItemList * libs;          // LIBRARIES: { m pthread }
        };
        struct {
            ItemList * headers;       // HEADERS: { include/ }
        };
        struct {
            char * outputName;        // OUTPUT: myapp
        };
        struct {
            ItemList * flags;         // FLAGS: { -O2, -Wall }
        };
        struct {
            char * logPath;          // LOG: path
            LogMode logMode;         // LOG: mode (append/overwrite)
        };
        struct {
            CommandList *commands;  // PRE_BUILD: { mkdir build }
        } preBuildCommands;        

        struct {
            CommandList *commands;  // POST_BUILD: { cp a.out bin/ }
        } postBuildCommands;       
        // BUILD y RUN no necesitan datos adicionales
    };
};

// Lista de declaraciones
struct DeclList {
    Decl * decl;
    DeclList * next;
};

// Item individual (archivo fuente)
struct Item {
    ItemType type;
    char * text;        // Nombre del archivo: "main.c"
};

// Lista de items (archivos fuente)
struct ItemList {
    Item * item;
    ItemList * next;
};

// Programa MakeLite-C
struct Program {
    Decl * projectDecl;    // Declaración del proyecto
    DeclList * sections;   // Lista de secciones (src, build, run)
};

// Estructura para un comando individual
struct Command {
    CommandType type;
    ItemList *args; 
};

// Lista de comandos
struct CommandList {
    Command *command;
    CommandList *next;
};

void destroyDecl(Decl * decl);
void destroyDeclList(DeclList * declList);
void destroyItem(Item * item);
void destroyItemList(ItemList * itemList);
void destroyProgram(Program * program);
void destroyCommand(Command *command);
void destroyCommandList(CommandList *commandList);

#endif
