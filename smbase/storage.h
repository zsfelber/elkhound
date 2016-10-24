#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <list>
#include <memory.h>
#include <malloc.h>
#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

#include "xassert.h"


struct __DbgStr {
    char const * str;
    explicit __DbgStr(char const * str) : str(str) {}
};

static struct __StoreAlreadyConstr {
} StoreAlreadyConstr;

class VoidList;
class VoidTailList;
class VoidNode;
class GrammarAnalysis;

#define DEBUG_MAX_IND 25

#define S1(x) #x
#define S2(x) S1(x)
#define __FILE_LINE__ __FILE__ " : " S2(__LINE__)

#ifdef DEBUG
#define DBG_INFO_FORMAL __DbgStr const objectName
#define DBG_INFO_FORMAL_FIRST __DbgStr const objectName,
#define DBG_INFO_ARG_FWD objectName
#define DBG_INFO_ARG_FWD_FIRST objectName,
#define DBG_INFO_ARG0 __DbgStr(__FILE_LINE__)
#define DBG_INFO_ARG0_SOLE (DBG_INFO_ARG0)
#define DBG_INFO_ARG0_FIRST __DbgStr(__FILE_LINE__),
#define DBG_INFO_FWD(a) a
#define DBG_INFO_FWD_COM(a) a,
#define DBG_INFO_FWD_PCOM(a) ,a
#else
#define DBG_INFO_FORMAL
#define DBG_INFO_FORMAL_FIRST
#define DBG_INFO_ARG_FWD
#define DBG_INFO_ARG_FWD_FIRST
#define DBG_INFO_ARG0
#define DBG_INFO_ARG0_SOLE
#define DBG_INFO_ARG0_FIRST
#define DBG_INFO_FWD(a)
#define DBG_INFO_FWD_COM(a)
#define DBG_INFO_FWD_PCOM(a)
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

#if (defined(_MSC_VER))
#define OPTL0 __pragma(optimize("gts", on))
#define OPTL
#define OPT inline
#elif (defined(__GNUC__))
#define OPTL0
#define OPTL __attribute__((optimize("O2")))
#define OPT inline OPTL
#else

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

template <typename P> OPT P* constcast(P const * p) {
    return const_cast<P*>(p);
}

template <typename P> OPT P& constcast(P const & p) {
    return const_cast<P&>(p);
}

#define STOREABLE_COPY_CON(classname) classname(classname const & src) : Storeable(src) {}
#define STOREABLE_COPY_CON2(classname,baseclass) classname(classname const & src) : baseclass(src) {}

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
OPT void memsetBuffer(T *buf, int32_t c, T n)
{
    for (uint32_t i = 0; i<n; i++, buf++) *buf = c;
}

template<typename T>
OPT void copyBuffer(T* src, size_t srclen, size_t srccap,
                       T*& dest, size_t &dstlen, size_t &dstcap) {
    xassert(srccap && !dest);
    dest = (T*)new uint8_t[srccap*sizeof(T)];//avoid using default constructor for class types
    if (!dest) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    dstlen = srclen;
    dstcap = srccap;
    memcpy(dest, src, srclen*sizeof(T));
}

template<typename T>
OPT void copyBuffer(T* src, size_t srclen, size_t srccap, T*& dest) {
    xassert(!dest);
    dest = (T*)new uint8_t[srccap*sizeof(T)];//avoid using default constructor for class types
    memcpy(dest, src, srclen*sizeof(T));
}

template<typename T>
OPT void extendBuffer(T*& buf, size_t size, size_t& capacity, size_t newcap) {
    xassert(newcap);
    T* old = buf;
    buf = (T*)new uint8_t[newcap*sizeof(T)];//avoid using default constructor for class types
    if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    if (old) {
        memcpy(buf, old, size*sizeof(T));
        delete[] old;
    }
    capacity = newcap;
}

template<typename T>
OPT void extendBuffer(T*& buf, size_t size, size_t newcap) {
    xassert(newcap);
    T* old = buf;
    buf = (T*)new uint8_t[newcap*sizeof(T)];//avoid using default constructor for class types
    if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    if (old) {
        memcpy(buf, old, size*sizeof(T));
        delete[] old;
    }
}

template<typename T>
OPT void extendBufferAndSet(T*& buf, size_t size, size_t newcap, uint8_t defaultByte) {
    xassert(newcap);
    T* old = buf;
    buf = (T*)new uint8_t[newcap*sizeof(T)];//avoid using default constructor for class types
    if (!buf) x_assert_fail("Memory allocation error.", __FILE__, __LINE__);
    if (old) {
        memcpy(buf, old, size*sizeof(T));
        delete[] old;
    }
    memset(buf+size, defaultByte, (newcap-size)*sizeof(T));
}

template<typename T>
OPT void removeBufferItem(T* buf, size_t& size, T* pos) {
    size--;
    // allow overlap
    memmove(pos, pos+1, (size-(pos-buf))*sizeof(T));
}
OPT void removeBufferItem(uint8_t* buf, size_t& size, uint8_t* pos, size_t size_of) {
    size-=size_of;
    // allow overlap
    memmove(pos, pos+size_of, size-(pos-buf));
}

template<typename T>
OPT void insertBufferItem(T* buf, size_t& size, T* pos) {
    // allow overlap
    memmove(pos+1, pos, (size-(pos-buf))*sizeof(T));
    size++;
}

OPT size_t getStoreSize(size_t size_of) {
    return ((size_of + STORE_BUF_ADDR_SZ - 1)>>STORE_BUF_VAR_SH)<<STORE_BUF_VAR_SH;
}

OPT size_t getMemBufSize(size_t memlength) {
    return ((memlength + STORE_BUF_SZ - 1)>>STORE_BUF_BITS)<<STORE_BUF_BITS;
}

OPT size_t getPtrBufSize(size_t ptrslength) {
    return ((ptrslength + STORE_BUF_PTR_SZ/*+1-1*/)>>STORE_BUF_PTR_BITS)<<STORE_BUF_PTR_BITS;
}

OPT size_t encodeDeltaPtr(uint8_t const * origin,
                             DBG_INFO_FWD_COM(uint8_t const * org_last)
                             uint8_t const * address) {
    if (!address || !origin) {
        return npos;
    } else {
       xassert(origin <= address);
       DBG_INFO_FWD(xassert(address < org_last);)
        return (size_t)(address - origin);
    }
}

OPT std::ptrdiff_t encodeSignedDeltaPtr(uint8_t const * origin,
                                           uint8_t const * address) {
    if (!address || !origin) {
        return 0;
    } else {
        return address - origin;
    }
}

OPT uint8_t const * decodeDeltaPtr(uint8_t const * origin,
                                      DBG_INFO_FWD_COM(uint8_t const * org_last)
                                      size_t delta) {
    if (delta == npos || !origin) {
        return NULL;
    } else {
        uint8_t const * address = origin + delta;
        xassert(origin <= address);
        DBG_INFO_FWD(xassert(address < org_last);)
        return address;
    }
}

/*OPT uint8_t const * decodeDeltaBackPtr(uint8_t const * origin,
                                          DBG_INFO_FWD_COM(uint8_t const * org_last)
                                          size_t delta) {
    return (delta == npos || !origin)  ? NULL : (origin - delta);
}*/

OPT uint8_t const * decodeSignedDeltaPtr(uint8_t const * origin,
                                            std::ptrdiff_t delta) {
    return (delta == 0 || !origin)  ? NULL : (origin + delta);
}

OPT std::ostream &ind(std::ostream &os, int indent)
{
  xassert(indent <= 110);
  char const *ib = "                                                                                                              ";
  os << "\n";
  os << ib+(110-indent);
  return os;
}

