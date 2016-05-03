// voidlist.h            see license.txt for copyright and terms of use
// list of void*

// Author: Scott McPeak, 2000

#ifndef __VOIDLIST_H
#define __VOIDLIST_H

#include "xassert.h"     // xassert
#include "typ.h"         // bool
#include "trdelete.h"    // TRASHINGDELETE
#include <iostream>      //
#include "storage.h"

// -------------------------- non-typesafe core -----------------------------
// non-typesafe list node
class VoidNode : public str::Storeable {
public:
  //TRASHINGDELETE

  VoidNode *next;           // (owner) next item in list, or NULL if last item
  str::Storeable *data;               // whatever it is the list is holding

  VoidNode(DBG_INFO_FORMAL_FIRST  str::StoragePool const &_pool, str::Storeable const *aData=NULL, VoidNode const *aNext=NULL) : str::Storeable(DBG_INFO_ARG_FWD_FIRST  _pool) {
      data=constcast(aData);
      next=constcast(aNext);
      str::StoragePool & pool = constcast(_pool);
      pool.addPointer(next);
      pool.addPointer(data);
  }

  virtual ~VoidNode() {
      str::StoragePool & pool = constcast(getParentRef());
      pool.removePointer(next);
      pool.removePointer(data);
  }
  void debugPrint(std::ostream& os, int indent = 0, char const * subtreeName = 0) const {
      if (indent > DEBUG_MAX_IND || isDeleted()) { str::ind(os,indent)<< "vnode"<<(void*)this<<"..."; return; }
      str::ind(os,indent)<< "vnode:";
      if (data) {
          os<<" ";
          data->debugPrint(os, indent+1);
      } else {
          os<<" NULL";
      }
      os<<std::flush;
  }
};


// forward decls for 'friend' decls
class VoidListIter;
class VoidListMutator;


// The difference function should return <0 if left should come before
// right, 0 if they are equivalent, and >0 if right should come before
// left.  For example, if we are sorting numbers into ascending order,
// then 'diff' could simply be subtraction.
typedef int (*VoidDiff)(str::Storeable const *left, str::Storeable const *right, str::Storeable const *extra);

typedef bool (*VoidEq)(str::Storeable const *left, str::Storeable const *right);


// list of void*; at this level, the void* are completely opaque;
// the list won't attempt to delete(), compare them, or anything else
// (well, some comparison has creeped in now... but only via VoidDiff)
class VoidList : public str::Storeable {
private:
  friend class VoidTailList;
  friend class VoidListIter;
  friend class VoidListMutator;

  uint8_t* glueNpools(VoidList const &tail);

#ifdef DEBUG
  VoidList(VoidList const &obj);     // undefined
#endif
public:
  inline str::StoragePool const &getPool() const {
      return npool;
  }

protected:
  str::StoragePool npool;

  VoidNode *top;                     // (owner) first node, or NULL if list is empty
  VoidNode *getTop() const { return top; } // for iterator, below

public:
  VoidList(DBG_INFO_FORMAL_FIRST  __StoreAlreadyConstr nothing) : Storeable(DBG_INFO_ARG_FWD_FIRST  nothing), npool(DBG_INFO_ARG_FWD_FIRST  nothing)
  {  }

  VoidList(DBG_INFO_FORMAL_FIRST  size_t size_of=0)  : str::Storeable(DBG_INFO_ARG_FWD_FIRST  size_of?size_of:sizeof(VoidList)), npool(DBG_INFO_ARG_FWD)
  { top=NULL;   if (getKind()==ST_VALUE) npool.addPointer(top); }

  VoidList(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, str::StoragePool::CopyMode copy)  : str::Storeable(DBG_INFO_ARG_FWD), npool(DBG_INFO_ARG_FWD_FIRST  pool, false, copy)
  { top=NULL;   if (getKind()==ST_VALUE) npool.addPointer(top); }

  VoidList(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool)  : str::Storeable(DBG_INFO_ARG_FWD_FIRST pool), npool(DBG_INFO_ARG_FWD_FIRST  *this, true)
  { top=NULL;   if (getKind()==ST_VALUE) npool.addPointer(top); }

  VoidList(DBG_INFO_FORMAL_FIRST  str::Storeable const &parent, size_t size_of=0) : str::Storeable(DBG_INFO_ARG_FWD_FIRST  parent, size_of?size_of:sizeof(VoidList), true), npool(DBG_INFO_ARG_FWD_FIRST  *this, true)
  { top=NULL;   if (getKind()==ST_VALUE) npool.addPointer(top); }

