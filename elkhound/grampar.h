// grampar.h            see license.txt for copyright and terms of use
// declarations for bison-generated grammar parser

#ifndef __GRAMPAR_H
#define __GRAMPAR_H

#include "typ.h"          // NULL
#include "sobjlist.h"     // SObjList
#include "exc.h"          // xBase
#include "strsobjdict.h"  // StringSObjDict
#include "locstr.h"       // LocString
#include <sstream>
#include <map>
#include "gramast.ast.gen.h" // grammar AST nodes

// linkdepend: grampar.tab.cc

// fwd decl
class GrammarAST;         // gramast.ast
class TF_nonterm;         // gramast.ast
class GrammarLexer;       // ../ast/gramlex.h
class StringTable;        // strtable.h


// -------- rest of the program's view of parser ------------
// name of extra parameter to yyparse (i.e. the context in
// which the parser operates, instead of that being stored
// in some collection of globals)
// removed -> %parse-param %lex-param %param https://lwn.net/Articles/561990/
// #define YYPARSE_PARAM parseParam

// type of thing extra param points at
struct ParseParams {
  GrammarAST *treeTop;    // set when parsing finishes; AST tree top
  GrammarLexer &lexer;    // lexer we're using

public:
  ParseParams(GrammarLexer &L) :
    treeTop(NULL),
    lexer(L)
  {}
};

// caller interface to Bison-generated parser; starts parsing
// (whatever stream lexer is reading) and returns 0 for success and
// 1 for error; the extra parameter is available to actions to use
int grampar_yyparse(void *YYPARSE_PARAM);

// when this is set to true, bison parser emits info about
// actions as it's taking them (shared by all instances of
// bison-generated parsers in a given program)
extern int yydebug;


// ---------- Bison's view of the rest of the program --------
// Bison calls this to get each token; returns token code,
// or 0 for eof; semantic value for returned token can be
// put into '*lvalp'
// TODO: Paul Hilfinger reports there's a problem saying "union
// YYSTYPE"; he's using bison 1.34 I think, so I need to upgrade
// and see what the problem is (suspect my 'sed' pattern isn't
// matching, in the Makefile)
int grampar_yylex(union YYSTYPE *lvalp, void *parseParam);

// error printer
void grampar_yyerror(char const *message, void *parseParam);


// ---------------- grampar's parsing structures ---------------
class Grammar;    // fwd


// only the one in the topmost environment is used
struct EnvironmentBuffer {
    // count of recoverable errors
    int errors = 0;
    LocString * startSymbol = NULL;
    LocString * startLexer = NULL;
    std::map<std::string, ProdDecl*> parserFuncs;
    std::stringstream bufIncl, bufHead, bufConsBase, bufHeadFun, bufCc;
};

// while walking the AST, we do a kind of recursive evaluation
// to handle things like inherited actions and self-updating
// (eval'd at grammar parse time) action expressions
class Environment {

public:      // data
  // grammar we're playing with (stored here because it's
  // more convenient than passing it to every fn separately)
  Grammar &g;

  // env in which we're nested, if any
  Environment *prevEnv;      // (serf)

  // maps from a nonterminal name to its declaration, if that
  // nonterminal has in fact been declared already
  StringSObjDict<TF_nonterm> nontermDecls;

  // only the one in the topmost environment is used
  EnvironmentBuffer * buffer;

  // reference to the one we're really using
  int& errors;
  LocString * const startSymbol;
  LocString * const startLexer;
  std::map<std::string, ProdDecl*>& parserFuncs;
  std::stringstream &bufIncl, &bufHead, &bufConsBase, &bufHeadFun, &bufCc;

public:
  Environment(Grammar &g);             // new env
  Environment(Environment &prevEnv);   // nested env
  Environment(Environment &prevEnv, Grammar &g);   // nested env
  ~Environment();
};


// --------------- grampar's external interface -----------
// parse grammar file 'fname' into grammar 'g', throwing exceptions
// if there are problems
void readGrammarFile(Environment &env, rostring fname);

// just do the parsing stage
GrammarAST *parseGrammarFile(rostring fname, bool useML);

// merge two grammar descriptions; neither argument is consumed,
// but subtrees of the 2nd argument get moved into the first tree
void mergeGrammar(GrammarAST *base, GrammarAST *ext);

void setAnnotations(GrammarAST *ast);

// GrammarAST -> Grammar
void parseGrammarAST(Environment &g, GrammarAST *treeTop, TermDecl const *& eof);
void synthesizeStartRule(Environment &env, GrammarAST *ast, TermDecl const *eof, int &multiIndex, LocString *& grType, std::string &name, std::string &usr);

class Nonterminal;
Nonterminal * complementNonterm(Environment &env, GrammarAST *ast, TF_nonterm * nt, int ntIndex, TermDecl const *eof);

void astParseError(rostring msg);

// thrown when there is an error parsing the AST
class XASTParse : public xBase {
public:    // data
  // token at or near failure
  LocString failToken;

  // what is wrong
  string message;

private:   // funcs
  static string constructMsg(LocString const &tok, rostring msg);

public:    // funcs
  XASTParse(LocString const &tok, rostring msg);
  XASTParse(XASTParse const &obj);
  ~XASTParse();
};


#endif // __GRAMPAR_H