#define BI_NI \
    if (*middle == nullItem) {                                                   \
        IT_OR_PTR  _middle = middle;                                             \
        do {                                                                     \
            _middle++;                                                           \
        } while (*_middle == nullItem && _middle < last);                        \
                                                                                 \
        if (_middle == last) {                                                   \
            nothingToTheRight = true;                                            \
            _middle = middle;                                                    \
                                                                                 \
            do {                                                                 \
                _middle--;                                                       \
            } while (*_middle == nullItem && _middle >= first);                  \
                                                                                 \
            if (_middle < first) {                                               \
                result = last;                                                   \
                return false;                                                    \
            }                                                                    \
        }                                                                        \
                                                                                 \
        middle = _middle;                                                        \
    }                                                                            \

#define BI(bi_ni, cmp, rcmp) \
    while (first < last) {                                                       \
      std::ptrdiff_t half = (last-first) >> 1;                                   \
      IT_OR_PTR  middle = first + half;                                          \
      bool nothingToTheRight = false;                                            \
                                                                                 \
      bi_ni                                                                      \
                                                                                 \
      if (cmp) {                                                                 \
          first = middle+1;                                                      \
          if (nothingToTheRight) {                                               \
              result = first;                                                    \
              return false;                                                      \
          }                                                                      \
      } else if (rcmp) {                                                         \
          /* exclusive index  (no -1)*/                                          \
          last = middle;                                                         \
      } else {                                                                   \
          result = middle;                                                       \
          return true;                                                           \
      }                                                                          \
    }                                                                            \
    result = first;                                                              \
    return false;                                                                \

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
template<typename IT_OR_PTR, typename T>
OPT bool binary_insert(IT_OR_PTR first, IT_OR_PTR last, T const & val, T const & nullItem, IT_OR_PTR& result) {
   BI(BI_NI, *middle < val, val < *middle)
}
template<typename IT_OR_PTR, typename T>
OPT bool binary_insert(IT_OR_PTR first, IT_OR_PTR last, T const & val, IT_OR_PTR& result) {
   BI(/**/, *middle < val, val < *middle)
}
template<typename IT_OR_PTR, typename T,typename CMP>
OPT bool binary_insert(IT_OR_PTR first, IT_OR_PTR last, T const & val, T const & nullItem, IT_OR_PTR& result, CMP cmp) {
   BI(BI_NI, cmp(*middle, val), cmp(val, *middle))
}
template<typename IT_OR_PTR, typename T,typename CMP>
OPT bool binary_insert(IT_OR_PTR first, IT_OR_PTR last, T const & val, IT_OR_PTR& result, CMP cmp) {
   BI(/**/, cmp(*middle, val), cmp(val, *middle))
}
template<typename IT_OR_PTR, typename T,typename CMP>
OPT bool binary_insert(IT_OR_PTR first, IT_OR_PTR last, T const & val, IT_OR_PTR& result, CMP cmp1, CMP cmp2) {
   BI(/**/, cmp1(*middle, val), cmp2(val, *middle))
}

template<typename T>
OPT void shrinkTail(size_t& length, T* vect, T nullitm) {
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
friend class ::GrammarAnalysis;

DBG_INFO_FWD(__DbgStr objectName;)

    uint8_t __kind;
    StoragePool const * __parent;
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

    OPT void clear(size_t size_of) {
#ifdef DEBUG
        uint8_t * bg = sizeof(objectName)+(uint8_t*)&objectName;
        uint8_t * th = (uint8_t*)this;
        memset(bg, 0, size_of-(bg-th));
#else
        memset(this, 0, size_of);
#endif
    }


    Storeable(Storeable & disable);

    void init(Storeable const & srcOrParent, size_t size_of, bool childOfParent, bool isPool);

    void removeInParent();

public:

    enum __Kind {
        ST_NONE = 0,
        ST_VALUE = 1,
        ST_STORAGE_POOL = 2,
        ST_PARENT = ST_VALUE | ST_STORAGE_POOL,
        ST_REMAINDER = 4,
        ST_IN_POOL = ST_VALUE | ST_STORAGE_POOL | ST_REMAINDER,
        ST_CHILD = 8,
        ST_VALUE_CHILD = ST_VALUE | ST_CHILD,
        ST_POOL_CHILD = ST_STORAGE_POOL | ST_CHILD,
        ST_DELETED = 16,
        ST_VALUE_DELETED = ST_VALUE | ST_DELETED,
        ST_POOL_DELETED = ST_STORAGE_POOL | ST_DELETED,
        ST_REMAIN_DELETED = ST_REMAINDER | ST_DELETED,
        ST_CHILD_DELETED = ST_CHILD | ST_DELETED,
        ST_VALUE_CHILD_DELETED = ST_VALUE | ST_CHILD | ST_DELETED,
        ST_POOL_CHILD_DELETED = ST_STORAGE_POOL | ST_CHILD | ST_DELETED,
    };

   typedef StoragePool* PtrToMe;
   typedef StoragePool const * CPtrToMe;
   typedef Storeable* DataPtr;
   typedef DataPtr* ExternalPtr;


   static void* operator new (std::size_t size);
   static void* operator new (std::size_t size, void* ptr);
   static void* operator new (std::size_t size, const std::nothrow_t& nothrow_value);
   static void* operator new[] (std::size_t size);
   static void* operator new[] (std::size_t size, void* ptr);
   static void* operator new[] (std::size_t size, const std::nothrow_t& nothrow_value);

   static void* operator new (std::size_t size, StoragePool const & pool);
   static void* operator new[] (std::size_t size, StoragePool const & pool);

   static void operator delete (void* ptr);
   static void operator delete (void* ptr, const std::nothrow_t& nothrow_constant);
   static void operator delete (void* ptr, void* voidptr2);
   static void operator delete (void* ptr, size_t size);
   static void operator delete[] (void* ptr);
   static void operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant);
   static void operator delete[] (void* ptr, void* voidptr2);
   static void operator delete[] (void* ptr, size_t size);


   Storeable(DBG_INFO_FORMAL_FIRST  __StoreAlreadyConstr nothing);

   Storeable(DBG_INFO_FORMAL_FIRST  size_t size_of = 0);

   /* new operator filled __pool and __store_size previously, we use passed argument to double check */
   Storeable(DBG_INFO_FORMAL_FIRST  StoragePool const & pool, bool isPool = false, size_t size_of = 0);

   /**
    * src: source object argument of copy constructor
    * default and very effective implementation
    * should be additionally invoked for each Storeable class field too
    * @brief Storeable::Storeable
    * @param parent
    */
   template<class ME>
   Storeable(DBG_INFO_FORMAL_FIRST  ME const & srcOrParent, bool childOfParent, bool isPool = false);

   /**
    * this object is a non-pointer class field of the pointer/non-pointer stored variable, parent
    * @brief Storeable::Storeable
    * @param parent
    */
   Storeable(DBG_INFO_FORMAL_FIRST  Storeable const & srcOrParent, size_t size_of, bool childOfParent, bool isPool = false);

   virtual ~Storeable();

   template<class ME>
   void assign(ME const & srcOrParent);

   void assign(Storeable const & srcOrParent, size_t size_of);

   void assignSameParent(Storeable const & srcOrParent, bool isPool = false, StoragePool const * oldPool = NULL);

   void assignParent(StoragePool const * srcPool, bool isPool = false, StoragePool const * oldPool = NULL);

   OPT __Kind getKind() const {
       return (__Kind)__kind;
   }

   OPT StoragePool const & getParentRef() const {
       return NN(__parent);
   }

   OPT StoragePool const * getParent() const {
       return __parent;
   }

   OPT StoragePool const * getPool() const {
       if (__kind & ST_STORAGE_POOL) {
           return (StoragePool*)this;
       } else if (__kind & ST_VALUE) {
           return __parent;
       } else if (__kind & ST_REMAINDER) {
           return NULL;
       } else {
           return asPool();
       }
   }

   OPT StoragePool const & getPoolRef() const {
       return NN(getPool());
   }

   OPT virtual StoragePool * asPool() const {
       return NULL;
   }

   OPT int debugPrint(int indent = 0) const
   {
       debugPrint(std::cout, indent);
       return 0;
   }

   virtual OPT void debugPrint(std::ostream& os, int indent = 0, char const *subtreeName = 0) const
   {
   }

   OPT bool isDeleted() const {
       return __kind & ST_DELETED;
   }

   void debugItm(std::ostream& os, int indent) const;

