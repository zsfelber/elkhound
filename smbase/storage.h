#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <memory.h>
#include <malloc.h>
#include <stdint.h>
#include <algorithm>
#include <iostream>

#include "xassert.h"

template <typename P>
P* NN(P* a) {
    if (a) {
        return a;
    } else {
        x_assert_fail("Non-nullable pointer is null.", __FILE__, __LINE__);
        return NULL;
    }
}

template <typename P> inline P* constcast(P const * p) {
    return const_cast<P*>(p);
}

template <typename P> inline P& constcast(P const & p) {
    return const_cast<P&>(p);
}

// 512
static const int STORE_BUF_PTR_BITS = 9;
static const int STORE_BUF_PTR_SZ = 1 << STORE_BUF_PTR_BITS;

// 16 Mbytes
static const int STORE_BUF_BITS = 24;
static const int STORE_BUF_SZ = 1 << STORE_BUF_BITS;

// 512 Kbytes  1 bit for each slot (32 bytes) : STORE_BUF_BITS-5
static const int STORE_BUF_VAR_SH = 5;
static const int STORE_BUF_ADDR_SZ = 1 << STORE_BUF_VAR_SH;
static const int STORE_BUF_VAR_BITS = STORE_BUF_BITS-STORE_BUF_VAR_SH;
static const int STORE_BUF_VAR_SZ = 1 << STORE_BUF_VAR_BITS;

// 64 Kbytes   1 bit for each VAR : VAR_BITS-3
static const int STORE_BUF_BIT_SH = 3;
static const int STORE_BUF_BIT_TOT_SH = STORE_BUF_VAR_SH + STORE_BUF_BIT_SH;
static const int STORE_BUF_BIT_BITS = STORE_BUF_VAR_BITS-STORE_BUF_BIT_SH;
static const int STORE_BUF_BIT_SZ = 1 << STORE_BUF_BIT_BITS;

class StoragePool;

#define STOREABLE_COPY_CON(classname) classname(classname const & src) : Storeable(src) {}
#define STOREABLE_COPY_CON(classname,baseclass) classname(classname const & src) : baseclass(src) {}



inline void copyBuffer(uint8_t* src, size_t srclen, size_t srccap,
                       uint8_t*& dest, size_t &dstlen, size_t &dstcap, size_t size_of) {
    xassert(srccap && !dest);
    dest = new uint8_t[srccap*size_of];
    if (!dest) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    dstlen = srclen;
    dstcap = srccap;
    memcpy(dest, src, srclen*size_of);
}

inline void copyBuffer(uint8_t* src, size_t srclen, uint8_t*& dest, size_t size_of) {
    xassert(!dest);
    dest = new uint8_t[srccap*size_of];
    memcpy(dest, src, srclen*size_of);
}

inline void extendBuffer(uint8_t*& buf, size_t size, size_t& capacity, size_t newcap, size_t size_of) {
    xassert(newcap);
    uint8_t* old = buf;
    buf = new uint8_t[newcap*size_of];
    if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    memcpy(buf, old, size*size_of);
    capacity = newcap;
    if (old) delete[] (uint8_t*)old;
}

inline void extendBuffer(uint8_t*& buf, size_t size, size_t newcap, size_t size_of) {
    xassert(newcap);
    uint8_t* old = buf;
    buf = new uint8_t[newcap*size_of];
    if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    memcpy(buf, old, size*size_of);
    if (old) delete[] (uint8_t*)old;
}

inline void remove(uint8_t* buf, size_t& size, uint8_t* pos, size_t size_of) {
    size--;
    // allow overlap
    memmove(pos, ((uint8_t*)pos)+size_of, size*size_of-(((uint8_t*)pos)-((uint8_t*)buf)));
}

inline void insert(uint8_t* buf, size_t& size, uint8_t* pos, size_t size_of) {
    // allow overlap
    memmove(((uint8_t*)pos)+size_of, pos, size*size_of-(((uint8_t*)pos)-((uint8_t*)buf)));
    size++;
}



class Storeable {
friend class StoragePool;

    typedef StoragePool* PtrToMe;
    typedef Storeable* DataPtr;
    typedef DataPtr* ExternalPtr;

    enum {
        ST_NONE,
        ST_PARENT,
        ST_CHILD,
        ST_DELETED
    } __Kind;

    uint8_t __kind;
    union {
        StoragePool * pool;
        Storeable const * parent;
    } __pp;
    size_t __sizeof_this;
    size_t __next;

