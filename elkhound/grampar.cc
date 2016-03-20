// grampar.cc            see license.txt for copyright and terms of use
// additional C++ code for the grammar parser; in essence,
// build the grammar internal representation out of what
// the user supplies in a .gr file

#include "grampar.h"         // this module
#include "gramlex.h"         // GrammarLexer
#include "trace.h"           // tracing debug functions
#include "gramast.ast.gen.h" // grammar AST nodes
#include "grammar.h"         // Grammar, Production, etc.
#include "owner.h"           // Owner (redundant dependency; dot's layout is better with it though)
#include "syserr.h"          // xsyserror
#include "strutil.h"         // quoted
#include "grampar.tab.h"     // token constant codes, union YYSTYPE
#include "mlsstr.h"          // MLSubstrate
#include "util.h"

#include <fstream>           // ifstream
#include <ctype.h>           // isspace, isalnum
#include <sstream>           // stringstream

#define LIT_STR(s) LocString(SL_INIT, grammarStringTable.add(s))

inline bool isVoid(char const * tp) {
    return !tp || !strcmp("void", tp);
}


// ------------------------- Environment ------------------------
Environment::Environment(Grammar &G)
  : g(G),
    prevEnv(NULL),
    nontermDecls(),
    errorCount(0),
    errors(errorCount)
{}

Environment::Environment(Environment &prev)
  : g(prev.g),
    prevEnv(&prev),
    nontermDecls(prev.nontermDecls),
    errorCount(-1000),      // should never be used
    errors(prev.errors)     // copy parent's 'errors' reference
{}

Environment::~Environment()
{}


// -------------------- XASTParse --------------------
STATICDEF string XASTParse::
  constructMsg(LocString const &tok, rostring msg)
{
  if (tok.validLoc()) {
    if (tok.isNonNull()) {
      return stringc << tok.locString() << ": near " << tok
                     << ", " << msg;
    }
    else {
      return stringc << tok.locString() << ": " << msg;
    }
  }
  else {
    return string(msg);
  }
}

XASTParse::XASTParse(LocString const &tok, rostring m)
  : xBase(constructMsg(tok, m)),
    failToken(tok),
    message(m)
{}


XASTParse::XASTParse(XASTParse const &obj)
  : xBase(obj),
    DMEMB(failToken),
    DMEMB(message)
{}

XASTParse::~XASTParse()
{}


// -------------------- AST parser support ---------------------
// fwd-decl of parsing fns
void astParseGrammar(Grammar &g, GrammarAST *treeTop, TermDecl const *eof, int & multiIndex);
void astParseTerminals(Environment &env, TF_terminals const &terms);
void astParseDDM(Environment &env, Symbol *sym,
                 ASTList<SpecFunc> const &funcs);
void astParseNonterm(Environment &env, GrammarAST *ast, TF_nonterm const *nt, TermDecl const *eof, int ntIndex, int & multiIndex);
void astParseProduction(Environment &env, GrammarAST *ast, Nonterminal *nonterm,
                        AbstractProdDecl const *prod, int prodi, TermDecl const *eof, int & multiIndex,
                        std::string errorHandler = "",
                        std::string tpref = "", std::string vpref = "", std::string fvpref = "", std::string indent = "   ",
                        std::stringstream * _bufAct = NULL, std::stringstream * _buf = NULL );


// really a static semantic error, more than a parse error..
void astParseError(LocString const &failToken, rostring msg)
{
  THROW(XASTParse(failToken, msg));
}

void astParseError(SourceLoc loc, rostring msg)
{
  LocString locstr(loc, NULL);
  THROW(XASTParse(locstr, msg));
}

void astParseError(rostring msg)
{
  LocString ls;   // no location info
  THROW(XASTParse(ls, msg));
}

