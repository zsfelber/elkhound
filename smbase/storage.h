﻿#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <list>
#include <memory.h>
#include <malloc.h>
#include <stdint.h>
#include <algorithm>
#include <iostream>

#include "xassert.h"


struct __DbgStr {
    char const * str;
    explicit __DbgStr(char const * str) : str(str) {}
};

class VoidList;
class VoidTailList;
class VoidNode;

#define S1(x) #x
#define S2(x) S1(x)
#define __FILE_LINE__ __FILE__ " : " S2(__LINE__)

#ifdef DEBUG
#define DBG_INFO_FORMAL __DbgStr const objectName
#define DBG_INFO_FORMAL_FIRST __DbgStr const objectName,
#define DBG_INFO_ARG_FWD objectName
#define DBG_INFO_ARG_FWD_FIRST objectName,
#define DBG_INFO_ARG0 __DbgStr(__FILE_LINE__)
#define DBG_INFO_ARG0_FIRST __DbgStr(__FILE_LINE__),
#else
#define DBG_INFO_FORMAL
#define DBG_INFO_FORMAL_FIRST
#define DBG_INFO_ARG_FWD
#define DBG_INFO_ARG_FWD_FIRST
#define DBG_INFO_ARG0
#define DBG_INFO_ARG0_FIRST
#endif

#ifdef REG_CHILD
#define REG_CHILD_COMMA ,
#else
#define REG_CHILD_COMMA
#endif

#ifdef DEBUG
#define NDEBUG_COLON
#else
#define NDEBUG_COLON :
#endif

namespace str {



/****************************************************************************************************
 * LIBRARY STUFF
 *
 * ..................................................................................................
 */




class StoragePool;



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
#define STORE_NEW_REF0(pool, classname, var) classname & var = * new (pool) classname(DBG_INFO_ARG0_FIRST  pool)
#define STORE_NEW_REF(pool, classname, var, ...) classname & var = * new (pool) classname(DBG_INFO_ARG0_FIRST  pool, __VA_ARGS__)

//#define REG_CHILD

// 512
static const int STORE_BUF_PTR_BITS = 9;
static const int STORE_BUF_PTR_SZ = 1 << STORE_BUF_PTR_BITS;

// 16 Mbytes
static const int STORE_BUF_BITS = 24;
static const int STORE_BUF_SZ = 1 << STORE_BUF_BITS;

// 1 bit for each slot (32 bytes) : STORE_BUF_BITS-5
static const int STORE_BUF_VAR_SH = 5;
static const int STORE_BUF_ADDR_SZ = 1 << STORE_BUF_VAR_SH;

static const size_t npos = std::string::npos;

static void const * const LNULL = NULL;

//extern const size_t STORAGE_POOL_SIZE;
static const size_t STORAGE_POOL_SIZE = 96;//str::getStoreSize(sizeof(str::StoragePool));

extern const char* lastObjName;


template<typename T>
inline void memsetBuffer(T *buf, int32_t c, T n)
{
    for (uint32_t i = 0; i<n; i++, buf++) *buf = c;
}

template<typename T>
inline void copyBuffer(T* src, size_t srclen, size_t srccap,
                       T*& dest, size_t &dstlen, size_t &dstcap) {
    xassert(srccap && !dest);
    dest = new T[srccap];
    if (!dest) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    dstlen = srclen;
    dstcap = srccap;
    memcpy(dest, src, srclen*sizeof(T));
}

template<typename T>
inline void copyBuffer(T* src, size_t srclen, size_t srccap, T*& dest) {
    xassert(!dest);
    dest = new T[srccap];
    memcpy(dest, src, srclen*sizeof(T));
}

template<typename T>
inline void extendBuffer(T*& buf, size_t size, size_t& capacity, size_t newcap) {
    xassert(newcap);
    T* old = buf;
    buf = new T[newcap];
    if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    if (old) {
        memcpy(buf, old, size*sizeof(T));
        delete[] old;
    }
    capacity = newcap;
}

template<typename T>
inline void extendBuffer(T*& buf, size_t size, size_t newcap) {
    xassert(newcap);
    T* old = buf;
    buf = new T[newcap];
    if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    if (old) {
        memcpy(buf, old, size*sizeof(T));
        delete[] old;
    }
}

template<typename T>
inline void removeBufferItem(T* buf, size_t& size, T* pos) {
    size--;
    // allow overlap
    memmove(pos, pos+1, (size-(pos-buf))*sizeof(T));
}

template<typename T>
inline void insertBufferItem(T* buf, size_t& size, T* pos) {
    // allow overlap
    memmove(pos+1, pos, (size-(pos-buf))*sizeof(T));
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


inline size_t encodeDeltaPtr(uint8_t const * origin, uint8_t const * address) {
    if (!address || !origin) {
        return npos;
    } else {
       xassert(origin <= address);
        return (size_t)(address - origin);
    }
}

inline std::ptrdiff_t encodeSignedDeltaPtr(uint8_t const * origin, uint8_t const * address) {
    if (!address || !origin) {
        return 0;
    } else {
        return address - origin;
    }
}

inline uint8_t const * decodeDeltaPtr(uint8_t const * origin, size_t delta) {
    return delta == npos ? NULL : origin + delta;
}

inline uint8_t const * decodeDeltaBackPtr(uint8_t const * origin, size_t delta) {
    return delta == npos ? NULL : origin - delta;
}

inline uint8_t const * decodeSignedDeltaPtr(uint8_t const * origin, std::ptrdiff_t delta) {
    return delta == 0 ? NULL : origin + delta;
}

inline std::ostream &ind(std::ostream &os, int indent)
{
  while (indent--) {
    os << " ";
  }
  return os;
}

/**
 * std::lower_bound  with  null item (we ignore null items)
 *  @brief Finds the first position in which @a val could be inserted
 *         without changing the ordering.
 *  @param  __first   An iterator.
 *  @param  __last    Another iterator.
 *  @param  __val     The search term.
 *  @return         An iterator pointing to the first element <em>not less
 *                  than</em> @a val, or end() if every element is less than
 *                  @a val.
 *  @ingroup binary_search_algorithms
*/
template<typename T>
T* lower_bound(T* first, T* last,
      const T& val, const T& nullItem) {

  while (first < last) {
    std::ptrdiff_t half = (last-first) >> 1;
    T* middle = first + half;
    bool nothingToTheRight = false;

    if (*middle == nullItem) {
        T* _middle = middle;
        do {
            _middle++;
        } while (*_middle == nullItem && _middle < last);

        if (_middle == last) {
            nothingToTheRight = true;
            _middle = middle;

            do {
                _middle--;
            } while (*_middle == nullItem && _middle >= first);

            if (_middle < first) {
                return last;
            }
        }

        middle = _middle;
    }

    if (*middle < val) {
        first = middle+1;
        if (nothingToTheRight) {
            return first;
        }
    } else if (*middle > val) {
        // exclusive index  (no -1)
        last = middle;
    } else {
        return middle;
    }
  }
  return first;
}

template<typename T>
inline void shrinkTail(size_t& length, T* vect, T nullitm) {
    length--;
    while (length>0) {
        size_t chl = length-1;
        if (vect[chl] == nullitm) {
            length = chl;
        } else {
            break;
        }
    }
}


/****************************************************************************************************
 * STOREABLE
 *
 * ..................................................................................................
 */


class Storeable {
friend class StoragePool;
friend class ::VoidList;
friend class ::VoidTailList;
friend class ::VoidNode;

#ifdef DEBUG
    __DbgStr objectName;
#endif

    enum __Kind {
        ST_NONE = 0,
        ST_VALUE,
        ST_STORAGE_POOL,
        ST_DELETED
    };

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
#ifdef DEBUG
        uint8_t * bg = sizeof(objectName)+(uint8_t*)&objectName;
        uint8_t * th = (uint8_t*)this;
        memset(bg, 0, size_of-(bg-th));
#else
        memset(this, 0, size_of);
#endif
    }


    Storeable(Storeable & disable);

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


   Storeable(DBG_INFO_FORMAL_FIRST  size_t size_of = 0);

   /* new operator filled __pool and __store_size previously, we use passed argument to double check */
   Storeable(DBG_INFO_FORMAL_FIRST StoragePool & pool);

   /**
    * src: source object argument of copy constructor
    * default and very effective implementation
    * should be additionally invoked for each Storeable class field too
    * @brief Storeable::Storeable
    * @param parent
    */
   template<class ME>
   Storeable(DBG_INFO_FORMAL_FIRST  ME const & srcOrParent, bool childOfParent);

   /**
    * this object is a non-pointer class field of the pointer/non-pointer stored variable, parent
    * @brief Storeable::Storeable
    * @param parent
    */
   Storeable(DBG_INFO_FORMAL_FIRST  Storeable const & srcOrParent, size_t size_of, bool childOfParent);

   virtual ~Storeable();

   void assign(Storeable const & srcOrParent, size_t size_of);

   void assignSameParent(Storeable const & srcOrParent);

   inline __Kind getKind() const {
       return (__Kind)__kind;
   }

   inline StoragePool & getParentRef() const {
       return NN(getParent());
   }

   inline StoragePool * getParent() const {
       switch (__kind) {
       case ST_NONE:
           return NULL;
       case ST_VALUE:
       case ST_STORAGE_POOL:
       {
           StoragePool** the_first = (StoragePool**) decodeDeltaBackPtr((uint8_t*)this, __parentVector);
           if (the_first) {
               return *the_first;
           } else {
               std::cout << "Warning  Storeable.getParent() : getParent() of temporary object. "
                         << std::endl;
               return NULL;
           }
       }
       default:
           x_assert_fail("Wrong kind.", __FILE__, __LINE__);
           break;
       }
   }

   inline StoragePool * getPool() const {
       switch (__kind) {
       case ST_NONE:
           return asPool();
       case ST_STORAGE_POOL:
           return (StoragePool*)this;
       case ST_VALUE:
           return getParent();
       default:
           x_assert_fail("Wrong kind.", __FILE__, __LINE__);
           break;
       }
   }

   inline StoragePool & getPoolRef() const {
       return NN(getPool());
   }

   inline virtual StoragePool * asPool() const {
       return NULL;
   }

   inline void debugPrint(int indent = 0) const
   {
       debugPrint(std::cout, indent);
   }

   virtual inline void debugPrint(std::ostream& os, int indent = 0, char const *subtreeName = 0) const
   {
   }
};





/****************************************************************************************************
 * STORAGE_POOL
 *
 * ..................................................................................................
 */


class SwapVars {
    friend class ::VoidList;
    friend class ::VoidTailList;
    friend class ::VoidNode;

