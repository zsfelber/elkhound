#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <memory.h>
#include <malloc.h>
#include <stdint.h>
#include <algorithm>
#include <iostream>

#include "xassert.h"



/****************************************************************************************************
 * LIBRARY STUFF
 *
 * ..................................................................................................
 */


template <typename P>
P& NN(P* a) {
    if (!a) {
        x_assert_fail("Non-nullable pointer is null.", __FILE__, __LINE__);
    }
    return *a;
}

template <typename P> inline P* constcast(P const * p) {
    return const_cast<P*>(p);
}

template <typename P> inline P& constcast(P const & p) {
    return const_cast<P&>(p);
}

#define STOREABLE_COPY_CON(classname) classname(classname const & src) : Storeable(src) {}
#define STOREABLE_COPY_CON2(classname,baseclass) classname(classname const & src) : baseclass(src) {}

//STORE_NEW_REF(pool, SObjList<char const>, list);
// ->
//SObjList<char const> &list = *new (pool) SObjList<char const>(pool);
#define STORE_NEW_REF0(pool, classname, var) classname & var = * new (pool) classname(pool)
#define STORE_NEW_REF(pool, classname, var, ...) classname & var = * new (pool) classname(pool, __VA_ARGS__)

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

inline void copyBuffer(uint8_t* src, size_t srclen, size_t srccap, uint8_t*& dest, size_t size_of) {
    xassert(!dest);
    dest = new uint8_t[srccap*size_of];
    memcpy(dest, src, srclen*size_of);
}

inline void extendBuffer(uint8_t*& buf, size_t size, size_t& capacity, size_t newcap, size_t size_of) {
    xassert(newcap);
    uint8_t* old = buf;
    buf = new uint8_t[newcap*size_of];
    if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    if (old) {
        memcpy(buf, old, size*size_of);
        delete[] old;
    }
    capacity = newcap;
}

inline void extendBuffer(uint8_t*& buf, size_t size, size_t newcap, size_t size_of) {
    xassert(newcap);
    uint8_t* old = buf;
    buf = new uint8_t[newcap*size_of];
    if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    if (old) {
        memcpy(buf, old, size*size_of);
        delete[] old;
    }
}

inline void removeBufferItem(uint8_t* buf, size_t& size, uint8_t* pos, size_t size_of) {
    size--;
    // allow overlap
    memmove(pos, pos+size_of, size*size_of-(pos-buf));
}

inline void insertBufferItem(uint8_t* buf, size_t& size, uint8_t* pos, size_t size_of) {
    // allow overlap
    memmove(pos+size_of, pos, size*size_of-(pos-buf));
    size++;
}

inline size_t getStoreSize(size_t size_of) {
    return ((size_of + STORE_BUF_ADDR_SZ - 1)>>STORE_BUF_VAR_SH)<<STORE_BUF_VAR_SH;
}