    uint8_t* operator new (std::size_t size, StoragePool & pool);
    uint8_t* operator new[] (std::size_t size, StoragePool & pool);

    StoragePool * getPool() {
        switch (__kind) {
        case ST_PARENT:
            return __pp.pool;
            break;
        case ST_CHILD:
            return __pp.parent->getPool();
            break;
        default:
            x_assert_fail("Wrong kind.", __FILE__, __LINE__);
            break;
        }
    }

    Storeable * getRoot() {
        switch (__kind) {
        case ST_PARENT:
            return this;
            break;
        case ST_CHILD:
            return __pp.parent->getRoot();
            break;
        default:
            x_assert_fail("Wrong kind.", __FILE__, __LINE__);
            break;
        }
    }

    void regChild(Storeable *child) {
        if (__next) {
            child->__next = __next;
        }
        __next = ((uint8_t*)child) - ((uint8_t*)this);
    }

public:


   uint8_t* operator new (std::size_t size);
   uint8_t* operator new (std::size_t size, uint8_t* ptr);
   uint8_t* operator new (std::size_t size, const std::nothrow_t& nothrow_value);
   uint8_t* operator new[] (std::size_t size);
   uint8_t* operator new[] (std::size_t size, uint8_t* ptr);
   uint8_t* operator new[] (std::size_t size, const std::nothrow_t& nothrow_value);

   void operator delete (uint8_t* ptr);
   void operator delete (uint8_t* ptr, const std::nothrow_t& nothrow_constant);
   void operator delete (uint8_t* ptr, uint8_t* voidptr2);
   void operator delete (uint8_t* ptr, size_t size);
   void operator delete[] (uint8_t* ptr);
   void operator delete[] (uint8_t* ptr, const std::nothrow_t& nothrow_constant);
   void operator delete[] (uint8_t* ptr, uint8_t* voidptr2);
   void operator delete[] (uint8_t* ptr, size_t size);

protected:

   Storeable() : __kind(ST_NONE), __pp(NULL), __sizeof_this(0), __next(0), __variable(NULL) {

   }

   /* new operator filled __pool and __sizeof_this previously, we use passed argument to double check */
   Storeable(StoragePool & pool) : __kind(ST_PARENT) {
       xassert(__pp.pool == &pool);
       xassert(pool.contains(this));
   }

   /**
    * src: source object argument of copy constructor
    * default and very effective implementation
    * should be additionally invoked for each Storeable class field too
    * @brief Storeable::Storeable
    * @param parent
    */
   template<class ME>
   Storeable(ME const & src);

   /**
    * this object is a non-pointer class field of the pointer/non-pointer stored variable, parent
    * @brief Storeable::Storeable
    * @param parent
    */
   Storeable(Storeable const & parent, size_t size_of);
};






class StoragePool {
   friend class Storeable;

   typedef StoragePool* PtrToMe;
   typedef Storeable* DataPtr;
   typedef DataPtr* ExternalPtr;

public:

   class iterator {
       int index;
       uint8_t* memend;
       DataPtr variablePtr;

       iterator(StoragePool const & pool, DataPtr variablePtr) : pool(pool), index(0), variablePtr(variablePtr) {
           memend = pool.memory + pool.memlength;
           check();
       }

       void check() {
           if (*variablePtr > memend) {
               x_assert_fail("Overindexed StoragePool.", __FILE__, __LINE__);
           } else if (*variablePtr == memend) {
               index = -1;
           } else {
               index++;
           }
       }

     public:
       iterator& operator++() {
           if (index>=0) {
               size_t store_size = ((variablePtr->__sizeof_this + STORE_BUF_ADDR_SZ - 1)>>STORE_BUF_VAR_SH)<<STORE_BUF_VAR_SH;
               variablePtr = (DataPtr) (((uint8_t*)variablePtr) + store_size);
               check();
           } else {
               x_assert_fail("Overindexed Storageable iterator.", __FILE__, __LINE__);
           }
       }

       void operator==(int arg) {
           return index == arg;
       }

       Storeable& operator*() {
           return *variablePtr;
       }

       DataPtr operator->() {
           return *variablePtr;
       }
   };

   class child_iterator {
       int index;
       uint8_t* variableMemFirst;
       uint8_t* memend;
       DataPtr variablePtr;

       child_iterator(DataPtr variablePtr) : index(0), variablePtr(variablePtr) {
           size_t store_size = ((__sizeof_this + STORE_BUF_ADDR_SZ - 1)>>STORE_BUF_VAR_SH)<<STORE_BUF_VAR_SH;
           variableMemFirst = (uint8_t*)variablePtr;
           memend = variableMemFirst + store_size;
           check();
       }

