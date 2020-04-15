// malloc_stub.h            see license.txt for copyright and terms of use
// no-op implementation of ckheap.h

#include "ckheap.h"     // interface to implement

void checkHeap() {}

#if defined(__WIN32__)||defined(_WIN32)
#else
unsigned numMallocCalls() { return 0; }
unsigned numFreeCalls() { return 0; }

void malloc_stats() {}
void checkHeapNode(void *node) {}
void walkMallocHeap(HeapWalkFn func) {}
#endif

// EOF
