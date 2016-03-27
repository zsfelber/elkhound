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

#define STOREABLE_COPY_CON(classname) classname(classname const & src) : Storeable(src) {}
#define STOREABLE_COPY_CON(classname,baseclass) classname(classname const & src) : baseclass(src) {}
//#define REG_CHILD

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

inline size_t getStoreSize(size_t size_of) {
    return ((size_of + STORE_BUF_ADDR_SZ - 1)>>STORE_BUF_VAR_SH)<<STORE_BUF_VAR_SH;
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
    size_t __store_size;
#ifdef REG_CHILD
    size_t __next;

    void regChild(Storeable *child) {
        if (__next) {
            child->__next = __next;
        }
        __next = ((uint8_t*)child) - ((uint8_t*)this);
    }
#endif

    void* operator new (std::size_t size, StoragePool & pool);
    void* operator new[] (std::size_t size, StoragePool & pool);

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

public:


   void* operator new (std::size_t size);
   void* operator new (std::size_t size, void* ptr);
   void* operator new (std::size_t size, const std::nothrow_t& nothrow_value);
   void* operator new[] (std::size_t size);
   void* operator new[] (std::size_t size, void* ptr);
   void* operator new[] (std::size_t size, const std::nothrow_t& nothrow_value);

   void operator delete (void* ptr);
   void operator delete (void* ptr, const std::nothrow_t& nothrow_constant);
   void operator delete (void* ptr, void* voidptr2);
   void operator delete (void* ptr, size_t size);
   void operator delete[] (void* ptr);
   void operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant);
   void operator delete[] (void* ptr, void* voidptr2);
   void operator delete[] (void* ptr, size_t size);

protected:

   Storeable() : __kind(ST_NONE), __pp(NULL), __store_size(0),
    #ifdef REG_CHILD
       __next(0),
    #endif
       __variable(NULL) {

   }

   /* new operator filled __pool and __store_size previously, we use passed argument to double check */
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
               variablePtr = (DataPtr) (((uint8_t*)variablePtr) + variablePtr->__store_size);
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

#ifdef REG_CHILD
   class child_iterator {
       int index;
       uint8_t* variableMemFirst;
       uint8_t* memend;
       DataPtr variablePtr;

       child_iterator(DataPtr variablePtr) : index(0), variablePtr(variablePtr) {
           variableMemFirst = (uint8_t*)variablePtr;
           memend = variableMemFirst + __store_size;
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

       Storeable& operator*() {
           return *variablePtr;
       }

       DataPtr operator->() {
           return *variablePtr;
       }
   };
#endif

private:

   StoragePool const * const parent;

   uint8_t *memory;
   DataPtr first_del_var;
   size_t deleted_vars;
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

   inline void allocParent(void*& data, size_t store_size) {

      // if hollow >25% :
      if (deleted_vars > memlength>>2) {
          DataPtr first = NULL;
          size_t continous = 0;
          for (iterator it(*this, first_del_var); it != -1; it++) {
              Storeable &cur = *it;
              if (cur.__kind == Storeable::ST_DELETED) {
                  if (!first) first = it.variablePtr;
                  continous += cur.__store_size;
                  if (continous >= store_size) {
                      xassert(first >= first_del_var);

                      data = first;
                      deleted_vars -= continous;

                      if (first == first_del_var) {
                          first_del_var = NULL;
                          if (deleted_vars) {
                              for (it++; it != -1; it++) {
                                  if (it->__kind == Storeable::ST_DELETED) {
                                      first_del_var = *it;
                                      break;
                                  }
                              }
                          }
                      }
                      goto ok;
                  }
              } else {
                  first = NULL;
                  continous = 0;
              }
          }
      }

      size_t oldmemlength = memlength;
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

      ok:
      const_cast<PtrToMe&>(((DataPtr)data)->__pp.pool) = this;
      const_cast<PtrToMe&>(((DataPtr)data)->__store_size) = store_size;
   }

   inline void delParent(DataPtr data) {
       const_cast<PtrToMe&>(((DataPtr)data)->__kind) = Storeable::ST_DELETED;
       if (data < first_del_var) {
           first_del_var = data;
           deleted_vars += data->__store_size;
       }
   }


public:

   StoragePool() : parent(0),
       memory(0), memlength(0), memcapacity(0),
       first_del_var(0), deleted_vars(0),
       pointers(0), ptrslength(0), ptrscapacity(0) {

   }

   StoragePool(StoragePool const & oldPool) : parent(&oldPool),
       memory(0), memlength(0), memcapacity(0),
       first_del_var(0), deleted_vars(0),
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

#ifdef REG_CHILD
   inline child_iterator begin_children(DataPtr variablePtr) {
       return child_iterator(variablePtr);
   }
#endif

   inline int end() {
       return -1;
   }
};

template<class ME>
inline Storeable::Storeable(ME const & src) : __kind(src.__kind), __pp(src.__pp), __store_size(getStoreSize(sizeof(ME)))
  #ifdef REG_CHILD
  , __next(0)
  #endif
{
    switch (__kind) {
    case ST_PARENT:
        xassert(__pp.pool->contains(this));
        break;
#ifdef REG_CHILD
    case ST_CHILD:
        getRoot()->regChild(this);
        break;
#endif
    default:
        break;
    }
}

inline Storeable::Storeable(Storeable const & parent, size_t size_of) : __kind(ST_CHILD), __pp(&parent), __store_size(getStoreSize(size_of))
#ifdef REG_CHILD
  , __next(0) {
    switch (__kind) {
    case ST_CHILD:
        getRoot()->regChild(this);
        break;
    default:
        break;
    }
#else
{
#endif
}

inline Storeable::~Storeable() {
    switch (__kind) {
    case ST_PARENT:
        __pp.pool->delParent(this);
        break;
    default:
        break;
    }
}

inline void* Storeable::operator new (std::size_t size, StoragePool& pool) {
    void* data;
    pool.allocParent(data, getStoreSize(size));
    return data;
}

inline void* Storeable::operator new[] (std::size_t size, StoragePool& pool) {
    void* data;
    pool.allocParent(data, getStoreSize(size));
    return data;
}

inline void Storeable::operator delete (void* ptr) {
    // Nothing to do here, everything is in ~Storeable
}

inline void Storeable::operator delete[] (void* ptr) {
    // Nothing to do here, everything is in ~Storeable
}

#endif // STORAGE_H