    friend void entry();

protected:
    uint8_t *memory;
    size_t first_del_var;
    size_t deleted_vars;
    size_t memlength;
    size_t memcapacity;
    size_t* intpointers;
    size_t intptrslength, intptrscapacity;

    Storeable::ExternalPtr* extpointers;
    size_t extptrslength, extptrscapacity;
};



class StoragePool : public Storeable, public SwapVars {
   friend class Storeable;
   friend class ::VoidList;
   friend class ::VoidTailList;
   friend class ::VoidNode;

   friend void entry();

public:

   class iterator {
       friend class StoragePool;

       int index;
       uint8_t* memend;
       uint8_t const * variablePtr;

       inline iterator(StoragePool const & pool, uint8_t const * variablePtr) : index(0), variablePtr(variablePtr) {
           memend = pool.memory + pool.memlength;
           check();
       }

       inline void check() {
           if (variablePtr >= memend) {
               index = -1;
           } else {
               index++;
           }
       }

     public:
       inline void pp() {
           if (index>=0) {
               size_t st = (*this)->__store_size;
               xassert(st);
               variablePtr += st;
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

   struct ChCh {
       StoragePool * pool;
       int index;
   };

   enum CopyMode {
       Cp_All,
       Cp_Move,
       Cp_Duplicate,
       Cp_TmpDuplicate,
   };

private:

   size_t* childpools;
   size_t chplslength, chplscapacity;

   StoragePool * ownerPool;

   // ! __parentVector0 is the last !

   size_t __parentVector0;

   inline void movePointers(StoragePool const & oldmem, uint8_t* origin = NULL) {
       xassert(ownerPool == this);
       if (!origin) {
           origin = memory;
       }

       moveInternalPointers(oldmem, origin);
       moveExternalPointers(oldmem, origin);
   }

   void moveInternalPointers(StoragePool const & oldmem, uint8_t* origin = NULL, uint8_t* deltaOrigin = NULL) {
       xassert(ownerPool == this);
       if (!origin) {
           origin = memory;
       }
       if (!deltaOrigin) {
           deltaOrigin = memory;
       }

       std::ptrdiff_t d = origin - oldmem.memory;

       size_t* intPointersFrom = intpointers;
       size_t* intPointersTo = intpointers+intptrslength;
       for (; intPointersFrom<intPointersTo; intPointersFrom++) {
#ifdef DEBUG
           ExternalPtr ptr0 = (ExternalPtr)decodeDeltaPtr(oldmem.memory, *intPointersFrom);
           if (ptr0) {
               xassert(oldmem.contains(ptr0));
#endif
               ExternalPtr ptr = (ExternalPtr)decodeDeltaPtr(deltaOrigin, *intPointersFrom);
               xassert(ptr);

               if (*ptr0 && oldmem.contains(*ptr0)) {
                   xassert(moveVariable(oldmem, *ptr, d));
               }
               size_t dpt = encodeDeltaPtr(origin, (uint8_t*)ptr);
               *intPointersFrom = dpt;
#ifdef DEBUG
           }
#endif
       }
   }

   void moveExternalPointers(StoragePool const & oldmem, uint8_t* origin = NULL) {
       xassert(ownerPool == this);
       if (!origin) {
           origin = memory;
       }

       std::ptrdiff_t d = origin - oldmem.memory;

       ExternalPtr* extPointersFrom = extpointers;
       ExternalPtr* extPointersTo = extpointers+extptrslength;
       for (; extPointersFrom<extPointersTo; extPointersFrom++) {
           ExternalPtr ptr = *extPointersFrom;
           if (ptr) {
               moveVariable(oldmem, *ptr, d);
           }
       }
   }

   inline void moveFrom(StoragePool const & parent, DataPtr & child, uint8_t * origin = NULL) {
       if (!origin) {
           origin = memory;
       }

       StoragePool*  pool = this;
       std::ptrdiff_t d;

       if (pool->chplslength) {
           std::list<ChCh> chch;
           parent.findChildChain(chch, child);
           xassert(chch.size());

           ChCh ch;
           ch.index = 0;
           std::list<ChCh>::iterator it ;
           for (it = chch.begin(); ; it++) {
                ch = *it;
                if (ch.index == -1) {
                    it++;
                    break;
                } else {
                    xassert(pool->chplslength == ch.pool->chplslength);
                    StoragePool*  chpool = (StoragePool*) decodeDeltaPtr(pool->memory, pool->childpools[ch.index]);
                    pool = chpool;
                }
           }

           xassert(it == chch.end());

           d = pool->memory - ch.pool->memory;
       } else {
           xassert(parent.contains(child));

           d = pool->memory - parent.memory;
       }

       uint8_t* variable_addr = (uint8_t*)child;
       variable_addr += d;
       child = (DataPtr)variable_addr;

       StoragePool const * pool0 = findChild(variable_addr);
       xassert(pool0 == pool);
   }

   bool moveVariable(StoragePool const & oldmem, DataPtr& variable, std::ptrdiff_t d) const {
       xassert(ownerPool == this);

       if (variable) {
           uint8_t* variable_addr = (uint8_t*)variable;
#ifdef DEBUG
           StoragePool const * pool0 = oldmem.findChild(variable_addr);
           if (pool0 == &oldmem) {
#else
           if (oldmem.contains(variable_addr)) {
#endif

                variable_addr += d;
                variable = (DataPtr)variable_addr;

#ifdef DEBUG
                StoragePool const * pool = findChild(variable_addr);
                xassert(pool == this);
#else
                xassert (contains(variable_addr));
#endif
                return true;
           } else {
                return (contains(variable_addr));
           }
       }
   }

   void fixAllPoolPointers() {
       fixPoolPointer();

       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+chplslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           if (ptr) {
               ptr->fixAllPoolPointers();
           }
       }
   }

   void copyChildPools(StoragePool const & source, uint8_t* deltaOrigin = NULL) {
       xassert(ownerPool == this);

       size_t* tar_chPoolsFrom = childpools;
       size_t* tar_chPoolsTo = childpools+chplslength;
       size_t* src_chPoolsFrom = source.childpools;
       size_t* src_chPoolsTo = source.childpools+source.chplslength;
       if (!deltaOrigin) {
           deltaOrigin = memory;
       }
       std::ptrdiff_t d = deltaOrigin - source.memory;
       if (d) {
           xassert(memory != source.memory);


           xassert(tar_chPoolsTo-tar_chPoolsFrom == src_chPoolsTo-src_chPoolsFrom);

           for (; tar_chPoolsFrom<tar_chPoolsTo; tar_chPoolsFrom++, src_chPoolsFrom++) {
               PtrToMe ptr = (PtrToMe)decodeDeltaPtr(deltaOrigin, *tar_chPoolsFrom);
               PtrToMe src_ptr = (PtrToMe)decodeDeltaPtr(source.memory, *src_chPoolsFrom);

               xassert(bool(ptr)==bool(src_ptr));
               xassert(ptr->__kind==src_ptr->__kind);
               xassert(ptr->__kind == ST_VALUE || ptr->__kind == ST_STORAGE_POOL);

               if (ptr) {
                   xassert( src_ptr->ownerPool == ptr->ownerPool );
                   xassert( (d+(uint8_t*)src_ptr) == (uint8_t*)ptr);
                   xassert( source.contains(src_ptr) );
                   xassert( contains(ptr) );

                   ptr->fixPoolPointer(src_ptr);
                   ptr->clear();
                   ptr->reassign(*src_ptr);
                   ptr->copyChildPools(*src_ptr);
               }
           }
       }
   }

   inline void fixPoolPointer() {
       xassert(ownerPool == this);

       if (memory) {
           (void*&)memory[0] = this;
       }  else {
           std::cout << "Warning  Storeable.fixPoolPointer(1) : fixPoolPointer() of empty pool. "
                     << std::endl;
       }
   }

   inline void fixPoolPointer(StoragePool const * src) {
       if (ownerPool == src) {
           ownerPool = this;
       } else {
           if (ownerPool != this) {
               xassert(ownerPool->isParentOf(*this));
           }
       }

       if (memory) {
           (void*&)memory[0] = this;
       }  else {
           std::cout << "Warning  Storeable.fixPoolPointer(2) : fixPoolPointer() of empty pool. "
                     << std::endl;
       }
   }


   void allocParentItem(void const *& _data, size_t store_size) {
       xassert(ownerPool == this);
       /*if (ownerPool != this) {
           ownerPool->allocParentItem(_data, store_size);
           return;
       }*/

       size_t oldmemlength, newmemlength, bufsz;

      // if hollow >50% :
      if (deleted_vars > memlength>>1) {
          uint8_t const * first = NULL;
          size_t continous = 0;
          uint8_t const * da = decodeDeltaPtr(memory, first_del_var);
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
                          first_del_var = npos;
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
      if (!oldmemlength) {
            bufsz = getMemBufSize(newmemlength);
            memory = new uint8_t[memcapacity = bufsz];
            fixPoolPointer();
            oldmemlength += sizeof(void*);
            newmemlength += sizeof(void*);
      } else if (newmemlength > memcapacity - STORAGE_POOL_SIZE) {
          // FIXME insert child pool instead of large block memcpy
          StoragePool * child = new (*this)  StoragePool(DBG_INFO_ARG0_FIRST  *this, true);
          swap(child);
      }
      memlength = newmemlength;
      _data = memory+oldmemlength;

      ok:

      DataPtr data = (DataPtr) _data;
      data->__kind = ST_VALUE;
      data->__parentVector = encodeDeltaPtr(memory, (uint8_t*)_data);
      if (store_size == STORAGE_POOL_SIZE) {
          ((StoragePool*)data)->__parentVector0 = data->__parentVector;
      }
      data->__store_size = store_size;
   }

   void freeParentItem(DataPtr data) {
       xassert(ownerPool == this);
       /*if (ownerPool != this) {
           ownerPool->freeParentItem(data);
           return;
       }*/

       data->__kind = ST_DELETED;
       size_t dd = encodeDeltaPtr(memory, (uint8_t*)data);
       if (dd < first_del_var) {
           first_del_var = dd;
       }
       deleted_vars += data->__store_size;
   }

   inline void assignImpl(StoragePool const & src, CopyMode copyMode=Cp_All) {

       switch (copyMode) {
       case Cp_Duplicate:
       {
           xassert(!isParentOf(src));
           xassert(!src.isParentOf(*this));
           xassert((__kind == ST_NONE || __kind == ST_STORAGE_POOL) && (src.__kind == ST_NONE || src.__kind == ST_STORAGE_POOL));

           __parentVector = __parentVector0;
           //// keep it 'this'!! ownerPool = (StoragePool*) &src;
           ownerPool = (StoragePool*) &src;
           xassert(getParentRef().contains(this));

           break;
       }
       case Cp_TmpDuplicate:
       {
           xassert(!isParentOf(src));
           xassert(!src.isParentOf(*this));
           xassert((__kind == ST_NONE || __kind == ST_STORAGE_POOL) && (src.__kind == ST_NONE || src.__kind == ST_STORAGE_POOL));

           ownerPool = (StoragePool*) &src;

           break;
       }
       case Cp_Move:
       {
           xassert(!isParentOf(src));
           xassert(!src.isParentOf(*this));
           xassert((__kind == ST_NONE || __kind == ST_STORAGE_POOL) && (src.__kind == ST_NONE || src.__kind == ST_STORAGE_POOL));

           //if (getParent() == src.getParent()) {
               fixPoolPointer(&src);

               constcast(src).clear();
           //} else {
           //    itt  reassign ?
           //}
           break;
       }
       case Cp_All:
       {
           size_t bufsz = getMemBufSize(memlength);
           xassert((__kind == ST_NONE || __kind == ST_STORAGE_POOL) && (src.__kind == ST_NONE || src.__kind == ST_STORAGE_POOL) && bufsz==memcapacity);

           reassign(src);
           break;
       }
       default:
           xassert(false);
           break;
       }
   }

   void reassign(StoragePool const & src) {
       xassert(ownerPool == this);
       xassert(!isParentOf(src));
       xassert(!src.isParentOf(*this));

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
       memcpy(intpointers, src.intpointers, sizeof(size_t)*(intptrslength = src.intptrslength));
       memcpy(childpools, src.childpools, sizeof(size_t)*(chplslength = src.chplslength));

       fixAllPoolPointers();
       copyChildPools(src);
       moveInternalPointers(src);
   }

   inline bool isParentOf(Storeable const & chpool) const {
       StoragePool * par = chpool.getParent();
       if (par == this) {
           return true;
       } else if (par) {
           return isParentOf(*par);
       } else {
           return false;
       }
   }

   inline StoragePool * getRootPool() const {
       switch (__kind) {
       case ST_NONE:
           return constcast(this);
       case ST_VALUE:
       case ST_STORAGE_POOL:
       {
           return getParent()->getRootPool();
       }
       default:
           x_assert_fail("Wrong kind.", __FILE__, __LINE__);
           break;
       }
   }

   inline StoragePool & getRootPoolRef() const {
       return NN(getRootPool());
   }

   inline StoragePool * asPool() const {
       return constcast(this);
   }



   /*...................................................................................................
    * public:
    * ..................................................................................................
    */


public:

   StoragePool(DBG_INFO_FORMAL) : Storeable(DBG_INFO_ARG_FWD) {
       clear();
       __store_size = STORAGE_POOL_SIZE;
   }

   StoragePool(DBG_INFO_FORMAL_FIRST Storeable const & srcOrParent, bool childOfParent, CopyMode copyMode=Cp_All) :
       Storeable(DBG_INFO_ARG_FWD_FIRST srcOrParent, sizeof(StoragePool)-sizeof(__parentVector0), childOfParent) {

       xassert(!isParentOf(srcOrParent));
       xassert(__store_size == STORAGE_POOL_SIZE);

       if (childOfParent) {
           memset(((uint8_t*)this)+sizeof(Storeable), 0, sizeof(StoragePool)-sizeof(Storeable));
           xassert((__kind == ST_VALUE||__kind == ST_STORAGE_POOL) && copyMode == Cp_All);
           __kind = ST_STORAGE_POOL;
           first_del_var = npos;
           ownerPool = this;
       } else {
           xassert(__kind == ST_NONE || __kind == ST_STORAGE_POOL);
           ownerPool = this;
           StoragePool & srcOrParentPool = (StoragePool &) srcOrParent;
           assignImpl(srcOrParentPool, copyMode);
       }

       if (copyMode != Cp_TmpDuplicate) {
           StoragePool * pool = getParent();
           if (pool) {
               pool->addChildPool(this);
           }
       }
   }

   virtual ~StoragePool() {
       StoragePool * p = getParent();
       if (p) {
           p->removeChildPool(this);
       }

       if (ownerPool != this) {
           clear();
       } else {
           del();
       }
   }

   inline void assign(StoragePool const & src, CopyMode copyMode=Cp_All) {
       xassert(!isParentOf(src));
       xassert(!src.isParentOf(*this));

#ifdef DEBUG
       uint8_t * bg = sizeof(objectName)+(uint8_t*)&objectName;
       uint8_t * th = (uint8_t*)this;
       std::ptrdiff_t d = bg-th;
       memcpy(bg, d+(uint8_t*)&src, sizeof(StoragePool)-sizeof(__parentVector0)-d);
#else
       memcpy(this, &src, sizeof(StoragePool)-sizeof(__parentVector0));
#endif
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
       size_t* chPoolsTo = childpools+chplslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           if (ptr && ptr->ownerPool == ptr) {
               ptr->del();
           }
       }
   }

   inline void clear() {
       __Kind k = getKind();
#ifdef DEBUG
       uint8_t * bg = sizeof(objectName)+(uint8_t*)&objectName;
       uint8_t * th = (uint8_t*)this;
       memset(bg, 0, sizeof(StoragePool)-(bg-th));
#else
       memset(this, 0, sizeof(StoragePool));
#endif
       __kind = k;
       __parentVector = npos;
        first_del_var = npos;
        ownerPool = this;
        __store_size = STORAGE_POOL_SIZE;
   }

   StoragePool& operator= (StoragePool const &src);

   inline StoragePool& operator+= (StoragePool const &src) {
       StoragePool childView(DBG_INFO_ARG0);
       append(src, childView);
       return *this;
   }

   void append(StoragePool const &src, StoragePool &childView, ExternalPtr* convertExtPointersFrom = NULL, ExternalPtr* convertExtPointersTo = NULL, bool copyChildPools = true) {
       xassert(ownerPool == this);

       size_t c;
       size_t snetmemlen = src.memlength - sizeof(void*);
       uint8_t* soldorigin = src.memory + sizeof(void*);

       if (memcapacity<(c=getMemBufSize(memlength+snetmemlen))) {
           extendBuffer(memory, memlength, c);
           memcapacity = c;
       }
       if (intptrscapacity<(c=getPtrBufSize(intptrslength+src.intptrslength))) {
           extendBuffer(intpointers, intptrslength, c);
           intptrscapacity = c;
       }
       if (copyChildPools && chplscapacity<(c=getPtrBufSize(chplslength+src.chplslength))) {
           extendBuffer(childpools, chplslength, c);
           chplscapacity = c;
       }

       memcpy(memory+memlength, soldorigin, snetmemlen);
       // ! still remains sorted
       memcpy(intpointers+intptrslength, src.intpointers, sizeof(size_t)*src.intptrslength);
       if (copyChildPools) {
           // ! still remains sorted
           memcpy(childpools+chplslength, src.childpools, sizeof(size_t)*src.chplslength);
       }

       childView.clear();
       childView.memory = memory + memlength;
       childView.memlength = snetmemlen;
       childView.memcapacity = src.memcapacity;
       childView.intpointers = intpointers + intptrslength;
       childView.intptrslength = src.intptrslength;
       childView.intptrscapacity = src.intptrscapacity;
       if (copyChildPools) {
           childView.childpools = childpools + chplslength;
           childView.chplslength = src.chplslength;
           childView.chplscapacity = src.chplscapacity;
       }

       memlength += snetmemlen;
       intptrslength += src.intptrslength;
       if (copyChildPools) {
            chplslength += src.chplslength;
       }

       uint8_t * deltaChildOrigin = childView.memory - sizeof(void*);

       childView.moveInternalPointers(src, memory, deltaChildOrigin);

       if (convertExtPointersFrom) {
           convertExternalPointers(src, convertExtPointersFrom, convertExtPointersTo);
       }

       if (copyChildPools) {
           childView.copyChildPools(src, deltaChildOrigin);
       }

       childView.ownerPool = this;

       if (copyChildPools) {
           size_t* chPoolsFrom = childView.childpools;
           size_t* chPoolsTo = childView.childpools+childView.chplslength;
           for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
               PtrToMe ptr = (PtrToMe)decodeDeltaPtr(deltaChildOrigin, *chPoolsFrom);
               if (ptr) {
                   *chPoolsFrom = encodeDeltaPtr(memory, (uint8_t*)ptr);
               }
           }
       }

   }

   inline void swap(StoragePool * pool) {
       uint8_t buf[sizeof(SwapVars)];
       uint8_t *_this = sizeof(Storeable) + (uint8_t*) this;
       uint8_t *_pool = sizeof(Storeable) + (uint8_t*) pool;

       memcpy(buf, _this, sizeof(SwapVars));
       memcpy(_this, _pool, sizeof(SwapVars));
       memcpy(_pool, buf, sizeof(SwapVars));

       fixAllPoolPointers();
       pool->fixAllPoolPointers();
   }

   void convertExternalPointers(StoragePool const &src, ExternalPtr* convertExtPointersFrom, ExternalPtr* convertExtPointersTo) {
       xassert(convertExtPointersFrom <= convertExtPointersTo);

       size_t plen = convertExtPointersTo - convertExtPointersFrom;
       size_t c;
       if (extptrscapacity<(c=getPtrBufSize(extptrslength+plen))) {
           extendBuffer(extpointers, extptrslength, c);
       }

       ExternalPtr* extPointersFrom = extpointers + extptrslength;
       ExternalPtr* extPointersTo = extPointersFrom + plen;
       std::ptrdiff_t d = memory - src.memory;
       for (; extPointersFrom<extPointersTo; extPointersFrom++, convertExtPointersFrom++) {
           ExternalPtr srcPtr = *convertExtPointersFrom;
           if (srcPtr) {
               *extPointersFrom = srcPtr;
               moveVariable(src, *srcPtr, d);
           }
       }

       extptrslength += plen;
       // ! still remains sorted
       std::sort(extpointers, extpointers+extptrslength);
   }

   inline size_t getExtPtrsLength() const {
       return extptrslength;
   }

   inline bool contains(void* pointer) const {
       return (memory<=pointer && pointer<memory+memlength);
   }

   inline StoragePool const * findChild(void * pointer) const {
       if (contains(pointer)) {
           return this;
       }
       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+chplslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           if (ptr) {
               StoragePool const * result = ptr->findChild(pointer);
               if (result) {
                   return result;
               }
           }
       }
       return NULL;
   }

