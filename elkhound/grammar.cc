// grammar.cc            see license.txt for copyright and terms of use
// code for grammar.h

#include "grammar.h"   // this module
#include "gramanl.h"   // this module
#include "syserr.h"    // xsyserror
#include "strtokp.h"   // StrtokParse
#include "trace.h"     // trace
#include "exc.h"       // xBase
#include "strutil.h"   // quoted, parseQuotedString
#include "flatten.h"   // Flatten
#include "flatutil.h"  // various 0 helpers
#include "asthelp.h"

#include <stdarg.h>    // variable-args stuff
#include <stdio.h>     // FILE, etc.
#include <ctype.h>     // isupper
#include <stdlib.h>    // atoi


// print a variable value
#define PVAL(var) os << " " << #var "=" << var;


StringTable grammarStringTable;


// ---------------------- Symbol --------------------
Symbol::Symbol(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, LocString const *n, bool t, bool e)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), name(n),
    isTerm(t),
    isEmptyString(e),
    type(NULL),
    dupParam(NULL),
    dupCode(NULL),
    delParam(NULL),
    delCode(NULL),
    reachable(false)
{}

Symbol::~Symbol()
{}


Symbol::Symbol(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten &flat)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), name(LIT_STR_1(flat)),
    isTerm(false),
    isEmptyString(false),
    type(NULL),
    dupParam(NULL),
    dupCode(NULL),
    delParam(NULL),
    delCode(NULL)
{}

void Symbol::xfer(str::StoragePool &pool, Flatten &flat)
{
  // have to break constness to unflatten
  constcast(name)->xfer(pool, flat);
  flat.xferBool(const_cast<bool&>(isTerm));
  flat.xferBool(const_cast<bool&>(isEmptyString));

  flattenStrTable->xfer(flat, type);

  flattenStrTable->xfer(flat, dupParam);
  constcast(dupCode)->xfer(pool, flat);

  flattenStrTable->xfer(flat, delParam);
  constcast(delCode)->xfer(pool, flat);
  
  flat.xferBool(reachable);
}


int Symbol::getTermOrNontermIndex() const
{
  if (isTerminal()) {
    return asTerminalC().termIndex;
  }
  else {
    return asNonterminalC().ntIndex;
  }
}


void Symbol::print(ostream &os) const
{
  os << name;
  if (type) {
    os << "[" << type << "]";
  }
  os << ":";
  PVAL(isTerm);
}


void Symbol::printDDM(ostream &os) const
{
  // don't print anything if no handlers
  if (!anyDDM()) return;

  // print with roughly the same syntax as input
  os << "  " << (isTerminal()? "token" : "nonterm");
  if (type) {
    os << "[" << type << "]";
  }
  os << " " << name << " {\n";

  internalPrintDDM(os);

  os << "  }\n";
}


void Symbol::internalPrintDDM(ostream &os) const
{
  if (dupCode && dupCode->isNonNull()) {
    os << "    dup(" << dupParam << ") [" << dupCode << "]\n";
  }

  if (delCode && delCode->isNonNull()) {
    os << "    del(" << (delParam? *delParam : "") << ") [" << delCode << "]\n";
  }
}


bool Symbol::anyDDM() const
{
  return (dupCode&&dupCode->isNonNull()) ||
         (delCode&&delCode->isNonNull());
}


Terminal const &Symbol::asTerminalC() const
{
  xassert(isTerminal());
  return (Terminal const &)(*this);
}

Nonterminal const &Symbol::asNonterminalC() const
{
  xassert(isNonterminal());
  return (Nonterminal const &)(*this);
}


Terminal const *Symbol::ifTerminalC() const
{
  return isTerminal()? (Terminal const *)this : NULL;
}

Nonterminal const *Symbol::ifNonterminalC() const
{
  return isNonterminal()? (Nonterminal const *)this : NULL;
}



// -------------------- Terminal ------------------------
Terminal::Terminal(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten &flat)
  : Symbol(DBG_INFO_ARG_FWD_FIRST  pool, flat),
    alias(LIT_STR_1(flat)),
    classifyParam(NULL),
    classifyCode(NULL)
{}

void Terminal::xfer(str::StoragePool &pool, Flatten &flat)
{
  Symbol::xfer(pool, flat);

  constcast(alias)->xfer(pool, flat);

  flat.xferInt(precedence);
  flat.xferInt((int&)associativity);

  flat.xferInt(termIndex);

  flattenStrTable->xfer(flat, classifyParam);
  constcast(classifyCode)->xfer(pool, flat);
}


void Terminal::print(ostream &os) const
{
  os << "[" << termIndex << "]";
  if (precedence) {
    os << "(" << ::toString(associativity) << " " << precedence << ")";
  }
  os << " ";
  Symbol::print(os);
}


void Terminal::internalPrintDDM(ostream &os) const
{
  Symbol::internalPrintDDM(os);

  if (classifyCode&&classifyCode->isNonNull()) {
    os << "    classify(" << classifyParam << ") [" << *classifyCode << "]\n";
  }
}


