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

void VoidTailList::prepend(Storeable *newitem)
{
  VoidList::prepend(newitem);
  if (!tail) {
    tail = top;
  }
}

void VoidTailList::append(Storeable *newitem)
{
  if (isEmpty()) {
    prepend(newitem);
  }
  else {
    // payoff: constant-time append
    tail->next = new (npool) VoidNode(npool, newitem, NULL);
    tail = tail->next;
  }
}

void VoidTailList::appendAll(VoidTailList const &tail)
{
  xassert(tail.npool.getExtPtrsLength() == 2);

  if (this->tail) {
      this->tail->next = tail.top;
  }
  if (tail.tail) {
      npool.removePointer(this->tail);
      this->tail = tail.tail;
  }
  ExternalPtr ptrs[] = { (ExternalPtr)&this->tail };
  StoragePool childView;
  npool.append(tail.npool, childView, ptrs, ptrs+1);

  /*VoidList::appendAll(src);
  if (!tail) {
      tail = top;
  }
  if (tail) {
      while (tail->next) tail = tail->next;
  }*/
}

void VoidTailList::appendAllNew(VoidTailList const &tail, VoidEq eq)
{
    VoidList::appendAllNew(tail, eq);
    adjustTails();
}

void VoidTailList::reappendAll(VoidTailList const &tail, VoidEq eq)
{
  VoidList::removeItems(tail, eq);
  this->tail = top;
  adjustTails();
  appendAll(tail);
}

void VoidTailList::insertAt(Storeable *newitem, int index)
{
  VoidList::insertAt(newitem, index);
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


Storeable *VoidTailList::removeFirst()
{
  xassert(top);
  if (top == tail) {
    tail = NULL;
  }
  Storeable *retval = top->data;
  VoidNode *tmp = top;
  top = top->next;
  delete tmp;
  return retval;
}

Storeable *VoidTailList::removeLast()
{
  xassert(top);
  if (top == tail) {
    return removeFirst();
  }
  
  VoidNode *before = top;
  while (before->next != tail) {
    before = before->next;
  }
  Storeable *retval = tail->data;
  delete tail;
  tail = before;
  tail->next = NULL;
  return retval;
}

Storeable *VoidTailList::removeAt(int index)
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
  Storeable *retval = toDelete->data;
  before->next = toDelete->next;
  delete toDelete;

  return retval;
}

void VoidTailList::removeAll()
{
  VoidList::removeAll();
  tail = NULL;
}

bool VoidTailList::prependUnique(Storeable *newitem)
{
  bool retval = VoidList::prependUnique(newitem);
  adjustTail();
  return retval;
}

bool VoidTailList::appendUnique(Storeable *newitem)
{
  bool retval = VoidList::appendUnique(newitem);
  adjustTail();
  return retval;
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
  VoidTailList list;
  Integer zero(0), one(1), two(2), three(3);

  // This isn't a very exhaustive test; it's mainly to check that
  // selfCheck doesn't do anything really stupid (it used to).

  list.selfCheck();

  list.append(&two);     list.selfCheck();
  list.prepend(&one);    list.selfCheck();
  list.append(&three);   list.selfCheck();
  list.prepend(&zero);   list.selfCheck();

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
