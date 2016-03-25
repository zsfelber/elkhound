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
#line 6 "agrampar.y" /* yacc.c:339  */


#include "agrampar.h"       // agrampar_yylex, etc.

#include <stdlib.h>         // malloc, free
#include <iostream>       // cout

// enable debugging the parser
#ifndef NDEBUG
  #define YYDEBUG 1
#endif

// permit having other parser's codes in the same program
#define yyparse agrampar_yyparse


#line 83 "agrampar.tab.c" /* yacc.c:339  */

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
   by #include "agrampar.tab.h".  */
#ifndef YY_YY_AGRAMPAR_TAB_H_INCLUDED
# define YY_YY_AGRAMPAR_TAB_H_INCLUDED
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
    TOK_NAME = 258,
    TOK_INTLIT = 259,
    TOK_EMBEDDED_CODE = 260,
    TOK_LBRACE = 261,
    TOK_RBRACE = 262,
    TOK_SEMICOLON = 263,
    TOK_ARROW = 264,
    TOK_LPAREN = 265,
    TOK_RPAREN = 266,
    TOK_LANGLE = 267,
    TOK_RANGLE = 268,
    TOK_STAR = 269,
    TOK_AMPERSAND = 270,
    TOK_COMMA = 271,
    TOK_EQUALS = 272,
    TOK_COLON = 273,
    TOK_CLASS = 274,
    TOK_PUBLIC = 275,
    TOK_PRIVATE = 276,
    TOK_PROTECTED = 277,
    TOK_VERBATIM = 278,
    TOK_IMPL_VERBATIM = 279,
    TOK_CTOR = 280,
    TOK_DTOR = 281,
    TOK_PURE_VIRTUAL = 282,
    TOK_CUSTOM = 283,
    TOK_OPTION = 284,
    TOK_NEW = 285,
    TOK_ENUM = 286
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 71 "agrampar.y" /* yacc.c:355  */

  ASTSpecFile *file;
  ASTList<ToplevelForm> *formList;
  TF_class *tfClass;
  ASTList<CtorArg> *ctorArgList;
  ASTList<Annotation> *userDeclList;
  string *str;
  enum AccessCtl accessCtl;
  AccessMod *accessMod;
  ToplevelForm *verbatim;
  Annotation *annotation;
  TF_option *tfOption;
  ASTList<string> *stringList;
  TF_enum *tfEnum;
  ASTList<string> *enumeratorList;
  string *enumerator;
  ASTList<BaseClass> *baseClassList;
  BaseClass *baseClass;
  CustomCode *customCode;

#line 176 "agrampar.tab.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void* parseParam);

#endif /* !YY_YY_AGRAMPAR_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 190 "agrampar.tab.c" /* yacc.c:358  */

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
#define YYLAST   111

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  72
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  114

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   286

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
      25,    26,    27,    28,    29,    30,    31
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   118,   118,   124,   125,   126,   127,   128,   129,   130,
     135,   139,   147,   148,   160,   162,   170,   171,   173,   175,
     183,   184,   190,   192,   197,   204,   209,   211,   217,   218,
     219,   220,   221,   222,   223,   227,   229,   236,   237,   243,
     245,   247,   253,   259,   261,   267,   268,   269,   270,   271,
     272,   276,   278,   283,   285,   290,   292,   297,   303,   304,
     309,   311,   316,   318,   323,   329,   330,   335,   337,   343,
     344,   345,   349
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_NAME", "TOK_INTLIT",
  "TOK_EMBEDDED_CODE", "\"{\"", "\"}\"", "\";\"", "\"->\"", "\"(\"",
  "\")\"", "\"<\"", "\">\"", "\"*\"", "\"&\"", "\",\"", "\"=\"", "\":\"",
  "\"class\"", "\"public\"", "\"private\"", "\"protected\"",
  "\"verbatim\"", "\"impl_verbatim\"", "\"ctor\"", "\"dtor\"",
  "\"pure_virtual\"", "\"custom\"", "\"option\"", "\"new\"", "\"enum\"",
  "$accept", "StartSymbol", "Input", "Class", "NewOpt", "ClassBody",
  "ClassMembersOpt", "CtorArgsOpt", "CtorArgs", "CtorArgList", "Arg",
  "ArgWord", "ArgList", "CtorMembersOpt", "Annotation", "CustomCode",
  "Embedded", "Public", "AccessMod", "StringList", "Verbatim", "Option",
  "OptionArgs", "Enum", "EnumeratorSeq", "Enumerator", "BaseClassesOpt",
  "BaseClassSeq", "BaseAccess", "BaseClass", YY_NULLPTR
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
     285,   286
};
# endif

