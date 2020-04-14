// vdtllist.cc            see license.txt for copyright and terms of use
// code for vdtllist.h

#include "vdtllist.h"      // this module
#include "int.h"

/*
void VoidTailList::steal(VoidTailList *src, bool deleteOrig)
{
  if (src) {
    npool.steal(src->npool);
    top = src->top;
    tail = src->tail;
    src->top = NULL;
    src->tail = NULL;
    if (deleteOrig) {
        delete src;
    }
  }
  else {
    top = NULL;
    tail = NULL;
  }
}
*/

void VoidTailList::prepend(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem)
{
  VoidList::prepend(DBG_INFO_ARG_FWD_FIRST  newitem);
  if (!tail) {
    tail = top;
  }
}

void VoidTailList::append(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem)
{
  if (isEmpty()) {
    prepend(DBG_INFO_ARG_FWD_FIRST  newitem);
  }
  else {
    // payoff: constant-time append
    tail->next = new (npool) VoidNode(DBG_INFO_ARG_FWD_FIRST  npool, newitem, NULL);
    tail = tail->next;
  }
}

void VoidTailList::appendAll(VoidTailList const &tail)
{
  this->tail = VoidList::appendAll(tail, this->tail, tail.tail);
}

void VoidTailList::prependAll(VoidTailList const &head)
{
  VoidNode *newTail = VoidList::prependAll(head, head.tail);
  if (newTail) {
      this->tail = newTail;
  }
}

void VoidTailList::appendAllNew(VoidTailList const &tail, VoidEq eq)
{
    VoidList::appendAllNew(tail, eq);
    adjustTails();
}

void VoidTailList::reappendAll(VoidTailList const &tail, VoidEq eq)
{
  if (VoidList::removeItems(tail, eq)) {
      this->tail = top;
      adjustTails();
  }
  appendAll(tail);
}

void VoidTailList::insertAt(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem, int index)
{
  VoidList::insertAt(DBG_INFO_ARG_FWD_FIRST  newitem, index);
  adjustTail();
}

void VoidTailList::concat(VoidTailList &srcList)
{
  // grab what will be the tail of the concatenated list
  VoidNode *catTail = srcList.top? srcList.tail : tail;

  // build proper backbone structure
  VoidList::concat(srcList);

  // fix tails
  tail = catTail;
  srcList.tail = NULL;
}

void VoidTailList::adjustTail()
{
  if (tail) {
      if (tail->next) tail = tail->next;
      xassert(tail->next == NULL);
  } else {
      tail = top;
      xassert(!tail || tail->next == NULL);
  }
}

void VoidTailList::adjustTails()
{
    if (!tail) {
        tail = top;
    }
    if (tail) {
        while (tail->next) tail = tail->next;
    }
}


str::Storeable *VoidTailList::removeFirst()
{
  xassert(top);
  if (top == tail) {
    tail = NULL;
  }
  str::Storeable *retval = top->data;
  VoidNode *tmp = top;
  top = top->next;
  delete tmp;
  return retval;
}

str::Storeable *VoidTailList::removeLast()
{
  xassert(top);
  if (top == tail) {
    return removeFirst();
  }
  
  VoidNode *before = top;
  while (before->next != tail) {
    before = before->next;
  }
  str::Storeable *retval = tail->data;
  delete tail;
  tail = before;
  tail->next = NULL;
  return retval;
}

str::Storeable *VoidTailList::removeAt(int index)
{
  xassert(top);
  if (index == 0) {
    return removeFirst();
  }

  VoidNode *before = top;    // will point to node before one to be removed
  index--;                    
  while (index > 0) {
    before = before->next;
    index--;
  }             
  xassert(index == 0);
  
  // fix 'tail' if necessary
  if (tail == before->next) {
    tail = before;
  }                         

  // patch around before->next
  VoidNode *toDelete = before->next;
  str::Storeable *retval = toDelete->data;
  before->next = toDelete->next;
  delete toDelete;

  return retval;
}

void VoidTailList::removeAll()
{
  VoidList::removeAll();
  tail = NULL;
}

bool VoidTailList::prependUnique(str::Storeable *newitem)
{
  bool retval = VoidList::prependUnique(DBG_INFO_ARG0_FIRST  newitem);
  adjustTail();
  return retval;
}

bool VoidTailList::appendUnique(str::Storeable *newitem)
{
  bool retval = VoidList::appendUnique(DBG_INFO_ARG0_FIRST  newitem);
  adjustTail();
  return retval;
}
void VoidTailList::reverse() {
    tail = top;
    VoidList::reverse();
}

void VoidTailList::selfCheck() const
{
  VoidList::selfCheck();

  if (isNotEmpty()) {
    // get last node
    VoidNode *n = top;
    while (n->next) {
      n = n->next;
    }

    // 'tail' should be the last one
    xassert(tail == n);
  }
  else {
    xassert(tail == NULL);
  }
}


// --------------------- test code ------------------
#ifdef TEST_VDTLLIST           

#include <stdio.h>    // printf

int main()
{
#if !defined(DBG_INFO_ARG0) || (EXPAND(DBG_INFO_ARG0) == 0)
    VoidTailList list;
#else
    VoidTailList list(DBG_INFO_ARG0);
#endif
  Integer zero(DBG_INFO_ARG0_FIRST  0),
          one(DBG_INFO_ARG0_FIRST  1),
          two(DBG_INFO_ARG0_FIRST  2),
          three(DBG_INFO_ARG0_FIRST  3);

  // This isn't a very exhaustive test; it's mainly to check that
  // selfCheck doesn't do anything really stupid (it used to).

  list.selfCheck();

  list.append(DBG_INFO_ARG0_FIRST  &two);     list.selfCheck();
  list.prepend(DBG_INFO_ARG0_FIRST  &one);    list.selfCheck();
  list.append(DBG_INFO_ARG0_FIRST  &three);   list.selfCheck();
  list.prepend(DBG_INFO_ARG0_FIRST  &zero);   list.selfCheck();

  xassert(list.nth(0) == &zero);
  xassert(list.nth(1) == &one);
  xassert(list.nth(2) == &two);
  xassert(list.nth(3) == &three);

  list.removeAll();
  list.selfCheck();

  printf("vdtllist works\n");

  return 0;
}

#endif // TEST_VDTLLIST