       void check() {
           if (*variablePtr > memend) {
               x_assert_fail("Overindexed StoragePool.", __FILE__, __LINE__);
           } else if (variablePtr->__next) {
               index++;
           } else {
               index = -1;
           }
       }

     public:
       child_iterator& operator++() {
           if (index>=0) {
               variablePtr = (DataPtr) (variableMemFirst + variablePtr->__next);
               check();
           } else {
               x_assert_fail("Overindexed Storageable iterator.", __FILE__, __LINE__);
           }
       }

       void operator==(int arg) {
           return index == arg;
       }

       DataPtr operator*() {
           return *variablePtr;
       }

       DataPtr operator->() {
           return *variablePtr;
       }
   };

private:

   StoragePool const * const parent;

   uint8_t *memory;
   DataPtr first_del_var;
   size_t memlength, memcapacity;

   ExternalPtr* pointers;// point to external pointer to "memory"
   size_t ptrslength, ptrscapacity;

   inline void convertAll(uint8_t* oldmemFrom, uint8_t* oldmemTo) {
       std::ptrdiff_t d = memory - ((uint8_t*)oldmemFrom);

       for (iterator it(*this, (DataPtr)memory); it!=end; it++) {
           const_cast<PtrToMe&>(it->__pp.pool) = this;
           //for (child_iterator ch_it(*it); ch_it!=end; ch_it++) {
           //    Storeable& variable = *ch_it;
           //}
       }

       ExternalPtr* pointersFrom = pointers;
       ExternalPtr* pointersTo = pointers+ptrslength;
       for (; pointersFrom<pointersTo; pointersFrom++) {
           ExternalPtr& ptr = *pointersFrom;
           movePointer(oldmemFrom, oldmemTo, ptr, d);
           const_cast<PtrToMe&>((*ptr)->__pool) = this;
       }
   }

   inline void moveVariable(uint8_t* oldmemFrom, uint8_t* oldmemTo, DataPtr& variable, std::ptrdiff_t d) {
       uint8_t* variable_addr = variable;
       xassert (oldmemFrom<=variable_addr && variable_addr<oldmemTo);

       ((uint8_t*&)variable_addr) += d;
       variable = (DataPtr)variable_addr;

       xassert(contains(variable_addr));
   }

   // NOTE movePointer used from convertAll only
   inline void movePointer(uint8_t* oldmemFrom, uint8_t* oldmemTo, ExternalPtr& pointer, std::ptrdiff_t d) {
       uint8_t* ptr_addr = pointer;
       if (oldmemFrom<=ptr_addr && ptr_addr<oldmemTo) {
           ((uint8_t*&)ptr_addr) += d;
           pointer = (ExternalPtr)ptr_addr;
       }
       xassert(contains(*pointer));
   }

