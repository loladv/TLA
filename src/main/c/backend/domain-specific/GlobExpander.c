#include "GlobExpander.h"
#include <glob.h>
#include <stdlib.h>
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownGlobExpanderModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: GlobExpander...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGlobExpanderModule() {
	_logger = createLogger("GlobExpander");
	return _shutdownGlobExpanderModule;
}

/** PRIVATE FUNCTIONS */

/**
 * Checks if a text string is a glob pattern.
 * Returns true if it contains glob characters (*, ?, [).
 */
static bool _isGlobPattern(const char * text) {
	if (text == NULL) {
		return false;
	}
	
	return strchr(text, '*') != NULL
		|| strchr(text, '?') != NULL
		|| strchr(text, '[') != NULL;
}

/**
 * Expands a single glob pattern to a list of items.
 * Returns the expanded item list, or NULL on error.
 */
static ItemList * _expandGlobPattern(const char * pattern) {
	if (pattern == NULL) {
		return NULL;
	}
	
	glob_t globResult;
	memset(&globResult, 0, sizeof(glob_t));
	
	int ret = glob(pattern, GLOB_TILDE, NULL, &globResult);
	if (ret != 0) {
		if (ret == GLOB_NOMATCH) {
			logError(_logger, "Glob pattern matches no files: %s", pattern);
		} else {
			logError(_logger, "Error expanding glob pattern '%s' (code %d).", pattern, ret);
		}
		
		globfree(&globResult);
		return NULL;
	}
	
	ItemList * head = NULL;
	ItemList * tail = NULL;
	
	for (size_t i = 0; i < globResult.gl_pathc; i++) {
		if (globResult.gl_pathv[i] == NULL) {
			continue;
		}
		
		ItemList * node = calloc(1, sizeof(ItemList));
		node->item = calloc(1, sizeof(Item));
		node->item->type = TEXT_ITEM;
		node->item->text = strdup(globResult.gl_pathv[i]);
		node->next = NULL;
		
		if (head == NULL) {
			head = node;
			tail = node;
		} else {
			tail->next = node;
			tail = node;
		}
	}
	
	globfree(&globResult);
	return head;
}

/**
 * Expands all glob patterns in the program AST.
 * Returns true if expansion succeeds, false otherwise.
 */
static bool _expandGlobPatternsInProgram(Program * program) {
	if (program == NULL || program->sections == NULL) {
		return true;
	}
	
	for (DeclList * it = program->sections; it != NULL; it = it->next) {
		if (it->decl == NULL) {
			continue;
		}
		
		ItemList ** targetList = NULL;
		
		switch (it->decl->type) {
			case SRC_DECL:
				targetList = &it->decl->srcFiles;
				break;
			case FLAGS_DECL:
				targetList = &it->decl->flags;
				break;
			case LIBS_DECL:
				targetList = &it->decl->libs;
				break;
			case HEADERS_DECL:
				targetList = &it->decl->headers;
				break;
			default:
				break;
		}
		
		if (targetList == NULL || *targetList == NULL) {
			continue;
		}
		
		bool requiresExpansion = false;
		for (ItemList * itemIt = *targetList; itemIt != NULL; itemIt = itemIt->next) {
			if (itemIt->item != NULL && _isGlobPattern(itemIt->item->text)) {
				requiresExpansion = true;
				break;
			}
		}
		
		if (!requiresExpansion) {
			continue;
		}
		
		ItemList * expandedHead = NULL;
		ItemList * expandedTail = NULL;
		
		for (ItemList * itemIt = *targetList; itemIt != NULL; itemIt = itemIt->next) {
			if (itemIt->item == NULL || itemIt->item->text == NULL) {
				continue;
			}
			
			if (_isGlobPattern(itemIt->item->text)) {
				ItemList * expanded = _expandGlobPattern(itemIt->item->text);
				if (expanded == NULL) {
					if (expandedHead != NULL) {
						destroyItemList(expandedHead);
					}
					return false;
				}
				
				if (expandedHead == NULL) {
					expandedHead = expanded;
					expandedTail = expanded;
				} else {
					expandedTail->next = expanded;
				}
				
				while (expandedTail->next != NULL) {
					expandedTail = expandedTail->next;
				}
			} else {
				ItemList * copyNode = calloc(1, sizeof(ItemList));
				copyNode->item = calloc(1, sizeof(Item));
				copyNode->item->type = itemIt->item->type;
				if (itemIt->item->text != NULL) {
					copyNode->item->text = strdup(itemIt->item->text);
				}
				copyNode->next = NULL;
				
				if (expandedHead == NULL) {
					expandedHead = copyNode;
					expandedTail = copyNode;
				} else {
					expandedTail->next = copyNode;
					expandedTail = copyNode;
				}
			}
		}
		
		if (expandedHead == NULL) {
			logError(_logger, "Glob expansion for declaration resulted in empty list.");
			return false;
		}
		
		destroyItemList(*targetList);
		*targetList = expandedHead;
	}
	
	return true;
}

/** PUBLIC FUNCTIONS */

CompilationStatus expandGlobPatterns(CompilerState * compilerState) {
	logDebugging(_logger, "Starting glob pattern expansion...");
	
	Program * program = compilerState->abstractSyntaxtTree;
	if (program == NULL) {
		logError(_logger, "Program is NULL, cannot expand glob patterns.");
		return FAILED;
	}
	
	if (!_expandGlobPatternsInProgram(program)) {
		logError(_logger, "Glob pattern expansion failed.");
		return FAILED;
	}
	
	logDebugging(_logger, "Glob pattern expansion completed successfully.");
	return SUCCEEDED;
}

