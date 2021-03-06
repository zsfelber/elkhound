// str.h            see license.txt for copyright and terms of use
// a string class
// the representation uses just one char*, so that a smart compiler
//   can pass the entire object as a single word
// Scott McPeak, 1995-2000  This file is public domain.

// 2005-03-01: See string.txt.  The plan is to evolve the class
// towards compatibility with string, such that eventually
// they will be interchangeable.  So far I have converted only
// the most problematic constructs, those involving construction,
// conversion, and internal pointers.

#ifndef STR_H
#define STR_H

#include "defs.h"
#include "stor0.h"
#include <string.h>

class Flatten;           // flatten.h

// certain unfortunate implementation decisions by some compilers
// necessitate avoiding the name 'string'
//
// 9/19/04: I originally made this definition to work around a problem
// with Borland C++ 4.5.  It causes a problem when using the new
// standard library, since the name clashes with string.  A
// simple solution is to remove the #definition and let namespaces do
// their job.  Since Intel's headers are the only ones that provoke
// the problem I'll confine it to that case for now.  Eventually I
// will do the same for gcc.
//
// 2005-02-28: Let's try getting rid of this.
//
// 2005-03-15: There were some problems on Redhat due to flex-2.5.4a-29.
//             I have solved them differently, but it is worth noting 
//             that re-enabling this #define also fixed the problem.
#if 0   //!defined(__INTEL_COMPILER)
  #define string mystring
#endif


// ------------------------- string ---------------------
// This is used when I want to call a function in smbase::string
// that does not exist or has different semantics in string.
// That way for now I can keep using the function, but it is 
// marked as incompatible.
enum SmbaseStringFunc { SMBASE_STRING_FUNC };



namespace str {

    extern StoragePool str_pool;

    class string : public Storeable {
    protected:     // data
      // 10/12/00: switching to never letting s be NULL
      char *s;     	       	       	       // string contents; never NULL
    public:
      static char * const emptyString;     // a global ""; should never be modified
      static char * const nullString;
      static size_t const npos = std::string::npos;

    protected:     // funcs
      void dup(char const *source);        // copies, doesn't dealloc first
      void kill();                         // dealloc if str != 0

    public:	       // funcs
    #ifdef DEBUG
      string(string const &src);
    #endif
      string(DBG_INFO_FORMAL_FIRST string const &src);
      string(DBG_INFO_FORMAL_FIRST char const *src);
      string(DBG_INFO_FORMAL_FIRST str::StoragePool & pool, string const &src);
      string(DBG_INFO_FORMAL_FIRST str::StoragePool & pool, char const *src);
      string(DBG_INFO_FORMAL_FIRST __StoreAlreadyConstr StoreAlreadyConstr);
      string(DBG_INFO_FORMAL );
      ~string() { kill(); }

      // for this one, use ::substring instead
      string(DBG_INFO_FORMAL_FIRST char const *src, int length/*, SmbaseStringFunc*/);

      // for this one, there are two alternatives:
      //   - stringstream has nearly the same constructor interface
      //     as string had, but cannot export a char* for writing
      //     (for the same reason string can't anymore); operator[] must
      //     be used
      //   - Array<char> is very flexible, but remember to add 1 to
      //     the length passed to its constructor!
      string(DBG_INFO_FORMAL_FIRST int length, SmbaseStringFunc);

      string(DBG_INFO_FORMAL_FIRST Flatten&);
      void xfer(Flatten &flat);

      // simple queries
      int length() const;  	       	// returns number of non-null chars in the string; length of "" is 0
      bool isempty() const { return s[0]==0; }
      bool contains(char c) const;

      // string has this instead; I will begin using slowly
      bool empty() const { return isempty(); }

      // array-like access
      char& operator[] (int i) { return s[i]; }
      char operator[] (int i) const { return s[i]; }

      // substring
      string substring(int startIndex, int length) const;

      // conversions
      #if 0    // removing these for more standard compliace
        //operator char* () { return s; }      // ambiguities...
        operator char const* () const { return s; }
        char *pchar() { return s; }
        char const *pcharc() const { return s; }
      #else
        char const *c_str() const { return s; }
      #endif

      // assignment
      string& operator=(string const &src)
        { if (&src != this) { kill(); dup(src.s); } return *this; }
      string& operator=(char const *src)
        { if (src != s) { kill(); dup(src); } return *this; }

      // allocate 'newlen' + 1 bytes (for null); initial contents is ""
      string& setlength(int newlen);

      // comparison; return value has same meaning as strcmp's return value:
      //   <0   if   *this < src
      //   0    if   *this == src
      //   >0   if   *this > src
      int compareTo(string const &src) const;
      inline int compare(string const &src) const
      { return compareTo(src); }
      int compareTo(char const *src) const;
      bool equals(char const *src) const { return compareTo(src) == 0; }
      bool equals(string const &src) const { return compareTo(src) == 0; }

