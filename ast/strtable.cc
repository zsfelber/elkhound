// strtable.cc            see license.txt for copyright and terms of use
// code for strtable.h

#include "strtable.h"    // this module
#include "xassert.h"     // xassert
#include "flatten.h"     // Flatten

#include <string.h>      // strlen


rostring StringTable::add(StringRef src) {
    int16_t hash = src ? src[0] : 0;
    if (hash) hash += src[1];
    Buf& itm = tbl[hash];
    size_t cap = str::getPtrBufSize(itm.sz);
    if (cap > itm.cap) {
        str::extendBuffer(itm.arr, itm.sz, itm.cap, cap);
    }
    E srch(src);
    E* r;
    if (!str::binary_insert(itm.arr,itm.arr+itm.sz, srch, r)) {
        str::insertBufferItem(itm.arr, itm.sz, r);
        *r = srch;
        r->rs = new string(src);
        r->s = r->rs->c_str();
    }
    return *r->rs;
}

/*
StringTable *flattenStrTable = NULL;


STATICDEF char const *StringTable::identity(void *data)
{
  return (char const*)data;
}


StringTable::StringTable()
  : hash(identity),
    racks(NULL),
    longStrings(NULL)
{}


StringTable::~StringTable()
{
  clear();
}

void StringTable::clear()
{
  hash.empty();
  
  while (racks != NULL) {
    Rack *temp = racks;
    racks = racks->next;
    delete temp;
  }

  while (longStrings != NULL) {
    LongString *temp = longStrings;
    longStrings = longStrings->next;
    delete temp->data;
    delete temp;
  }
}


StringRef StringTable::add(char const *src)
{
  if (!src) return NULL;

  // see if it's already here
  StringRef ret = get(src);
  if (ret) {
    return ret;
  }

  int len = strlen(src)+1;     // include null terminator

  // is it a long string?
  if (len >= longThreshold) {
    char *d = new char[len];
    ret = d;
    memcpy(d, src, len);

    // prepend a new long-string entry
    longStrings = new LongString(longStrings, d);
  }

  else {
    // see if we need a new rack
    if (!racks || len > racks->availBytes()) {
      // need a new rack
      xassert(len <= rackSize);
      racks = new Rack(racks);     // prepend new rack
    }

    // add the string to the last rack
    ret = racks->nextByte();
    memcpy(racks->nextByte(), src, len);
    racks->usedBytes += len;
  }

  // enter the intended location into the indexing structures
  hash.add(ret, (void*)ret);

  return ret;
}


StringRef StringTable::get(char const *src) const
{
  return (StringRef)hash.get(src);
}

*/

                                        
// for now, just store each instance separately ...
void StringTable::xfer(Flatten &flat, string const *& ref)
{
  if (flat.reading()) {
    // read the string
    char *str;
    flat.xferCharString(str);

    if (str) {
      // add to table
      ref = &add(str);

      // delete string's storage
      delete str;
    }
    else {
      // was NULL
      ref = NULL;
    }
  }
  
  else {
    // copy it to disk
    // since we're writing, the cast is ok
    char const * r = ref->c_str();
    flat.xferCharString(const_cast<char*&>(r));
  }
}
