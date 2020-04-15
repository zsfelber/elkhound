#include "storage.h"
#include "str.h"

//const size_t str::STORAGE_POOL_SIZE = str::getStoreSize(sizeof(str::StoragePool));

const char* str::lastObjName = "";


namespace str {

OPT str::stringstream &ind(str::stringstream &os, int indent)
{
  xassert(indent <= 110);
  char const *ib = "                                                                                                              ";
  os << "\n";
  os << ib+(110-indent);
  return os;
}


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










void Storeable::debugVar(str::stringstream& os, DataPtr& var, bool ptr, int indent) const {
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

void Storeable::debugPtr(str::stringstream& os, ExternalPtr ptr, int indent) const {
    if (*ptr) {
        debugVar(os, *ptr, true, indent);
    } else {
        os<<" ";
        os<<(void*)ptr<<":NULL";
    }
}

void Storeable::debugPrint(str::stringstream& os, int indent = 0, char const *subtreeName = 0) const
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

      ind(os,indent)<< "}"<<str::flush;
  }
}













OPT void Storeable::debugItm(str::stringstream& os, int indent) const {
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

OPT int Storeable::debugPrint(int indent) const
{
    str::stringstream sis;
    debugPrint(sis, indent);
    std::cout << sis.c_str();
    return 0;
}

OPT void operator<<(str::stringstream &os, Storeable const & st) {
    st.debugPrint(os);
}

}// namespace str

OPT void debugString(str::stringstream &os, str::Storeable const & st, int level) {
    st.debugPrint(os, level);
}

OPT str::string toString(str::Storeable const & st) {
  str::stringstream s;
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
