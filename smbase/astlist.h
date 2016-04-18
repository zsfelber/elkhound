// astlist.h            see license.txt for copyright and terms of use
// owner list wrapper around VoidTailList
// name 'AST' is because the first application is in ASTs

#ifndef ASTLIST_H
#define ASTLIST_H

#include <boost/type_index.hpp>
#include "vdtllist.h"     // VoidTailList
#include "storage.h"

template <class T> class ASTListIter;
template <class T> class ASTListIterNC;

// a list which owns the items in it (will deallocate them), and
// has constant-time access to the last element
template <class T>
class ASTList : public str::Storeable {
private:
  friend class ASTListIter<T>;        
  friend class ASTListIterNC<T>;

protected:
  VoidTailList list;                    // list itself

private:
  ASTList(ASTList const &obj);          // not allowed

public:
  static ASTList<T> const EMPTY;

  ASTList(DBG_INFO_FORMAL) : str::Storeable(DBG_INFO_ARG_FWD_FIRST  sizeof(ASTList<T>)), list(DBG_INFO_ARG_FWD) {}
  ASTList(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool) : str::Storeable(DBG_INFO_ARG_FWD_FIRST  pool), list(DBG_INFO_ARG_FWD_FIRST  *this) {}
  ASTList(DBG_INFO_FORMAL_FIRST  str::Storeable &parent) : str::Storeable(DBG_INFO_ARG_FWD_FIRST  parent, sizeof(ASTList)), list(DBG_INFO_ARG_FWD_FIRST  *this) {}
  ~ASTList()                            {  }

  // ctor to make singleton list; often quite useful
  ASTList(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, T *elt)                       : str::Storeable(DBG_INFO_ARG_FWD_FIRST  pool), list(DBG_INFO_ARG_FWD_FIRST  *this) { prepend(DBG_INFO_ARG_FWD_FIRST  elt); }

  // use move:true as stealing ctor; among other things, since &*this is assumed to
  // point at 'src', this class can't have virtual functions;
  // these ctors delete 'src'
  ASTList(DBG_INFO_FORMAL_FIRST  str::StoragePool const *pool0, ASTList<T> &src, bool move=true) : str::Storeable(DBG_INFO_ARG_FWD_FIRST  src, false), list(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr) { assignParent(pool0); list.chk_assign(src.list, move); }
  ASTList(DBG_INFO_FORMAL_FIRST  str::StoragePool const *pool0, ASTList<T> *src, bool move=true) : str::Storeable(DBG_INFO_ARG_FWD_FIRST  NN(src), false), list(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr) { assignParent(pool0); list.chk_assign(src->list, move); }
  // move:false is default with const making ast tree generation easy
  ASTList(DBG_INFO_FORMAL_FIRST  str::StoragePool const *pool0, ASTList<T> const &src) : str::Storeable(DBG_INFO_ARG_FWD_FIRST  src, false), list(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr) { assignParent(pool0); list.chk_assign(src.list, false); }
  ASTList(DBG_INFO_FORMAL_FIRST  str::StoragePool const *pool0, ASTList<T> const *src) : str::Storeable(DBG_INFO_ARG_FWD_FIRST  NN(src), false), list(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr) { assignParent(pool0); list.chk_assign(src->list, false); }

  VoidTailList const & getList() {
      return list;
  }

  void assign(ASTList<T> const &src, bool move)           { list.assign(src.list, move); }
  void assign(ASTList<T> const *src, bool move)           { list.assign(NN(src).list, move); }

  inline void assignParent(str::StoragePool const *pool0) {
      Storeable::assignParent(pool0);
      list.assignParent(pool0);
  }

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
  void prepend(DBG_INFO_FORMAL_FIRST  T *newitem)              { list.prepend(DBG_INFO_ARG_FWD_FIRST  newitem); }
  void append(DBG_INFO_FORMAL_FIRST  T *newitem)               { list.append(DBG_INFO_ARG_FWD_FIRST  newitem); }
  void appendAll(ASTList<T> &tail)      { list.appendAll(tail.list); }
  void prependAll(ASTList<T> &tail)      { list.prependAll(tail.list); }
  void appendAllNew(ASTList<T> const &tail, VoidEq eq)    { list.appendAllNew(tail.list, eq); }
  void reappendAll(ASTList<T> const &tail, VoidEq eq)    { list.reappendAll(tail.list, eq); }
  void insertAt(DBG_INFO_FORMAL_FIRST  T *newitem, int index)  { list.insertAt(DBG_INFO_ARG_FWD_FIRST  newitem, index); }
  void concat(ASTList<T> &tail)         { list.concat(tail.list); }

