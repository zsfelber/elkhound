// boxprint.cc
// code for boxprint.h

#include "boxprint.h"       // this module
#include "strutil.h"        // quoted

#include <string.h>         // strlen


// ----------------------- BPRender ----------------------
BPRender::BPRender()
  : sb(),         // initially empty
    margin(72),   
    curCol(0),
    lineStartText("")
{}

BPRender::~BPRender()
{}


void BPRender::reset()
{
  sb.clear();
  sb << lineStartText;
}


void BPRender::add(rostring &text)
{
  int len = strlen(text);
  sb << text;
  curCol += len;
}

void BPRender::breakLine(int ind)
{
  sb << "\n" << lineStartText;

  for (int i=0; i < ind; i++) {
    sb << ' ';
  }

  curCol = ind;
}


// TODO fail-safe
str::string BPRender::takeAndRender(BoxPrint &bld)
{
  BPBox* /*owner*/ tree = bld.takeTree();
  tree->render(*this);
  // TODO fail-safe
  str::string ret(sb.str());
  sb.clear();
  delete tree;
  return ret;
}


// ----------------------- BPElement ---------------------
int BPElement::oneLineWidth()
{
  bool dummy;
  return oneLineWidthEx(dummy);
}

bool BPElement::isBreak() const
{
  return false;
}

bool BPElement::isForcedBreak() const
{
  return false;
}

BPElement::~BPElement()
{}


// ------------------------- BPText ----------------------
BPText::BPText(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, rostring &t)
  : BPElement(DBG_INFO_ARG_FWD_FIRST  pool), text(t)
{}
BPText::BPText(DBG_INFO_FORMAL_FIRST  str::Storeable &parent, rostring &t)
  : BPElement(DBG_INFO_ARG_FWD_FIRST  parent, sizeof(BPText)), text(t)
{}

BPText::~BPText()
{}


int BPText::oneLineWidthEx(bool &forced)
{
  forced = false;
  return text.length();
}

void BPText::render(BPRender &mgr)
{
  mgr.add(text);
}


void BPText::debugPrint(std::ostream &os, int /*ind*/) const
{
  os << "text(" << quoted(text) << ")";
}


// ------------------------ BPBreak ---------------------
BPBreak::BPBreak(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, BreakType e, int i)
  : BPElement(DBG_INFO_ARG_FWD_FIRST  pool), enabled(e),
    indent(i)
{}

BPBreak::~BPBreak()
{}

int BPBreak::oneLineWidthEx(bool &forced)
{
  if (enabled >= BT_FORCED) {
    forced = true;
    return 0;
  }
  else {
    forced = false;
    return 1;
  }
}

void BPBreak::render(BPRender &mgr)
{
  // if we're being asked to render, then this break must not be taken
  if (enabled != BT_LINE_START) {
    mgr.add(" ");
  }
}

bool BPBreak::isBreak() const
{
  return enabled;
}

bool BPBreak::isForcedBreak() const
{
  return enabled == BT_FORCED;
}

void BPBreak::debugPrint(std::ostream &os, int /*ind*/) const
{
  os << "break(en=" << (int)enabled << ", ind=" << indent << ")";
}


// ------------------------- BPBox ------------------------
BPBox::BPBox(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, BPKind k)
  : BPElement(DBG_INFO_ARG_FWD_FIRST  pool), elts(DBG_INFO_ARG0_FIRST  *this),      // initially empty
    kind(k)
{
  xassert((unsigned)k < NUM_BPKINDS);
}
BPBox::BPBox(DBG_INFO_FORMAL_FIRST  str::Storeable &parent, BPKind k)
  : BPElement(DBG_INFO_ARG_FWD_FIRST  parent, sizeof(BPBox)), elts(DBG_INFO_ARG0_FIRST  parent),      // initially empty
    kind(k)
{
  xassert((unsigned)k < NUM_BPKINDS);
}

BPBox::~BPBox()
{}


int BPBox::oneLineWidthEx(bool &forced)
{
  forced = false;
  int sum = 0;
  FOREACH_ASTLIST_NC(BPElement, elts, iter) {
    sum += iter.data()->oneLineWidthEx(forced);
    if (forced) {
      break;
    }
  }
  return sum;
}


void takeBreak(BPRender &mgr, int &startCol, BPBreak *brk)
{
  startCol += brk->indent;

  if (brk->enabled == BT_LINE_START && 
      mgr.curCol == startCol) {
    // do not add a line
  }
  else {
    // add the newline
    mgr.breakLine(startCol);
  }
}