inline size_t getMemBufSize(size_t memlength) {
    return ((memlength + STORE_BUF_SZ - 1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
}

inline size_t getPtrBufSize(size_t ptrslength) {
    return ((ptrslength + STORE_BUF_PTR_SZ/*+1-1*/)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
}


inline size_t encodeDeltaPtr(uint8_t* origin, uint8_t* address) {
    if (!address || !origin) {
        return std::string::npos;
    } else {
        xassert(origin < address);
        return (size_t)(address - origin);
    }
}

inline std::ptrdiff_t encodeSignedDeltaPtr(uint8_t* origin, uint8_t* address) {
    if (!address || !origin) {
        return 0;
    } else {
        return address - origin;
    }
}

inline uint8_t* decodeDeltaPtr(uint8_t* origin, size_t delta) {
    return delta == std::string::npos ? NULL : origin + delta;
}

inline uint8_t* decodeDeltaBackPtr(uint8_t* origin, size_t delta) {
    return delta == std::string::npos ? NULL : origin - delta;
}

inline uint8_t* decodeSignedDeltaPtr(uint8_t* origin, std::ptrdiff_t delta) {
    return delta == 0 ? NULL : origin + delta;
}



/****************************************************************************************************
 * STOREABLE
 *
 * ..................................................................................................
 */


class Storeable {
friend class StoragePool;
friend class VoidList;
friend class VoidTailList;
friend class VoidNode;

    enum {
        ST_NONE = 0,
        ST_PARENT,
        ST_CHILD,
        ST_DELETED
    } __Kind;

    uint8_t __kind;
    size_t __parentVector;
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

    inline void clear(size_t size_of) {
        memset(this, 0, size_of);
    }


    void init(Storeable const & srcOrParent, size_t size_of, bool childOfParent);

public:

   typedef StoragePool* PtrToMe;
   typedef Storeable* DataPtr;
   typedef DataPtr* ExternalPtr;


   void* operator new (std::size_t size);
   void* operator new (std::size_t size, void* ptr);
   void* operator new (std::size_t size, const std::nothrow_t& nothrow_value);
   void* operator new[] (std::size_t size);
   void* operator new[] (std::size_t size, void* ptr);
   void* operator new[] (std::size_t size, const std::nothrow_t& nothrow_value);

   void* operator new (std::size_t size, StoragePool & pool);
   void* operator new[] (std::size_t size, StoragePool & pool);

   void operator delete (void* ptr);
   void operator delete (void* ptr, const std::nothrow_t& nothrow_constant);
   void operator delete (void* ptr, void* voidptr2);
   void operator delete (void* ptr, size_t size);
   void operator delete[] (void* ptr);
   void operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant);
   void operator delete[] (void* ptr, void* voidptr2);
   void operator delete[] (void* ptr, size_t size);


   Storeable();

   /* new operator filled __pool and __store_size previously, we use passed argument to double check */
   Storeable(StoragePool & pool);

   /**
    * src: source object argument of copy constructor
    * default and very effective implementation
    * should be additionally invoked for each Storeable class field too
    * @brief Storeable::Storeable
    * @param parent
    */
   template<class ME>
   Storeable(ME const & srcOrParent, bool childOfParent);

   /**
    * this object is a non-pointer class field of the pointer/non-pointer stored variable, parent
    * @brief Storeable::Storeable
    * @param parent
    */
   Storeable(Storeable const & srcOrParent, size_t size_of, bool childOfParent);

   virtual ~Storeable();

   void assign(Storeable const & srcOrParent, size_t size_of);

   inline StoragePool & getPoolRef() const {
       return NN(getPool());
   }

   inline StoragePool * getPool() const {
       switch (__kind) {
       case ST_NONE:
           return NULL;
       case ST_PARENT:
       {
           StoragePool** the_first = (StoragePool**) decodeDeltaBackPtr((uint8_t*)this, __parentVector);
           xassert(the_first);
           return *the_first;
       }
       case ST_CHILD:
       {
           Storeable * st = getParent();
           xassert(st);
           return st->getPool();
       }
       default:
           x_assert_fail("Wrong kind.", __FILE__, __LINE__);
           break;
       }
   }

   inline Storeable * getParent() const {
       switch (__kind) {
       case ST_NONE:
       case ST_PARENT:
           return NULL;
       case ST_CHILD:
           return (Storeable*) decodeDeltaBackPtr((uint8_t*)this, __parentVector);
       default:
           x_assert_fail("Wrong kind.", __FILE__, __LINE__);
           break;
       }
   }

   inline Storeable const * getRoot() const {
       switch (__kind) {
       case ST_NONE:
           return NULL;
       case ST_PARENT:
           return this;
       case ST_CHILD:
       {
           Storeable * st = getParent();
           xassert(st);
           return st->getRoot();
       }
       default:
           x_assert_fail("Wrong kind.", __FILE__, __LINE__);
           break;
       }
   }
};





/****************************************************************************************************
 * STORAGE_POOL
 *
 * ..................................................................................................
 */




class StoragePool : public Storeable {
   friend class Storeable;

public:

   class iterator {
       friend class StoragePool;

       int index;
       uint8_t* memend;
       uint8_t* variablePtr;

       inline iterator(StoragePool const & pool, uint8_t* variablePtr) : index(0), variablePtr(variablePtr) {
           memend = pool.memory + pool.memlength;
           check();
       }

       inline void check() {
           if (variablePtr > memend) {
               x_assert_fail("Overindexed StoragePool.", __FILE__, __LINE__);
           } else if (variablePtr == memend) {
               index = -1;
           } else {
               index++;
           }
       }

     public:
       inline void pp() {
           if (index>=0) {
               variablePtr += (*this)->__store_size;
               check();
           } else {
               x_assert_fail("Overindexed Storageable iterator.", __FILE__, __LINE__);
           }
       }

       inline iterator& operator++() {
           pp();
           return *this;
       }
       inline iterator& operator++(int postfix) {
           pp();
           return *this;
       }

       inline bool operator==(int arg) {
           return index == arg;
       }

       inline bool operator!=(int arg) {
           return index != arg;
       }

       inline DataPtr operator*() {
           return (DataPtr)variablePtr;
       }

       inline DataPtr operator->() {
           return (DataPtr)variablePtr;
       }
   };

#ifdef REG_CHILD
   class child_iterator {
       friend class StoragePool;

       int index;
       uint8_t* variableMemFirst;
       uint8_t* memend;
       DataPtr variablePtr;

       inline child_iterator(DataPtr variablePtr) : index(0), variablePtr(variablePtr) {
           variableMemFirst = (uint8_t*)variablePtr;
           memend = variableMemFirst + __store_size;
           check();
       }

       inline void check() {
           if (variablePtr > memend) {
               x_assert_fail("Overindexed StoragePool.", __FILE__, __LINE__);
           } else if (variablePtr->__next) {
               index++;
           } else {
               index = -1;
           }
       }

     public:
       inline void pp() {
           if (index>=0) {
               variablePtr = (DataPtr) (variableMemFirst + variablePtr->__next);
               check();
           } else {
               x_assert_fail("Overindexed Storageable iterator.", __FILE__, __LINE__);
           }
       }

       inline iterator& operator++() {
           pp();
           return *this;
       }
       inline iterator& operator++(int postfix) {
           pp();
           return *this;
       }

       inline void operator==(int arg) {
           return index == arg;
       }

       inline void operator!=(int arg) {
           return index != arg;
       }

       inline Storeable& operator*() {
           return *variablePtr;
       }

       inline DataPtr operator->() {
           return *variablePtr;
       }
   };
#endif

   enum CopyMode {
       Cp_All,
       Cp_Move,
       Cp_TmpDuplicate
   };

private:

   uint8_t *memory;
   size_t first_del_var;
   size_t deleted_vars;
   size_t memlength, memcapacity;

   size_t* intpointers;
   size_t intptrslength, intptrscapacity;
   ExternalPtr* extpointers;
   size_t extptrslength, extptrscapacity;

   size_t* childpools;
   size_t chplslength, chplscapacity;

   StoragePool * ownerPool;

   inline void movePointers(uint8_t* oldmemFrom, uint8_t* oldmemTo, uint8_t* origin = NULL) {
       xassert(ownerPool == this);
       if (!origin) {
           origin = memory;
       }

       moveInternalPointers(oldmemFrom, oldmemTo, origin);
       moveExternalPointers(oldmemFrom, oldmemTo, origin);
   }

   inline void moveInternalPointers(uint8_t* oldmemFrom, uint8_t* oldmemTo, uint8_t* origin = NULL) {
       xassert(ownerPool == this);
       if (!origin) {
           origin = memory;
       }

       std::ptrdiff_t d = origin - oldmemFrom;

       size_t* intPointersFrom = intpointers;
       size_t* intPointersTo = intpointers+intptrslength;
       for (; intPointersFrom<intPointersTo; intPointersFrom++) {
           ExternalPtr ptr = (ExternalPtr)decodeDeltaPtr(origin, *intPointersFrom);
           if (ptr) {
               moveVariable(oldmemFrom, oldmemTo, *ptr, d);
           }
       }
   }

   inline void moveExternalPointers(uint8_t* oldmemFrom, uint8_t* oldmemTo, uint8_t* origin = NULL) {
       xassert(ownerPool == this);
       if (!origin) {
           origin = memory;
       }

       std::ptrdiff_t d = origin - oldmemFrom;

       ExternalPtr* extPointersFrom = extpointers;
       ExternalPtr* extPointersTo = extpointers+extptrslength;
       for (; extPointersFrom<extPointersTo; extPointersFrom++) {
           ExternalPtr ptr = *extPointersFrom;
           if (ptr) {
               moveVariable(oldmemFrom, oldmemTo, *ptr, d);
           }
       }
   }

   inline void moveVariable(uint8_t* oldmemFrom, uint8_t* oldmemTo, DataPtr& variable, std::ptrdiff_t d) {
       xassert(ownerPool == this);

       uint8_t* variable_addr = (uint8_t*)variable;
       if (oldmemFrom<=variable_addr && variable_addr<oldmemTo) {

           variable_addr += d;
           variable = (DataPtr)variable_addr;

           xassert(contains(variable_addr));
       } else {
           std::cout << "Warning  StoragePool.moveVariable : (old) pointer to external data : "
                     << (void*) variable  << " of " << (void*) oldmemFrom << " .. " << (void*) oldmemTo << std::endl;
           StoragePool const * parent = getPool();
           if (parent && parent != this && !parent->contains(variable)) {
               std::cout << "Warning  StoragePool.moveVariable : (new) (parent) pointer to external data : "
                         << (void*) variable  << " of " << (void*) parent->memory << " .. " << (void*) (parent->memory+parent->memlength) << std::endl;
           }
       }
   }

   inline void fixAllPoolPointers() {
       fixPoolPointer();

       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+intptrslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           if (ptr) {
               ptr->fixAllPoolPointers();
           }
       }
   }

   inline void copyChildPools(StoragePool const & source) {
       xassert(ownerPool == this);

       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+intptrslength;
       size_t* src_chPoolsFrom = source.childpools;
       size_t* src_chPoolsTo = source.childpools+source.intptrslength;

       xassert(chPoolsTo-chPoolsFrom == src_chPoolsTo-src_chPoolsFrom);

       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++, src_chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           PtrToMe src_ptr = (PtrToMe)decodeDeltaPtr(source.memory, *src_chPoolsFrom);

           if (ptr && src_ptr) {
               ptr->fixPoolPointer();
               ptr->clear();
               *ptr = source;
               ptr->copyChildPools(*src_ptr);
           }
       }
   }

   inline void fixPoolPointer() {
       xassert(ownerPool == this);

       (void*&)memory[0] = this;
   }



   inline void allocParentItem(void*& _data, size_t store_size) {
       if (ownerPool != this) {
           ownerPool->allocParentItem(_data, store_size);
           return;
       }

       size_t oldmemlength, newmemlength, bufsz;
       uint8_t* oldmem;

      // if hollow >50% :
      if (deleted_vars > memlength>>1) {
          uint8_t* first = NULL;
          size_t continous = 0;
          uint8_t* da = decodeDeltaPtr(memory, first_del_var);
          for (iterator it(*this, da); it != -1; it++) {
              Storeable *cur = *it;
              if (cur->__kind == ST_DELETED) {
                  if (!first) first = it.variablePtr;
                  continous += cur->__store_size;
                  if (continous >= store_size) {
                      xassert(first >= da);

                      _data = first;
                      deleted_vars -= continous;

                      if (first == da) {
                          first_del_var = std::string::npos;
                          if (deleted_vars) {
                              for (it++; it != -1; it++) {
                                  if (it->__kind == ST_DELETED) {
                                      first_del_var = encodeDeltaPtr(memory, it.variablePtr);
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

      oldmemlength = memlength;
      newmemlength = oldmemlength + store_size;
      bufsz = getMemBufSize(newmemlength);
      oldmem = memory;
      if (memcapacity < bufsz) {
          extendBuffer(memory,           memlength,                       memcapacity, bufsz, 1/*sizeof(uint8_t)*/);
          if (oldmemlength) {
              movePointers(oldmem, oldmem+oldmemlength);
              fixAllPoolPointers();
          } else {
              fixPoolPointer();
              oldmemlength += sizeof(void*);
              newmemlength += sizeof(void*);
          }
      }
      memlength = newmemlength;
      _data = memory+oldmemlength;

      ok:

      DataPtr data = (DataPtr) _data;
      data->__kind = ST_PARENT;
      data->__parentVector = encodeDeltaPtr(memory, (uint8_t*)_data);
      data->__store_size = store_size;
   }

   inline void freeParentItem(DataPtr data) {
       if (ownerPool != this) {
           ownerPool->freeParentItem(data);
           return;
       }

       data->__kind = ST_DELETED;
       size_t dd = encodeDeltaPtr(memory, (uint8_t*)data);
       if (dd < first_del_var) {
           first_del_var = dd;
       }
       deleted_vars += data->__store_size;
   }

   inline void assignImpl(StoragePool const & src, CopyMode copyMode=Cp_All) {
       switch (copyMode) {
       case Cp_TmpDuplicate:
       {
           xassert(__kind != ST_DELETED && __kind == src.__kind && getPool() == src.getPool());

           ownerPool = (StoragePool*) &src;

           break;
       }
       case Cp_Move:
       {
           xassert(__kind != ST_DELETED && __kind == src.__kind && getPool() == src.getPool());

           fixPoolPointer();

           constcast(src).clear();
           break;
       }
       case Cp_All:
       {
           size_t bufsz = getMemBufSize(memlength);
           xassert(__kind != ST_DELETED && bufsz==memcapacity);

           memory = NULL;
           intpointers = NULL;
           extpointers = NULL;
           childpools = NULL;

           extptrslength = 0;
           extptrscapacity = 0;

           copyBuffer(src.memory, memlength, memcapacity, memory, 1/*sizeof(uint8_t)*/);
           copyBuffer((uint8_t*)src.intpointers, intptrslength, intptrscapacity, (uint8_t*&)intpointers, sizeof(size_t));
           copyBuffer((uint8_t*)src.childpools, chplslength, chplscapacity, (uint8_t*&)childpools, sizeof(size_t));

           moveInternalPointers(src.memory, src.memory+memlength);
           fixAllPoolPointers();
           break;
       }
       default:
           break;
       }
   }


   /*...................................................................................................
    * public:
    * ..................................................................................................
    */


public:

   StoragePool() : Storeable() {
       clear();
   }

   StoragePool(Storeable const & srcOrParent, bool childOfParent, CopyMode copyMode=Cp_All) :
       Storeable(srcOrParent, sizeof(StoragePool), childOfParent) {

       ownerPool = this;

       if (childOfParent) {
           memset(((uint8_t*)this)+sizeof(Storeable), 0, sizeof(StoragePool)-sizeof(Storeable));
           xassert(__kind == ST_CHILD && copyMode == Cp_All);
           first_del_var = std::string::npos;
       } else {
           StoragePool & srcOrParentPool = (StoragePool &) srcOrParent;
           assignImpl(srcOrParentPool, copyMode);
       }

       getPool()->addChildPool(this);
   }

   virtual ~StoragePool() {
       if (ownerPool != this) {
           clear();
       } else {
           del();
       }
   }

   inline void assign(StoragePool const & src, CopyMode copyMode=Cp_All) {
       memcpy(this, &src, sizeof(StoragePool));
       assignImpl(src, copyMode);
   }

   inline void del() {
       xassert(ownerPool == this);

       delChildPools();

       if (memory) delete[] memory;
       if (intpointers) delete[] intpointers;
       if (extpointers) delete[] extpointers;
       if (childpools) delete[] childpools;

       clear();
   }

   inline void delChildPools() {
       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+intptrslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           if (ptr) {
               ptr->del();
           }
       }
   }

   inline void clear() {
       memset(this, 0, sizeof(StoragePool));
       __parentVector = std::string::npos;
        first_del_var = std::string::npos;
        ownerPool = this;
   }

   StoragePool& operator= (StoragePool const &src) {
       xassert(ownerPool == this);

       delChildPools();

       if (memcapacity<src.memcapacity) {
           memcapacity = src.memcapacity;
           delete[] memory;
           memory = new uint8_t[memcapacity];
       }
       if (intptrscapacity<src.intptrscapacity) {
           intptrscapacity = src.intptrscapacity;
           delete[] intpointers;
           intpointers = new size_t[intptrscapacity];
       }
       if (chplscapacity<src.chplscapacity) {
           chplscapacity = src.chplscapacity;
           delete[] childpools;
           childpools = new size_t[chplscapacity];
       }

       extptrslength=0;
       extptrscapacity=0;
       if (extpointers) {
           delete[] extpointers;
           extpointers = NULL;
       }

       memcpy(memory, src.memory, memlength = src.memlength);
       memcpy(intpointers, src.intpointers, intptrslength = src.intptrslength);
       memcpy(childpools, src.childpools, chplslength = src.chplslength);

       copyChildPools(src);
       moveInternalPointers(src.memory, src.memory+memlength);

       return *this;
   }

   StoragePool& operator+= (StoragePool const &src) {
       StoragePool childView;
       append(src, childView);
       return *this;
   }

   void append(StoragePool const &src, StoragePool &childView, ExternalPtr* convertExtPointersFrom = NULL, ExternalPtr* convertExtPointersTo = NULL) {
       xassert(ownerPool == this);

       size_t c;
       if (memcapacity<(c=getMemBufSize(memlength+src.memlength))) {
           extendBuffer(memory, memlength, c, 1);
       }
       if (intptrscapacity<(c=getPtrBufSize(intptrslength+src.intptrslength))) {
           extendBuffer((uint8_t*&)intpointers, intptrslength, c, sizeof(size_t));
       }
       if (chplscapacity<(c=getPtrBufSize(chplslength+src.chplslength))) {
           extendBuffer((uint8_t*&)childpools, chplslength, c, sizeof(size_t));
       }

       memcpy(memory+memlength, src.memory, src.memlength);
       memcpy(intpointers+intptrslength, src.intpointers, src.intptrslength);
       memcpy(childpools+chplslength, src.childpools, src.chplslength);

       childView.clear();
       childView.ownerPool = NULL;
       childView.memory = memory + memlength;
       childView.memlength = src.memlength;
       childView.memcapacity = src.memcapacity;
       childView.intpointers = intpointers + intptrslength;
       childView.intptrslength = src.intptrslength;
       childView.intptrscapacity = src.intptrscapacity;
       childView.extpointers = NULL;
       childView.extptrslength = 0;
       childView.extptrscapacity = 0;
       childView.childpools = childpools + chplslength;
       childView.chplslength = src.chplslength;
       childView.chplscapacity = src.chplscapacity;

       childView.copyChildPools(src);
       childView.moveInternalPointers(src.memory, src.memory+memlength, memory);

       if (convertExtPointersFrom) {
           size_t oldl = extptrslength;

           convertExternalPointers(src, convertExtPointersFrom, convertExtPointersTo);

           childView.extpointers = extpointers + oldl;
           childView.extptrslength = extptrslength - oldl;
           childView.extptrscapacity = extptrscapacity - oldl;
       }

       childView.ownerPool = this;

       memlength += src.memlength;
       intptrslength += src.intptrslength;
       chplslength += src.chplslength;

   }

   void convertExternalPointers(StoragePool const &src, ExternalPtr* convertExtPointersFrom, ExternalPtr* convertExtPointersTo) {
       xassert(convertExtPointersFrom <= convertExtPointersTo);

       size_t plen = (convertExtPointersTo-convertExtPointersFrom) / sizeof(std::ptrdiff_t);
       size_t c;
       if (extptrscapacity<(c=getPtrBufSize(extptrslength+plen))) {
           extendBuffer((uint8_t*&)extpointers, extptrslength, c, sizeof(ExternalPtr));
       }

       ExternalPtr* extPointersFrom = extpointers + extptrslength;
       ExternalPtr* extPointersTo = extpointers + extptrslength + plen;
       uint8_t* src_bg = src.memory;
       uint8_t* src_end = src_bg + memlength;
       std::ptrdiff_t d = memory - src_bg;
       for (; extPointersFrom<extPointersTo; extPointersFrom++, convertExtPointersFrom++) {
           ExternalPtr srcPtr = *convertExtPointersFrom;
           if (srcPtr) {
               *extPointersFrom = srcPtr;
               moveVariable(src_bg, src_end, *srcPtr, d);
           }
       }

       extptrslength += plen;
   }

   inline size_t getExtPtrsLength() const {
       return extptrslength;
   }

   inline bool contains(void* pointer) const {
       return (memory<=pointer && pointer<memory+memlength);
   }

   template<class ST>
   inline void addPointer(ST*& _externalPointer) {
       addPointer((DataPtr&)_externalPointer);
   }

   template<class ST>
   inline void removePointer(ST*& _externalPointer) {
       removePointer((DataPtr&)_externalPointer);
   }

   inline void addPointer(DataPtr& dataPointer) {
       if (ownerPool != this) {
           ownerPool->addPointer(dataPointer);
           return;
       }

       if (dataPointer) {
           if (contains(dataPointer)) {
               xassert(dataPointer->getPool() == this);
           } else {
               std::cout << "Warning  StoragePool.addPointer : pointer to external data : "
                         << (void*) dataPointer  << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
               StoragePool const * parent = getPool();
               if (parent && parent != this && !parent->contains(dataPointer)) {
                   std::cout << "Warning  StoragePool.addPointer : (parent) pointer to external data : "
                             << (void*) dataPointer  << " of " << (void*) parent->memory << " .. " << (void*) (parent->memory+parent->memlength) << std::endl;
               }
           }
       }

       if (contains(&dataPointer)) {
           size_t pbufsz = getPtrBufSize(intptrslength);
           if (intptrscapacity < pbufsz) {
               extendBuffer((uint8_t*&)intpointers, intptrslength, intptrscapacity, pbufsz, sizeof(size_t));
           }
           intpointers[intptrslength++] = encodeDeltaPtr(memory, (uint8_t*)&dataPointer);
       } else {
           size_t pbufsz = getPtrBufSize(extptrslength);
           if (extptrscapacity < pbufsz) {
               extendBuffer((uint8_t*&)extpointers, extptrslength, extptrscapacity, pbufsz, sizeof(ExternalPtr));
           }
           extpointers[extptrslength++] = &dataPointer;
       }
   }

   inline void removePointer(DataPtr& dataPointer) {
       if (ownerPool != this) {
           ownerPool->removePointer(dataPointer);
           return;
       }

       if (dataPointer) {
           if (contains(dataPointer)) {
               xassert(dataPointer->getPool() == this);
           } else {
               std::cout << "Warning  StoragePool.removePointer : pointer to external data : "
                         << (void*) dataPointer  << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
               StoragePool const * parent = getPool();
               if (parent && parent != this && !parent->contains(dataPointer)) {
                   std::cout << "Warning  StoragePool.removePointer : (parent) pointer to external data : "
                             << (void*) dataPointer  << " of " << (void*) parent->memory << " .. " << (void*) (parent->memory+parent->memlength) << std::endl;
               }
           }
       }

       if (contains(&dataPointer)) {
           size_t dd = encodeDeltaPtr(memory, (uint8_t*)&dataPointer);
           size_t* val = std::lower_bound(intpointers, intpointers+intptrslength, dd);
           size_t vval = *val;
           if (vval == std::string::npos) {
               std::cout << "Warning  StoragePool.removePointer : internal poinrer already removed : " << (void*) &dataPointer
                         << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
           } else {
               xassert (vval == dd);
               //removeBufferItem((uint8_t*)intpointers, intptrslength, (uint8_t*)val, sizeof(size_t));
               *val = std::string::npos;
           }
       } else {
           ExternalPtr dd = &dataPointer;
           ExternalPtr* val = std::lower_bound(extpointers, extpointers+extptrslength, dd);
           ExternalPtr vval = *val;
           if (vval) {
               xassert (vval == dd);
               //removeBufferItem((uint8_t*)extpointers, extptrslength, (uint8_t*)val, sizeof(ExternalPtr));
               *val = NULL;
           } else {
               std::cout << "Warning  StoragePool.removePointer : external poinrer already removed : " << (void*) &dataPointer
                         << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
           }
       }
   }

   inline void removeAllExternalPointers() {
       delete extpointers;
       extptrslength = 0;
       extptrscapacity = 0;
   }

   inline void addChildPool(PtrToMe childPoolPointer) {
       xassert(ownerPool == this && contains(childPoolPointer));

       size_t pbufsz = getPtrBufSize(chplslength);
       if (chplscapacity < pbufsz) {
           extendBuffer((uint8_t*&)childpools, chplslength, chplscapacity, pbufsz, sizeof(size_t));
       }
       childpools[chplslength++] = encodeDeltaPtr(memory, (uint8_t*)childPoolPointer);
   }

   inline void removeChildPool(PtrToMe childPoolPointer) {
       xassert(ownerPool == this && contains(childPoolPointer));

       size_t dd = encodeDeltaPtr(memory, (uint8_t*)childPoolPointer);
       size_t* val = std::lower_bound(childpools, childpools+chplslength, dd);
       xassert (*val == dd);
       //removeBufferItem((uint8_t*)childpools, chplslength, (uint8_t*)val, sizeof(size_t));
       *val = std::string::npos;
   }

   inline iterator begin(DataPtr variablePtr = 0) {
       if (!variablePtr) {
           variablePtr = (DataPtr) memory;
       }
       return iterator(*this, (uint8_t*)variablePtr);
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



/****************************************************************************************************
 * STOREABLE
 *
 * ..................................................................................................
 */


inline Storeable::Storeable()
 #ifdef REG_CHILD
   , __next(0)
 #endif
{
#if debug_this
    if (__kind == ST_PARENT && __parentVector && __parentVector != std::string::npos) {
        StoragePool** the_first = (StoragePool**) decodeDeltaBackPtr((uint8_t*)this, __parentVector);
        StoragePool* pool = *the_first;
        if (pool->contains(this)) {
            std::cout<<"Bad allocation, use Storable(StoragePool &pool)" << std::endl;
        }
    }
#endif
    __kind = ST_NONE;
    __parentVector = std::string::npos;
    __store_size  = 0;
}

/* new operator filled __pool and __store_size previously, we use passed argument to double check */
inline Storeable::Storeable(StoragePool & pool) {
    xassert(__kind == ST_PARENT && getPool() == &pool && pool.contains(this));
}

template<class ME>
inline Storeable::Storeable(ME const & srcOrParent, bool childOfParent)
{
    init(srcOrParent, sizeof(ME), childOfParent);
}

inline Storeable::Storeable(Storeable const & srcOrParent, size_t size_of, bool childOfParent)
{
    init(srcOrParent, size_of, childOfParent);
}

inline Storeable::~Storeable() {
    switch (__kind) {
    case ST_PARENT:
        getPool()->freeParentItem(this);
        break;
    default:
        break;
    }
}

inline void Storeable::init(Storeable const & srcOrParent, size_t size_of, bool childOfParent) {
    if (childOfParent) {
        StoragePool * srcPool = srcOrParent.getPool();
        xassert(srcPool->contains(this));
        __kind = ST_CHILD;
        __parentVector = encodeDeltaPtr((uint8_t*)&srcOrParent, (uint8_t*)this);
        __store_size = getStoreSize(size_of);
        xassert(&srcOrParent == getParent());
        xassert(srcPool == getPool());
    #ifdef REG_CHILD
        __next = 0;
        getParent()->regChild(this);
    #endif
    } else {
        assign(srcOrParent, size_of);
    }

}

inline void Storeable::assign(Storeable const & src, size_t size_of) {
    // !
    xassert(src.__store_size == getStoreSize(size_of));

    memcpy(this, &src, size_of);

    // transfer __parentVector (we keep parent)
    switch (__kind) {
    case ST_NONE:
        xassert(__parentVector == std::string::npos);
        break;
    case ST_PARENT:
    {
        StoragePool * srcPool = src.getPool();
        xassert(srcPool->contains(this));
        __parentVector = encodeDeltaPtr((uint8_t*)srcPool->memory, (uint8_t*)this);
        xassert(srcPool == getPool());
        break;
    }
    case ST_CHILD:
    {
        StoragePool * srcPool = src.getPool();
        xassert(srcPool->contains(this));
        Storeable * srcParent = src.getParent();
        __parentVector = encodeDeltaPtr((uint8_t*)srcParent, (uint8_t*)this);
        xassert(srcParent == getParent());
        break;
    }
    default:
        x_assert_fail("Invalid kind.", __FILE__, __LINE__);
    }
}

inline void* Storeable::operator new (std::size_t size) {
    void* result = malloc(size);
    if (result) {
        return result;
    } else {
        throw std::bad_alloc();
    }
}

inline void* Storeable::operator new (std::size_t size, const std::nothrow_t& nothrow_value) {
    void* result = malloc(size);
    return result;
}

inline void* Storeable::operator new[] (std::size_t size) {
    void* result = malloc(size);
    if (result) {
        return result;
    } else {
        throw std::bad_alloc();
    }
}

inline void* Storeable::operator new[] (std::size_t size, const std::nothrow_t& nothrow_value) {
    void* result = malloc(size);
    return result;
}

inline void* Storeable::operator new (std::size_t size, StoragePool& pool) {
    void* data;
    pool.allocParentItem(data, getStoreSize(size));
    return data;
}

inline void* Storeable::operator new[] (std::size_t size, StoragePool& pool) {
    void* data;
    pool.allocParentItem(data, getStoreSize(size));
    return data;
}

inline void Storeable::operator delete (void* _ptr) {
    DataPtr ptr = (DataPtr) _ptr;
    switch (ptr->__kind) {
    case ST_NONE:
        free(_ptr);
        break;
    case ST_DELETED:
        x_assert_fail("Already deleted.", __FILE__, __LINE__);
        break;
    default:
        // Nothing to do here, everything is in ~Storeable
        break;
    }
}

inline void Storeable::operator delete[] (void* _ptr) {
    DataPtr ptr = (DataPtr) _ptr;
    switch (ptr->__kind) {
    case ST_NONE:
        free(_ptr);
        break;
    case ST_DELETED:
        x_assert_fail("Already deleted[].", __FILE__, __LINE__);
        break;
    default:
        // Nothing to do here, everything is in ~Storeable
        break;
    }
}

#endif // STORAGE_H

