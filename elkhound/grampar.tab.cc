/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 6 "grampar.y" /* yacc.c:339  */


#include "grampar.h"        // yylex, etc.
#include "gramast.ast.gen.h"// grammar syntax AST definition
#include "gramlex.h"        // GrammarLexer
#include "owner.h"          // Owner

#include <stdlib.h>         // malloc, free
#include <iostream>       // cout

// enable debugging the parser
#ifndef NDEBUG
  #define YYDEBUG 1
#endif

// name of extra parameter to yylex
// removed -> %parse-param %lex-param %param https://lwn.net/Articles/561990/
//#define YYLEX_PARAM parseParam

// make it call my yylex
#define yylex(lv, param) grampar_yylex(lv, param)

// Bison calls yyerror(msg) on error; we need the extra
// parameter too, so the macro shoehorns it in there
// bison2:
// #define yyerror(msg) grampar_yyerror(msg, YYPARSE_PARAM)
// bison3:
#define yyerror(pp, msg) grampar_yyerror(msg, pp)

// rename the externally-visible parsing routine to make it
// specific to this instance, so multiple bison-generated
// parsers can coexist
#define yyparse grampar_yyparse


// grab the parameter
#define PARAM ((ParseParams*)parseParam)

// return a locstring for 'str' with no location information
#define noloc(str)                                                    \
  new LocString(SL_UNKNOWN,      /* unknown location */               \
                PARAM->lexer.strtable.add(str))
                
// locstring for NULL, with no location
#define nolocNULL()                                                   \
  new LocString(SL_UNKNOWN, NULL)

// return a locstring with same location info as something else
// (passed as a pointer to a SourceLocation)
#define sameloc(otherLoc, str)                                        \
  new LocString(otherLoc->loc, PARAM->lexer.strtable.add(str))

// return a locstring with same location info as something else
// (passed as a pointer to a SourceLocation)
#define strloc(otherLoc, str)                                        \
  new LocString(otherLoc, PARAM->lexer.strtable.add(str))

// interpret the word into an associativity kind specification
AssocKind whichKind(LocString * /*owner*/ kind);


#line 128 "grampar.tab.cc" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "grampar.tab.h".  */
#ifndef YY_YY_GRAMPAR_TAB_H_INCLUDED
# define YY_YY_GRAMPAR_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOK_INTEGER = 258,
    TOK_NAME = 259,
    TOK_STRING = 260,
    TOK_LIT_CODE = 261,
    TOK_LBRACE = 262,
    TOK_RBRACE = 263,
    TOK_COLON = 264,
    TOK_SEMICOLON = 265,
    TOK_ARROW = 266,
    TOK_TRAVERSE = 267,
    TOK_VALIDATE = 268,
    TOK_TOKENS = 269,
    TOK_LPAREN = 270,
    TOK_RPAREN = 271,
    TOK_COMMA = 272,
    TOK_TERMINALS = 273,
    TOK_TOKEN = 274,
    TOK_NONTERM = 275,
    TOK_FUN = 276,
    TOK_VERBATIM = 277,
    TOK_IMPL_VERBATIM = 278,
    TOK_PRECEDENCE = 279,
    TOK_OPTION = 280,
    TOK_EXPECT = 281,
    TOK_CONTEXT_CLASS = 282,
    TOK_SUBSETS = 283,
    TOK_DELETE = 284,
    TOK_REPLACE = 285,
    TOK_TREE = 286,
    TOK_NULL = 287,
    TOK_FORBID_NEXT = 288,
    TOK_PARSE_ERROR = 289
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 122 "grampar.y" /* yacc.c:355  */

  int num;
  LocString *str;
  SourceLoc loc;

  ASTList<TopForm> *topFormList;
  TopForm *topForm;

  ASTList<TermDecl> *termDecls;
  TermDecl *termDecl;
  ASTList<TermType> *termTypes;
  TermType *termType;
  ASTList<PrecSpec> *precSpecs;

  ASTList<SpecFunc> *specFuncs;
  SpecFunc *specFunc;
  ASTList<LocString> *stringList;

  ASTList<AbstractProdDecl> *absProdDecls;
  AbstractProdDecl *absProdDecl;
  ASTList<TreeProdDecl> *treeProdDecls;
  TreeProdDecl *treeProdDecl;
  ASTList<RHSElt> *rhsList;
  RHSElt *rhsElt;
  ErrorAct *errorAct;
  ASTList<ErrorAct> *errorActs;

