// grammar.h            see license.txt for copyright and terms of use
// representation and algorithms for context-free grammars

// Author: Scott McPeak, April 2000

// Unfortunately, representation and algorithm tend to get
// mixed together.  Separating them entirely is possible,
// but syntactically inconvenient.  So, instead, I try to
// document the separation in comments.  Specifically,
// sections beginning with ---- representation ---- are data
// for representation of the underlying concept, while
// sections with ---- annotation ---- are data created by
// algorithms manipulating the data.

// Another measure is I've split all grammar-wide algorithm
// stuff into GrammarAnalysis (gramanl.h).  Things should
// only be put into Grammar if they are directly related
// to the grammar representation.  (However, constitutent
// objects like Production will continue to be a mix.)

#ifndef __GRAMMAR_H
#define __GRAMMAR_H

#include <iostream>    // ostream

#include "str.h"         // string
#include "objlist.h"     // ObjList
#include "sobjlist.h"    // SObjList
#include "storage.h"
#include "util.h"        // OSTREAM_OPERATOR, INTLOOP
#include "locstr.h"      // LocString, StringRef
#include "strobjdict.h"  // StringObjDict
#include "owner.h"       // Owner
#include "asockind.h"    // AssocKind
#include "array.h"       // GrowArray
#include <sstream>
#include <string>
#include <vector>
#include <map>

using std::ostream;
using std::ofstream;
using std::ifstream;
using std::istream;
using std::cout;
using std::endl;

class StrtokParse;       // strtokp.h

// fwds defined below
class Symbol;
class Terminal;
class Nonterminal;
class Production;
class DottedProduction;
class Grammar;

// transitional definitions
typedef StringObjDict<LocString> LitCodeDict;
typedef LocString LiteralCode;


// everywhere in the Grammar specification we have a StringRef, it
// refers to this string table
extern StringTable grammarStringTable;




// ---------------- Symbol --------------------
// either a nonterminal or terminal symbol
class Symbol : public str::Storeable {
// ------ representation ------
public:
  LocString const* name;     // symbol's name in grammar
  bool const isTerm;        // true: terminal (only on right-hand sides of productions)
                            // false: nonterminal (can appear on left-hand sides)
  bool const isEmptyString; // true only for the emptyString nonterminal

  StringRef type;           // C type of semantic value

  StringRef dupParam;       // name of parameter to 'dup'
  LocString const *dupCode;        // code to duplicate a semantic value

  StringRef delParam;       // param name; may be NULL to indicate not used
  LocString const *delCode;        // code
  
// ----------- annotation ------------
public:
  bool reachable;           // computed by constructLRItemSets; true when nonterminal reachable from start symbol

protected:  // funcs
  virtual void internalPrintDDM(ostream &os) const;

public:      // funcs
  Symbol(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, LocString const *n, bool t, bool e = false);
  virtual ~Symbol();

  Symbol(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten&);
  void xfer(str::StoragePool &pool, Flatten &flat);

  // symmetric selectors
  bool isTerminal() const { return isTerm; }
  bool isNonterminal() const { return !isTerm; }

  // both terminals and nonterminals have ids; this gets the
  // id for whichever kind this object happens to be
  int getTermOrNontermIndex() const;

  // casting
  Terminal const &asTerminalC() const;       // checks 'isTerminal' for cast safety
  Terminal &asTerminal()
    { return const_cast<Terminal&>(asTerminalC()); }

  Nonterminal const &asNonterminalC() const;
  Nonterminal &asNonterminal()
    { return const_cast<Nonterminal&>(asNonterminalC()); }

  // cast or NULL
  Terminal const *ifTerminalC() const;
  Terminal *ifTerminal()
    { return const_cast<Terminal*>(ifTerminalC()); }

  Nonterminal const *ifNonterminalC() const;
  Nonterminal *ifNonterminal() 
    { return const_cast<Nonterminal*>(ifNonterminalC()); }