#ifdef DEBUG
   OPT char const * getObjectName() const {
       return objectName.str;
   }
#endif
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
    size_t* intvariables;
    size_t intvarslength, intvarscapacity;
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

       OPT iterator() {}
       OPT iterator(StoragePool const & pool, uint8_t const * _variablePtr) : index(0), variablePtr(_variablePtr) {
           if (!variablePtr) {
               variablePtr = pool.memory;
           }
           memend = pool.memory + pool.memlength;
           check();
       }

       OPT void check() {
           if (variablePtr >= memend) {
               index = -1;
           } else {
               index++;
           }
       }

     public:
       OPT void pp() {
           if (index>=0) {
               size_t st = (*this)->__store_size;
               xassert(st);
               variablePtr += st;
               check();
           } else {
               x_assert_fail("Overindexed Storeable iterator.", __FILE__, __LINE__);
           }
       }

       OPT iterator& operator++() {
           pp();
           return *this;
       }
       OPT iterator& operator++(int postfix) {
           pp();
           return *this;
       }

       OPT bool operator==(int arg) {
           return index == arg;
       }

       OPT bool operator!=(int arg) {
           return index != arg;
       }

       OPT DataPtr operator*() {
           return (DataPtr)variablePtr;
       }

       OPT DataPtr operator->() {
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

       OPT child_iterator(DataPtr variablePtr) : index(0), variablePtr(variablePtr) {
           variableMemFirst = (uint8_t*)variablePtr;
           memend = variableMemFirst + __store_size;
           check();
       }

       OPT void check() {
           if (variablePtr > memend) {
               x_assert_fail("Overindexed StoragePool.", __FILE__, __LINE__);
           } else if (variablePtr->__next) {
               index++;
           } else {
               index = -1;
           }
       }

     public:
       OPT void pp() {
           if (index>=0) {
               variablePtr = (DataPtr) (variableMemFirst + variablePtr->__next);
               check();
           } else {
               x_assert_fail("Overindexed Storeable iterator.", __FILE__, __LINE__);
           }
       }

       OPT iterator& operator++() {
           pp();
           return *this;
       }
       OPT iterator& operator++(int postfix) {
           pp();
           return *this;
       }

       OPT void operator==(int arg) {
           return index == arg;
       }

       OPT void operator!=(int arg) {
           return index != arg;
       }

       OPT Storeable& operator*() {
           return *variablePtr;
       }

       OPT DataPtr operator->() {
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
       Cp_TmpDuplicate,
   };

private:

   size_t* childpools;
   size_t chplslength, chplscapacity;

   StoragePool * ownerPool;

   OPT void movePointers(StoragePool const & oldmem) {
       xassert(ownerPool == this);

       moveInternalPointers(oldmem);
       moveExternalPointers(oldmem);
   }

   void moveInternalPointers(StoragePool const & oldmem) {
       std::ptrdiff_t d = memory - oldmem.memory;

       size_t* intPointersFrom = intpointers;
       size_t* intPointersTo = intpointers+intptrslength;
       for (; intPointersFrom<intPointersTo; intPointersFrom++) {
           ExternalPtr ptr = (ExternalPtr)decodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) *intPointersFrom);
           if (ptr) {
               movePointer(oldmem, *ptr, d);
           }
       }
   }

   /*void moveInternalPointers(StoragePool const & oldmem, size_t* intPointersFrom, size_t* intPointersTo) {
       std::ptrdiff_t d = memory - oldmem.memory;

       for (; intPointersFrom<intPointersTo; intPointersFrom++) {
           ExternalPtr ptr = (ExternalPtr)decodeDeltaPtr(memory, *intPointersFrom);
           if (ptr) {
               movePointer(oldmem, *ptr, d);
           }
       }
   }*/

   void moveExternalPointers(StoragePool const & oldmem) {
       std::ptrdiff_t d = memory - oldmem.memory;

       ExternalPtr* extPointersFrom = extpointers;
       ExternalPtr* extPointersTo = extpointers+extptrslength;
       for (; extPointersFrom<extPointersTo; extPointersFrom++) {
           ExternalPtr ptr = *extPointersFrom;
           if (ptr) {
               movePointer(oldmem, *ptr, d);
           }
       }
   }

   OPT void convertDeltas(size_t* buf, size_t* last, uint8_t const * oldOrigin  DBG_INFO_FWD_PCOM(uint8_t const * oldLast)) {
       for (; buf<last; buf++) {
           uint8_t const * ptr = decodeDeltaPtr(oldOrigin, DBG_INFO_FWD_COM(oldLast) *buf);
           if (ptr) {
               *buf = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) ptr);
           }
       }
   }

   template <typename T>
   OPT void sort(T * buf, size_t buflength, T * src_buf, size_t src_buflength) {
       // should remain sorted
       if (buflength && src_buflength &&
               buf[buflength-1] > src_buf[0]) {
           std::sort(buf, buf+buflength+src_buflength);
       }
   }

   bool movePointer(StoragePool const & oldmem, DataPtr& variable, std::ptrdiff_t d) const {
       xassert(ownerPool == this || ownerPool == &oldmem);

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
       } else {
           return false;
       }
   }


   OPT void moveFrom(StoragePool const & parent, DataPtr & child, uint8_t * origin = NULL) {

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
                    StoragePool*  chpool = (StoragePool*) decodeDeltaPtr(pool->memory, DBG_INFO_FWD_COM(pool->memory+pool->memlength) pool->childpools[ch.index]);
                    pool = chpool;
                }
           }

           xassert(it == chch.end());
           if (!origin) {
               xassert(pool == this);
               origin = pool->memory;
           }

           d = origin - ch.pool->memory;
       } else {
           xassert(parent.contains(child));

           if (!origin) {
               origin = pool->memory;
           }
           d = origin - parent.memory;
       }

       uint8_t* variable_addr = (uint8_t*)child;
       variable_addr += d;
       child = (DataPtr)variable_addr;

       StoragePool const * pool0 = findChild(variable_addr);
       xassert(pool0 == pool);
   }

   void fixChildren(StoragePool const * target, StoragePool const & source) {
       xassert(ownerPool == this);
       std::ptrdiff_t d = memory - source.memory;
       //if (d) {
           fixChildren(target, source, d, true);
       //}
   }

   void fixChildren(StoragePool const * target, StoragePool const & source,
                    std::ptrdiff_t d, bool req) {
       fixChildValues(target, source, d, req);
       fixChildPools(target, source, d, req);

       for (iterator it = begin(), src_it = source.begin(); it != -1; it++, src_it++) {
           xassert((it==-1)==(src_it==-1));
           Storeable *cur = *it;
           Storeable *src_cur = *src_it;
           xassert(!(cur->__kind & ST_CHILD));
           if (cur->__kind & ST_DELETED) {

           } else if (cur->__kind & ST_VALUE) {
               fixChild<Storeable>(cur, src_cur, target, source, d, req);
           }
           //else if (cur->__kind & ST_STORAGE_POOL) {
               // already processed in fixChildPools
               //fixChild<StoragePool>((StoragePool*)cur, (StoragePool*)src_cur, source, d);
           //}
       }
   }

   OPT void fixChildValues(StoragePool const * target, StoragePool const & source,
                              std::ptrdiff_t d, bool req = true) {

       size_t* tar_varsFrom = intvariables;
       size_t* tar_varsTo = intvariables+intvarslength;
       size_t* src_varsFrom = source.intvariables;
       size_t* src_varsTo = source.intvariables+source.intvarslength;

       fixChildren<Storeable>(tar_varsFrom, tar_varsTo, src_varsFrom, src_varsTo,
                      target, source, d, req);
   }

   OPT void fixChildPools(StoragePool const * target, StoragePool const & source,
                             std::ptrdiff_t d, bool req = true) {

       size_t* tar_chPoolsFrom = childpools;
       size_t* tar_chPoolsTo = childpools+chplslength;
       size_t* src_chPoolsFrom = source.childpools;
       size_t* src_chPoolsTo = source.childpools+source.chplslength;

       fixChildren<StoragePool>(tar_chPoolsFrom, tar_chPoolsTo, src_chPoolsFrom, src_chPoolsTo,
                      target, source, d, req);
   }

   template <class V>
   OPT void fixChildren(size_t* tar_chFrom, size_t* tar_chTo,
                       size_t* src_chFrom, size_t* src_chTo,
                       StoragePool const * target, StoragePool const & source,
                           std::ptrdiff_t d, bool req) {
       xassert( !source.memory || memory == (d+source.memory));
       xassert(tar_chTo-tar_chFrom == src_chTo-src_chFrom);

       for (; tar_chFrom<tar_chTo; tar_chFrom++, src_chFrom++) {
           V* ptr = (V*)decodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) *tar_chFrom);
           V* src_ptr = (V*)decodeDeltaPtr(source.memory, DBG_INFO_FWD_COM(source.memory+source.memlength) *src_chFrom);

           fixChild(ptr, src_ptr, target, source, d, req);
       }
   }

   template <class V>
   OPT void fixChild(V* ptr, V* src_ptr,
                        StoragePool const * target, StoragePool const & source,
                        std::ptrdiff_t d, bool req) {

       xassert(bool(ptr)==bool(src_ptr));
       if (ptr) {
           xassert(ptr->__kind==src_ptr->__kind);
           xassert(ptr->__kind & ST_PARENT);
           xassert(!(ptr->__kind & ST_DELETED));
           xassert( (d+(uint8_t*)src_ptr) == (uint8_t*)ptr);
           xassert( source.contains(src_ptr) );
           xassert(src_ptr->__parent == &source);
           xassert(ptr->__parent == &source);

           fixPoolPointer(target, ptr, src_ptr);
           if (req) {
               fixPoolPointerChildren(target, ptr, src_ptr);
           }
       }
   }

   OPT void fixPoolPointer(StoragePool const * target, Storeable * ptr, Storeable const * src_ptr) {
       xassert( contains(ptr) );

       ptr->__parent = target;
   }

   OPT void fixPoolPointer(StoragePool const * target, StoragePool * ptr, StoragePool const * src_ptr) {
       fixPoolPointer(target, (Storeable*) ptr, src_ptr);

       xassert( src_ptr->ownerPool == ptr->ownerPool );

       if (ptr->ownerPool == src_ptr) {
           ptr->ownerPool = this;
       } else {
          xassert(ptr->ownerPool == this || ptr->ownerPool->isParentOf(*this));
       }
   }

   OPT void fixPoolPointerChildren(StoragePool const * target, Storeable * ptr, Storeable const * src_ptr) {

   }

   OPT void fixPoolPointerChildren(StoragePool const * target, StoragePool * ptr, StoragePool const * src_ptr) {
       ptr->fixChildren(ptr, *src_ptr);
   }

   void allocParentItem(void const *& _data, size_t store_size) {
       xassert(ownerPool == this);
       /*if (ownerPool != this) {
           ownerPool->allocParentItem(_data, store_size);
           return;
       }*/

       size_t oldmemlength, newmemlength, bufsz;

      // if hollow >50% :
       if (deleted_vars) {
           size_t chkmemlen = memlength;
           if (first_del_var != npos) {
              chkmemlen -= first_del_var;
           }
           chkmemlen>>=1;
          if (deleted_vars > chkmemlen) {
              iterator first;
              size_t continous = 0;
              uint8_t const * da = decodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) first_del_var);
              for (iterator it(*this, da); it != -1; it++) {
                  Storeable *cur = *it;
                  if (cur->__kind & ST_DELETED) {
                      if (!continous) {
                          first = it;
                      }
                      size_t cur_stos = cur->__store_size;
                      size_t kontinous = continous + cur_stos;
                      if (kontinous >= store_size) {
                          xassert(first.variablePtr >= da);
                          if (continous) {
                              it = first;
                              cur = *it;
                          }
                          _data = cur;
                          deleted_vars -= store_size;

                          size_t store_size_remains = kontinous - store_size;

                          cur->__store_size = store_size;

                          it++;
                          if (store_size_remains) {
                              xassert(it != -1);
                              cur = *it;
                              cur->__store_size = store_size_remains;
                              cur->__kind = ST_REMAINDER | ST_DELETED;
                          }

                          if (first.variablePtr == da) {
                              first_del_var = npos;
                              if (deleted_vars) {
                                  for (; it != -1; it++) {
                                      if (it->__kind & ST_DELETED) {
                                          first_del_var = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) it.variablePtr);
                                          break;
                                      }
                                  }
                              }
                          }
                          goto ok;
                      } else {
                          continous = kontinous;
                      }
                  } else {
                      continous = 0;
                  }
              }
          }
      }

      oldmemlength = memlength;
      newmemlength = oldmemlength + store_size;
      if (!oldmemlength) {
            bufsz = getMemBufSize(newmemlength);
            memory = new uint8_t[memcapacity = bufsz];
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
      data->__parent = this;
      data->__store_size = store_size;
   }


   void reassign(StoragePool const & src) {
       if (ownerPool == &src) {

           StoragePool const * p = __parent;
           clear();
           ownerPool = this;
           __parent = p;
           if (src.memory) copyBuffer(src.memory, src.memlength, src.memcapacity, memory, memlength, memcapacity);
           if (src.intvariables) copyBuffer(src.intvariables, src.intvarslength, src.intvarscapacity, intvariables, intvarslength, intvarscapacity);
           if (src.intpointers) copyBuffer(src.intpointers, src.intptrslength, src.intptrscapacity, intpointers, intptrslength, intptrscapacity);
           if (src.childpools) copyBuffer(src.childpools, src.chplslength, src.chplscapacity, childpools, chplslength, chplscapacity);
           extptrslength=0;
           extptrscapacity=0;
           extpointers = NULL;

           fixChildren(this, src);
           moveInternalPointers(src);

           removeChildPool(&src);
           removeChildPool(this);

       } else if (ownerPool == this) {

           delChildPools();

           if (memcapacity<src.memcapacity) {
               memcapacity = src.memcapacity;
               delete[] memory;
               memory = new uint8_t[memcapacity];
           }
           if (intvarscapacity<src.intvarscapacity) {
               intvarscapacity = src.intvarscapacity;
               delete[] intvariables;
               intvariables = new size_t[intvarscapacity];
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
           memcpy(intvariables, src.intvariables, sizeof(size_t)*(intvarslength = src.intvarslength));
           memcpy(intpointers, src.intpointers, sizeof(size_t)*(intptrslength = src.intptrslength));
           memcpy(childpools, src.childpools, sizeof(size_t)*(chplslength = src.chplslength));

           fixChildren(this, src);
           moveInternalPointers(src);

           removeChildPool(&src);
           removeChildPool(this);
       } else {
           x_assert_fail("reassign ownerPool mismatch", __FILE__, __LINE__);
       }

   }

   OPT bool isParentOf(Storeable const & chpool) const {
       StoragePool const * par = chpool.__parent;
       if (par == this) {
           return true;
       } else if (par) {
           return isParentOf(*par);
       } else {
           return false;
       }
   }

   OPT StoragePool * getRootPool() const {
       if (__kind & ST_STORAGE_POOL) {
           return __parent->getRootPool();
       } else {
           xassert(!(__kind & ST_VALUE));
           return constcast(this);
       }
   }

   OPT StoragePool & getRootPoolRef() const {
       return NN(getRootPool());
   }

   OPT StoragePool * asPool() const {
       return constcast(this);
   }

   template <typename T>
   OPT void addValue(T*& vars, size_t &length, size_t &capacity, T dd, T nullitm) {
       // to ensure it is ordered (binary search invariant)
       T *val;
       size_t pbufsz = getPtrBufSize(length);
       if (capacity < pbufsz) {
           extendBuffer(vars, length, capacity, pbufsz);
       }

       T* last = vars + length;

       if (length) {
           if (dd < vars[length-1]) {
               binary_insert(vars, last, dd, nullitm, val);
               if (val != last && *val == dd) {
                   std::cout << "Warning  StoragePool.addValue : variable already inserted : " << (void*) dd
                             << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
                   return;
               }
               insertBufferItem(vars, length, val);
           } else {
               val = last;
               length++;
           }
       } else {
           val = vars;
           length++;
       }
       *val = dd;
   }

   template <typename T>
   OPT void removeValue(T* vars, size_t &length, T dd, T nullitm) {

       T* last = vars + length;
       T* val;
       if (vars)
           binary_insert(vars, last, dd, nullitm, val);
       else
           val = last;
       if (val == last) {
       } else {
           T vval = *val;
           if (vval == nullitm) {
               std::cout << "Warning  StoragePool.removeValue : variable already removed : " << (void*) dd
                         << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << std::endl;
           } else if (val == last-1) {
               if (vval == dd) {
                   shrinkTail(length, vars, nullitm);
               } else {
                   std::cout << "Warning  StoragePool.removeValue : variable not found : " << (void*) dd
                             << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << "  next:"<< vval << std::endl;
               }
           } else {
               if (vval == dd) {
                   *val = nullitm;
               } else {
                   std::cout << "Warning  StoragePool.removeValue : variable not found : " << (void*) dd
                             << " of " << (void*) memory << " .. " << (void*) (memory+memlength) << "  next:"<< vval << std::endl;
               }
           }
       }
   }


   void addVariable(DataPtr dataVariable) {
       xassert(ownerPool == this && dataVariable && (dataVariable->__kind & ST_VALUE) && !(dataVariable->__kind & ST_CHILD_DELETED));
       dataVariable->__kind |= ST_CHILD;
       /*if (ownerPool != this) {
           ownerPool->addVariable(dataVariable);
           return;
       }*/

       StoragePool const * child = findChild(dataVariable);
       xassert(child == this && dataVariable->__parent == this);

       size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)dataVariable);
       addValue(intvariables, intvarslength, intvarscapacity, dd, npos);
   }

   void removeVariable(DataPtr dataVariable) {
       xassert(ownerPool == this && dataVariable && dataVariable->__kind & ST_VALUE);
       /*if (ownerPool != this) {
           ownerPool->removeVariable(dataVariable);
           return;
       }*/

       StoragePool const * child = findChild(dataVariable);
       xassert(child == this && dataVariable->__parent == this);

       size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)dataVariable);

       if (dataVariable->__kind & ST_CHILD) {
           removeValue(intvariables, intvarslength, dd, npos);
       } else {
           if (dd < first_del_var) {
               first_del_var = dd;
           }
           deleted_vars += dataVariable->__store_size;
       }
   }




   /*...................................................................................................
    * public:
    * ..................................................................................................
    */