bool Terminal::anyDDM() const
{
  return Symbol::anyDDM() ||
         (classifyCode&&classifyCode->isNonNull());
}


string Terminal::toString(bool quoteAliases) const
{
  if (alias&&alias->length() > 0) {
    if (quoteAliases) {
      return stringc << "\"" << ::toString(*alias) << "\"";
    }
    else {
      return ::toString(*alias);
    }
  }
  else {
    return ::toString(*name);
  }
}


// ----------------- Nonterminal ------------------------
Nonterminal::Nonterminal(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, LocString const *name, bool isEmpty)
  : Symbol(DBG_INFO_ARG_FWD_FIRST  pool, name, false /*terminal*/, isEmpty),
    mergeParam1(NULL),
    mergeParam2(NULL),
    mergeCode(NULL),
    keepParam(NULL),
    keepCode(NULL),
    maximal(false),
    subsets(DBG_INFO_ARG_FWD_FIRST  *this),
    ntIndex(-1),
    cyclic(false),
    first(DBG_INFO_ARG_FWD_FIRST  *this,0),
    follow(DBG_INFO_ARG_FWD_FIRST  *this,0),
    productions(DBG_INFO_ARG_FWD_FIRST  *this),
    superset(NULL)
{}

Nonterminal::~Nonterminal()
{}


Nonterminal::Nonterminal(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten &flat)
  : Symbol(DBG_INFO_ARG_FWD_FIRST  pool, flat),
    mergeParam1(NULL),
    mergeParam2(NULL),
    mergeCode(NULL),
    keepParam(NULL),
    keepCode(NULL),
    first(DBG_INFO_ARG_FWD_FIRST  pool,flat),
    follow(DBG_INFO_ARG_FWD_FIRST  pool,flat),
    subsets(DBG_INFO_ARG_FWD_FIRST  pool),
    productions(DBG_INFO_ARG_FWD_FIRST  pool),
    superset(NULL)
{}

void Nonterminal::xfer(str::StoragePool &pool, Flatten &flat)
{
  Symbol::xfer(pool, flat);

  flattenStrTable->xfer(flat, mergeParam1);
  flattenStrTable->xfer(flat, mergeParam2);
  constcast(mergeCode)->xfer(pool, flat);

  flattenStrTable->xfer(flat, keepParam);
  constcast(keepCode)->xfer(pool, flat);
}

void Nonterminal::xferSerfs(str::StoragePool &pool, Flatten &flat, Grammar &g)
{
  // annotation
  flat.xferInt(ntIndex);
  flat.xferBool(cyclic);
  first.xfer(pool, flat);
  follow.xfer(pool, flat);
}


void Nonterminal::print(ostream &os, Grammar const *grammar) const
{
  os << "[" << ntIndex << "] ";
  Symbol::print(os);

  // cyclic?
  if (cyclic) {
    os << " (cyclic!)";
  }

  if (grammar) {
    // first
    os << " first={";
    first.print(os, *grammar);
    os << "}";

    // follow
    os << " follow={";
    follow.print(os, *grammar);
    os << "}";
  }
}


void Nonterminal::internalPrintDDM(ostream &os) const
{
  Symbol::internalPrintDDM(os);
  
  if (mergeCode&&mergeCode->isNonNull()) {
    os << "    merge(" << mergeParam1 << ", " << mergeParam2
       << ") [" << *mergeCode << "]\n";
  }

  if (keepCode&&keepCode->isNonNull()) {
    os << "    keep(" << keepParam << ") [" << *keepCode << "]\n";
  }
}


bool Nonterminal::anyDDM() const
{
  return Symbol::anyDDM() ||
         (mergeCode&&mergeCode->isNonNull()) ||
         (keepCode&&keepCode->isNonNull());
}


// -------------------- TerminalSet ------------------------
STATICDEF Terminal const *TerminalSet::suppressExcept = NULL;

TerminalSet::TerminalSet(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, int numTerms) : Storeable(DBG_INFO_ARG_FWD_FIRST  pool)
{
  init(numTerms);
}

TerminalSet::TerminalSet(DBG_INFO_FORMAL_FIRST  Storeable const &parent, int numTerms) : Storeable(DBG_INFO_ARG_FWD_FIRST  parent, sizeof(TerminalSet), true)
{
  init(numTerms);
}

TerminalSet::TerminalSet(DBG_INFO_FORMAL_FIRST  int numTerms) : Storeable(DBG_INFO_ARG_FWD)
{
  init(numTerms);
}

TerminalSet::TerminalSet(DBG_INFO_FORMAL_FIRST  TerminalSet const &obj) : Storeable(DBG_INFO_ARG_FWD_FIRST  obj, true)
{
  init(obj.bitmapLen * 8);    // close enough; same # of bytes at least
  if (bitmapLen) {
      copy(obj);
  }
}

