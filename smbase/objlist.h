// objlist.h
// owner list of arbitrary dynamically-allocated objects
// NOTE: automatically generated from xobjlist.h -- do not edit directly

// Author: Scott McPeak, 2000

#ifndef OBJLIST_H
#define OBJLIST_H

#include <boost/type_index.hpp>
#include "voidlist.h"    // VoidList
#include "storage.h"    // VoidList


// forward declarations of template classes, so we can befriend them in ObjList
// (not required by Borland C++ 4.5, but GNU wants it...)
template <class T> class ObjListIter;
template <class T> class ObjListMutator;
template <class T> class ObjListIterNC;


// the list is considered to own all of the items; it is an error to insert
// an item into more than one such list, or to insert an item more than once
// into any such list
template<typename T> class SObjList;
template <class T>
class ObjList : public str::Storeable {
private:
  friend class ObjListIter<T>;
  friend class ObjListMutator<T>;
  friend class ObjListIterNC<T>;
  friend class Grammar;
  friend class SObjList<T>;

#ifdef DEBUG
  ObjList(ObjList const &obj);//undefined
#endif

protected:
  VoidList list;                        // list itself


  #define OWN xassert(owning);
  #define NOWN xassert(!owning);
private:
  bool const owning;
  // make shallow copies and non-owning list
  ObjList(DBG_INFO_FORMAL_FIRST ObjList const &obj) : str::Storeable(DBG_INFO_ARG_FWD), list(obj.list), owning(false) { }
  ObjList& operator= (ObjList const &src) { NOWN list = src.list; return *this;  }

  inline void del_itm(T* itm) { if (owning) delete itm; }

  public:
  ObjList(DBG_INFO_FORMAL)                            : str::Storeable(DBG_INFO_ARG_FWD), list(DBG_INFO_ARG_FWD), owning(true) {}
  ObjList(DBG_INFO_FORMAL_FIRST str::StoragePool &pool)                            : str::Storeable(DBG_INFO_ARG_FWD_FIRST  pool), list(DBG_INFO_ARG_FWD_FIRST  *this,0), owning(true) {}
  ObjList(DBG_INFO_FORMAL_FIRST str::Storeable const &parent)                      : str::Storeable(DBG_INFO_ARG_FWD_FIRST  parent, sizeof(ObjList)), list(DBG_INFO_ARG_FWD_FIRST  *this,0), owning(true) {}

  ~ObjList()                           { deleteAll(); }

  // The difference function should return <0 if left should come before
  // right, 0 if they are equivalent, and >0 if right should come before
  // left.  For example, if we are sorting numbers into ascending order,
  // then 'diff' would simply be subtraction.
  typedef int (*Diff)(T const *left, T const *right, str::Storeable const *extra);

  // selectors
  int count() const                     { return list.count(); }
  bool isEmpty() const                  { return list.isEmpty(); }
  bool isNotEmpty() const               { return list.isNotEmpty(); }
  T *nth(int which)                     { return (T*)list.nth(which); }
  T const *nthC(int which) const        { return (T const*)list.nth(which); }
  T *first()                            { return (T*)list.first(); }
  T const *firstC() const               { return (T const*)list.first(); }
  T *last()                             { return (T*)list.last(); }
  T const *lastC() const                { return (T const*)list.last(); }

  // insertion
  void prepend(DBG_INFO_FORMAL_FIRST  T *newitem)              { OWN list.prepend(DBG_INFO_ARG_FWD_FIRST  newitem); }
  VoidNode* append(DBG_INFO_FORMAL_FIRST  T *newitem)          { OWN return list.append(DBG_INFO_ARG_FWD_FIRST  newitem); }
  void insertAt(DBG_INFO_FORMAL_FIRST  T *newitem, int index)  { OWN list.insertAt(DBG_INFO_ARG_FWD_FIRST  newitem, index); }
  void insertSorted(DBG_INFO_FORMAL_FIRST  T *newitem, Diff diff, str::Storeable const *extra=NULL)
    { OWN list.insertSorted(DBG_INFO_ARG_FWD_FIRST  newitem, (VoidDiff)diff, extra); }

  // removal
  T *removeAt(int index)                { return (T*)list.removeAt(index); }
  T *removeFirst()                      { return (T*)list.removeFirst(); }
  T *removeLast()                       { return (T*)list.removeLast(); }
  void deleteAt(int index)              { del_itm((T*)list.removeAt(index)); }
  void deleteAll();

  // list-as-set: selectors
  int indexOf(T const *item) const      { return list.indexOf(item); }
  int indexOfF(str::Storeable const *item) const        { return list.indexOfF(item); }
  bool contains(T const *item) const    { return list.contains(item); }

