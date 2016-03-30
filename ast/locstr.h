// locstr.h            see license.txt for copyright and terms of use
// location & string table reference

#ifndef LOCSTR_H
#define LOCSTR_H
                                          
#include <iostream>    // std::ostream
#include <string.h>      // strlen

#include "strtable.h"    // StringRef
#include "srcloc.h"      // SourceLoc
#include "storage.h"      // SourceLoc

class LocString : public Storeable {
public:    // data
  SourceLoc loc;
  StringRef str;

public:    // funcs
  LocString(StoragePool &pool);
  LocString(LocString const &obj);
  LocString(Storeable const &parent);
  LocString(StoragePool &pool, SourceLoc loc, StringRef str);

  LocString(StoragePool &pool, Flatten&);
  LocString(Storeable const &parent, Flatten&);
  void xfer(StoragePool &pool, Flatten &flat);

  // deallocates its argument; intended for convenient use in bison grammar files
  //EXPLICIT LocString(LocString *obj) : Storeable(obj) { copyAndDel(obj); }
  //void copyAndDel(LocString *obj);

  // sometimes useful for generating arguments to the above ctor
  LocString *clone(StoragePool &pool) const;
  LocString *clone(StoragePool &pool, int deepness, int listDeepness) const;

  LocString& operator= (LocString const &obj)
    { loc = obj.loc; str = obj.str; return *this; }

  // string with location info
  string locString() const { return toString(loc); }

  // (read-only) string-like behavior
  friend std::ostream& operator<< (std::ostream &os, LocString const &loc)
    { return os << loc.str; }
  friend stringBuilder& operator<< (stringBuilder &sb, LocString const &loc)
    { return sb << loc.str; }
  StringRef strref() const { return str; }
  operator StringRef () const { return str; }
  char operator [] (int index) const { return str[index]; }
  bool equals(char const *other) const;    // string comparison
  int length() const { return strlen(str); }

  // experimenting with allowing 'str' to be null, which is convenient
  // when the string table isn't available
  bool isNull() const { return str == NULL; }
  bool isNonNull() const { return !isNull(); }
  
  bool validLoc() const { return loc != SL_UNKNOWN; }
};

// yields simply the string, no location info
string toString(LocString const &s);

// xml stuff...
string toXml(LocString op);
void fromXml(LocString &out, rostring str);


// useful for constructing literal strings in source code
#define LITERAL_LOCSTRING(str)                                   \
  LocString(HERE_SOURCELOC, str)


#endif // LOCSTR_H
