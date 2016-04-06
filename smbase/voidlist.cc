// voidlist.cc            see license.txt for copyright and terms of use
// code for voidlist.h

#include "voidlist.h"   // this module
#include "breaker.h"    // breaker
#include "str.h"        // stringc
#include "int.h"     // checkHeap
#include "ckheap.h"     // checkHeap

#include <stdlib.h>     // rand()
#include <stdio.h>      // printf()
#include <ios>


VoidList::VoidList(DBG_INFO_FORMAL_FIRST  VoidList const &src, size_t size_of, bool move)
  : str::Storeable(DBG_INFO_ARG_FWD_FIRST  src, size_of?size_of:sizeof(VoidList), false),
    npool(DBG_INFO_ARG0_FIRST  src.npool, false, move ?  str::StoragePool::Cp_Move :  str::StoragePool::Cp_All)
{
    chk_assign(src);
}

void VoidList::assign(VoidList const &src, size_t size_of, bool move) {
    str::Storeable::assign(src, size_of?size_of:sizeof(VoidList));
    npool.assign(src.npool, move ?  str::StoragePool::Cp_Move :  str::StoragePool::Cp_All);
    chk_assign(src);
}

void VoidList::chk_assign(VoidList const &src) {
    xassert(__kind == src.__kind && __parentVector == src.__parentVector);

    top = src.top;
    ExternalPtr ptrs[] = { (ExternalPtr)&top };
    npool.convertExternalPointers(src.npool, ptrs, ptrs+1);
}



// # of items in list
int VoidList::count() const
{
  int ct=0;
  for(VoidNode *p = top; p; p = p->next) {
    ct++;
  }
  return ct;
}


// get particular item, 0 is first (item must exist)
str::Storeable *VoidList::nth(int which) const
{
  VoidNode *p;
  xassert(which>=0);
  for (p = top; which > 0; which--) {
    xassert(p);
    p = p->next;
  }
  if (p == NULL) {
    xfailure(stringc << "asked for list element "
                     << (count()+which) << " (0-based) but list only has "
                     << count() << " elements");
  }
  return p->data;
}


// fail assertion if list fails integrity check
void VoidList::selfCheck() const
{
  if (!top) {
    return;
  }

  npool.selfCheck();

  // The technique here is the fast/slow list traversal to find loops (which
  // are the only way a singly-linked list can be bad). Basically, if there
  // is a loop then the fast ptr will catch up to and equal the slow one; if
  // not, the fast will simply find the terminating null. It is the only way
  // I know of to find loops in O(1) space and O(n) time.

  VoidNode *slow=top, *fast=top->next;
  while (fast && fast != slow) {
    xassert(npool.findChild(slow));

    // check heap properties
    checkHeapNode(fast);

    slow = slow->next;
    fast = fast->next;
    if (fast) {
      checkHeapNode(fast);
      fast = fast->next;      // usually, fast jumps 2 spots per slow's 1
    }
  }

  if (fast == slow) {
    xfailure("linked list has a cycle");
  }
  else {
    return;         // no loop
  }
}


void VoidList::checkHeapDataPtrs() const
{
  for (VoidNode *p=top; p!=NULL; p=p->next) {
    checkHeapNode(p->data);
  }
}


void VoidList::checkUniqueDataPtrs() const
{
  for (VoidNode *p=top; p!=NULL; p=p->next) {
    // run 'q' from 'top' to 'p', looking for any
    // collisions with 'p->data'
    for (VoidNode *q=top; q!=p; q=q->next) {
      if (q->data == p->data) {
        xfailure("linked list with duplicate element");
      }
    }
  }
}


// insert at front
void VoidList::prepend(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem)
{
  top = new (npool) VoidNode(DBG_INFO_ARG_FWD_FIRST  npool, newitem, top);
}


