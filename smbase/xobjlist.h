m4_dnl // xobjlist.h            see license.txt for copyright and terms of use
m4_dnl // template file to be processed with m4 to generate one
m4_dnl // of the wrappers around VoidList
m4_dnl
m4_changequote([, ])m4_dnl      // for this section
m4_changecom[]m4_dnl            // no m4 "comments"
m4_ifelse(m4_output, sobjlist.h, [m4_dnl
// sobjlist.h
// serf list of arbitrary objects
m4_define(makeName, S[$1])m4_dnl
m4_define(outputCond, [$1])m4_dnl       // select 1st arg
m4_define(SPC, [])m4_dnl
], [m4_dnl
// objlist.h
// owner list of arbitrary dynamically-allocated objects
m4_define(makeName, [$1])m4_dnl
m4_define(outputCond, [$2])m4_dnl       // select 2nd arg
m4_define(SPC, [ ])m4_dnl               // for balancing lined-up comments
])m4_dnl
m4_define(includeLatch, makeName(OBJLIST_H))m4_dnl
m4_define(className, makeName(ObjList))m4_dnl
m4_define(iterName, makeName(ObjListIter))m4_dnl
m4_define(mutatorName, makeName(ObjListMutator))m4_dnl
m4_define(iterNameNC, makeName(ObjListIterNC))m4_dnl
m4_define(multiIterName, makeName(ObjListMultiIter))m4_dnl
m4_changequote(, )m4_dnl              // so quotes are not quoted..
m4_changequote([[[, ]]])m4_dnl        // reduce likelihood of confusion
// NOTE: automatically generated from xobjlist.h -- do not edit directly

// Author: Scott McPeak, 2000

#ifndef includeLatch
#define includeLatch

#include <boost/type_index.hpp>
#include "voidlist.h"    // VoidList
#include "storage.h"    // VoidList


// forward declarations of template classes, so we can befriend them in className
// (not required by Borland C++ 4.5, but GNU wants it...)
template <class T> class iterName;
template <class T> class mutatorName;
template <class T> class iterNameNC;