public:

   StoragePool(DBG_INFO_FORMAL_FIRST  __StoreAlreadyConstr nothing) : Storeable(DBG_INFO_ARG_FWD_FIRST  nothing) {

   }

   StoragePool(DBG_INFO_FORMAL) : Storeable(DBG_INFO_ARG_FWD) {
       clear();
       __store_size = STORAGE_POOL_SIZE;
   }

   StoragePool(DBG_INFO_FORMAL_FIRST Storeable const & srcOrParent, bool childOfParent, CopyMode copyMode=Cp_All) :
       Storeable(DBG_INFO_ARG_FWD_FIRST srcOrParent, sizeof(StoragePool), childOfParent, true) {

       xassert(!isParentOf(srcOrParent));
       xassert(__store_size == STORAGE_POOL_SIZE && !(__kind & ST_REMAIN_DELETED));

       if (childOfParent) {
           memset(((uint8_t*)this)+sizeof(Storeable), 0, sizeof(StoragePool)-sizeof(Storeable));
           xassert((__kind & ST_PARENT) && copyMode == Cp_All);
           __kind = ST_STORAGE_POOL;
           first_del_var = npos;
           ownerPool = this;
       } else {
           StoragePool & srcOrParentPool = (StoragePool &) srcOrParent;
           ownerPool = &srcOrParentPool;
           assigned(srcOrParentPool, copyMode);
       }
   }

   virtual ~StoragePool() {
       if (ownerPool != this) {
           removeInParent();
           clear();
       } else {
           del();
       }
   }

   OPT void assign(StoragePool const & src, CopyMode copyMode=Cp_All) {
       xassert(!isParentOf(src));
       xassert(!src.isParentOf(*this));

#ifdef DEBUG
       uint8_t * bg = sizeof(objectName)+(uint8_t*)&objectName;
       uint8_t * th = (uint8_t*)this;
       std::ptrdiff_t d = bg-th;
       memcpy(bg, d+(uint8_t*)&src, sizeof(StoragePool)-d);
#else
       memcpy(this, &src, sizeof(StoragePool));
#endif
       assigned(src, copyMode);
   }

   OPT void assigned(StoragePool const & src, CopyMode copyMode=Cp_All) {

       switch (copyMode) {
       case Cp_TmpDuplicate:
       {
           ownerPool = (StoragePool*) &src;

           removeChildPool(&src);
           removeChildPool(this);

           break;
       }
       case Cp_Move:
       {
           xassert(__parent == src.__parent);

           ownerPool = this;

           removeChildPool(&src);
           removeChildPool(this);

           fixChildren(this, src, 0, false);

           constcast(src).clear();

           break;
       }
       case Cp_All:
       {
           size_t bufsz = getMemBufSize(memlength);
           xassert(bufsz==memcapacity);

           reassign(src);
           break;
       }
       default:
           x_assert_fail("Wrong kind.", __FILE__, __LINE__);
           break;
       }
   }

   OPT void removeInParent() {
       StoragePool * pool = constcast(__parent);
       if (pool) {
           pool->removeChildPool(this);
       }
   }

   OPT void del() {
       xassert(ownerPool == this);
       if (__kind & ST_DELETED) {
           x_assert_fail("Already deleted.", __FILE__, __LINE__);
       }

       delChildPools();

       if (memory) delete[] memory;
       if (intvariables) delete[] intvariables;
       if (intpointers) delete[] intpointers;
       if (extpointers) delete[] extpointers;
       if (childpools) delete[] childpools;

       removeInParent();
       clear();

       __kind |= ST_DELETED;
   }

   OPT void delChildPools() {
       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+chplslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) *chPoolsFrom);
           if (ptr && ptr->ownerPool == ptr) {
               ptr->del();
           }
       }
   }

   OPT void clear() {
       __Kind k = getKind();
#ifdef DEBUG
       uint8_t * bg = sizeof(objectName)+(uint8_t*)&objectName;
       uint8_t * th = (uint8_t*)this;
       memset(bg, 0, sizeof(StoragePool)-(bg-th));
#else
       memset(this, 0, sizeof(StoragePool));
#endif
       __kind = k;
       __parent = NULL;
        first_del_var = npos;
        ownerPool = this;
        __store_size = STORAGE_POOL_SIZE;
   }

   StoragePool& operator= (StoragePool const &src);

   OPT StoragePool& operator+= (StoragePool const &src) {
       StoragePool childView DBG_INFO_ARG0_SOLE;
       append(src, childView);
       return *this;
   }

   void append(StoragePool const &src, StoragePool &childView, ExternalPtr* convertExtPointersFrom = NULL, ExternalPtr* convertExtPointersTo = NULL) {
       xassert (ownerPool == this);

       size_t c;

       if (memcapacity<(c=getMemBufSize(memlength+src.memlength))) {
           extendBuffer(memory, memlength, c);
           memcapacity = c;
       }
       if (intvarscapacity<(c=getPtrBufSize(intvarslength+src.intvarslength))) {
           extendBuffer(intvariables, intvarslength, c);
           intvarscapacity = c;
       }
       if (intptrscapacity<(c=getPtrBufSize(intptrslength+src.intptrslength))) {
           extendBuffer(intpointers, intptrslength, c);
           intptrscapacity = c;
       }
       if (chplscapacity<(c=getPtrBufSize(chplslength+src.chplslength))) {
           extendBuffer(childpools, chplslength, c);
           chplscapacity = c;
       }

       childView.clear();
       childView.memory = memory + memlength;
       childView.memlength = src.memlength;
       childView.memcapacity = src.memcapacity;
       childView.intvariables = intvariables + intvarslength;
       childView.intvarslength = src.intvarslength;
       childView.intvarscapacity = src.intvarscapacity;
       childView.intpointers = intpointers + intptrslength;
       childView.intptrslength = src.intptrslength;
       childView.intptrscapacity = src.intptrscapacity;
       childView.childpools = childpools + chplslength;
       childView.chplslength = src.chplslength;
       childView.chplscapacity = src.chplscapacity;

       memcpy(childView.memory, src.memory, src.memlength);
       memcpy(childView.intvariables, src.intvariables, sizeof(size_t)*src.intvarslength);
       memcpy(childView.intpointers, src.intpointers, sizeof(size_t)*src.intptrslength);
       memcpy(childView.childpools, src.childpools, sizeof(size_t)*src.chplslength);

       //size_t memlength0 = memlength;
       memlength += src.memlength;

       childView.moveInternalPointers(src);

       convertDeltas(childView.intvariables, childView.intvariables+childView.intvarslength, childView.memory  DBG_INFO_FWD_PCOM(childView.memory+childView.memlength) );
       convertDeltas(childView.intpointers, childView.intpointers+childView.intptrslength, childView.memory  DBG_INFO_FWD_PCOM(childView.memory+childView.memlength));
       convertDeltas(childView.childpools, childView.childpools+childView.chplslength, childView.memory  DBG_INFO_FWD_PCOM(childView.memory+childView.memlength));

       if (convertExtPointersFrom) {
           convertExternalPointers(src, convertExtPointersFrom, convertExtPointersTo);
       }

       childView.fixChildren(this, src);
       childView.ownerPool = this;

       // These should remain sorted
       sort(intvariables, intvarslength, childView.intvariables, childView.intvarslength);
       sort(intpointers, intptrslength, childView.intpointers, childView.intptrslength);
       sort(childpools, chplslength, childView.childpools, childView.chplslength);

       intvarslength += src.intvarslength;
       intptrslength += src.intptrslength;
       chplslength += src.chplslength;

       removeChildPool(&src);
       removeChildPool(this);
   }

   OPT void swap(StoragePool * pool) {
       uint8_t buf[sizeof(SwapVars)];
       uint8_t *_this = sizeof(Storeable) + (uint8_t*) this;
       uint8_t *_pool = sizeof(Storeable) + (uint8_t*) pool;

       memcpy(buf, _this, sizeof(SwapVars));
       memcpy(_this, _pool, sizeof(SwapVars));
       memcpy(_pool, buf, sizeof(SwapVars));

   }

   void convertExternalPointers(StoragePool const &src, ExternalPtr* convertExtPointersFrom, ExternalPtr* convertExtPointersTo) {
       xassert(convertExtPointersFrom <= convertExtPointersTo);

       size_t plen = convertExtPointersTo - convertExtPointersFrom;
       size_t c;
       if (extptrscapacity<(c=getPtrBufSize(extptrslength+plen))) {
           extendBuffer(extpointers, extptrslength, c);
           extptrscapacity = c;
       }

       ExternalPtr* extPointersFrom = extpointers + extptrslength;
       ExternalPtr* extPointersTo = extPointersFrom + plen;
       std::ptrdiff_t d = memory - src.memory;
       for (; extPointersFrom<extPointersTo; extPointersFrom++, convertExtPointersFrom++) {
           ExternalPtr srcPtr = *convertExtPointersFrom;
           if (srcPtr) {
               *extPointersFrom = srcPtr;
               movePointer(src, *srcPtr, d);
           }
       }

       // ! still remains sorted
       sort(extpointers, extptrslength, extpointers+extptrslength, plen);

       extptrslength += plen;
   }

   OPT size_t getExtPtrsLength() const {
       return extptrslength;
   }

   OPT bool contains(void const * pointer) const {
       return (memory<=pointer && pointer<memory+memlength);
   }

   OPT StoragePool const * findChild(void * pointer) const {
       if (contains(pointer)) {
           return this;
       }
       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+chplslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) *chPoolsFrom);
           if (ptr) {
               StoragePool const * result = ptr->findChild(pointer);
               if (result) {
                   return result;
               }
           }
       }
       return NULL;
   }

   OPT void findChildChain(std::list<ChCh> & result, void * pointer) const {
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
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) *chPoolsFrom);
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

   OPT size_t* getChildPointer(StoragePool const * child) const {
       size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)child);
       size_t * last = childpools+chplslength;
       size_t* val;
       binary_insert(childpools, last, dd, npos, val);
       if (val == last) {
           // TODO
           return NULL;
       } else {
           if (*val == dd) {
               return val;
           } else {
               return NULL;
           }
       }
   }

   template<class ST>
   OPT void addPointer(ST*& _externalPointer) {
       addPointer((DataPtr&)_externalPointer);
   }

   template<class ST>
   OPT void removePointer(ST*& _externalPointer) {
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
               xassert(dataPointer->__parent == this);
           } else {

               if (!child) {
                   StoragePool const * parent = __parent;
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
           size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)&dataPointer);
           addValue(intpointers, intptrslength, intptrscapacity, dd, npos);
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
           addValue(extpointers, extptrslength, extptrscapacity, &dataPointer, (ExternalPtr)NULL);
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
               xassert(dataPointer->__parent == this);
           } else {

               if (!child) {
                   StoragePool const * parent = __parent;
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
           size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)&dataPointer);
           removeValue(intpointers, intptrslength, dd, npos);
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
           removeValue(extpointers, extptrslength, &dataPointer, (ExternalPtr)NULL);
       } else {
           std::cout << "Warning  StoragePool.removePointer : not external pointers but not in : " << (void*) &dataPointer
                     << " of " << (void*) memory << " .. " << (void*) (memory+memlength);
           if (child) {
               std::cout <<  " (though pointing to inside *:" << (void*) dataPointer << ")";
           }
           std::cout << std::endl;
       }
   }




   OPT void removeAllExternalPointers() {
       delete extpointers;
       extptrslength = 0;
       extptrscapacity = 0;
   }

   OPT void addChildPool(CPtrToMe childPoolPointer) {
       xassert(ownerPool == this && contains(childPoolPointer));

       size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)childPoolPointer);
       addValue(childpools, chplslength, chplscapacity, dd, npos);
   }

   OPT void removeChildPool(CPtrToMe childPoolPointer) {
       if (memory <= (uint8_t*)childPoolPointer && ((uint8_t*)childPoolPointer) < (memory+memlength)) {
           size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)childPoolPointer);
           removeValue(childpools, chplslength, dd, npos);
      }
   }

   OPT void removeChildPool(size_t * val) {
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
       xassert(bool(__kind&ST_IN_POOL) == bool(__parent));
       xassert(ownerPool);
       xassert(bool(memory) == bool(memlength));
       xassert(bool(memcapacity) == bool(memlength));
       xassert(bool(intvariables) == bool(intvarscapacity));
       xassert(bool(intpointers) == bool(intptrscapacity));
       xassert(bool(extpointers) == bool(extptrscapacity));
       xassert(bool(childpools) == bool(chplscapacity));

       PtrToMe lastm = NULL;
       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+chplslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) *chPoolsFrom);
           if (ptr) {
               xassert(contains(ptr));
               xassert(ptr->__parent == this || ptr->__parent == ownerPool);
               xassert(!ptr->isParentOf(*this));

               ptr->selfCheck();

               xassert(ptr > lastm);
               lastm = ptr;
           }
       }
       DataPtr lastd = NULL;
       size_t* varsFrom = intvariables;
       size_t* varsTo = intvariables+intvarslength;
       for (; varsFrom<varsTo; varsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) *varsFrom);
           if (ptr) {
               xassert(contains(ptr));
               xassert(ptr->__parent == this || ptr->__parent == ownerPool);

               xassert(ptr > lastd);
               lastd = ptr;
           }
       }
       long long lastv = -1;
       size_t* intPtrsFrom = intpointers;
       size_t* intPtrsTo = intpointers+intptrslength;
       for (; intPtrsFrom<intPtrsTo; intPtrsFrom++) {
           size_t v = *intPtrsFrom;
           if (v != npos) {
               xassert(v < memlength);
               ExternalPtr p = (ExternalPtr) decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) v);
               if (p) {
                   if (*p) {
                       StoragePool const * chpool = findChild(*p);
                       if (chpool) {
                           xassert((*p)->__parent == chpool || (*p)->__parent == chpool->ownerPool);
                       }
                   }
               }
               xassert(v > lastv);
               lastv = v;
           }
       }
       ExternalPtr lastp = NULL;
       ExternalPtr* extPtrsFrom = extpointers;
       ExternalPtr* extPtrsTo = extpointers+extptrslength;
       for (; extPtrsFrom<extPtrsTo; extPtrsFrom++) {
           ExternalPtr p = *extPtrsFrom;
           if (p) {
               xassert(p > lastp);
               if (*p) {
                   StoragePool const * chpool = findChild(*p);
                   if (chpool) {
                       xassert((*p)->__parent == chpool || (*p)->__parent == chpool->ownerPool);
                   }
               }
               lastp = p;
           }
       }
       for (iterator it = begin(); it != -1; it++) {
           Storeable *cur = *it;
           if (cur->__kind & ST_PARENT) {
               xassert(cur->__parent == this || cur->__parent == ownerPool);
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

   void debugVar(std::ostream& os, DataPtr& var, bool ptr, int indent) const {
       os<<" ";
       StoragePool const *p;
       if (contains(var)) {
       } else if (ownerPool!=this) {
           if (!(p=ownerPool->findChild(var))) {
               os<<"out!";
           } else if (!findChild(var)) {
               if (p == ownerPool)
                   os<<"own!";
               else
                   os<<"och!";
           } else {
               os<<"chi!";
           }
       } else if (!findChild(var)) {
           os<<"out!";
       } else {
           os<<"chi!";
       }
       if (ptr) os<<(void*)&var<<":";
       os<<(void*)var<<":";

       if (indent > DEBUG_MAX_IND || isDeleted()) { os<<"..."; return; }

       var->debugPrint(os, indent+1);
   }

   void debugPtr(std::ostream& os, ExternalPtr ptr, int indent) const {
       if (*ptr) {
           debugVar(os, *ptr, true, indent);
       } else {
           os<<" ";
           os<<(void*)ptr<<":NULL";
       }
   }

   void debugPrint(std::ostream& os, int indent = 0, char const *subtreeName = 0) const
   {
     if (indent > DEBUG_MAX_IND || isDeleted()) {
         ind(os,indent)<< "pool...";
     } else {
         ind(os,indent) << "pool:"<<(void*)this;
#ifdef DEBUG
         os<<":"<<objectName.str;
#endif
         os<<":mem:"<<(void*)memory<<":"<<memlength;
         os<<":var:"<<(void*)intvariables<<":"<<intvarslength;
         os<<":int:"<<(void*)intpointers<<":"<<intptrslength;
         os<<":ext:"<<(void*)extpointers<<":"<<extptrslength;
         os<<":chp:"<<(void*)childpools<<":"<<chplslength;
         if (__parent) {
             os<<":par:"<<(void*)__parent;
         }
         if (this != ownerPool) {
             os<<":own:"<<(void*)ownerPool;
         }
         //os<<std::dec;

         os<<":{";
         size_t* chPoolsFrom = childpools;
         size_t* chPoolsTo = childpools+chplslength;
         for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
             PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) *chPoolsFrom);
             if (ptr) {
                 ptr->debugPrint(os, indent+2);
             }
         }

         if (memlength) {
             ind(os,indent)<<"  values: ";
             for (iterator it=begin(); it != -1; it++) {
                 Storeable *cur = *it;
                 os<<" ";
                 cur->debugItm(os, indent+1);
             }
         }

         if (intvarslength) {
             ind(os,indent)<<"  intvars:";
             size_t* intVarsFrom = intvariables;
             size_t* intVarsTo = intvariables+intvarslength;
             for (; intVarsFrom<intVarsTo; intVarsFrom++) {
                 DataPtr var = (DataPtr)decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) *intVarsFrom);
                 if (var) {
                     debugVar(os, var, false, indent+1);
                 }
             }
         }

         if (intptrslength) {
             ind(os,indent)<<"  intptrs:";
             size_t* intPointersFrom = intpointers;
             size_t* intPointersTo = intpointers+intptrslength;
             for (; intPointersFrom<intPointersTo; intPointersFrom++) {
                 ExternalPtr ptr = (ExternalPtr)decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) *intPointersFrom);
                 if (ptr) {
                     debugPtr(os, ptr, indent+1);
                 }
             }
         }

         if (extptrslength) {
             ind(os,indent)<<"  extptrs:";
             ExternalPtr* extPointersFrom = extpointers;
             ExternalPtr* extPointersTo = extpointers+extptrslength;
             for (; extPointersFrom<extPointersTo; extPointersFrom++) {
                 ExternalPtr ptr = *extPointersFrom;
                 if (ptr) {
                     debugPtr(os, ptr, indent+1);
                 }
             }
         }

         ind(os,indent)<< "}"<<std::flush;
     }
   }

   OPT iterator begin(DataPtr variablePtr = 0) const {
       return iterator(*this, (uint8_t*)variablePtr);
   }