// insert at rear
VoidNode* VoidList::append(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem)
{
  if (!top) {
    prepend(DBG_INFO_ARG_FWD_FIRST  newitem);
    return top;
  }
  else {
    VoidNode *p;
    for (p = top; p->next; p = p->next)
      {}
    p->next = new (npool) VoidNode(DBG_INFO_ARG_FWD_FIRST  npool, newitem);
    return p->next;
  }
}


// insert at particular point, index of new node becomes 'index'
void VoidList::insertAt(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem, int index)
{
  if (index == 0 || isEmpty()) {
    // special case prepending or an empty list
    xassert(index == 0);     // if it's empty, index should be 0
    prepend(DBG_INFO_ARG_FWD_FIRST  newitem);
  }

  else {
    // Looking at the loop below, the key things to note are:
    //  1. If index started as 1, the loop isn't executed, and the new
    //     node goes directly after the top node.
    //  2. p is never allowed to become NULL, so we can't walk off the
    //     end of the list.

    index--;
    VoidNode *p;
    for (p = top; p->next && index; p = p->next) {
      index--;
    }
    xassert(index == 0);
      // if index isn't 0, then index was greater than count()

    // put a node after p
    VoidNode *n = new (npool) VoidNode(DBG_INFO_ARG_FWD_FIRST  npool, newitem);
    n->next = p->next;
    p->next = n;
  }
}


void VoidList::insertSorted(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem, VoidDiff const diff, str::Storeable const *extra)
{
  // put it first?
  if (!top ||
      diff(newitem, top->data, extra) <= 0) {                // newitem <= top
    prepend(DBG_INFO_ARG_FWD_FIRST  newitem);
    return;
  }

  // we will be considering adding 'newitem' *after* cursor, as we go
  VoidNode *cursor = top;
  while (cursor->next != NULL &&
         diff(cursor->next->data, newitem, extra) < 0) {     // cursor->next < newitem
    cursor = cursor->next;
  }
  
  // insert 'newitem' after 'cursor'
  VoidNode *newNode = new (npool) VoidNode(DBG_INFO_ARG_FWD_FIRST  npool, newitem);
  newNode->next = cursor->next;
  cursor->next = newNode;
}


// ----------------- list-as-set stuff -------------------
// get the index of an item's first occurrance
int VoidList::indexOf(str::Storeable const *item) const
{
  int index = 0;
  for (VoidNode *p = top; p != NULL; p = p->next, index++) {
    if (p->data == item) {
      return index;
    }
  }
  return -1;
}

int VoidList::indexOf(str::Storeable const *item, VoidEq const eq) const
{
  int index = 0;
  for (VoidNode *p = top; p != NULL; p = p->next, index++) {
    if (eq(p->data, item)) {
      return index;
    }
  }
  return -1;
}


int VoidList::indexOfF(str::Storeable const *item) const
{
  int ret = indexOf(item);
  xassert(ret >= 0);
  return ret;
}


bool VoidList::prependUnique(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem)
{
  if (!contains(newitem)) {
    prepend(DBG_INFO_ARG_FWD_FIRST  newitem);
    return true;
  }
  else {
    return false;   // no change
  }
}


bool VoidList::appendUnique(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem)
{
  if (!top) {
    prepend(DBG_INFO_ARG_FWD_FIRST  newitem);
    return true;
  }

  // walk to the end of list, while checking to
  // see if 'newitem' is already in the list
  VoidNode *p;
  for (p = top; p->next; p = p->next) {
    if (p->data == newitem) {
      return false;      // item is already on list; no change
    }
  }
  if (p->data == newitem) {
    return false;
  }

  p->next = new (npool) VoidNode(DBG_INFO_ARG_FWD_FIRST  npool, newitem);
  return true;
}


bool VoidList::removeIfPresent(str::Storeable const *item)
{
  // for now, not a real fast implementation
  int index = indexOf(item);
  if (index == -1) {
    return false;   // not there
  }
  else {
    removeAt(index);
    return true;
  }
}

bool VoidList::removeIfPresent(str::Storeable const *item, VoidEq eq)
{
  // for now, not a real fast implementation
  int index = indexOf(item, eq);
  if (index == -1) {
    return false;   // not there
  }
  else {
    removeAt(index);
    return true;
  }
}