outputCond([[[m4_dnl      // sobjlist
// the list is considered to not own any of the items; it's ok to
// insert items multiple times or into multiple lists
template<typename T> class ObjList;
]]], [[[m4_dnl            // objlist
// the list is considered to own all of the items; it is an error to insert
// an item into more than one such list, or to insert an item more than once
// into any such list
template<typename T> class SObjList;
]]])m4_dnl
template <class T>
class className : public str::Storeable {
private:
  friend class iterName<T>;
  friend class mutatorName<T>;
  friend class iterNameNC<T>;
  friend class Grammar;
outputCond([[[m4_dnl      // sobjlist
  friend class ObjList<T>;
]]], [[[m4_dnl            // objlist
  friend class SObjList<T>;
]]])m4_dnl

#ifdef DEBUG
  className[[[]]](className const &obj);//undefined
#endif

protected:
  VoidList list;                        // list itself


outputCond([[[m4_dnl    // sobjlist
  #define OWN
  #define NOWN
public:
  // make shallow copies
  className[[[]]](DBG_INFO_FORMAL_FIRST className const &obj, bool move=false)         : str::Storeable(DBG_INFO_ARG_FWD_FIRST  obj, false), list(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr) {  list.chk_assign(obj.list, move);   }
  className[[[]]](DBG_INFO_FORMAL_FIRST __StoreAlreadyConstr StoreAlreadyConstr)       : str::Storeable(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr), list(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr) {     }
  className& operator= (className const &src)         { list = src.list; return *this; }

  public:
  className[[[]]](DBG_INFO_FORMAL)                            : str::Storeable(DBG_INFO_ARG_FWD_FIRST  sizeof(className[[[]]])), list(DBG_INFO_ARG_FWD) {}
  className[[[]]](DBG_INFO_FORMAL_FIRST str::StoragePool &pool)                            : str::Storeable(DBG_INFO_ARG_FWD_FIRST  pool), list(DBG_INFO_ARG_FWD_FIRST  *this,0) {}
  className[[[]]](DBG_INFO_FORMAL_FIRST str::Storeable const &parent)                      : str::Storeable(DBG_INFO_ARG_FWD_FIRST  parent, sizeof(className)), list(DBG_INFO_ARG_FWD_FIRST  *this,0) {}
]]], [[[m4_dnl          // objlist
  //#define OWN xassert(owning);
  //#define NOWN xassert(!owning);
  #define OWN
  #define NOWN
private:
  //bool const owning;
  // make shallow copies and non-owning list
  className[[[]]](DBG_INFO_FORMAL_FIRST className const &obj, bool move=false)         : str::Storeable(DBG_INFO_ARG_FWD_FIRST  obj, false), list(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr)/*, owning(false)*/ {  list.chk_assign(obj.list, move);   }
  className[[[]]](DBG_INFO_FORMAL_FIRST __StoreAlreadyConstr StoreAlreadyConstr)       : str::Storeable(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr), list(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr) {     }
  className& operator= (className const &src) { NOWN list = src.list; return *this;  }

  inline void del_itm(T* itm) { /*if (owning) delete itm;*/ }

  public:
  className[[[]]](DBG_INFO_FORMAL)                            : str::Storeable(DBG_INFO_ARG_FWD_FIRST  sizeof(className[[[]]])), list(DBG_INFO_ARG_FWD)/*, owning(true)*/ {}
  className[[[]]](DBG_INFO_FORMAL_FIRST str::StoragePool &pool)                            : str::Storeable(DBG_INFO_ARG_FWD_FIRST  pool), list(DBG_INFO_ARG_FWD_FIRST  *this,0)/*, owning(true)*/ {}
  className[[[]]](DBG_INFO_FORMAL_FIRST str::Storeable const &parent)                      : str::Storeable(DBG_INFO_ARG_FWD_FIRST  parent, sizeof(className)), list(DBG_INFO_ARG_FWD_FIRST  *this,0)/*, owning(true)*/ {}
]]])m4_dnl

  ~className[[[]]]()                      m4_dnl
     outputCond({}    /* all items removed */, { deleteAll(); })

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
outputCond([[[m4_dnl     // sobjlist
  void removeAll()                      { list.removeAll(); }
]]], [[[m4_dnl           // objlist
  void deleteAt(int index)              { del_itm((T*)list.removeAt(index)); }
  void deleteAll();
]]])m4_dnl

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

  //outputCond([[[m4_dnl    // sobjlist
  //]]], [[[m4_dnl          // objlist
    // (we do *not* have appendAll, since these are supposed to be owner lists)
  //]]])m4_dnl

  template <class XObjList>
  void appendAll(XObjList const &tail)              { NOWN list.appendAll(tail.list); }

  template <class XObjList>
  void prependAll(XObjList const &head)             { NOWN list.prependAll(head.list); }

  // steal
  void stealTailAt(int index, className &tail)       { list.stealTailAt(index, tail.list); }

  // equal items in equal positions
  bool equalAsLists(className const &otherList, Diff diff, str::Storeable const *extra=NULL) const
    { return list.equalAsLists(otherList.list, (VoidDiff)diff, extra); }
  int compareAsLists(className const &otherList, Diff diff, str::Storeable const *extra=NULL) const
    { return list.compareAsLists(otherList.list, (VoidDiff)diff, extra); }

  // last-as-set: comparisons (NOT efficient)
  bool equalAsSets(className const &otherList, Diff diff, str::Storeable const *extra=NULL) const
    { return list.equalAsSets(otherList.list, (VoidDiff)diff, extra); }
  bool isSubsetOf(className const &otherList, Diff diff, str::Storeable const *extra=NULL) const
    { return list.isSubsetOf(otherList.list, (VoidDiff)diff, extra); }
  bool containsByDiff(T const *item, Diff diff, str::Storeable const *extra=NULL) const
    { return list.containsByDiff(item, (VoidDiff)diff, extra); }