#ifdef REG_CHILD
   OPT child_iterator begin_children(DataPtr variablePtr) {
       return child_iterator(variablePtr);
   }
#endif

   OPT int end() {
       return -1;
   }
};

/****************************************************************************************************
 * STOREABLE
 *
 * ..................................................................................................
 */


OPT Storeable::Storeable(DBG_INFO_FORMAL_FIRST  __StoreAlreadyConstr nothing)
DBG_INFO_FWD(: objectName(objectName)  REG_CHILD_COMMA)
{
    DBG_INFO_FWD(lastObjName = objectName.str;)
}

OPT Storeable::Storeable(DBG_INFO_FORMAL_FIRST  size_t size_of)
DBG_INFO_FWD(: objectName(objectName)  REG_CHILD_COMMA)
#ifdef REG_CHILD
    NDEBUG_COLON __next(0)
#endif
{
#if debug_this
    if (__kind & ST_VALUE && __parent) {
        if (__parent->contains(this)) {
            std::cout<<"Bad allocation, use Storable(StoragePool &pool)" << std::endl;
        }
    }
#endif
DBG_INFO_FWD(lastObjName = objectName.str;)
    __kind = ST_NONE;
    __parent = NULL;
    __store_size  = getStoreSize(size_of);
}

/* new operator filled __pool and __store_size previously, we use passed argument to double check */
OPT Storeable::Storeable(DBG_INFO_FORMAL_FIRST  StoragePool const & pool, bool isPool, size_t size_of)
DBG_INFO_FWD(: objectName(objectName))
{
    DBG_INFO_FWD(lastObjName = objectName.str;)
    if (pool.contains(this)) {
        xassert((isPool?__kind & ST_STORAGE_POOL:__kind & ST_VALUE) && __parent == &pool);
    } else {
        __kind = ST_NONE;
        __parent = NULL;
        __store_size  = getStoreSize(size_of);
    }
}

