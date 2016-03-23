#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <memory.h>

template <typename P> inline P* constcast(P const * p) {
    return const_cast<P*>(p);
}

template <typename P> inline P& constcast(P const & p) {
    return const_cast<P&>(p);
}

// 128
static const int STORE_BUF_PTR_BITS = 7;
static const int STORE_BUF_PTR_SZ = 1 << STORE_BUF_PTR_BITS;

// 16 Mbytes
static const int STORE_BUF_BITS = 24;
static const int STORE_BUF_SZ = 1 << STORE_BUF_BITS;

class StoragePool;

class Storeable {

public:
   StoragePool const * const __pool;

protected:
   Storeable() : __pool(0) {}
};

class StoragePool {
private:

   StoragePool const * const parent;
   void* memory;
   size_t memlength, memcapacity;
   Storeable*** pointers;
   size_t pointerslength, pointerscapacity;

   inline void convertAll(Storeable*** oldPoolFrom, Storeable*** oldPoolTo, void* oldmem, void* oldmemed) {
       long long d = ((long long)memory)-oldmem;
       for (; oldPoolFrom<oldPoolTo; oldPoolFrom++) {
           Storeable**& ptr = *oldPoolFrom;
           convert(oldmem, oldmemed, ptr, d);
           const_cast<StoragePool*&>((*ptr)->__pool) = this;
       }
   }

   template<class ST>
   inline void convert(void* oldmem, void* oldmemed, ST**& pointer, long long d) {
       long long ptr_addr = (long long)pointer;
       if (((long long)oldmem)<=ptr_addr && ptr_addr<(long long)oldmemed) {
           pointer = (ST**)(ptr_addr + d);
       }
       convert(oldmem, oldmemed, *pointer, d); \
   }

   template<class ST>
   inline void convert(void* oldmem, void* oldmemed, ST*& pointer, long long d) {
       long long ptr_addr = (long long)pointer;
       xassert (((long long)oldmem)<=ptr_addr && ptr_addr<(long long)oldmemed);

       pointer = (ST*)(ptr_addr + d);
   }

   template<class ST>
   inline void alloc0(ST*& pointer) {
      size_t size0 = memlength;
      size_t size = size0+sizeof(void*);
      size_t bufsz = ((size+STORE_BUF_SZ-1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
      long long oldmem = (long long)memory;
      if (memcapacity < bufsz) {
          extendBuffer(memory, memlength, memcapacity, bufsz, 1);
          if (size0) {
              convertAll(pointers, pointers+pointerslength, oldmem, oldmem+size0);
          }
      }
      pointer = (ST*) &memory[size0];
      const_cast<StoragePool*&>(pointer->__pool) = this;
   }

   inline void copyBuffer(void* src, size_t srclen, size_t srccap,
                          void*& dest, size_t &dstlen, size_t &dstcap, size_t size_of) {
       dest = new void[srccap];
       dstlen = srclen;
       dstcap = srccap;
       memcpy(dest, src, srclen*size_of);
   }

   inline void extendBuffer(void* buf, size_t size, size_t& capacity, size_t newcap, size_t size_of) {
       void* old = buf;
       buf = new void[newcap];
       memcpy(buf, old, size*size_of);
       capacity = newcap;
       delete old;
   }

public:

   StoragePool() : parent(0), memory(0), memlength(0), memcapacity(0), pointers(0), pointerslength(0), pointerscapacity(0) {

   }

   StoragePool(StoragePool const & oldPool) : parent(&oldPool), memory(0), memlength(0), memcapacity(0), pointers(0), pointerslength(0), pointerscapacity(0) {
       size_t size = oldPool.memlength;
       size_t bufsz = ((size+STORE_BUF_SZ-1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
       xassert(bufsz==oldPool.memcapacity);

       copyBuffer(oldPool.memory, oldPool.memlength, oldPool.memcapacity, memory, memlength, memcapacity, 1);
       copyBuffer(oldPool.pointers, oldPool.pointerslength, oldPool.pointerscapacity, pointers, pointerslength, pointerscapacity, sizeof(void*));

       long long oldmem = (long long)oldPool.memory;
       convertAll(oldPool.pointers, oldPool.pointers+oldPool.pointerslength, oldmem, oldmem+size);
   }

   ~StoragePool() {
       if (memory) delete[] memory;
       if (pointers) delete[] pointers;
   }

   bool isParentOf(StoragePool const & pool) const {
       if (pool.parent == this) {
           return true;
       } else if (pool.parent) {
           return isParentOf(*pool.parent);
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
      size_t pbufsz = ((pointerslength+STORE_BUF_PTR_SZ-1)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
      if (pointerscapacity < pbufsz) {
          extendBuffer(pointers, pointerslength, pointerscapacity, pbufsz, sizeof(void*));
      }
      pointers[pointerslength++] = (Storeable**)&pointer;
   }

   template<class ST>
   inline void add(ST*& pointer) {
       xassert(pointer->__pool == this);

       size_t pbufsz = ((pointerslength+STORE_BUF_PTR_SZ-1)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
       if (pointerscapacity < pbufsz) {
           extendBuffer(pointers, pointerslength, pointerscapacity, pbufsz, sizeof(void*));
       }
       pointers[pointerslength++] = (Storeable**)&pointer;
   }

   template<class ST>
   inline void copyFrom(ST*& pointer) {
       xassert(pointer->__pool);
       StoragePool const & oldPool = *pointer->__pool;
       xassert(oldPool.isParentOf(*this));

       size_t size = oldPool.memlength;
       long long oldmem = (long long)oldPool.memory;
       long long d = ((long long)memory)-oldmem;

       convert(oldmem, oldmem+size, pointer, d);
       xassert(pointer->__pool == this);
   }

};


#endif // STORAGE_H