void TerminalSet::init(int numTerms)
{
  if (numTerms != 0) {
    // allocate enough space for one bit per terminal; I assume
    // 8 bits per byte
    bitmapLen = (numTerms + 7)  >> 3;
    bitmap = new unsigned char[bitmapLen];

    // initially the set will be empty
    memset(bitmap, 0, bitmapLen);
  }
  else {
    // intended for situations where reset() will be called later
    // to allocate some space
    bitmapLen = 0;
    bitmap = NULL;
  }
}

void TerminalSet::convert(GrammarAnalysis& g) {
  SObjList<Terminal>& oldts = g.allTerminals;
  SObjList<Terminal>& newts = g.terminals;
  xassert(bitmapLen == ((oldts.count()+7)>>3));


  TerminalSet dup(DBG_INFO_ARG0_FIRST  *this);
  init(newts.count());

  unsigned char const bits[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

  int ind = 0;
  SObjListIter<Terminal> iter(oldts);
  for (int i = 0; i < dup.bitmapLen; i++) {
    unsigned char byte = dup.bitmap[i];
    for (unsigned char bit = 0x01; bit && !iter.isDone(); iter.adv(), bit<<=1, ind++) {
        Terminal * t = constcast(iter.data());
        if (t && (byte&bit)) {
           try {
              xassert(ind == t->externalTermIndex);
           } catch (...) {
              std::cout << t->name << " : " << t->externalTermIndex << "->" << t->termIndex << std::endl;
              std::cout << "ind:" << ind << " t->externalTermIndex:" << t->externalTermIndex << std::endl;
              throw;
           }

           if (t->termIndex) {
               int nbyti = t->termIndex >> 3;
               int nbiti = t->termIndex & 7;

               bitmap[nbyti] |= bits[nbiti];
           }
        }
    }
  }

  if (oldts.count() || newts.count()) {
      ostream & os = trace("prec");
      os << "\nTerminalSet converted from:\n";
      dup.print_ext(os, g, "");
      os << "\n";

      os << "to             TerminalSet:\n";
      print(os, g, "");
      os << "\n";
  }

}


TerminalSet::~TerminalSet()
{
  if (bitmap) {
    delete[] bitmap;
  }
}


TerminalSet::TerminalSet(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten&) : Storeable(DBG_INFO_ARG_FWD_FIRST  pool)
  , bitmap(NULL)
{}

TerminalSet::TerminalSet(DBG_INFO_FORMAL_FIRST  Storeable const &parent, Flatten&) : Storeable(DBG_INFO_ARG_FWD_FIRST  parent, sizeof(TerminalSet), true)
  , bitmap(NULL)
{}

void TerminalSet::xfer(str::StoragePool &pool, Flatten &flat)
{
  flat.xferInt(bitmapLen);

  if (bitmapLen > 0) {
    if (flat.reading()) {
      bitmap = new unsigned char[bitmapLen];
    }
    flat.xferSimple(bitmap, bitmapLen);
  }
}


void TerminalSet::reset(int numTerms)
{
  if (bitmap) {
    delete[] bitmap;
  }
  init(numTerms);
}


unsigned char *TerminalSet::getByte(int index) const
{
  int offset = (unsigned)index >> 3;
  xassert(offset < bitmapLen);

  return bitmap + offset;
}


bool TerminalSet::contains(int index) const
{
  unsigned char *p = getByte(index);
  return ((*p >> getBit(index)) & 1) == 1;
}


bool TerminalSet::isEqual(TerminalSet const &obj) const
{
  xassert(obj.bitmapLen == bitmapLen);
  return 0==memcmp(bitmap, obj.bitmap, bitmapLen);
}


void TerminalSet::add(int index)
{
  unsigned char *p = getByte(index);
  *p |= (unsigned char)(1 << getBit(index));
}


void TerminalSet::remove(int index)
{                             
  unsigned char *p = getByte(index);
  *p &= (unsigned char)(~(1 << getBit(index)));
}


void TerminalSet::clear()
{
  memset(bitmap, 0, bitmapLen);
}


void TerminalSet::copy(TerminalSet const &obj)
{
  xassert(obj.bitmapLen == bitmapLen);
  memcpy(bitmap, obj.bitmap, bitmapLen);
}


bool TerminalSet::merge(TerminalSet const &obj)
{
  bool changed = false;
  for (int i=0; i<bitmapLen; i++) {
    unsigned before = bitmap[i];
    unsigned after = before | obj.bitmap[i];
    if (after != before) {
      changed = true;
      bitmap[i] = after;
    }
  }
  return changed;
}


bool TerminalSet::removeSet(TerminalSet const &obj)
{
  xassertdb(obj.bitmapLen == bitmapLen);
  bool changed = false;
  for (int i=0; i<bitmapLen; i++) {
    unsigned before = bitmap[i];
    unsigned after = before & ~(obj.bitmap[i]);
    if (after != before) {
      changed = true;
      bitmap[i] = after;
    }
  }
  return changed;
}


#define print_terminal_adv(termIndexField) \
      Terminal const *t = iter.data(); \
      if (!t || !contains(t->termIndexField)) continue; \
      if (suppressExcept &&                  /* suppressing..*/ \
          suppressExcept != t) continue;     /* and this isn't the exception*/ \
      if (ct++ == 0) { \
        /* by waiting until now to print this, if the set has no symbols */ \
        /* (e.g. we're in SLR(1) mode), then the comma won't be printed */ \
        /* either */ \
        os << lead; \
      } \
      else { \
        os << "/"; \
      } \
     \
      os << t->toString(); \


void TerminalSet::print(std::ostream &os, Grammar const &g, char const *lead) const
{
    int ct=0;
    SFOREACH_TERMINAL(g.terminals, iter) {
        print_terminal_adv(termIndex);
    }
}


void TerminalSet::print_ext(std::ostream &os, GrammarAnalysis const &g, char const *lead) const
{
    int ct=0;
    SFOREACH_TERMINAL(g.allTerminals, iter) {
        print_terminal_adv(externalTermIndex);
    }
}


// -------------------- Production::RHSElt -------------------------
Production::RHSElt::~RHSElt()
{}


Production::RHSElt::RHSElt(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten &flat)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), sym(NULL),
    tag(LIT_STR_1(flat))
{}