   inline void findChildChain(std::list<ChCh> & result, void * pointer) const {
       if (contains(pointer)) {
           ChCh ch;
           ch.pool = constcast(this);
           ch.index = -1;
           result.push_back(ch);
           return;
       }
       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+chplslength;
       for (int i = 0; chPoolsFrom<chPoolsTo; i++, chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           if (ptr) {
               ptr->findChildChain(result, pointer);
               if (result.size()) {
                   ChCh ch;
                   ch.pool = constcast(this);
                   ch.index = i;
                   result.push_front(ch);
                   return;
               }
           }
       }
   }

   inline size_t* getChildPointer(StoragePool const * child) const {
       size_t dd = encodeDeltaPtr(memory, (uint8_t*)child);
       size_t* val = lower_bound(childpools, childpools+chplslength, dd, npos);
       if (*val == dd) {
           return val;
       } else {
           return NULL;
       }
   }

   template<class ST>
   inline void addPointer(ST*& _externalPointer) {
       addPointer((DataPtr&)_externalPointer);
   }

   template<class ST>
   inline void removePointer(ST*& _externalPointer) {
       removePointer((DataPtr&)_externalPointer);
   }

   void addPointer(DataPtr& dataPointer) {
       xassert(ownerPool == this);
       /*if (ownerPool != this) {
           ownerPool->addPointer(dataPointer);
           return;
       }*/

       StoragePool const * child = dataPointer?findChild(dataPointer):NULL, * pchild = findChild(&dataPointer);

       if (dataPointer) {
           if (child == this) {
               xassert(dataPointer->getParent() == this);
           } else {

               if (!child) {
                   StoragePool const * parent = getParent();
                   if (parent && parent != this && !parent->contains(dataPointer)) {
                       std::cout << "Warning  StoragePool.addPointer : pointer to external data : "
                                 << (void*) dataPointer  << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
                       std::cout << "Warning  StoragePool.addPointer : (parent) pointer to external data : "
                                 << (void*) dataPointer  << " of " << (void*) parent->memory << " .. " << (void*) (parent->memory+parent->memlength) << std::endl;
                   }
               }
           }
       } else {
           child = pchild;
       }

       if (pchild == this) {
           size_t dd = encodeDeltaPtr(memory, (uint8_t*)&dataPointer);
           // to ensure it is ordered (binary search invariant)
           if (intptrslength) {
               xassert(dd > intpointers[intptrslength-1]);
           }
           size_t pbufsz = getPtrBufSize(intptrslength);
           if (intptrscapacity < pbufsz) {
               extendBuffer(intpointers, intptrslength, intptrscapacity, pbufsz);
           }
           intpointers[intptrslength++] = dd;
       } else if (pchild) {
           xassert(child == pchild);
           /*if (child != pchild) {
               std::cout << "Warning  StoragePool.addPointer : pointer pool mismatch:  (**) "
                         << (void*) &dataPointer  << "  (*) " << (void*) dataPointer  << " of "
                         << "child(**):" << (void*) pchild->memory << " .. " << (void*) (pchild->memory+pchild->memlength)
                         << " child(*):" << (void*) child->memory << " .. " << (void*) (child->memory+child->memlength)
                         << "   parent:" << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
           }*/
           constcast(pchild)->addPointer(dataPointer);
       } else {
           ExternalPtr dd = &dataPointer;
           size_t pbufsz = getPtrBufSize(extptrslength);
           if (extptrscapacity < pbufsz) {
               extendBuffer(extpointers, extptrslength, extptrscapacity, pbufsz);
           }
           // to ensure it is ordered (binary search invariant)
           ExternalPtr* val = lower_bound(extpointers, extpointers+extptrslength, dd, (ExternalPtr&)LNULL);
           insertBufferItem(extpointers, extptrslength, val);
           *val = dd;
       }
   }