#define YYPACT_NINF -54

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-54)))

#define YYTABLE_NINF -13

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -54,    12,     1,   -54,   -54,    13,    13,    23,    40,   -54,
      41,   -54,    66,   -54,   -54,   -54,   -54,    81,    85,   -54,
     -54,    13,   -54,    65,    88,   -54,    86,   -54,     3,    89,
      84,   -54,   -54,   -54,   -54,    -2,   -54,    46,    77,    84,
     -54,    52,   -54,   -54,   -54,    69,   -54,   -54,   -54,   -54,
      -3,    69,   -54,    48,    56,    77,   -54,   -54,    63,    83,
     -54,    69,   -54,   -54,   -54,   -54,    82,    94,   -54,   -54,
     -54,   -54,    56,    69,   -54,    69,    48,   -54,    14,   -54,
     -54,   -54,   -54,    96,   -54,   -54,   -54,   -54,   -54,   -54,
     -54,   -54,    90,    32,    84,    98,    -1,   -54,    77,   -54,
     -54,     6,    97,    68,   -54,   100,    99,   -54,   -54,   -54,
     -54,    26,   -54,   -54
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     9,     0,     0,     0,     0,    13,
       0,     4,     0,     8,     5,     6,     7,     0,     0,    55,
      56,     0,    58,     0,     0,    43,     0,    42,     0,     0,
      20,    44,    59,    57,    64,     0,    62,     0,    65,    21,
      60,     0,    28,    29,    22,     0,    31,    32,    33,    34,
       0,    24,    26,     0,     0,    65,    61,    63,    35,     0,
      23,     0,    27,    69,    70,    71,    66,     0,    67,    16,
      15,    10,     0,     0,    30,    25,     0,    72,     0,    11,
      36,    68,    14,     0,    45,    46,    47,    48,    49,    50,
      19,    41,    51,     0,    20,     0,     0,    39,    65,    21,
      53,     0,     0,     0,    52,     0,     0,    37,    17,    54,
      40,     0,    18,    38
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -54,   -54,   -54,   -54,   -54,    33,   -54,    10,   -38,   -54,
     -33,   -48,    35,   -54,    -5,   107,    -6,   -54,   -54,   -54,
     -54,   -54,   -54,   -54,   -54,    70,   -53,   -54,   -54,    34
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    11,    12,    71,    78,    38,    39,    50,
      58,    52,    59,   111,    90,    91,    19,    92,    93,   101,
      14,    15,    28,    16,    35,    36,    54,    66,    67,    68
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      20,    55,    72,    62,    51,    40,    32,    25,    60,     4,
      62,    33,     3,    61,    41,    27,   102,   104,    17,    18,
     -12,    82,   105,    83,     5,     6,    21,    62,    75,     7,
       8,     9,    10,   112,    84,    85,    86,    96,    18,    87,
      88,    89,     7,    22,    23,   103,    84,    85,    86,    42,
      43,    87,    88,    89,     7,    34,    99,    44,    45,    56,
      46,    47,    69,    48,    70,    49,    42,    43,    63,    64,
      65,    29,    42,    43,   107,    45,   108,    46,    47,    73,
      48,    45,    49,    46,    47,    24,    48,    97,    49,    25,
      26,    30,    34,    31,    37,    53,    74,    77,    76,    94,
      95,   100,   106,   109,    98,    79,   113,   110,    80,    13,
      81,    57
};