#line 231 "grampar.tab.cc" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void* parseParam);

#endif /* !YY_YY_GRAMPAR_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 245 "grampar.tab.cc" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   160

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  35
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  78
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  159

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   289

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   189,   189,   194,   195,   199,   200,   201,   202,   203,
     207,   212,   213,   218,   219,   230,   235,   236,   244,   246,
     251,   252,   256,   257,   261,   263,   268,   269,   274,   275,
     280,   281,   285,   286,   292,   293,   297,   302,   303,   307,
     308,   319,   322,   327,   328,   332,   333,   334,   335,   338,
     339,   340,   341,   342,   343,   346,   347,   350,   352,   354,
     356,   360,   361,   364,   365,   368,   372,   373,   376,   377,
     386,   387,   388,   391,   392,   393,   394,   398,   399
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_INTEGER", "TOK_NAME", "TOK_STRING",
  "TOK_LIT_CODE", "\"{\"", "\"}\"", "\":\"", "\";\"", "\"->\"", "\"~>\"",
  "\"=\"", "\">\"", "\"(\"", "\")\"", "\",\"", "\"terminals\"",
  "\"token\"", "\"nonterm\"", "\"fun\"", "\"verbatim\"",
  "\"impl_verbatim\"", "\"precedence\"", "\"option\"", "\"expect\"",
  "\"context_class\"", "\"subsets\"", "\"delete\"", "\"replace\"",
  "\"tree\"", "\"null\"", "\"forbid_next\"", "\"#parse\"", "$accept",
  "StartSymbol", "TopFormList", "TopForm", "ContextClass", "Verbatim",
  "Option", "Terminals", "TermDecls", "TerminalDecl", "Type", "TermTypes",
  "TermType", "Precedence", "PrecSpecs", "NameOrStringList",
  "NameOrString", "SpecFuncs", "SpecFunc", "FormalsOpt", "Formals",
  "Nonterminal", "Productions", "Production", "TreeValidation0",
  "TreeValidations", "TreeValidation", "Action", "ErrorActions",
  "ErrorAction", "RHS", "RHS2", "RHSElt", "RHSElt2", "Subsets", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289
};
# endif