template<class ME>
OPT Storeable::Storeable(DBG_INFO_FORMAL_FIRST  ME const & srcOrParent, bool childOfParent, bool isPool)
DBG_INFO_FWD(: objectName(objectName))
{
    DBG_INFO_FWD(lastObjName = objectName.str;)
    init(srcOrParent, sizeof(ME), childOfParent, isPool);
}

OPT Storeable::Storeable(DBG_INFO_FORMAL_FIRST   Storeable const & srcOrParent, size_t size_of, bool childOfParent, bool isPool)
DBG_INFO_FWD(: objectName(objectName))
{
    DBG_INFO_FWD(lastObjName = objectName.str;)
    init(srcOrParent, size_of, childOfParent, isPool);
}

OPT Storeable::~Storeable() {
    if (__kind & ST_VALUE) {
        if (__kind & ST_DELETED) {
            x_assert_fail("Already deleted.", __FILE__, __LINE__);
        }
        removeInParent();
        __kind |= ST_DELETED;
    }
}

OPT void Storeable::init(Storeable const & srcOrParent, size_t size_of, bool childOfParent, bool isPool) {
    if (childOfParent) {
        StoragePool const & srcPool = srcOrParent.getPoolRef();
        xassert(srcPool.contains(this));
        __kind = isPool ? ST_STORAGE_POOL : ST_VALUE;
        __parent = &srcPool;
        __store_size = getStoreSize(size_of);
        xassert(&srcPool == __parent);
    #ifdef REG_CHILD
        __next = 0;
        __parent->regChild(this);
    #endif
        if (!isPool) {
            constcast(getParentRef()).addVariable(this);
        } else {
            constcast(getParentRef()).addChildPool((CPtrToMe)this);
        }
    } else {
        __parent = NULL;
        assign(srcOrParent, size_of);
    }
}