   void removePointer(DataPtr& dataPointer) {
       xassert(ownerPool == this);
       /*if (ownerPool != this) {
           ownerPool->removePointer(dataPointer);
           return;
       }*/

       StoragePool const * child = dataPointer?findChild(dataPointer):NULL, * pchild = findChild(&dataPointer);

       if (dataPointer) {
           if (child == this) {
               xassert(dataPointer->getParent() == this);
           } else {

               if (!child) {
                   StoragePool const * parent = getParent();
                   if (parent && parent != this && !parent->contains(dataPointer)) {
                       std::cout << "Warning  StoragePool.removePointer : pointer to external data : "
                                 << (void*) dataPointer  << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
                       std::cout << "Warning  StoragePool.removePointer : (parent) pointer to external data : "
                                 << (void*) dataPointer  << " of " << (void*) parent->memory << " .. " << (void*) (parent->memory+parent->memlength) << std::endl;
                   }
               }
           }
       } else {
           child = pchild;
       }

       if (pchild == this) {
           size_t dd = encodeDeltaPtr(memory, (uint8_t*)&dataPointer);
           size_t* last = intpointers+intptrslength;
           size_t* val = lower_bound(intpointers, last, dd, npos);
           size_t vval = *val;
           if (vval == npos) {
               std::cout << "Warning  StoragePool.removePointer : internal poinrer already removed : " << (void*) &dataPointer
                         << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
           } else {
               xassert (vval == dd);
               if (val == last) {
               } else if (val == last-1) {
                   shrinkTail(intptrslength, intpointers, npos);
               } else {
                   *val = npos;
               }
           }
       } else if (pchild) {
           if (child != pchild) {
               std::cout << "Warning  StoragePool.removePointer : pointer pool mismatch:  (**) "
                         << (void*) &dataPointer  << "  (*) " << (void*) dataPointer  << " of "
                         << "child(**):" << (void*) pchild->memory << " .. " << (void*) (pchild->memory+pchild->memlength)
                         << " child(*):" << (void*) child->memory << " .. " << (void*) (child->memory+child->memlength)
                         << "   parent:" << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
           }
           constcast(pchild)->removePointer(dataPointer);
       } else if (extpointers) {
           ExternalPtr dd = &dataPointer;
           ExternalPtr* last = extpointers+extptrslength;
           ExternalPtr* val = lower_bound(extpointers, last, dd, (ExternalPtr&)LNULL);
           ExternalPtr vval = *val;
           if (vval) {
               xassert (vval == dd);
               if (val == last) {
               } else if (val == last-1) {
                   shrinkTail(extptrslength, extpointers, (ExternalPtr) NULL);
               } else {
                   *val = NULL;
               }
           } else {
               std::cout << "Warning  StoragePool.removePointer : external poinrer already removed : " << (void*) &dataPointer
                         << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
           }
       } else {
           std::cout << "Warning  StoragePool.removePointer : not external poinrers but not in : " << (void*) &dataPointer
                     << " of " << (void*) memory << " .. " << (void*) (memory+memlength);
           if (child) {
               std::cout <<  " (though pointing to inside *:" << (void*) dataPointer << ")";
           }
           std::cout << std::endl;
       }
   }