  // debugging
  // print as '$name: isTerminal=$isTerminal' (no newline)
  virtual void print(ostream &os) const;
  OSTREAM_OPERATOR(Symbol)

  // print 'token[type] name { dup.. del.. merge.. }' (with newlines)
  void printDDM(ostream &os) const;

  // true if any of the handlers were specified
  virtual bool anyDDM() const;

  virtual string toString() const { return string(DBG_INFO_ARG0_FIRST  *name); }

};

// I have several needs for serf lists of symbols, so let's use this for now
typedef SObjList<Symbol> SymbolList;
typedef SObjListIter<Symbol> SymbolListIter;
typedef SObjListMutator<Symbol> SymbolListMutator;

#define FOREACH_SYMBOL(list, iter) FOREACH_OBJLIST(Symbol, list, iter)
#define MUTATE_EACH_SYMBOL(list, iter) MUTATE_EACH_OBJLIST(Symbol, list, iter)
#define SFOREACH_SYMBOL(list, iter) SFOREACH_OBJLIST(Symbol, list, iter)
#define SMUTATE_EACH_SYMBOL(list, iter) SMUTATE_EACH_OBJLIST(Symbol, list, iter)

// format: "s1 s2 s3"
string symbolSequenceToString(SymbolList const &list);


// ---------------- Terminal --------------------
// something that only appears on the right-hand side of
// productions, and is an element of the source language
// NOTE:  This is really a terminal *class*, in that it's possible
// for several different tokens to be classified into the same
// terminal class (e.g. "foo" and "bar" are both identifiers)
class Terminal : public Symbol {
// -------- representation ---------
public:     // data
  // whereas 'name' is the canonical name for the terminal class,
  // this field is an alias; for example, if the canonical name is
  // L2_EQUALEQUAL, the alias might be "=="; the alias should *not*
  // include actual double-quote characters
  // if the alias is "", there is no alias
  LocString const *alias;

  // parsgen-time conflict resolution: if a shift/reduce conflict
  // occurs between a production and a symbol, both with specified
  // precedence (not 0), then the one with the numerically higher
  // precedence will be used
  int precedence;

  // if, in the above scenario, the precedence values are the same,
  // then the associativity kind will be used to decide which to use
  AssocKind associativity;

  StringRef classifyParam;      // name of parameter to 'classify'
  LocString const *classifyCode;       // code to reclassify a token type

// ------ annotation ------
public:     // data
  // terminal class index - this terminal's id; -1 means unassigned
  int termCode;
  int termIndex;
  int externalTermIndex;

protected:  // funcs  
  virtual void internalPrintDDM(ostream &os) const;

public:     // funcs
  Terminal(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, LocString const *name)        // canonical name for terminal class
    : Symbol(DBG_INFO_ARG_FWD_FIRST  pool, name, true /*terminal*/),
      alias(NULL),
      precedence(0),
      associativity(AK_NONASSOC),
      classifyParam(NULL),
      classifyCode(NULL),
      termCode(-1),
      termIndex(-1)
  {}

  Terminal(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten &flat);
  void xfer(str::StoragePool &pool, Flatten &flat);

  virtual void print(ostream &os) const;
  OSTREAM_OPERATOR(Terminal)

  virtual bool anyDDM() const;

  // return alias if defined, name otherwise
  virtual string toString(bool quoteAliases = false) const;

};

typedef SObjList<Terminal> TerminalList;
typedef SObjListIter<Terminal> TerminalListIter;

#define FOREACH_TERMINAL(list, iter) FOREACH_OBJLIST(Terminal, list, iter)
#define MUTATE_EACH_TERMINAL(list, iter) MUTATE_EACH_OBJLIST(Terminal, list, iter)
#define SFOREACH_TERMINAL(list, iter) SFOREACH_OBJLIST(Terminal, list, iter)
#define SMUTATE_EACH_TERMINAL(list, iter) SMUTATE_EACH_OBJLIST(Terminal, list, iter)

