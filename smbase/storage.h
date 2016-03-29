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
#define STOREABLE_COPY_CON2(classname,baseclass) classname(classname const & src) : baseclass(src) {}
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
    memcpy(buf, old, size*size_of);
    capacity = newcap;
    if (old) delete[] old;
}

inline void extendBuffer(uint8_t*& buf, size_t size, size_t newcap, size_t size_of) {
    xassert(newcap);
    uint8_t* old = buf;
    buf = new uint8_t[newcap*size_of];
    if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    memcpy(buf, old, size*size_of);
    if (old) delete[] old;
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

    typedef StoragePool* PtrToMe;
    typedef Storeable* DataPtr;
    typedef DataPtr* ExternalPtr;

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


public:


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
   Storeable(ME const & src, bool childOfParent);

   /**
    * this object is a non-pointer class field of the pointer/non-pointer stored variable, parent
    * @brief Storeable::Storeable
    * @param parent
    */
   Storeable(Storeable const & parent, size_t size_of);

   virtual ~Storeable();


   inline StoragePool & getPoolRef() const {
       StoragePool * pool = getPool();
       xassert(pool);
       return *pool;
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

   typedef StoragePool* PtrToMe;
   typedef Storeable* DataPtr;
   typedef DataPtr* ExternalPtr;

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


   inline void convertAll(uint8_t* oldmemFrom, uint8_t* oldmemTo) {
       std::ptrdiff_t d = memory - oldmemFrom;

       size_t* intPointersFrom = intpointers;
       size_t* intPointersTo = intpointers+intptrslength;
       for (; intPointersFrom<intPointersTo; intPointersFrom++) {
           ExternalPtr ptr = (ExternalPtr)decodeDeltaPtr(memory, *intPointersFrom);
           moveVariable(oldmemFrom, oldmemTo, *ptr, d);
       }

       ExternalPtr* extPointersFrom = extpointers;
       ExternalPtr* extPointersTo = extpointers+extptrslength;
       for (; extPointersFrom<extPointersTo; extPointersFrom++) {
           ExternalPtr ptr = *extPointersFrom;
           moveVariable(oldmemFrom, oldmemTo, *ptr, d);
       }

       fixChildPoolsInMem();
   }

   inline void moveVariable(uint8_t* oldmemFrom, uint8_t* oldmemTo, DataPtr& variable, std::ptrdiff_t d) {
       uint8_t* variable_addr = (uint8_t*)variable;
       if (oldmemFrom<=variable_addr && variable_addr<oldmemTo) {

           variable_addr += d;
           variable = (DataPtr)variable_addr;

           xassert(contains(variable_addr));
       } else {
           std::cout << "Warning  StoragePool.moveVariable : pointer to external data : "
                     << (void*) variable  << " of " << (void*) oldmemFrom << " .. " << (void*) oldmemTo << std::endl;
       }
   }

   inline void fixChildPoolsInMem() {
       fixThisPtrInMem();

       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+intptrslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           ptr->fixThisPtrInMem();
       }
   }

   inline void fixThisPtrInMem() {
       (void*&)memory[0] = this;
   }



   inline void allocParentItem(void*& data, size_t store_size) {
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

                      data = first;
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
      bufsz = ((newmemlength + STORE_BUF_SZ - 1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
      oldmem = memory;
      if (memcapacity < bufsz) {
          extendBuffer(memory,           memlength,                       memcapacity, bufsz, 1/*sizeof(uint8_t)*/);
          if (oldmemlength) {
              convertAll(oldmem, oldmem+oldmemlength);
          } else {
              fixThisPtrInMem();
              oldmemlength += sizeof(void*);
              newmemlength += sizeof(void*);
          }
      }
      memlength = newmemlength;
      data = memory+oldmemlength;

      ok:

      ((DataPtr)data)->__parentVector = encodeDeltaPtr(memory, (uint8_t*)data);
      ((DataPtr)data)->__store_size = store_size;
   }

   inline void freeParentItem(DataPtr data) {
       data->__kind = ST_DELETED;
       size_t dd = encodeDeltaPtr(memory, (uint8_t*)data);
       if (dd < first_del_var) {
           first_del_var = dd;
       }
       deleted_vars += data->__store_size;
   }


   /*...................................................................................................
    * public:
    * ..................................................................................................
    */


public:

   StoragePool() : Storeable() {
       clear();
   }

   StoragePool(StoragePool const & srcOrParentPool, bool childOfParent) : Storeable(srcOrParentPool, childOfParent),
       memory(0), intpointers(0), extpointers(0), childpools(0) {


       if (childOfParent) {
           xassert(__kind == ST_CHILD);
           first_del_var = std::string::npos;
       } else {
           xassert(__kind == ST_CHILD || __kind == ST_PARENT);

           size_t bufsz = ((memlength+STORE_BUF_SZ-1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
           xassert(bufsz==memcapacity);

           copyBuffer(srcOrParentPool.memory, memlength, memcapacity, memory, 1/*sizeof(uint8_t)*/);
           copyBuffer((uint8_t*)srcOrParentPool.intpointers, intptrslength, intptrscapacity, (uint8_t*&)intpointers, sizeof(size_t));
           copyBuffer((uint8_t*)srcOrParentPool.extpointers, extptrslength, extptrscapacity, (uint8_t*&)extpointers, sizeof(ExternalPtr));
           copyBuffer((uint8_t*)srcOrParentPool.childpools, chplslength, chplscapacity, (uint8_t*&)childpools, sizeof(size_t));

           convertAll(srcOrParentPool.memory, srcOrParentPool.memory+memlength);
       }

       getPool()->addChildPool(this);
   }

   virtual ~StoragePool() {
       del();
   }

   inline void del() {
       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+intptrslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           ptr->del();
       }

       if (memory) delete[] memory;
       if (intpointers) delete[] intpointers;
       if (extpointers) delete[] extpointers;
       if (childpools) delete[] childpools;

       clear();
   }

   inline void clear() {
       memset(this, 0, sizeof(StoragePool));
       __parentVector = std::string::npos;
        first_del_var = std::string::npos;
   }

   StoragePool& operator= (StoragePool const &src) {
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
       if (extptrscapacity<src.extptrscapacity) {
           extptrscapacity = src.extptrscapacity;
           delete[] extpointers;
           extpointers = new ExternalPtr[extptrscapacity];
       }
       if (chplscapacity<src.chplscapacity) {
           chplscapacity = src.chplscapacity;
           delete[] childpools;
           childpools = new size_t[chplscapacity];
       }

       memcpy(memory, src.memory, memlength = src.memlength);
       memcpy(intpointers, src.intpointers, intptrslength = src.intptrslength);
       memcpy(extpointers, src.extpointers, extptrslength = src.extptrslength);
       memcpy(childpools, src.childpools, chplslength = src.chplslength);

       convertAll(src.memory, src.memory+memlength);

       return *this;
   }

   void steal(StoragePool * src) {
       xassert(getPool() == src->getPool());

       del();

       memcpy(this, src, sizeof(StoragePool));

       fixChildPoolsInMem();

       src->clear();

       delete src;
   }


   inline bool contains(void* pointer) const {
       return (memory<=pointer && pointer<memory+memlength);
   }

   template<class ST>
   inline void addPointer(ST*& _externalPointer) {
       addPointer((void*&)_externalPointer);
   }

   template<class ST>
   inline void removePointer(ST*& _externalPointer) {
       removePointer((void*&)_externalPointer);
   }

   inline void addPointer(DataPtr& dataPointer) {
       if (dataPointer) {
           if (contains(dataPointer)) {
               xassert(dataPointer->getPool() == this);
           } else {
               std::cout << "Warning  StoragePool.addPointer : pointer to external data : "
                         << (void*) dataPointer  << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
           }
       }

       if (contains(&dataPointer)) {
           size_t pbufsz = ((intptrslength+STORE_BUF_PTR_SZ/*+1-1*/)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
           if (intptrscapacity < pbufsz) {
               extendBuffer((uint8_t*&)intpointers, intptrslength, intptrscapacity, pbufsz, sizeof(size_t));
           }
           intpointers[intptrslength++] = encodeDeltaPtr(memory, (uint8_t*)&dataPointer);
       } else {
           size_t pbufsz = ((extptrslength+STORE_BUF_PTR_SZ/*+1-1*/)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
           if (extptrscapacity < pbufsz) {
               extendBuffer((uint8_t*&)extpointers, extptrslength, extptrscapacity, pbufsz, sizeof(ExternalPtr));
           }
           extpointers[extptrslength++] = &dataPointer;
       }
   }

   inline void removePointer(DataPtr& externalPointer) {
       if (externalPointer) {
           if (contains(externalPointer)) {
               xassert(externalPointer->getPool() == this);
           } else {
               std::cout << "Warning  StoragePool.removePointer : pointer to external data : "
                         << (void*) externalPointer  << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
           }
       }

       if (contains(&externalPointer)) {
           size_t dd = encodeDeltaPtr(memory, (uint8_t*)&externalPointer);
           size_t* val = std::lower_bound(intpointers, intpointers+intptrslength, dd);
           xassert (*val == dd);
           removeBufferItem((uint8_t*)intpointers, intptrslength, (uint8_t*)val, sizeof(size_t));
       } else {
           ExternalPtr dd = &externalPointer;
           ExternalPtr* val = std::lower_bound(extpointers, extpointers+extptrslength, dd);
           xassert (*val == dd);
           removeBufferItem((uint8_t*)extpointers, extptrslength, (uint8_t*)val, sizeof(ExternalPtr));
       }
   }

   inline void addChildPool(PtrToMe childPoolPointer) {
       xassert(contains(childPoolPointer));

       size_t pbufsz = ((chplslength+STORE_BUF_PTR_SZ/*+1-1*/)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
       if (chplscapacity < pbufsz) {
           extendBuffer((uint8_t*&)childpools, chplslength, chplscapacity, pbufsz, sizeof(size_t));
       }
       childpools[chplslength++] = encodeDeltaPtr(memory, (uint8_t*)childPoolPointer);
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


inline Storeable::Storeable() : __kind(ST_NONE), __parentVector(std::string::npos), __store_size(0)
 #ifdef REG_CHILD
   , __next(0)
 #endif
{
}

/* new operator filled __pool and __store_size previously, we use passed argument to double check */
inline Storeable::Storeable(StoragePool & pool) : __kind(ST_PARENT) {
    xassert(getPool() == &pool);
    xassert(pool.contains(this));
}

template<class ME>
inline Storeable::Storeable(ME const & srcOrParent, bool childOfParent)
{
    if (childOfParent) {
        clear(sizeof(ME));
        __kind = ST_CHILD;
        __parentVector = encodeDeltaPtr((uint8_t*)&srcOrParent, (uint8_t*)this);
        __store_size = getStoreSize(sizeof(ME));
#ifdef REG_CHILD
        __next = 0;
        getParent()->regChild(this);
#endif
    } else {
        memcpy(this, &srcOrParent, sizeof(ME));

        // transfer __parentVector (we keep parent)
        switch (__kind) {
        case ST_PARENT:
            StoragePool * srcPool = srcOrParent.getPool();
            __parentVector = encodeDeltaPtr((uint8_t*)srcPool->memory, (uint8_t*)this);
            break;
        case ST_CHILD:
            Storeable * srcParent = srcOrParent.getParent();
            __parentVector = encodeDeltaPtr((uint8_t*)srcParent, (uint8_t*)this);
            break;
        default:
            x_assert_fail("Invalid kind.", __FILE__, __LINE__);
        }
    }

    StoragePool *pool = getPool();
    xassert(pool);
    xassert(pool->contains(this));
}

inline Storeable::Storeable(Storeable const & parent, size_t size_of)
{
    clear(size_of);
    __kind = ST_CHILD;
    __parentVector = encodeDeltaPtr((uint8_t*)&parent, (uint8_t*)this);
    __store_size = getStoreSize(size_of);
#ifdef REG_CHILD
    __next = 0;
    getParent()->regChild(this);
#endif
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

inline void Storeable::operator delete (void* ptr) {
    // Nothing to do here, everything is in ~Storeable
}

inline void Storeable::operator delete[] (void* ptr) {
    // Nothing to do here, everything is in ~Storeable
}

#endif // STORAGE_H

