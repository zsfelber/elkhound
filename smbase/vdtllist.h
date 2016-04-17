// vdtllist.h            see license.txt for copyright and terms of use
// list of void*, with a pointer maintained to the last (tail)
// element, for constant-time append

#ifndef VDTLLIST_H
#define VDTLLIST_H

#include "voidlist.h"      // VoidList

// inherit privately so I can choose what to expose
class VoidTailList : private VoidList {
private:
  // by making this a friend, it should see VoidList as a
  // base class, and thus simply work
  // but it doesn't..
  //friend VoidListIter;

  friend class VoidTailListIter;

  // no mutator for now

protected:
  VoidNode *tail;       // (serf) last element of list, or NULL if list is empty
  VoidNode *getTop() const { return VoidList::getTop(); }

private:
  VoidTailList(VoidTailList const &obj);    // not allowed

  void adjustTail();
  void adjustTails();

public:
  VoidTailList(DBG_INFO_FORMAL_FIRST  __StoreAlreadyConstr nothing) : VoidList(DBG_INFO_ARG_FWD_FIRST  nothing)
  {  }

  VoidTailList(DBG_INFO_FORMAL) : VoidList(DBG_INFO_ARG_FWD_FIRST  sizeof(VoidTailList))
  { tail = NULL;  if (getKind()==ST_PARENT) npool.addPointer(tail); }

  VoidTailList(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool) : VoidList(DBG_INFO_ARG_FWD_FIRST  pool)
  { tail = NULL;  if (getKind()==ST_PARENT) npool.addPointer(tail); }
  VoidTailList(DBG_INFO_FORMAL_FIRST  str::Storeable &parent) : VoidList(DBG_INFO_ARG_FWD_FIRST  parent,sizeof(VoidTailList))
  { tail = NULL;  if (getKind()==ST_PARENT) npool.addPointer(tail); }
  ~VoidTailList()                    { if (getKind()==ST_PARENT) npool.removePointer(tail); }
  
  // move:true  special ctor which steals the list. NOTE invoker should always deallocate the header
  VoidTailList(DBG_INFO_FORMAL_FIRST  VoidTailList const &src, bool move) : VoidList(DBG_INFO_ARG_FWD_FIRST  src, sizeof(VoidTailList), move)
  { }

  void assign(VoidTailList const &src, bool move) {
    Storeable::assign(src, sizeof(VoidTailList));
    chk_assign(src, move);
  }
  void chk_assign(VoidTailList const &src, bool move) {
    VoidList::chk_assign(src, move);
    tail = src.tail;
    ExternalPtr ptrs[] = { (ExternalPtr)&tail };
    npool.convertExternalPointers(src.npool, ptrs, ptrs+1);
  }


  VoidList::assignParent;

  VoidList::getPool;


  //void steal(VoidTailList *src,bool deleteOrig=true);

  // this syntax just makes the implementation inherited from
  // 'VoidList' public, whereas it would default to private,
  // since it was inherited privately
  VoidList::count;

  // see voidlist.h for documentation of each of these functions
  VoidList::isEmpty;
  VoidList::isNotEmpty;
  VoidList::nth;
  VoidList::first;
  str::Storeable *last() const                 { xassert(tail); return tail->data; }

  // insertion
  void prepend(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem);
  void append(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem);
  void appendAll(VoidTailList const &tail);
  void prependAll(VoidTailList const &head);
  void appendAllNew(VoidTailList const &tail, VoidEq eq);
  void reappendAll(VoidTailList const &tail, VoidEq eq);
  void insertAt(DBG_INFO_FORMAL_FIRST  str::Storeable *newitem, int index);
  void concat(VoidTailList &tail);

  // removal
  str::Storeable *removeFirst();               // remove first, return data; must exist
  str::Storeable *removeLast();
  str::Storeable *removeAt(int index);
  void removeAll();
  VoidList::removeItem;

  // list-as-set: selectors
  VoidList::indexOf;
  VoidList::indexOfF;
  VoidList::contains;

  // list-as-set: mutators
  bool prependUnique(str::Storeable *newitem);
  bool appendUnique(str::Storeable *newitem);
  //void removeItem(str::Storeable *item);
  //bool removeIfPresent(str::Storeable *item);

  // debugging
  void selfCheck() const;
  VoidList::debugPrint;
};


// copied from voidlist.h because g++ won't do what I want..
class VoidTailListIter {
protected:
  VoidNode *p;                        // (serf) current item

public:
  VoidTailListIter(VoidTailList const &list)  { reset(list); }
  ~VoidTailListIter()                         {}

  void reset(VoidTailList const &list)        { p = list.getTop(); }

  // iterator copying; generally safe
  VoidTailListIter(VoidTailListIter const &obj)             { p = obj.p; }
  VoidTailListIter& operator=(VoidTailListIter const &obj)  { p = obj.p; return *this; }

  // but copying from a mutator is less safe; see above
  //VoidTailListIter(VoidListMutator &obj)      { p = obj.current; }

  // iterator actions
  bool isDone() const                         { return p == NULL; }
  void adv()                                  { p = p->next; }
  str::Storeable *data() const                          { return p->data; }
  str::Storeable *&dataRef()                            { return p->data; }

  // iterator mutation; use with caution
  void setDataLink(str::Storeable *newData)             { p->data = newData; }
};



#endif // VDTLLIST_H