   inline void removeAllExternalPointers() {
       delete extpointers;
       extptrslength = 0;
       extptrscapacity = 0;
   }

   inline void addChildPool(PtrToMe childPoolPointer) {
       xassert(ownerPool == this && contains(childPoolPointer));

       size_t dd = encodeDeltaPtr(memory, (uint8_t*)childPoolPointer);
       // to ensure it is ordered (binary search invariant)
       if (chplslength) {
           xassert(dd > childpools[chplslength-1]);
       }
       size_t pbufsz = getPtrBufSize(chplslength);
       if (chplscapacity < pbufsz) {
           extendBuffer(childpools, chplslength, chplscapacity, pbufsz);
       }
       childpools[chplslength++] = dd;
   }

   inline void removeChildPool(PtrToMe childPoolPointer) {
       xassert(ownerPool == this && contains(childPoolPointer));

       size_t dd = encodeDeltaPtr(memory, (uint8_t*)childPoolPointer);
       size_t* last = childpools+chplslength;
       size_t* val = lower_bound(childpools, last, dd, npos);
       xassert (*val == dd);
       if (val == last) {
       } else if (val == last-1) {
           shrinkTail(chplslength, childpools, npos);
       } else {
           *val = npos;
       }
   }

   inline void removeChildPool(size_t * val) {
       size_t* last = childpools+chplslength;

       if (val == last) {
       } else if (val == last-1) {
           shrinkTail(chplslength, childpools, npos);
       } else {
           *val = npos;
       }
   }