#define YYPACT_NINF -114

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-114)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -114,    34,    60,  -114,    47,    53,    64,    73,    48,    99,
    -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,   102,  -114,
    -114,     7,    97,   105,    42,   100,  -114,  -114,   103,  -114,
      -8,  -114,  -114,    98,   104,   106,  -114,  -114,   107,    53,
     109,  -114,   110,    92,     2,  -114,  -114,   115,    45,   116,
    -114,  -114,   117,  -114,    46,   113,  -114,  -114,  -114,   108,
     111,  -114,  -114,  -114,    23,     2,    77,  -114,   114,  -114,
      91,    80,   112,   121,  -114,   120,    63,    95,    95,  -114,
    -114,   125,  -114,   118,  -114,  -114,  -114,  -114,   127,  -114,
     121,  -114,    14,  -114,  -114,  -114,  -114,  -114,   119,   122,
      90,     2,  -114,     9,    -3,  -114,   123,   124,  -114,   128,
    -114,  -114,  -114,   129,  -114,  -114,     5,  -114,  -114,  -114,
      41,   130,  -114,     2,  -114,     9,   126,    52,    87,   131,
    -114,  -114,   126,  -114,  -114,  -114,  -114,    16,  -114,   136,
    -114,   133,  -114,  -114,   139,   126,    87,   126,  -114,    82,
     126,   126,   135,  -114,  -114,  -114,   126,   126,   126
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,    21,     0,     0,     0,     0,
       4,     5,     6,     7,     8,     9,    16,    20,     0,    11,
      12,     0,     0,    22,     0,     0,    13,    10,     0,    17,
      26,    34,    66,     0,     0,     0,    41,    14,     0,    21,
       0,    23,     0,    43,     0,    66,    66,     0,     0,     0,
      28,    15,     0,    35,    77,    73,    75,    61,    62,     0,
       0,    45,    67,    70,     0,     0,     0,    48,     0,    18,
       0,     0,     0,     0,    44,     0,     0,     0,     0,    47,
      46,     0,    66,     0,    68,    19,    34,    24,     0,    27,
      37,    39,     0,    42,    74,    76,    32,    33,     0,     0,
       0,     0,    55,     0,     0,    30,     0,    38,    78,     0,
      71,    72,    66,     0,    68,    63,     0,    63,    69,    25,
       0,     0,    40,     0,    55,     0,    53,     0,     0,     0,
      60,    56,    51,    29,    31,    36,    63,     0,    63,     0,
      64,     0,    63,    63,     0,    54,     0,    52,    65,     0,
      57,    49,     0,    63,    63,    63,    50,    58,    59
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,
      75,  -114,  -114,  -114,  -114,  -114,   -77,    61,  -114,  -114,
      44,  -114,  -114,    94,    84,    22,  -114,   -65,  -113,  -114,
     -43,    35,  -114,   -81,  -114
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    10,    11,    12,    13,    14,    23,    29,
      18,    30,    41,    42,    71,   120,    98,    43,    53,   106,
      92,    15,    54,    36,   130,   116,   131,    61,   126,   140,
      44,   103,    62,    63,    75
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      80,    99,    64,    65,   132,   119,    55,    56,    57,   127,
      25,    39,    58,    55,    56,    57,    40,    26,    52,    58,
     127,   128,   118,   145,   108,   147,    59,    55,    56,   150,
     151,   109,   146,    79,     3,    60,   115,   129,   117,   101,
     156,   157,   158,   134,   118,    96,    97,    59,   129,    31,
      68,   133,    21,    32,    16,    69,    60,    32,   136,    17,
     138,   141,   142,   143,    82,    83,    84,    94,    95,   123,
      19,    33,    34,    35,    73,    33,    34,    35,     4,    20,
       5,   153,     6,     7,    88,     8,    81,     9,    89,    82,
      83,    84,   154,    57,   112,   113,   114,    58,    86,    96,
      97,    87,   112,   113,   114,    22,    24,    27,    28,    45,
      37,    48,    38,    52,    49,    46,    50,    47,    51,    66,
      70,    72,    76,    77,    85,    91,    78,    90,    93,   100,
     105,    67,   122,   102,   107,   110,   135,   149,   111,   121,
     144,   109,   148,   152,   124,   155,   137,   104,    74,   125,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     139
};

