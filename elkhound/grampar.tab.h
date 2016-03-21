/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

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
    TOK_COLON_COLON = 265,
    TOK_SEMICOLON = 266,
    TOK_ARROW = 267,
    TOK_TRAVERSE = 268,
    TOK_VALIDATE = 269,
    TOK_TOKENS = 270,
    TOK_LPAREN = 271,
    TOK_RPAREN = 272,
    TOK_COMMA = 273,
    TOK_START = 274,
    TOK_SYMBOL = 275,
    TOK_LEXER = 276,
    TOK_TERMINALS = 277,
    TOK_TOKEN = 278,
    TOK_NONTERM = 279,
    TOK_FUN = 280,
    TOK_VERBATIM = 281,
    TOK_IMPL_VERBATIM = 282,
    TOK_PRECEDENCE = 283,
    TOK_OPTION = 284,
    TOK_EXPECT = 285,
    TOK_CONTEXT_CLASS = 286,
    TOK_SUBSETS = 287,
    TOK_DELETE = 288,
    TOK_REPLACE = 289,
    TOK_TREE = 290,
    TOK_NULL = 291,
    TOK_FORBID_NEXT = 292,
    TOK_PARSE_ERROR = 293
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 127 "grampar.y" /* yacc.c:1909  */

  int num;
  LocString *str;
  SourceLoc loc;

  ASTList<TopForm> *topFormList;
  TopForm *topForm;
  TF_start *start;

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
  Name3 *name3;

#line 123 "grampar.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void* parseParam);

#endif /* !YY_YY_GRAMPAR_TAB_H_INCLUDED  */