  // treating the pointer values themselves as the basis for comparison
  bool equalAsPointerLists(className const &otherList) const
    { return list.equalAsPointerLists(otherList.list); }
  bool equalAsPointerSets(className const &otherList) const
    { return list.equalAsPointerSets(otherList.list); }

outputCond([[[m4_dnl    // sobjlist
  // removing duplicates
  void removeDuplicatesAsMultiset(Diff diff, str::Storeable const *extra=NULL)
    { list.removeDuplicatesAsMultiset((VoidDiff)diff, extra); }
  void removeDuplicatesAsPointerMultiset()
    { list.removeDuplicatesAsPointerMultiset(); }

  // debugging: no invariants beyond VoidList
  void selfCheck() const                { list.selfCheck(); }
  
  // but export the additional checks for cases where they apply anyway
  void checkHeapDataPtrs() const        { list.checkHeapDataPtrs(); }
  void checkUniqueDataPtrs() const      { list.checkUniqueDataPtrs(); }
]]], [[[m4_dnl          // objlist
  // debugging: two additional invariants
  void selfCheck() const { 
    list.selfCheck();
    list.checkHeapDataPtrs();
    list.checkUniqueDataPtrs();
  }
]]])m4_dnl

  Storeable::debugPrint;
  outputCond([[[m4_dnl      // sobjlist
  void debugPrint(str::stringstream& os, int indent = 0, char const *subtreeName = 0) const        {
      if (indent > DEBUG_MAX_IND || isDeleted()) { str::ind(os,indent)<< "sobj<"<< boost::typeindex::type_id<T>().pretty_name() <<">..."; return; }
      str::ind(os,indent)<<"sobj<"<< boost::typeindex::type_id<T>().pretty_name() <<">:"; list.debugPrint(os, indent); }
  ]]], [[[m4_dnl            // objlist
  void debugPrint(str::stringstream& os, int indent = 0, char const *subtreeName = 0) const        {
      if (indent > DEBUG_MAX_IND || isDeleted()) { str::ind(os,indent)<< "obj<"<< boost::typeindex::type_id<T>().pretty_name() <<">..."; return; }
      str::ind(os,indent)<<"obj<"<< boost::typeindex::type_id<T>().pretty_name() <<">:"; list.debugPrint(os, indent); }
  ]]])m4_dnl
};


outputCond(, [[[m4_dnl      // objlist
template <class T>
void ObjList<T>::deleteAll()
{
  while (!list.isEmpty()) {
    deleteAt(0);
  }
}


]]])m4_dnl
// for traversing the list and modifying it (nodes and/or structure)
// NOTE: no list-modification fns should be called on 'list' while this
//       iterator exists, and only one such iterator should exist for
//       any given list
template <class T>
class mutatorName {
  friend class iterName<T>;

protected:
  VoidListMutator mut;       // underlying mutator

outputCond([[[m4_dnl    // sobjlist
  #define OWN
public:
  mutatorName[[[]]](className<T> &lst)     : mut(lst.list) { reset(); }
]]], [[[m4_dnl          // objlist
  #define OWN
  //#define OWN xassert(owning);
private:
  //bool const owning;

  inline void del_itm(T* itm) { /*if (owning) delete itm;*/ }

public:
  mutatorName[[[]]](className<T> &lst)     : mut(lst.list)/*, owning(lst.owning)*/ { reset(); }
]]])m4_dnl

  ~mutatorName[[[]]]()                    {}

  void reset()                          { mut.reset(); }

