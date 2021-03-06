// str.cpp            see license.txt for copyright and terms of use
// code for str.h
// Scott McPeak, 1995-2000  This file is public domain.

#include "str.h"            // this module

#include <iostream>	 // istream, ostream
#include <stdarg.h>      // va_list
#include <string.h>      // strcmp, etc.

#include <stdlib.h>         // atoi
#include <stdio.h>          // sprintf
#include <ctype.h>          // isspace
#include <string.h>         // strcmp
#include <iostream>       // std::ostream << char*
#include <assert.h>         // assert
#ifdef _MSC_VER
#include <io.h>         // write
#else
#include <unistd.h>         // write
#endif
#include <iostream>

#include "storage.h"            // this module
#include "xassert.h"        // xassert
#include "ckheap.h"         // checkHeapNode
#include "flatten.h"        // Flatten
#include "nonport.h"        // vnprintf
#include "array.h"          // Array





int x_assert_fail(const char * cond, const char * file, int line) {
    return x_assert_fail(str::string(cond), str::string(file), line);
}

namespace str {

StoragePool str_pool DBG_INFO_ARG0_SOLE;

// ----------------------- string ---------------------

// put the empty string itself in read-only memory
char const nul_byte = 0;

// deliberately cast away the constness; I cannot declare
// 'emptyString' to be const because it gets assigned to 's', but it
// is nevertheless the intent that I never modify 'nul_byte'
char * const string::emptyString = const_cast<char*>(&nul_byte);
char * const string::nullString = const_cast<char*>("<null>");

#ifdef DEBUG
string::string(string const &src) : Storeable(DBG_INFO_ARG0_FIRST  str_pool) { if (src.s) dup(src.s); else s = nullString; }
#endif
string::string(DBG_INFO_FORMAL_FIRST string const &src) : Storeable(DBG_INFO_ARG_FWD_FIRST  str_pool) { if (src.s) dup(src.s); else s = nullString; }
string::string(DBG_INFO_FORMAL_FIRST char const *src) : Storeable(DBG_INFO_ARG_FWD_FIRST  str_pool) { if (src) dup(src); else s = nullString; }
string::string(DBG_INFO_FORMAL_FIRST str::StoragePool & pool, string const &src) : Storeable(DBG_INFO_ARG_FWD_FIRST  pool) { if (src.s) dup(src.s); else s = nullString; }
string::string(DBG_INFO_FORMAL_FIRST str::StoragePool & pool, char const *src) : Storeable(DBG_INFO_ARG_FWD_FIRST  pool) { if (src) dup(src); else s = nullString; }
string::string(DBG_INFO_FORMAL_FIRST __StoreAlreadyConstr StoreAlreadyConstr) : Storeable(DBG_INFO_ARG_FWD_FIRST  StoreAlreadyConstr) {  }
string::string(DBG_INFO_FORMAL ) : Storeable(DBG_INFO_ARG_FWD_FIRST  str_pool) { s=emptyString; }


string::string(DBG_INFO_FORMAL_FIRST  char const *src, int length/*, SmbaseStringFunc*/) : str::Storeable(DBG_INFO_ARG_FWD)
{
  s=emptyString;
  setlength(length);       // setlength already has the +1; sets final NUL
  memcpy(s, src, length);
}
string::string(DBG_INFO_FORMAL_FIRST int length, SmbaseStringFunc) : Storeable(DBG_INFO_ARG_FWD) { s=emptyString; setlength(length); }

void* string::operator new (size_t size) {
    return str::Storeable::operator new(size, str_pool);
}

void* string::operator new (size_t size, str::StoragePool &pool) {
    return str::Storeable::operator new(size, pool);
}


void string::dup(char const *src)
{
  if (!src) {
    s = nullString;
  } else if (src[0]==0) {
    s = emptyString;
  }
  else {
    s = new char[ strlen(src) + 1 ];
    xassert(s);
    strcpy(s, src);
  }
}

void string::kill()
{
  if (s != emptyString && s != nullString) {
    delete s;
  }
}


string::string(DBG_INFO_FORMAL_FIRST  Flatten&)
  : str::Storeable(DBG_INFO_ARG_FWD), s(emptyString)
{}

void string::xfer(Flatten &flat)
{
  flat.xferCharString(s);
}


int string::length() const
{
  xassert(s);
  return strlen(s);
}

bool string::contains(char c) const
{
  xassert(s);
  return !!strchr(s, c);
}


string string::substring(int startIndex, int len) const
{
  xassert(startIndex >= 0 &&
          len >= 0 &&
          startIndex + len <= length());

  return str::substring(s+startIndex, len);
}


string &string::setlength(int length)
{
  kill();
  if (length > 0) {
    s = new char[ length+1 ];
    xassert(s);
    s[length] = 0;      // final NUL in expectation of 'length' chars
    s[0] = 0;           // in case we just wanted to set allocated length
  }
  else {
    xassert(length == 0);     // negative wouldn't make sense
    s = emptyString;
  }
  return *this;
}


int string::compareTo(string const &src) const
{
  return compareTo(src.s);
}

int string::compareTo(char const *src) const
{
  if (src == NULL) {
    src = emptyString;
  }
  return strcmp(s, src);
}


string string::operator&(string const &tail) const
{
  string dest(DBG_INFO_ARG0_FIRST  length() + tail.length(), SMBASE_STRING_FUNC);
  strcpy(dest.s, s);
  strcat(dest.s, tail.s);
  return dest;
}

string& string::operator&=(string const &tail)
{
  return *this = *this & tail;
}

string string::operator+(char const c)
{
    string dest(DBG_INFO_ARG0_FIRST  length() + 1, SMBASE_STRING_FUNC);
    strcpy(dest.s, s);
    dest.s[length()] = c;
    return dest;
}

string& string::operator+=(char const c)
{
    return *this = *this + c;
}

void string::readdelim(std::istream &is, char const *delim)
{
  stringstream sb  DBG_INFO_ARG0_SOLE;
  sb.readdelim(is, delim);
  operator= (sb);
}


void string::write(std::ostream &os) const
{
  os << (s?s:"#null");     // standard char* writing routine
}


void string::selfCheck() const
{
  if (s != emptyString) {
    checkHeapNode(s);
  }
}

void string::debugPrint(str::stringstream& os, int indent, char const * subtreeName) const
{
    str::ind(os,indent)<< "str:"<< (s?s:"#null");
}


// ----------------------- rostring &---------------------


char const *strstr(rostring &haystack, char const *needle)
{
  return strstr(haystack.c_str(), needle);
}


int atoi(rostring &s)
{
  return atoi(toCStr(s));
}

string substring(char const *p, int n)
{
  return string(DBG_INFO_ARG0_FIRST  p, n/*, SMBASE_STRING_FUNC*/);
}


// --------------------- stringstream ------------------
stringstream::stringstream(DBG_INFO_FORMAL_FIRST  int len) : string(DBG_INFO_ARG_FWD)
{
  init(len);
}

void stringstream::init(int initSize)
{
  size = initSize + EXTRA_SPACE + 1;     // +1 to be like string::setlength
  s = new char[size];
  end = s;
  end[initSize] = 0;
}


void stringstream::dup(char const *str)
{
  int len = strlen(str);
  init(len);
  strcpy(s, str);
  end += len;
}


stringstream::stringstream(DBG_INFO_FORMAL_FIRST  char const *str) : string(DBG_INFO_ARG_FWD)
{
  dup(str);
}


stringstream::stringstream(DBG_INFO_FORMAL_FIRST  char const *str, int len) : string(DBG_INFO_ARG_FWD)
{
  init(len);
  memcpy(s, str, len);
  end += len;
}


stringstream& stringstream::operator=(char const *src)
{
  if (s != src) {
    kill();
    dup(src);
  }
  return *this;
}


stringstream& stringstream::setlength(int newlen)
{
  kill();
  init(newlen);
  return *this;
}


void stringstream::adjustend(char* newend)
{
  xassert(s <= newend  &&  newend < s + size);

  end = newend;
  *end = 0;        // sm 9/29/00: maintain invariant
}


void stringstream::truncate(int newLength)
{
  xassert(0 <= newLength && newLength <= length());
  adjustend(s + newLength);
}


stringstream& stringstream::operator&= (char const *tail)
{
  append(tail, strlen(tail));
  return *this;
}

void stringstream::append(char const *tail, int len)
{
  ensure(length() + len);

  memcpy(end, tail, len);
  end += len;
  *end = 0;
}


stringstream& stringstream::indent(int amt)
{
  xassert(amt >= 0);
  ensure(length() + amt);

  memset(end, ' ', amt);
  end += amt;
  *end = 0;
  
  return *this;
}


void stringstream::grow(int newMinLength)
{
  // I want at least EXTRA_SPACE extra
  int newMinSize = newMinLength + EXTRA_SPACE + 1;         // compute resulting allocated size

  // I want to grow at the rate of at least 50% each time
  int suggest = size * 3 / 2;

  // see which is bigger
  newMinSize = max(newMinSize, suggest);

  // remember old length..
  int len = length();

  // realloc s to be newMinSize bytes
  char *temp = new char[newMinSize];
  xassert(len+1 <= newMinSize);    // prevent overrun
  memcpy(temp, s, len+1);          // copy null too
  if (s != nullString && s != emptyString) {
      delete s;
  }
  s = temp;

  // adjust other variables
  end = s + len;
  size = newMinSize;
}


stringstream& stringstream::operator<< (char c)
{
  ensure(length() + 1);
  *(end++) = c;
  *end = 0;
  return *this;
}


#define MAKE_LSHIFT(Argtype, fmt)                        \
  stringstream& stringstream::operator<< (Argtype arg) \
  {                                                      \
    char buf[60];      /* big enough for all types */    \
    int len = sprintf(buf, fmt, arg);                    \
    if (len >= 60) {					 \
      abort();    /* too big */                          \
    }                                                    \
    return *this << buf;                                 \
  }

MAKE_LSHIFT(long, "%ld")
MAKE_LSHIFT(unsigned long, "%lu")
MAKE_LSHIFT(double, "%g")
MAKE_LSHIFT(void*, "%p")

#undef MAKE_LSHIFT


stringstream& stringstream::operator<< (
  stringstream::Hex const &h)
{
  char buf[32];        // should only need 19 for 64-bit word..
  int len = sprintf(buf, "0x%lX", h.value);
  if (len >= 20) {
    abort();
  }
  return *this << buf;

  // the length check above isn't perfect because we only find out there is
  // a problem *after* trashing the environment.  it is for this reason I
  // use 'assert' instead of 'xassert' -- the former calls abort(), while the
  // latter throws an exception in anticipation of recoverability
}


stringstream& stringstream::operator<< (Manipulator manip)
{
  return manip(*this);
}


// slow but reliable
void stringstream::readdelim(std::istream &is, char const *delim)
{
  char c;
  is.get(c);
  while (!is.eof() &&
         (!delim || !strchr(delim, c))) {
    *this << c;
    is.get(c);
  }
}


// ---------------------- toString ---------------------
#define TOSTRING(type)        \
  string toString(type val)   \
  {                           \
    stringstream sis;         \
    sis << val;               \
    return sis.str();         \
  }                           \
  void debugString(std::ostream &os, type i, int level) { \
    for (int a=0; a<level; a++) os <<  "  ";              \
    os <<  i;                                             \
  }                                                       \

TOSTRING(int)
TOSTRING(unsigned)
TOSTRING(char)
TOSTRING(long)
TOSTRING(float)

#undef TOSTRING

// this one is more liberal than 'stringc << null' because it gets
// used by the PRINT_GENERIC macro in my astgen tool
string toString(char const *str)
{
  if (!str) {
    return string("(null)");
  }
  else {
    return string(str);
  }
}




// ------------------- stringf -----------------
string stringf(char const *format, ...)
{
  va_list args;
  va_start(args, format);
  string ret = vstringf(format, args);
  va_end(args);
  return ret;
}
 

// this should eventually be put someplace more general...
#ifndef va_copy
  #ifdef __va_copy
    #define va_copy(a,b) __va_copy(a,b)
  #else
    #define va_copy(a,b) (a)=(b)
  #endif
#endif


string vstringf(char const *format, va_list args)
{
  // estimate string length
  va_list args2;
  va_copy(args2, args);
  int est = vnprintf(format, args2);
  va_end(args2);

  // allocate space
  Array<char> buf(est+1);

  // render the string
  int len = vsprintf(buf, format, args);

  // check the estimate, and fail *hard* if it was low, to avoid any
  // possibility that this might become exploitable in some context
  // (do *not* turn this check off in an NDEGUG build)
  if (len > est) {
    // don't go through fprintf, etc., because the state of memory
    // makes that risky
    static char const msg[] =
      "fatal error: vnprintf failed to provide a conservative estimate,\n"
      "memory is most likely corrupted\n";
    write(2 /*stderr*/, msg, strlen(msg));
    abort();
  }

  // happy
  return string(buf);
}


// ------------------ test code --------------------
#ifdef TEST_STR

#include <iostream>    // cout

void test(unsigned long val)
{
  //cout << stringb(val << " in hex: 0x" << stringstream::Hex(val)) << endl;

  std::cout << stringb(val << " in hex: " << SBHex(val)) << std::endl;
}

int main()
{
  // for the moment I just want to test the hex formatting
  test(64);
  test(0xFFFFFFFF);
  test(0);
  test((unsigned long)(-1));
  test(1);

  std::cout << "stringf: " << stringf("int=%d hex=%X str=%s char=%c float=%f",
                                 5, 0xAA, "hi", 'f', 3.4) << std::endl;

  std::cout << "tests passed\n";

  return 0;
}


#endif // TEST_STR


}// namespace