      #define MAKEOP(op)							       	 \
        bool operator op (string const &src) const { return compareTo(src) op 0; }	 \
        /*bool operator op (const char *src) const { return compareTo(src) op 0; }*/ \
        /* killed stuff with char* because compilers are too flaky; use compareTo */
      MAKEOP(==)  MAKEOP(!=)
      MAKEOP(>=)  MAKEOP(>)
      MAKEOP(<=)  MAKEOP(<)
      #undef MAKEOP

      // concatenation (properly handles string growth)
      // uses '&' instead of '+' to avoid char* coercion problems
      string operator& (string const &tail) const;
      string& operator&= (string const &tail);

      inline string operator+ (string const &tail) const
      { return operator&(tail); }
      inline string& operator& (string const &tail)
      { return operator&=(tail); }

      string operator+ (char const c);
      string& operator+= (char const c);

      // input/output
      friend std::istream& operator>> (std::istream &is, string &obj)
        { obj.readline(is); return is; }
      friend std::ostream& operator<< (std::ostream &os, string const &obj)
        { obj.write(os); return os; }

      // note: the read* functions are currently implemented in a fairly
      // inefficient manner (one char at a time)

      void readdelim(std::istream &is, char const *delim);
        // read from is until any character in delim is encountered; consumes that
        // character, but does not put it into the string; if delim is null or
        // empty, reads until EOF

      void readall(std::istream &is) { readdelim(is, NULL); }
        // read all remaining chars of is into this

      void readline(std::istream &is) { readdelim(is, "\n"); }
        // read a line from input stream; consumes the \n, but doesn't put it into
        // the string

      void write(std::ostream &os) const;
        // writes all stored characters (but not '\0')

      // debugging
      void selfCheck() const;
        // fail an assertion if there is a problem

      void* operator new (size_t size);
      void* operator new (size_t size, str::StoragePool &pool);

      void debugPrint(str::stringstream& os, int indent = 0, char const * subtreeName = 0) const;
    };



    // --------------------- stringstream --------------------
    // this class is specifically for appending lots of things
    class stringstream : public string {
    protected:
      enum { EXTRA_SPACE = 30 };    // extra space allocated in some situations
      char *end;          // current end of the string (points to the NUL character)
      int size;           // amount of space (in bytes) allocated starting at 's'

    protected:
      void init(int initSize);
      void dup(char const *src);

    public:
      stringstream(DBG_INFO_FORMAL_FIRST  int length=0);    // creates an empty string
      stringstream(DBG_INFO_FORMAL_FIRST  char const *str);
      stringstream(DBG_INFO_FORMAL_FIRST  char const *str, int length);
      stringstream(DBG_INFO_FORMAL_FIRST  string const &str) : string(DBG_INFO_ARG_FWD) { dup(str.c_str()); }
      stringstream(DBG_INFO_FORMAL_FIRST  stringstream const &obj) : string(DBG_INFO_ARG_FWD) { dup(obj.c_str()); }
      ~stringstream() {}

      stringstream& operator= (char const *src);
      stringstream& operator= (string const &s) { return operator= (s.c_str()); }
      stringstream& operator= (stringstream const &s) { return operator= (s.c_str()); }

      int length() const { return end-s; }
      bool isempty() const { return length()==0; }

      string & str() { return *this; }

      // unlike 'string' above, I will allow stringstream to convert to
      // char const * so I can continue to use 'stringc' to build strings
      // for functions that accept char const *; this should not conflict
      // with string, since I am explicitly using a different class
      // (namely stringstream) when I use this functionality
      operator char const * () const { return c_str(); }

      stringstream& setlength(int newlen);    // change length, forget current data

      // make sure we can store 'someLength' non-null chars; grow if necessary
      void ensure(int someLength) { if (someLength >= size) { grow(someLength); } }

      // grow the string's length (retaining data); make sure it can hold at least
      // 'newMinLength' non-null chars
      void grow(int newMinLength);

      // this can be useful if you modify the string contents directly..
      // it's not really the intent of this class, though
      void adjustend(char* newend);

      // remove characters from the end of the string; 'newLength' must
      // be at least 0, and less than or equal to current length
      void truncate(int newLength);

      // make the string be the empty string, but don't change the
      // allocated space
      void clear() { adjustend(s); }

      // concatenation, which is the purpose of this class
      stringstream& operator&= (char const *tail);

      // useful for appending substrings or strings with NUL in them
      void append(char const *tail, int length);

      // append a given number of spaces; meant for contexts where we're
      // building a multi-line string; returns '*this'
      stringstream& indent(int amt);

