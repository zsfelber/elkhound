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
#line 72 "agrampar.y" /* yacc.c:1909  */

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

#line 107 "agrampar.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void* parseParam);

#endif /* !YY_YY_AGRAMPAR_TAB_H_INCLUDED  */
