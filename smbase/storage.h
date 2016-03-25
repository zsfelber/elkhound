#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <memory.h>
#include <malloc.h>
#include <stdint.h>

template <typename P> inline P* constcast(P const * p) {
    return const_cast<P*>(p);
}

template <typename P> inline P& constcast(P const & p) {
    return const_cast<P&>(p);
}

// 512
static const int STORE_BUF_PTR_BITS = 9;
static const int STORE_BUF_PTR_SZ = 1 << STORE_BUF_PTR_BITS;

// 512 Kbytes
static const int STORE_BUF_VAR_BITS = 19;
static const int STORE_BUF_VAR_SZ = 1 << STORE_BUF_VAR_BITS;

// 16 Mbytes
static const int STORE_BUF_BITS = 24;
static const int STORE_BUF_SZ = 1 << STORE_BUF_BITS;

class StoragePool;

class Storeable {
friend class StoragePool;

public:
   // left uninitialized (so default constructor doesn't clean it up before operator new)
   StoragePool const * const __pool;

   void* operator new (std::size_t size);
   void* operator new (std::size_t size, void* ptr);
   void* operator new (std::size_t size, const std::nothrow_t& nothrow_value);
   void* operator new[] (std::size_t size);
   void* operator new[] (std::size_t size, void* ptr);
   void* operator new[] (std::size_t size, const std::nothrow_t& nothrow_value);

   void* operator new (std::size_t size, StoragePool& pool);
   void* operator new[] (std::size_t size, StoragePool& pool);

   void operator delete (void* ptr);
   void operator delete (void* ptr, const std::nothrow_t& nothrow_constant);
   void operator delete (void* ptr, void* voidptr2);
   void operator delete[] (void* ptr);
   void operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant);
   void operator delete[] (void* ptr, void* voidptr2);

protected:
   // fake init just to remove compilation error (using constcast and assigning previously or later in Pool)
   Storeable() : __pool(__pool) {}

};

class StoragePool {
   friend class Storeable;

private:
   typedef StoragePool* PtrToMe;
   typedef Storeable* DataPtr;
   typedef DataPtr* ExternalPtr;

   StoragePool const * const parent;
   void* memory;
   size_t memlength, memcapacity;
   DataPtr* variables;// point to "memory"
   size_t varslength, varscapacity;
   ExternalPtr* pointers;// point to external pointer to "memory"
   size_t ptrslength, ptrscapacity;

   inline void convertAll(void* oldmemFrom, void* oldmemTo) {
       std::ptrdiff_t d = ((int8_t*)memory) - ((int8_t*)oldmemFrom);

       DataPtr* variablesFrom = variables;
       DataPtr* variablesTo = variables+varslength;
       for (; variablesFrom<variablesTo; variablesFrom++) {
           DataPtr& variable = *variablesFrom;
           moveVariable(oldmemFrom, oldmemTo, variable, d);
           const_cast<PtrToMe&>(variable->__pool) = this;
       }

       ExternalPtr* pointersFrom = pointers;
       ExternalPtr* pointersTo = pointers+ptrslength;
       for (; pointersFrom<pointersTo; pointersFrom++) {
           ExternalPtr& ptr = *pointersFrom;
           movePointer(oldmemFrom, oldmemTo, ptr, d);
           const_cast<PtrToMe&>((*ptr)->__pool) = this;
       }
   }

   inline void moveVariable(void* oldmemFrom, void* oldmemTo, DataPtr& variable, std::ptrdiff_t d) {
       void* variable_addr = variable;
       xassert (oldmemFrom<=variable_addr && variable_addr<oldmemTo);

       ((int8_t*&)variable_addr) += d;
       variable = (DataPtr)variable_addr;

       xassert(contains(variable));
   }

