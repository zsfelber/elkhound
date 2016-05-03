// grampar.cc            see license.txt for copyright and terms of use
// additional C++ code for the grammar parser; in essence,
// build the grammar internal representation out of what
// the user supplies in a .gr file

#include "grampar.h"         // this module
#include "gramlex.h"         // GrammarLexer
#include "trace.h"           // tracing debug functions
#include "grammar.h"         // Grammar, Production, etc.
#include "owner.h"           // Owner (redundant dependency; dot's layout is better with it though)
#include "syserr.h"          // xsyserror
#include "strutil.h"         // quoted
#include "grampar.tab.h"     // token constant codes, union YYSTYPE
#include "mlsstr.h"          // MLSubstrate
#include "util.h"
#include "gramanl.h"

#include <fstream>           // ifstream
#include <ctype.h>           // isspace, isalnum
#include <sstream>           // stringstream

inline bool _isVoid(char const * tp) {
    return !tp || !strcmp("void", tp);
}
inline bool isVoid(string const * tp) {
    return !tp || _isVoid(tp->c_str());
}


// ------------------------- Environment ------------------------
Environment::Environment(DBG_INFO_FORMAL_FIRST  Grammar &G)
  : Storeable(DBG_INFO_ARG0),
    g(G),
    prevEnv(NULL),
    nontermDecls(),
    buffer(new EnvironmentBuffer),
    errors(buffer->errors),
    startSymbol(buffer->startSymbol),
    startLexer(buffer->startLexer),
    parserFuncs(buffer->parserFuncs),
    bufIncl(buffer->bufIncl),
    bufHead(buffer->bufHead),
    bufConsBase(buffer->bufConsBase),
    bufHeadFun(buffer->bufHeadFun),
    bufCc(buffer->bufCc)
{}

Environment::Environment(DBG_INFO_FORMAL_FIRST  Environment &prev)
  : Storeable(DBG_INFO_ARG0),
    g(prev.g),
    prevEnv(&prev),
    nontermDecls(prev.nontermDecls),
    buffer(0),
    errors(prev.errors),
    startSymbol(prev.startSymbol),     // copy parent's 'errors' reference
    startLexer(prev.startLexer),
    parserFuncs(prev.parserFuncs),
    bufIncl(prev.bufIncl),
    bufHead(prev.bufHead),
    bufConsBase(prev.bufConsBase),
    bufHeadFun(prev.bufHeadFun),
    bufCc(prev.bufCc)
{}

Environment::Environment(DBG_INFO_FORMAL_FIRST  Environment &prev, Grammar &g)
  : Storeable(DBG_INFO_ARG0),
    g(g),
    prevEnv(&prev),
    nontermDecls(prev.nontermDecls),
    buffer(0),
    errors(prev.errors),
    startSymbol(prev.startSymbol),     // copy parent's 'errors' reference
    startLexer(prev.startLexer),
    parserFuncs(prev.parserFuncs),
    bufIncl(prev.bufIncl),
    bufHead(prev.bufHead),
    bufConsBase(prev.bufConsBase),
    bufHeadFun(prev.bufHeadFun),
    bufCc(prev.bufCc)
{}



Environment::Environment(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, Grammar &G)
  : Storeable(DBG_INFO_ARG0_FIRST  pool),
    g(G),
    prevEnv(NULL),
    nontermDecls(),
    buffer(new EnvironmentBuffer),
    errors(buffer->errors),
    startSymbol(buffer->startSymbol),
    startLexer(buffer->startLexer),
    parserFuncs(buffer->parserFuncs),
    bufIncl(buffer->bufIncl),
    bufHead(buffer->bufHead),
    bufConsBase(buffer->bufConsBase),
    bufHeadFun(buffer->bufHeadFun),
    bufCc(buffer->bufCc)
{}

Environment::Environment(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, Environment &prev)
  : Storeable(DBG_INFO_ARG0_FIRST  pool),
    g(prev.g),
    prevEnv(&prev),
    nontermDecls(prev.nontermDecls),
    buffer(0),
    errors(prev.errors),
    startSymbol(prev.startSymbol),     // copy parent's 'errors' reference
    startLexer(prev.startLexer),
    parserFuncs(prev.parserFuncs),
    bufIncl(prev.bufIncl),
    bufHead(prev.bufHead),
    bufConsBase(prev.bufConsBase),
    bufHeadFun(prev.bufHeadFun),
    bufCc(prev.bufCc)
{}

Environment::Environment(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, Environment &prev, Grammar &g)
  : Storeable(DBG_INFO_ARG0_FIRST  pool),
    g(g),
    prevEnv(&prev),
    nontermDecls(prev.nontermDecls),
    buffer(0),
    errors(prev.errors),
    startSymbol(prev.startSymbol),     // copy parent's 'errors' reference
    startLexer(prev.startLexer),
    parserFuncs(prev.parserFuncs),
    bufIncl(prev.bufIncl),
    bufHead(prev.bufHead),
    bufConsBase(prev.bufConsBase),
    bufHeadFun(prev.bufHeadFun),
    bufCc(prev.bufCc)
{}


Environment::~Environment()
{
    if (buffer) {
        delete buffer;
    }
}


// -------------------- XASTParse --------------------
STATICDEF string XASTParse::
  constructMsg(LocString const *&tok, rostring msg)
{
  if (tok && tok->validLoc()) {
    if (tok->isNonNull()) {
      return stringc << tok->locString() << ": near " << tok
                     << ", " << msg;
    }
    else {
      return stringc << tok->locString() << ": " << msg;
    }
  }
  else {
    return string(msg);
  }
}

XASTParse::XASTParse(LocString const *&tok, rostring m)
  : xBase(constructMsg(tok, m)),
    failToken(tok),
    message(m)
{}


XASTParse::XASTParse(XASTParse const *&obj)
  : xBase(obj),
    PMEMB(failToken),
    PMEMB(message)
{}

XASTParse::~XASTParse()
{}


// -------------------- AST parser support ---------------------
// fwd-decl of parsing fns
void astParseGrammar(Environment &g, GrammarAST *treeTop);
void astParseTerminals(Environment &env, TF_terminals const &terms);
void astParseDDM(Environment &env, Symbol *sym,
                 ASTList<SpecFunc> const &funcs);
void astParseNonterm(Environment &env, TF_nonterm const *nt, int ntIndex);
void astParseProduction(Environment &env, Nonterminal *nonterm, AbstractProdDecl const *prodDecl);

void traverseProduction(Environment &env, GrammarAST *ast, Nonterminal *nonterm,
                        AbstractProdDecl const *prod, int prodi, TermDecl const *eof,
                        std::string errorHandler = "",
                        std::string tpref = "", std::string vpref = "", std::string fvpref = "", std::string indent = "   ",
                        std::stringstream * _bufAct = NULL, std::stringstream * _buf = NULL );


// really a static semantic error, more than a parse error..
void astParseError(LocString const *&failToken, rostring msg)
{
  THROW(XASTParse(failToken, msg));
}

void astParseError(SourceLoc loc, rostring msg)
{
  LocString const *locstr = LIT_STR_2(loc, NULL);
  THROW(XASTParse(locstr, msg));
}

void astParseError(rostring msg)
{
  LocString const *ls;    // no location info
  THROW(XASTParse(ls, msg));
}

// print the same message, but keep going anyway
void astParseErrorCont(Environment &env, LocString const *&failToken,
                       rostring msg)
{
  XASTParse x(failToken, msg);
  cout << x.why() << std::endl;
  env.errors++;
}


// to put as the catch block; so far it's kind of ad-hoc where
// I actually put 'try' blocks..
#define CATCH_APPLY_CONTEXT(tok)        \
  catch (XASTParse &x) {                \
    /* leave unchanged */               \
    throw x;                            \
  }                                     \
  catch (xBase &x) {                    \
    /* add context */                   \
    astParseError(tok, x.why());        \
    throw 0;     /* silence warning */  \
  }


// ---------------------- AST "parser" --------------------------
// set the annotation pointers
void setAnnotations(GrammarAST *ast)
{
  // work through the toplevel forms
  FOREACH_ASTLIST_NC(TopForm, ast->forms, iter) {
    ASTSWITCH(TopForm, iter.data()) {
      ASTCASE(TF_terminals, t) {
        if (!ast->terms) {
          ast->terms = t;
        }
        else {
          // 12/07/04: Allow more than one 'terminals' section,
          // by simply concatenating them.  This is useful when
          // I want to merge two grammars together textually.
          ast->terms->decls.concat(t->decls);
          ast->terms->types.concat(t->types);
          ast->terms->prec.concat(t->prec);
        }
      }

      ASTNEXT(TF_nonterm, nt) {
        if (!ast->firstNT) {
          ast->firstNT = nt;
        }
      }

      ASTENDCASED
    }
  }

  if (!ast->terms) {
    astParseError("'Terminals' specification is missing");
  }
  if (!ast->firstNT) {
    astParseError("you have to have at least one nonterminal");
  }
}


