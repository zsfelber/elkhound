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

// interpret the word into an associativity kind specification
AssocKind whichKind(LocString * /*owner*/ kind);


#line 123 "grampar.tab.cc" /* yacc.c:339  */

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
    TOK_TREE = 267,
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
    TOK_FORBID_NEXT = 286
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 114 "grampar.y" /* yacc.c:355  */

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

#line 221 "grampar.tab.cc" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void* parseParam);

#endif /* !YY_YY_GRAMPAR_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 235 "grampar.tab.cc" /* yacc.c:358  */

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
#define YYLAST   135

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  74
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  142

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
       0,   177,   177,   182,   183,   187,   188,   189,   190,   191,
     195,   200,   201,   206,   207,   218,   223,   224,   232,   234,
     239,   240,   244,   245,   249,   251,   256,   257,   262,   263,
     268,   269,   273,   274,   280,   281,   285,   290,   291,   295,
     296,   307,   310,   315,   316,   320,   321,   322,   323,   326,
     327,   328,   329,   330,   331,   334,   335,   338,   339,   340,
     344,   345,   349,   350,   353,   354,   363,   364,   365,   368,
     369,   370,   371,   375,   376
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
  "\"forbid_next\"", "$accept", "StartSymbol", "TopFormList", "TopForm",
  "ContextClass", "Verbatim", "Option", "Terminals", "TermDecls",
  "TerminalDecl", "Type", "TermTypes", "TermType", "Precedence",
  "PrecSpecs", "NameOrStringList", "NameOrString", "SpecFuncs", "SpecFunc",
  "FormalsOpt", "Formals", "Nonterminal", "Productions", "Production",
  "TreeValidation0", "TreeValidations", "TreeValidation", "Action", "RHS",
  "RHS2", "RHSElt", "RHSElt2", "Subsets", YY_NULLPTR
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

#define YYPACT_NINF -83

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-83)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -83,    31,    33,   -83,    59,    27,    63,    74,    90,    89,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,    92,   -83,
     -83,    26,    87,    95,    10,    91,   -83,   -83,    93,   -83,
      18,   -83,   -83,    96,    94,    97,   -83,   -83,    99,    27,
     100,   -83,    98,    78,     1,    68,   -83,   -83,   -83,    49,
     105,   -83,   -83,   106,   -83,    15,   102,   -83,   -83,   -83,
     101,   103,   -83,   -83,   -83,   108,   -83,   104,   -83,     4,
       1,   107,   -83,     9,    70,   109,   110,   -83,   112,    42,
      88,    88,    77,     1,   -83,    57,   -83,   -83,   -83,   -83,
     -83,   118,   -83,   110,   -83,    24,   -83,   -83,   -83,   -83,
     -83,   111,   113,   -83,   115,   -83,   -83,     8,   -83,   -83,
       2,   -83,   116,   114,   -83,   119,   -83,   -83,     1,   -83,
      57,    62,    79,   -83,   -83,   -83,    60,   120,   -83,   -83,
      14,   -83,   121,   -83,   -83,   -83,   -83,   -83,    79,    73,
     -83,   -83
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,    21,     0,     0,     0,     0,
       4,     5,     6,     7,     8,     9,    16,    20,     0,    11,
      12,     0,     0,    22,     0,     0,    13,    10,     0,    17,
      26,    34,    62,     0,     0,     0,    41,    14,     0,    21,
       0,    23,     0,    43,     0,     0,    48,    62,    62,     0,
       0,    28,    15,     0,    35,    73,    69,    71,    60,    61,
       0,     0,    45,    63,    66,     0,    62,     0,    64,     0,
       0,     0,    18,     0,     0,     0,     0,    44,     0,     0,
       0,     0,     0,     0,    55,     0,    47,    46,    19,    34,
      24,     0,    27,    37,    39,     0,    42,    70,    72,    32,
      33,     0,     0,    62,     0,    64,    53,     0,    51,    65,
       0,    30,     0,    38,    74,     0,    67,    68,     0,    55,
       0,     0,     0,    59,    56,    25,     0,     0,    40,    54,
       0,    52,     0,    57,    49,    29,    31,    36,     0,     0,
      50,    58
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
      65,   -83,   -83,   -83,   -83,   -83,   -77,    39,   -83,   -83,
      20,   -83,   -83,    67,    82,    16,   -83,   -70,   -46,    28,
     -83,   -82,   -83
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    10,    11,    12,    13,    14,    23,    29,
      18,    30,    41,    42,    74,   126,   101,    43,    54,   112,
      95,    15,    55,    36,   123,   107,   124,    62,    44,    85,
      63,    64,    78
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      87,    69,    70,   109,   102,    56,    57,    58,    56,    57,
     125,    59,   121,   106,    86,   108,    89,    31,   121,    90,
      83,    32,    33,    53,   122,    60,    32,    33,    60,    25,
     138,     3,    61,    17,   114,    61,    26,    39,   109,    34,
      35,   115,    40,    76,    34,    35,    97,    98,   129,   136,
     131,     4,   134,     5,    71,     6,     7,   118,     8,    72,
       9,    56,    57,    58,    99,   100,    16,    59,   140,    19,
     135,   132,   133,    66,    91,    67,    68,    65,    92,    66,
      20,    67,    68,   141,   103,    58,   104,   105,   103,    59,
     104,   105,    99,   100,    21,    22,    24,    27,    28,    53,
      45,    37,    38,    49,    50,    47,    52,    51,    48,    73,
      75,    79,    82,   113,    94,    46,    80,    88,    81,    84,
      96,   111,    77,   128,    93,   139,   137,   116,   110,   117,
     119,   115,   127,   120,     0,   130
};