// this function is the heart of the rendering engine
void BPBox::render(BPRender &mgr)
{
  int startCol = mgr.getCurCol();

  bool fbreak = false;
  if (kind == BP_vertical ||
      (kind == BP_correlated && (oneLineWidthEx(/*OUT*/fbreak) > mgr.remainder() ||
                                 fbreak))) {
    // take all of the breaks
    FOREACH_ASTLIST_NC(BPElement, elts, iter) {
      BPElement *elt = iter.data();
      if (elt->isBreak()) {
        takeBreak(mgr, startCol, static_cast<BPBreak*>(elt));
      }
      else {
        elt->render(mgr);
      }
    }
    return;
  }

  if (kind == BP_correlated) {
    // if we got here, we're taking none of the breaks
    FOREACH_ASTLIST_NC(BPElement, elts, iter) {
      BPElement *elt = iter.data();
      elt->render(mgr);
    }
    return;
  }

  xassert(kind == BP_sequence);

  // this cursor points to the next element that has not been rendered
  ASTListIterNC<BPElement> cursor(elts);

  // when not NULL, the cursor has just passed a break, but we haven't
  // actually decided whether to take it or not
  BPBreak *pendingBreak = NULL;

  while (!cursor.isDone()) {
    // is there room for the elements up to the first break?
    int segmentWidth = pendingBreak? 1 : 0;
    ASTListIterNC<BPElement> lookahead(cursor);
    while (!lookahead.isDone() && !lookahead.data()->isBreak()) {
      segmentWidth += lookahead.data()->oneLineWidth();
      lookahead.adv();
    }

    if (pendingBreak && segmentWidth > mgr.remainder()) {
      // take the pending break
      takeBreak(mgr, startCol, pendingBreak);
      pendingBreak = NULL;
    }

    // the segment will be put here without a preceding break
    else if (pendingBreak) {
      startCol += pendingBreak->indent;
      pendingBreak->render(mgr);
      pendingBreak = NULL;
    }

    xassert(pendingBreak == NULL);

    // render the segment
    while (!cursor.isDone() && !cursor.data()->isBreak()) {
      cursor.data()->render(mgr);
      cursor.adv();
    }

    if (!cursor.isDone()) {
      // we stopped on a break
      pendingBreak = static_cast<BPBreak*>(cursor.data());

      if (pendingBreak->isForcedBreak()) {
        // take the forced break
        takeBreak(mgr, startCol, pendingBreak);
        pendingBreak = NULL;
      }

      cursor.adv();
    }
  }

  if (pendingBreak) {
    // ended with a break.. strange, but harmless I suppose
    pendingBreak->render(mgr);

    if (pendingBreak->isForcedBreak()) {
      takeBreak(mgr, startCol, pendingBreak);
    }
  }
}


void BPBox::debugPrint(std::ostream &os, int ind) const
{           
  static char const * const map[] = {
    "vert",
    "seq",
    "corr"
  };

  os << "box(kind=" << map[kind] << ") {\n";
  ind += 2;
  
  FOREACH_ASTLIST(BPElement, elts, iter) {
    for (int i=0; i<ind; i++) {
      os << " ";
    }

    iter.data()->debugPrint(os, ind);
    os << "\n";
  }

  ind -= 2;
  for (int i=0; i<ind; i++) {
    os << " ";
  }
  os << "}";
}


// ------------------------ BoxPrint ----------------------
BPKind const BoxPrint::vert = BP_vertical;
BPKind const BoxPrint::seq  = BP_sequence;
BPKind const BoxPrint::hv   = BP_correlated;
BPKind const BoxPrint::end  = NUM_BPKINDS;


BoxPrint::BoxPrint(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool)
  : str::Storeable(DBG_INFO_ARG_FWD_FIRST  pool), boxStack(DBG_INFO_ARG_FWD_FIRST  *this, sizeof(ObjArrayStack<BPBox>)),
    levelIndent(2)
{         
  // initial vert box
  // TODO dummy, it is bad, if str::StoragePool autogrows
  boxStack.push(new (getParentRef()) BPBox(DBG_INFO_ARG0_FIRST  getParentRef(), BP_vertical));
}

/*
BoxPrint::BoxPrint(DBG_INFO_FORMAL)
  : str::Storeable(DBG_INFO_ARG_FWD), boxStack(DBG_INFO_ARG_FWD_FIRST  sizeof(ObjArrayStack<BPBox>)),
    levelIndent(2)
{
  // initial vert box
  // TODO dummy, it is bad, if str::StoragePool autogrows
  boxStack.push(new BPBox(DBG_INFO_ARG0_FIRST  BP_vertical));
}*/