static const yytype_uint8 yycheck[] =
{
       6,    39,    55,    51,    37,     7,     3,     8,    11,     8,
      58,     8,     0,    16,    16,    21,    17,    11,     5,     6,
      19,     7,    16,     9,    23,    24,     3,    75,    61,    28,
      29,    30,    31,     7,    20,    21,    22,     5,     6,    25,
      26,    27,    28,     3,     3,    98,    20,    21,    22,     3,
       4,    25,    26,    27,    28,     3,    94,    11,    12,     7,
      14,    15,     6,    17,     8,    19,     3,     4,    20,    21,
      22,     6,     3,     4,     6,    12,     8,    14,    15,    16,
      17,    12,    19,    14,    15,    19,    17,    93,    19,     8,
       5,     3,     3,     7,    10,    18,    13,     3,    16,     3,
      10,     3,     5,     3,    94,    72,   111,     8,    73,     2,
      76,    41
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    33,    34,     0,     8,    23,    24,    28,    29,    30,
      31,    35,    36,    47,    52,    53,    55,     5,     6,    48,
      48,     3,     3,     3,    19,     8,     5,    48,    54,     6,
       3,     7,     3,     8,     3,    56,    57,    10,    39,    40,
       7,    16,     3,     4,    11,    12,    14,    15,    17,    19,
      41,    42,    43,    18,    58,    40,     7,    57,    42,    44,
      11,    16,    43,    20,    21,    22,    59,    60,    61,     6,
       8,    37,    58,    16,    13,    42,    16,     3,    38,    37,
      44,    61,     7,     9,    20,    21,    22,    25,    26,    27,
      46,    47,    49,    50,     3,    10,     5,    48,    39,    40,
       3,    51,    17,    58,    11,    16,     5,     6,     8,     3,
       8,    45,     7,    46
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    32,    33,    34,    34,    34,    34,    34,    34,    34,
      35,    35,    36,    36,    37,    37,    38,    38,    38,    38,
      39,    39,    40,    40,    41,    41,    42,    42,    43,    43,
      43,    43,    43,    43,    43,    44,    44,    45,    45,    46,
      46,    46,    47,    48,    48,    49,    49,    49,    49,    49,
      49,    50,    50,    51,    51,    52,    52,    53,    54,    54,
      55,    55,    56,    56,    57,    58,    58,    59,    59,    60,
      60,    60,    61
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     2,     2,     2,     2,     2,
       6,     7,     0,     1,     3,     1,     0,     6,     8,     2,
       0,     1,     2,     3,     1,     3,     1,     2,     1,     1,
       3,     1,     1,     1,     1,     1,     3,     0,     2,     2,
       5,     1,     3,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     4,     1,     3,     2,     2,     4,     0,     2,
       5,     6,     1,     3,     1,     0,     2,     1,     3,     1,
       1,     1,     2
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
#line 119 "agrampar.y" /* yacc.c:1646  */
    { (yyval.file) = *((ASTSpecFile**)parseParam) = new (y_pool) ASTSpecFile(y_pool,(yyvsp[0].formList)); }
#line 1357 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 124 "agrampar.y" /* yacc.c:1646  */
    { (yyval.formList) = new (y_pool) ASTList<ToplevelForm>(y_pool); }
#line 1363 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 125 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.formList)=(yyvsp[-1].formList))->append((yyvsp[0].tfClass)); }
#line 1369 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 126 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.formList)=(yyvsp[-1].formList))->append((yyvsp[0].verbatim)); }
#line 1375 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 127 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.formList)=(yyvsp[-1].formList))->append((yyvsp[0].tfOption)); }
#line 1381 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 128 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.formList)=(yyvsp[-1].formList))->append((yyvsp[0].tfEnum)); }
#line 1387 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 129 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.formList)=(yyvsp[-1].formList))->append(new (y_pool) TF_custom(y_pool,(yyvsp[0].customCode))); }
#line 1393 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 130 "agrampar.y" /* yacc.c:1646  */
    { (yyval.formList)=(yyvsp[-1].formList); }
#line 1399 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 136 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.tfClass)=(yyvsp[0].tfClass))->super->name = unbox((yyvsp[-3].str)); 
           (yyval.tfClass)->super->args.steal((yyvsp[-2].ctorArgList)); 
           (yyval.tfClass)->super->bases.steal((yyvsp[-1].baseClassList)); }
#line 1407 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 140 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.tfClass)=(yyvsp[0].tfClass))->super->name = unbox((yyvsp[-4].str));
           (yyval.tfClass)->super->args.steal((yyvsp[-3].ctorArgList));
           (yyval.tfClass)->super->lastArgs.steal((yyvsp[-2].ctorArgList));
           (yyval.tfClass)->super->bases.steal((yyvsp[-1].baseClassList)); }
#line 1416 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 147 "agrampar.y" /* yacc.c:1646  */
    {}
#line 1422 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 148 "agrampar.y" /* yacc.c:1646  */
    {}
#line 1428 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 161 "agrampar.y" /* yacc.c:1646  */
    { (yyval.tfClass)=(yyvsp[-1].tfClass); }