void Production::RHSElt::xfer(str::StoragePool &pool, Flatten &flat)
{
  constcast(tag)->xfer(pool, flat);
}

void Production::RHSElt::xferSerfs(Flatten &flat, Grammar &g)
{
  xferSerfPtr(flat, sym);
}



// -------------------- Production -------------------------
Production::Production(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Nonterminal *L, char const *Ltag)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), left(L),
    right(DBG_INFO_ARG_FWD_FIRST  *this),
    precedence(0),
    forbid(NULL),
    forbid_owned(false),
    rhsLen(-1),
    prodIndex(-1),
    action(NULL),
    firstSet(DBG_INFO_ARG_FWD_FIRST  *this)       // don't allocate bitmap yet
{
    constcast(pool).addPointer(forbid);
}

Production::~Production()
{
  constcast(getPoolRef()).removePointer(forbid);
  if (forbid_owned) {
      delete forbid;
  }
}


Production::Production(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten &flat)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), left(NULL), right(DBG_INFO_ARG_FWD_FIRST  *this),
    forbid(NULL),
    forbid_owned(false),
    action(LIT_STR_1(flat)),
    firstSet(DBG_INFO_ARG_FWD_FIRST  *this,flat)
{}

void Production::xfer(str::StoragePool &pool, Flatten &flat)
{
  xferObjList(pool, flat, right);
  constcast(action)->xfer(pool, flat);
  flat.xferInt(precedence);
  xferNullableOwnerPtr(pool, flat, forbid);

  flat.xferInt(rhsLen);
  flat.xferInt(prodIndex);
  firstSet.xfer(pool, flat);
}

void Production::xferSerfs(Flatten &flat, Grammar &g)
{
  // must break constness in xfer

  xferSerfPtrToSList(flat, const_cast<Nonterminal*&>(left),
                          g.nonterminals);

  // xfer right's 'sym' pointers
  MUTATE_EACH_OBJLIST(RHSElt, right, iter) {
    iter.data()->xferSerfs(flat, g);
  }

  // compute derived data
  if (flat.reading()) {
    computeDerived();
  }
}


#if 0   // optimized away, using 'rhsLen' instead
int Production::rhsLength() const
{
  if (!right.isEmpty()) {
    // I used to have code here which handled this situation by returning 0;
    // since it should now never happen, I'll check that here instead
    xassert(!right.nthC(0)->sym->isEmptyString);
  }

  return right.count();
}
#endif // 0


#if 0    // useful for verifying 'finish' is called before rhsLen
int Production::rhsLength() const
{
  xassert(rhsLen != -1);     // otherwise 'finish' wasn't called
  return rhsLen;
}
#endif // 0


int Production::numRHSNonterminals() const
{
  int ct = 0;
  FOREACH_OBJLIST(RHSElt, right, iter) {
    if (iter.data()->sym->isNonterminal()) {
      ct++;
    }
  }
  return ct;
}


bool Production::rhsHasSymbol(Symbol const *sym) const
{
  FOREACH_OBJLIST(RHSElt, right, iter) {
    if (iter.data()->sym == sym) {
      return true;
    }
  }
  return false;
}


void Production::getRHSSymbols(DBG_INFO_FORMAL_FIRST SymbolList &output) const
{
  FOREACH_OBJLIST(RHSElt, right, iter) {
    output.append(DBG_INFO_ARG_FWD_FIRST  iter.data()->sym);
  }
}


Production::RHSElt* Production::append(DBG_INFO_FORMAL_FIRST  Grammar &g, Symbol *sym, LocString const *tag)
{
  // my new design decision (6/26/00 14:24) is to disallow the
  // emptyString nonterminal from explicitly appearing in the
  // productions
  xassert(!sym->isEmptyString);

  RHSElt *r = new (g.pool) RHSElt(DBG_INFO_ARG_FWD_FIRST  g.pool, sym, tag);
  right.append(DBG_INFO_ARG_FWD_FIRST  r);
  return r;
}