// casting aggregates
inline ObjList<Symbol> const &toObjList(ObjList<Terminal> const &list)
  { return reinterpret_cast< ObjList<Symbol>const& >(list); }

inline SObjList<Symbol> const &toSObjList(SObjList<Terminal> const &list)
  { return reinterpret_cast< SObjList<Symbol>const& >(list); }

// format: "t1 t2 t3"
string terminalSequenceToString(TerminalList const &list);

class TerminalSet;
struct TerminalOrSet {
    bool set;
    union {
        Terminal *t;
        TerminalSet *s;
    };
};

class GrammarAnalysis;

// ----------------- TerminalSet -------------------
// used for the lookahead sets of LR items, and for the First()
// sets of production RHSs
class TerminalSet : public str::Storeable {

private:    // data
  unsigned char *bitmap;      // (owner) bitmap of terminals, indexed by
                              // terminal id; lsb of byte 0 is index 0
  int bitmapLen;              // # of bytes in 'bitmap'
#ifdef DEBUG
  TerminalSet(TerminalSet const &obj);//undefined
#endif

public:     // data
  // printing customization: when non-NULL only print tokens if
  // it includes this token, and then *only* print this one
  static Terminal const *suppressExcept;

private:    // funcs
  void init(int numTerms);
  unsigned char *getByte(int terminalIndex) const;
  int getBit(int terminalIndex) const
    { return ((unsigned)terminalIndex & 7); }

public:     // funcs
  TerminalSet(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, int numTerms=0);                   // allocate new set, initially empty
  TerminalSet(DBG_INFO_FORMAL_FIRST  Storeable const &parent, int numTerms=0);                   // allocate new set, initially empty
  TerminalSet(DBG_INFO_FORMAL_FIRST  TerminalSet const &obj);
  ~TerminalSet();

  TerminalSet& operator= (TerminalSet const &obj)
    { copy(obj); return *this; }

  void convert(GrammarAnalysis& g);

  TerminalSet(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten&);
  TerminalSet(DBG_INFO_FORMAL_FIRST  Storeable const &parent, Flatten&);
  void xfer(str::StoragePool &pool, Flatten &flat);

  // call this to re-allocate at a new size; set is emptied
  void reset(int numTerms);
                                               
  // true when the # of symbols is 0; an unfinished state
  bool nullMap() const { return bitmap==NULL; }

  bool contains(int terminalIndex) const;
  
  // NOTE: can only compare dotted productions which have the
  // same number of symbols (assertion fail otherwise)
  bool isEqual(TerminalSet const &obj) const;

  void add(int terminalIndex);
  void remove(int terminalIndex);
  void clear();

  void copy(TerminalSet const &obj);      // lengths must be the same
  bool merge(TerminalSet const &obj);     // union; returns true if merging changed set
  bool removeSet(TerminalSet const &obj); // intersect with complement; returns true if this changed set

  void print(std::ostream &os, Grammar const &g, char const *lead = ", ") const;
  void print_ext(std::ostream &os, GrammarAnalysis const &g, char const *lead = ", ") const;


};


// ---------------- Nonterminal --------------------
// something that can appear on the left-hand side of a production
// (or, emptyString, since we classify that as a nonterminal also)
class Nonterminal : public Symbol {
// ---------- representation --------
public:
  StringRef mergeParam1;    // param name for first alternative
  StringRef mergeParam2;    // and 2nd alt
  LocString const *mergeCode;      // code to resolve then

  StringRef keepParam;      // name of parameter to 'keep'
  LocString const *keepCode;       // code to decide whether to keep a reduction

  bool maximal;             // if true, use maximal munch disambiguation
  
  SObjList<Nonterminal> subsets;      // preferred subsets (for scannerless)
  SObjList<Production> productions;   //it is a copy of pointers and not an item owner list (in contrast of ObjList)