LocString* extractActionClassName(LocString const *body)
{
  // find start of first token
  char const *start = body->str.c_str();
  while (isspace(*start)) start++;

  // find end of first token
  char const *p = start;
  while (isspace(*p)) p++;
  while (isalnum(*p) || *p=='_') p++;
  
  // yield that, with the same source location
  return LIT_STR_2_L_ADD(body, substring(start, p-start).c_str());
}


// handle TF_verbatim and TF_option
void astParseOptions(Grammar &g, GrammarAST *ast)
{
  // handle TF_verbatim and TF_option
  FOREACH_ASTLIST_NC(TopForm, ast->forms, iter) {
    ASTSWITCH(TopForm, iter.data()) {
      ASTCASE(TF_context, c) {
        // overwrite the context class name, and append to
        // its body verbatim list
        g.actionClassName = extractActionClassName(c->body);

        // 11/13/04: There is a subtle problem with keeping the body
        // from the base specification, when the following conditions
        // hold:
        //   - the base spec is compiled on its own (w/o the extension)
        //   - some translation unit "A" sees the resulting .gr.gen.h file
        //   - the extension spec is compiled
        //   - some translation unit "B" sees the resulting .gr.gen.h file
        //   - A and B are linked together in one executable
        // In that case, the context_class from the base will have an
        // inconsistent definition in A and B, since in A it will be
        // whatever the user wrote plus, the declarations for the
        // action functions, whereas in B it will be just what the
        // user wrote, since the action functions end up in the
        // extension context_class.
        //
        // What is even more subtle is the *manifestation* of this
        // problem, which is linking problems with vtables.  C++
        // compilers do not explicitly check that classes declared in
        // multiple translation units have identical declarations
        // (token for token), but they *do* of course rely on them
        // being so.  That reliance shows up in the decisions
        // regarding which module has the vtable, among other places.
        // So this problem did not show up immediately, and was only
        // revealed as initially mysterious portability problems
        // (since my development toolchain happend to be fairly
        // lenient w.r.t. vtable placement).
        //
        // Therefore the new policy is that context_classes from the
        // base are *not* emitted, and consequently it is impossible
        // to inherit from them in subsequent context_classes.  The
        // user must put data/functions that are meant to be shared
        // into a common base class that is *not* the context_class
        // of any grammar or extension.
        //
        // old:
        //g.actionClasses.append(new LocString(c->body));
        //
        // new:
        g.actionClasses.deleteAll();
        g.actionClasses.append(DBG_INFO_ARG0_FIRST  c->body);
      }

      ASTNEXT(TF_verbatim, v) {
        if (v->isImpl) {
          g.implVerbatim.append(DBG_INFO_ARG0_FIRST  v->code);
        }
        else {
          g.verbatim.append(DBG_INFO_ARG0_FIRST  v->code);
        }
      }

      ASTNEXT(TF_option, op) {
        LocString const *name = op->name;
        int value = op->value;
        bool boolVal = !!value;

        if (name->equals("useGCDefaults")) {
          g.useGCDefaults = boolVal;
        }
        else if (name->equals("defaultMergeAborts")) {
          g.defaultMergeAborts = boolVal;
        }
        else if (name->equals("shift_reduce_conflicts")) {
          g.expectedSR = value;
        }
        else if (name->equals("reduce_reduce_conflicts")) {
          g.expectedRR = value;
        }
        else if (name->equals("unreachable_nonterminals")) {
          g.expectedUNRNonterms = value;
        }
        else if (name->equals("unreachable_terminals")) {
          g.expectedUNRTerms = value;
        }
        else if (name->equals("lang_OCaml")) {
          //g.targetLang = "OCaml";
          //
          // I'm retarded.. I need to know if we're parsing ocaml *before*
          // we actually parse it, otherwise I can't skip the embedded
          // action fragments properly!
          astParseError(name, "The `lang_OCaml' option has been replaced with "
                              "the `-ocaml' command-line switch.  Please use "
                              "that instead.  (Sorry for the inconvenience.)");
        }
        else if (name->equals("allow_continued_nonterminals")) {
          ast->allowContinuedNonterminals = boolVal;
        }
        else {
          astParseError(name, "unknown option name");
        }
      }

      ASTENDCASED
    }
  }
}


// map the grammar definition AST into a Grammar data structure
Nonterminal * createNonterm(Environment &env, GrammarAST *ast, TF_nonterm *nt) {
    // check for already declared
    if (env.nontermDecls.isMapped(nt->name->str)) {
      if (!ast->allowContinuedNonterminals) {
        astParseError(nt->name, string("nonterminal already declared"));
      }
      else {
        // check for consistent type
        if (!nt->type->equals(*env.nontermDecls.queryf(*nt->name)->type)) {
          astParseError(nt->name, "continued nonterminal with different type");
        }

        // just allow it; it seems the parser just iterates over
        // all the TF_nonterms, and will do the right thing
        //continue;
        return NULL;
      }
    }

    // make the Grammar object to represent the new nonterminal
    Nonterminal *result = env.g.getOrMakeNonterminal(DBG_INFO_ARG0_FIRST  nt->name);

    // add this decl to our running list (in the original environment)
    //
    // 12/09/04: As far as I can tell, 'nontermDecls' is in fact not
    // used except for right here, to check whether a nonterminal
    // declaration is duplicated.
    env.nontermDecls.add(*nt->name, nt);
    return result;
}

// map the grammar definition AST into a Grammar data structure
void astParseGrammar(Environment &env, GrammarAST *ast)
{

  // handle TF_terminals
  astParseTerminals(env, *(ast->terms));

  // process all nonterminal declarations first, so while we're
  // looking at their bodies we can tell if one isn't declared
  {
    FOREACH_ASTLIST_NC(TopForm, ast->forms, iter) {

      if (iter.data()->isTF_start()) {
          constcast(env.startSymbol) = iter.data()->asTF_start()->symbol->clone(DBG_INFO_ARG0_FIRST  env.g.pool);
          constcast(env.startLexer) = iter.data()->asTF_start()->lexer->clone(DBG_INFO_ARG0_FIRST  env.g.pool);
      }

      if (!iter.data()->isTF_nonterm()) continue;
      TF_nonterm *nt = iter.data()->asTF_nonterm();

      createNonterm(env, ast, nt);
    }
  }

  // process nonterminal bodies
  {
    int ni = 1;
    FOREACH_ASTLIST(TopForm, ast->forms, iter) {
      if (!iter.data()->isTF_nonterm()) continue;
      TF_nonterm const *nt = iter.data()->asTF_nontermC();

      // new environment since it can contain a grouping construct
      // (at this very moment it actually can't because there is no syntax..)
      Environment newEnv(DBG_INFO_ARG0_FIRST  env);

      // parse it
      astParseNonterm(newEnv, nt, ni++);
    }
  }

  if (env.g.actionClassName->str.isempty()) {
    astParseError("you must specify a context class; for example:\n"
                  "  context_class Context : public UserActions {};\n");
  }

  if (env.errors) {
    astParseError("halting due to previously reported errors");
  }
}

// validate 'name'
TerminalOrSet astParseTokens(Environment &env, LocString const *name)
{
  Terminal *t = env.g.findTerminal(*name);
  TerminalOrSet s;

  if (t) {
    s.set = false;
    s.t = t;
  } else {
    Nonterminal *nt = env.g.findNonterminal(*name);
    if (nt) {
        s.set = true;
        s.s = &nt->first;
    } else {
        s.set = false;
        s.t = 0;
        astParseError(name, "undeclared token/noneterminal");
    }
  }
  return s;
}

// validate 'name'
Terminal *astParseToken(Environment &env, LocString const *name)
{
  Terminal *t = env.g.findTerminal(name->str.c_str());
  if (!t) {
    astParseError(name, "undeclared token");
  }
  return t;
}


// needed to ensure the GrowArray below has its values initialized
// to false when the array expands
class InitFalseBool {
public:
  bool b;
public:
  InitFalseBool() : b(false) {}
};


