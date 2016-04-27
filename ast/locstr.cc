// locstr.cc            see license.txt for copyright and terms of use
// code for locstr.h

#include "locstr.h"     // this module
#include "exc.h"        // LocString

LocString::LocString(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), loc(SL_UNKNOWN),
    str(NULL)           // problem with "" is we don't have the string table here..
{}

LocString::LocString(DBG_INFO_FORMAL_FIRST  LocString const &obj)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  obj,false), str(NULL)
{}

LocString::LocString(DBG_INFO_FORMAL_FIRST  Storeable const &parent)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  parent, true), str(NULL)
{}

LocString::LocString(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, SourceLoc L, rostring s)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), loc(L),
    str(s)
{}

LocString::LocString(DBG_INFO_FORMAL_FIRST  SourceLoc L, rostring s)
  : Storeable(DBG_INFO_ARG_FWD), loc(L), str(s)
{}


LocString::LocString(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, Flatten&)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), loc(SL_UNKNOWN), str(NULL)
{}

LocString::LocString(DBG_INFO_FORMAL_FIRST  Storeable const &parent, Flatten&)
  : Storeable(DBG_INFO_ARG_FWD_FIRST  parent, sizeof(LocString), true), loc(SL_UNKNOWN), str(NULL)
{}

void LocString::xfer(str::StoragePool &pool, Flatten &flat)
{
  // doh.. flattening locs is hard.  I wasn't even doing
  // it before.  issues:
  //   - don't want to store the file name lots of times
  //   - what if the file goes away, or we're in a different directory?
  //   - what if the file is changed, what loc to use then?
  // so for now I'm punting and not saving the loc at all...

  xassert(flattenStrTable);
  const string *result = &str;
  flattenStrTable->xfer(flat, result);
}


/*void LocString::copyAndDel(LocString *obj)
{
  loc = obj->loc;
  str = obj->str;
  delete obj;
}*/

LocString *LocString::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool) const
{
  return new (pool) LocString(DBG_INFO_ARG_FWD_FIRST  *this);
}

LocString *LocString::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool const &pool, int deepness, int listDeepness) const
{
  return new (pool) LocString(DBG_INFO_ARG_FWD_FIRST  *this);
}


bool LocString::equals(StringRef other) const
{
  if (!str.length()) {
    return !other;                            // equal if both null
  }
  else {
    return other && str.equals(other); // or same contents
  }
}

bool LocString::equals(rostring other) const
{
  return str.equals(other.c_str()); // or same contents
}

string toString(LocString const &s)
{
  return string(s.str);
}


string toXml(LocString op)
{
  xunimp("XML serialization of LocString");
  return "";
}

void fromXml(LocString &out, rostring str)
{
  xunimp("XML serialization of LocString");
}


// EOF
