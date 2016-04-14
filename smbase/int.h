#ifndef INT_H
#define INT_H

#include "storage.h"

// ------------ object class --------------------
// class of objects to hold in the list
class Integer : public str::Storeable {
public:
  static int ctorcount;     // # of calls to ctor
  static int dtorcount;     // # of calls to dtor
  int i;                    // data this class holds

public:
  Integer(DBG_INFO_FORMAL_FIRST  int ii);
  Integer(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int ii);
  ~Integer();

  void debugPrint(std::ostream& os, int indent = 0, char const * subtreeName = 0) const
  {
      str::ind(os,indent)<< "int:"<<i;
  }

};


#endif // INT_H