#line 1434 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 163 "agrampar.y" /* yacc.c:1646  */
    { (yyval.tfClass) = new (y_pool) TF_class(y_pool,new (y_pool) ASTClass(y_pool,"(placeholder)", NULL, NULL, NULL, NULL), NULL); }
#line 1440 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 170 "agrampar.y" /* yacc.c:1646  */
    { (yyval.tfClass) = new (y_pool) TF_class(y_pool,new (y_pool) ASTClass(y_pool,"(placeholder)", NULL, NULL, NULL, NULL), NULL); }
#line 1446 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 172 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.tfClass)=(yyvsp[-5].tfClass))->ctors.append(new (y_pool) ASTClass(y_pool,unbox((yyvsp[-3].str)), (yyvsp[-2].ctorArgList), NULL, (yyvsp[-1].baseClassList), NULL)); }
#line 1452 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 174 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.tfClass)=(yyvsp[-7].tfClass))->ctors.append(new (y_pool) ASTClass(y_pool,unbox((yyvsp[-5].str)), (yyvsp[-4].ctorArgList), NULL, (yyvsp[-3].baseClassList), (yyvsp[-1].userDeclList))); }
#line 1458 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 176 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.tfClass)=(yyvsp[-1].tfClass))->super->decls.append((yyvsp[0].annotation)); }
#line 1464 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 183 "agrampar.y" /* yacc.c:1646  */
    { (yyval.ctorArgList) = new (y_pool) ASTList<CtorArg>(y_pool); }
#line 1470 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 185 "agrampar.y" /* yacc.c:1646  */
    { (yyval.ctorArgList) = (yyvsp[0].ctorArgList); }
#line 1476 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 191 "agrampar.y" /* yacc.c:1646  */
    { (yyval.ctorArgList) = new (y_pool) ASTList<CtorArg>(y_pool); }
#line 1482 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 193 "agrampar.y" /* yacc.c:1646  */
    { (yyval.ctorArgList) = (yyvsp[-1].ctorArgList); }
#line 1488 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 198 "agrampar.y" /* yacc.c:1646  */
    { (yyval.ctorArgList) = new (y_pool) ASTList<CtorArg>(y_pool);
                 {
                   string tmp = unbox((yyvsp[0].str));
                   (yyval.ctorArgList)->append(parseCtorArg(tmp));
                 }
               }
#line 1499 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 205 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.ctorArgList)=(yyvsp[-2].ctorArgList))->append(parseCtorArg(unbox((yyvsp[0].str)))); }
#line 1505 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 210 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1511 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 212 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = appendStr((yyvsp[-1].str), (yyvsp[0].str)); }
#line 1517 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 217 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = appendStr((yyvsp[0].str), box(" ")); }
#line 1523 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 218 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = appendStr((yyvsp[0].str), box(" ")); }
#line 1529 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 219 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = appendStr(box("<"), appendStr((yyvsp[-1].str), box(">"))); }
#line 1535 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 220 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = box("*"); }
#line 1541 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 221 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = box("&"); }
#line 1547 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 222 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = box("="); }
#line 1553 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 223 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = box("class "); }
#line 1559 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 228 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1565 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 230 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = appendStr((yyvsp[-2].str), appendStr(box(","), (yyvsp[0].str))); }
#line 1571 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 236 "agrampar.y" /* yacc.c:1646  */
    { (yyval.userDeclList) = new (y_pool) ASTList<Annotation>(y_pool); }
#line 1577 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 238 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.userDeclList)=(yyvsp[-1].userDeclList))->append((yyvsp[0].annotation)); }
#line 1583 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 244 "agrampar.y" /* yacc.c:1646  */
    { (yyval.annotation) = new (y_pool) UserDecl(y_pool,(yyvsp[-1].accessMod), unbox((yyvsp[0].str)), ""); }
#line 1589 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 246 "agrampar.y" /* yacc.c:1646  */
    { (yyval.annotation) = new (y_pool) UserDecl(y_pool,(yyvsp[-4].accessMod), unbox((yyvsp[-3].str)), unbox((yyvsp[-1].str))); }
#line 1595 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 248 "agrampar.y" /* yacc.c:1646  */
    { (yyval.annotation) = (yyvsp[0].customCode); }
#line 1601 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 254 "agrampar.y" /* yacc.c:1646  */
    { (yyval.customCode) = new (y_pool) CustomCode(y_pool,unbox((yyvsp[-1].str)), unbox((yyvsp[0].str))); }