  // list-as-set: mutators
  bool prependUnique(DBG_INFO_FORMAL_FIRST  T *newitem)        { OWN return list.prependUnique(DBG_INFO_ARG_FWD_FIRST  newitem); }
  bool appendUnique(DBG_INFO_FORMAL_FIRST  T *newitem)         { OWN return list.appendUnique(DBG_INFO_ARG_FWD_FIRST  newitem); }
  void removeItem(T const *item)        { list.removeItem(item); }    // whether the arg should be const is debatable..
  bool removeIfPresent(T const *item)   { return list.removeIfPresent(item); }

  // complex modifiers
  void reverse()                                    { list.reverse(); }
  void insertionSort(Diff diff, str::Storeable const *extra=NULL)   { list.insertionSort((VoidDiff)diff, extra); }
  void mergeSort(Diff diff, str::Storeable const *extra=NULL)       { list.mergeSort((VoidDiff)diff, extra); }

  // and a related test
  bool isSorted(Diff diff, str::Storeable const *extra=NULL) const  { return list.isSorted((VoidDiff)diff, extra); }

  // multiple lists
  template <class XObjList>
  void concat(XObjList &tail)                       { OWN list.concat(tail.list); }

  //    // (we do *not* have appendAll, since these are supposed to be owner lists)
  //
  template <class XObjList>
  void appendAll(XObjList const &tail)              { NOWN list.appendAll(tail.list); }

  template <class XObjList>
  void prependAll(XObjList const &head)             { NOWN list.prependAll(head.list); }

  // steal
  void stealTailAt(int index, ObjList &tail)       { list.stealTailAt(index, tail.list); }

  // equal items in equal positions
  bool equalAsLists(ObjList const &otherList, Diff diff, str::Storeable const *extra=NULL) const
    { return list.equalAsLists(otherList.list, (VoidDiff)diff, extra); }
  int compareAsLists(ObjList const &otherList, Diff diff, str::Storeable const *extra=NULL) const
    { return list.compareAsLists(otherList.list, (VoidDiff)diff, extra); }

  // last-as-set: comparisons (NOT efficient)
  bool equalAsSets(ObjList const &otherList, Diff diff, str::Storeable const *extra=NULL) const
    { return list.equalAsSets(otherList.list, (VoidDiff)diff, extra); }
  bool isSubsetOf(ObjList const &otherList, Diff diff, str::Storeable const *extra=NULL) const
    { return list.isSubsetOf(otherList.list, (VoidDiff)diff, extra); }
  bool containsByDiff(T const *item, Diff diff, str::Storeable const *extra=NULL) const
    { return list.containsByDiff(item, (VoidDiff)diff, extra); }

  // treating the pointer values themselves as the basis for comparison
  bool equalAsPointerLists(ObjList const &otherList) const
    { return list.equalAsPointerLists(otherList.list); }
  bool equalAsPointerSets(ObjList const &otherList) const
    { return list.equalAsPointerSets(otherList.list); }

  // debugging: two additional invariants
  void selfCheck() const { 
    list.selfCheck();
    list.checkHeapDataPtrs();
    list.checkUniqueDataPtrs();
  }

  Storeable::debugPrint;
    void debugPrint(std::ostream& os, int indent = 0, char const *subtreeName = 0) const        { str::ind(os,indent)<<"obj<"<< boost::typeindex::type_id<T>().pretty_name() <<">:"; list.debugPrint(os); }
  };


template <class T>
void ObjList<T>::deleteAll()
{
  while (!list.isEmpty()) {
    deleteAt(0);
  }
}


// for traversing the list and modifying it (nodes and/or structure)
// NOTE: no list-modification fns should be called on 'list' while this
//       iterator exists, and only one such iterator should exist for
//       any given list
template <class T>
class ObjListMutator {
  friend class ObjListIter<T>;

protected:
  VoidListMutator mut;       // underlying mutator

  #define OWN xassert(owning);
private:
  bool const owning;

  inline void del_itm(T* itm) { if (owning) delete itm; }

public:
  ObjListMutator(ObjList<T> &lst)     : mut(lst.list), owning(lst.owning) { reset(); }

  ~ObjListMutator()                    {}

  void reset()                          { mut.reset(); }

  // iterator copying; safe *only* until one of the mutators modifies
  // the list structure (by inserting or removing), at which time all
  // other iterators might be in limbo
ObjListMutator(ObjListMutator const &obj)             : mut(obj.mut), owning(obj.owning) {}

  ObjListMutator& operator=(ObjListMutator const &obj)  { mut = obj.mut;  return *this; }
    // requires that 'this' and 'obj' already refer to the same 'list'

  // iterator actions
  bool isDone() const                   { return mut.isDone(); }
  void adv()                            { mut.adv(); }
  T *data()                             { return (T*)mut.data(); }
  T *&dataRef()                         { return (T*&)mut.dataRef(); }

  // insertion
  void insertBefore(DBG_INFO_FORMAL_FIRST  T *item)            { OWN mut.insertBefore(DBG_INFO_ARG_FWD_FIRST  item); }
    // 'item' becomes the new 'current', and the current 'current' is
    // pushed forward (so the next adv() will make it current again)