bool VoidList::removeItems(VoidList const &lst, VoidEq eq) {   // remove all; return true if changed
  bool removed = false;
  for(VoidNode *p = lst.top; p; p = p->next) {
      removed |= removeIfPresent(p->data, eq);
  }
  return removed;
}


void VoidList::removeItem(str::Storeable const *item)
{
  bool wasThere = removeIfPresent(item);
  xassert(wasThere);
}

// ----------------- end of list-as-set stuff -------------------


str::Storeable *VoidList::removeAt(int index)
{
  if (index == 0) {
    xassert(top != NULL);   // element must exist to remove
    VoidNode *temp = top;
    str::Storeable *retval = temp->data;
    top = top->next;
    delete temp;
    return retval;
  }

  // will look for the node just before the one to delete
  index--;

  VoidNode *p;	   
  for (p = top; p->next && index>0;
       p = p->next, index--)
    {}

  if (p->next) {
    // index==0, so p->next is node to remove
    VoidNode *temp = p->next;
    str::Storeable *retval = temp->data;
    p->next = p->next->next;
    delete temp;
    return retval;
  }
  else {
    // p->next==NULL, so index was too large
    xfailure("Tried to remove an element not on the list");
    return NULL;    // silence warning
  }
}


void VoidList::removeAll()
{
  while (top != NULL) {
    VoidNode *temp = top;
    top = top->next;
    delete temp;
  }
}

	    
void VoidList::reverse()
{
  // transfer list to a temporary
  VoidNode *oldlist = top;
  top = NULL;
  
  // prepend all nodes
  while (oldlist != NULL) {
    // take first node from oldlist
    VoidNode *node = oldlist;
    oldlist = oldlist->next;

    // prepend it to new list
    node->next = top;
    top = node;
  }
}


//   The difference function should return <0 if left should come before
// right, 0 if they are equivalent, and >0 if right should come before
// left. For example, if we are sorting numbers into ascending order,
// then 'diff' would simply be subtraction.
//   The 'extra' parameter passed to sort is passed to diff each time it
// is called.
//   O(n^2) time, O(1) space
void VoidList::insertionSort(VoidDiff diff, str::Storeable const *extra)
{
  VoidNode *primary = top;                   // primary sorting pointer
  while (primary && primary->next) {
    if (diff(primary->data, primary->next->data, extra) > 0) {   // must move next node?
      VoidNode *tomove = primary->next;
      primary->next = primary->next->next;    // patch around moving node

      if (diff(tomove->data, top->data, extra) < 0) {           // new node goes at top?
        tomove->next = top;
        top = tomove;
      }

      else {                                  // new node *not* at top
        VoidNode *searcher = top;
        while (diff(tomove->data, searcher->next->data, extra) > 0) {
          searcher = searcher->next;
        }

        tomove->next = searcher->next;        // insert new node into list
        searcher->next = tomove;
      }
    }

    else {
      primary = primary->next;              // go on if no changes
    }
  }
}


