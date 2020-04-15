#ifndef DEFS_H
#define DEFS_H


class VoidList;
class VoidTailList;
class VoidNode;
class GrammarAnalysis;
namespace str {
class string;
class stringstream;
class Storeable;
class StoragePool;
static char const * flush = "";
static char const * endl = "\n";
}
typedef str::string const rostring;

#define DEBUG_MAX_IND 25

#define S1(x) #x
#define S2(x) S1(x)
#define __FILE_LINE__ __FILE__ " : " S2(__LINE__)

#define DO_EXPAND(VAL)  VAL ## 0
#define EXPAND(VAL)     DO_EXPAND(VAL)

#ifdef DEBUG
#define DBG_INFO_FORMAL __DbgStr const objectName
#define DBG_INFO_FORMAL_FIRST __DbgStr const objectName,
#define DBG_INFO_ARG_FWD objectName
#define DBG_INFO_ARG_FWD_FIRST objectName,
#define DBG_INFO_ARG0 __DbgStr(__FILE_LINE__)
#define DBG_INFO_ARG0_SOLE (DBG_INFO_ARG0)
#define DBG_INFO_ARG0_FIRST __DbgStr(__FILE_LINE__),
#define DBG_INFO_FWD(a) a
#define DBG_INFO_FWD_COM(a) a,
#define DBG_INFO_FWD_PCOM(a) ,a
#else
#define DBG_INFO_FORMAL
#define DBG_INFO_FORMAL_FIRST
#define DBG_INFO_ARG_FWD
#define DBG_INFO_ARG_FWD_FIRST
#define DBG_INFO_ARG0
#define DBG_INFO_ARG0_SOLE
#define DBG_INFO_ARG0_FIRST
#define DBG_INFO_FWD(a)
#define DBG_INFO_FWD_COM(a)
#define DBG_INFO_FWD_PCOM(a)
#endif

#ifdef REG_CHILD
#define REG_CHILD_COMMA ,
#else
#define REG_CHILD_COMMA
#endif

#ifdef DEBUG
#define NDEBUG_COLON
#else
#define NDEBUG_COLON :
#endif

#if (defined(_MSC_VER))
#define OPTL0 __pragma(optimize("gts", on))
#define OPTL
#define OPT inline
#elif (defined(__MINGW32__))
#define OPTL0
#define OPTL
#define OPT inline
#elif (defined(__GNUC__))
#define OPTL0
#define OPTL __attribute__((optimize("O2")))
#define OPT inline OPTL
#else

#endif






// byte
typedef unsigned char byte;
typedef signed char signed_byte;

// int32 used to be here, but defined nonportably, and I don't use
// it anyway, so I ripped it out


// NULL
#ifndef NULL
#  define NULL 0
#endif // NULL


// bool
#ifdef LACKS_BOOL
  typedef int bool;
  bool const false=0;
  bool const true=1;
#endif // LACKS_BOOL



// min, max
#undef min
#undef max

template <class T>
inline T min(T const &a, T const &b)
{
  return a<b? a:b;
}

template <class T>
inline T max(T const &a, T const &b)
{
  return a>b? a:b;
}


#if 0   // old
  #ifndef __MINMAX_DEFINED
  # ifndef min
  #  define min(a,b) ((a)<(b)?(a):(b))
  # endif
  # ifndef max
  #  define max(a,b) ((a)>(b)?(a):(b))
  # endif
  # define __MINMAX_DEFINED
  #endif // __MINMAX_DEFINED
#endif // 0


// tag for definitions of static member functions; there is no
// compiler in existence for which this is useful, but I like
// to see *something* next to implementations of static members
// saying that they are static, and this seems slightly more
// formal than just a comment
#define STATICDEF /*static*/


// often-useful number-of-entries function
#define TABLESIZE(tbl) ((int)(sizeof(tbl)/sizeof((tbl)[0])))


// concise way to loop on an integer range
#define loopi(end) for(int i=0; i<(int)(end); i++)
#define loopj(end) for(int j=0; j<(int)(end); j++)
#define loopk(end) for(int k=0; k<(int)(end); k++)


// for using selfCheck methods
// to explicitly check invariants in debug mode
//
// dsw: debugging *weakly* implies selfchecking: if we are debugging,
// do selfcheck unless otherwise specified
#ifndef NDEBUG
  #ifndef DO_SELFCHECK
    #define DO_SELFCHECK 1
  #endif
#endif
// dsw: selfcheck *bidirectionally* configurable from the command line: it
// may be turned on *or* off: any definition other than '0' counts as
// true, such as -DDO_SELFCHECK=1 or just -DDO_SELFCHECK
#ifndef DO_SELFCHECK
  #define DO_SELFCHECK 0
#endif
#if DO_SELFCHECK != 0
  #define SELFCHECK() selfCheck()
#else
  #define SELFCHECK() ((void)0)
#endif


// division with rounding towards +inf
// (when operands are positive)
template <class T>
inline T div_up(T const &x, T const &y)
{ return (x + y - 1) / y; }


// mutable
#ifdef __BORLANDC__
#  define MUTABLE
#  define EXPLICIT
#else
#  define MUTABLE mutable
#  define EXPLICIT explicit
#endif


#define SWAP(a,b) \
  temp = a;       \
  a = b;          \
  b = temp /*user supplies semicolon*/


// verify something is true at compile time (will produce
// a compile error if it isn't)
// update: use STATIC_ASSERT defined in macros.h instead
//#define staticAssert(cond) extern int dummyArray[cond? 1 : 0]


// this functions accepts raw 'char const *' instead of 'rostring'
// because I do not want this interface to depend on str.h, and also
// because I do not want the many call sites to have the overhead
// of constructing and destructing temporary objects

int x_assert_fail(const char * cond, const char * file, int line);// NORETURN;
int x_assert_fail(rostring &cond, rostring &file, int line);// NORETURN;


#endif // DEFS_H