   // NOTE movePointer used from convertAll only
   inline void movePointer(void* oldmemFrom, void* oldmemTo, ExternalPtr& pointer, std::ptrdiff_t d) {
       void* ptr_addr = pointer;
       if (oldmemFrom<=ptr_addr && ptr_addr<oldmemTo) {
           void* data_addr = *pointer;
           xassert (oldmemFrom<=data_addr && data_addr<oldmemTo);

           ((int8_t*&)ptr_addr) += d;
           pointer = (ExternalPtr)ptr_addr;

           xassert(contains(*pointer));
       }
   }

   inline void alloc0(void*& data, size_t size_of) {
      size_t oldmemlength = memlength;
      size_t newmemlength = oldmemlength+size_of;
      size_t bufsz = ((newmemlength+STORE_BUF_SZ-1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
      void* oldmem = memory;
      if (memcapacity < bufsz) {
          extendBuffer((void*&)memory, memlength, memcapacity, bufsz, 1/*sizeof(int8_t)*/);
          if (oldmemlength) {
              convertAll(oldmem, ((int8_t*)oldmem)+oldmemlength);
          }
      }
      memlength = newmemlength;
      data = ((int8_t*)memory)+oldmemlength;

      xassert(contains(data));

      size_t vbufsz = ((varslength+STORE_BUF_VAR_SZ/*+1-1*/)>>STORE_BUF_VAR_BITS)<<STORE_BUF_VAR_BITS;
      if (varscapacity < vbufsz) {
          extendBuffer((void*&)variables, varslength, varscapacity, vbufsz, sizeof(DataPtr));
      }
      variables[varslength++] = (DataPtr)data;

      const_cast<PtrToMe&>(((DataPtr)data)->__pool) = this;
   }

   template<class ST>
   inline ST* alloc() {
      ST* pointer;
      alloc0((void*&)pointer, sizeof(ST));// not ST* or ST**
      return pointer;
   }

   template<class ST>
   inline void alloc(ST*& externalUninitializedPointer) {
      alloc0((void*&)externalUninitializedPointer, sizeof(ST));// not ST* or ST**

      add(externalUninitializedPointer);
   }

   inline void copyBuffer(void* src, size_t srclen, size_t srccap,
                          void*& dest, size_t &dstlen, size_t &dstcap, size_t size_of) {
       xassert(srccap);
       dest = new int8_t[srccap*size_of];
       if (!dest) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
       dstlen = srclen;
       dstcap = srccap;
       memcpy(dest, src, srclen*size_of);
   }

   inline void extendBuffer(void*& buf, size_t size, size_t& capacity, size_t newcap, size_t size_of) {
       xassert(newcap);
       void* old = buf;
       buf = new int8_t[newcap*size_of];
       if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
       memcpy(buf, old, size*size_of);
       capacity = newcap;
       if (old) delete[] old;
   }

public:

   StoragePool() : parent(0),
       memory(0), memlength(0), memcapacity(0),
       variables(0), varslength(0), varscapacity(0),
       pointers(0), ptrslength(0), ptrscapacity(0) {

   }

   StoragePool(StoragePool const & oldPool) : parent(&oldPool),
       memory(0), memlength(0), memcapacity(0),
       variables(0), varslength(0), varscapacity(0),
       pointers(0), ptrslength(0), ptrscapacity(0) {

       size_t oldmemlength = oldPool.memlength;
       size_t bufsz = ((oldmemlength+STORE_BUF_SZ-1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
       xassert(bufsz==oldPool.memcapacity);

       copyBuffer(oldPool.memory, oldPool.memlength, oldPool.memcapacity, (void*&)memory, memlength, memcapacity, 1/*sizeof(int8_t)*/);
       copyBuffer(oldPool.variables, oldPool.varslength, oldPool.varscapacity, (void*&)variables, varslength, varscapacity, sizeof(DataPtr));
       copyBuffer(oldPool.pointers, oldPool.ptrslength, oldPool.ptrscapacity, (void*&)pointers, ptrslength, ptrscapacity, sizeof(ExternalPtr));

       void* oldmem = oldPool.memory;
       convertAll(oldmem, ((int8_t*)oldmem)+oldmemlength);
   }

   ~StoragePool() {
       if (memory) delete[] memory;
       if (variables) delete[] variables;
       if (pointers) delete[] pointers;
   }

   inline bool isParentOf(StoragePool const & pool) const {
       if (pool.parent == this) {
           return true;
       } else if (pool.parent) {
           return isParentOf(*pool.parent);
       } else {
           return false;
       }
   }

   inline bool contains(void* pointer) const {
       return (memory<=pointer && pointer<((int8_t*)memory)+memlength);
   }

   template<class ST>
   inline void add(ST*& externalPointerToConstructedData) {
       xassert(contains(externalPointerToConstructedData));

       size_t pbufsz = ((ptrslength+STORE_BUF_PTR_SZ/*+1-1*/)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
       if (ptrscapacity < pbufsz) {
           extendBuffer((void*&)pointers, ptrslength, ptrscapacity, pbufsz, sizeof(ExternalPtr));
       }
       pointers[ptrslength++] = (ExternalPtr)&externalPointerToConstructedData;

       const_cast<PtrToMe&>(externalPointerToConstructedData->__pool) = this;
   }

   template<class ST>
   inline void movePointerToChild(ST*& dataPointerConvertFromParentPool) {
       StoragePool const & oldPool = *dataPointerConvertFromParentPool->__pool;
       xassert(oldPool.isParentOf(*this));

       size_t size = oldPool.memlength;
       void* oldmem = oldPool.memory;
       std::ptrdiff_t d = ((int8_t*)memory) - ((int8_t*)oldmem);

       moveVariable(oldmem, ((int8_t*)oldmem)+size, (DataPtr&)dataPointerConvertFromParentPool, d);
       xassert(dataPointerConvertFromParentPool->__pool == this);
   }

};

/* undefined
inline void* Storeable::operator new (std::size_t size) {
    throw std::bad_alloc();
}
inline void* Storeable::operator new (std::size_t size, const std::nothrow_t& nothrow_value) {
    return 0;
}
inline void* Storeable::operator new (std::size_t size, void* ptr) {
    throw std::bad_alloc();
}
inline void* Storeable::operator new[] (std::size_t size) {
    throw std::bad_alloc();
}
inline void* Storeable::operator new[] (std::size_t size, const std::nothrow_t& nothrow_value) {
    return 0;
}
inline void* Storeable::operator new[] (std::size_t size, void* ptr) {
    throw std::bad_alloc();
} */

inline void* Storeable::operator new (std::size_t size, StoragePool& pool) {
    void* data;
    pool.alloc0(data, size);
    return data;
}

inline void* Storeable::operator new[] (std::size_t size, StoragePool& pool) {
    void* data;
    pool.alloc0(data, size);
    return data;
}

inline void Storeable::operator delete (void* ptr) {
    x_assert_fail("Don't use delete for Storeable object.", __FILE__, __LINE__);
}
inline void Storeable::operator delete (void* ptr, const std::nothrow_t& nothrow_constant) {
    x_assert_fail("Don't use delete for Storeable object.", __FILE__, __LINE__);
}
inline void Storeable::operator delete (void* ptr, void* voidptr2) {
    x_assert_fail("Don't use delete for Storeable object.", __FILE__, __LINE__);
}

inline void Storeable::operator delete[] (void* ptr) {
    x_assert_fail("Don't use delete for Storeable object.", __FILE__, __LINE__);
}
inline void Storeable::operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant) {
    x_assert_fail("Don't use delete for Storeable object.", __FILE__, __LINE__);
}
inline void Storeable::operator delete[] (void* ptr, void* voidptr2) {
    x_assert_fail("Don't use delete for Storeable object.", __FILE__, __LINE__);
}

#endif // STORAGE_H

