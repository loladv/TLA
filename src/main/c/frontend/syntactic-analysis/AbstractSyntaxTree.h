#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/* Forward declarations for recursive types */
typedef enum DeclType DeclType;
typedef enum ItemType ItemType;
typedef enum LogMode LogMode;
typedef enum CommandType CommandType;
typedef enum ConditionType ConditionType;


typedef struct Decl Decl;
typedef struct DeclList DeclList;
typedef struct Item Item;
typedef struct ItemList ItemList;
typedef struct Program Program;
typedef struct Command Command;
typedef struct CommandList CommandList;
typedef struct Condition Condition;

enum DeclType {
    PROJECT_DECL,
    SRC_DECL,
    COMPILER_DECL,
    LIBS_DECL,
    HEADERS_DECL,
    OUTPUT_DECL,
    PRE_BUILD_DECL,
    POST_BUILD_DECL,
    FLAGS_DECL,
    BUILD_DECL,
    RUN_DECL,
    LOG_DECL,
    CONDITIONAL_DECL,
    CUSTOM_PHASE_DECL
};

enum ItemType {
    TEXT_ITEM
};

enum LogMode {
    APPEND_MODE,
    OVERWRITE_MODE
};

enum CommandType {
    MKDIR_CMD,
    RM_CMD,
    CP_CMD,
    MV_CMD
};

enum ConditionType {
    CONDITION_FAIL,
    CONDITION_SUCCESS
};

struct Decl {
    DeclType type;
    union {
        struct {
            char * projectName;
        };
        struct {
            ItemList * srcFiles;
        };
        struct {
            char * compilerName;
        };
        struct {
            ItemList * libs;
        };
        struct {
            ItemList * headers;
        };
        struct {
            char * outputName;
        };
        struct {
            ItemList * flags;
        };
        struct {
            char * logPath;
            LogMode logMode;
        };
        struct {
            CommandList *commands;
        } preBuildCommands;        

        struct {
            CommandList *commands;
        } postBuildCommands;       

        struct {
            Condition* condition;
            CommandList* body;
        } conditionalPhase;       

        struct {
            char* phaseName;
            CommandList* commands;
        } customPhase;            

        /* BUILD and RUN share the union but have no extra data */
    };
};

struct DeclList {
    Decl * decl;
    DeclList * next;
};

struct Item {
    ItemType type;
    char * text;
};

struct ItemList {
    Item * item;
    ItemList * next;
};

struct Program {
    Decl * projectDecl;
    DeclList * sections;
};

struct Command {
    CommandType type;
    char* commandLine;
};

struct CommandList {
    Command *command;
    CommandList *next;
};

struct Condition {
    ConditionType type;
    char* phaseName;
};

void destroyDecl(Decl * decl);
void destroyDeclList(DeclList * declList);
void destroyItem(Item * item);
void destroyItemList(ItemList * itemList);
void destroyProgram(Program * program);
void destroyCommand(Command *command);
void destroyCommandList(CommandList *commandList);
void destroyCondition(Condition* condition);

#endif