static const yytype_int16 yycheck[] =
{
      65,    78,    45,    46,   117,     8,     4,     5,     6,     4,
       3,    19,    10,     4,     5,     6,    24,    10,    21,    10,
       4,    16,   103,   136,    10,   138,    24,     4,     5,   142,
     143,    17,    16,    10,     0,    33,   101,    32,   103,    82,
     153,   154,   155,   120,   125,     4,     5,    24,    32,     7,
       5,    10,     4,    11,     7,    10,    33,    11,   123,     6,
     125,     9,    10,   128,    12,    13,    14,     4,     5,   112,
       6,    29,    30,    31,    28,    29,    30,    31,    18,     6,
      20,   146,    22,    23,     4,    25,     9,    27,     8,    12,
      13,    14,    10,     6,    12,    13,    14,    10,     7,     4,
       5,    10,    12,    13,    14,     6,     4,    10,     3,    11,
      10,     4,     9,    21,    39,    11,     7,    11,     8,     4,
       4,     4,     9,    15,    10,     4,    15,    15,     8,     4,
       3,    47,     4,    15,    90,    16,     6,     4,    16,    16,
       9,    17,     6,     4,    15,    10,   124,    86,    54,   114,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    36,    37,     0,    18,    20,    22,    23,    25,    27,
      38,    39,    40,    41,    42,    56,     7,     6,    45,     6,
       6,     4,     6,    43,     4,     3,    10,    10,     3,    44,
      46,     7,    11,    29,    30,    31,    58,    10,     9,    19,
      24,    47,    48,    52,    65,    11,    11,    11,     4,    45,
       7,     8,    21,    53,    57,     4,     5,     6,    10,    24,
      33,    62,    67,    68,    65,    65,     4,    59,     5,    10,
       4,    49,     4,    28,    58,    69,     9,    15,    15,    10,
      62,     9,    12,    13,    14,    10,     7,    10,     4,     8,
      15,     4,    55,     8,     4,     5,     4,     5,    51,    51,
       4,    65,    15,    66,    52,     3,    54,    55,    10,    17,
      16,    16,    12,    13,    14,    62,    60,    62,    68,     8,
      50,    16,     4,    65,    15,    66,    63,     4,    16,    32,
      59,    61,    63,    10,    51,     6,    62,    60,    62,    34,
      64,     9,    10,    62,     9,    63,    16,    63,     6,     4,
      63,    63,     4,    62,    10,    10,    63,    63,    63
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    35,    36,    37,    37,    38,    38,    38,    38,    38,
      39,    40,    40,    41,    41,    42,    43,    43,    44,    44,
      45,    45,    46,    46,    47,    47,    48,    48,    49,    49,
      50,    50,    51,    51,    52,    52,    53,    54,    54,    55,
      55,    56,    56,    57,    57,    58,    58,    58,    58,    59,
      59,    59,    59,    59,    59,    60,    60,    61,    61,    61,
      61,    62,    62,    63,    63,    64,    65,    65,    66,    66,
      67,    67,    67,    68,    68,    68,    68,    69,    69
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
       3,     2,     2,     3,     4,     6,     0,     2,     4,     5,
       1,     0,     0,     2,     4,     6,     0,     4,     0,     5,
       0,     2,     1,     1,     0,     2,     6,     0,     1,     1,
       3,     4,     8,     0,     2,     3,     4,     4,     3,     7,
       9,     5,     7,     5,     7,     0,     2,     3,     5,     5,
       1,     1,     1,     0,     2,     2,     0,     2,     0,     2,
       1,     4,     4,     1,     3,     1,     3,     0,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (parseParam, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, parseParam); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void* parseParam)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (parseParam);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void* parseParam)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parseParam);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, void* parseParam)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , parseParam);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, parseParam); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void* parseParam)
{
  YYUSE (yyvaluep);
  YYUSE (parseParam);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void* parseParam)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, parseParam);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 190 "grampar.y" /* yacc.c:1646  */
    { ((ParseParams*)parseParam)->treeTop = new GrammarAST((yyvsp[0].topFormList)); (yyval.num)=0; }
#line 1438 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 3:
#line 194 "grampar.y" /* yacc.c:1646  */
    { (yyval.topFormList) = new ASTList<TopForm>; }
#line 1444 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 4:
#line 195 "grampar.y" /* yacc.c:1646  */
    { ((yyval.topFormList)=(yyvsp[-1].topFormList))->append((yyvsp[0].topForm)); }
#line 1450 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 5:
#line 199 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = (yyvsp[0].topForm); }
#line 1456 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 200 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = (yyvsp[0].topForm); }
#line 1462 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 7:
#line 201 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = (yyvsp[0].topForm); }
#line 1468 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 8:
#line 202 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = (yyvsp[0].topForm); }
#line 1474 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 9:
#line 203 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = (yyvsp[0].topForm); }
#line 1480 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 10:
#line 208 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_context((yyvsp[-1].str)); }
#line 1486 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 11:
#line 212 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_verbatim(false, (yyvsp[0].str)); }
#line 1492 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 12:
#line 213 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_verbatim(true, (yyvsp[0].str)); }
#line 1498 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 218 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_option((yyvsp[-1].str), 1); }
#line 1504 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 219 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_option((yyvsp[-2].str), (yyvsp[-1].num)); }
#line 1510 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 15:
#line 231 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_terminals((yyvsp[-3].termDecls), (yyvsp[-2].termTypes), (yyvsp[-1].precSpecs)); }
#line 1516 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 16:
#line 235 "grampar.y" /* yacc.c:1646  */
    { (yyval.termDecls) = new ASTList<TermDecl>; }