// O(n log n) time, O(log n) space
void VoidList::mergeSort(VoidDiff diff, str::Storeable const *extra)
{
  if (top == NULL || top->next == NULL) {
    return;   // base case: 0 or 1 elements, already sorted
  }

  // half-lists
  VoidList leftHalf(DBG_INFO_ARG0_FIRST  npool,  str::StoragePool::Cp_TmpDuplicate);
  VoidList rightHalf(DBG_INFO_ARG0_FIRST  npool,  str::StoragePool::Cp_TmpDuplicate);

  // divide the list
  {
    // to find the halfway point, we use the slow/fast
    // technique; to get the right node for short lists
    // (like 2-4 nodes), we start fast off one ahead

    VoidNode *slow = top;
    VoidNode *fast = top->next;

    while (fast && fast->next) {
      slow = slow->next;
      fast = fast->next;
      fast = fast->next;
    }

    // at this point, 'slow' points to the node
    // we want to be the last of the 'left' half;
    // the left half will either be the same length
    // as the right half, or one node longer

    // division itself
    rightHalf.top = slow->next;	 // top half to right
    leftHalf.top = this->top;    // bottom half to left
    slow->next = NULL; 	       	 // cut the link between the halves
  }

  // recursively sort the halves
  leftHalf.mergeSort(diff, extra);
  rightHalf.mergeSort(diff, extra);

  // merge the halves into a single, sorted list
  VoidNode *merged = NULL;     	 // tail of merged list
  while (leftHalf.top != NULL &&
         rightHalf.top != NULL) {
    // see which first element to select, and remove it
    VoidNode *selected;
    if (diff(leftHalf.top->data, rightHalf.top->data, extra) < 0) {
      selected = leftHalf.top;
      leftHalf.top = leftHalf.top->next;
    }
    else {
      selected = rightHalf.top;
      rightHalf.top = rightHalf.top->next;
    }

    // append it to the merged list
    if (merged == NULL) {
      // first time; special case
      merged = this->top = selected;
    }
    else {
      // 2nd and later; normal case
      merged = merged->next = selected;
    }
  }

  // one of the halves is exhausted; concat the
  // remaining elements of the other one
  if (leftHalf.top != NULL) {
    merged->next = leftHalf.top;
    leftHalf.top = NULL;
  }
  else {
    merged->next = rightHalf.top;
    rightHalf.top = NULL;
  }

  // verify both halves are now exhausted
  xassert(leftHalf.top == NULL &&
          rightHalf.top == NULL);

  // list is sorted
}


bool VoidList::isSorted(VoidDiff diff, str::Storeable const *extra) const
{
  if (isEmpty()) {
    return true;
  }

  str::Storeable *prev = top->data;
  VoidListIter iter(*this);
  iter.adv();
  for (; !iter.isDone(); iter.adv()) {
    str::Storeable *current = iter.data();

    if (diff(prev, current, extra) <= 0) {
      // ok: prev <= current
    }
    else {
      return false;
    }

    prev = current;
  }
  
  return true;
}


// attach tail's nodes to this; empty the tail
void VoidList::concat(VoidList &tail)
{
  xassert(getParent() == tail.getParent());

  // TODO top and tail externalpointer can be mixed of stealSP child pool and this->npool

  tail.getParentRef().removeChildPool(&tail.npool);
  tail.npool.removeAllExternalPointers();

  //StoragePool stealSP =
  new (npool)  str::StoragePool(DBG_INFO_ARG0_FIRST  tail.npool, false,  str::StoragePool::Cp_Move);

  if (!top) {
    top = tail.top;
  }
  else {
    VoidNode *n = top;
    for(; n->next; n = n->next)
      {}
    n->next = tail.top;
  }

  tail.top = NULL;

}


// attach some of source's nodes to this, removing them from 'source'
void VoidList::stealTailAt(int index, VoidList &source)
{
  if (index == 0) {
    concat(source);
    return;
  }

  // TODO top and tail externalpointer can be mixed of stealSP (child pool) and this->npool

  // TODO fixme : ownerPool == NULL is ok?  see StoragePool.assignImpl
  str::StoragePool * stealSP =
  new (npool)  str::StoragePool(DBG_INFO_ARG0_FIRST  source.npool, false,  str::StoragePool::Cp_Duplicate);

  stealSP->selfCheck();

  // find the node in 'source' just before the first one that
  // will be transferred
  VoidNode *beforeTransfer = source.top;
  index--;
  while (index--) {
    beforeTransfer = beforeTransfer->next;
  }
  
  // break off the tail
  VoidNode *tailStart = beforeTransfer->next;
  beforeTransfer->next = NULL;

  // transfer 'tailStart' and beyond to 'this'
  if (!top) {
    top = tailStart;
  }
  else {
    // find the end of 'this' list
    VoidNode *n = top;
    for(; n->next; n = n->next)
      {}
    n->next = tailStart;
  }
}

