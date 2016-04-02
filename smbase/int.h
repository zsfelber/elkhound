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
  ~Integer();
};


#endif // INT_H