   inline void alloc0(uint8_t*& data, size_t size_of) {
      // TODO using holes first
      // TODO re-inserting residual holes (maybe join to next hole)

      size_t oldmemlength = memlength;
      size_t store_size = ((size_of + STORE_BUF_ADDR_SZ - 1)>>STORE_BUF_VAR_SH)<<STORE_BUF_VAR_SH;
      size_t newmemlength = oldmemlength + store_size;
      size_t bufsz = ((newmemlength + STORE_BUF_SZ - 1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
      uint8_t* oldmem = memory;
      if (memcapacity < bufsz) {
          extendBuffer(memory,           memlength,                       memcapacity, bufsz, 1/*sizeof(uint8_t)*/);
          if (oldmemlength) {
              convertAll(oldmem, oldmem+oldmemlength);
          }
      }
      memlength = newmemlength;
      data = memory+oldmemlength;

      const_cast<PtrToMe&>(((DataPtr)data)->__pp.pool) = this;
      const_cast<PtrToMe&>(((DataPtr)data)->__sizeof_this) = size_of;
   }

   template<class ST>
   inline ST* alloc() {
      ST* pointer;
      alloc0((uint8_t*&)pointer, sizeof(ST));// not ST* or ST**
      return pointer;
   }

   template<class ST>
   inline void alloc(ST*& externalUninitializedPointer) {
      alloc0((uint8_t*&)externalUninitializedPointer, sizeof(ST));// not ST* or ST**

      add(externalUninitializedPointer);
   }

   inline void del(DataPtr data) {
       const_cast<PtrToMe&>(((DataPtr)data)->__kind) = Storeable::ST_DELETED;
       if (data < first_del_var) {
           first_del_var = data;
       }
   }


public:

   StoragePool() : parent(0),
       memory(0), memlength(0), memcapacity(0),
       first_del_var(0),
       pointers(0), ptrslength(0), ptrscapacity(0) {

   }

   StoragePool(StoragePool const & oldPool) : parent(&oldPool),
       memory(0), memlength(0), memcapacity(0),
       first_del_var(0),
       pointers(0), ptrslength(0), ptrscapacity(0) {

       xassert(!oldPool.parent);
       xassert(oldPool.isParentOf(*this));

       size_t oldmemlength = oldPool.memlength;
       size_t bufsz = ((oldmemlength+STORE_BUF_SZ-1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
       xassert(bufsz==oldPool.memcapacity);

       copyBuffer(oldPool.memory, oldPool.memlength, oldPool.memcapacity, memory, memlength, memcapacity, 1/*sizeof(uint8_t)*/);
       copyBuffer(oldPool.pointers, oldPool.ptrslength, oldPool.ptrscapacity, (uint8_t*&)pointers, ptrslength, ptrscapacity, sizeof(ExternalPtr));

       uint8_t* oldmem = oldPool.memory;
       convertAll(oldmem, oldmem+oldmemlength);
   }

   ~StoragePool() {
       if (memory) delete[] memory;
       if (variables) delete[] variables;
       if (deleted_var_bits) delete[] deleted_var_bits;
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
       return (memory<=pointer && pointer<memory+memlength);
   }

   template<class ST>
   inline void add(ST*& externalPointerToConstructedData) {
       xassert(contains(externalPointerToConstructedData));

       size_t pbufsz = ((ptrslength+STORE_BUF_PTR_SZ/*+1-1*/)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
       if (ptrscapacity < pbufsz) {
           extendBuffer((uint8_t*&)pointers, ptrslength, ptrscapacity, pbufsz, sizeof(ExternalPtr));
       }
       pointers[ptrslength++] = (ExternalPtr)&externalPointerToConstructedData;

       const_cast<PtrToMe&>(externalPointerToConstructedData->__pool) = this;
   }

   template<class ST>
   inline void movePointerToChild(ST*& dataPointerConvertFromParentPool) {
       StoragePool const & oldPool = *dataPointerConvertFromParentPool->__pool;
       xassert(oldPool.isParentOf(*this));

       size_t size = oldPool.memlength;
       uint8_t* oldmem = oldPool.memory;
       std::ptrdiff_t d = memory - oldmem;

       moveVariable(oldmem, oldmem+size, (DataPtr&)dataPointerConvertFromParentPool, d);
       xassert(dataPointerConvertFromParentPool->__pool == this);
   }

   inline iterator begin(DataPtr variablePtr = 0) {
       if (!variablePtr) {
           variablePtr = (DataPtr) memory;
       }
       return iterator(*this, variablePtr);
   }

   inline child_iterator begin_children(DataPtr variablePtr) {
       return child_iterator(variablePtr);
   }

   inline int end() {
       return -1;
   }
};

template<class ME>
inline Storeable::Storeable(ME const & src) : __kind(src.__kind), __pp(src.__pp), __sizeof_this(sizeof(ME)), __next(0) {
    switch (__kind) {
    case ST_PARENT:
        xassert(__pp.pool->contains(this));
        break;
    case ST_CHILD:
        getRoot()->regChild(this);
        break;
    default:
        break;
    }
}

inline Storeable::Storeable(Storeable const & parent, size_t size_of) : __kind(ST_CHILD), __pp(&parent), __sizeof_this(size_of), __next(0) {
    switch (__kind) {
    case ST_CHILD:
        getRoot()->regChild(this);
        break;
    default:
        break;
    }
}

inline Storeable::~Storeable() {
    switch (__kind) {
    case ST_PARENT:
        __pp.pool->del(this);
        break;
    case ST_CHILD:
        getPool()->del(this);
        break;
    default:
        break;
    }
}

inline uint8_t* Storeable::operator new (std::size_t size, StoragePool& pool) {
    uint8_t* data;
    pool.alloc0(data, size);
    return data;
}

inline uint8_t* Storeable::operator new[] (std::size_t size, StoragePool& pool) {
    uint8_t* data;
    pool.alloc0(data, size);
    return data;
}

inline void Storeable::operator delete (uint8_t* ptr) {
    // Nothing to do here, everything is in ~Storeable
}

inline void Storeable::operator delete[] (uint8_t* ptr) {
    // Nothing to do here, everything is in ~Storeable
}

#endif // STORAGE_H