void Nonterminal::appendProd(DBG_INFO_FORMAL_FIRST  Production *prod) {
  productions.append(DBG_INFO_ARG_FWD_FIRST  prod);
}


void Production::finished(int numTerms)
{
  computeDerived();
  firstSet.reset(numTerms);
}

void Production::computeDerived()
{
  rhsLen = right.count();
}


// basically strcmp but without the segfaults when s1 or s2
// is null; return true if strings are equal
// update: now that they're StringRef, simple "==" suffices
bool tagCompare(StringRef s1, StringRef s2)
{
  return s1 == s2;
}


int Production::findTag(StringRef tag) const
{
  // walk RHS list looking for a match
  ObjListIter<RHSElt> tagIter(right);
  int index=1;
  for(; !tagIter.isDone(); tagIter.adv(), index++) {
    if (tagCompare(tagIter.data()->tag->str.c_str(), tag)) {
      return index;
    }
  }

  // not found
  return -1;
}


// assemble a possibly tagged name for printing
string taggedName(rostring name, char const *tag)
{
  if (tag == NULL || tag[0] == 0) {
    return string(name);
  }
  else {
    return stringb(tag << ":" << name);
  }
}


string Production::symbolTag(int index) const
{
  // no longer have tags for LHS
  xassert(index != 0);

  // find index in RHS list
  index--;
  return right.nthC(index)->tag->str;
}


Symbol const *Production::symbolByIndexC(int index) const
{
  // check LHS
  if (index == 0) {
    return left;
  }

  // find index in RHS list
  index--;
  return right.nthC(index)->sym;
}


#if 0
DottedProduction const *Production::getDProdC(int dotPlace) const
{
  xassert(0 <= dotPlace && dotPlace < numDotPlaces);
  return &dprods[dotPlace];
}    
#endif // 0


// it's somewhat unfortunate that I have to be told the
// total number of terminals, but oh well
void Production::addForbid(DBG_INFO_FORMAL_FIRST  Grammar &g, Terminal *t, int numTerminals)
{
  if (forbid) {
     if (!forbid_owned) {
        throw std::exception();
     }
  } else {
     forbid = new (g.pool) TerminalSet(DBG_INFO_ARG_FWD_FIRST  g.pool, numTerminals);

     forbid_owned = true;
  }

  forbid->add(t->termIndex);
}

void Production::addForbid(DBG_INFO_FORMAL_FIRST  Grammar &g, TerminalSet *s)
{
  if (forbid) {
    if (forbid_owned) {
        forbid->merge(*s);
    } else {
        throw std::exception();
    }
  } else {
      forbid = s;
      forbid_owned = false;
  }
}


void Production::print(ostream &os) const
{
  os << toString();
}


string Production::toString(bool printType, bool printIndex) const
{
  // LHS "->" RHS
  stringBuilder sb(DBG_INFO_ARG0);
  if (printIndex) {
    sb << "[" << prodIndex << "] ";
  }

  sb << left->name;
  if (printType && left->type) {
    sb << "[" << left->type << "]";
  }
  sb << " -> " << rhsString();
  
  if (printType && precedence) {
    // take this as licence to print prec too
    sb << " %prec(" << precedence << ")";
  }
  return sb;
}


string Production::rhsString(bool printTags, bool quoteAliases) const
{
  stringBuilder sb(DBG_INFO_ARG0);

  if (right.isNotEmpty()) {
    // print the RHS symbols
    int ct=0;
    FOREACH_OBJLIST(RHSElt, right, iter) {
      RHSElt const &elt = *(iter.data());

      if (ct++ > 0) {
        sb << " ";
      }

      string symName(DBG_INFO_ARG0);
      if (elt.sym->isNonterminal()) {
        symName = *elt.sym->name;
      }
      else {
        // print terminals as aliases if possible
        symName = elt.sym->asTerminalC().toString(quoteAliases);
      }

      if (printTags) {
        // print tag if present
        sb << taggedName(symName, elt.tag->str.c_str());
      }
      else {
        sb << symName;
      }
    }
  }

  else {
    // empty RHS
    sb << "empty";
  }

  return sb;
}


string Production::toStringMore(bool printCode) const
{
  stringBuilder sb(DBG_INFO_ARG0);
  sb << toString();

  if (printCode && action && !action->isNull()) {
    sb << "\t\t[" << action->strref() << "]";
  }

  sb << "\n";

  return sb;
}