static const yytype_int16 yycheck[] =
{
      70,    47,    48,    85,    81,     4,     5,     6,     4,     5,
       8,    10,     4,    83,    10,    85,     7,     7,     4,    10,
      66,    11,    12,    21,    16,    24,    11,    12,    24,     3,
      16,     0,    31,     6,    10,    31,    10,    19,   120,    29,
      30,    17,    24,    28,    29,    30,     4,     5,   118,   126,
     120,    18,   122,    20,     5,    22,    23,   103,    25,    10,
      27,     4,     5,     6,     4,     5,     7,    10,   138,     6,
      10,     9,    10,    11,     4,    13,    14,     9,     8,    11,
       6,    13,    14,    10,    11,     6,    13,    14,    11,    10,
      13,    14,     4,     5,     4,     6,     4,    10,     3,    21,
       4,    10,     9,     4,    39,    11,     8,     7,    11,     4,
       4,     9,     4,    93,     4,    33,    15,    10,    15,    15,
       8,     3,    55,     4,    15,     4,     6,    16,    89,    16,
      15,    17,    16,   105,    -1,   119
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    33,    34,     0,    18,    20,    22,    23,    25,    27,
      35,    36,    37,    38,    39,    53,     7,     6,    42,     6,
       6,     4,     6,    40,     4,     3,    10,    10,     3,    41,
      43,     7,    11,    12,    29,    30,    55,    10,     9,    19,
      24,    44,    45,    49,    60,     4,    56,    11,    11,     4,
      42,     7,     8,    21,    50,    54,     4,     5,     6,    10,
      24,    31,    59,    62,    63,     9,    11,    13,    14,    60,
      60,     5,    10,     4,    46,     4,    28,    55,    64,     9,
      15,    15,     4,    60,    15,    61,    10,    59,    10,     7,
      10,     4,     8,    15,     4,    52,     8,     4,     5,     4,
       5,    48,    48,    11,    13,    14,    59,    57,    59,    63,
      49,     3,    51,    52,    10,    17,    16,    16,    60,    15,
      61,     4,    16,    56,    58,     8,    47,    16,     4,    59,
      57,    59,     9,    10,    59,    10,    48,     6,    16,     4,
      59,    10
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    32,    33,    34,    34,    35,    35,    35,    35,    35,
      36,    37,    37,    38,    38,    39,    40,    40,    41,    41,
      42,    42,    43,    43,    44,    44,    45,    45,    46,    46,
      47,    47,    48,    48,    49,    49,    50,    51,    51,    52,
      52,    53,    53,    54,    54,    55,    55,    55,    55,    56,
      56,    56,    56,    56,    56,    57,    57,    58,    58,    58,
      59,    59,    60,    60,    61,    61,    62,    62,    62,    63,
      63,    63,    63,    64,    64
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
       3,     2,     2,     3,     4,     6,     0,     2,     4,     5,
       1,     0,     0,     2,     4,     6,     0,     4,     0,     5,
       0,     2,     1,     1,     0,     2,     6,     0,     1,     1,
       3,     4,     8,     0,     2,     3,     4,     4,     2,     6,
       8,     4,     6,     4,     6,     0,     2,     2,     4,     1,
       1,     1,     0,     2,     0,     2,     1,     4,     4,     1,
       3,     1,     3,     0,     3
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
#line 178 "grampar.y" /* yacc.c:1646  */
    { ((ParseParams*)parseParam)->treeTop = new GrammarAST((yyvsp[0].topFormList)); (yyval.num)=0; }
#line 1418 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 3:
#line 182 "grampar.y" /* yacc.c:1646  */
    { (yyval.topFormList) = new ASTList<TopForm>; }
#line 1424 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 4:
#line 183 "grampar.y" /* yacc.c:1646  */
    { ((yyval.topFormList)=(yyvsp[-1].topFormList))->append((yyvsp[0].topForm)); }
#line 1430 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 5:
#line 187 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = (yyvsp[0].topForm); }
#line 1436 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 188 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = (yyvsp[0].topForm); }
#line 1442 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 7:
#line 189 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = (yyvsp[0].topForm); }
#line 1448 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 8:
#line 190 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = (yyvsp[0].topForm); }
#line 1454 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 9:
#line 191 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = (yyvsp[0].topForm); }
#line 1460 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 10:
#line 196 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_context((yyvsp[-1].str)); }
#line 1466 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 11:
#line 200 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_verbatim(false, (yyvsp[0].str)); }
#line 1472 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 12:
#line 201 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_verbatim(true, (yyvsp[0].str)); }
#line 1478 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 206 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_option((yyvsp[-1].str), 1); }
#line 1484 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 207 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_option((yyvsp[-2].str), (yyvsp[-1].num)); }
#line 1490 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 15:
#line 219 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_terminals((yyvsp[-3].termDecls), (yyvsp[-2].termTypes), (yyvsp[-1].precSpecs)); }
#line 1496 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 16:
#line 223 "grampar.y" /* yacc.c:1646  */
    { (yyval.termDecls) = new ASTList<TermDecl>; }
#line 1502 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 224 "grampar.y" /* yacc.c:1646  */
    { ((yyval.termDecls)=(yyvsp[-1].termDecls))->append((yyvsp[0].termDecl)); }
#line 1508 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 233 "grampar.y" /* yacc.c:1646  */
    { (yyval.termDecl) = new TermDecl((yyvsp[-3].num), (yyvsp[-1].str), sameloc((yyvsp[-1].str), "")); }
#line 1514 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 235 "grampar.y" /* yacc.c:1646  */
    { (yyval.termDecl) = new TermDecl((yyvsp[-4].num), (yyvsp[-2].str), (yyvsp[-1].str)); }
#line 1520 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 239 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1526 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 240 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = nolocNULL(); }
#line 1532 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 244 "grampar.y" /* yacc.c:1646  */
    { (yyval.termTypes) = new ASTList<TermType>; }
#line 1538 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 245 "grampar.y" /* yacc.c:1646  */
    { ((yyval.termTypes)=(yyvsp[-1].termTypes))->append((yyvsp[0].termType)); }
#line 1544 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 250 "grampar.y" /* yacc.c:1646  */
    { (yyval.termType) = new TermType((yyvsp[-1].str), (yyvsp[-2].str), new ASTList<SpecFunc>); }
#line 1550 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 252 "grampar.y" /* yacc.c:1646  */
    { (yyval.termType) = new TermType((yyvsp[-3].str), (yyvsp[-4].str), (yyvsp[-1].specFuncs)); }
#line 1556 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 256 "grampar.y" /* yacc.c:1646  */
    { (yyval.precSpecs) = new ASTList<PrecSpec>; }
#line 1562 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 27:
#line 257 "grampar.y" /* yacc.c:1646  */
    { (yyval.precSpecs) = (yyvsp[-1].precSpecs); }
#line 1568 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 28:
#line 262 "grampar.y" /* yacc.c:1646  */
    { (yyval.precSpecs) = new ASTList<PrecSpec>; }
#line 1574 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 264 "grampar.y" /* yacc.c:1646  */
    { ((yyval.precSpecs)=(yyvsp[-4].precSpecs))->append(new PrecSpec(whichKind((yyvsp[-3].str)), (yyvsp[-2].num), (yyvsp[-1].stringList))); }
#line 1580 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 268 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = new ASTList<LocString>; }
#line 1586 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 269 "grampar.y" /* yacc.c:1646  */
    { ((yyval.stringList)=(yyvsp[-1].stringList))->append((yyvsp[0].str)); }
#line 1592 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 273 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1598 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 33:
#line 274 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1604 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 280 "grampar.y" /* yacc.c:1646  */
    { (yyval.specFuncs) = new ASTList<SpecFunc>; }
#line 1610 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 281 "grampar.y" /* yacc.c:1646  */
    { ((yyval.specFuncs)=(yyvsp[-1].specFuncs))->append((yyvsp[0].specFunc)); }
#line 1616 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 286 "grampar.y" /* yacc.c:1646  */
    { (yyval.specFunc) = new SpecFunc((yyvsp[-4].str), (yyvsp[-2].stringList), (yyvsp[0].str)); }
#line 1622 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 290 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = new ASTList<LocString>; }
#line 1628 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 291 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = (yyvsp[0].stringList); }
#line 1634 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 295 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = new ASTList<LocString>((yyvsp[0].str)); }
#line 1640 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 40:
#line 296 "grampar.y" /* yacc.c:1646  */
    { ((yyval.stringList)=(yyvsp[-2].stringList))->append((yyvsp[0].str)); }
#line 1646 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 308 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_nonterm((yyvsp[-1].str), (yyvsp[-2].str), new ASTList<SpecFunc>,
                                     new ASTList<AbstractProdDecl>((yyvsp[0].absProdDecl)), NULL); }