uint8_t* VoidList::glueNpools(VoidList const &tail) {
    xassert(tail.npool.getExtPtrsLength() == 1);

    str::StoragePool const * chpool = npool.findChild(tail.npool.memory);

    uint8_t * oldmemend = NULL;

    if (chpool) {
        if (chpool->contains(tail.npool.memory+tail.npool.memlength-1)) {
            ;
        } else {
            chpool = NULL;
        }
    } else {
        chpool = tail.npool.findChild(npool.memory);
        if (chpool) {
            oldmemend = npool.memory+npool.memlength;

            str::StoragePool childView(DBG_INFO_ARG0);
            npool.append(tail.npool, childView);

            std::list<str::StoragePool::ChCh> badChildren;
            tail.npool.findChildChain(badChildren, npool.memory);
            if (badChildren.size()) {
                int ind = badChildren.front().index;
                childView.removeChildPool(childView.childpools + ind);
            }
        }
    }

    if (!chpool) {
        oldmemend = npool.memory+npool.memlength;
        npool += tail.npool;
    }

    return oldmemend;
}

// using str::StoragePool impl based on buffer-copy
void VoidList::appendAll(VoidList const &tail)
{
  uint8_t * oldmemend = glueNpools(tail);

  // find the end of 'this' list
  VoidNode *n = top;
  for(; n->next; n = n->next)
  {}

  if (n) {
      n->next = tail.top;

      if (oldmemend) {
         npool.moveFrom(tail.npool, (str::Storeable::DataPtr&)n->next, oldmemend);
      }
  }
}

// TODO better using str::StoragePool impl based on buffer-copy
void VoidList::appendAllNew(VoidList const &tail, VoidEq eq)
{
  VoidList dest(DBG_INFO_ARG0_FIRST  npool);
  VoidListIter srcIter(tail);
  for (; !srcIter.isDone(); srcIter.adv()) {
    str::Storeable *item = srcIter.data();
    int index = indexOf(item, eq);
    if (index == -1) {
       dest.append(DBG_INFO_ARG0_FIRST  item);
    }
  }
  concat(dest);
}


// using str::StoragePool impl based on buffer-copy
void VoidList::prependAll(VoidList const &head)
{
    uint8_t * oldmemend = glueNpools(head);

    // find the end of 'head' list
    VoidNode *n;
    for(n = head.top; n->next; n = n->next) {
        xassert(npool.findChild(n));
    }

    if (n) {
        n->next = top;

        if (oldmemend) {
            top = head.top;
            npool.moveFrom(head.npool, (str::Storeable::DataPtr&)top, oldmemend);
        }
    }
}


VoidList& VoidList::operator= (VoidList const &src)
{
  xassert(__kind == src.__kind);
  xassert(__parentVector == src.__parentVector);
  npool = src.npool;
  /*xassert(__pool == src.__pool);
  if (this != &src) {
    removeAll();
    appendAll(src);
  }*/
  return *this;
}


bool VoidList::equalAsLists(VoidList const &otherList, VoidDiff const diff, str::Storeable const *extra) const
{
  return 0==compareAsLists(otherList, diff, extra);
}

int VoidList::compareAsLists(VoidList const &otherList, VoidDiff const diff, str::Storeable const *extra) const
{
  VoidListIter mine(*this);
  VoidListIter his(otherList);

  while (!mine.isDone() && !his.isDone()) {
    int cmp = diff(mine.data(), his.data(), extra);
    if (cmp == 0) {
      // they are equal; keep going
    }
    else {
      // unequal, return which way comparison went
      return cmp;
    }

    mine.adv();
    his.adv();
  }

  if (!mine.isDone() || !his.isDone()) {
    // unequal lengths: shorter compares as less
    return mine.isDone()? -1 : +1;
  }

  return 0;        // everything matches
}