  void insertAfter(DBG_INFO_FORMAL_FIRST  T *item)             { OWN mut.insertAfter(DBG_INFO_ARG_FWD_FIRST  item); }
    // 'item' becomes what we reach with the next adv();
    // isDone() must be false

  void append(DBG_INFO_FORMAL_FIRST  T *item)                  { OWN mut.append(DBG_INFO_ARG_FWD_FIRST  item); }
    // only valid while isDone() is true, it inserts 'item' at the end of
    // the list, and advances such that isDone() remains true; equivalent
    // to { xassert(isDone()); insertBefore(item); adv(); }

  // removal
  T *remove()                           { return (T*)mut.remove(); }
  T *removeAndStuck()                   { return (T*)mut.removeAndStuck(); }
    // 'current' is removed from the list and returned, and whatever was
    // next becomes the new 'current'

  void deleteIt()                       { del_itm((T*)mut.remove()); }
    // same as remove(), except item is deleted also

  // debugging
  void selfCheck() const                { mut.selfCheck(); }
};

#define MUTATE_EACH_OBJLIST(T, list, iter) \
  for(ObjListMutator< T > iter(list); !iter.isDone(); iter.adv())


// for traversing the list without modifying it (neither nodes nor structure)
// NOTE: no list-modification fns should be called on 'list' while this
//       iterator exists
template <class T>
class ObjListIter {
protected:
  VoidListIter iter;      // underlying iterator

public:
  ObjListIter(ObjList<T> const &list) : iter(list.list) {}
  ObjListIter(ObjList<T> const &list, int pos) : iter(list.list, pos) {}
  ~ObjListIter()                       {}

  void reset(ObjList<T> const &list)   { iter.reset(list.list); }

  // iterator copying; generally safe
  ObjListIter(ObjListIter const &obj)             : iter(obj.iter) {}
  ObjListIter& operator=(ObjListIter const &obj)  { iter = obj.iter;  return *this; }

  // but copying from a mutator is less safe; see above
  ObjListIter(ObjListMutator<T> &obj)             : iter(obj.mut) {}

  // iterator actions
  bool isDone() const                   { return iter.isDone(); }
  void adv()                            { iter.adv(); }
  T const *data() const                 { return (T const*)iter.data(); }
};

#define FOREACH_OBJLIST(T, list, iter) \
  for(ObjListIter< T > iter(list); !iter.isDone(); iter.adv())


// intermediate to the above two, this allows modification of the
// objects stored on the list, but not the identity or order of
// the objects in the list
template <class T>
class ObjListIterNC {
protected:
  VoidListIter iter;      // underlying iterator

public:
  ObjListIterNC(ObjList<T> &list) : iter(list.list) {}
  ObjListIterNC(ObjList<T> &list, int pos) : iter(list.list, pos) {}
  ~ObjListIterNC()                     {}

  void reset(ObjList<T> &list)         { iter.reset(list.list); }

  // iterator copying; generally safe
  ObjListIterNC(ObjListIterNC const &obj)             : iter(obj.iter) {}
  ObjListIterNC& operator=(ObjListIterNC const &obj)  { iter = obj.iter;  return *this; }

  // but copying from a mutator is less safe; see above
  ObjListIterNC(ObjListMutator<T> &obj)               : iter(obj.mut) {}

  // iterator actions
  bool isDone() const                   { return iter.isDone(); }
  void adv()                            { iter.adv(); }
  T *data() const                       { return (T*)iter.data(); }
};

#define FOREACH_OBJLIST_NC(T, list, iter) \
  for(ObjListIterNC< T > iter(list); !iter.isDone(); iter.adv())


// iterate over the combined elements of two or more lists
template <class T>
class ObjListMultiIter {
private:
  // all the lists
  ObjList<T> **lists;                // serf array of serf list pointers
  int numLists;                      // length of this array

  // current element
  int curList;                       // which list we're working on
  ObjListIter<T> iter;               // current element of that list

  // invariant:
  //   either curList==numLists, or
  //   iter is not 'done'

public:
  ObjListMultiIter(ObjList<T> **L, int n)
    : lists(L),
      numLists(n),
      curList(0),
      iter(*(lists[0]))
  {
    xassert(n > 0);
    normalize();
  }

  // advance the iterator to the next element of the next non-empty list;
  // establishes invariant above
  void normalize();

  bool isDone() const {
    return curList == numLists;
  }

  T const *data() const {
    return iter.data();
  }

  void adv() {
    iter.adv();
    normalize();
  }
};

// this was originally inline, but that was causing some strange
// problems (compiler bug?)
template <class T>
void ObjListMultiIter<T>::normalize()
{
  while (iter.isDone() && curList < numLists) {
    curList++;
    if (curList < numLists) {
      iter.reset(*(lists[curList]));
    }
  }
}


#endif // OBJLIST_H
