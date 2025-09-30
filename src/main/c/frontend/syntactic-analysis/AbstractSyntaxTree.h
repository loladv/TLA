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

typedef struct Decl Decl;
typedef struct DeclList DeclList;
typedef struct Item Item;
typedef struct ItemList ItemList;
typedef struct Program Program;

// Tipos de declaraciones
enum DeclType {
    PROJECT_DECL,    // project helloWorld
    SRC_DECL,        // src { main.c }
    FLAGS_DECL,      // flags { -O2, -Wall }
    BUILD_DECL,      // build
    RUN_DECL         // run
};

// Tipos de items 
enum ItemType {
    TEXT_ITEM        // Para archivos como "main.c"
};

// Estructura para declaraciones individuales
struct Decl {
    DeclType type;
    union {
        struct {
            char * projectName;        // PROJECT: "helloWorld"
        };
        struct {
            ItemList * srcFiles;      // SRC: { main.c }
        };
        struct {
            ItemList * flags;         // FLAGS: { -O2, -Wall }
        };
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


void destroyDecl(Decl * decl);
void destroyDeclList(DeclList * declList);
void destroyItem(Item * item);
void destroyItemList(ItemList * itemList);
void destroyProgram(Program * program);

#endif