#line 1653 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 311 "grampar.y" /* yacc.c:1646  */
    { (yyval.topForm) = new TF_nonterm((yyvsp[-5].str), (yyvsp[-6].str), (yyvsp[-3].specFuncs), (yyvsp[-2].absProdDecls), (yyvsp[-1].stringList)); }
#line 1659 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 315 "grampar.y" /* yacc.c:1646  */
    { (yyval.absProdDecls) = new ASTList<AbstractProdDecl>; }
#line 1665 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 44:
#line 316 "grampar.y" /* yacc.c:1646  */
    { ((yyval.absProdDecls)=(yyvsp[-1].absProdDecls))->append((yyvsp[0].absProdDecl)); }
#line 1671 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 45:
#line 320 "grampar.y" /* yacc.c:1646  */
    { (yyval.absProdDecl) = new ProdDecl((yyvsp[-2].loc), PDK_NEW, (yyvsp[-1].rhsList), (yyvsp[0].str), nolocNULL(), nolocNULL()); }
#line 1677 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 46:
#line 321 "grampar.y" /* yacc.c:1646  */
    { (yyval.absProdDecl) = new ProdDecl((yyvsp[-2].loc), PDK_REPLACE,(yyvsp[-1].rhsList), (yyvsp[0].str), nolocNULL(), nolocNULL()); }
