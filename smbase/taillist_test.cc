// taillist_test.cc; see License.txt for copyright and terms of use

// test code for taillist
#include "taillist.h"
#include "test.h" // USUAL_MAIN
#include "int.h"

void entry()
{
  Integer a(DBG_INFO_ARG0_FIRST  2),
            b(DBG_INFO_ARG0_FIRST  4),
              c(DBG_INFO_ARG0_FIRST  8),
                d(DBG_INFO_ARG0_FIRST  16),
                  e(DBG_INFO_ARG0_FIRST  42);

  TailList<Integer> list;

  list.append(DBG_INFO_ARG0_FIRST  &d);
  list.prepend(DBG_INFO_ARG0_FIRST  &b);		// 4, 16
  list.insertAt(DBG_INFO_ARG0_FIRST  &c,1); 		// 4, 8, 16
  list.append(DBG_INFO_ARG0_FIRST  &e); 		// 4, 8, 16, 42
  
  list.removeLast();		// 4, 8, 16

  xassert( list.count() == 3 &&
	   !list.isEmpty() &&
	   list.nth(0) == &b &&
	   list.nth(1) == &c &&
	   list.nth(2) == &d &&
	   list.indexOf(&b) == 0 &&
	   list.indexOf(&c) == 1 &&
	   list.indexOf(&e) == -1 &&
	   list.indexOf(&d) == 2
	   );
  
  // FIX: the selfCheck routine in the VoidTailList superclass is broken. 
  // list.selfCheck();
  
  list.prepend(DBG_INFO_ARG0_FIRST  &a);		// 2, 4, 8, 16
  
  int count = 2;
  FOREACH_TAILLIST_NC(Integer, list, iter) {
    xassert( iter.data()->i == count);
    count *= 2;
  }

}

USUAL_MAIN
