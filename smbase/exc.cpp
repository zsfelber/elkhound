// exc.cc            see license.txt for copyright and terms of use
// code for exc.h
// Scott McPeak, 1996-1998  This file is public domain.

#include "exc.h"          // this module

#include <string.h>       // strlen, strcpy
#include <iostream>       // clog
#include <stdarg.h>       // va_xxx
#include <ctype.h>        // toupper, tolower
#include <sstream>


// ------------------------- xBase -----------------
bool xBase::logExceptions = true;
int xBase::creationCount = 0;


xBase::xBase(rostring m)
  : msg(m)
{
  if (logExceptions) {
    std::clog << "Exception thrown: " << m << std::endl;
  }

  // done at very end when we know this object will
  // successfully be created
  creationCount++;
}


xBase::xBase(xBase const *obj)
  : msg(obj->msg)
{
  creationCount++;
}


xBase::~xBase()
{
  creationCount--;
}


// this is obviously not perfect, since exception objects can be
// created and not thrown; I heard the C++ standard is going to,
// or already does, include (by this name?) a function that does this
// correctly; until then, this will serve as a close approximation
// (this kind of test is, IMO, not a good way to handle the underlying
// problem, but it does reasonably handle 70-90% of the cases that
// arise in practice, so I will endorse it for now)
bool unwinding()
{
  return xBase::creationCount != 0;
}


// tweaked version
bool unwinding_other(xBase const &)
{
  // we know the passed xBase exists.. any others?
  return xBase::creationCount > 1;
}


void xBase::insert(std::ostream &os) const
{
  os << why();
}


void xbase(rostring msg)
{
  xBase x(msg);
  THROW(x);
}


void xBase::addContext(rostring context)
{
  // for now, fairly simple
  msg = stringb( "while " << context << ",\n" << msg ).str();
}


// ------------------- x_assert -----------------
x_assert::x_assert(char const* cond, char const* fname, int line)
  : xBase(stringb(
      "Assertion failed: " << cond <<
      ", file " << fname <<
      " line " << line).str().c_str())
    ,condition(cond),
    filename(fname),
    lineno(line)
{}

x_assert::x_assert(rostring cond, rostring fname, int line)
  : xBase(stringb(
      "Assertion failed: " << cond <<
      ", file " << fname <<
      " line " << line).str().c_str()),
    condition(cond),
    filename(fname),
    lineno(line)
{}

x_assert::x_assert(x_assert const &obj)
  : xBase(obj),
    condition(obj.condition),
    filename(obj.filename),
    lineno(obj.lineno)
{}

x_assert::~x_assert()
{}


// failure function, declared in xassert.h
int x_assert_fail(char const *cond, char const *file, int line)
{
  THROW(x_assert(cond, file, line));
  return 0;
}


// --------------- xFormat ------------------
xFormat::xFormat(rostring cond)
  : xBase(stringb("Formatting error: " << cond).str().c_str()),
    condition(cond)
{}

xFormat::xFormat(xFormat const &obj)
  : xBase(obj),
    condition(obj.condition)
{}

xFormat::~xFormat()
{}


void xformat(rostring condition)
{
  xFormat x(condition);
  THROW(x);
}

void formatAssert_fail(char const *cond, char const *file, int line)
{
  xFormat x(stringb ("format assertion failed, "
                    << file << ":" << line << ": "
                    << cond).str());
  THROW(x);
}


// -------------------- XOpen -------------------
XOpen::XOpen(rostring fname)
  : xBase(stringb("failed to open file: " << fname).str()),
    filename(fname)
{}

XOpen::XOpen(XOpen const &obj)
  : xBase(obj),
    DMEMB(filename)
{}

XOpen::~XOpen()
{}


void throw_XOpen(rostring fname)
{
  XOpen x(fname);
  THROW(x);
}


// -------------------- XOpenEx ---------------------
XOpenEx::XOpenEx(rostring fname, rostring m, rostring c)
  : XOpen(fname),
    mode(m),
    cause(c)
{
  msg = stringb("failed to open file \"" << fname
                << "\" for " << interpretMode(mode)
                << ": " << cause).str();
}

XOpenEx::XOpenEx(XOpenEx const &obj)
  : XOpen(obj),
    DMEMB(mode),
    DMEMB(cause)
{}

XOpenEx::~XOpenEx()
{}

//TODO
STATICDEF std::string XOpenEx::interpretMode(rostring mode)
{
  if (mode[0]=='r') {
    if (mode[1]=='+') {
      return "reading and writing";
    }
    else {
      return "reading";
    }
  }

  if (mode[0]=='w') {
    if (mode[1]=='+') {
      return "reading and writing";
    }
    else {
      return "writing";
    }
  }

  if (mode[0]=='a') {
    if (mode[1]=='+') {
      return "reading and appending";
    }
    else {
      return "appending";
    }
  }

  return stringb("(unknown action mode \"" << mode << "\")").str();
}


void throw_XOpenEx(rostring fname, rostring mode, rostring cause)
{
  XOpenEx x(fname, mode, cause);
  THROW(x);
}


// -------------------- XUnimp -------------------
XUnimp::XUnimp(rostring msg)
  : xBase(stringb("unimplemented: " << msg).str())
{}

XUnimp::XUnimp(XUnimp const &obj)
  : xBase(obj)
{}

XUnimp::~XUnimp()
{}


void throw_XUnimp(rostring msg)
{
  XUnimp x(msg);
  THROW(x);
}


void throw_XUnimp(char const *msg, char const *file, int line)
{
  throw_XUnimp(stringb(file << ":" << line << ": " << msg).str());
}


// -------------------- XFatal -------------------
// That this error is "fatal" need not be stated in the error message
// itself.  Doing so would unnecessarily alarm novice users, and the
// fatal-ness is sufficiently expressed by the fact that an exception
// is thrown, as opposed to simply printing the message and continuing.
XFatal::XFatal(rostring msg)
  : xBase(stringb(  "error: " << msg).str())
{}

XFatal::XFatal(XFatal const &obj)
  : xBase(obj)
{}

XFatal::~XFatal()
{}


void throw_XFatal(rostring msg)
{
  XFatal x(msg);
  THROW(x);
}


// ---------------- test code ------------------
#ifdef TEST_EXC

int main()
{
  xBase x("yadda");
  cout << x << std::endl;

  try {
    THROW(x);
  }
  catch (xBase &x) {
    cout << "caught xBase: " << x << std::endl;
  }

  return 0;
}

#endif // TEST_EXC

