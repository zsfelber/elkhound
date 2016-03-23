#ifndef STORAGE_H
#define STORAGE_H

#include <vector>

template <typename P> inline P* constcast(P const * p) {
    return const_cast<P*>(p);
}

template <typename P> inline P& constcast(P const & p) {
    return const_cast<P&>(p);
}

// 128
static const int STORE_BUF_PTR_BITS = 7;
static const int STORE_BUF_PTR_SZ = 1 << STORE_BUF_BITS;

// 16 Mbytes
static const int STORE_BUF_BITS = 24;
static const int STORE_BUF_SZ = 1 << STORE_BUF_BITS;

class Storeable {

public:
   StoragePool const * const __pool;

protected:
   Storeable() : __pool(0) {}
};

class StoragePool {
private:

   StoragePool const * const parent;
   std::vector<unsigned char> memory;
   std::vector<Storeable**> pointers;

#define CPY(oldPool,op,oldmem,oldmemed) \
   long long d = ((long long)&memory[0])-oldmem; \
   for (std::vector<Storeable**>::iterator it = constcast(oldPool.pointers).begin(); it!=constcast(oldPool.pointers).end(); it++) { \
       Storeable** ptr = *it; \
       convert(oldPool, oldmem, ptr, d); \
       op; \
   } \

   inline void convert(long long oldmem, long long oldmemed, Storeable**& pointer, long long d) {
       long long ptr_addr = (long long)pointer;
       if (oldmem<=ptr_addr && ptr_addr<oldmemed) {
           pointer = (Storeable**)(ptr_addr + d);
       }
       convert(oldPool, oldmem, *pointer, d); \
   }

   inline void convert(long long oldmem, long long oldmemed, Storeable*& pointer, long long d) {
       long long ptr_addr = (long long)pointer;
       xassert (oldmem<=ptr_addr && ptr_addr<oldmemed);

       pointer = (oldPool, Storeable*)(ptr_addr + d);
       pointer->__pool = this; \
   }

   template<class ST>
   inline void alloc0(ST*& pointer) {
      size_t size0 = memory.size();
      size_t size = size0+sizeof(ST);
      size_t bufsz = ((size+STORE_BUF_SZ-1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
      if (bufsz > memory.capacity()) {
          long long oldmem = (long long)&memory[0];
          memory.reserve(bufsz);
          memory[size-1];
          CPY(*this, *it = ptr, oldmem, oldmem+size0)
      }
      pointer = (ST*) &memory[size0];
      constcast(pointer->__pool) = this;
   }

public:

   StoragePool() : parent(0) {

   }

   StoragePool(StoragePool const & oldPool) : parent(oldPool) {
       size_t size = oldPool.memory.size();
       size_t bufsz = ((size+STORE_BUF_SZ-1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
       xassert(bufsz==oldPool.memory.capacity());

       memory = oldPool.memory;
       pointers.reserve(oldPool.pointers.capacity());
       long long oldmem = (long long)&oldPool.memory[0];
       CPY(oldPool, pointers.push_back(ptr), oldmem, oldmem+size)
   }

   bool isParentOf(StoragePool const & pool) {
       if (pool.parent == this) {
           return true;
       } else if (pool.parent) {
           return isParentOf(pool.parent);
       } else {
           return false;
       }
   }

   template<class ST>
   inline ST* alloc() {
      ST* pointer;
      alloc0(pointer);
   }

   template<class ST>
   inline void alloc(ST*& pointer) {
      alloc0(pointer);
      size_t pbufsz = ((pointers.size()+STORE_BUF_PTR_SZ-1)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
      pointers.reserve(pbufsz);
      pointers.push_back(&pointer);
   }

   template<class ST>
   inline void add(ST*& pointer) {
       assert(pointer->__pool);

       if (pointer->__pool == this) {
           size_t pbufsz = ((pointers.size()+STORE_BUF_PTR_SZ-1)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
           pointers.reserve(pbufsz);
           pointers.push_back((Storeable**)&pointer);
       } else {
           StoragePool const & oldPool = pointer->__pool;
           xassert(oldPool.isParentOf(*this));

           size_t size = oldPool.memory.size();
           long long oldmem = (long long)&oldPool.memory[0];
           long long d = ((long long)&memory[0])-oldmem;

           convert(oldmem, oldmem+size, pointer, d);
       }
   }

   template<class ST>
   inline void copy(ST*& pointer) {
       xassert (pointer->__pool);

       size_t pbufsz = ((pointers.size()+STORE_BUF_PTR_SZ-1)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
       pointers.reserve(pbufsz);
       pointers.push_back((Storeable**)&pointer);
   }

};


#endif // STORAGE_H

