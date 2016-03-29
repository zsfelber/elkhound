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
  VoidTailList(StoragePool &pool) : VoidList(pool)
  { tail = NULL;  npool.addPointer(tail); }
  VoidTailList(Storeable &parent) : VoidList(parent,sizeof(VoidTailList))
  { tail = NULL;  npool.addPointer(tail); }
  ~VoidTailList()                    { npool.removePointer(tail); }
  
  // move:true  special ctor which steals the list. NOTE invoker should always deallocate the header
  VoidTailList(VoidTailList &src, bool move) : VoidList(src, sizeof(VoidTailList), move)
  { }

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
  Storeable *last() const                 { xassert(tail); return tail->data; }

  // insertion
  void prepend(Storeable *newitem);
  void append(Storeable *newitem);
  void appendAll(VoidTailList const &tail);
  void appendAllNew(VoidTailList const &tail, VoidEq eq);
  void reappendAll(VoidTailList const &tail, VoidEq eq);
  void insertAt(Storeable *newitem, int index);
  void concat(VoidTailList &tail);

  // removal
  Storeable *removeFirst();               // remove first, return data; must exist
  Storeable *removeLast();
  Storeable *removeAt(int index);
  void removeAll();
  VoidList::removeItem;

  // list-as-set: selectors
  VoidList::indexOf;
  VoidList::indexOfF;
  VoidList::contains;

  // list-as-set: mutators
  bool prependUnique(Storeable *newitem);
  bool appendUnique(Storeable *newitem);
  //void removeItem(Storeable *item);
  //bool removeIfPresent(Storeable *item);

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
  Storeable *data() const                          { return p->data; }
  Storeable *&dataRef()                            { return p->data; }

  // iterator mutation; use with caution
  void setDataLink(Storeable *newData)             { p->data = newData; }
};



#endif // VDTLLIST_H
