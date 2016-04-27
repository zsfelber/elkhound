// agrampar.cc            see license.txt for copyright and terms of use
// code for agrampar.h

#include "ast.h"        // this module
#include "agrampar.h"        // this module
#include "agrampar.tab.h"    // YYSTYPE union
#include "gramlex.h"         // GrammarLexer
#include "exc.h"             // xformat
#include "trace.h"           // tracing debugging functions
#include "owner.h"           // Owner
#include "strutil.h"         // trimWhitespace
#include "strtable.h"        // StringTable
#include "ast.h"

#include <string>          // strncmp
#include <ctype.h>           // isalnum
#include <fstream>         // std::ifstream

//extern
str::StoragePool y_pool(DBG_INFO_ARG0);

/*string unbox(string *s)
{
  string ret = *s;      // later optimization: transfer s.p directly
  delete s;
  return ret;
}*/

string const *box(char const *s)
{
  // return new string(DBG_INFO_ARG0_FIRST  s);
  return &stringTable.add(s);
}

string *appendStr(string const *left, string const *right)
{
  string *ret = new string(DBG_INFO_ARG0_FIRST  *left & *right);
  //delete left;
  //delete right;
  return ret;
}


CtorArg *parseCtorArg(DBG_INFO_FORMAL_FIRST  str::StoragePool const & pool, rostring origStr)
{
  CtorArg *ret = new (pool) CtorArg(DBG_INFO_ARG_FWD_FIRST  pool, false, "", "", "");

  // strip leading and trailing whitespace
  string str = trimWhitespace(origStr);

  // check for owner flag
  if (prefixEquals(str, "owner")) {
    ret->isOwner = true;
    str = str.substring(6, str.length() - 6);    // skip "owner "
  }

  // check for an initial value
  char const *equals = strchr(str.c_str(), '=');
  if (equals) {
    ret->defaultValue = equals+1;
    str = trimWhitespace(str.substring(0, equals-str.c_str()));
    trace("defaultValue") << "split into `" << str
                          << "' and `" << ret->defaultValue << "'\n";
  }

  // work from the right adge, collecting alphanumerics into the name;
  // this restricts the kinds of C type syntaxes we allow, but you can use
  // typedefs to express any type within these restrictions
  char const *start = str.c_str();
  char const *p = start + str.length() - 1;
  while ((isalnum(*p) || *p == '_') && p > start) {
    p--;
  }
  if (p == start) {
    xformat(stringc << "missing type specifier in \"" << origStr << "\"");
  }
  p++;

  ret->type = trimWhitespace(substring(start, p-start));
  ret->name = trimWhitespace(string(p));

  return ret;
}


// Bison parser calls this to get a token
int agrampar_yylex(YYSTYPE *lvalp, void *parseParam)
{
  ASTParseParams *par = (ASTParseParams*)parseParam;
  GrammarLexer &lexer = par->lexer;

  int code = lexer.yylexInc();

  // yield semantic values for some things
  switch (code) {
    case TOK_NAME:
    case TOK_INTLIT:
      lvalp->str = constcast(&lexer.curToken());
      break;

    case TOK_EMBEDDED_CODE:
      lvalp->str = constcast(&lexer.curFuncBody());
      break;

    default:
      lvalp->str = NULL;
  }

  static bool traceIt = tracingSys("tokens");
  if (traceIt) {
    std::ostream &os = trace("tokens");
    os << lexer.curLocStr() << ": " << code;
    if (lvalp->str) {
      os << ", \"" << *(lvalp->str) << "\"";
    }
    os << "\n";
  }

  return code;
}


void agrampar_yyerror(char const *msg, void *parseParam)
{
  ((ASTParseParams*)parseParam)->lexer.err(msg);
}


// ---------------- external interface -------------------
StringTable stringTable;

ASTSpecFile *readAbstractGrammar(char const *fname)
{
  if (tracingSys("yydebug")) {
    #ifndef NDEBUG
      yydebug = true;
    #else
      std::cout << "debugging disabled by -DNDEBUG\n";
    #endif
  }

  //Owner<GrammarLexer> lexer;
  Owner<std::ifstream> in;
  if (fname == NULL) {
    // stdin
    lexer = new (astgen_pool) GrammarLexer(astgen_pool, isAGramlexEmbed, stringTable);
  }
  else {
    // file
    in = new std::ifstream(fname);
    if (!*in) {
      throw_XOpen(fname);
    }
    trace("tmp") << "in is " << in.get() << std::endl;
    lexer = new (astgen_pool) GrammarLexer(astgen_pool, isAGramlexEmbed, stringTable, fname, in.xfr());
  }

  ASTParseParams params(*lexer);

  traceProgress() << "parsing grammar source..\n";
  int retval;
  try {
    retval = agrampar_yyparse(&params);
  }
  catch (xFormat &x) {
    lexer->err(x.cond());     // print with line number info
    throw;
  }

  if (retval == 0) {

    delete lexer;
    lexer = NULL;

    return params.treeTop;
  }
  else {
    xformat("parsing finished with an error");
  }
}



// ----------------------- test code -----------------------
#ifdef TEST_AGRAMPAR

#include "test.h"    // ARGS_MAIN

void entry(int argc, char **argv)
{
  TRACE_ARGS();

  if (argc != 2) {
    std::cout << "usage: " << argv[0] << " ast-spec-file\n";
    return;
  }

  // parse the grammar spec
  ast = readAbstractGrammar(argv[1]);

  // print it out
  //ast->debugPrint(std::cout, 0);
  debugEverything();
}

ARGS_MAIN

#endif // TEST_AGRAMPAR