  VoidList(DBG_INFO_FORMAL_FIRST  VoidList const &obj, size_t size_of=0, bool move=false);     // makes a (shallow) copy of the contents
  virtual ~VoidList()                { /*npool clears it completely*/  }

  void assignParent(str::StoragePool const *pool0);
  void assign(VoidList const &src, size_t size_of=0, bool move=false);
  void chk_assign(VoidList const &obj, bool move);

  // selectors
  int count() const;                 // # of items in list
  bool isEmpty() const               { return top == NULL; }
  bool isNotEmpty() const            { return top != NULL; }
  str::Storeable *nth(int which) const;        // get particular item, 0 is first (item must exist)
  str::Storeable *first() const { return nth(0); }
  str::Storeable *last() const { return nth(count()-1); }

  // insertion
  void prepend(DBG_INFO_FORMAL_FIRST  str::Storeable const *newitem);       // insert at front
  VoidNode* append(DBG_INFO_FORMAL_FIRST  str::Storeable const *newitem);        // insert at rear
  void insertAt(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem, int index);
    // new item is inserted such that its index becomdes 'index'
  void insertSorted(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem, VoidDiff const diff, str::Storeable const *extra=NULL);
    // insert into an already-sorted list so that the list is sorted afterwards

  // removal
  str::Storeable *removeAt(int index);         // remove from list (must exist), and return removed item
  str::Storeable *removeFirst()                { return removeAt(0); }
  str::Storeable *removeLast()                 { return removeAt(count()-1); }
  void removeAll();

  // list-as-set: selectors
  int indexOf(str::Storeable const *item) const;     // returns index of *first* occurrance, or -1 if not present
  int indexOf(str::Storeable const *item, VoidEq const eq) const;     // returns index of *first* occurrance, or -1 if not present
  int indexOfF(str::Storeable const *item) const;    // same as indexOf, but throws exception if not present
  bool contains(str::Storeable const *item) const    // true if the item appears in the list
    { return indexOf(item) >= 0; }

  // list-as-set: mutators
  bool prependUnique(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem); // prepend only if not already there
  bool appendUnique(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem);  // append   "            "
  void removeItem(str::Storeable const *item);       // remove first occurrance -- must exist
  bool removeIfPresent(str::Storeable const *item);  // remove first occurrance; return true if changed
  bool removeIfPresent(str::Storeable const *item, VoidEq eq);  // remove first occurrance; return true if changed
  bool removeItems(VoidList const &lst, VoidEq eq);   // remove all; return true if changed

  // complex modifiers
  void reverse();
  void insertionSort(VoidDiff diff, str::Storeable const *extra=NULL);
  void mergeSort(VoidDiff diff, str::Storeable const *extra=NULL);

  // and a related test
  bool isSorted(VoidDiff diff, str::Storeable const *extra=NULL) const;

  // multiple lists
  void concat(VoidList &tail);           // tail is emptied, nodes appended to this
  VoidNode* appendAll(VoidList const &tail, VoidNode *myTail = NULL, VoidNode *tailTail = NULL);  // tail is untouched.. but its contents are now exposed to non-constness... ug... oh well
  void appendAllNew(VoidList const &tail, VoidEq eq);
  VoidNode* prependAll(VoidList const &head, VoidNode *headTail = NULL);
  VoidList& operator= (VoidList const &src);  // afterwards, 'this' and 'src' have same contents

  // steal (become the container for) the tail of a source list at any
  // point; if 'index' is 0, the entire 'source' is stolen (i.e.
  // index=0 is equivalent to 'concat', above); stolen items appended
  // to 'this'
  void stealTailAt(int index, VoidList &source);

  // equal items in equal positions
  bool equalAsLists(VoidList const &otherList, VoidDiff const diff, str::Storeable const *extra=NULL) const;
  
  // if equal, returns 0; otherwise, return order (-1/+1) according to
  // the first differing pair of elements; a shorter (but otherwise
  // idential list) will compare as being less
  int compareAsLists(VoidList const &otherList, VoidDiff const diff, str::Storeable const * extra=NULL) const;

  // last-as-set: comparisons (NOT efficient)
  bool equalAsSets(VoidList const &otherList, VoidDiff const diff, str::Storeable const * extra=NULL) const;
    // A subset of B, and vice-versa
  bool isSubsetOf(VoidList const &otherList, VoidDiff const diff, str::Storeable const * extra=NULL) const;
    // uses slow elementwise containment
  bool containsByDiff(str::Storeable const * item, VoidDiff const diff, str::Storeable const * extra=NULL) const;