#line 1522 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 236 "grampar.y" /* yacc.c:1646  */
    { ((yyval.termDecls)=(yyvsp[-1].termDecls))->append((yyvsp[0].termDecl)); }
#line 1528 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 245 "grampar.y" /* yacc.c:1646  */
    { (yyval.termDecl) = new TermDecl((yyvsp[-3].num), (yyvsp[-1].str), sameloc((yyvsp[-1].str), "")); }
#line 1534 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 247 "grampar.y" /* yacc.c:1646  */
    { (yyval.termDecl) = new TermDecl((yyvsp[-4].num), (yyvsp[-2].str), (yyvsp[-1].str)); }
#line 1540 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 251 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1546 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 252 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = nolocNULL(); }
#line 1552 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 256 "grampar.y" /* yacc.c:1646  */
    { (yyval.termTypes) = new ASTList<TermType>; }
#line 1558 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 257 "grampar.y" /* yacc.c:1646  */
    { ((yyval.termTypes)=(yyvsp[-1].termTypes))->append((yyvsp[0].termType)); }
#line 1564 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 262 "grampar.y" /* yacc.c:1646  */
    { (yyval.termType) = new TermType((yyvsp[-1].str), (yyvsp[-2].str), new ASTList<SpecFunc>); }
#line 1570 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 264 "grampar.y" /* yacc.c:1646  */
    { (yyval.termType) = new TermType((yyvsp[-3].str), (yyvsp[-4].str), (yyvsp[-1].specFuncs)); }
#line 1576 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 268 "grampar.y" /* yacc.c:1646  */
    { (yyval.precSpecs) = new ASTList<PrecSpec>; }
#line 1582 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 27:
#line 269 "grampar.y" /* yacc.c:1646  */
    { (yyval.precSpecs) = (yyvsp[-1].precSpecs); }
#line 1588 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 28:
#line 274 "grampar.y" /* yacc.c:1646  */
    { (yyval.precSpecs) = new ASTList<PrecSpec>; }
#line 1594 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 276 "grampar.y" /* yacc.c:1646  */
    { ((yyval.precSpecs)=(yyvsp[-4].precSpecs))->append(new PrecSpec(whichKind((yyvsp[-3].str)), (yyvsp[-2].num), (yyvsp[-1].stringList))); }
#line 1600 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 280 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = new ASTList<LocString>; }
#line 1606 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 281 "grampar.y" /* yacc.c:1646  */
    { ((yyval.stringList)=(yyvsp[-1].stringList))->append((yyvsp[0].str)); }
#line 1612 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 285 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1618 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 33:
#line 286 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1624 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 292 "grampar.y" /* yacc.c:1646  */
    { (yyval.specFuncs) = new ASTList<SpecFunc>; }
#line 1630 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 293 "grampar.y" /* yacc.c:1646  */
    { ((yyval.specFuncs)=(yyvsp[-1].specFuncs))->append((yyvsp[0].specFunc)); }
#line 1636 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 298 "grampar.y" /* yacc.c:1646  */
    { (yyval.specFunc) = new SpecFunc((yyvsp[-4].str), (yyvsp[-2].stringList), (yyvsp[0].str)); }
#line 1642 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 302 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = new ASTList<LocString>; }
#line 1648 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 303 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = (yyvsp[0].stringList); }
#line 1654 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 307 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = new ASTList<LocString>((yyvsp[0].str)); }
#line 1660 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 40:
#line 308 "grampar.y" /* yacc.c:1646  */
    { ((yyval.stringList)=(yyvsp[-2].stringList))->append((yyvsp[0].str)); }
#line 1666 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 320 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_nonterm((yyvsp[-1].str), (yyvsp[-2].str), new ASTList<SpecFunc>,
                                     new ASTList<AbstractProdDecl>((yyvsp[0].absProdDecl)), NULL); }