BoxPrint::~BoxPrint()
{
}


void BoxPrint::append(DBG_INFO_FORMAL_FIRST  BPElement *elt)
{
  box()->elts.append(DBG_INFO_ARG_FWD_FIRST  elt);
}


BoxPrint& BoxPrint::operator<< (rostring &s)
{
    str::StoragePool *pool = constcast(getParent());
    xassert(pool);
  append(DBG_INFO_ARG0_FIRST  new (*pool) BPText(DBG_INFO_ARG0_FIRST  *pool, s));
  return *this;
}

BoxPrint& BoxPrint::operator<< (char const *s)
{
    str::StoragePool *pool = constcast(getParent());
    xassert(pool);
  append(DBG_INFO_ARG0_FIRST  new (*pool) BPText(DBG_INFO_ARG0_FIRST  *pool, s));
  return *this;
}

BoxPrint& BoxPrint::operator<< (int i)
{
  return operator<< ((stringb(i)).str().c_str());
}


BoxPrint& BoxPrint::operator<< (BPKind k)
{
  if (k == NUM_BPKINDS) {
    // close current box
    append(DBG_INFO_ARG0_FIRST  boxStack.pop());
  }
  else {
    // open new box
    str::StoragePool *pool = constcast(getParent());
    xassert(pool);
    // TODO dummy, it is bad, if str::StoragePool autogrows
    boxStack.push(new (*pool) BPBox(DBG_INFO_ARG0_FIRST  *pool, k));
  }
  return *this;
}


BoxPrint& BoxPrint::operator<< (Cmd c)
{
  switch (c) {
    default: xfailure("bad cmd");
    case sp:        append(DBG_INFO_ARG0_FIRST  new BPBreak(DBG_INFO_ARG0_FIRST  getParentRef(), BT_DISABLED, 0 /*indent*/)); break;
    case br:        append(DBG_INFO_ARG0_FIRST  new BPBreak(DBG_INFO_ARG0_FIRST  getParentRef(), BT_ENABLED, 0 /*indent*/)); break;
    case fbr:       append(DBG_INFO_ARG0_FIRST  new BPBreak(DBG_INFO_ARG0_FIRST  getParentRef(), BT_FORCED, 0 /*indent*/)); break;
    case lineStart: append(DBG_INFO_ARG0_FIRST  new BPBreak(DBG_INFO_ARG0_FIRST  getParentRef(), BT_LINE_START, 0 /*indent*/)); break;
    case ind:       append(DBG_INFO_ARG0_FIRST  new BPBreak(DBG_INFO_ARG0_FIRST  getParentRef(), BT_ENABLED, levelIndent)); break;
    case und:       append(DBG_INFO_ARG0_FIRST  new BPBreak(DBG_INFO_ARG0_FIRST  getParentRef(), BT_ENABLED, -levelIndent)); break;
  }
  return *this;
}


BoxPrint& BoxPrint::operator<< (IBreak b)
{
  append(DBG_INFO_ARG0_FIRST  new BPBreak(DBG_INFO_ARG0_FIRST  getParentRef(), BT_ENABLED, b.indent /*indent*/));
  return *this;
}


BoxPrint& BoxPrint::operator<< (Op o)
{
  return *this << sp << o.text << br;
}


BPBox* /*owner*/ BoxPrint::takeTree()
{
  // all boxes must be closed
  xassert(boxStack.length() == 1);

  BPBox *ret = boxStack.pop();

  // initialize the box stack again, in case the user wants
  // to build another tree
  // TODO dummy, it is bad, if str::StoragePool autogrows
  boxStack.push(new (getParentRef()) BPBox(DBG_INFO_ARG0_FIRST  getParentRef(), BP_vertical));

  return ret;
}


void BoxPrint::debugPrint(std::ostream &os) const
{                             
  for (int i=0; i < boxStack.length(); i++) {
    os << "----- frame -----\n";
    boxStack[i]->debugPrint(os, 0 /*ind*/);
    os << "\n";
  }
}

void BoxPrint::debugPrintCout() const
{
  debugPrint(std::cout);
}


// ------------------------ test code ----------------------
#ifdef TEST_BOXPRINT

#include <stdlib.h>       // atoi
#include "ckheap.h"       // malloc_stats