  // use 'diff' to mergesort the list, then remove duplicate entries
  void removeDuplicatesAsMultiset(VoidDiff diff, str::Storeable const *extra=NULL);

  // treating the pointer values themselves as the basis for comparison
  static int pointerAddressDiff(str::Storeable const *left, str::Storeable const *right, str::Storeable const *);
  bool equalAsPointerLists(VoidList const &otherList) const
    { return equalAsLists(otherList, pointerAddressDiff); }
  bool equalAsPointerSets(VoidList const &otherList) const
    { return equalAsSets(otherList, pointerAddressDiff); }
  void removeDuplicatesAsPointerMultiset()
    { removeDuplicatesAsMultiset(pointerAddressDiff); }

  // debugging
  void selfCheck() const;            // test this list; fail assertion if malformed

  Storeable::debugPrint;

  void debugPrint(std::ostream& os, int indent = 0, char const * subtreeName = 0) const;     // print list contents to stdout
  void checkHeapDataPtrs() const;    // fail assertion if any 'data' ptr isn't valid heap ptr
  void checkUniqueDataPtrs() const;  // fail assertion if any 'data' ptr matches any other in this list
};


// for traversing the list and modifying it
// NOTE: no list-modification fns should be called on 'list' while this
//       iterator exists, and only one such iterator should exist for
//       any given list
class VoidListMutator {
  friend class VoidListIter;

protected:
  VoidList &list; 	  // underlying list
  VoidNode *prev;         // (serf) previous node; NULL if at list's head
  VoidNode *current;      // (serf) node we're considered to be pointing at
  bool stuck = false;

public:
  VoidListMutator(VoidList &lst)   : list(lst) { reset(); list.npool.addPointer(prev); list.npool.addPointer(current); }
  ~VoidListMutator()               {list.npool.removePointer(prev); list.npool.removePointer(current); }

  void reset()                     { prev = NULL;  current = list.top; }

  // iterator copying; safe *only* until one of the mutators modifies
  // the list structure (by inserting or removing), at which time all
  // other iterators might be in limbo
  VoidListMutator(VoidListMutator const &obj)
    : list(obj.list), prev(obj.prev), current(obj.current) { list.npool.addPointer(prev); list.npool.addPointer(current); }
  VoidListMutator& operator=(VoidListMutator const &obj);
    // requires that 'this' and 'obj' already refer to the same 'list'

  // iterator actions
  bool isDone() const              { return current == NULL; }
  void adv()                       { if (stuck) stuck = false; else {prev = current;  current = current->next;} }
  str::Storeable *data()                     { return current->data; }
  str::Storeable *&dataRef()                 { return current->data; }

  // insertion
  void insertBefore(DBG_INFO_FORMAL_FIRST  str::Storeable const *item);
    // 'item' becomes the new 'current', and the current 'current' is
    // pushed forward (so the next adv() will make it current again)

  void insertAfter(DBG_INFO_FORMAL_FIRST  str::Storeable const *item);
    // 'item' becomes what we reach with the next adv();
    // isDone() must be false

  void append(DBG_INFO_FORMAL_FIRST  str::Storeable const *item);
    // only valid while isDone() is true, it inserts 'item' at the end of
    // the list, and advances such that isDone() remains true; equivalent
    // to { xassert(isDone()); insertBefore(item); adv(); }

  // removal
  str::Storeable *remove();
    // 'current' is removed from the list and returned, and whatever was
    // next becomes the new 'current'
  str::Storeable *removeAndStuck()
  {
    stuck = true;
    return remove();
  }

  // debugging
  void selfCheck() const
    { xassert((prev->next == current  &&  current != list.top) ||
              (prev==NULL && current==list.top)); }
};


// for traversing the list without modifying it
// NOTE: no list-modification fns should be called on 'list' while this
//       iterator exists
class VoidListIter {
protected:
  VoidNode *p;                        // (serf) current item

public:
  VoidListIter(VoidList const &list)  { reset(list); }
  VoidListIter(VoidList const &list, int pos);    // advance 'pos' times
  ~VoidListIter()                     {}

  void reset(VoidList const &list)    { p = list.getTop(); }

  // iterator copying; generally safe
  VoidListIter(VoidListIter const &obj)             { p = obj.p; }
  VoidListIter& operator=(VoidListIter const &obj)  { p = obj.p;  return *this; }

  // but copying from a mutator is less safe; see above
  VoidListIter(VoidListMutator &obj)  { p = obj.current; }

  // iterator actions
  bool isDone() const                 { return p == NULL; }
  void adv()                          { p = p->next; }
  str::Storeable *data() const                  { return p->data; }
};


#endif // __VOIDLIST_H