// print the same message, but keep going anyway
void astParseErrorCont(Environment &env, LocString const &failToken,
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


LocString extractActionClassName(LocString const &body)
{
  // find start of first token
  char const *start = body.str;
  while (isspace(*start)) start++;

  // find end of first token
  char const *p = start;
  while (isspace(*p)) p++;
  while (isalnum(*p) || *p=='_') p++;
  
  // yield that, with the same source location
  return LocString(body.loc, grammarStringTable.add(
    substring(start, p-start).c_str()));
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
        g.actionClasses.append(new LocString(c->body));
      }

      ASTNEXT(TF_verbatim, v) {
        if (v->isImpl) {
          g.implVerbatim.append(new LocString(v->code));
        }
        else {
          g.verbatim.append(new LocString(v->code));
        }
      }

      ASTNEXT(TF_option, op) {
        LocString const &name = op->name;
        int value = op->value;
        bool boolVal = !!value;

        if (name.equals("useGCDefaults")) {
          g.useGCDefaults = boolVal;
        }
        else if (name.equals("defaultMergeAborts")) {
          g.defaultMergeAborts = boolVal;
        }
        else if (name.equals("shift_reduce_conflicts")) {
          g.expectedSR = value;
        }
        else if (name.equals("reduce_reduce_conflicts")) {
          g.expectedRR = value;
        }
        else if (name.equals("unreachable_nonterminals")) {
          g.expectedUNRNonterms = value;
        }
        else if (name.equals("unreachable_terminals")) {
          g.expectedUNRTerms = value;
        }
        else if (name.equals("lang_OCaml")) {
          //g.targetLang = "OCaml";
          //
          // I'm retarded.. I need to know if we're parsing ocaml *before*
          // we actually parse it, otherwise I can't skip the embedded
          // action fragments properly!
          astParseError(name, "The `lang_OCaml' option has been replaced with "
                              "the `-ocaml' command-line switch.  Please use "
                              "that instead.  (Sorry for the inconvenience.)");
        }
        else if (name.equals("allow_continued_nonterminals")) {
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
void astParseGrammar(Grammar &g, GrammarAST *ast, TermDecl const *eof, int & multiIndex)
{
  // default, empty environment
  Environment env(g);

  // handle TF_terminals
  astParseTerminals(env, *(ast->terms));

  // process all nonterminal declarations first, so while we're
  // looking at their bodies we can tell if one isn't declared
  {
    FOREACH_ASTLIST_NC(TopForm, ast->forms, iter) {
      if (!iter.data()->isTF_nonterm()) continue;
      TF_nonterm *nt = iter.data()->asTF_nonterm();

      // check for already declared
      if (env.nontermDecls.isMapped(nt->name)) {
        if (!ast->allowContinuedNonterminals) {
          astParseError(nt->name, "nonterminal already declared");
        }
        else {
          // check for consistent type
          if (!nt->type.equals(env.nontermDecls.queryf(nt->name)->type)) {
            astParseError(nt->name, "continued nonterminal with different type");
          }
          
          // just allow it; it seems the parser just iterates over
          // all the TF_nonterms, and will do the right thing
          continue;
        }
      }

      // make the Grammar object to represent the new nonterminal
      env.g.getOrMakeNonterminal(nt->name);

      // add this decl to our running list (in the original environment)
      //
      // 12/09/04: As far as I can tell, 'nontermDecls' is in fact not
      // used except for right here, to check whether a nonterminal
      // declaration is duplicated.
      env.nontermDecls.add(nt->name, nt);
    }
  }

  // process nonterminal bodies
  {
    int ni = 0;
    FOREACH_ASTLIST(TopForm, ast->forms, iter) {
      if (!iter.data()->isTF_nonterm()) continue;
      TF_nonterm const *nt = iter.data()->asTF_nontermC();

      // new environment since it can contain a grouping construct
      // (at this very moment it actually can't because there is no syntax..)
      Environment newEnv(env);

      // parse it
      astParseNonterm(newEnv, ast, nt, eof, ni++, multiIndex);
    }
  }

  if (!g.actionClassName.str) {
    astParseError("you must specify a context class; for example:\n"
                  "  context_class Context : public UserActions {};\n");
  }

  if (env.errors) {
    astParseError("halting due to previously reported errors");
  }
}

// validate 'name'
TerminalOrSet astParseTokens(Environment &env, LocString const &name)
{
  Terminal *t = env.g.findTerminal(name);
  TerminalOrSet s;

  if (t) {
    s.set = false;
    s.t = t;
  } else {
    Nonterminal *nt = env.g.findNonterminal(name);
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
Terminal *astParseToken(Environment &env, LocString const &name)
{
  Terminal *t = env.g.findTerminal(name);
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
      StringRef name = term.name;
      trace("grampar") << "token: code=" << code
                       << ", name=" << name << std::endl;

      if (!env.g.declareToken(term.name, code, term.alias)) {
        astParseError(term.name, "token already declared");
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
        astParseError(type.name, "this token already has a type");
      }

      // annotate with declared type
      t->type = type.type;

      // parse the dup/del/merge spec
      astParseDDM(env, t, type.funcs);
    }
  }

  // precedence specifications
  {
    FOREACH_ASTLIST(PrecSpec, terms.prec, iter) {
      PrecSpec const &spec = *(iter.data());

      FOREACH_ASTLIST(LocString, spec.tokens, tokIter) {
        LocString const &tokName = *(tokIter.data());
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
    SpecFunc const &func = *(iter.data());
    int numFormals = func.formals.count();

    // decide what to do based on the name

    if (func.name.equals("dup")) {
      if (numFormals != 1) {
        astParseError(func.name, "'dup' function must have one formal parameter");
      }
      if (sym->dupParam) {
        astParseError(func.name, "duplicate 'dup' function");
      }
      sym->dupParam = func.nthFormal(0);
      sym->dupCode = func.code;
    }

    else if (func.name.equals("del")) {
      if (numFormals == 0) {
        // not specified is ok, since it means the 'del' function
        // doesn't use its parameter
        sym->delParam = NULL;
      }
      else if (numFormals == 1) {
        sym->delParam = func.nthFormal(0);
      }
      else {
        astParseError(func.name, "'del' function must have either zero or one formal parameters");
      }

      if (sym->delCode) {
        astParseError(func.name, "duplicate 'del' function");
      }
      sym->delCode = func.code;
    }

    else if (func.name.equals("merge")) {
      if (nonterm) {
        if (numFormals != 2) {
          astParseError(func.name, "'merge' function must have two formal parameters");
        }
        if (nonterm->mergeParam1) {
          astParseError(func.name, "duplicate 'merge' function");
        }
        nonterm->mergeParam1 = func.nthFormal(0);
        nonterm->mergeParam2 = func.nthFormal(1);
        nonterm->mergeCode = func.code;
      }
      else {
        astParseError(func.name, "'merge' can only be applied to nonterminals");
      }
    }

    else if (func.name.equals("keep")) {
      if (nonterm) {
        if (numFormals != 1) {
          astParseError(func.name, "'keep' function must have one formal parameter");
        }
        if (nonterm->keepParam) {
          astParseError(func.name, "duplicate 'keep' function");
        }
        nonterm->keepParam = func.nthFormal(0);
        nonterm->keepCode = func.code;
      }
      else {
        astParseError(func.name, "'keep' can only be applied to nonterminals");
      }
    }

    else if (func.name.equals("classify")) {
      if (term) {
        if (numFormals != 1) {
          astParseError(func.name, "'classify' function must have one formal parameter");
        }
        if (term->classifyParam) {
          astParseError(func.name, "duplicate 'classify' function");
        }
        term->classifyParam = func.nthFormal(0);
        term->classifyCode = func.code;
      }
      else {
        astParseError(func.name, "'classify' can only be applied to terminals");
      }
    }

    else if (func.name.equals("maximal")) {
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

    traceProgress() << "Nonterminal: " << nonterm->name << "  type:" << (nonterm->type?nonterm->type:"0") << std::endl;
    // also ignore circles:
    if (!nonterm->deftravd) {

        nonterm->deftravd = true;
        StringRef empty = grammarStringTable.add("");

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
                p->defaultSymbol->tag.str = empty;
                p->defaultSymbol = NULL;

                err_in_one = true;

                continue;
            }

            if (init_void) {

                //trace("prec") << "Nonterminal: " << nonterm->name << " default type candidate:" << sym->type << std::endl;

                if (!isVoid(nonterm->type) && nonterm->type != sym->type) {
                    if (!err_concur) {
                        concur_types = concur_types + nonterm->type;
                    }
                    err_concur = true;
                    concur_types = concur_types + "," + sym->type;
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
                nonterm->type = "void";
                nonterm->type_is_default = false;
            } else {
                traceProgress() << "Nonterminal   type errors: " << nonterm->name << "  user defined type kept:" << nonterm->type << std::endl;
            }

            for (SObjListIter<Production> syIter(nonterm->productions);
                 !syIter.isDone(); syIter.adv()) {
                Production *p = constcast(syIter.data());
                if (p->defaultSymbol) {
                    std::stringstream s;
                    s<<"/* return "<< p->defaultSymbol ->tag.str <<";  /* inconsistent nonterm("<<nonterm->name<<") type*/";
                    p->action.str = grammarStringTable.add(s.str().c_str());
                }
            }
        }
    }
}

void addDefaultTypesActions(Grammar &g, GrammarAST *ast)
{
  // language defaults
  StringRef defaultType, defaultAction;
  if (g.targetLang.equals("OCaml")) {
    defaultType = grammarStringTable.add("unit");
    defaultAction = grammarStringTable.add("()");
  }
  else /*C*/ {
    defaultType = grammarStringTable.add("void");
    defaultAction = grammarStringTable.add("return;");
  }

  // hook to allow me to force defaults everywhere (this is useful
  // when I want to try a grammar written for one language using
  // another language's core)
  bool forceDefaults = tracingSys("forceDefaultActions");

  // iterate over nonterminals
  for (ObjListIter<Nonterminal> ntIter(g.nonterminals);
       !ntIter.isDone(); ntIter.adv()) {
      // convenient alias
      Nonterminal *nonterm = constcast(ntIter.data());
      fillDefaultType(nonterm);

      // default type
      if (forceDefaults || !nonterm->type) {
        nonterm->type = defaultType;
      }

      // loop over all productions
      for (SObjListIter<Production> prodIter(nonterm->productions);
           !prodIter.isDone(); prodIter.adv()) {
        // convenient alias
        Production *prod = constcast(prodIter.data());

        // default action
        if (forceDefaults || prod->action.isNull()) {
            if (prod->defaultSymbol) {
                std::stringstream s;
                s<<"return "<< prod->defaultSymbol ->tag.str <<";";
                StringRef defaultTagAction = grammarStringTable.add(s.str().c_str());
                prod->action.str = defaultTagAction;
            } else {
                prod->action.str = defaultAction;
            }
        }

        if (forceDefaults) {
          // TODO FIXME ??->ok this way? rhs->sym->name = "" ->
          // clear RHSElt tags, since otherwise the lack of types
          // will provoke errors; and default actions don't refer to
          // the RHSElts anyway
          StringRef empty = grammarStringTable.add("");
          for (ObjListIter<Production::RHSElt> rhsIter(prod->right);
               !rhsIter.isDone(); rhsIter.adv()) {
              // convenient alias
              Production::RHSElt *rhs = constcast(rhsIter.data());
              //TODO constcast(rhs->sym->name) = "";
          }
        }
      }
  }
  /*FOREACH_ASTLIST_NC(TopForm, ast->forms, iter) {
    if (!iter.data()->isTF_nonterm()) { continue; }
    TF_nonterm *nt = iter.data()->asTF_nonterm();
    Nonterminal *nonterm = g.findNonterminal(nt->name);

    // default type
    if (forceDefaults || !nonterm->type) {
      nt->type.str = defaultType;
    }

    // iterate over productions
    FOREACH_ASTLIST_NC(ProdDecl, nt->productions, iter2) {
      ProdDecl *pd = iter2.data();

      // default action
      if (forceDefaults || prod.isNull()) {
        pd->actionCode.str = isnull ? defaultAction : defaultTagAction;
      }

      if (forceDefaults) {
        // clear RHSElt tags, since otherwise the lack of types
        // will provoke errors; and default actions don't refer to
        // the RHSElts anyway
        StringRef empty = grammarStringTable.add("");
        FOREACH_ASTLIST_NC(RHSElt, pd->rhs, iter3) {
          ASTSWITCH(RHSElt, iter3.data()) {
            ASTCASE(RH_name, n)
              n->tag.str = empty;

            ASTNEXT(RH_string, s)
              s->tag.str = empty;
            
            ASTENDCASED
          }
        }
      }
    }
  }*/
}

void createEarlyRule(GrammarAST *ast, AbstractProdDecl *prod, TermDecl const *eof) {
    RHSElt *poss_eof = prod->rhs.last();
    RH_name *rh_eof = 0;
    if (poss_eof && poss_eof->kind()==RH_name::RH_NAME) {
        rh_eof = poss_eof->asRH_name();
        if (strcmp(rh_eof->name, eof->name)) {
          rh_eof = 0;
        }
    }
    if (!rh_eof) {
        rh_eof = new RH_name(LIT_STR("").clone(), eof->name.clone());
        prod->rhs.append(rh_eof);
    }
    if (ast->earlyStartNT) {
        ast->earlyStartNT->productions.removeFirst();
        ast->earlyStartNT->productions.prepend(prod);
        ast->earlyStartNT->type.str = prod->type.str;
    } else {
        ast->forms.removeItem(ast->firstNT);

        ast->earlyStartNT
                = new TF_nonterm(
                    LIT_STR("__EarlyStartSymbol").clone(),   // name
                    prod->type.clone()/*ast->firstNT->type.clone()*/,                   // type
                    NULL,                                    // empty list of functions
                    new ASTList<AbstractProdDecl>(prod, false),      // productions  TODO memleak?
                    NULL                                     // subsets
                  );
        ast->forms.prepend(ast->earlyStartNT);
    }
}

void synthesizeStartRule(Grammar &g, GrammarAST *ast, TermDecl const *eof, int &multiIndex)
{

  if (multiIndex > 0) {
      if (ast->childrenNT &&
                 multiIndex <= ast->childrenNT->productions.count() ) {

          AbstractProdDecl *prod = ast->childrenNT->productions.nth(multiIndex-1);
          createEarlyRule(ast, prod, eof);
      }
  } else {

      // build a start production
      RHSElt *rhs1 = new RH_name(LIT_STR("top").clone(), ast->firstNT->name.clone());
      RHSElt *rhs2 = new RH_name(LIT_STR("").clone(), eof->name.clone());
      ASTList<RHSElt> *rhs = new ASTList<RHSElt>();
      rhs->append(rhs1);
      rhs->append(rhs2);
      // zsf : default action filled later, in addDefaultTypesActions (which now also finds heuristic return types)
      //char const *action = g.targetLang.equals("OCaml")? " top " :
      //                     ast->firstNT->type.equals("void")? " return; " :
      //                                                   " return top; ";
      ProdDecl *startProd = new ProdDecl(SL_INIT, PDK_NEW, rhs, new LocString(SL_UNKNOWN, NULL), new LocString(SL_UNKNOWN, NULL), new LocString(SL_UNKNOWN, NULL)/*LIT_STR(action).clone()*/);
      // build an even earlier start symbol
      ast->earlyStartNT
        = new TF_nonterm(
            LIT_STR("__EarlyStartSymbol").clone(),   // name
            new LocString(SL_UNKNOWN, NULL)/*ast->firstNT->type.clone()*/,                   // type
            NULL,                                    // empty list of functions
            new ASTList<AbstractProdDecl>(startProd),        // productions
            NULL                                     // subsets
          );
      // put it into the AST
      ast->forms.prepend(ast->earlyStartNT);
  }

  if (multiIndex >= 0) {
      multiIndex++;
      if (multiIndex > (ast->childrenNT?ast->childrenNT->productions.count():0) ) {
          multiIndex = -1;
      }
  }
}


void astParseNonterm(Environment &env, GrammarAST *ast, TF_nonterm const *nt, TermDecl const *eof, int ntIndex, int & multiIndex)
{
  LocString const &name = nt->name;

  // get the Grammar object that represents the nonterminal
  Nonterminal *nonterm = env.g.findNonterminal(name);
  xassert(nonterm);

  nonterm->type = nt->type;
  nonterm->ntIndex = ntIndex;


  // iterate over the productions
  int prodi = 0;
  FOREACH_ASTLIST(AbstractProdDecl, nt->productions, iter) {
    astParseProduction(env, ast, nonterm, iter.data(), prodi++, eof, multiIndex);
  }

  // parse dup/del/merge
  astParseDDM(env, nonterm, nt->funcs);
  
  // record subsets                       
  {
    FOREACH_ASTLIST(LocString, nt->subsets, iter) {
      LocString const *ls = iter.data();
      Nonterminal *sub = env.g.findNonterminal(*ls);
      if (!sub) {
        astParseError(*ls, "nonexistent nonterminal");
      }

      // note that, since context-free language inclusion is
      // undecidable (Hopcroft/Ullman), we can't actually check that
      // the given nonterminals really are in the subset relation
      nonterm->subsets.prepend(sub);
    }
  }
}


void astParseProduction(Environment &env, GrammarAST *ast, Nonterminal *nonterm,
                        AbstractProdDecl const *prodDecl, int prodi,
                        TermDecl const *eof, int & multiIndex,
                        std::string errorHandler,
                        std::string tpref, std::string vpref, std::string fvpref, std::string indent,
                        std::stringstream * _bufAct, std::stringstream * _buf)
{

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
              bufAct << indent << "" << std::endl;
          } else {
              st0 << tpref;
          }
          tp = st0.str();
          st0 << "::Type__";
          tpref = st0.str();
          trace("prec") << "Traversing " << nonterm->name << " : " << tp << " : " << fvpref << std::endl;

          ASTList<RHSElt> &orhs = constcast(prodDecl->rhs);
          LocString *origAction = prodDecl->actionCode.clone();

          ASTList<RHSElt> *rhs;

          rhs = new ASTList<RHSElt>();
          RHSElt *reof = new RH_name(LIT_STR("").clone(), eof->name.clone());
          rhs->steal(&orhs, false);
          rhs->append(reof);

          int vi = 0;
          LocString * type ;
          ProdDecl *newStart;
          std::stringstream nms;

          type = v0 && nonterm->type  ?
                      LIT_STR(nonterm->type).clone() : LIT_STR((tp+"*").c_str()).clone();

          switch (prodDecl->pkind) {
          case PDK_TRAVERSE_VAL:

              buf << indent << "" << std::endl;
              FOREACH_ASTLIST(TreeProdDecl, tprod->treeValidations, iter) {
                  TreeProdDecl const * prod = iter.data();
                  buf << indent << tpref << prod->name << "_star tag" << vpref << "_" << vi
                    << " = tag" << vpref << "->" << prod->name << ";" << std::endl;
                  buf << indent << "if (tag"<<vpref<< "_" << vi<<") {" << std::endl;
                  std::stringstream st, sv, sfv, ind;
                  st << tpref << prod->name << std::flush;
                  sv << vpref << "_" << vi << std::flush;
                  sfv << fvpref << "->" << prod->name << std::flush;
                  ind << indent << "   " << std::flush;

                  if (prod->tag && prod->tag.isNonNull()) {
                      bufAct << "   " << tpref << prod->name << "_star "
                             << prod->tag << " = NULL;" << std::endl;
                      buf << indent << "   " << prod->tag << " = tag"
                          << vpref << "_" << vi << ";" << std::endl;
                  }

                  std::string ehnx = prod->errorActs.count() ? "err"+sv.str() : errorHandler ;

                  astParseProduction(env, ast, nonterm, prod, prodi, eof, multiIndex, ehnx, st.str(), sv.str(), sfv.str(), ind.str(), _bufAct, _buf);

                  if (prod->actionCode && prod->actionCode.isNonNull()) {
                      buf << indent << "   " << prod->actionCode << std::endl;
                  }

                  buf << indent << "} else {" << std::endl;
                  if (prod->errorActs.count()) {
                      buf << indent << "   err" << sv.str() << ":" << std::endl;
                      buf << indent << "   switch(errorKind) {" << std::endl;
                      FOREACH_ASTLIST(ErrorAct, prod->errorActs, ierr) {
                          buf << indent << "   case " << toString(ierr.data()->ekind) << ":" << std::endl;
                          buf << indent << "      " << ierr.data()->actionCode << std::endl;
                          buf << indent << "      break;" << std::endl;
                      }
                      buf << indent << "   default:" << std::endl;
                      buf << indent << "      break;" << std::endl;
                      buf << indent << "   }" << std::endl;
                  }

                  if (errorHandler.length()) {
                      buf << indent << "   // jump to upper level" << std::endl;
                      buf << indent << "   goto "<<errorHandler<<";" << std::endl;
                  } else {
                      buf << indent << "   goto err;" << std::endl;
                  }
                  buf << indent << "}" << std::endl;

                  vi++;
              }

              break;

          case PDK_TRAVERSE_GR:
          case PDK_TRAVERSE_TKNS:

              buf << indent << "AstTreeNodeLexer treeLexer"<<vpref<<"(tag"<<vpref<<", charLexer";
              if (prodDecl->pkind == PDK_TRAVERSE_TKNS) {
                  FOREACH_ASTLIST(RHSElt, *rhs, iter) {
                       LocString symName;
                        ASTSWITCHC(RHSElt, iter.data()) {
                            ASTCASEC(RH_name, tname) {
                                symName = tname->name;
                            }
                            ASTNEXTC(RH_string, ts) {
                                symName = ts->str;
                            }
                            ASTDEFAULTC {
                              xfailure("bad RHSElt kind");
                            }
                            ASTENDCASEC
                        }
                        Terminal *term = env.g.findTerminal(symName);
                        if (!term) {
                            astParseError(symName, "Traverse mode '>' should all be followed by terminals.");
                        }
                  }
                  buf << ", false);" << std::endl;
              } else {
                  buf << ");" << std::endl;
              }

              nms << nonterm->name << "_" << prodi << vpref;

              env.g.bufIncl << "#include \""<< env.g.prefix0 << "_" << nms.str() <<".h\"" << std::endl;
              env.g.bufHead << "   "<< env.g.actionClassName << nms.str() <<" _usr_" << nonterm->ntIndex << "_" << prodi << "_" << vpref << ";" << std::endl;
              env.g.bufConsBase << ", _usr_" << nonterm->ntIndex << "_" << prodi << "_" << vpref << "(this)";

              buf << indent << "// initialize the parser" << std::endl;
              buf << indent << "GLR glrNode"<<vpref<<"(_usr_" << nonterm->ntIndex << "_" << prodi << "_" << vpref<<", _usr_"
                  << nonterm->ntIndex << "_" << prodi << "_" << vpref<<"::parseTables, tag"<<vpref<<");" << std::endl;
              buf << indent << "" << std::endl;
              buf << indent << "// parse the input" << std::endl;
              buf << indent << "if (!glrNode"<<vpref<<".glrParse(treeLexer"<<vpref<<", (SemanticValue&)tag"<<vpref<<")) {" << std::endl;
              if (errorHandler.length()) {
                  buf << indent << "   // jump to error handler" << std::endl;
                  buf << indent << "   goto "<<errorHandler<<";" << std::endl;
              } else {
                  buf << indent << "   goto err;" << std::endl;
              }
              buf << indent << "}" << std::endl;

              // append to multiple start symbol (will process later at last step, see 'int &multiIndex')
              newStart = new ProdDecl(SL_INIT, PDK_NEW/*prodDecl->pkind*/, rhs, origAction,
                                       LIT_STR(nms.str().c_str()).clone(), type->clone());
              // newStart->traversed = true;

              if (ast->childrenNT) {
                  ast->childrenNT->productions.append(newStart);
              } else {
                  ast->childrenNT
                          = new TF_nonterm(
                              LIT_STR("__GeneratedChildren").clone(),   // name
                              type->clone(),          // type
                              NULL,                                     // empty list of functions
                              new ASTList<AbstractProdDecl>(newStart),          // productions
                              NULL                                      // subsets
                            );
              }

              if (multiIndex == -1) {
                  // reset to this ast->childrenNT :
                  multiIndex = ast->childrenNT->productions.count();
              }

              break;
          default:
              astParseError("Invalid traverse mode production declaration.");
              break;
          }

          if (v0) {

              env.g.bufHeadFun << "   " << type->str << " parse_" << nonterm->ntIndex << "_" << prodi << "("
                               << tp <<"* tag);" << std::endl;
              env.g.bufCc << type->str << " "<< env.g.prefix0 << "Parsers::parse_" << nonterm->ntIndex << "_" << prodi << "("
                               << tp <<"* tag) {" << std::endl;
              env.g.bufCc << bufAct.str();
              env.g.bufCc << buf.str();
              env.g.bufCc << "   done:" << std::endl;
              env.g.bufCc << "   return tag;" << std::endl;

              env.g.bufCc << "   err:" << std::endl;

              if (tprod->errorActs.count()) {
                  env.g.bufCc << "   switch(errorKind) {" << std::endl;
                  FOREACH_ASTLIST(ErrorAct, tprod->errorActs, ierr) {
                      env.g.bufCc << "   case " << toString(ierr.data()->ekind) << ":" << std::endl;
                      env.g.bufCc << "      " << ierr.data()->actionCode << std::endl;
                      env.g.bufCc << "      break;" << std::endl;
                  }
                  env.g.bufCc << "   default:" << std::endl;
                  env.g.bufCc << "      break;" << std::endl;
                  env.g.bufCc << "   }" << std::endl;
              } else {
                  env.g.bufCc << "   // TODO default error handler" << std::endl;
                  env.g.bufCc << "   startLexer->parseError(\"Invalid '"<< nonterm->name << "' .\");" << std::endl;
              }
              env.g.bufCc << "   return NULL;" << std::endl;

              env.g.bufCc << "}" << std::endl;

              std::stringstream s;
              s << std::endl;
              s << "   " << type->str << " result = parsers->parse_" << nonterm->ntIndex << "_" << prodi << "(tag);" << std::endl;
              s << "   return result;" << std::endl;

              constcast(prodDecl->actionCode).str = LIT_STR(s.str().c_str()).clone()->str;
              orhs.append(new RH_name(new LocString(SL_UNKNOWN, NULL), LIT_STR(prodDecl->name).clone()));

          }

      }

      if (v0) {
          goto produce;
      }

  } else {
      produce:

      // is this the special start symbol I inserted?
      bool synthesizedStart = nonterm->name.equals("__EarlyStartSymbol");

      // build a production; use 'this' as the tag for LHS elements
      Production *prod = new Production(nonterm, "this");

      // put the code into it
      prod->action = prodDecl->actionCode;

      int tags = 0;
      Production::RHSElt *first = 0;

      // deal with RHS elements
      FOREACH_ASTLIST(RHSElt, prodDecl->rhs, iter) {
        RHSElt const *n = iter.data();
        LocString symName;
        LocString symTag;
        LocString attrName;
        bool isString = false;
        bool isAnnotation = false;

        // pull various info out of the AST node
        ASTSWITCHC(RHSElt, n) {
            ASTCASEC(RH_name, tname) {
              symName = tname->name;
              symTag = tname->tag;
            }

            /*ASTNEXTC(RH_attr, tattr) {
              symTag = tattr->tag;
              attrName = tattr->attrName;
              symName = tattr->attrValue;
              isString = tattr->akind & RHA_STRING;
            }*/

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
                 prod->addForbid(s.s);
              } else if (s.t) {
                 prod->addForbid(s.t, env.g.numTerminals());
              }
            } catch (std::exception) {
               astParseError(f->tokName, "forbid_next : only single noneterminal allowed");
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
        Terminal *term = env.g.findTerminal(symName);
        Nonterminal *nonterm = env.g.findNonterminal(symName);
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
          nonterm = env.g.getOrMakeNonterminal(symName);
        }

        if (term && term->termIndex==0 && !synthesizedStart) {
          astParseError(symName, "you cannot use the EOF token in your rules");
        }

        if (symTag.equals("loc")) {
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
          Production::RHSElt * r = prod->append(s, symTag);

          if (!first) first = r;
          tags++;
        }
      }
      // generating default rule
      if (synthesizedStart ? tags<=2 : (tags==1)) {
        if (!first->tag || !first->tag.length()) {
           first->tag.str = synthesizedStart ? "top" : "tag";
        }
        prod->defaultSymbol = first;
      } else if (synthesizedStart && isVoid(nonterm->type)) {
        traceProgress() << "tags: " << tags << std::endl;
        astParseErrorCont(env, nonterm->name, "Synthetic start is missing type, and unable to determine default.");
      }

      nonterm->appendProd(prod);

      // after constructing the production we need to do this
      // update: no we don't -- GrammarAnalysis takes care of it (and
      // complains if we do)
      //prod->finished();

      // add production to grammar
      env.g.addProduction(prod);
  }
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
        lvalp->str = new LocString(lexer.curLoc(), lexer.stringLiteral);
        break;

      case TOK_NAME:
        lvalp->str = new LocString(lexer.curLoc(), lexer.curToken());
        break;

      case TOK_LIT_CODE:
        lvalp->str = new LocString(lexer.curLoc(), lexer.curFuncBody());
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
  base->forms.append(ext);

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
    
    if (op->name.str == ext->name.str) {
      // replace the old value
      op->value = ext->value;
      delete ext;
      return;
    }
  }

  // otherwise, just add the new option
  base->forms.append(ext);
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
  base->forms.append(ext);
}


void mergeSpecFunc(TF_nonterm *base, SpecFunc * /*owner*/ ext)
{
  // find an existing spec func with the same name
  FOREACH_ASTLIST_NC(SpecFunc, base->funcs, iter) {
    SpecFunc *f = iter.data();
    if (f->name.str == ext->name) {
      // replace the old code with the extension code
      base->funcs.removeItem(f);
      delete f;
      break;
    }
  }

  // just add it
  base->funcs.append(ext);
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
    return elt1->asRH_nameC()->name.str == elt2->asRH_nameC()->name.str;
  }
  if (elt1->isRH_string()) {
    return elt1->asRH_stringC()->str.str == elt2->asRH_stringC()->str.str;
  }
  if (elt1->isRH_prec()) {
    // this means you can't change the precedence..
    return elt1->asRH_precC()->tokName.str == elt2->asRH_precC()->tokName.str;
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
  base->productions.append(ext);
}


void mergeNonterminal(GrammarAST *base, TF_nonterm * /*owner*/ ext)
{
  // find an existing nonterminal with the same name
  TF_nonterm *exist = NULL;
  FOREACH_ASTLIST_NC(TopForm, base->forms, iter) {
    if (iter.data()->isTF_nonterm() &&
        iter.data()->asTF_nonterm()->name.str == ext->name) {
      exist = iter.data()->asTF_nonterm();
    }
  }

  if (!exist) {
    // no pre-existing, just append it
    base->forms.append(ext);
    return;
  }

  // make sure the types agree
  if (exist->type.str != ext->type) {
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
        base->forms.append(v);
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

GrammarAST *parseGrammarFile(rostring origFname, bool useML)
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
  GrammarLexer lexer(isGramlexEmbed,
                     grammarStringTable,
                     fname.c_str(),
                     in.xfr(),
                     embed);
  if (embed) {
    // install the refined error reporter
    embed->err = &lexer.altReporter;
  }

  ParseParams params(lexer);

  traceProgress() << "parsing grammar source: " << fname << std::endl;
  int retval = grampar_yyparse(&params);
  if (retval==0 && lexer.errors==0) {
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


void parseGrammarAST(Grammar &g, GrammarAST *treeTop, int &multiIndex)
{

  // look at TF_options before synthesizing start rule,
  // so we can know what language is the target
  astParseOptions(g, treeTop);

  if (!treeTop->firstNT) {
      astParseError("you have to define at least 1 nonterm symbol");
      return;
  }
  // find the name of the user's EOF token
  TermDecl const *eof = NULL;
  FOREACH_ASTLIST(TermDecl, treeTop->terms->decls, iter) {
    if (iter.data()->code == 0) {
      eof = iter.data();
      break;
    }
  }
  if (!eof) {
    astParseError("you have to have an EOF token, with code 0");
    return;
  }

  // moved to after parse
  //addDefaultTypesActions(g, treeTop);

  // synthesize a rule "TrueStart -> Start EOF"
  synthesizeStartRule(g, treeTop, eof, multiIndex);

  // parse the AST into a Grammar
  traceProgress() << "parsing grammar AST..\n";
  astParseGrammar(g, treeTop, eof, multiIndex);

  // fill in default types and actions
  addDefaultTypesActions(g, treeTop);

  // then check grammar properties; throws exception
  // on failure
  traceProgress() << "beginning grammar analysis..\n";
  g.checkWellFormed();
}


void readGrammarFile(Grammar &g, rostring fname, int &multiIndex)
{
  // make sure the tree gets deleted
  Owner<GrammarAST> treeTop(parseGrammarFile(fname, false /*useML*/));

  setAnnotations(treeTop);
  parseGrammarAST(g, treeTop, multiIndex);

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
      readGrammarFile(g1, argv[1], multiIndex);

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