   void selfCheck() const {
#ifdef DEBUG
       xassert(__store_size == getStoreSize(sizeof(StoragePool)));
       xassert(__kind < ST_DELETED);
       xassert((__kind == ST_NONE) == (__parentVector == npos));
       xassert(ownerPool);
       xassert(bool(memory) == bool(memlength));
       xassert(bool(memcapacity) == bool(memlength));
       xassert(bool(intpointers) == bool(intptrslength));
       xassert(bool(intptrscapacity) == bool(intptrslength));
       xassert(bool(extpointers) == bool(extptrslength));
       xassert(bool(extptrscapacity) == bool(extptrslength));
       xassert(bool(childpools) == bool(chplslength));
       xassert(bool(chplscapacity) == bool(chplslength));

       PtrToMe lastm = NULL;
       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+chplslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           if (ptr) {
               xassert(contains(ptr));
               xassert(!ptr->isParentOf(*this));

               ptr->selfCheck();

               xassert(ptr > lastm);
               lastm = ptr;
           }
       }

#else
       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+chplslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
           if (ptr) {
               xassert(!ptr->isParentOf(*this));
           }
       }
#endif
   }

   Storeable::debugPrint;

   void debugPtr(std::ostream& os, ExternalPtr ptr) const {
       if (*ptr) {
           os<<" ";
           StoragePool const *p;
           if (contains(*ptr)) {
           } else if (ownerPool!=this) {
               if (!(p=ownerPool->findChild(*ptr))) {
                   os<<"out!";
               } else if (!findChild(*ptr)) {
                   if (p == ownerPool)
                       os<<"own!";
                   else
                       os<<"och!";
               } else {
                   os<<"chi!";
               }
           } else if (!findChild(*ptr)) {
               os<<"out!";
           } else {
               os<<"chi!";
           }
           os<<(void*)ptr<<":";
           os<<(void*)*ptr<<":";
           (*ptr)->debugPrint(os);
       } else {
           os<<" ";
           os<<(void*)ptr<<":NULL";
       }
   }

