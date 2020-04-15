// tsobjlist.cc            see license.txt for copyright and terms of use
// test of sobjlist.h

#include "sobjlist.h"    // SObjList
#include "str.h"    // SObjList
#include <stdio.h>       // printf

#ifdef TEST_OBJLIST

int main()
{
#if !defined(DBG_INFO_ARG0) || (EXPAND(DBG_INFO_ARG0) == 0)
    str::StoragePool pool;
#else
    str::StoragePool pool(DBG_INFO_ARG0);
#endif
  char const *_hi = "hi there";
  char const *_what = "what's up?";
  str::string hi(_hi);
  str::string what(_what);

  // the real purpose of this test is to make sure it's ok to
  // add a 'const' qualifier inside the angle brackets, and get
  // the effect I'm after
  STORE_NEW_REF0(pool, SObjList<str::string>, list);
  
  // 'prepend' accepts a T*, which should become a char const *;
  // if it only becomes (e.g.) a char*, then this call should
  // trigger a compile error
  list.prepend(DBG_INFO_ARG0_FIRST  &hi);

  list.append(DBG_INFO_ARG0_FIRST  &what);
  
  // 'indexOf' accepts a T const *, so here I'm essentially verifying
  // the compiler doesn't mind seeing 'const' twice
  int i = list.indexOf(&hi);
  printf("index of 'hi' is %d\n", i);

  i = list.indexOf(&what);
  printf("index of 'what' is %d\n", i);

  // random test of extra 'const' outside the template context
  // (gcc-2.95.3 doesn't like it, interesting..)
  int const /*const*/ x = 5;
  printf("x is %d\n", x);

  return 0;
}


#endif
