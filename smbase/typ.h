// typ.h            see license.txt for copyright and terms of use
// various types and definitions, some for portability, others for convenience
// Scott McPeak, 1996-2000  This file is public domain.

#ifndef __TYP_H
#define __TYP_H


#include "defs.h"
#include <cstdint>


// This used when I want to cast a pointer to an integer for something
// like hashing the address.  It need not be injective.
//inline long pointerToInteger(void const *p)
//  { return (long)p; }
inline uintptr_t pointerToInteger(void const *p)
    { return reinterpret_cast<std::uintptr_t>(p); }

// This can be used to compare two pointers, even when they do not point
// into the same object.
inline int comparePointerAddresses(void const *p, void const *q)
{
  // John Skaller points out that comparing addresses directly is
  // nonportable, and that std::less<> provides a solution (cppstd
  // 20.3.3p8).  But, I'm concerned about the portability of std::less
  // more than I am about the portability of address comparison.  The
  // existence of this function at least ensures I only have to change
  // one place.
  return p==q?               0 :
         pointerToInteger(p) < pointerToInteger(q)? -1 :      // would use std::less<> here
                            +1 ;
}


template <typename P> OPT P* constcast(P const * p) {
    return const_cast<P*>(p);
}

template <typename P> OPT P& constcast(P const & p) {
    return const_cast<P&>(p);
}

template <typename P>
P& NN(P* a) {
    if (!a) {
        x_assert_fail("Non-nullable pointer is null.", __FILE__, __LINE__);
    }
    return *a;
}

#endif // __TYP_H

