#ifndef STOR0_H
#define STOR0_H

#include <cstddef>
#include <cstdint>
#include <new>
#include <memory.h>
#include <malloc.h>
#include <iostream>

#include "defs.h"
#include "typ.h"

namespace str {



struct __DbgStr {
    char const * str;
    explicit __DbgStr(char const * str) : str(str) {}
};

static struct __StoreAlreadyConstr {
} StoreAlreadyConstr;


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


}
#endif // STOR0_H