void astParseTerminals(Environment &env, TF_terminals const &terms)
{
  // basic declarations
  {
    FOREACH_ASTLIST(TermDecl, terms.decls, iter) {
      TermDecl const &term = *(iter.data());

      // process the terminal declaration
      int code = term.code;
      LocString const* name = term.name;
      trace("grampar") << "token: code=" << code
                       << ", name=" << name << std::endl;

      if (!env.g.declareToken(DBG_INFO_ARG0_FIRST  term.name, code, term.alias)) {
        astParseError(constcast(term).name, "token already declared");
      }
    }

    // fill in any gaps in the code space; this is required because
    // later analyses assume the terminal code space is dense
    /*SourceLoc dummyLoc(HERE_SOURCELOC);
    for (int i=0; i<maxCode; i++) {
      if (!codeHasTerm[i].b) {
        LocString dummy(dummyLoc, grammarStringTable.add(
          stringc << "__dummy_filler_token" << i));
        env.g.declareToken(dummy, i, dummy);
      }
    }*/
  }

  // type annotations
  {
    FOREACH_ASTLIST(TermType, terms.types, iter) {
      TermType const &type = *(iter.data());
      trace("grampar") << "token type: name=" << type.name
                       << ", type=" << type.type << std::endl;

      // look up the name
      Terminal *t = astParseToken(env, type.name);
      if (t->type) {
        astParseError(constcast(type).name, "this token already has a type");
      }

      // annotate with declared type
      t->type = &type.type->str;

      // parse the dup/del/merge spec
      astParseDDM(env, t, type.funcs);
    }
  }

  // precedence specifications
  {
    FOREACH_ASTLIST(PrecSpec, terms.prec, iter) {
      PrecSpec const &spec = *(iter.data());

      FOREACH_ASTLIST(LocString, spec.tokens, tokIter) {
        LocString const *tokName = tokIter.data();
        trace("grampar") << "prec: " << toString(spec.kind)
                         << " " << spec.prec << " " << tokName;

        // look up the token
        Terminal *t = astParseToken(env, tokName);
        if (t->precedence) {
          astParseError(tokName,
            stringc << tokName << " already has a specified precedence");
        }

        if (spec.prec == 0) {
          // 0 means precedence isn't specified
          astParseError(tokName,
            "you can't use 0 as a precedence level, because that value "
            "is used internally to mean something else");
        }

        // apply spec
        t->precedence = spec.prec;
        t->associativity = spec.kind;
      }
    }
  }
}


void astParseDDM(Environment &env, Symbol *sym,
                 ASTList<SpecFunc> const &funcs)
{
  Terminal *term = sym->ifTerminal();
  Nonterminal *nonterm = sym->ifNonterminal();

  FOREACH_ASTLIST(SpecFunc, funcs, iter) {
    SpecFunc &func = *constcast(iter.data());
    int numFormals = func.formals.count();

    // decide what to do based on the name

    if (func.name->equals("dup")) {
      if (numFormals != 1) {
        astParseError(func.name, "'dup' function must have one formal parameter");
      }
      if (sym->dupParam) {
        astParseError(func.name, "duplicate 'dup' function");
      }
      sym->dupParam = &func.nthFormal(0)->str;
      sym->dupCode = func.code;
    }

    else if (func.name->equals("del")) {
      if (numFormals == 0) {
        // not specified is ok, since it means the 'del' function
        // doesn't use its parameter
        sym->delParam = NULL;
      }
      else if (numFormals == 1) {
        sym->delParam = &func.nthFormal(0)->str;
      }
      else {
        astParseError(func.name, "'del' function must have either zero or one formal parameters");
      }

      if (sym->delCode) {
        astParseError(func.name, "duplicate 'del' function");
      }
      sym->delCode = func.code;
    }

    else if (func.name->equals("merge")) {
      if (nonterm) {
        if (numFormals != 2) {
          astParseError(func.name, "'merge' function must have two formal parameters");
        }
        if (nonterm->mergeParam1) {
          astParseError(func.name, "duplicate 'merge' function");
        }
        nonterm->mergeParam1 = &func.nthFormal(0)->str;
        nonterm->mergeParam2 = &func.nthFormal(1)->str;
        nonterm->mergeCode = func.code;
      }
      else {
        astParseError(func.name, "'merge' can only be applied to nonterminals");
      }
    }

    else if (func.name->equals("keep")) {
      if (nonterm) {
        if (numFormals != 1) {
          astParseError(func.name, "'keep' function must have one formal parameter");
        }
        if (nonterm->keepParam) {
          astParseError(func.name, "duplicate 'keep' function");
        }
        nonterm->keepParam = func.nthFormal(0)->strref();
        nonterm->keepCode = func.code;
      }
      else {
        astParseError(func.name, "'keep' can only be applied to nonterminals");
      }
    }

    else if (func.name->equals("classify")) {
      if (term) {
        if (numFormals != 1) {
          astParseError(func.name, "'classify' function must have one formal parameter");
        }
        if (term->classifyParam) {
          astParseError(func.name, "duplicate 'classify' function");
        }
        term->classifyParam = func.nthFormal(0)->strref();
        term->classifyCode = func.code;
      }
      else {
        astParseError(func.name, "'classify' can only be applied to terminals");
      }
    }

    else if (func.name->equals("maximal")) {
      if (nonterm) {
        if (nonterm->maximal) {
          astParseError(func.name, "duplicate 'maximal' declaration");
        }
        nonterm->maximal = true;     // function body has no meaning
      }
      else {
        astParseError(func.name, "'maximal' can only be applied to nonterminals");
      }
    }

    else {
      astParseError(func.name,
        stringc << "unrecognized spec function \"" << func.name << "\"");
    }
  }
}


void fillDefaultType(Nonterminal* nonterm) {

    traceProgress() << "Nonterminal: " << nonterm->name << "  type:" << (nonterm->type?nonterm->type->c_str():"0") << std::endl;
    // fix circles:
    if (!nonterm->deftravd) {

        nonterm->deftravd = true;
        //rostring empty = grammarStringTable.add("");

        bool err_concur = false;
        bool err_in_one = false;
        bool init_void = isVoid(nonterm->type);
        std::string concur_types;
        // loop over default type providing single productions
        // (their single symbols were collected into 'defaults')
        for (SObjListIter<Production> syIter(nonterm->productions);
             !syIter.isDone(); syIter.adv()) {
            // convenient alias
            Production *p = constcast(syIter.data());

            Symbol *sym;
            if (p->defaultSymbol) {
                sym = p->defaultSymbol->sym;
            } else {
                if (!p->action) {
                   err_in_one = true;
                }
                continue;
            }

            if (sym->isNonterminal()) {
                Nonterminal &nt = sym->asNonterminal();
                fillDefaultType(&nt);
            }

            if (isVoid(sym->type)) {
                // remove "tag" for void args:
                //TODO ? p->defaultSymbol->tag.str = empty;
                p->defaultSymbol = NULL;

                err_in_one = true;

                continue;
            }

            if (init_void) {

                //trace("prec") << "Nonterminal: " << nonterm->name << " default type candidate:" << sym->type << std::endl;

                if (!isVoid(nonterm->type) && nonterm->type != sym->type) {
                    if (!err_concur) {
                        concur_types = concur_types + nonterm->type->c_str();
                    }
                    err_concur = true;
                    concur_types = concur_types + "," + sym->type->c_str();
                } else if (!nonterm->type_is_default) {
                    nonterm->type = sym->type;
                    nonterm->type_is_default = true;
                }
            } else {
                //trace("prec") << "Nonterminal: " << nonterm->name << "  non-void   production type:" << sym->type << std::endl;
            }
        }

        if (err_in_one) {
            trace("prec") << "Nonterminal " << nonterm->name << " has an undefined (default) type, but determining default type is failed in case of one or more productions." << std::endl;
        }
        if (err_concur) {
            trace("prec") << "Nonterminal " << nonterm->name << " has an undefined (default) type, but determining default type is not possible, candidates:"
                 << concur_types << std::endl;
            //errors++;
        }

        if (!err_in_one && !err_concur && isVoid(nonterm->type)) {
            trace("prec") << "Nonterminal " << nonterm->name << " has an undefined (default) type, but after looking for default type it is still void." << std::endl;
            //errors++;
        }

        if (err_in_one || err_concur) {
            if (nonterm->type_is_default) {
                nonterm->type = &grammarStringTable.add("void");
                nonterm->type_is_default = false;
            } else {
                traceProgress() << "Nonterminal   type errors: " << nonterm->name << "  user defined type kept:" << nonterm->type << std::endl;
            }

            for (SObjListIter<Production> syIter(nonterm->productions);
                 !syIter.isDone(); syIter.adv()) {
                Production *p = constcast(syIter.data());
                if (p->defaultSymbol) {
                    std::stringstream s;
                    s<<"/* return "<< p->defaultSymbol ->tag->str <<";  /* inconsistent nonterm("<<*nonterm->name<<") type*/";
                    p->action = LIT_STR_2_L_ADD(p->action, s.str().c_str());
                }
            }
        }
    }
}