// ------------------ Grammar -----------------
Grammar::Grammar(DBG_INFO_FORMAL)
  :
    Storeable(DBG_INFO_ARG_FWD),
    pool(DBG_INFO_ARG_FWD),
    nonterminals(DBG_INFO_ARG_FWD),
    terminals(DBG_INFO_ARG_FWD),
    productions(DBG_INFO_ARG_FWD),
    prefix0(DBG_INFO_ARG_FWD), pref(DBG_INFO_ARG_FWD),
    startSymbol(NULL),
    targetLang(DBG_INFO_ARG_FWD_FIRST  "C++"),
    useGCDefaults(false),
    defaultMergeAborts(false),
    expectedSR(-1),
    expectedRR(-1),
    expectedUNRNonterms(-1),
    expectedUNRTerms(-1),
    terminalCodeMapped(0),
    maxCode(0),
    verbatim(DBG_INFO_ARG_FWD),
    actionClassName(NULL),
    actionClasses(DBG_INFO_ARG_FWD),
    implVerbatim(DBG_INFO_ARG_FWD)
{
    emptyString = new (pool)
      Nonterminal(DBG_INFO_ARG_FWD_FIRST  pool, LIT_STR_2(HERE_SOURCELOC, "empty"),
                true /*isEmptyString*/);
    pool.addPointer(emptyString);
}

Grammar::Grammar(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool)
  :
    Storeable(DBG_INFO_ARG_FWD_FIRST  pool, false, sizeof(Grammar)),
    pool(DBG_INFO_ARG_FWD_FIRST  pool, true),
    nonterminals(DBG_INFO_ARG_FWD_FIRST  *this),
    terminals(DBG_INFO_ARG_FWD_FIRST  *this),
    productions(DBG_INFO_ARG_FWD_FIRST  *this),
    prefix0(DBG_INFO_ARG_FWD), pref(DBG_INFO_ARG_FWD),
    startSymbol(NULL),
    targetLang(DBG_INFO_ARG_FWD_FIRST  "C++"),
    useGCDefaults(false),
    defaultMergeAborts(false),
    expectedSR(-1),
    expectedRR(-1),
    expectedUNRNonterms(-1),
    expectedUNRTerms(-1),
    terminalCodeMapped(0),
    maxCode(0),
    verbatim(DBG_INFO_ARG_FWD_FIRST  *this),
    actionClassName(NULL),
    actionClasses(DBG_INFO_ARG_FWD_FIRST  *this),
    implVerbatim(DBG_INFO_ARG_FWD_FIRST  *this)
{
    emptyString = new (pool)
      Nonterminal(DBG_INFO_ARG_FWD_FIRST  pool, LIT_STR_2(HERE_SOURCELOC, "empty"),
                true /*isEmptyString*/);
    pool.addPointer(emptyString);
}

//Grammar::Grammar(DBG_INFO_FORMAL_FIRST  Grammar const &cpy) :


Grammar::~Grammar()
{}

void Grammar::xfer(Flatten &flat)
{
  // owners
  flat.checkpoint(0xC7AB4D86);
  xferSObjList(pool, flat, nonterminals);
  xferSObjList(pool, flat, terminals);
  xferSObjList(pool, flat, productions);

  // emptyString is const

  xferObjList(pool, flat, verbatim);

  constcast(actionClassName)->xfer(pool, flat);
  xferObjList(pool, flat, actionClasses);

  xferObjList(pool, flat, implVerbatim);
                               
  targetLang.xfer(flat);
  flat.xferBool(useGCDefaults);
  flat.xferBool(defaultMergeAborts);

  flat.xferInt(expectedSR);
  flat.xferInt(expectedRR);
  flat.xferInt(expectedUNRNonterms);
  flat.xferInt(expectedUNRTerms);

  // serfs
  flat.checkpoint(0x8580AAD2);

  SMUTATE_EACH_OBJLIST(Nonterminal, nonterminals, nt) {
    nt.data()->xferSerfs(pool, flat, *this);
  }
  SMUTATE_EACH_OBJLIST(Production, productions, p) {
    p.data()->xferSerfs(flat, *this);
  }

  xferSerfPtrToSList(flat, startSymbol, nonterminals);

  flat.checkpoint(0x2874DB95);
}

int Grammar::numTerminals() const
{
  return terminals.count();
}

int Grammar::numNonterminals() const
{                                
  // everywhere, we regard emptyString as a nonterminal
  return nonterminals.count() + 1;
}

void Grammar::printSymbolTypes(ostream &os) const
{
  os << "Grammar terminals with types or precedence:\n";
  SFOREACH_OBJLIST(Terminal, terminals, term) {
    Terminal const &t = *(term.data());
    t.printDDM(os);
    if (t.precedence) {
      os << "  " << t.name << " " << ::toString(t.associativity)
         << " %prec " << t.precedence << endl;
    }
  }

  os << "Grammar nonterminals with types:\n";
  SFOREACH_OBJLIST(Nonterminal, nonterminals, nt) {
    nt.data()->printDDM(os);
  }
}


void Grammar::printProductions(ostream &os, bool code) const
{
  os << "Grammar productions:\n";
  for (SObjListIter<Production> iter(productions);
       !iter.isDone(); iter.adv()) {
    os << "  " << iter.data()->toStringMore(code);
  }
}


