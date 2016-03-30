#ifndef INT_H
#define INT_H


// ------------ object class --------------------
// class of objects to hold in the list
class Integer : public Storeable {
public:
  static int ctorcount;     // # of calls to ctor
  static int dtorcount;     // # of calls to dtor
  int i;                    // data this class holds

public:
  Integer(int ii);
  ~Integer();
};

int Integer::ctorcount = 0;
int Integer::dtorcount = 0;

Integer::Integer(int ii)
  : i(ii)
{
  ctorcount++;
}

Integer::~Integer()
{
  dtorcount++;
}


#endif // INT_H