#line 1683 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 47:
#line 322 "grampar.y" /* yacc.c:1646  */
    { (yyval.absProdDecl) = new ProdDecl((yyvsp[-2].loc), PDK_DELETE, (yyvsp[-1].rhsList), nolocNULL(), nolocNULL(), nolocNULL()); }
#line 1689 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 323 "grampar.y" /* yacc.c:1646  */
    { (yyval.absProdDecl) = (yyvsp[0].treeProdDecl); }
#line 1695 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 326 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-4].loc), PDK_TRAVERSE_VAL, NULL, (yyvsp[0].str), (yyvsp[-5].str), nolocNULL(), nolocNULL(), (yyvsp[-2].treeProdDecls)); }
#line 1701 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 50:
#line 327 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-4].loc), PDK_TRAVERSE_VAL, NULL, (yyvsp[0].str), (yyvsp[-5].str), nolocNULL(), (yyvsp[-7].str), (yyvsp[-2].treeProdDecls)); }
#line 1707 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 51:
#line 328 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-2].loc), PDK_TRAVERSE_TKNS, (yyvsp[-1].rhsList), (yyvsp[0].str), (yyvsp[-3].str), nolocNULL(), nolocNULL(), NULL); }
#line 1713 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 52:
#line 329 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-2].loc), PDK_TRAVERSE_TKNS, (yyvsp[-1].rhsList), (yyvsp[0].str), (yyvsp[-3].str), nolocNULL(), (yyvsp[-5].str), NULL); }
#line 1719 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 330 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-2].loc), PDK_TRAVERSE_GR, (yyvsp[-1].rhsList), (yyvsp[0].str), (yyvsp[-3].str), nolocNULL(), nolocNULL(), NULL); }
#line 1725 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 331 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((yyvsp[-2].loc), PDK_TRAVERSE_GR, (yyvsp[-1].rhsList), (yyvsp[0].str), (yyvsp[-3].str), nolocNULL(), (yyvsp[-5].str), NULL); }
#line 1731 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 334 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecls) = new ASTList<TreeProdDecl>; }
#line 1737 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 56:
#line 335 "grampar.y" /* yacc.c:1646  */
    { ((yyval.treeProdDecls)=(yyvsp[-1].treeProdDecls))->append((yyvsp[0].treeProdDecl)); }