#line 1607 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 260 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[-1].str); }
#line 1613 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 262 "agrampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[-1].str); }
#line 1619 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 267 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessCtl) = AC_PUBLIC; }
#line 1625 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 268 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessCtl) = AC_PRIVATE; }
#line 1631 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 269 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessCtl) = AC_PROTECTED; }
#line 1637 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 270 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessCtl) = AC_CTOR; }
#line 1643 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 271 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessCtl) = AC_DTOR; }
#line 1649 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 272 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessCtl) = AC_PUREVIRT; }
#line 1655 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 277 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessMod) = new (y_pool) AccessMod(y_pool,(yyvsp[0].accessCtl), NULL); }
#line 1661 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 279 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessMod) = new (y_pool) AccessMod(y_pool,(yyvsp[-3].accessCtl), (yyvsp[-1].stringList)); }
#line 1667 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 284 "agrampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = new (y_pool) ASTList<string>(y_pool,(yyvsp[0].str)); }
#line 1673 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 286 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.stringList)=(yyvsp[-2].stringList))->append((yyvsp[0].str)); }
#line 1679 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 291 "agrampar.y" /* yacc.c:1646  */
    { (yyval.verbatim) = new (y_pool) TF_verbatim(y_pool,unbox((yyvsp[0].str))); }
#line 1685 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 293 "agrampar.y" /* yacc.c:1646  */
    { (yyval.verbatim) = new (y_pool) TF_impl_verbatim(y_pool,unbox((yyvsp[0].str))); }
#line 1691 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 298 "agrampar.y" /* yacc.c:1646  */
    { (yyval.tfOption) = new (y_pool) TF_option(y_pool,unbox((yyvsp[-2].str)), (yyvsp[-1].stringList)); }
#line 1697 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 303 "agrampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = new (y_pool) ASTList<string>(y_pool); }
#line 1703 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 305 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.stringList)=(yyvsp[-1].stringList))->append((yyvsp[0].str)); }
#line 1709 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 310 "agrampar.y" /* yacc.c:1646  */
    { (yyval.tfEnum) = new (y_pool) TF_enum(y_pool,unbox((yyvsp[-3].str)), (yyvsp[-1].enumeratorList)); }
#line 1715 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 312 "agrampar.y" /* yacc.c:1646  */
    { (yyval.tfEnum) = new (y_pool) TF_enum(y_pool,unbox((yyvsp[-4].str)), (yyvsp[-2].enumeratorList)); }
#line 1721 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 317 "agrampar.y" /* yacc.c:1646  */
    { (yyval.enumeratorList) = new (y_pool) ASTList<string>(y_pool,(yyvsp[0].enumerator)); }
#line 1727 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 319 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.enumeratorList)=(yyvsp[-2].enumeratorList))->append((yyvsp[0].enumerator)); }
#line 1733 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 324 "agrampar.y" /* yacc.c:1646  */
    { (yyval.enumerator) = (yyvsp[0].str); }
#line 1739 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 329 "agrampar.y" /* yacc.c:1646  */
    { (yyval.baseClassList) = new (y_pool) ASTList<BaseClass>(y_pool); }
#line 1745 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 331 "agrampar.y" /* yacc.c:1646  */
    { (yyval.baseClassList) = (yyvsp[0].baseClassList); }
#line 1751 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 336 "agrampar.y" /* yacc.c:1646  */
    { (yyval.baseClassList) = new (y_pool) ASTList<BaseClass>(y_pool,(yyvsp[0].baseClass)); }
#line 1757 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 338 "agrampar.y" /* yacc.c:1646  */
    { ((yyval.baseClassList)=(yyvsp[-2].baseClassList))->append((yyvsp[0].baseClass)); }
#line 1763 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 343 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessCtl) = AC_PUBLIC; }
#line 1769 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 344 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessCtl) = AC_PRIVATE; }
#line 1775 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 345 "agrampar.y" /* yacc.c:1646  */
    { (yyval.accessCtl) = AC_PROTECTED; }
#line 1781 "agrampar.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 350 "agrampar.y" /* yacc.c:1646  */
    { (yyval.baseClass) = new (y_pool) BaseClass(y_pool,(yyvsp[-1].accessCtl), unbox((yyvsp[0].str))); }
#line 1787 "agrampar.tab.c" /* yacc.c:1646  */
    break;


#line 1791 "agrampar.tab.c" /* yacc.c:1646  */
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
#line 353 "agrampar.y" /* yacc.c:1906  */


/* ----------------- extra C code ------------------- */