void addDefaultTypesActions(Environment &env, GrammarAST *ast, Nonterminal *nonterm, TermDecl const * eof)
{
    Grammar &g = env.g;
    // language defaults
    string const *defaultType, *defaultAction;
    if (g.targetLang.equals("OCaml")) {
      defaultType = &grammarStringTable.add("unit");
      defaultAction = &grammarStringTable.add("()");
    }
    else /*C*/ {
      defaultType = &grammarStringTable.add("void");
      defaultAction = &grammarStringTable.add("return;");
    }

    fillDefaultType(nonterm);

    // hook to allow me to force defaults everywhere (this is useful
    // when I want to try a grammar written for one language using
    // another language's core)
    bool forceDefaults = tracingSys("forceDefaultActions");

    // default type
    if (forceDefaults || !nonterm->type) {
      nonterm->type = defaultType;
    }

    int prodi = 0;
    // loop over all productions
    for (SObjListIter<Production> prodIter(nonterm->productions);
         !prodIter.isDone(); prodIter.adv()) {
      // convenient alias
      Production *prod = constcast(prodIter.data());
      AbstractProdDecl * prodDecl = (AbstractProdDecl*)prod->prodDecl;

      traverseProduction(env, ast, nonterm, prodDecl, prodi++, eof);

      // put the code into it
      prod->action = prodDecl->actionCode;

      // default action
      if (forceDefaults || (!prod->action||prod->action->isNull())) {
          if (prod->defaultSymbol) {
              std::stringstream s;
              s<<"return "<< prod->defaultSymbol ->tag->str <<";";
              rostring defaultTagAction = grammarStringTable.add(s.str().c_str());
              prod->action = LIT_STR_2(prod->action->loc, defaultTagAction);
          } else {
              prod->action = LIT_STR_2(prod->action->loc, *defaultAction);
          }
      }

      if (forceDefaults) {
        // TODO FIXME ??->ok this way? rhs->sym->name = "" ->
        // clear RHSElt tags, since otherwise the lack of types
        // will provoke errors; and default actions don't refer to
        // the RHSElts anyway
        //StringRef empty = grammarStringTable.add("");
        for (ObjListIter<Production::RHSElt> rhsIter(prod->right);
             !rhsIter.isDone(); rhsIter.adv()) {
            // convenient alias
            Production::RHSElt *rhs = constcast(rhsIter.data());
            //TODO constcast(rhs->sym->name) = "";
        }
      }
    }
}

void addDefaultTypesActions(Environment &env, GrammarAST *ast, TermDecl const * eof)
{
    Grammar &g = env.g;
  // iterate over nonterminals
  for (SObjListIter<Nonterminal> ntIter(g.nonterminals);
       !ntIter.isDone(); ntIter.adv()) {
      // convenient alias
      Nonterminal *nonterm = constcast(ntIter.data());
      addDefaultTypesActions(env, ast, nonterm, eof);
  }
}

Nonterminal * complementNonterm(Environment &env, GrammarAST *ast, TF_nonterm * nt, int ntIndex, TermDecl const *eof) {
    Nonterminal * result = createNonterm(env, ast, nt);

    xassert (result);

    // new environment since it can contain a grouping construct
    // (at this very moment it actually can't because there is no syntax..)
    Environment newEnv(DBG_INFO_ARG0_FIRST  env);
    astParseNonterm(newEnv, nt, ntIndex);
    addDefaultTypesActions(env, ast, result, eof);
    return result;
}

void generateErrorHandlers(std::stringstream &buf, TreeProdDecl const *prod, std::string &indent) {
    buf << indent << "switch(errorKind) {" << std::endl;
    FOREACH_ASTLIST(MarkedAction, prod->markedActs, ierr) {
        if (ierr.data()->ekind <= ERR_PARSE) {
            buf << indent << "case " << toString(ierr.data()->ekind) << ":" << std::endl;
            buf << indent << "   " << ierr.data()->actionCode << std::endl;
            buf << indent << "   break;" << std::endl;
        }
    }
    buf << indent << "default:" << std::endl;
    buf << indent << "   break;" << std::endl;
    buf << indent << "}" << std::endl;
}

inline void generateJumpToErrorHandler(std::stringstream &buf, std::string &errorHandler, std::string &indent) {
    if (errorHandler.length()) {
        buf << indent << "// jump to error handler" << std::endl;
        buf << indent << "goto "<<errorHandler<<";" << std::endl;
    } else {
        buf << indent << "goto err;" << std::endl;
    }
}


ProdDecl *synthesizeChildRule(Environment &env, GrammarAST *ast, ASTList<RHSElt> *rhs, LocString const*startRuleAction, LocString const*& grType, std::string& name, std::string& usr) {
    Grammar &g = env.g;

    if (rhs->count()==2 && !startRuleAction) { // 1 + reof
        Symbol *s = NULL;
        if (rhs->first()->isRH_name())
           s = g.findSymbol(*rhs->first()->asRH_name()->name);
        else if (rhs->first()->isRH_string())
           s = g.findSymbol(*rhs->first()->asRH_string()->str);

        if (s && s->type) {

            //grType = LIT_STR(s->type);
            grType = LIT_STR(*s->type);

            usr = name = s->name->strref();

        }
    } else {
        trace("prec") << "Child rule rhs count : " << rhs->count() << " startRuleAction:" << (startRuleAction?"yes":"no") << std::endl;
    }

    std::map<std::string, ProdDecl*>::iterator it;
    if (grType && (it=env.parserFuncs.find(name))==env.parserFuncs.end()) {
        trace("prec") << "Creating  __GeneratedChildren -> " << name << "    type : " << (grType->isNull()?"":grType->str) << "   parser class field : _usr_" << usr << std::endl;

        env.bufIncl << "#include \""<< g.prefix0;
        if (name.length()) {
            env.bufIncl << "_" << name;
        }
        env.bufIncl <<".h\"" << std::endl;

        env.bufHead << "   "<< g.actionClassName << name << " _usr_" << usr << ";" << std::endl;
        env.bufConsBase << ", _usr_" << usr << "(this)";

        // append to multiple start symbol (will process later at last step, see 'int &multiIndex')
        ProdDecl *newStart;

        if (startRuleAction) {
            newStart = new (env.g.pool) ProdDecl(DBG_INFO_ARG0_FIRST  env.g.pool, SL_INIT, PDK_NEW/*prodDecl->pkind*/, rhs,
                                    startRuleAction,
                                    LIT_STR(name.c_str()), grType);
        } else {
            newStart = new (env.g.pool) ProdDecl(DBG_INFO_ARG0_FIRST  env.g.pool, SL_INIT, PDK_NEW/*prodDecl->pkind*/, rhs,
                                    LIT_STR_2(SL_UNKNOWN, NULL),
                                    LIT_STR(name.c_str()), grType);
        }
        env.parserFuncs[name] = newStart;

        if (ast->childrenNT) {
            ast->childrenNT->productions.append(DBG_INFO_ARG0_FIRST  newStart);
        } else {
            ast->childrenNT
                    = new (env.g.pool) TF_nonterm(
                        DBG_INFO_ARG0_FIRST
                        env.g.pool,
                        LIT_STR("__GeneratedChildren"),   // name
                        grType,          // type
                        &ASTList <SpecFunc >::EMPTY,                                     // empty list of functions
                        new (env.g.pool) ASTList<AbstractProdDecl>(DBG_INFO_ARG0_FIRST  env.g.pool, constcast(newStart)),          // productions
                        &ASTList <LocString >::EMPTY                                      // subsets
                      );
        }

        //if (multiIndex == -1) {
            // reset to this ast->childrenNT :
            // multiIndex = ast->childrenNT->productions.count();
        //}
        return newStart;
    } else if (grType) {
        xassert(!startRuleAction);
        return it->second;
    } else {
        std::stringstream s;
        FOREACH_ASTLIST(RHSElt, *rhs, iter) {
            LocString const *symTag, *symName;
            ASTSWITCHC(RHSElt, iter.data()) {
                ASTCASEC(RH_name, tname) {
                    symTag = tname->tag;
                    symName = tname->name;
                }
                ASTNEXTC(RH_string, ts) {
                    symTag = ts->tag;
                    symName = ts->str;
                }
                ASTDEFAULTC {
                    astParseError(symName, "Traverse mode '>' must be followed by terminals only.");
                }
                ASTENDCASEC
            }

            if (symTag && symTag->isNonNull() && symTag->length()) {
                s << symTag->str << ":" << symName->str;
            } else {
                s << symName->str;
            }
            s << "  ";
        }

        trace("prec") << "Failed creating __GeneratedChildren start rule (no type):" << s.str() << std::endl;

        return NULL;
    }
}

