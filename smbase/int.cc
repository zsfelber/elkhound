#include "int.h"

int Integer::ctorcount = 0;
int Integer::dtorcount = 0;

Integer::Integer(DBG_INFO_FORMAL_FIRST  int ii)
  : str::Storeable(DBG_INFO_ARG_FWD), i(ii)
{
  ctorcount++;
}

Integer::Integer(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int ii)
  : str::Storeable(DBG_INFO_ARG_FWD_FIRST  pool), i(ii)
{
  ctorcount++;
}

Integer::~Integer()
{
  dtorcount++;
}