#if 0
void Grammar::addProduction(Nonterminal *lhs, Symbol *firstRhs, ...)
{
  va_list argptr;                   // state for working through args
  Symbol *arg;
  va_start(argptr, firstRhs);       // initialize 'argptr'

  Production *prod = new Production(lhs, NULL /*tag*/);
  prod->append(firstRhs, NULL /*tag*/);
  for(;;) {
    arg = va_arg(argptr, Symbol*);  // get next argument
    if (arg == NULL) {
      break;    // end of list
    }

    prod->append(arg, NULL /*tag*/);
  }

  addProduction(prod);
}
#endif // 0


void Grammar::addProduction(DBG_INFO_FORMAL_FIRST  Production *prod)
{
  // I used to add emptyString if there were 0 RHS symbols,
  // but I've now switched to not explicitly saying that

  prod->prodIndex = productions.count();
  productions.append(DBG_INFO_ARG_FWD_FIRST  prod);

  // if the start symbol isn't defined yet, we can here
  // implement the convention that the LHS of the first
  // production is the start symbol
  // -> createEarlyRule
  //if (startSymbol == NULL) {
  //  startSymbol = prod->left;
  //}
}


// add a token to those we know about
bool Grammar::declareToken(DBG_INFO_FORMAL_FIRST  LocString const *symbolName, int code,
                           LocString const *alias)
{
  // verify that this token hasn't been declared already
  if (findSymbolC(*symbolName)) {
    cout << "token " << symbolName << " has already been declared\n";
    return false;
  }

  int index = terminals.count();

  // create a new terminal class
  Terminal *term = getOrMakeTerminal(DBG_INFO_ARG_FWD_FIRST  symbolName);

  // assign fields specified in %token declaration
  term->termIndex = index;
  term->termCode = code;
  term->alias = alias;

  return true;
}


// well-formedness check
void Grammar::checkWellFormed() const
{
  // after removing some things, now there's nothing to check...
}


// syntax for identifying tokens in Bison output
string bisonTokenName(Terminal const *t)
{
  // this worked with older versions of Bison
  //return stringc << "\"" << t->name << "\"";

  // but the newer ones don't like quoted terminal names..
  return t->name->str;
}

// print the grammar in a form that Bison likes
void Grammar::printAsBison(ostream &os) const
{
  os << "/* automatically generated grammar */\n\n";

  os << "/* -------- tokens -------- */\n";
  SFOREACH_TERMINAL(terminals, term) {
    // I'll surround all my tokens with quotes and see how Bison likes it
    // TODO: the latest bison does *not* like it!
    os << "%token " << bisonTokenName(term.data()) << " "
       << term.data()->termIndex << "\n";
  }
  os << "\n\n";

  os << "/* -------- precedence and associativity ---------*/\n"
        "/* low precedence */\n";
  {
    // first, compute the highest precedence used anywhere in the grammar
    int highMark=0;
    SFOREACH_TERMINAL(terminals, iter) {
      highMark = max(iter.data()->precedence, highMark);
    }
            
    // map AssocKind to bison declaration; map stuff bison doesn't
    // have to %nonassoc
    static char const * const kindMap[NUM_ASSOC_KINDS] =
      { "%left", "%right", "%nonassoc", "%nonassoc", "%nonassoc" };

    // now iterate over the precedence levels (level 0 is skipped
    // because it means 'unspecified')
    for (int level=1; level <= highMark; level++) {
      AssocKind kind = NUM_ASSOC_KINDS;   // means we haven't seen any kind yet
      SFOREACH_TERMINAL(terminals, iter) {
        Terminal const *t = iter.data();

        if (t->precedence == level) {
          if (kind == NUM_ASSOC_KINDS) {
            // first token at this level
            kind = t->associativity;
            os << kindMap[kind];
          }
          else if (kind != t->associativity) {
            xfailure("different associativities at same precedence?!");
          }

          // print the token itself
          os << " " << bisonTokenName(t);
        }
      }

      // end of the level
      os << "\n";
    }
  }
  os << "/* high precedence */\n"
        "\n\n";

  os << "/* -------- productions ------ */\n"
        "%%\n\n";
  // print every nonterminal's rules
  SFOREACH_NONTERMINAL(nonterminals, nt) {
    // look at every rule where this nonterminal is on LHS
    bool first = true;
    SFOREACH_PRODUCTION(productions, prod) {
      if (prod.data()->left == nt.data()) {

        if (first) {
          os << nt.data()->name << ":";
        }
        else {       
          os << "\n";
          INTLOOP(i, 0, nt.data()->name->length()) {
            os << " ";
          }
          os << "|";
        }

        // print RHS symbols
        FOREACH_OBJLIST(Production::RHSElt, prod.data()->right, symIter) {
          Symbol const *sym = symIter.data()->sym;
          if (sym != emptyString) {
            if (sym->isTerminal()) {
              os << " " << bisonTokenName(&( sym->asTerminalC() ));
            }
            else {
              os << " " << sym->name;
            }
          }
        }

        // or, if empty..
        if (prod.data()->rhsLength() == 0) {
          os << " /* empty */";
        }

        // precedence?
        if (prod.data()->precedence) {
          // search for a terminal with the required precedence level
          bool found=false;
          SFOREACH_TERMINAL(terminals, iter) {
            if (iter.data()->precedence == prod.data()->precedence) {
              // found suitable token
              os << " %prec " << bisonTokenName(iter.data());
              found = true;
              break;
            }
          }
          if (!found) {
            cout << "warning: cannot find token for precedence level "
                 << prod.data()->precedence << endl;
            os << " /* no token precedence level "/* */
               << prod.data()->precedence << " */";
          }
        }

        // dummy action to help while debugging
        os << " { $$=" << prod.data()->prodIndex << "; }";

        first = false;
      }
    }

    if (first) {
      // no rules..
      os << "/* no rules for " << nt.data()->name << " */";
    }
    else {
      // finish the rules with a semicolon
      os << "\n";
      INTLOOP(i, 0, nt.data()->name->length()) {
        os << " ";
      }
      os << ";";
    }

    os << "\n\n";
  }
}