void createEarlyRule(Environment &env, GrammarAST *ast, AbstractProdDecl *prod, TermDecl const *eof) {
    xassert(prod);
    xassert(prod->rhs.count());

    RHSElt *poss_eof = prod->rhs.last();
    RH_name *rh_eof = 0;
    if (poss_eof && poss_eof->kind()==RH_name::RH_NAME) {
        rh_eof = poss_eof->asRH_name();
        if (rh_eof->name->equals(*eof->name)) {
          rh_eof = 0;
        }
    }
    if (!rh_eof) {
        rh_eof = new (env.g.pool) RH_name(DBG_INFO_ARG0_FIRST  env.g.pool, LIT_STR(""), eof->name);
        prod->rhs.append(DBG_INFO_ARG0_FIRST  rh_eof);
    }
    if (ast->earlyStartNT) {
        trace("prec") << "Replace "
                      << ast->earlyStartNT->productions.count() << " rules to  __EarlyStartSymbol -> "
                      << (prod->rhs.first()->isRH_name()?prod->rhs.first()->asRH_name()->name->strref():
                         prod->rhs.first()->isRH_string()?prod->rhs.first()->asRH_string()->str->strref(): "?")
                      << (prod->rhs.count()>1?" ...":"")       << std::endl;

        ast->earlyStartNT->productions.removeAll_dontDelete();
        ast->earlyStartNT->productions.prepend(DBG_INFO_ARG0_FIRST  prod);
        constcast(ast->earlyStartNT->type->str) = prod->type->str;

    } else {
        trace("prec") << "Create first  __EarlyStartSymbol -> "
                      << (prod->rhs.first()->isRH_name()?prod->rhs.first()->asRH_name()->name->strref():
                         prod->rhs.first()->isRH_string()?prod->rhs.first()->asRH_string()->str->strref(): "?")
                                                        << std::endl;
        ast->earlyStartNT
                = new (env.g.pool) TF_nonterm(
                    DBG_INFO_ARG0_FIRST
                    env.g.pool,
                    LIT_STR("__EarlyStartSymbol"),   // name
                    prod->type/*ast->firstNT->type.clone()*/,                   // type
                    &ASTList <SpecFunc >::EMPTY,                                    // empty list of functions
                    new (env.g.pool) ASTList<AbstractProdDecl>(DBG_INFO_ARG0_FIRST  env.g.pool, prod),      // productions  TODO memleak?
                    &ASTList <LocString >::EMPTY                                     // subsets
                  );
        ast->forms.prepend(DBG_INFO_ARG0_FIRST  ast->earlyStartNT);

    }
    env.g.startSymbol = complementNonterm(env, ast, ast->earlyStartNT, 0, eof);
    xassert(env.g.startSymbol);
}

void synthesizeStartRule(Environment &env, GrammarAST *ast, TermDecl const *eof, int &multiIndex, LocString const*& grType, std::string &name, std::string &usr)
{

  if (multiIndex > 0) {
      if (ast->childrenNT &&
                 multiIndex <= ast->childrenNT->productions.count() ) {

          AbstractProdDecl *prod = ast->childrenNT->productions.nth(multiIndex-1);
          createEarlyRule(env, ast, prod, eof);
      }
  } else {
      bool starts = env.startSymbol&&env.startSymbol->isNonNull();
      xassert(starts || ast->firstNT);

      // build a start production
      // zsf : default action filled later, in addDefaultTypesActions (which now also finds heuristic return types)
      RHSElt *rhs1 = new (env.g.pool) RH_name(DBG_INFO_ARG0_FIRST  env.g.pool, LIT_STR("top"), starts ? env.startSymbol : ast->firstNT->name);
      RHSElt *rhs2 = new (env.g.pool) RH_name(DBG_INFO_ARG0_FIRST  env.g.pool, LIT_STR(""), eof->name);
      ASTList<RHSElt> *rhs = new (env.g.pool) ASTList<RHSElt>(DBG_INFO_ARG0_FIRST  env.g.pool);
      rhs->append(DBG_INFO_ARG0_FIRST  rhs1);
      rhs->append(DBG_INFO_ARG0_FIRST  rhs2);

      LocString const* start = NULL;
      synthesizeChildRule(env, ast, rhs, start, grType, name, usr);
  }

  if (multiIndex >= 0) {
      multiIndex++;
      if (multiIndex > (ast->childrenNT?ast->childrenNT->productions.count():0) ) {
          multiIndex = -1;
      }
  }
}