  // removal
  T *removeFirst()                      { return (T*)list.removeFirst(); }
  T *removeLast()                       { return (T*)list.removeLast(); }
  T *removeAt(int index)                { return (T*)list.removeAt(index); }
  void removeItem(T *item)              { list.removeItem(item); }
  
  // this one is awkwardly named to remind the user that it's
  // contrary to the usual intent of this class
  void removeAll_dontDelete()           { list.removeAll(); }

  // deletion
  void deleteFirst()                    { delete (T*)list.removeFirst(); }
  //void deleteAllOwning();
  //void deleteAll();

  // list-as-set: selectors
  int indexOf(T const *item) const      { return list.indexOf(item); }
  int indexOfF(T const *item) const     { return list.indexOfF(item); }
  bool contains(T const *item) const    { return list.contains(item); }

  // list-as-set: mutators
  bool prependUnique(T *newitem)        { return list.prependUnique(newitem); }
  bool appendUnique(T *newitem)         { return list.appendUnique(newitem); }

  // debugging: two additional invariants
  void selfCheck() const                { list.selfCheck(); }
  void debugPrint(std::ostream& os, int indent = 0, char const *subtreeName = 0) const        { str::ind(os,indent)<<"ast<"<< boost::typeindex::type_id<T>().pretty_name() <<">:"; list.debugPrint(os); }
};


template <class T>
#ifdef DEBUG
ASTList<T> const ASTList<T>::EMPTY(DBG_INFO_ARG0);
#else
ASTList<T> const ASTList<T>::EMPTY;
#endif

/*
template <class T>
void ASTList<T>::deleteAllOwning()
{
  if (owning) {
    deleteAll();
  }
}

template <class T>
void ASTList<T>::deleteAll()
{
  //while (!list.isEmpty()) {
  //  deleteFirst();
  //}
  list.deleteAll();
}
*/

template <class T>
class ASTListIter {
protected:
  VoidTailListIter iter;      // underlying iterator

public:
  ASTListIter(ASTList<T> const &list) : iter(list.list) {}
  ~ASTListIter()                       {}

  void reset(ASTList<T> const &list)   { iter.reset(list.list); }

  // iterator copying; generally safe
  ASTListIter(ASTListIter const &obj)             : iter(obj.iter) {}
  ASTListIter& operator=(ASTListIter const &obj)  { iter = obj.iter;  return *this; }

  // iterator actions
  bool isDone() const                   { return iter.isDone(); }
  void adv()                            { iter.adv(); }
  T const *data() const                 { return (T const*)iter.data(); }
};

#define FOREACH_ASTLIST(T, list, iter) \
  for(ASTListIter<T> iter(list); !iter.isDone(); iter.adv())


// version of the above, but for non-const-element traversal
template <class T>
class ASTListIterNC {
protected:
  VoidTailListIter iter;      // underlying iterator

public:
  ASTListIterNC(ASTList<T> &list)      : iter(list.list) {}
  ~ASTListIterNC()                     {}

  void reset(ASTList<T> &list)         { iter.reset(list.list); }

  // iterator copying; generally safe
  ASTListIterNC(ASTListIterNC const &obj)             : iter(obj.iter) {}
  ASTListIterNC& operator=(ASTListIterNC const &obj)  { iter = obj.iter;  return *this; }

  // iterator actions
  bool isDone() const                   { return iter.isDone(); }
  void adv()                            { iter.adv(); }
  T *data() const                       { return (T*)iter.data(); }
  T *&dataRef()                         { return (T*&)iter.dataRef(); }

  // iterator mutation; use with caution
  void setDataLink(T *newData)          { iter.setDataLink(newData); }
};

#define FOREACH_ASTLIST_NC(T, list, iter) \
  for(ASTListIterNC<T> iter(list); !iter.isDone(); iter.adv())


// this function is somewhat at odds with the nominal purpose
// of ASTLists, but I need it in a weird situation so ...
template <class T>
ASTList<T> *shallowCopy(str::StoragePool &pool, ASTList<T> *src)
{
  ASTList<T> *ret = new (pool) ASTList<T>;
  FOREACH_ASTLIST_NC(T, *src, iter) {
    ret->append(iter.data());
  }
  return ret;
}


#endif // ASTLIST_H