   void debugPrint(std::ostream& os, int indent = 0, char const *subtreeName = 0) const
   {
     if (indent > 100) {
         ind(os,indent)<< "...";
     } else {
         ind(os,indent) << "pool:"<<(void*)this;
#ifdef DEBUG
         os<<":"<<objectName.str;
#endif
         os<<":mem:"<<(void*)memory<<":"<<memlength;
         os<<":int:"<<(void*)intpointers<<":"<<intptrslength;
         os<<":ext:"<<(void*)extpointers<<":"<<extptrslength;
         os<<":chp:"<<(void*)childpools<<":"<<chplslength;
         if (getParent()) {
             os<<":par:"<<(void*)getParent();
         }
         if (this != ownerPool) {
             os<<":own:"<<(void*)ownerPool;
         }
         //os<<std::dec;

         os<<":{\n";
         size_t* chPoolsFrom = childpools;
         size_t* chPoolsTo = childpools+chplslength;
         for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
             PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, *chPoolsFrom);
             if (ptr) {
                 ptr->debugPrint(os, indent+2);
                 os<<"\n";
             }
         }

         if (memlength) {
             ind(os,indent)<<"  values: ";
             for (iterator it=begin(); it != -1; it++) {
                 Storeable *cur = *it;
                 switch (cur->__kind) {
                 case ST_NONE:
                     os<<" stack:"<<(void*)cur;
                     break;
                 case ST_VALUE:
                     os<<" "<<(void*)cur<<":";
                     cur->debugPrint(os);
                     break;
                 case ST_STORAGE_POOL:
                     os<<" pool:"<<(void*)cur;
                     break;
                 case ST_DELETED:
                     os<<" del:"<<(void*)cur;
                     break;
                 default:
                     break;
                 }
             }
             os<<"\n";
         }