OPT void Storeable::removeInParent() {
    StoragePool * pool = constcast(__parent);
    if (pool) {
        pool->removeVariable(this);
    }
}

template<class ME>
OPT void Storeable::assign(ME const & srcOrParent) {
    assign(srcOrParent, sizeof(ME));
}

OPT void Storeable::assign(Storeable const & src, size_t size_of) {
    // !
    xassert(src.__store_size == getStoreSize(size_of));
    StoragePool * oldpar = constcast(__parent);

#ifdef DEBUG
    uint8_t * bg = sizeof(objectName)+(uint8_t*)&objectName;
    uint8_t * th = (uint8_t*)this;
    std::ptrdiff_t d = bg-th;
    memcpy(bg, d+(uint8_t*)&src, size_of-d);
#else
    memcpy(this, &src, size_of);
#endif

    // transfer __parentVector (we keep parent)
    if (__kind & ST_VALUE) {
        xassert(__parent);
        assignSameParent(src, false, oldpar);
    } else if (__kind & ST_STORAGE_POOL) {
        xassert(__parent);
        assignSameParent(src, true, oldpar);
    } else {
        xassert(!__parent);
    }

    if (__kind & ST_REMAIN_DELETED) {
        x_assert_fail("Invalid kind.", __FILE__, __LINE__);
    }
}