void traverseProduction(Environment &env, GrammarAST *ast, Nonterminal *nonterm,
                        AbstractProdDecl const *prodDecl, int prodi,
                        TermDecl const *eof,
                        std::string errorHandler,
                        std::string tpref, std::string vpref, std::string fvpref, std::string indent,
                        std::stringstream * _bufAct, std::stringstream * _buf)
{
  Grammar &g = env.g;

  if (prodDecl->pkind >= PDK_TRAVERSE_GR) {

      TreeProdDecl * tprod = constcast(prodDecl)->asTreeProdDecl();

      bool v0 = !vpref.length();

      if (!prodDecl->traversed) {

          constcast(prodDecl)->traversed = true;

          std::stringstream bufAct0, buf0;
          if (!_buf) {
              _bufAct = &bufAct0;
              _buf = &buf0;
          }
          std::stringstream &bufAct=*_bufAct, &buf=*_buf;
          std::stringstream st0;
          std::string tp;

          if (v0) {
              st0 << "Ast_" << prodDecl->name;
              vpref = "";
              fvpref = "tag";
              bufAct << std::endl;
          } else {
              st0 << tpref;
          }
          tp = st0.str();
          st0 << "::Type__";
          tpref = st0.str();
          trace("prec") << "Traversing " << nonterm->name << " : " << tp << " : " << fvpref << std::endl;

          ASTList<RHSElt> &orhs = constcast(prodDecl->rhs);
          LocString const*origAction = prodDecl->actionCode;
          LocString const*startAction = NULL;

          ASTList<RHSElt> *rhs;

          rhs = new (env.g.pool) ASTList<RHSElt>(DBG_INFO_ARG0_FIRST  env.g.pool);
          RHSElt *reof = new (env.g.pool) RH_name(DBG_INFO_ARG0_FIRST  env.g.pool, LIT_STR(""), eof->name);
          rhs->assign(&orhs, true);
          rhs->append(DBG_INFO_ARG0_FIRST  reof);

          int vi = 0;
          LocString const* type, * grType;
          std::stringstream nms;
          std::stringstream us;
          std::string name, usr;
          std::string indp = indent + "   ";

          type = v0 && nonterm->type  ?
                      LIT_STR(*nonterm->type) : LIT_STR_ADD((tp+"*").c_str());

          switch (prodDecl->pkind) {
          case PDK_TRAVERSE_VAL:

              buf << indent << "" << std::endl;
              FOREACH_ASTLIST(TreeProdDecl, tprod->treeValidations, iter) {
                  TreeProdDecl const * prod = iter.data();

                  if (prod->label && prod->label->isNonNull()) {
                      buf << indent << prod->label->str << ":" << std::endl;
                  }

                  buf << indent << tpref << prod->name << "_star tag" << vpref << "_" << vi
                    << " = tag" << vpref << "->" << prod->name << ";" << std::endl;
                  if (prod->pkind == PDK_TRAVERSE_NULL) {
                      buf << indent << "if (!tag" << vpref <<  "_" << vi<<") {" << std::endl;
                  } else {
                      buf << indent << "if (tag" << vpref <<  "_" << vi<<") {" << std::endl;
                  }
                  std::stringstream st, sv, sfv, ind;
                  st << tpref << prod->name << std::flush;
                  sv << vpref << "_" << vi << std::flush;
                  sfv << fvpref << "->" << prod->name << std::flush;
                  ind << indp << std::flush;

                  if (prod->tag && prod->tag->isNonNull()) {
                      bufAct << "   " << tpref << prod->name << "_star "
                             << prod->tag << " = NULL;" << std::endl;
                      buf << indp << prod->tag << " = tag"
                          << vpref << "_" << vi << ";" << std::endl;
                  }

                  bool err = false;
                  FOREACH_ASTLIST(MarkedAction, prod->markedActs, ierr) {
                      if (ierr.data()->ekind <= ERR_PARSE) {
                          err = true;
                          break;
                      }
                  }
                  std::string ehnx = err ? "err"+sv.str() : errorHandler ;

                  if (prod->pkind != PDK_TRAVERSE_NULL) {
                      traverseProduction(env, ast, nonterm, prod, prodi, eof, ehnx, st.str(), sv.str(), sfv.str(), ind.str(), _bufAct, _buf);
                  }

                  buf << indent << "} else {" << std::endl;

                  if (err) {
                      buf << indp << "err" << sv.str() << ":" << std::endl;
                      generateErrorHandlers(buf, prod, indp);
                  }
                  generateJumpToErrorHandler(buf, errorHandler, indp);

                  buf << indent << "}" << std::endl;

                  vi++;
              }

              // fall through intended ...

          case PDK_TRAVERSE_NULL:

              if (v0) {
                  us << nonterm->ntIndex << "_" << prodi;
                  usr = us.str();
              } else if (origAction && origAction->isNonNull()) {
                  buf << indent << "// user action:" << std::endl;
                  buf << indent << origAction->str << std::endl;
              }
              break;

          case PDK_TRAVERSE_TKNS:
              buf << indent << "AstTreeNodeLexer treeLexer" << vpref << "(tag" << vpref << ", charLexer, false);" << std::endl;
              buf << std::endl;
              FOREACH_ASTLIST(RHSElt, *rhs, iter) {
                    LocString const *symTag, *symName;
                    ASTSWITCHC(RHSElt, iter.data()) {
                        ASTCASEC(RH_name, tname) {
                            symTag = tname->tag;
                            symName = tname->name;
                        }
                        ASTNEXTC(RH_string, ts) {
                            symTag = ts->tag;
                            symName = ts->str;
                        }
                        ASTDEFAULTC {
                            astParseError(symName, "Traverse mode '>' must be followed by terminals only.");
                        }
                        ASTENDCASEC
                    }
                    Terminal *term = g.findTerminal(*symName);
                    if (!term) {
                        astParseError(symName, "It is nonterminal. Traverse mode '>' should be followed by terminals.");
                    }
                    // NOTE We don't skip EOF !
                    buf << indent << "treeLexer" << vpref << ".adv();" << std::endl;
                    if (symTag && symTag->isNonNull() && symTag->length()) {
                        bufAct << "   " << term->type << " "
                               << symTag->str << " = NULL;" << std::endl;

                        buf << indent << "if (treeLexer" << vpref << ".type == " << term->name << ") {" << std::endl;
                        buf << indp << symTag->str << " = (" << term->type << ") treeLexer" << vpref << ".sval;" << std::endl;
                        buf << indent << "} else {" << std::endl;
                        generateJumpToErrorHandler(buf, errorHandler, indp);
                        buf << indent << "}" << std::endl;
                    } else {
                        buf << indent << "if (treeLexer" << vpref << ".type != " << term->name << ") {" << std::endl;
                        generateJumpToErrorHandler(buf, errorHandler, indp);
                        buf << indent << "}" << std::endl;
                    }
              }

              break;

          case PDK_TRAVERSE_GR:

              if (v0 && tprod->label && tprod->label->isNonNull()) {
                  buf << indent << tprod->label->str << ":" << std::endl;
              }

              buf << indent << "AstTreeNodeLexer treeLexer" << vpref << "(tag" << vpref << ", charLexer);" << std::endl;

              grType = type;

              nms << nonterm->name << "_" << prodi << "_" << vpref;

              if (v0) {
                  us << nonterm->ntIndex << "_" << prodi;
              } else {
                  us << nonterm->ntIndex << "_" << prodi << "_" << vpref;
              }

              name = nms.str();
              usr = us.str();

              FOREACH_ASTLIST(MarkedAction, tprod->markedActs, ierr) {
                  if (ierr.data()->ekind == START_RULE) {
                      startAction = ierr.data()->actionCode;
                      break;
                  }
              }

              synthesizeChildRule(env, ast, rhs, startAction, grType, name, usr);

              if (v0 && nonterm->type) {
                  buf << indent << nonterm->type << " result = NULL;" << std::endl;
              }

              buf << indent << "// initialize the parser" << std::endl;
              buf << indent << "GLR glrNode" << vpref << "(_usr_" << usr << ", _usr_" << usr << ".parseTables, tag" << vpref << ");" << std::endl;
              buf << indent << "" << std::endl;
              buf << indent << "// parse the input" << std::endl;
              buf << indent << "if (glrNode" << vpref << ".glrParse(treeLexer" << vpref << ", (SemanticValue&)";
              if (v0 && nonterm->type) {
                  buf <<"result";
              } else {
                  buf <<"tag";
              }
              buf << vpref << ")) {" << std::endl;

              if (origAction && origAction->isNonNull()) {
                  buf << indp << "// user action:" << std::endl;
                  buf << indp << origAction->str << std::endl;
              }

              if (tprod->label && tprod->label->isNonNull()) {
                  buf << indp << "goto done;" << std::endl;
              } else {
                  buf << indp << "// Nothing to do" << std::endl;
              }
              buf << indent << "} else {" << std::endl;

              generateJumpToErrorHandler(buf, errorHandler, indp);

              buf << indent << "}" << std::endl;

              break;
          default:
              astParseError("Invalid traverse mode production declaration.");
              break;
          }

          if (v0) {

              env.bufHeadFun << "   " << type->str << " parse_" << usr << "("
                               << tp <<"* tag);" << std::endl;
              env.bufCc << type->str << " "<< g.prefix0 << "Parsers::parse_" << usr << "("
                               << tp <<"* tag) {" << std::endl;
              env.bufCc << bufAct.str();
              env.bufCc << buf.str();
              env.bufCc << "   done:" << std::endl;

              if (origAction && origAction->isNonNull()) {
                  // Nothing to do
              } else if (prodDecl->pkind == PDK_TRAVERSE_NULL || prodDecl->pkind == PDK_TRAVERSE_VAL) {
                  env.bufCc << "   return tag;" << std::endl;
              } else if (v0 && nonterm->type) {
                  env.bufCc << "   return result;" << std::endl;
              } else {
                  env.bufCc << "   return tag;" << std::endl;
              }

              env.bufCc << "   err:" << std::endl;

              bool err = false;
              FOREACH_ASTLIST(MarkedAction, tprod->markedActs, ierr) {
                  if (ierr.data()->ekind <= ERR_PARSE) {
                      err = true;
                      break;
                  }
              }
              if (err) {
                  generateErrorHandlers(env.bufCc, tprod, indp);
              } else {
                  env.bufCc << "   // TODO default error handler" << std::endl;
                  env.bufCc << "   startLexer->parseError(\"Invalid '"<< nonterm->name << "' .\");" << std::endl;
              }
              env.bufCc << "   return NULL;" << std::endl;

              env.bufCc << "}" << std::endl;

              std::stringstream s;
              s << std::endl;
              s << "   " << type->str << " result = parsers->parse_" << usr << "(tag);" << std::endl;
              s << "   return result;" << std::endl;

              constcast(prodDecl)->actionCode = LIT_STR(s.str().c_str());
              orhs.append(DBG_INFO_ARG0_FIRST  new (env.g.pool) RH_name(DBG_INFO_ARG0_FIRST  env.g.pool, NOLOC_NULL, prodDecl->name));

          }

      }

      //if (v0) {
      //    goto produce;
      //}
  }
}

void astParseNonterm(Environment &env, TF_nonterm const *nt, int ntIndex)
{
  LocString const *name = nt->name;

  // get the Grammar object that represents the nonterminal
  Nonterminal *nonterm = env.g.findNonterminal(*name);
  xassert(nonterm);

  nonterm->type = &nt->type->str;
  nonterm->ntIndex = ntIndex;


  // iterate over the productions
  //int prodi = 0;
  FOREACH_ASTLIST(AbstractProdDecl, nt->productions, iter) {
    //traverseProduction(env, ast, nonterm, iter.data(), prodi++, eof);
    astParseProduction(env, nonterm, iter.data());
  }

  // parse dup/del/merge
  astParseDDM(env, nonterm, nt->funcs);
  
  // record subsets                       
  {
    FOREACH_ASTLIST(LocString, nt->subsets, iter) {
      LocString const *ls = iter.data();
      Nonterminal *sub = env.g.findNonterminal(*ls);
      if (!sub) {
        astParseError(ls, "nonexistent nonterminal");
      }

      // note that, since context-free language inclusion is
      // undecidable (Hopcroft/Ullman), we can't actually check that
      // the given nonterminals really are in the subset relation
      nonterm->subsets.prepend(DBG_INFO_ARG0_FIRST  sub);
    }
  }
}


