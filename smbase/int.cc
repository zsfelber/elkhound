#include "int.h"

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