#line 1673 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 323 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_nonterm((yyvsp[-5].str), (yyvsp[-6].str), (yyvsp[-3].specFuncs), (yyvsp[-2].absProdDecls), (yyvsp[-1].stringList)); }
#line 1679 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 327 "grampar.y" /* yacc.c:1646  */
    { (yyval.absProdDecls) = new ASTList<AbstractProdDecl>; }
#line 1685 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 44:
#line 328 "grampar.y" /* yacc.c:1646  */
    { ((yyval.absProdDecls)=(yyvsp[-1].absProdDecls))->append((yyvsp[0].absProdDecl)); }
#line 1691 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 45:
#line 332 "grampar.y" /* yacc.c:1646  */
    { (yyval.absProdDecl) = new ProdDecl((yyvsp[-2].loc), PDK_NEW, (yyvsp[-1].rhsList), (yyvsp[0].str), nolocNULL(), nolocNULL()); }
#line 1697 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 46:
#line 333 "grampar.y" /* yacc.c:1646  */
    { (yyval.absProdDecl) = new ProdDecl((yyvsp[-2].loc), PDK_REPLACE,(yyvsp[-1].rhsList), (yyvsp[0].str), nolocNULL(), nolocNULL()); }
#line 1703 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 47:
#line 334 "grampar.y" /* yacc.c:1646  */
    { (yyval.absProdDecl) = new ProdDecl((yyvsp[-2].loc), PDK_DELETE, (yyvsp[-1].rhsList), nolocNULL(), nolocNULL(), nolocNULL()); }
#line 1709 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 335 "grampar.y" /* yacc.c:1646  */
    { (yyval.absProdDecl) = (yyvsp[0].treeProdDecl); }
#line 1715 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 338 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-5].loc), PDK_TRAVERSE_VAL, NULL, (yyvsp[-1].str), (yyvsp[-6].str), sameloc((yyvsp[-6].str), ""), sameloc((yyvsp[-6].str), NULL), (yyvsp[-3].treeProdDecls), (yyvsp[0].errorActs)); }
#line 1721 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 50:
#line 339 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-5].loc), PDK_TRAVERSE_VAL, NULL, (yyvsp[-1].str), (yyvsp[-6].str), sameloc((yyvsp[-8].str), ""), (yyvsp[-8].str), (yyvsp[-3].treeProdDecls), (yyvsp[0].errorActs)); }
#line 1727 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 51:
#line 340 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-3].loc), PDK_TRAVERSE_TKNS, (yyvsp[-2].rhsList),  (yyvsp[-1].str), (yyvsp[-4].str), sameloc((yyvsp[-4].str), ""), sameloc((yyvsp[-4].str), NULL), NULL, (yyvsp[0].errorActs)); }
#line 1733 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 52:
#line 341 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-3].loc), PDK_TRAVERSE_TKNS, (yyvsp[-2].rhsList),  (yyvsp[-1].str), (yyvsp[-4].str), sameloc((yyvsp[-6].str), ""), (yyvsp[-6].str), NULL, (yyvsp[0].errorActs)); }
#line 1739 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 342 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-3].loc), PDK_TRAVERSE_GR, (yyvsp[-2].rhsList),    (yyvsp[-1].str), (yyvsp[-4].str), sameloc((yyvsp[-4].str), ""), sameloc((yyvsp[-4].str), NULL), NULL, (yyvsp[0].errorActs)); }
#line 1745 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 343 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-3].loc), PDK_TRAVERSE_GR, (yyvsp[-2].rhsList),    (yyvsp[-1].str), (yyvsp[-4].str), sameloc((yyvsp[-6].str), ""), (yyvsp[-6].str), NULL, (yyvsp[0].errorActs)); }
#line 1751 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 346 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecls) = new ASTList<TreeProdDecl>; }
#line 1757 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 56:
#line 347 "grampar.y" /* yacc.c:1646  */
    { ((yyval.treeProdDecls)=(yyvsp[-1].treeProdDecls))->append((yyvsp[0].treeProdDecl)); }
