// asthelp.h            see license.txt for copyright and terms of use
// included by generated ast code

#ifndef ASTHELP_H
#define ASTHELP_H

#include "astlist.h"     // ASTList
#include "fakelist.h"    // FakeList
#include "str.h"         // string
#include "locstr.h"      // LocString

#include <iostream>    // std::ostream
#include <climits>
#include "storage.h"

// ----------------- downcasts --------------------
// the 'if' variants return NULL if the type isn't what's expected;
// the 'as' variants throw an exception in that case
#define DECL_AST_DOWNCASTS(type, tag)            \
  type const *if##type##C() const;               \
  type *if##type()                               \
    { return const_cast<type*>(if##type##C()); } \
  type const *as##type##C() const;               \
  type *as##type()                               \
    { return const_cast<type*>(as##type##C()); } \
  bool is##type() const                          \
    { return kind() == tag; }


#define DEFN_AST_DOWNCASTS(superclass, type, tag)\
  type const *superclass::if##type##C() const    \
  {                                              \
    if (kind() == tag) {                         \
      return (type const*)this;                  \
    }                                            \
    else {                                       \
      return NULL;                               \
    }                                            \
  }                                              \
                                                 \
  type const *superclass::as##type##C() const    \
  {                                              \
    xassert(kind() == tag);                      \
    return (type const*)this;                    \
  }


// ------------------- const typecase --------------------
#define ASTSWITCHC(supertype, nodeptr)           \
{                                                \
  supertype const *switch_nodeptr = (nodeptr);   \
  switch (switch_nodeptr->kind())

#define ASTCASEC(type, var)                           \
  case type::TYPE_TAG: {                              \
    type const *var = switch_nodeptr->as##type##C();

// the "1" versions mean "one argument", i.e. they
// do not bind a variable of the specified type
#define ASTCASEC1(type)                               \
  case type::TYPE_TAG: {

#define ASTNEXTC(type, var)                           \
    break;                                            \
  } /* end previous case */                           \
  case type::TYPE_TAG: {                              \
    type const *var = switch_nodeptr->as##type##C();

#define ASTNEXTC1(type)                               \
    break;                                            \
  } /* end previous case */                           \
  case type::TYPE_TAG: {

// end a case, and add an empty 'default' construct
#define ASTENDCASECD                                  \
    break;                                            \
  } /* end final case */                              \
  default: ;    /* silence warning */                 \
} /* end scope started before switch */

#define ASTDEFAULTC                                   \
    break;                                            \
  } /* end final case */                              \
  default: {

// end a case where an explicit default was present, or
// there is no need to add one (e.g. because it was exhaustive)
#define ASTENDCASEC                                   \
    break;                                            \
  } /* end final case */                              \
} /* end scope started before switch */


// ------------------- non-const typecase --------------------
#define ASTSWITCH(supertype, nodeptr)            \
{                                                \
  supertype *switch_nodeptr = (nodeptr);         \
  switch (switch_nodeptr->kind())

#define ASTCASE(type, var)                            \
  case type::TYPE_TAG: {                              \
    type *var = switch_nodeptr->as##type();

#define ASTCASE1(type)                                \
  case type::TYPE_TAG: {

#define ASTNEXT(type, var)                            \
    break;                                            \
  } /* end previous case */                           \
  case type::TYPE_TAG: {                              \
    type *var = switch_nodeptr->as##type();

#define ASTNEXT1(type)                                \
    break;                                            \
  } /* end previous case */                           \
  case type::TYPE_TAG: {

// end-of-switch behavior is same as in const case
#define ASTENDCASED ASTENDCASECD
#define ASTDEFAULT ASTDEFAULTC
#define ASTENDCASE ASTENDCASEC


// ------------------- const parallel typecase --------------------
// nodeptr1 and nodeptr2 should already be known to have the same kind
#define ASTSWITCH2C(supertype, nodeptr1, nodeptr2)      \
{                                                       \
  supertype const *switch_nodeptr1 = (nodeptr1);        \
  supertype const *switch_nodeptr2 = (nodeptr2);        \
  switch (switch_nodeptr1->kind())

#define ASTCASE2C(type, var1, var2)                     \
  case type::TYPE_TAG: {                                \
    type const *var1 = switch_nodeptr1->as##type##C();  \
    type const *var2 = switch_nodeptr2->as##type##C();

#define ASTCASE2C1(type)                                \
  case type::TYPE_TAG: {

#define ASTNEXT2C(type, var1, var2)                     \
    break;                                              \
  } /* end previous case */                             \
  case type::TYPE_TAG: {                                \
    type const *var1 = switch_nodeptr1->as##type##C();  \
    type const *var2 = switch_nodeptr2->as##type##C();

#define ASTNEXT2C1(type)                                \
    break;                                              \
  } /* end previous case */                             \
  case type::TYPE_TAG: {

// same invocation syntax as ASTNEXT2C but without actually
// declaring the variables because they are unused
#define ASTNEXT2CU(type, var1, var2)                    \
    break;                                              \
  } /* end previous case */                             \
  case type::TYPE_TAG: {

#define ASTENDCASE2CD                                   \
    break;                                              \
  } /* end final case */                                \
  default: ;    /* silence warning */                   \
} /* end scope started before switch */

#define ASTDEFAULT2C                                    \
    break;                                              \
  } /* end final case */                                \
  default: {

#define ASTENDCASE2C                                    \
    break;                                              \
  } /* end final case */                                \
} /* end scope started before switch */



// ------------------- debug print helpers -----------------
//std::ostream &ind(std::ostream &os, int indent);

// I occasionally want to see addresses, so I just throw this
// switch and recompile..
#if 1
  // headers w/o addresses
  #define PRINT_HEADER(subtreeName, clsname)                 \
    str::ind(os, indent) << (subtreeName?subtreeName:"") << " = " #clsname ":";  \
    indent += 2   /* user ; */
#else
  // headers w/ addresses
  #define PRINT_HEADER(subtreeName, clsname)                                           \
    str::ind(os, indent) << (subtreeName?subtreeName:"") << " = " #clsname " (" << ((void*)this) << "):";  \
    indent += 2   /* user ; */
#endif


#define PRINT_STRING(var) \
  debugPrintStr((var), #var, os, indent)    /* user ; */

void debugPrintStr(string const &s, char const *name,
                   std::ostream &os, int indent);
void debugPrintStr(char const *s, char const *name,
                   std::ostream &os, int indent);


#define PRINT_CSTRING(var) \
  debugPrintCStr(var, #var, os, indent)    /* user ; */

void debugPrintCStr(char const *s, char const *name,
                    std::ostream &os, int indent);


#define PRINT_LIST(T, list) \
  debugPrintList(list, #list, os, indent)     /* user ; */

template <class T>
void debugPrintList(ASTList<T> const &list, char const *name,
                    std::ostream &os, int indent)
{
  if (indent > DEBUG_MAX_IND || list.isDeleted()) { str::ind(os,indent)<< "name"<<(void*)&list<<"..."; return; }
  str::ind(os, indent) << name << ":";
  int ct=0;
  {
    FOREACH_ASTLIST(T, list, iter) {
      iter.data()->debugPrint(os, indent+2,
        stringc << name << "[" << ct++ << "]");
    }
  }
}

// provide explicit specialization for strings
void debugPrintList(ASTList<string> const &list, char const *name,
                    std::ostream &os, int indent);
void debugPrintList(ASTList<LocString> const &list, char const *name,
                    std::ostream &os, int indent);


#define PRINT_FAKE_LIST(T, list) \
  debugPrintFakeList(list, #list, os, indent)     /* user ; */

template <class T>
void debugPrintFakeList(FakeList<T> const *list, char const *name,
                        std::ostream &os, int indent)
{
  if (indent > DEBUG_MAX_IND || !list || list->isDeleted()) { str::ind(os,indent)<< "name"<<(void*)list<<"..."; return; }
  str::ind(os, indent) << name << ":";
  int ct=0;
  {
    FAKELIST_FOREACH(T, list, iter) {
      iter->debugPrint(os, indent+2,
        stringc << name << "[" << ct++ << "]");
    }
  }
}

// note that we never make FakeLists of strings, since of course
// strings do not have a 'next' pointer


#define PRINT_SUBTREE(tree)                     \
  if (tree) {                                   \
    (tree)->debugPrint(os, indent, #tree);      \
  }                                             \
  else {                                        \
    str::ind(os, indent) << #tree << " is null";   \
  } /* user ; (optional) */


#define PRINT_GENERIC(var) \
  str::ind(os, indent) << #var << " = "; debugString(os, var, indent+1); os << ""   /* user ; */


#define PRINT_BOOL(var) \
  str::ind(os, indent) << #var << " = " << (var? "true" : "false") << ""   /* user ; */


// ------------------- xml print helpers -----------------
// dsw: given above in the debug print section.
//  std::ostream &ind(std::ostream &os, int indent);

#define XMLPRINT_HEADER(clsname)                            \
  str::ind(os, indent) << "<object type=\"" << #clsname "\">"; \
  indent += 2   /* user ; */                                \

#define XMLPRINT_FOOTER(clsname)                            \
  indent -= 2;                                              \
  str::ind(os, indent) << "</object>" /* user ; */

#define XMLPRINT_STRING(var)                                \
  xmlPrintStr(var, #var, os, indent) /* user ; */

void xmlPrintStr(string const &s, char const *name,
                 std::ostream &os, int indent);


#define XMLPRINT_LIST(T, list)                              \
  xmlPrintList(list, #list, os, indent) /* user ; */

template <class T>
void xmlPrintList(ASTList<T> const &list, char const *name,
                  std::ostream &os, int indent)
{
  if (indent > DEBUG_MAX_IND || list.isDeleted()) { str::ind(os,indent)<< "name"<<(void*)&list<<"..."; return; }
  str::ind(os, indent) << "<member type=list name=\"" << name << "\">";
  {
    FOREACH_ASTLIST(T, list, iter) {
      iter.data()->xmlPrint(os, indent+2);
    }
  }
  str::ind(os, indent) << "</member>";
}

// provide explicit specialization for strings
void xmlPrintList(ASTList<string> const &list, char const *name,
                    std::ostream &os, int indent);
void xmlPrintList(ASTList<LocString> const &list, char const *name,
                    std::ostream &os, int indent);


#define XMLPRINT_FAKE_LIST(T, list) \
  xmlPrintFakeList(list, #list, os, indent)     /* user ; */

template <class T>
void xmlPrintFakeList(FakeList<T> const *list, char const *name,
                        std::ostream &os, int indent)
{
  if (indent > DEBUG_MAX_IND || !list || list->isDeleted()) { str::ind(os,indent)<< "name"<<(void*)list<<"..."; return; }
  str::ind(os, indent) << "<member type=fakelist name=\"" << name << "\">";
  {
    FAKELIST_FOREACH(T, list, iter) {
      iter->xmlPrint(os, indent+2);
    }
  }
  str::ind(os, indent) << "</member>";
}

// note that we never make FakeLists of strings, since of course
// strings do not have a 'next' pointer


#define XMLPRINT_SUBTREE(tree)                         \
  if (tree) {                                          \
    (tree)->xmlPrint(os, indent);                      \
  }                                                    \
  else {                                               \
    xassert(0); /* dsw:not sure what to do here yet */ \
    str::ind(os, indent) << #tree << " is null";          \
  } /* user ; (optional) */


// dsw: there's no way this can work in general
#define XMLPRINT_GENERIC(var)                                                         \
  str::ind(os, indent) << "<member type=generic name=\"" << #var << "\">";               \
  str::ind(os, indent+2) << "<object type=generic val=\"" << ::toString(var) << "\" />"; \
  str::ind(os, indent) << "</member>"   /* user ; */


#define XMLPRINT_BOOL(var)                                                                 \
  str::ind(os, indent) << "<member type=bool name=\"" << #var << "\">";                       \
  str::ind(os, indent+2) << "<object type=bool val=\"" << (var? "true" : "false") << "\" />"; \
  str::ind(os, indent) << "</member>"   /* user ; */

  
// print a pointer address as an id, for example "FL0x12345678";
// guaranteed to print (e.g.) "FL0" for NULL pointers; the
// "FL" part is the label
void xmlPrintPointer(std::ostream &os, char const *label, void const *p);


// ---------------------- deep-copy ------------------
// returns a new'd list because the AST node ctors want
// to accept an owner ptr to a list
template <class T>
ASTList<T> * /*owner*/ cloneASTList(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, ASTList<T> const &src, int deepness=1, int listDeepness=1)
{
  deepness--;listDeepness--;

  const bool d = (deepness >= 0);
  ASTList<T> *ret = new (pool) ASTList<T>(DBG_INFO_ARG_FWD_FIRST  pool);
  if (d) {
      FOREACH_ASTLIST(T, src, iter) {
        ret->append(DBG_INFO_ARG_FWD_FIRST  iter.data()->clone(DBG_INFO_ARG_FWD_FIRST  pool, deepness, listDeepness));
      }
  } else {
      ret->assign(src, false);
      //FOREACH_ASTLIST(T, src, iter) {
      //  ret->append(constcast(iter.data()));
      //}
  }

  return ret;
}


// returns owner pointer to list of serfs.. using this isn't ideal
// because ASTList normally is owning, and probably deletes its
// elements in its destructor..
template <class T>
ASTList<T> * /*owner*/ shallowCloneASTList(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, ASTList<T> const &src)
{
  ASTList<T> *ret = new (pool) ASTList<T>(DBG_INFO_ARG_FWD_FIRST  pool);
  ret->assign(src, false);

  //FOREACH_ASTLIST(T, src, iter) {
    // list backbone is const, but nodes' constness leaks away..
  //  ret->append(const_cast<T*>(iter.data()));
  //}

  return ret;
}


// deep copy of a FakeList
template <class T>
FakeList<T> * /*owner*/ cloneFakeList(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, FakeList<T> const *src, int const deepness=1, int const listDeepness=1)
{
  if (!src) {
    return FakeList<T>::emptyList();     // base case of recursion
  }

  T *head = constcast(src->firstC());
  if (deepness > 0) {
      // clone first element
      head = head->clone(DBG_INFO_ARG_FWD_FIRST  pool, deepness-1, listDeepness-1);
      xassert(head->next == NULL);     // it had better not copy the list tail itself!
  }
  // attach to result of cloning the tail
  FakeList<T> *tail = cloneFakeList(DBG_INFO_ARG_FWD_FIRST  pool, src->butFirstC(), deepness, listDeepness);
  return tail->prepend(head);
}


#endif // ASTHELP_H