OPT void Storeable::assignSameParent(Storeable const & src, bool isPool, StoragePool const * oldPool) {
    StoragePool const * srcPool = src.__parent;
    if (srcPool) {
        assignParent(srcPool, isPool, oldPool);
    } else {
        if (__parent || __kind) {
            std::cout << "Warning  Storeable.assignSameParent(" << getKind()
        #ifdef DEBUG
                      << " " << objectName.str
        #endif
                      << ")  copy to stack   from pool:"
                      << (void*) srcPool << " -> " << (void*) this << std::endl;
            __parent = NULL;
            __kind = ST_NONE;
        }
    }
}

OPT void Storeable::assignParent(StoragePool const * srcPool, bool isPool, StoragePool const * oldPool) {
    StoragePool const * par = srcPool->findChild(this);
    StoragePool * par0 = NULL;

    if (__parent && __kind && oldPool) {
        par0 = constcast(oldPool->findChild(this));
        xassert(par0);
        if (par0 != par) {
            if (__kind & ST_VALUE) {
                par0->removeVariable(this);
            } else if (__kind & ST_STORAGE_POOL) {
                par0->removeChildPool((CPtrToMe)this);
            } else {
                x_assert_fail("wrong kind", __FILE__, __LINE__);
            }

            if (__kind & ST_DELETED) {
                x_assert_fail("wrong kind", __FILE__, __LINE__);
            }
        }
    }

    if (par) {
        __parent = par;
        if (!__kind || par0 != par) {
            if (__kind) {
                if (isPool?!(__kind&ST_STORAGE_POOL):!(__kind&ST_VALUE)) {
                    x_assert_fail("wrong kind", __FILE__, __LINE__);
                }
                if (__kind & ST_DELETED) {
                    x_assert_fail("wrong kind", __FILE__, __LINE__);
                }
            }
            if (isPool) {
                __kind = ST_STORAGE_POOL;
                constcast(par)->addChildPool((CPtrToMe)this);
            } else {
                __kind = ST_VALUE;
                constcast(par)->addVariable(this);
            }
        } else {
            xassert(isPool ? (__kind&ST_STORAGE_POOL) : __kind&ST_VALUE);
        }
    } else {
        std::cout << "Warning  Storeable.assignParent(" << getKind()
    #ifdef DEBUG
                  << " " << objectName.str
    #endif
                  << ")  copy to stack   from pool:"
                  << (void*) srcPool << " -> " << (void*) this << std::endl;
        __parent = NULL;
        __kind = ST_NONE;
    }
}

OPT void* Storeable::operator new (std::size_t size) {
    void* result = malloc(size);
    if (result) {
        return result;
    } else {
        throw std::bad_alloc();
    }
}

OPT void* Storeable::operator new (std::size_t size, const std::nothrow_t& nothrow_value) {
    void* result = malloc(size);
    return result;
}

OPT void* Storeable::operator new[] (std::size_t size) {
    void* result = malloc(size);
    if (result) {
        return result;
    } else {
        throw std::bad_alloc();
    }
}

OPT void* Storeable::operator new[] (std::size_t size, const std::nothrow_t& nothrow_value) {
    void* result = malloc(size);
    return result;
}

OPT void* Storeable::operator new (std::size_t size, StoragePool const & pool) {
    void const * data;
    constcast(pool).allocParentItem(data, getStoreSize(size));
    return constcast(data);
}

OPT void* Storeable::operator new[] (std::size_t size, StoragePool const & pool) {
    void const * data;
    constcast(pool).allocParentItem(data, getStoreSize(size));
    return constcast(data);
}

OPT void Storeable::operator delete (void* _ptr) {
    DataPtr ptr = (DataPtr) _ptr;
    if (!(ptr->__kind & ST_IN_POOL)) {
        free(_ptr);
    }
    // else Nothing to do here, everything is in ~Storeable
}

OPT void Storeable::operator delete[] (void* _ptr) {
    DataPtr ptr = (DataPtr) _ptr;
    if (!(ptr->__kind & ST_IN_POOL)) {
        free(_ptr);
    }
    // else Nothing to do here, everything is in ~Storeable
}

OPT void Storeable::debugItm(std::ostream& os, int indent) const {
    if (__kind & ST_VALUE) {
    } else if (__kind & ST_STORAGE_POOL) {
        os<<"pool:";
    } else if (__kind & ST_REMAINDER) {
        os<<"remain:";
    } else {
        os<<"stack:";
    }

    if (__kind & ST_CHILD) {
        os<<"child:";
    }

    if (__kind & ST_DELETED) {
        os<<"del:"<<(void*)this;
    } else if (__kind & ST_VALUE) {
        os<<(void*)this<<":";
        debugPrint(os, indent);
    } else {
        os<<(void*)this;
    }
}

OPT void operator<<(std::ostream & os, Storeable const & st) {
    st.debugPrint(os);
}

}// namespace str

OPT void debugString(std::ostream & os, str::Storeable const & st, int level) {
    st.debugPrint(os, level);
}

OPT std::string toString(str::Storeable const & st) {
  std::stringstream s;
  st.debugPrint(s);
  return s.str();
}
/*
OPT char const * toString(str::StoragePool const & st) {
#ifdef DEBUG
   return st.getObjectName();
#endif
   return "storage pool " __FILE_LINE__;
#else
}*/

#endif // STORAGE_H