bool VoidList::equalAsSets(VoidList const &otherList, VoidDiff const diff, str::Storeable const *extra) const
{
  return this->isSubsetOf(otherList, diff, extra) &&
         otherList.isSubsetOf(*this, diff, extra);
}


bool VoidList::isSubsetOf(VoidList const &otherList, VoidDiff const diff, str::Storeable const *extra) const
{
  for (VoidListIter iter(*this); !iter.isDone(); iter.adv()) {
    if (!otherList.containsByDiff(iter.data(), diff, extra)) {
      return false;
    }
  }
  return true;
}


bool VoidList::containsByDiff(str::Storeable const *item, VoidDiff const diff, str::Storeable const *extra) const
{
  for (VoidListIter iter(*this); !iter.isDone(); iter.adv()) {
    if (0==diff(item, iter.data(), extra)) {
      return true;
    }
  }
  return false;
}


void VoidList::removeDuplicatesAsMultiset(VoidDiff diff, str::Storeable const *extra)
{
  if (isEmpty()) {
    return;
  }

  mergeSort(diff, extra);

  VoidListMutator mut(*this);

  str::Storeable *prevItem = mut.data();
  mut.adv();

  while (!mut.isDone()) {
    if (0==diff(prevItem, mut.data(), extra)) {
      // this element is identical to the previous one, so remove
      // it from the list
      mut.remove();
    }
    else {
      prevItem = mut.data();
      mut.adv();
    }
  }
}


STATICDEF int VoidList::pointerAddressDiff(str::Storeable const *left, str::Storeable const *right, str::Storeable const * )
{
  return comparePointerAddresses(left, right);
}

void VoidList::debugPrint(std::ostream& os, std::string indent) const
{
  os<<std::hex<<indent<< "{ ";
  for (VoidListIter iter(*this); !iter.isDone(); iter.adv()) {
    if (iter.data()) {
        os<<" "<< *iter.data();
    } else {
        os<<" NULL";
    }
  }
  os<< "}"<<std::flush<<std::dec;
}


// --------------- VoidListMutator ------------------
VoidListMutator&
  VoidListMutator::operator=(VoidListMutator const &obj)
{			      
  // we require that the underlying lists be the same
  // because we can't reset the 'list' reference if they
  // aren't
  xassert(&list == &obj.list);

  prev = obj.prev;
  current = obj.current;

  return *this;
}


void VoidListMutator::insertBefore(DBG_INFO_FORMAL_FIRST  str::Storeable *item)
{
  if (prev == NULL) {
    // insert at start of list
    list.prepend(DBG_INFO_ARG_FWD_FIRST  item);
    reset();
  }
  else {
    current = prev->next = new (list.npool) VoidNode(DBG_INFO_ARG_FWD_FIRST  list.npool, item, current);
  }
}


void VoidListMutator::insertAfter(DBG_INFO_FORMAL_FIRST  str::Storeable *item)
{
  xassert(!isDone());
  current->next = new (list.npool) VoidNode(DBG_INFO_ARG_FWD_FIRST  list.npool, item, current->next);
}


void VoidListMutator::append(DBG_INFO_FORMAL_FIRST  str::Storeable *item)
{
  xassert(isDone());
  insertBefore(DBG_INFO_ARG_FWD_FIRST  item);
  adv();
}


str::Storeable *VoidListMutator::remove()
{
  xassert(!isDone());
  str::Storeable *retval = data();
  if (prev == NULL) {
    // removing first node
    list.top = current->next;
    delete current;
    current = list.top;
  }
  else {
    current = current->next;
    delete prev->next;   // old 'current'
    prev->next = current;
  }
  return retval;
}


// --------------- VoidListIter --------------------
VoidListIter::VoidListIter(VoidList const &list, int pos)
{
  reset(list);
  while (pos--) {
    adv();
  }
}


// -------------- testing --------------
#ifdef TEST_VOIDLIST
#include "test.h"     // USUAL_MAIN