void astParseProduction(Environment &env, Nonterminal *nonterm,
                      AbstractProdDecl const *prodDecl)
{
      Grammar &g = env.g;

      // is this the special start symbol I inserted?
      bool synthesizedStart = nonterm->name->equals("__EarlyStartSymbol");

      // build a production; use 'this' as the tag for LHS elements
      Production *prod = new (g.pool) Production(DBG_INFO_ARG0_FIRST g.pool, nonterm, "this");
      prod->prodDecl = prodDecl;

      int tags = 0;
      Production::RHSElt *first = 0;

      // deal with RHS elements
      FOREACH_ASTLIST(RHSElt, prodDecl->rhs, iter) {
        RHSElt const *n = iter.data();
        LocString const * symName;
        LocString const * symTag;
        bool isString = false;
        bool isAnnotation = false;

        // pull various info out of the AST node
        ASTSWITCHC(RHSElt, n) {
            ASTCASEC(RH_name, tname) {
              symName = tname->name;
              symTag = tname->tag;
            }

          ASTNEXTC(RH_string, ts) {
            symName = ts->str;
            symTag = ts->tag;
            isString = true;
          }

          ASTNEXTC(RH_prec, p) {
            Terminal *t = astParseToken(env, p->tokName);
            if (!t) { break; }

            // apply the specified precedence
            prod->precedence = t->precedence;
            isAnnotation = true;
          }

          ASTNEXTC(RH_forbid, f) {
            TerminalOrSet s = astParseTokens(env, f->tokName);

            try {
              if (s.set) {
                 prod->addForbid(DBG_INFO_ARG0_FIRST  g, s.s);
              } else if (s.t) {
                 prod->addForbid(DBG_INFO_ARG0_FIRST  g, s.t, g.numTerminals());
              }
            } catch (std::exception) {
               astParseError(const_cast<LocString const *&>(f->tokName),
                             "forbid_next : only single noneterminal allowed");
            }
            isAnnotation = true;
          }

          ASTDEFAULTC {
            xfailure("bad RHSElt kind");
          }

          ASTENDCASEC
        }

        if (isAnnotation) {
          // code below is for things other than annotations
          continue;
        }

        // see which (if either) thing this name already is
        Terminal *term = g.findTerminal(*symName);
        Nonterminal *nonterm = g.findNonterminal(*symName);
        if (term && nonterm) {
          if (isString) {
            astParseError(symName, "token alias has same name as a nonterminal");
          }
          else {
            astParseError(symName, "token and nonterminal have same name");
          }
          nonterm = NULL;                // error recovery
        }

        // syntax rules
        if (isString  &&  !term) {
          astParseError(symName, "terminals must be declared");
        }

        if (!term && !nonterm) {
          astParseErrorCont(env, symName, "undeclared symbol");

          // synthesize one anyway so we can find more errors
          nonterm = g.getOrMakeNonterminal(DBG_INFO_ARG0_FIRST  symName);
        }

        if (term && term->termIndex==0 && !synthesizedStart) {
          astParseError(symName, "you cannot use the EOF token in your rules");
        }

        if (symTag->equals("loc")) {
          // bad because loc is the name of the automatically-propagated
          // source location information
          astParseErrorCont(env, symTag, "cannot use \"loc\" as a tag");
        }

        // whenever we see a terminal, copy its precedence spec to
        // the production; thus, the last symbol appearing in the
        // production will be the one that gives the precedence
        if (term) {
          prod->precedence = term->precedence;
        }

        // decide which symbol to put in the production
        Symbol *s;
        if (nonterm) {
          s = nonterm;            // could do these two with a bitwise OR
        }                         // if I were feeling extra clever today
        else {
          s = term;
        }

        if (s->isEmptyString) {
          // "empty" is a syntactic convenience; it doesn't get
          // added to the production
        }
        else {
          // add it to the production
          Production::RHSElt * r = prod->append(DBG_INFO_ARG0_FIRST  g, s, symTag);

          if (!first) first = r;
          tags++;
        }
      }
      // generating default rule
      if (synthesizedStart ? tags<=2 : (tags==1)) {
        if (!first->tag || !first->tag->length()) {
           constcast(first->tag->str) = synthesizedStart ? "top" : "tag";
        }
        prod->defaultSymbol = first;
      } else if (synthesizedStart && isVoid(nonterm->type)) {
        traceProgress() << "tags: " << tags << std::endl;
        astParseErrorCont(env, nonterm->name, "Synthetic start is missing type, and unable to determine default.");
      }

      nonterm->appendProd(DBG_INFO_ARG0_FIRST  prod);

      // after constructing the production we need to do this
      // update: no we don't -- GrammarAnalysis takes care of it (and
      // complains if we do)
      //prod->finished();

      // add production to grammar
      g.addProduction(DBG_INFO_ARG0_FIRST  prod);
}


// ----------------------- parser support ---------------------
// Bison parser calls this to get a token
int grampar_yylex(YYSTYPE *lvalp, void *parseParam)
{
  ParseParams *par = (ParseParams*)parseParam;
  GrammarLexer &lexer = par->lexer;

  int code = lexer.yylexInc();

  try {
    // yield semantic values for some things
    // note that the yielded semantic value must be consistent with
    // what is declared for these token types in grampar.y
    switch (code) {
      case TOK_INTEGER:
        lvalp->num = lexer.integerLiteral;
        break;

      case TOK_STRING:
        lvalp->str = LIT_STR_2(lexer.curLoc(), lexer.stringLiteral);
        break;

      case TOK_NAME:
        lvalp->str = LIT_STR_2(lexer.curLoc(), lexer.curToken());
        break;

      case TOK_LIT_CODE:
        lvalp->str = LIT_STR_2(lexer.curLoc(), lexer.curFuncBody());
        break;

      case TOK_ARROW:
        lvalp->loc = lexer.curLoc();
        break;

      default:
        lvalp->str = NULL;        // any attempt to use will segfault
    }
  }
  catch (xBase &x) {
    // e.g. malformed fundecl
    cout << lexer.curLocStr() << ": " << x << std::endl;

    // optimistically try just skipping the bad token
    return grampar_yylex(lvalp, parseParam);
  }

  return code;
}


void grampar_yyerror(char const *message, void *parseParam)
{
  ParseParams *par = (ParseParams*)parseParam;
  cout << par->lexer.curLocStr() << ": " << message << std::endl;
}


// ---------------------- merging -----------------------
void mergeContext(GrammarAST *base, TF_context * /*owner*/ ext)
{
  // do simple append, since the grammar parser above knows how
  // to handle multiple context classes
  base->forms.append(DBG_INFO_ARG0_FIRST  ext);

  #if 0
  // find 'base' context
  TF_context *baseContext = NULL;
  FOREACH_ASTLIST_NC(TopForm, base->forms, iter) {
    if (iter.data()->isTF_context()) {
      baseContext = iter.data()->asTF_context();
      break;
    }
  }

  if (!baseContext) {
    // base does not have a context class, so 'ext' becomes it
    base->forms.append(ext);
  }

  else if (baseContext->name.str == ext->name.str) {
    // same name; I'd like to append the code to what's already
    // there, but that's tricky because the location won't
    // be right..
    astParseError(ext->name, "context append not implemented");
  }

  else {
    // different name, replace the old
    base->forms.removeItem(baseContext);
    delete baseContext;
    base->forms.append(ext);
  }
  #endif // 0
}


void mergeOption(GrammarAST *base, TF_option * /*owner*/ ext)
{                    
  // find option with the same name
  FOREACH_ASTLIST_NC(TopForm, base->forms, iter) {
    if (!iter.data()->isTF_option()) continue;
    TF_option *op = iter.data()->asTF_option();
    
    if (op->name->equals(*ext->name)) {
      // replace the old value
      op->value = ext->value;
      delete ext;
      return;
    }
  }

  // otherwise, just add the new option
  base->forms.append(DBG_INFO_ARG0_FIRST  ext);
}


void mergeTerminals(GrammarAST *base, TF_terminals * /*owner*/ ext)
{
  FOREACH_ASTLIST_NC(TopForm, base->forms, iter) {
    if (iter.data()->isTF_terminals()) {
      TF_terminals *t = iter.data()->asTF_terminals();
      
      // there's no point to changing codes, so all the
      // TermDecls just get added (collisions are detected
      // later, during AST parsing)
      t->decls.concat(ext->decls);
      
      // in fact, I'll do the same for the others, even though
      // it might make sense to do some replacement; my immediate
      // needs don't include replacement at this level
      t->types.concat(ext->types);
      t->prec.concat(ext->prec);
      
      delete ext;
      return;
    }

  }
  
  // no TF_terminals in 'base'.. unusual, but easy to handle
  base->forms.append(DBG_INFO_ARG0_FIRST  ext);
}


void mergeSpecFunc(TF_nonterm *base, SpecFunc * /*owner*/ ext)
{
  // find an existing spec func with the same name
  FOREACH_ASTLIST_NC(SpecFunc, base->funcs, iter) {
    SpecFunc *f = iter.data();
    if (f->name->equals(*ext->name)) {
      // replace the old code with the extension code
      base->funcs.removeItem(f);
      delete f;
      break;
    }
  }

  // just add it
  base->funcs.append(DBG_INFO_ARG0_FIRST  ext);
}


bool equalRHSElt(RHSElt const *elt1, RHSElt const *elt2)
{
  if (elt1->kind() != elt2->kind()) {
    return false;
  }

  // if the RHS names a terminal, this isn't perfect because one might
  // use an alias.. but I don't have the necessary information to detect
  // that, since I haven't yet computed the associated Symbols
  if (elt1->isRH_name()) {
    return elt1->asRH_nameC()->name->equals(*elt2->asRH_nameC()->name);
  }
  if (elt1->isRH_string()) {
    return elt1->asRH_stringC()->str->equals(*elt2->asRH_stringC()->str);
  }
  if (elt1->isRH_prec()) {
    // this means you can't change the precedence..
    return elt1->asRH_precC()->tokName->equals(*elt2->asRH_precC()->tokName);
  }

  xfailure("unknown RHSElt kind");
  return false;     // silence warning
}


bool equalRHS(AbstractProdDecl const *prod1, AbstractProdDecl const *prod2)
{
  if (prod1->rhs.count() != prod2->rhs.count()) {
    return false;
  }

  for (ASTListIter<RHSElt> iter1(prod1->rhs), iter2(prod2->rhs);
       !iter1.isDone(); iter1.adv(), iter2.adv()) {
    if (!equalRHSElt(iter1.data(), iter2.data())) {
      return false;
    }
  }
  return true;
}