  bool deftravd = false;
  bool type_is_default = false;

protected:  // funcs
  virtual void internalPrintDDM(ostream &os) const;

public:     // funcs
  Nonterminal(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, LocString const *name, bool isEmptyString=false);
  virtual ~Nonterminal();

  Nonterminal(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten &flat);
  void xfer(str::StoragePool &pool, Flatten &flat);
  void xferSerfs(str::StoragePool &pool, Flatten &flat, Grammar &g);

  virtual void print(ostream &os, Grammar const *grammer = NULL) const;
  OSTREAM_OPERATOR(Nonterminal)

  virtual bool anyDDM() const;

  void appendProd(DBG_INFO_FORMAL_FIRST  Production *prod);

// ------ annotation ------
public:     // data
  int ntIndex;           // nonterminal index; see Grammar::computeWhatCanDeriveWhat
  bool cyclic;           // true if this can derive itself in 1 or more steps
  TerminalSet first;     // set of terminals that can be start of a string derived from 'this'
  TerminalSet follow;    // set of terminals that can follow a string derived from 'this'
  Nonterminal *superset; // inverse of 'subsets'
};

typedef SObjList<Nonterminal> NonterminalList;
typedef SObjListIter<Nonterminal> NonterminalListIter;

#define FOREACH_NONTERMINAL(list, iter) FOREACH_OBJLIST(Nonterminal, list, iter)
#define MUTATE_EACH_NONTERMINAL(list, iter) MUTATE_EACH_OBJLIST(Nonterminal, list, iter)
#define SFOREACH_NONTERMINAL(list, iter) SFOREACH_OBJLIST(Nonterminal, list, iter)
#define SMUTATE_EACH_NONTERMINAL(list, iter) SMUTATE_EACH_OBJLIST(Nonterminal, list, iter)

// casting aggregates
inline ObjList<Symbol> const &toObjList(ObjList<Nonterminal> const &list)
  { return reinterpret_cast< ObjList<Symbol>const& >(list); }

inline SObjList<Symbol> const &toSObjList(SObjList<Nonterminal> const &list)
  { return reinterpret_cast< SObjList<Symbol>const& >(list); }

// ---------------- Production --------------------
// a rewrite rule
class Production : public str::Storeable {
// ------ representation ------
public:     // types
  class RHSElt : public str::Storeable {
  public:
    Symbol *sym;                // (serf) rhs element symbol

    // tags applied to the symbols for purposes of unambiguous naming in
    // actions, and for self-commenting value as role indicators; an
    // empty tag ("") is allowed and means there is no tag
    LocString const *tag;             // tag for this symbol; can be ""

  public:
    RHSElt(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Symbol *s, LocString const *t) : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), sym(s), tag(t) {}
    ~RHSElt();

    RHSElt(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten&);
    void xfer(str::StoragePool &pool, Flatten &flat);
    void xferSerfs(Flatten &flat, Grammar &g);

  };

public:	    // data
  // fundamental context-free grammar (CFG) component
  Nonterminal * const left;     // (serf) left hand side; must be nonterminal
  ObjList<RHSElt> right;        // right hand side; terminals & nonterminals
  int precedence;               // precedence level for disambiguation (0 for none specified)
  TerminalSet *forbid;          // (nullable owner) forbidden next tokens
  bool forbid_owned;

  // user-supplied reduction action code
  LocString const *action;
  RHSElt* defaultSymbol = 0;          // default type determination (of 1-symbol-wide productions) :
                                      // analyzing its consistency

  void const * prodDecl = 0;

private:    // funcs

  void computeDerived();

