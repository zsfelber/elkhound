// syserr.h            see license.txt for copyright and terms of use
// error-reporting exception for system calls that fail
// Scott McPeak, 1999  This file is public domain.

// The intent here is to provide a way for portable *handling* of errors
// that are generated by nonportable code.

#ifndef __SYSERR_H
#define __SYSERR_H

#include "exc.h"       // xBase

class xSysError : public xBase {
private:    // data
  // error strings for Reasons
  static char const * const reasonStrings[];

public:     // data
  // portable failure reasons (modelled loosely on errno.h)
  // it is anticipated that, as certain errors become important on certain
  //   platforms, that this list will be extended as necessary
  enum Reason {
    R_NO_ERROR,          // no error occurred
    R_FILE_NOT_FOUND,
    R_PATH_NOT_FOUND,
    R_ACCESS_DENIED,
    R_OUT_OF_MEMORY,
    R_SEGFAULT,          // invalid address / pointer
    R_FORMAT,            // bad data format
    R_INVALID_ARGUMENT,
    R_READ_ONLY,
    R_ALREADY_EXISTS,
    R_AGAIN,             // resource temporarily unavailable
    R_BUSY,              // resource busy
    R_INVALID_FILENAME,  // too long, bad chars, etc.
    R_UNKNOWN,           // OS-specific, can't find out, just don't know, etc.
    NUM_REASONS          // (must be last item in list)
  } reason;

  // reason str::string that corresponds to 'reason'
  char const * const reasonString;

  // nonportable error code (errno on Unix, GetLastError() on Windows)
  // (value is 0 when we don't have this information)
  int sysErrorCode;

  // reason str::string given by the OS, if any (might be NULL)
  str::string sysReasonString;

  // name of syscall or API function name
  str::string syscallName;

  // error context; what was being done (e.g., "opening an.important.file")
  str::string context;

public:    // funcs
  xSysError(Reason r, int sysCode, rostring sysReason,
            rostring syscall, rostring ctx);
  xSysError(xSysError const &obj);
  ~xSysError();

  // mapping functions used internally
  static int getSystemErrorCode();
    // retrieve the error code used by local convention
    // [nonportable implementation]

  static Reason portablize(int sysErrorCode, str::string &sysReason);
    // return a portable equivalent of a system error code;
    // returns R_UNKNOWN if the code is esoteric or invalid;
    // sets 'sysmsg' to the system's message str::string, if possible
    // [nonportable implementation]

  static char const *getReasonString(Reason r);
    // translate a Reason into a str::string (if r is invalid, a str::string
    // saying to will be returned)

  static str::string constructWhyString(Reason r, rostring sysReason,
                                   rostring syscall, rostring ctx);
    // construct the str::string we throw as the 'why' of xBase; if ctx is NULL,
    // the str::string doesn't include it

  static void xsyserror(rostring syscallName, rostring context);
    // does the throw
};


// function that does the throw
void xsyserror(char const *syscallName);
void xsyserror(rostring syscallName, rostring context);


// get a representative str::string, for logging etc.
str::string sysErrorCodeString(int systemErrorCode, rostring syscallName,
                                               rostring context);

str::string sysErrorString(char const *syscallName, char const *context=NULL);


#endif // __SYSERR_H