void mergeProduction(TF_nonterm *base, AbstractProdDecl *ext)
{
  bool found = false;

  // look for a production with an identical RHS
  FOREACH_ASTLIST_NC(AbstractProdDecl, base->productions, iter) {
    AbstractProdDecl *prod = iter.data();

    // check RHSs for equality
    if (equalRHS(prod, ext)) {
      found = true;

      if (ext->pkind == PDK_NEW || ext->pkind >= PDK_TRAVERSE_GR) {
        astParseError(ext->loc,
                      "production has the same RHS as an existing production; "
                      "if intent is to replace, use the 'replace' keyword");
        // not reached
      }

      // delete old
      base->productions.removeItem(prod);
      delete prod;

      if (ext->pkind == PDK_DELETE) {
        delete ext;       // drop new on the floor, too
        return;
      }

      // will replace old with new
      xassert(ext->pkind == PDK_REPLACE);
      break;
    }
  }

  if (ext->pkind > PDK_NEW && ext->pkind < PDK_TRAVERSE_GR && !found) {
    astParseError(ext->loc,
                  "production marked with 'delete' or 'replace' does not match "
                  "any in the base specification");
  }

  // add the production
  base->productions.append(DBG_INFO_ARG0_FIRST  ext);
}


void mergeNonterminal(GrammarAST *base, TF_nonterm * /*owner*/ ext)
{
  // find an existing nonterminal with the same name
  TF_nonterm *exist = NULL;
  FOREACH_ASTLIST_NC(TopForm, base->forms, iter) {
    if (iter.data()->isTF_nonterm() &&
        iter.data()->asTF_nonterm()->name->equals(*ext->name)) {
      exist = iter.data()->asTF_nonterm();
    }
  }

  if (!exist) {
    // no pre-existing, just append it
    base->forms.append(DBG_INFO_ARG0_FIRST  ext);
    return;
  }

  // make sure the types agree
  if (!exist->type->str.equals(*ext->type)) {
    astParseError(ext->type, "cannot redefine the type of a nonterminal");
  }

  // merge the spec funcs
  while (ext->funcs.isNotEmpty()) {
    mergeSpecFunc(exist, ext->funcs.removeFirst());
  }

  // merge the productions
  while (ext->productions.isNotEmpty()) {
    mergeProduction(exist, ext->productions.removeFirst());
  }

  delete ext;
}


void mergeGrammar(GrammarAST *base, GrammarAST *ext)
{
  // work through all the forms in 'ext', removing each
  // one; it will then either be added to 'base', or
  // discarded entirely
  while (ext->forms.isNotEmpty()) {
    TopForm *form = ext->forms.removeFirst();

    ASTSWITCH(TopForm, form) {
      ASTCASE(TF_context, c) {
        mergeContext(base, c);
      }

      ASTNEXT(TF_verbatim, v) {
        // verbatims simply accumulate
        base->forms.append(DBG_INFO_ARG0_FIRST  v);
      }

      ASTNEXT(TF_option, op) {
        mergeOption(base, op);
      }

      ASTNEXT(TF_terminals, t) {
        mergeTerminals(base, t);
      }

      ASTNEXT(TF_nonterm, n) {
        mergeNonterminal(base, n);
      }
      
      ASTDEFAULT {
        xfailure("doh");
      }
      
      ASTENDCASE
    }
  }
}


// ---------------- external interface -------------------
bool isGramlexEmbed(int code);     // defined in gramlex.lex

GrammarAST *parseGrammarFile(Environment &env, rostring origFname, bool useML)
{
  string fname = origFname;

  #ifndef NDEBUG
  if (tracingSys("yydebug")) {
    yydebug = true;    // this flag goes away when NDEBUG is specified..
  }
  #endif // NDEBUG

  // open input file
  Owner<ifstream> in;
  if (fname.empty()) {
    fname = "<stdin>";
  }
  else {
    in = new ifstream(fname.c_str());
    if (!*in) {
      xsyserror("open", stringc << "error opening input file " << fname);
    }
  }

  // choose embedded language              
  EmbeddedLang *embed = NULL;
  if (useML) {
    embed = new MLSubstrate;
  }
  
  // build lexer
  GrammarLexer *lexer = new (env.g.pool) GrammarLexer(env.g.pool, isGramlexEmbed,
                                                      grammarStringTable,
                                                      fname.c_str(),
                                                      in.xfr(),
                                                      embed);
  if (embed) {
    // install the refined error reporter
    embed->err = &lexer->altReporter;
  }

  ParseParams params(*lexer);

  traceProgress() << "parsing grammar source: " << fname << std::endl;
  int retval = grampar_yyparse(&params);
  if (retval==0 && lexer->errors==0) {
    GrammarAST *ret = params.treeTop;

    if (tracingSys("printGrammarAST")) {
      // print AST
      cout << "AST:\n";
      ret->debugPrint(cout, 2);
    }

    return ret;
  }
  else {
    xbase("parsing finished with an error");
    return NULL;     // silence warning
  }
}


void parseGrammarAST(Environment &env, GrammarAST *treeTop, TermDecl const *& eof)
{

  // look at TF_options before synthesizing start rule,
  // so we can know what language is the target
  astParseOptions(env.g, treeTop);

  if (!treeTop->firstNT) {
      astParseError("you have to define at least 1 nonterm symbol");
      return;
  }

  // find the name of the user's EOF token
  eof = NULL;
  FOREACH_ASTLIST(TermDecl, treeTop->terms->decls, iter) {
    if (iter.data()->code == 0) {
      eof = iter.data();
      break;
    }
  }
  if (!eof) {
    astParseError("you have to have an EOF token, with code 0");
  }

  // moved to after parse
  //addDefaultTypesActions(g, treeTop);

  // moved to after outside cycle - grammanl.cc
  // synthesize a rule "TrueStart -> Start EOF"
  // synthesizeStartRule(g, treeTop, eof, multiIndex);

  // parse the AST into a Grammar
  traceProgress() << "parsing grammar AST..\n";
  astParseGrammar(env, treeTop);

  // fill in default types and actions
  addDefaultTypesActions(env, treeTop, eof);

  // then check grammar properties; throws exception
  // on failure
  traceProgress() << "beginning grammar analysis..\n";
  env.g.checkWellFormed();
}


void readGrammarFile(Environment &env, rostring fname)
{
  // make sure the tree gets deleted
  Owner<GrammarAST> treeTop(parseGrammarFile(env, fname, false /*useML*/));

  TermDecl const *eof = NULL;

  setAnnotations(treeTop);
  parseGrammarAST(env, treeTop, eof);

  treeTop.del();

  // hmm.. I'd like to restore this functionality...
  //if (ASTNode::nodeCount > 0) {
  //  cout << "leaked " << ASTNode::nodeCount << " AST nodes\n";
  //}
}


// ----------------------- test code -----------------------
#ifdef TEST_GRAMPAR

#include "bflatten.h"     // BFlatten
#include <stdlib.h>       // system

int main(int argc, char **argv)
{
  if (argc < 2) {
    cout << "usage: " << argv[0] << " [-tr flags] filename.gr\n";
    cout << "  interesting trace flags:\n";
    cout << "    keep-tmp      do not delete the temporary files\n";
    //cout << "    cat-grammar   print the ascii rep to the screen\n";
    return 0;
  }

  traceAddSys("progress");
  TRACE_ARGS();

  bool printCode = true;

  int multiIndex = 0;


  do {
      // read the file
      Grammar g1;
      readGrammarFile(g1, argv[1]);

      // and print the grammar
      char const g1Fname[] = "grammar.g1.tmp";
      traceProgress() << "printing initial grammar to " << g1Fname << "\n";
      {
        ofstream out(g1Fname);
        g1.printSymbolTypes(out);
        g1.printProductions(out, printCode);
      }

      //if (tracingSys("cat-grammar")) {
        system("cat grammar.g1.tmp");
      //}

      // before using 'xfer' we have to tell it about the string table
      flattenStrTable = &grammarStringTable;

      // write it to a binary file
      char const binFname[] = "grammar.bin.tmp";
      traceProgress() << "writing initial grammar to " << binFname << "\n";
      {
        BFlatten flat(binFname, false /*reading*/);
        g1.xfer(flat);
      }

      // read it back
      traceProgress() << "reading grammar from " << binFname << "\n";
      Grammar g2;
      {
        BFlatten flat(binFname, true /*reading*/);
        g2.xfer(flat);
      }

      // print that too
      char const g2Fname[] = "grammar.g2.tmp";
      traceProgress() << "printing just-read grammar to " << g2Fname << "\n";
      {
        ofstream out(g2Fname);
        g2.printSymbolTypes(out);
        g2.printProductions(out, printCode);
      }

      // compare the two written files
      int result = system(stringc << "diff " << g1Fname << " " << g2Fname);
      if (result != 0) {
        cout << "the two ascii representations differ!!\n";
        return 4;
      }

      // remove the temp files
      if (!tracingSys("keep-tmp")) {
        remove(g1Fname);
        remove(g2Fname);
        remove(binFname);
      }

      cout << "successfully parsed, printed, wrote, and read a grammar!\n";

  } while (multiIndex >= 0);

  return 0;

}

#endif // TEST_GRAMPAR
