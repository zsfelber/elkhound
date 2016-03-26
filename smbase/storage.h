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

// 2048
static const int STORE_BUF_HOL_BITS = 11;
static const int STORE_BUF_HOL_SZ = 1 << STORE_BUF_HOL_BITS;

// 256 Kbytes
static const int STORE_BUF_VAR_BITS = 18;
static const int STORE_BUF_VAR_SZ = 1 << STORE_BUF_VAR_BITS;

// 16 Mbytes
static const int STORE_BUF_BITS = 24;
static const int STORE_BUF_SZ = 1 << STORE_BUF_BITS;

static const uint32_t STORE_NEW_ID = 0x9a48fa11;

class StoragePool;

#define STOREABLE_COPY_CON(classname) classname(classname const & src) : Storeable(src) {}
#define STOREABLE_COPY_CON(classname,baseclass) classname(classname const & src) : baseclass(src) {}
#define STOREABLE_ALLOCATE(classname, pool, ...)    Construct(pool, new (pool) classname(__VA_ARGS__))


class Storeable {
friend class StoragePool;
    enum {
        ST_PARENT,
        ST_CHILD
    } __Kind;

    typedef StoragePool* PtrToMe;
    typedef Storeable* DataPtr;
    typedef DataPtr* ExternalPtr;

    int8_t __kind;
    union {
        StoragePool * pool;
        Storeable const * parent;
    } __pool_parent;
    size_t __sizeof_this;
    DataPtr* __children;

    void* operator new (std::size_t size, StoragePool & pool);
    void* operator new[] (std::size_t size, StoragePool & pool);

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

   Storeable() : __kind(0), __(NULL), __sizeof_this(0), __children(NULL) {

   }

   /* new operator filled __pool and __sizeof_this previously, we use passed argument to double check */
   Storeable(StoragePool & pool) : __kind(ST_PARENT) {
       xassert(__.pool == &pool);
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

private:
   typedef StoragePool* PtrToMe;
   typedef Storeable* DataPtr;
   typedef DataPtr* ExternalPtr;

   StoragePool const * const parent;
   void* memory;
   size_t memlength, memcapacity;
   DataPtr* variables;
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

       DataPtr* childrenFrom = children;
       DataPtr* childrenTo = children+chldlength;
       for (; childrenFrom<childrenTo; childrenFrom++) {
           DataPtr& child = *childrenFrom;
           moveVariable(oldmemFrom, oldmemTo, child, d);
           moveVariable(oldmemFrom, oldmemTo, constcast(child->__pool_parent), d);
           const_cast<PtrToMe&>(child->__pool) = this;
       }

       ExternalPtr* pointersFrom = pointers;
       ExternalPtr* pointersTo = pointers+ptrslength;
       for (; pointersFrom<pointersTo; pointersFrom++) {
           ExternalPtr& ptr = *pointersFrom;
           movePointer(oldmemFrom, oldmemTo, ptr, d);
           const_cast<PtrToMe&>((*ptr)->__pool) = this;
       }

       Variable* holesFrom = holes;
       Variable* holesTo = holes+holeslength;
       for (; holesFrom<holesTo; holesFrom++) {
           Variable& hole = *holesFrom;
           moveHole(oldmemFrom, oldmemTo, hole, d);
       }
   }

   inline void moveVariable(void* oldmemFrom, void* oldmemTo, DataPtr& variable, std::ptrdiff_t d) {
       void* variable_addr = variable;
       xassert (oldmemFrom<=variable_addr && variable_addr<oldmemTo);

       ((int8_t*&)variable_addr) += d;
       variable = (DataPtr)variable_addr;

       xassert(contains(variable_addr));
   }

   // NOTE movePointer used from convertAll only
   inline void movePointer(void* oldmemFrom, void* oldmemTo, ExternalPtr& pointer, std::ptrdiff_t d) {
       void* ptr_addr = pointer;
       if (oldmemFrom<=ptr_addr && ptr_addr<oldmemTo) {
           ((int8_t*&)ptr_addr) += d;
           pointer = (ExternalPtr)ptr_addr;
       }
       xassert(contains(*pointer));
   }

   inline void moveHole(void* oldmemFrom, void* oldmemTo, Variable& hole, std::ptrdiff_t d) {
       void* hole_addr = hole.mem;
       xassert (oldmemFrom<=hole_addr && hole_addr<oldmemTo);

       ((int8_t*&)hole_addr) += d;
       hole.mem = hole_addr;

       xassert(contains(hole_addr));

       void* hole_var = hole.var;
       ((int8_t*&)hole_var) += d;
       hole.var = (DataPtr*)hole_var;
   }