#line 1763 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 57:
#line 350 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-2].str)->loc, PDK_TRAVERSE_VAL, new ASTList<RHSElt>, 
                                                                         sameloc((yyvsp[-2].str), ""), (yyvsp[-2].str), sameloc((yyvsp[-2].str), ""), sameloc((yyvsp[-2].str), NULL), NULL, (yyvsp[0].errorActs)); }
#line 1770 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 352 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-4].str)->loc, PDK_TRAVERSE_VAL, new ASTList<RHSElt>, 
                                                                         sameloc((yyvsp[-4].str), ""), (yyvsp[-2].str), sameloc((yyvsp[-4].str), ""), (yyvsp[-4].str), NULL, (yyvsp[0].errorActs)); }
#line 1777 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 354 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-4].loc), PDK_TRAVERSE_NULL, new ASTList<RHSElt>,
                                                                         strloc((yyvsp[-4].loc), ""), (yyvsp[-2].str), strloc((yyvsp[-4].loc), ""), strloc((yyvsp[-4].loc), NULL), NULL, (yyvsp[0].errorActs)); }
#line 1784 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 356 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = (yyvsp[0].treeProdDecl); }
#line 1790 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 61:
#line 360 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1796 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 361 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = nolocNULL(); }
#line 1802 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 364 "grampar.y" /* yacc.c:1646  */
    { (yyval.errorActs) = new ASTList<ErrorAct>; }
#line 1808 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 365 "grampar.y" /* yacc.c:1646  */
    { ((yyval.errorActs)=(yyvsp[-1].errorActs))->append((yyvsp[0].errorAct)); }
#line 1814 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 368 "grampar.y" /* yacc.c:1646  */
    { (yyval.errorAct) = new ErrorAct(ERR_PARSE, (yyvsp[0].str)); }
#line 1820 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 66:
#line 372 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsList) = new ASTList<RHSElt>; }
#line 1826 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 373 "grampar.y" /* yacc.c:1646  */
    { ((yyval.rhsList)=(yyvsp[-1].rhsList))->append((yyvsp[0].rhsElt)); }
#line 1832 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 376 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsList) = new ASTList<RHSElt>; }
#line 1838 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 377 "grampar.y" /* yacc.c:1646  */
    { ((yyval.rhsList)=(yyvsp[-1].rhsList))->append((yyvsp[0].rhsElt)); }
#line 1844 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 71:
#line 387 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_prec((yyvsp[-1].str)); }
#line 1850 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 72:
#line 388 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_forbid((yyvsp[-1].str)); }
#line 1856 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 73:
#line 391 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_name(sameloc((yyvsp[0].str), ""), (yyvsp[0].str)); }
#line 1862 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 74:
#line 392 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_name((yyvsp[-2].str), (yyvsp[0].str)); }
#line 1868 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 75:
#line 393 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_string(sameloc((yyvsp[0].str), ""), (yyvsp[0].str)); }
#line 1874 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 76:
#line 394 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_string((yyvsp[-2].str), (yyvsp[0].str)); }
#line 1880 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 77:
#line 398 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = NULL; }
#line 1886 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 78:
#line 399 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = (yyvsp[-1].stringList); }
#line 1892 "grampar.tab.cc" /* yacc.c:1646  */
    break;


#line 1896 "grampar.tab.cc" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (parseParam, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (parseParam, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, parseParam);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, parseParam);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (parseParam, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, parseParam);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, parseParam);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 403 "grampar.y" /* yacc.c:1906  */

/* ------------------ extra C code ------------------ */
AssocKind whichKind(LocString * /*owner*/ kind)
{ 
  // delete 'kind' however we exit
  Owner<LocString> killer(kind);
  
  #define CHECK(syntax, value)   \
    if (kind->equals(syntax)) {  \
      return value;              \
    }
  CHECK("left", AK_LEFT);
  CHECK("right", AK_RIGHT);
  CHECK("nonassoc", AK_NONASSOC);
  CHECK("prec", AK_NEVERASSOC);
  CHECK("assoc_split", AK_SPLIT);
  #undef CHECK

  xbase(stringc << kind->locString()
                << ": invalid associativity kind: " << *kind);
}