      // sort of a mixture of Java compositing and C++ i/o strstream
      inline stringstream& operator << (rostring &text) { return operator&=(text.c_str()); }
      inline stringstream& operator << (char const *text) { return operator&=(text); }
      stringstream& operator << (char c);
      inline stringstream& operator << (unsigned char c) { return operator<<((char)c); }
      stringstream& operator << (long i);
      stringstream& operator << (unsigned long i);
      inline stringstream& operator << (int i) { return operator<<((long)i); }
      inline stringstream& operator << (unsigned i) { return operator<<((unsigned long)i); }
      inline stringstream& operator << (short i) { return operator<<((long)i); }
      inline stringstream& operator << (unsigned short i) { return operator<<((long)i); }
      inline stringstream& operator << (size_t i)  { return operator<<((void*)i); }
      stringstream& operator << (double d);
      stringstream& operator << (void *ptr);     // inserts address in hex
      #ifndef LACKS_BOOL
        inline stringstream& operator << (bool b) { return operator<<((long)b); }
      #endif // LACKS_BOOL

      // useful in places where long << expressions make it hard to
      // know when arguments will be evaluated, but order does matter
      typedef stringstream& (*Manipulator)(stringstream &sb);
      stringstream& operator<< (Manipulator manip);

      // stream readers
      friend std::istream& operator>> (std::istream &is, stringstream &sb)
        { sb.readline(is); return is; }
      void readall(std::istream &is) { readdelim(is, NULL); }
      void readline(std::istream &is) { readdelim(is, "\n"); }

      void readdelim(std::istream &is, char const *delim);

      // an experiment: hex formatting (something I've sometimes done by resorting
      // to sprintf in the past)
      class Hex {
      public:
        unsigned long value;

        Hex(unsigned long v) : value(v) {}
        Hex(Hex const &obj) : value(obj.value) {}
      };
      stringstream& operator<< (Hex const &h);
      #define SBHex stringstream::Hex
    };



    // ------------------------ rostring &----------------------
    // My plan is to use this in places I currently use 'char const *'.
    // TODO fail-safe

    // I have the modest hope that the transition to 'rostring' might be
    // reversible, so this function converts to 'char const *' but with a
    // syntax that could just as easily apply to 'char const *' itself
    // (and in that case would be the identity function).
    inline char const *toCStr(rostring &s) { return s.c_str(); }

    // at the moment, if I do this it is a mistake, so catch it; this
    // function is not implemented anywhere
    void/*unusable*/ toCStr(char const *s);

    // I need some compatibility functions
    inline int strlen(rostring &s) { return s.length(); }

    inline int cmp(rostring &s1, rostring &s2)
      { return strcmp(s1.c_str(), s2.c_str()); }
    inline int cmp(rostring &s1, char const *s2)
      { return strcmp(s1.c_str(), s2); }
    inline int cmp(char const *s1, rostring &s2)
      { return strcmp(s1, s2.c_str()); }
    // string.h, above, provides:
    // int strcmp(char const *s1, char const *s2);

    // dsw: this is what we are asking most of the time so let's special
    // case it
    inline bool streq(rostring &s1, rostring &s2)       {return cmp(s1, s2) == 0;}
    inline bool streq(rostring &s1, char const *s2)    {return cmp(s1, s2) == 0;}
    inline bool streq(char const *s1, rostring &s2)    {return cmp(s1, s2) == 0;}
    inline bool streq(char const *s1, char const *s2) {return strcmp(s1, s2) == 0;}

    char const *strstr(rostring &haystack, char const *needle);

    // there is no wrapper for 'strchr'; use string::contains

    int atoi(rostring &s);

    // construct a string out of characters from 'p' up to 'p+n-1',
    // inclusive; resulting string length is 'n'
    str::string substring(char const *p, int n);
    inline str::string substring(rostring &p, int n)
      { return substring(p.c_str(), n); }


    // ---------------------- misc utils ------------------------
    // the real strength of this entire module: construct strings in-place
    // using the same syntax as C++ iostreams.  e.g.:
    //   puts(stringb("x=" << x << ", y=" << y));
    //#define stringb(expr) (stringstream(DBG_INFO_ARG0) << expr)
    // TODO fail safe
    #define stringb(expr) (str::stringstream() << expr)

    // experimenting with dropping the () in favor of <<
    // (the "c" can be interpreted as "constructor", or maybe just
    // the successor to "b" above)
    // TODO fail-safe
    //#define stringc stringstream()

    // experimenting with using toString as a general method for datatypes
    str::string toString(int i);
    str::string toString(unsigned i);
    str::string toString(char c);
    str::string toString(long i);
    str::string toString(char const *str);
    str::string toString(float f);
    str::string toString(char const *str);
    void debugString(std::ostream &os, int i, int level);
    void debugString(std::ostream &os, unsigned i, int level);
    void debugString(std::ostream &os, char i, int level);
    void debugString(std::ostream &os, long i, int level);
    void debugString(std::ostream &os, char const * i, int level);
    void debugString(std::ostream &os, float i, int level);

    // printf-like construction of a string; often very convenient, since
    // you can use any of the formatting characters (like %X) that your
    // libc's sprintf knows about
    str::string stringf(char const *format, ...);
    str::string vstringf(char const *format, va_list args);
}


#include "typ.h"         // bool
#include "storage.h"


#endif // STR_H