  // iterator copying; safe *only* until one of the mutators modifies
  // the list structure (by inserting or removing), at which time all
  // other iterators might be in limbo
outputCond([[[m4_dnl    // sobjlist
mutatorName[[[]]](mutatorName const &obj)             : mut(obj.mut) {}
]]], [[[m4_dnl          // objlist
mutatorName[[[]]](mutatorName const &obj)             : mut(obj.mut)/*, owning(obj.owning)*/ {}
]]])m4_dnl

  mutatorName& operator=(mutatorName const &obj)  { mut = obj.mut;  return *this; }
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

outputCond(, [[[m4_dnl    // sobjlist
  void deleteIt()                       { del_itm((T*)mut.remove()); }
    // same as remove(), except item is deleted also

]]])m4_dnl
  // debugging
  void selfCheck() const                { mut.selfCheck(); }
};

#define makeName(MUTATE_EACH_OBJLIST)(T, list, iter) \
  for(mutatorName< T > iter(list); !iter.isDone(); iter.adv())


// for traversing the list without modifying it (neither nodes nor structure)
// NOTE: no list-modification fns should be called on 'list' while this
//       iterator exists
template <class T>
class iterName {
protected:
  VoidListIter iter;      // underlying iterator

public:
  iterName[[[]]](className<T> const &list) : iter(list.list) {}
  iterName[[[]]](className<T> const &list, int pos) : iter(list.list, pos) {}
  ~iterName[[[]]]()                       {}

  void reset(className<T> const &list)   { iter.reset(list.list); }

  // iterator copying; generally safe
  iterName[[[]]](iterName const &obj)             : iter(obj.iter) {}
  iterName& operator=(iterName const &obj)  { iter = obj.iter;  return *this; }

  // but copying from a mutator is less safe; see above
  iterName[[[]]](mutatorName<T> &obj)             : iter(obj.mut) {}

  // iterator actions
  bool isDone() const                   { return iter.isDone(); }
  void adv()                            { iter.adv(); }
  T const *data() const                 { return (T const*)iter.data(); }
};

#define makeName(FOREACH_OBJLIST)(T, list, iter) \
  for(iterName< T > iter(list); !iter.isDone(); iter.adv())


// intermediate to the above two, this allows modification of the
// objects stored on the list, but not the identity or order of
// the objects in the list
template <class T>
class iterNameNC {
protected:
  VoidListIter iter;      // underlying iterator

public:
  iterNameNC[[[]]](className<T> &list) : iter(list.list) {}
  iterNameNC[[[]]](className<T> &list, int pos) : iter(list.list, pos) {}
  ~iterNameNC[[[]]]()                     {}

  void reset(className<T> &list)         { iter.reset(list.list); }

  // iterator copying; generally safe
  iterNameNC[[[]]](iterNameNC const &obj)             : iter(obj.iter) {}
  iterNameNC& operator=(iterNameNC const &obj)  { iter = obj.iter;  return *this; }

  // but copying from a mutator is less safe; see above
  iterNameNC[[[]]](mutatorName<T> &obj)               : iter(obj.mut) {}

  // iterator actions
  bool isDone() const                   { return iter.isDone(); }
  void adv()                            { iter.adv(); }
  T *data() const                       { return (T*)iter.data(); }
};

#define makeName(FOREACH_OBJLIST_NC)(T, list, iter) \
  for(iterNameNC< T > iter(list); !iter.isDone(); iter.adv())


// iterate over the combined elements of two or more lists
template <class T>
class multiIterName {
private:
  // all the lists
  className<T> **lists;               SPC// serf array of serf list pointers
  int numLists;                      // length of this array

  // current element
  int curList;                       // which list we're working on
  iterName<T> iter;              SPC// current element of that list

  // invariant:
  //   either curList==numLists, or
  //   iter is not 'done'

public:
  multiIterName[[[]]](className<T> **L, int n)
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
void multiIterName<T>::normalize()
{
  while (iter.isDone() && curList < numLists) {
    curList++;
    if (curList < numLists) {
      iter.reset(*(lists[curList]));
    }
  }
}


#endif // includeLatch
