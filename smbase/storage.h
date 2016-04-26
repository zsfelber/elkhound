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
#define DBG_INFO_FWD(a) a
#define DBG_INFO_FWD_COM(a) a,
#define DBG_INFO_FWD_PCOM(a) ,a
#else
#define DBG_INFO_FORMAL
#define DBG_INFO_FORMAL_FIRST
#define DBG_INFO_ARG_FWD
#define DBG_INFO_ARG_FWD_FIRST
#define DBG_INFO_ARG0
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

inline size_t encodeDeltaPtr(uint8_t const * origin,
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

inline std::ptrdiff_t encodeSignedDeltaPtr(uint8_t const * origin,
                                           uint8_t const * address) {
    if (!address || !origin) {
        return 0;
    } else {
        return address - origin;
    }
}

inline uint8_t const * decodeDeltaPtr(uint8_t const * origin,
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

/*inline uint8_t const * decodeDeltaBackPtr(uint8_t const * origin,
                                          DBG_INFO_FWD_COM(uint8_t const * org_last)
                                          size_t delta) {
    return (delta == npos || !origin)  ? NULL : (origin - delta);
}*/

inline uint8_t const * decodeSignedDeltaPtr(uint8_t const * origin,
                                            std::ptrdiff_t delta) {
    return (delta == 0 || !origin)  ? NULL : (origin + delta);
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

    void init(Storeable const & srcOrParent, size_t size_of, bool childOfParent, bool isPool);

    void removeInParent();

public:

    enum __Kind {
        ST_NONE = 0,
        ST_PARENT,
        ST_STORAGE_POOL,
        ST_CHILD,
        ST_DELETED
    };

   typedef StoragePool* PtrToMe;
   typedef StoragePool const * CPtrToMe;
   typedef Storeable* DataPtr;
   typedef DataPtr* ExternalPtr;


   void* operator new (std::size_t size);
   void* operator new (std::size_t size, void* ptr);
   void* operator new (std::size_t size, const std::nothrow_t& nothrow_value);
   void* operator new[] (std::size_t size);
   void* operator new[] (std::size_t size, void* ptr);
   void* operator new[] (std::size_t size, const std::nothrow_t& nothrow_value);

   void* operator new (std::size_t size, StoragePool const & pool);
   void* operator new[] (std::size_t size, StoragePool const & pool);

   void operator delete (void* ptr);
   void operator delete (void* ptr, const std::nothrow_t& nothrow_constant);
   void operator delete (void* ptr, void* voidptr2);
   void operator delete (void* ptr, size_t size);
   void operator delete[] (void* ptr);
   void operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant);
   void operator delete[] (void* ptr, void* voidptr2);
   void operator delete[] (void* ptr, size_t size);


   Storeable(DBG_INFO_FORMAL_FIRST  __StoreAlreadyConstr nothing);

   Storeable(DBG_INFO_FORMAL_FIRST  size_t size_of = 0);

   /* new operator filled __pool and __store_size previously, we use passed argument to double check */
   Storeable(DBG_INFO_FORMAL_FIRST  StoragePool const & pool, bool isPool = false);

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

   void assign(Storeable const & srcOrParent, size_t size_of);

   void assignSameParent(Storeable const & srcOrParent);

   void assignParent(StoragePool const * srcPool, __Kind def = ST_PARENT);

   inline __Kind getKind() const {
       return (__Kind)__kind;
   }

   inline StoragePool const & getParentRef() const {
       return NN(getParent());
   }

   inline StoragePool const * getParent() const {
       return __parent;
   }

   inline StoragePool const * getPool() const {
       switch (__kind) {
       case ST_NONE:
           return asPool();
       case ST_STORAGE_POOL:
           return (StoragePool*)this;
       case ST_PARENT:
       case ST_CHILD:
           return __parent;
       default:
           x_assert_fail("Wrong kind.", __FILE__, __LINE__);
           break;
       }
   }

   inline StoragePool const & getPoolRef() const {
       return NN(getPool());
   }

   inline virtual StoragePool * asPool() const {
       return NULL;
   }

   inline int debugPrint(int indent = 0) const
   {
       debugPrint(std::cout, indent);
       return 0;
   }

   virtual inline void debugPrint(std::ostream& os, int indent = 0, char const *subtreeName = 0) const
   {
   }
#ifdef DEBUG
   inline char const * getObjectName() const {
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

       inline iterator() {}
       inline iterator(StoragePool const & pool, uint8_t const * _variablePtr) : index(0), variablePtr(_variablePtr) {
           if (!variablePtr) {
               variablePtr = pool.memory;
           }
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
               x_assert_fail("Overindexed Storeable iterator.", __FILE__, __LINE__);
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
               x_assert_fail("Overindexed Storeable iterator.", __FILE__, __LINE__);
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
       Cp_TmpDuplicate,
   };

private:

   size_t* childpools;
   size_t chplslength, chplscapacity;

   StoragePool * ownerPool;

   inline void movePointers(StoragePool const & oldmem) {
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

   inline void convertDeltas(size_t* buf, size_t* last, uint8_t const * oldOrigin  DBG_INFO_FWD_PCOM(uint8_t const * oldLast)) {
       for (; buf<last; buf++) {
           uint8_t const * ptr = decodeDeltaPtr(oldOrigin, DBG_INFO_FWD_COM(oldLast) *buf);
           if (ptr) {
               *buf = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) ptr);
           }
       }
   }

   template <typename T>
   inline void sort(T * buf, size_t buflength, T * src_buf, size_t src_buflength) {
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


   inline void moveFrom(StoragePool const & parent, DataPtr & child, uint8_t * origin = NULL) {

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
           switch (cur->__kind) {
           case ST_PARENT:
               fixChild<Storeable>(cur, src_cur, target, source, d, req);
               break;
           case ST_STORAGE_POOL:
               // already processed in fixChildPools
               //fixChild<StoragePool>((StoragePool*)cur, (StoragePool*)src_cur, source, d);
               break;
           case ST_CHILD:
               x_assert_fail("child directly in iterator not supported", __FILE__, __LINE__);
               break;
           default:
               break;
           }
       }
   }

   inline void fixChildValues(StoragePool const * target, StoragePool const & source,
                              std::ptrdiff_t d, bool req = true) {

       size_t* tar_varsFrom = intvariables;
       size_t* tar_varsTo = intvariables+intvarslength;
       size_t* src_varsFrom = source.intvariables;
       size_t* src_varsTo = source.intvariables+source.intvarslength;

       fixChildren<Storeable>(tar_varsFrom, tar_varsTo, src_varsFrom, src_varsTo,
                      target, source, d, req);
   }

   inline void fixChildPools(StoragePool const * target, StoragePool const & source,
                             std::ptrdiff_t d, bool req = true) {

       size_t* tar_chPoolsFrom = childpools;
       size_t* tar_chPoolsTo = childpools+chplslength;
       size_t* src_chPoolsFrom = source.childpools;
       size_t* src_chPoolsTo = source.childpools+source.chplslength;

       fixChildren<StoragePool>(tar_chPoolsFrom, tar_chPoolsTo, src_chPoolsFrom, src_chPoolsTo,
                      target, source, d, req);
   }

   template <class V>
   inline void fixChildren(size_t* tar_chFrom, size_t* tar_chTo,
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
   inline void fixChild(V* ptr, V* src_ptr,
                        StoragePool const * target, StoragePool const & source,
                        std::ptrdiff_t d, bool req) {

       xassert(bool(ptr)==bool(src_ptr));
       if (ptr) {
           xassert(ptr->__kind==src_ptr->__kind);
           xassert(ptr->__kind != ST_NONE || ptr->__kind != ST_DELETED);
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

   inline void fixPoolPointer(StoragePool const * target, Storeable * ptr, Storeable const * src_ptr) {
       xassert( contains(ptr) );

       ptr->__parent = target;
   }

   inline void fixPoolPointer(StoragePool const * target, StoragePool * ptr, StoragePool const * src_ptr) {
       fixPoolPointer(target, (Storeable*) ptr, src_ptr);

       xassert( src_ptr->ownerPool == ptr->ownerPool );

       if (ptr->ownerPool == src_ptr) {
           ptr->ownerPool = this;
       } else {
          xassert(ptr->ownerPool == this || ptr->ownerPool->isParentOf(*this));
       }
   }

   inline void fixPoolPointerChildren(StoragePool const * target, Storeable * ptr, Storeable const * src_ptr) {

   }

   inline void fixPoolPointerChildren(StoragePool const * target, StoragePool * ptr, StoragePool const * src_ptr) {
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
                  if (cur->__kind == ST_DELETED) {
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
                              cur->__kind = ST_DELETED;
                          }

                          if (first.variablePtr == da) {
                              first_del_var = npos;
                              if (deleted_vars) {
                                  for (; it != -1; it++) {
                                      if (it->__kind == ST_DELETED) {
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
      data->__kind = ST_PARENT;
      data->__parent = this;
      data->__store_size = store_size;
   }

   inline void assignImpl(StoragePool const & src, CopyMode copyMode=Cp_All) {

       switch (copyMode) {
       case Cp_TmpDuplicate:
       {
           xassert((__kind == ST_NONE || __kind == ST_STORAGE_POOL) && (src.__kind == ST_NONE || src.__kind == ST_STORAGE_POOL));

           ownerPool = (StoragePool*) &src;

           removeChildPool(&src);
           removeChildPool(this);

           break;
       }
       case Cp_Move:
       {
           xassert(__parent == src.__parent);
           xassert((__kind == ST_NONE || __kind == ST_STORAGE_POOL) && (src.__kind == ST_NONE || src.__kind == ST_STORAGE_POOL));

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
           xassert((__kind == ST_NONE || __kind == ST_STORAGE_POOL) && (src.__kind == ST_NONE || src.__kind == ST_STORAGE_POOL) && bufsz==memcapacity);

           reassign(src);
           break;
       }
       default:
           x_assert_fail("Wrong kind.", __FILE__, __LINE__);
           break;
       }
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

   inline bool isParentOf(Storeable const & chpool) const {
       StoragePool const * par = chpool.getParent();
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
       case ST_PARENT:
       case ST_STORAGE_POOL:
       case ST_CHILD:
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

   template <typename T>
   inline void addValue(T*& vars, size_t &length, size_t &capacity, T dd, T nullitm) {
       // to ensure it is ordered (binary search invariant)
       T *val;
       size_t pbufsz = getPtrBufSize(length);
       if (capacity < pbufsz) {
           extendBuffer(vars, length, capacity, pbufsz);
       }

       T* last = vars + length;

       if (length) {
           if (dd < vars[length-1]) {
               val = lower_bound(vars, last, dd, nullitm);
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
   inline void removeValue(T* vars, size_t &length, T dd, T nullitm) {

       T* last = vars + length;
       T* val = vars ? lower_bound(vars, last, dd, nullitm) : last;
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
       xassert(ownerPool == this && dataVariable && dataVariable->__kind == ST_PARENT);
       __kind = ST_CHILD;
       /*if (ownerPool != this) {
           ownerPool->addVariable(dataVariable);
           return;
       }*/

       StoragePool const * child = findChild(dataVariable);
       xassert(child == this && dataVariable->getParent() == this);

       size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)dataVariable);
       addValue(intvariables, intvarslength, intvarscapacity, dd, npos);
   }

   void removeVariable(DataPtr dataVariable) {
       xassert(ownerPool == this && dataVariable);
       /*if (ownerPool != this) {
           ownerPool->removeVariable(dataVariable);
           return;
       }*/

       size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)dataVariable);

       if (dataVariable->__kind == ST_CHILD) {
           StoragePool const * child = findChild(dataVariable);
           xassert(child == this && dataVariable->getParent() == this);

           removeValue(intvariables, intvarslength, dd, npos);
       }

       dataVariable->__kind = ST_DELETED;
       if (dd < first_del_var) {
           first_del_var = dd;
       }
       deleted_vars += dataVariable->__store_size;
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
       xassert(__store_size == STORAGE_POOL_SIZE);

       if (childOfParent) {
           memset(((uint8_t*)this)+sizeof(Storeable), 0, sizeof(StoragePool)-sizeof(Storeable));
           xassert((__kind == ST_PARENT||__kind == ST_STORAGE_POOL) && copyMode == Cp_All);
           __kind = ST_STORAGE_POOL;
           first_del_var = npos;
           ownerPool = this;
       } else {
           xassert(__kind == ST_NONE || __kind == ST_STORAGE_POOL);
           StoragePool & srcOrParentPool = (StoragePool &) srcOrParent;
           ownerPool = &srcOrParentPool;
           assignImpl(srcOrParentPool, copyMode);
       }

       if (copyMode != Cp_TmpDuplicate) {
           StoragePool * pool = constcast(getParent());
           if (pool) {
               pool->addChildPool(this);
           }
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

   inline void assign(StoragePool const & src, CopyMode copyMode=Cp_All) {
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
       assignImpl(src, copyMode);
   }

   inline void removeInParent() {
       StoragePool * pool = constcast(getParent());
       if (pool) {
           pool->removeChildPool(this);
       }
   }

   inline void del() {
       xassert(ownerPool == this);

       delChildPools();

       if (memory) delete[] memory;
       if (intvariables) delete[] intvariables;
       if (intpointers) delete[] intpointers;
       if (extpointers) delete[] extpointers;
       if (childpools) delete[] childpools;

       removeInParent();
       clear();
   }

   inline void delChildPools() {
       size_t* chPoolsFrom = childpools;
       size_t* chPoolsTo = childpools+chplslength;
       for (; chPoolsFrom<chPoolsTo; chPoolsFrom++) {
           PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) *chPoolsFrom);
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
       __parent = NULL;
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

   inline void swap(StoragePool * pool) {
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

   inline size_t getExtPtrsLength() const {
       return extptrslength;
   }

   inline bool contains(void const * pointer) const {
       return (memory<=pointer && pointer<memory+memlength);
   }

   inline StoragePool const * findChild(void * pointer) const {
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

   inline size_t* getChildPointer(StoragePool const * child) const {
       size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)child);
       size_t * last = childpools+chplslength;
       size_t* val = lower_bound(childpools, last, dd, npos);
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




   inline void removeAllExternalPointers() {
       delete extpointers;
       extptrslength = 0;
       extptrscapacity = 0;
   }

   inline void addChildPool(CPtrToMe childPoolPointer) {
       xassert(ownerPool == this && contains(childPoolPointer));

       size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)childPoolPointer);
       addValue(childpools, chplslength, chplscapacity, dd, npos);
   }

   inline void removeChildPool(CPtrToMe childPoolPointer) {
       if (memory <= (uint8_t*)childPoolPointer && ((uint8_t*)childPoolPointer) < (memory+memlength)) {
           size_t dd = encodeDeltaPtr(memory, DBG_INFO_FWD_COM(memory+memlength) (uint8_t*)childPoolPointer);
           removeValue(childpools, chplslength, dd, npos);
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
       xassert(bool(__kind) == bool(__parent));
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
           switch (cur->__kind) {
           case ST_PARENT:
           case ST_STORAGE_POOL:
           case ST_CHILD:
               xassert(cur->__parent == this || cur->__parent == ownerPool);
               break;
           default:
               break;
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

   void debugVar(std::ostream& os, DataPtr& var, bool ptr) const {
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
       var->debugPrint(os);
   }

   void debugPtr(std::ostream& os, ExternalPtr ptr) const {
       if (*ptr) {
           debugVar(os, *ptr, true);
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
         os<<":var:"<<(void*)intvariables<<":"<<intvarslength;
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
             PtrToMe ptr = (PtrToMe)decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) *chPoolsFrom);
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
                 case ST_PARENT:
                     os<<" "<<(void*)cur<<":";
                     cur->debugPrint(os);
                     break;
                 case ST_CHILD:
                     x_assert_fail("child directly in iterator not supported", __FILE__, __LINE__);
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

         if (intvarslength) {
             ind(os,indent)<<"  intvars:";
             size_t* intVarsFrom = intvariables;
             size_t* intVarsTo = intvariables+intvarslength;
             for (; intVarsFrom<intVarsTo; intVarsFrom++) {
                 DataPtr var = (DataPtr)decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) *intVarsFrom);
                 if (var) {
                     debugVar(os, var, false);
                 }
             }
             os<<"\n";
         }

         if (intptrslength) {
             ind(os,indent)<<"  intptrs:";
             size_t* intPointersFrom = intpointers;
             size_t* intPointersTo = intpointers+intptrslength;
             for (; intPointersFrom<intPointersTo; intPointersFrom++) {
                 ExternalPtr ptr = (ExternalPtr)decodeDeltaPtr(memory,  DBG_INFO_FWD_COM(memory+memlength) *intPointersFrom);
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


inline Storeable::Storeable(DBG_INFO_FORMAL_FIRST  __StoreAlreadyConstr nothing)
DBG_INFO_FWD(: objectName(objectName)  REG_CHILD_COMMA)
{
    DBG_INFO_FWD(lastObjName = objectName.str;)
}

inline Storeable::Storeable(DBG_INFO_FORMAL_FIRST  size_t size_of)
DBG_INFO_FWD(: objectName(objectName)  REG_CHILD_COMMA)
#ifdef REG_CHILD
    NDEBUG_COLON __next(0)
#endif
{
#if debug_this
    if (__kind == ST_PARENT && __parent) {
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
inline Storeable::Storeable(DBG_INFO_FORMAL_FIRST  StoragePool const & pool, bool isPool)
DBG_INFO_FWD(: objectName(objectName))
{
    DBG_INFO_FWD(lastObjName = objectName.str;)
    if (pool.contains(this)) {
        xassert((isPool?__kind == ST_STORAGE_POOL:__kind == ST_PARENT) && getParent() == &pool);
    } else {
        __kind = ST_NONE;
        __parent = NULL;
        __store_size  = 0;
    }
}

template<class ME>
inline Storeable::Storeable(DBG_INFO_FORMAL_FIRST  ME const & srcOrParent, bool childOfParent, bool isPool)
DBG_INFO_FWD(: objectName(objectName))
{
    DBG_INFO_FWD(lastObjName = objectName.str;)
    init(srcOrParent, sizeof(ME), childOfParent, isPool);
}

inline Storeable::Storeable(DBG_INFO_FORMAL_FIRST   Storeable const & srcOrParent, size_t size_of, bool childOfParent, bool isPool)
DBG_INFO_FWD(: objectName(objectName))
{
    DBG_INFO_FWD(lastObjName = objectName.str;)
    init(srcOrParent, size_of, childOfParent, isPool);
}

inline Storeable::~Storeable() {
    switch (__kind) {
    case ST_PARENT:
    case ST_STORAGE_POOL:
    case ST_CHILD:
        removeInParent();
        break;
    case ST_DELETED:
        x_assert_fail("Already deleted.", __FILE__, __LINE__);
        break;
    default:
        break;
    }
}

inline void Storeable::init(Storeable const & srcOrParent, size_t size_of, bool childOfParent, bool isPool) {
    if (childOfParent) {
        StoragePool const & srcPool = srcOrParent.getPoolRef();
        xassert(srcPool.contains(this));
        __kind = isPool ? ST_STORAGE_POOL : ST_PARENT;
        __parent = &srcPool;
        __store_size = getStoreSize(size_of);
        xassert(&srcPool == getParent());
    #ifdef REG_CHILD
        __next = 0;
        getParent()->regChild(this);
    #endif
    } else {
        assign(srcOrParent, size_of);
    }
    if (!isPool && __kind) {
        constcast(getParentRef()).addVariable(this);
    }
}

inline void Storeable::removeInParent() {
    StoragePool * pool = constcast(getParent());
    if (pool) {
        pool->removeVariable(this);
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
        xassert(!__parent);
        break;
    case ST_PARENT:
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
    StoragePool const * srcPool = src.getParent();
    if (srcPool) {
        assignParent(srcPool);
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

inline void Storeable::assignParent(StoragePool const * srcPool, __Kind def) {
    StoragePool const * par = srcPool->findChild(this);
    if (par) {
        __parent = par;
        if (!__kind) {
            __kind = def;
            switch (__kind) {
            case ST_PARENT:
                constcast(par)->addVariable(this);
                break;
            case ST_STORAGE_POOL:
                constcast(par)->addChildPool(asPool());
                break;
            default:
                x_assert_fail("wrong kind", __FILE__, __LINE__);
                break;
            }
        }
        xassert(par == getParent());
    } else {
        if (__kind && __parent) {
            switch (__kind) {
            case ST_CHILD:
                constcast(__parent)->removeVariable(this);
                break;
            case ST_STORAGE_POOL:
                constcast(__parent)->removeChildPool(asPool());
                break;
            case ST_PARENT:
                break;
            default:
                x_assert_fail("wrong kind", __FILE__, __LINE__);
                break;
            }
        }
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

inline void* Storeable::operator new (std::size_t size, StoragePool const & pool) {
    void const * data;
    constcast(pool).allocParentItem(data, getStoreSize(size));
    return constcast(data);
}

inline void* Storeable::operator new[] (std::size_t size, StoragePool const & pool) {
    void const * data;
    constcast(pool).allocParentItem(data, getStoreSize(size));
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

inline std::string toString(str::Storeable const & st) {
  std::stringstream s;
  st.debugPrint(s);
  return s.str();
}
/*
inline char const * toString(str::StoragePool const & st) {
#ifdef DEBUG
   return st.getObjectName();
#endif
   return "storage pool " __FILE_LINE__;
#else
}*/

#endif // STORAGE_H

