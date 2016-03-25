// locstr.cc            see license.txt for copyright and terms of use
// code for locstr.h

#include "locstr.h"     // this module
#include "exc.h"        // LocString

LocString::LocString(StoragePool &pool)
  : Storeable(pool), loc(SL_UNKNOWN),
    str(NULL)           // problem with "" is we don't have the string table here..
{}

LocString::LocString(Storeable *master)
  : Storeable(master), loc(SL_UNKNOWN),
    str(NULL)           // problem with "" is we don't have the string table here..
{}

LocString::LocString(LocString const &obj)
  : Storeable(obj), loc(obj.loc),
    str(obj.str)
{}

LocString::LocString(StoragePool &pool, SourceLoc L, StringRef s)
  : Storeable(pool), loc(L),
    str(s)
{}


LocString::LocString(StoragePool &pool, Flatten&)
  : Storeable(pool), loc(SL_UNKNOWN), str(NULL)
{}

void LocString::xfer(StoragePool &pool, Flatten &flat)
{
  // doh.. flattening locs is hard.  I wasn't even doing
  // it before.  issues:
  //   - don't want to store the file name lots of times
  //   - what if the file goes away, or we're in a different directory?
  //   - what if the file is changed, what loc to use then?
  // so for now I'm punting and not saving the loc at all...

  xassert(flattenStrTable);
  flattenStrTable->xfer(flat, str);
}


void LocString::copyAndDel(LocString *obj)
{
  loc = obj->loc;
  str = obj->str;
  delete obj;
}

LocString *LocString::clone(StoragePool &pool) const
{
  return new (pool) LocString(*this);
}

LocString *LocString::clone(StoragePool &pool, int deepness, int listDeepness) const
{
  return new (pool) LocString(*this);
}


bool LocString::equals(char const *other) const
{
  if (!str) {
    return !other;                            // equal if both null
  }
  else {
    return other && streq(str, other); // or same contents
  }
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