// ------------------- symbol access -------------------
Nonterminal const *Grammar::findNonterminalC(rostring name) const
{
  // check for empty first, since it's not in the list
  if (emptyString->name->equals(name)) {
    return emptyString;
  }

  SFOREACH_NONTERMINAL(nonterminals, iter) {
    if (iter.data()->name->equals(name)) {
      return iter.data();
    }
  }
  return NULL;
}


Terminal const *Grammar::findTerminalC(rostring name) const
{
  SFOREACH_TERMINAL(terminals, iter) {
    if (iter.data()->name->equals(name) ||
        iter.data()->alias->equals(name)) {
      return iter.data();
    }
  }
  return NULL;
}


Symbol const *Grammar::findSymbolC(rostring name) const
{
  // try nonterminals
  Nonterminal const *nt = findNonterminalC(name);
  if (nt) {
    return nt;
  }

  // now try terminals; if it fails, we fail
  return findTerminalC(name);
}



Nonterminal *Grammar::getOrMakeNonterminal(DBG_INFO_FORMAL_FIRST  LocString const *name)
{
  Nonterminal *nt = findNonterminal(*name);
  if (nt != NULL) {
    return nt;
  }

  nt = new (pool) Nonterminal(DBG_INFO_ARG_FWD_FIRST  pool, name);
  nonterminals.append(DBG_INFO_ARG_FWD_FIRST  nt);
  return nt;
}

Terminal *Grammar::getOrMakeTerminal(DBG_INFO_FORMAL_FIRST  LocString const *name)
{
  Terminal *term = findTerminal(*name);
  if (term != NULL) {
    return term;
  }

  term = new (pool) Terminal(DBG_INFO_ARG_FWD_FIRST  pool, name);
  terminals.append(DBG_INFO_ARG_FWD_FIRST  term);
  return term;
}

Symbol *Grammar::getOrMakeSymbol(DBG_INFO_FORMAL_FIRST  LocString const *name)
{
  Symbol *sym = findSymbol(*name);
  if (sym != NULL) {
    return sym;
  }

  // Since name is not already defined, we don't know whether
  // it will be a nonterminal or a terminal.  For now, I will
  // use the lexical convention that nonterminals are
  // capitalized and terminals are not.
  if (isupper((*name)[0])) {
    return getOrMakeNonterminal(DBG_INFO_ARG_FWD_FIRST  name);
  }
  else {
    return getOrMakeTerminal(DBG_INFO_ARG_FWD_FIRST  name);
  }
}


int Grammar::getProductionIndex(Production const *prod) const
{
  int ret = productions.indexOf(prod);
  xassert(ret != -1);
  return ret;
}


string symbolSequenceToString(SymbolList const &list)
{
  stringBuilder sb(DBG_INFO_ARG0);   // collects output

  bool first = true;
  SFOREACH_SYMBOL(list, sym) {
    if (!first) {
      sb << " ";
    }

    if (sym.data()->isTerminal()) {
      sb << sym.data()->asTerminalC().toString();
    }
    else {
      sb << sym.data()->name;
    }
    first = false;
  }

  return sb;
}


string terminalSequenceToString(TerminalList const &list)
{
  // this works because access is read-only
  return symbolSequenceToString(reinterpret_cast<SymbolList const&>(list));
}


// ------------------ emitting C++ code ---------------------
#if 0     // not done
void Grammar::emitSelfCC(ostream &os) const
{
  os << "void buildGrammar(Grammar *g)\n"
        "{\n";

  FOREACH_OBJLIST(Terminal, terminals, termIter) {
    Terminal const *term = termIter.data();

    os << "g->declareToken(" << term->name
       << ", " << term->termIndex
       << ", " << quoted(term->alias)
       << ");\n";
  }

  FOREACH_OBJLIST(Nonterminal, nonterminals, ntIter) {
    Nonterminal const *nt = ntIter.data();

    os << ...
  }

  os << "}\n";

  // todo: more
}
#endif // 0