void doit(int argc, char *argv[])
{
#if !defined(DBG_INFO_ARG0) || (EXPAND(DBG_INFO_ARG0) == 0)
    str::StoragePool pool;
#else
    str::StoragePool pool(DBG_INFO_ARG0);
#endif

  STORE_NEW_REF0(pool, BoxPrint, bp);

  bp << "int foo()" << bp.br
     << "{" << bp.ind;

  bp << bp.lineStart
     << "// wazoo"
     << bp.fbr;

  bp << "printf(" << bp.seq
        << "\"hello there %d!\\n\"," << bp.br
        << "123456789"
     << bp.end << ");" << bp.br;

  bp << "bar(" << bp.seq
        << "1" << bp.op("+")
        << "2" << bp.op("+")
        << "3" << bp.op("+")
        << "4" << bp.op("+")
        << "5" << bp.op("+")
        << "6" << bp.op("+")
        << "7" << bp.op("+")
        << "8" << bp.op("+")
        << "9" << bp.op("+")
        << "10"
     << bp.end << ");" << bp.br;

  bp << "baz(" << bp.seq
        << "\"a really long line that has no optional breaks at all\""
     << bp.end << ");" << bp.br;

  bp << "zoo(" << bp.seq
        << "\"one break is here, but it is very\"," << bp.br
        << "\"far from the start\""
     << bp.end << ");" << bp.br;

  bp << "assert(" << bp.seq
        << bp.seq << "x" << bp.op("=") << "y" << bp.end << bp.op("&&")
        << bp.seq << "z" << bp.op("=") << "w" << bp.end << bp.op("&&")
        << "(" << bp.seq
           << bp.seq << "moron" << bp.op("!=") << "fool" << bp.end << bp.op("||")
           << "taxes->theRich"
        << bp.end << ")"
     << bp.end << ")" << bp.br;

  bp << bp.hv
        << "forall(" << bp.seq
           << "x," << bp.br << "y," << bp.br << "z"
        << bp.end << "). if {" << bp.ind
        << bp.seq << "x" << bp.op("==") << "yooey_more" << bp.end << ";" << bp.br
        << bp.seq << "yowza" << bp.op("!=") << "fooey" << bp.end << ";"
        << bp.und << "} /*==>*/ {" << bp.ind
        << bp.seq << "z(x,y,z)" << bp.op("==") << "3" << bp.end << ";" << bp.br
        << "ay_caramba" << ";"
        << bp.und << "};"
     << bp.end << bp.br;

  // here is a 'forall' with a comment surrounded by forced breaks
  bp << bp.hv
        << bp.lineStart
        << "// forced break comment"
        << bp.fbr
        << "forall(" << bp.seq
           << "x," << bp.br << "y," << bp.br << "z"
        << bp.end << "). if {" << bp.ind
        << bp.seq << "x" << bp.op("==") << "yooey_more" << bp.end << ";" << bp.br
        << bp.seq << "yowza" << bp.op("!=") << "fooey" << bp.end << ";"
        << bp.und << "} /*==>*/ {" << bp.ind
        << bp.seq << "z(x,y,z)" << bp.op("==") << "3" << bp.end << ";" << bp.br
        << "ay_caramba" << ";"
        << bp.und << "};"
     << bp.end << bp.br;

  // here is a 'forall' with a comment surrounded by forced breaks
  bp << bp.hv
        << bp.lineStart
        << "// forced break comment"
        << bp.fbr
        << "forall(" << bp.seq
           << "x," << bp.br << "y," << bp.br << "z"
        << bp.end << "). if {" << bp.ind
        << bp.seq << "x" << bp.op("==") << "yooey_more" << bp.end << ";" << bp.br
        << bp.seq << "yowza" << bp.op("!=") << "fooey" << bp.end << ";"
        << bp.und << "} /*==>*/ {" << bp.ind
        << bp.seq << "z(x,y,z)" << bp.op("==") << "3" << bp.end << ";" << bp.br
        << "ay_caramba" << ";"
        << bp.und << "};"
     << bp.end;

  bp << bp.und << "}" << bp.br;

  BPBox *tree = bp.takeTree();

  BPRender ren;
  ren.margin = 30;
  if (argc >= 2) {
    ren.margin = atoi(argv[1]);
  }
  std::cout << "margin: " << ren.margin << "\n";

  tree->render(ren);
  delete tree;

  std::cout << "         1    1    2    2    3    3    4    4    5    5    6    6    7\n";
  std::cout << "1---5----0----5----0----5----0----5----0----5----0----5----0----5----0\n";
  std::cout << ren.takeString();
}

int main(int argc, char *argv[])
{
  doit(argc, argv);
  //malloc_stats();
  return 0;
}

#endif // TEST_BOXPRINT