// assumes we're using pointerAddressDiff as the comparison fn
// (I don't use isSorted because this fn will throw at the disequality,
// whereas isSorted would forget that info)
template<typename T>
void verifySorted(VoidList const &list)
{
  T* prev = 0;
  VoidListIter iter(list);
  for (; !iter.isDone(); iter.adv()) {
    T* current = (T*)iter.data();
    xassert(prev <= current);    // numeric address test
    prev = current;
  }
}


#define PRINT(lst) printf("%s: ", #lst); lst.debugPrint(); printf("\n") /* user ; */


void testSorting()
{
  enum { ITERS=100, ITEMS=20 };

  loopi(ITERS) {
    // construct a list (and do it again if it ends up already sorted)
    VoidList list1(DBG_INFO_ARG0);
    VoidList list3(DBG_INFO_ARG0);     // this one will be constructed sorted one at a time
    int numItems;
    do {
      list1.removeAll();    // clear them in case we have to build it more than once
      list3.removeAll();
      numItems = rand()%ITEMS;
      loopj(numItems) {
        Integer *toInsert = new Integer(DBG_INFO_ARG0_FIRST  (rand()%ITEMS) * 4 );
        list1.prepend(DBG_INFO_ARG0_FIRST  toInsert);
        list3.insertSorted(DBG_INFO_ARG0_FIRST  toInsert, VoidList::pointerAddressDiff);
      }
    } while (list1.isSorted(VoidList::pointerAddressDiff));

    // list3 should be sorted already
    //PRINT(list3);
    verifySorted<Integer>(list3);

    // duplicate it for use with other algorithm
    VoidList list2(DBG_INFO_ARG0);
    list2 = list1;

    // sort them
    list1.insertionSort(VoidList::pointerAddressDiff);
    xassert(list1.equalAsPointerSets(list2));      // verify intermediate equality
    xassert(!list1.equalAsPointerLists(list2));    // but not elementwise
    list2.mergeSort(VoidList::pointerAddressDiff);
    //PRINT(list1);

    // verify structure
    list1.selfCheck();
    list2.selfCheck();

    // verify length
    xassert(list1.count() == numItems && list2.count() == numItems);

    // verify sortedness
    verifySorted<Integer>(list1);
    verifySorted<Integer>(list2);

    // verify equality
    xassert(list1.equalAsPointerLists(list2));
    xassert(list1.equalAsPointerLists(list3));

    // to test as-sets equality
    str::Storeable *first = list1.first();
    while (list1.removeIfPresent(first))
      {}     // remove all occurrances of 'first'
    xassert(!list1.equalAsPointerSets(list2));
  }
}