public:	    // funcs
  Production(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Nonterminal *left, char const *leftTag);
  ~Production();

  Production(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten &flat);
  void xfer(str::StoragePool &pool, Flatten &flat);
  void xferSerfs(Flatten &flat, Grammar &g);

  // length *not* including emptySymbol, if present
  // UPDATE: I'm now disallowing emptySymbol from ever appearing in 'right'
  int rhsLength() const { return rhsLen; }

  // number of nonterminals on RHS
  int numRHSNonterminals() const;

  // true if the given symbol appears in 'right'
  bool rhsHasSymbol(Symbol const *sym) const;

  // retrieve the RHS as a list of symbols, rather than as a list of RHSElts
  void getRHSSymbols(DBG_INFO_FORMAL_FIRST  SymbolList &output) const;

  // append a RHS symbol
  RHSElt* append(DBG_INFO_FORMAL_FIRST  Grammar &g, Symbol *sym, LocString const *tag);

  // call this when production is built, so it can compute annotations
  // (this is called by GrammarAnalysis::initializeAuxData, from
  // inside runAnalyses)
  void finished(int numTerms);

  // find a symbol by tag; returns 1 for first RHS symbol, 2 for
  // second, etc.; returns -1 if the tag doesn't match anything
  int findTag(StringRef tag) const;

  // given an index as returned by 'findTaggedSymbol', translate that
  // back into a tag
  string symbolTag(int symbolIndex) const;

  // or translate a symbol index into a symbol
  Symbol const *symbolByIndexC(int symbolIndex) const;
  Symbol *symbolByIndex(int symbolIndex)
    { return const_cast<Symbol*>(symbolByIndexC(symbolIndex)); }

  #if 0
  // retrieve an item
  DottedProduction const *getDProdC(int dotPlace) const;
  DottedProduction *getDProd(int dotPlace)
    { return const_cast<DottedProduction*>(getDProdC(dotPlace)); }
  #endif // 0

  // add a terminal to the 'forbid' set
  void addForbid(DBG_INFO_FORMAL_FIRST  Grammar &g, Terminal *t, int totalNumTerminals);
  void addForbid(DBG_INFO_FORMAL_FIRST  Grammar &g, TerminalSet *s);

  // print 'A -> B c D' (no newline)
  string toString(bool printType = true, bool printIndex = true) const;

  // this one prints 'B c D' for above example rule
  string rhsString(bool printTags = true, bool quoteAliases = false) const;

  void print(ostream &os) const;
  OSTREAM_OPERATOR(Production)

  // print entire input syntax, with newlines, e.g.
  //   A -> B c D { return foo; }
  string toStringMore(bool printCode) const;


// ------ annotation ------
private:    // data
  int rhsLen;                   // right.count()

public:     // data
  int prodIndex;                // unique production id
  TerminalSet firstSet;         // First(RHS); computed by GrammarAnalysis::computeFirst
};

typedef SObjList<Production> ProductionList;
typedef SObjListIter<Production> ProductionListIter;

#define FOREACH_PRODUCTION(list, iter) FOREACH_OBJLIST(Production, list, iter)
#define MUTATE_EACH_PRODUCTION(list, iter) MUTATE_EACH_OBJLIST(Production, list, iter)
#define SFOREACH_PRODUCTION(list, iter) SFOREACH_OBJLIST(Production, list, iter)
#define SMUTATE_EACH_PRODUCTION(list, iter) SMUTATE_EACH_OBJLIST(Production, list, iter)

typedef ObjList<Production::RHSElt> RHSEltList;
typedef ObjListIter<Production::RHSElt> RHSEltListIter;
typedef ObjListMutator<Production::RHSElt> RHSEltListMutator;

class Environment;
class AbstractProdDecl;
class GrammarAST;

// ---------------- Grammar --------------------
// represent a grammar: nonterminals, terminals, productions, and start-symbol
class Grammar {

// ------ representation ------
public:	    // data

  virtual void thisIsAbstract() = 0;

  // ! the first !
  str::StoragePool pool;

  SObjList<Nonterminal> nonterminals;
  SObjList<Terminal> terminals;
  SObjList<Production> productions;

  string prefix0, pref;

  bool terminalCodeMapped;
  int maxCode;
  Nonterminal *startSymbol;             // (serf) a particular nonterminal

