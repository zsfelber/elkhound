#ifndef AST_H
#define AST_H

#include "storage.h"
#include "srcloc.h"
#include "astlist.h"

class GrammarLexer;
class TF_class;
class ListClass;

extern str::StoragePool astgen_pool;
extern SourceLocManager *_mgr;

extern SObjList<TF_class> *_allClasses;
extern ASTList<ListClass> *_listClasses;

extern GrammarLexer *lexer;

#endif // AST_H