         if (intptrslength) {
             ind(os,indent)<<"  intptrs:";
             size_t* intPointersFrom = intpointers;
             size_t* intPointersTo = intpointers+intptrslength;
             for (; intPointersFrom<intPointersTo; intPointersFrom++) {
                 ExternalPtr ptr = (ExternalPtr)decodeDeltaPtr(memory, *intPointersFrom);
                 if (ptr) {
                     debugPtr(os, ptr);
                 }
             }
             os<<"\n";
         }

         if (extptrslength) {
             ind(os,indent)<<"  extptrs:";
             ExternalPtr* extPointersFrom = extpointers;
             ExternalPtr* extPointersTo = extpointers+extptrslength;
             for (; extPointersFrom<extPointersTo; extPointersFrom++) {
                 ExternalPtr ptr = *extPointersFrom;
                 if (ptr) {
                     debugPtr(os, ptr);
                 }
             }
             os<<"\n";
         }

         ind(os,indent)<< "}"<<std::flush;
     }
   }

   inline iterator begin(DataPtr variablePtr = 0) const {
       if (!variablePtr) {
           variablePtr = (DataPtr) (memory+sizeof(void*));
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


inline Storeable::Storeable(DBG_INFO_FORMAL_FIRST  size_t size_of)
#ifdef DEBUG
    : objectName(objectName)  REG_CHILD_COMMA
#endif
#ifdef REG_CHILD
    NDEBUG_COLON __next(0)
#endif
{
#if debug_this
    if (__kind == ST_PARENT && __parentVector && __parentVector != npos) {
        StoragePool** the_first = (StoragePool**) decodeDeltaBackPtr((uint8_t*)this, __parentVector);
        StoragePool* pool = *the_first;
        if (pool->contains(this)) {
            std::cout<<"Bad allocation, use Storable(StoragePool &pool)" << std::endl;
        }
    }
#endif
#ifdef DEBUG
    lastObjName = objectName.str;
#endif
    __kind = ST_NONE;
    __parentVector = npos;
    __store_size  = getStoreSize(size_of);
}

/* new operator filled __pool and __store_size previously, we use passed argument to double check */
inline Storeable::Storeable(DBG_INFO_FORMAL_FIRST  StoragePool & pool)
#ifdef DEBUG
    : objectName(objectName)
#endif
{
#ifdef DEBUG
    lastObjName = objectName.str;
#endif
    if (pool.contains(this)) {
        xassert((__kind == ST_VALUE||__kind == ST_STORAGE_POOL) && getParent() == &pool);
    } else {
        __kind = ST_NONE;
        __parentVector = npos;
        __store_size  = 0;
    }
}

template<class ME>
inline Storeable::Storeable(DBG_INFO_FORMAL_FIRST  ME const & srcOrParent, bool childOfParent)
#ifdef DEBUG
    : objectName(objectName)
#endif
{
#ifdef DEBUG
    lastObjName = objectName.str;
#endif
    init(srcOrParent, sizeof(ME), childOfParent);
}

inline Storeable::Storeable(DBG_INFO_FORMAL_FIRST   Storeable const & srcOrParent, size_t size_of, bool childOfParent)
#ifdef DEBUG
    : objectName(objectName)
#endif
{
#ifdef DEBUG
    lastObjName = objectName.str;
#endif
    init(srcOrParent, size_of, childOfParent);
}

inline Storeable::~Storeable() {
    switch (__kind) {
    case ST_VALUE:
    case ST_STORAGE_POOL:
        constcast(getPoolRef()).freeParentItem(this);
        break;
    case ST_DELETED:
        x_assert_fail("Already deleted.", __FILE__, __LINE__);
        break;
    default:
        break;
    }
}

inline void Storeable::init(Storeable const & srcOrParent, size_t size_of, bool childOfParent) {
    if (childOfParent) {
        StoragePool const & srcPool = srcOrParent.getPoolRef();
        xassert(srcPool.contains(this));
        __kind = ST_VALUE;
        __parentVector = encodeDeltaPtr((uint8_t*)srcPool.memory, (uint8_t*)this);
        __store_size = getStoreSize(size_of);
        xassert(&srcPool == getParent());
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

#ifdef DEBUG
    uint8_t * bg = sizeof(objectName)+(uint8_t*)&objectName;
    uint8_t * th = (uint8_t*)this;
    std::ptrdiff_t d = bg-th;
    memcpy(bg, d+(uint8_t*)&src, size_of-d);
#else
    memcpy(this, &src, size_of);
#endif

    // transfer __parentVector (we keep parent)
    switch (__kind) {
    case ST_NONE:
        xassert(__parentVector == npos);
        break;
    case ST_VALUE:
    case ST_STORAGE_POOL:
    {
        assignSameParent(src);
        break;
    }
    /*case ST_CHILD:
    {
        StoragePool const * par = src.getParentRef().getRootPoolRef().findChild(this);
        __kind = ST_PARENT;
        __parentVector = encodeDeltaPtr((uint8_t*)par->memory, (uint8_t*)this);
        xassert(par == getParent());
        break;
    }*/
    default:
        x_assert_fail("Invalid kind.", __FILE__, __LINE__);
    }
}

inline void Storeable::assignSameParent(Storeable const & src) {
    StoragePool * srcPool = src.getParent();
    StoragePool const * par = srcPool->findChild(this);
    if (par) {
        __parentVector = encodeDeltaPtr((uint8_t*)par->memory, (uint8_t*)this);
        xassert(par == getParent());
    } else {
        std::cout << "Warning  Storeable.assign(" << getKind()
    #ifdef DEBUG
                  << " " << objectName.str
    #endif
                  << ")  copy to stack : "
                  << (void*) &src << " -> " << (void*) this << std::endl;
        __parentVector = npos;
        __kind = ST_NONE;
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
    void const * data;
    pool.allocParentItem(data, getStoreSize(size));
    return constcast(data);
}

inline void* Storeable::operator new[] (std::size_t size, StoragePool& pool) {
    void const * data;
    pool.allocParentItem(data, getStoreSize(size));
    return constcast(data);
}

inline void Storeable::operator delete (void* _ptr) {
    DataPtr ptr = (DataPtr) _ptr;
    switch (ptr->__kind) {
    case ST_NONE:
        free(_ptr);
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
    default:
        // Nothing to do here, everything is in ~Storeable
        break;
    }
}

inline void operator<<(std::ostream & os, Storeable const & st) {
    st.debugPrint(os);
}

}// namespace str


#endif // STORAGE_H