  // the special terminal for the empty string; does not appear in the
  // list of nonterminals or terminals for a grammar, but can be
  // referenced by productions, etc.; the decision to explicitly have
  // such a symbol, instead of letting it always be implicit, is
  // motivated by things like the derivability relation, where it's
  // nice to treat empty like any other symbol
  Nonterminal* emptyString;

  // sections of verbatim code emitted into the interface file, before
  // the parser context class body
  ObjList<LocString> verbatim;

  // name of the class into which the action functions are placed
  LocString const *actionClassName;
  StringRef actionClassName0;

  // verbatim action class declaration, and additional codes from
  // extension modules to append to it (but see note of 11/13/04
  // in grampar.cc)
  ObjList<LocString> actionClasses;

  // code emitted into the implementation file at the end
  ObjList<LocString> implVerbatim;

  // ---- declarative options ----
  // name of the target language; nominally "C++"
  string targetLang;

  // when true, the default dup/del is what's expected for a
  // garbage-collected system: dup() is the identity function,
  // and del() is a no-op
  bool useGCDefaults;

  // when true, unspecified merge() functions abort()
  bool defaultMergeAborts;

  // expected numbers of various anomalies; -1 means no
  // expectation has been supplied; this informtion is used
  // to control what is reported after grammar analysis
  int expectedSR;                       // shift/reduce conflicts
  int expectedRR;                       // reduce/reduce conflicts
  int expectedUNRNonterms;              // # unreachable nonterminals
  int expectedUNRTerms;                 // # unreachable terminals

public:     // funcs
  Grammar(DBG_INFO_FORMAL);                            // set everything manually
  ~Grammar();

  // read/write as binary file
  void xfer(Flatten &flat);

  // simple queries
  int numTerminals() const;
  int numNonterminals() const;


  // ---- building a grammar ----
  // declare a new token exists, with name and optional alias;
  // return false if it's already declared
  bool declareToken(DBG_INFO_FORMAL_FIRST   LocString const *symbolName, int code,
                    LocString const *alias);

  // add a new production; the rhs arg list must be terminated with a NULL
  //void addProduction(Nonterminal *lhs, Symbol *rhs, ...);

  // add a pre-constructed production
  void addProduction(DBG_INFO_FORMAL_FIRST  Production *prod);

  // ---------- outputting a grammar --------------
  // print the list of symbols with type annotations
  void printSymbolTypes(ostream &os) const;

  // print the current list of productions
  void printProductions(ostream &os, bool printCode=true) const;

  // emit C++ code to construct this grammar later
  void emitSelfCC(ostream &os) const;

  // ---- whole-grammar stuff ----
  // after adding all rules, check that all nonterminals have
  // at least one rule; also checks referential integrity
  // in actions and conditions; throw exception if there is a
  // problem
  void checkWellFormed() const;

  // output grammar in Bison's syntax
  // (coincidentally, when bison dumps its table with '-v', its table
  // dump syntax is similar to my input syntax)
  void printAsBison(ostream &os) const;

  // ---- symbol access ----
  #define SYMBOL_ACCESS(Thing)                              \
    /* retrieve, return NULL if not there */                \
    Thing const *find##Thing##C(rostring name) const;    \
    Thing *find##Thing(rostring name)                    \
      { return const_cast<Thing*>(find##Thing##C(name)); }  \
                                                            \
    /* retrieve, or create it if not already there */       \
    Thing *getOrMake##Thing(DBG_INFO_FORMAL_FIRST  LocString const *name);

  SYMBOL_ACCESS(Symbol)        // findSymbolC, findSymbol, getOrMakeSymbol
  SYMBOL_ACCESS(Terminal)      // findTerminal{C,}, getOrMakeTerminal
  SYMBOL_ACCESS(Nonterminal)   // findNonterminal{C,}, getOrMakeNonterminal
  #undef SYMBOL_ACCESS

  // map a production to a unique index
  int getProductionIndex(Production const *prod) const;
};


#endif // __GRAMMAR_H