namespace str {
void entry()
{
  str::StoragePool pool(DBG_INFO_ARG0);
  VoidList *_list = NULL;
  VoidList *_thief = NULL;
  // first set of tests
  try {

    _list = new (pool) VoidList(DBG_INFO_ARG0_FIRST  pool);
    VoidList &list = *_list;

    // some sample items
    str::Storeable
            *a=new (list.getPool()) Integer(DBG_INFO_ARG0_FIRST  list.getPool(),4),
            *b=new (list.getPool()) Integer(DBG_INFO_ARG0_FIRST  list.getPool(),8),
            *c=new (list.getPool()) Integer(DBG_INFO_ARG0_FIRST  list.getPool(),12),
            *d=new (list.getPool()) Integer(DBG_INFO_ARG0_FIRST  list.getPool(),16);

    // test simple modifiers and info
    list.append(DBG_INFO_ARG0_FIRST  c);     PRINT(list);   // c
    list.prepend(DBG_INFO_ARG0_FIRST  b);   	PRINT(list);   // b c
    list.append(DBG_INFO_ARG0_FIRST  d);	PRINT(list);   // b c d
    list.prepend(DBG_INFO_ARG0_FIRST  a);	PRINT(list);   // a b c d
    list.removeAt(2);	PRINT(list);   // a b d

    xassert( list.count() == 3 &&
             !list.isEmpty() &&
             list.nth(0) == a &&
             list.nth(1) == b &&
             list.nth(2) == d &&
             list.indexOf(a) == 0 &&
             list.indexOf(b) == 1 &&
             list.indexOf(c) == -1 &&
             list.indexOf(d) == 2
           );
    list.selfCheck();

    // test mutator s
    {
      VoidListMutator mut(list);
      list.selfCheck();

      mut.adv();
	// now it's pointing at b
      mut.insertAfter(DBG_INFO_ARG0_FIRST  c);
    // now list is (a b c d) and mut points at b still
      verifySorted<Integer>(list);
      list.selfCheck();
      mut.remove();
	// now list is (a c d) and mut points at c
      xassert(mut.data() == c);
      list.selfCheck();

      // copy the mutator
      VoidListMutator mut2(mut);
      mut2.adv();
      xassert(mut.data() == c  &&  mut2.data() == d);

      // copy to a normal iterator
      VoidListIter iter(mut);
      iter.adv();
      xassert(iter.data() == d);
      iter.adv();
      xassert(iter.isDone()  &&  mut.data() == c);

      list.selfCheck();
      PRINT(list);
    }

    // test appendUnique and prependUnique
    // list starts as (a c d)
    xassert(list.appendUnique(DBG_INFO_ARG0_FIRST  c) == false &&
            list.prependUnique(DBG_INFO_ARG0_FIRST  d) == false &&
            list.prependUnique(DBG_INFO_ARG0_FIRST  b) == true);
      // now it is (b a c d)
    list.removeItem(a);
    xassert(list.removeIfPresent(a) == false);
      // now it is (b c d)
    verifySorted<Integer>(list);
    list.selfCheck();
    PRINT(list);

    // test reverse
    list.reverse();
      // list is now (d c b)
    xassert(list.indexOf(d) == 0 &&
            list.indexOf(c) == 1 &&
            list.indexOf(b) == 2);
    list.selfCheck();
    PRINT(list);

    // test stealTailAt
    _thief = new (pool) VoidList(DBG_INFO_ARG0_FIRST  pool);
    VoidList &thief = *_thief;
    thief.selfCheck();

    thief.stealTailAt(1, list);

    thief.selfCheck();
    list.selfCheck();
      // list is now (d)
      // thief is now (c b)
    xassert(list.count() == 1 &&
            list.indexOf(d) == 0 &&
            thief.count() == 2 &&
            thief.indexOf(c) == 0 &&
            thief.indexOf(b) == 1);

    // test appendAll
    list.appendAll(thief);      // list: (d c b)
    thief.selfCheck();
    list.selfCheck();
    PRINT(list);
    xassert(list.count() == 3 &&
            list.indexOf(d) == 0 &&
            list.indexOf(c) == 1 &&
            list.indexOf(b) == 2);

    // test prependAll
    list.prependAll(thief);     // list: (c b d c b)
    thief.selfCheck();
    list.selfCheck();
    PRINT(list);
    xassert(list.count() == 5 &&
            list.nth(0) == c &&
            list.nth(1) == b &&
            list.nth(2) == d &&
            list.nth(3) == c &&
            list.nth(4) == b);

    xassert(thief.count() == 2);    // not modified

    // test removeDuplicatesAsMultiset
    list.removeDuplicatesAsPointerMultiset();     // list: (b c d)
    list.selfCheck();
    PRINT(list);
    xassert(list.count() == 3 &&
            list.nth(0) == b &&
            list.nth(1) == c &&
            list.nth(2) == d);
  } catch (x_assert const & ex) {
    std::cout<<"Pool:\n";
    pool.debugPrint();
    if (_list) {
        std::cout<<"list:\n";
        _list->debugPrint();
    }
    if (_thief) {
        std::cout<<"thief:\n";
        _thief->debugPrint();
    }
  }

  // this hits most of the remaining code
  // (a decent code coverage tool for C++ would be nice!)
  testSorting();

  printf("voidlist ok\n");
}

}

inline void entry() {
    str::entry();
}


USUAL_MAIN

#endif // NDEBUG