   inline void alloc0(void*& data, size_t size_of) {
      // TODO using holes first
      // TODO re-inserting residual holes (maybe join to next hole)

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

      reg((DataPtr)data);

      const_cast<PtrToMe&>(((DataPtr)data)->__pool) = this;
      const_cast<PtrToMe&>(((DataPtr)data)->__sizeof_this) = size_of;
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

   inline void reg0(DataPtr data, size_t length, size_t& capacity, DataPtr *&buf, bool chld) {
       // always sorted, ensure binary_search invariant
       if (length) {
           DataPtr lastvar = buf[length-1];
           xassert(data > lastvar);
           xassert((int8_t*)data < ((int8_t*)memory)+memlength);
       } else {
           xassert(chld ? data >= memory : data == memory);
       }

       size_t bufsz = ((length+STORE_BUF_VAR_SZ/*+1-1*/)>>STORE_BUF_VAR_BITS)<<STORE_BUF_VAR_BITS;
       if (capacity < bufsz) {
           extendBuffer((void*&)buf, length, capacity, bufsz, sizeof(DataPtr));
       }
       buf[length+1] = data;
   }

   inline void reg(DataPtr data) {
       if (data->__kind == Storeable::ST_PARENT) {
           reg0(data, varslength++, varscapacity, variables, false);
       } else {
           reg0(data, chldlength++, chldcapacity, children, true);
       }
   }

   inline void del(DataPtr data) {
       // TODO join to next hole

       std::cout << "delete " << (void*) data << " : " << data->__sizeof_this << "   field : " << (data->__parent?"yes":"no") << std::endl;
       xassert(contains(data));

       // binary_search
       DataPtr* ins_var = std::lower_bound(variables, variables+varslength, data);
       // should be in:
       xassert(*ins_var == data);

       // TODO not using until basic features debugged

       // may be either parent or field object

       Variable h;
       h.mem = data;
       h.var = ins_var;

       Variable* last_hole = variables+varslength;
       Variable* ins_hole = std::upper_bound(holes, last_hole, data);

       if (data->__parent) {
           remove(holes, holeslength, ins_hole, sizeof(Variable));
       } else {
           size_t hbufsz = ((holeslength+STORE_BUF_HOL_SZ/*+1-1*/)>>STORE_BUF_HOL_BITS)<<STORE_BUF_HOL_BITS;
           if (holescapacity < hbufsz) {
               extendBuffer((void*&)holes, holeslength, holescapacity, hbufsz, sizeof(Variable));
           }

           if (ins_hole != last_hole) {
               insert(holes, holeslength++, ins_hole, sizeof(Variable));
               *ins_hole = h;
           } else {
               holes[holeslength++] = h;
           }
       }
#if _0

#endif
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
       if (old) delete[] (int8_t*)old;
   }

   inline void remove(void* buf, size_t& size, void* pos, size_t size_of) {
       size--;
       memcpy(pos, ((int8_t*)pos)+size_of, size*size_of-(((int8_t*)pos)-((int8_t*)buf)));
   }

   inline void insert(void* buf, size_t& size, void* pos, size_t size_of) {
       // allow overlap
       memmove(((int8_t*)pos)+size_of, pos, size*size_of-(((int8_t*)pos)-((int8_t*)buf)));
       size++;
   }

public:
   struct Construct {
       StoragePool &pool;
       size_t const size_of;
       Construct(StoragePool &pool, size_t size_of) : pool(pool), size_of(size_of) {
       }
   };

   StoragePool() : parent(0),
       memory(0), memlength(0), memcapacity(0),
       variables(0), varslength(0), varscapacity(0),
       pointers(0), ptrslength(0), ptrscapacity(0) {

   }

   StoragePool(StoragePool const & oldPool) : parent(&oldPool),
       memory(0), memlength(0), memcapacity(0),
       variables(0), varslength(0), varscapacity(0),
       pointers(0), ptrslength(0), ptrscapacity(0) {

       xassert(!oldPool.parent);
       xassert(oldPool.isParentOf(*this));

       size_t oldmemlength = oldPool.memlength;
       size_t bufsz = ((oldmemlength+STORE_BUF_SZ-1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
       xassert(bufsz==oldPool.memcapacity);

       copyBuffer(oldPool.memory, oldPool.memlength, oldPool.memcapacity, (void*&)memory, memlength, memcapacity, 1/*sizeof(int8_t)*/);
       copyBuffer(oldPool.variables, oldPool.varslength, oldPool.varscapacity, (void*&)variables, varslength, varscapacity, sizeof(DataPtr));
       copyBuffer(oldPool.pointers, oldPool.ptrslength, oldPool.ptrscapacity, (void*&)pointers, ptrslength, ptrscapacity, sizeof(ExternalPtr));
       copyBuffer(oldPool.holes, oldPool.holeslength, oldPool.holescapacity, (void*&)holes, holeslength, holescapacity, sizeof(Variable));

       void* oldmem = oldPool.memory;
       convertAll(oldmem, ((int8_t*)oldmem)+oldmemlength);
   }

   ~StoragePool() {
       if (memory) delete[] (int8_t*)memory;
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

template<class ME>
inline Storeable::Storeable(ME const & src) : __kind(src.__kind), __(&src), __sizeof_this(sizeof(ME)){
    if (__pool) {
        __pool->reg(this);
    }
}

inline Storeable::Storeable(Storeable const & parent, size_t size_of) : __kind(), __pool(parent.__pool), __parent(parent), __sizeof_this(size_of){
    if (__pool) {
        __pool->reg(this);
    }
}

inline Storeable::~Storeable() {
    if (__pool) {
        __pool->del(this);
    }
}

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

inline void Storeable::operator delete (void* ptr) {
    // Nothing to do here, everything is in ~Storeable
}
/*inline void Storeable::operator delete (void* ptr, const std::nothrow_t& nothrow_constant) {
    Storeable *itm = (Storeable*)ptr;
    xassert (itm->__pool);
    itm->__pool->del(itm);
}
inline void Storeable::operator delete (void* ptr, void* voidptr2) {
    x_assert_fail("Don't use this delete op for Storeable object.", __FILE__, __LINE__);
}*/


inline void Storeable::operator delete[] (void* ptr) {
    // Nothing to do here, everything is in ~Storeable
}

/*inline void Storeable::operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant) {
    Storeable *itm = (Storeable*)ptr;
    xassert (itm->__pool);
    itm->__pool->del(itm);
}
inline void Storeable::operator delete[] (void* ptr, void* voidptr2) {
    x_assert_fail("Don't use this delete op for Storeable object.", __FILE__, __LINE__);
}*/

#endif // STORAGE_H