#line 1743 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 57:
#line 338 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((SourceLoc)0, PDK_TRAVERSE_VAL, NULL, NULL, (yyvsp[-1].str), nolocNULL(), nolocNULL(), NULL); }
#line 1749 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 339 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = new TreeProdDecl((SourceLoc)0, PDK_TRAVERSE_VAL, NULL, NULL, (yyvsp[-1].str), nolocNULL(), (yyvsp[-3].str), NULL); }
#line 1755 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 340 "grampar.y" /* yacc.c:1646  */
    { (yyval.treeProdDecl) = (yyvsp[0].treeProdDecl); }
#line 1761 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 344 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 1767 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 61:
#line 345 "grampar.y" /* yacc.c:1646  */
    { (yyval.str) = nolocNULL(); }
#line 1773 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 349 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsList) = new ASTList<RHSElt>; }
#line 1779 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 350 "grampar.y" /* yacc.c:1646  */
    { ((yyval.rhsList)=(yyvsp[-1].rhsList))->append((yyvsp[0].rhsElt)); }
#line 1785 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 353 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsList) = new ASTList<RHSElt>; }
#line 1791 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 354 "grampar.y" /* yacc.c:1646  */
    { ((yyval.rhsList)=(yyvsp[-1].rhsList))->append((yyvsp[0].rhsElt)); }
#line 1797 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 364 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_prec((yyvsp[-1].str)); }
#line 1803 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 365 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_forbid((yyvsp[-1].str)); }
#line 1809 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 368 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_name(sameloc((yyvsp[0].str), ""), (yyvsp[0].str)); }
#line 1815 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 369 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_name((yyvsp[-2].str), (yyvsp[0].str)); }
#line 1821 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 71:
#line 370 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_string(sameloc((yyvsp[0].str), ""), (yyvsp[0].str)); }
#line 1827 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 72:
#line 371 "grampar.y" /* yacc.c:1646  */
    { (yyval.rhsElt) = new RH_string((yyvsp[-2].str), (yyvsp[0].str)); }
#line 1833 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 73:
#line 375 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = NULL; }
#line 1839 "grampar.tab.cc" /* yacc.c:1646  */
    break;

  case 74:
#line 376 "grampar.y" /* yacc.c:1646  */
    { (yyval.stringList) = (yyvsp[-1].stringList); }
#line 1845 "grampar.tab.cc" /* yacc.c:1646  */
    break;


#line 1849 "grampar.tab.cc" /* yacc.c:1646  */
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
#line 380 "grampar.y" /* yacc.c:1906  */

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
