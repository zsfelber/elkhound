// malloc_stub.h            see license.txt for copyright and terms of use
// no-op implementation of ckheap.h

#include "ckheap.h"     // interface to implement

void checkHeap() {}

unsigned numMallocCalls() { return 0; }
unsigned numFreeCalls() { return 0; }

#if defined(_MSC_VER)|!defined(__WIN32__)
void malloc_stats() {}
void checkHeapNode(void *node) {}
void walkMallocHeap(HeapWalkFn func) {}
#endif

// EOF
