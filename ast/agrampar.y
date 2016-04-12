/* agrampar.y            see license.txt for copyright and terms of use
 * parser for abstract grammar definitions (.ast files) */


/* C declarations */
%{

#include "agrampar.h"       // agrampar_yylex, etc.

#include <stdlib.h>         // malloc, free
#include <iostream>       // cout

// enable debugging the parser
#ifndef NDEBUG
  #define YYDEBUG 1
#endif

// permit having other parser's codes in the same program
#define yyparse agrampar_yyparse

%}


/* ================== bison declarations =================== */
// don't use globals
//bison2 : %pure_parser -> bison3
%define api.pure full
%parse-param {void* parseParam}
%lex-param {void* parseParam}


/* ===================== tokens ============================ */
/* tokens that have many lexical spellings */
%token <str> TOK_NAME
%token <str> TOK_INTLIT
%token <str> TOK_EMBEDDED_CODE

/* punctuators */
%token TOK_LBRACE "{"
%token TOK_RBRACE "}"
%token TOK_SEMICOLON ";"
%token TOK_ARROW "->"
%token TOK_LPAREN "("
%token TOK_RPAREN ")"
%token TOK_LANGLE "<"
%token TOK_RANGLE ">"
%token TOK_STAR "*"
%token TOK_AMPERSAND "&"
%token TOK_COMMA ","
%token TOK_EQUALS "="
%token TOK_COLON ":"

/* keywords */
%token TOK_CLASS "class"
%token TOK_PUBLIC "public"
%token TOK_PRIVATE "private"
%token TOK_PROTECTED "protected"
%token TOK_VERBATIM "verbatim"
%token TOK_IMPL_VERBATIM "impl_verbatim"
%token TOK_CTOR "ctor"
%token TOK_DTOR "dtor"
%token TOK_PURE_VIRTUAL "pure_virtual"
%token TOK_CUSTOM "custom"
%token TOK_OPTION "option"
%token TOK_NEW "new"
%token TOK_ENUM "enum"


/* ======================== types ========================== */
/* all pointers are interpreted as owner pointers */
%union {
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
}

%type <file> StartSymbol
%type <formList> Input
%type <tfClass> Class ClassBody ClassMembersOpt
%type <ctorArgList> CtorArgsOpt CtorArgs CtorArgList
%type <userDeclList> CtorMembersOpt
%type <str> Arg ArgWord Embedded ArgList
%type <accessCtl> Public
%type <accessMod> AccessMod
%type <verbatim> Verbatim
%type <annotation> Annotation
%type <tfOption> Option
%type <stringList> StringList OptionArgs
%type <tfEnum> Enum
%type <enumeratorList> EnumeratorSeq
%type <enumerator> Enumerator
%type <baseClassList> BaseClassesOpt BaseClassSeq
%type <accessCtl> BaseAccess
%type <baseClass> BaseClass
%type <customCode> CustomCode;


/* ===================== productions ======================= */
%%

/* start symbol */
/* yields ASTSpecFile, though really $$ isn't used.. */
StartSymbol: Input
               { $$ = *((ASTSpecFile**)parseParam) = new (y_pool) ASTSpecFile(DBG_INFO_ARG0_FIRST  y_pool,$1); }
           ;

/* sequence of toplevel forms */
/* yields ASTList<ToplevelForm> */
Input: /* empty */           { $$ = new (y_pool) ASTList<ToplevelForm>(DBG_INFO_ARG0_FIRST  y_pool); }
     | Input Class           { ($$=$1)->append(DBG_INFO_ARG0_FIRST  $2); }
     | Input Verbatim        { ($$=$1)->append(DBG_INFO_ARG0_FIRST  $2); }
     | Input Option          { ($$=$1)->append(DBG_INFO_ARG0_FIRST  $2); }
     | Input Enum            { ($$=$1)->append(DBG_INFO_ARG0_FIRST  $2); }
     | Input CustomCode      { ($$=$1)->append(DBG_INFO_ARG0_FIRST  new (y_pool) TF_custom(DBG_INFO_ARG0_FIRST  y_pool,$2)); }
     | Input ";"             { $$=$1; }     /* ignore extraneous semis */
     ;

/* a class is a nonterminal in the abstract grammar */
/* yields TF_class */
Class: NewOpt "class" TOK_NAME CtorArgsOpt BaseClassesOpt ClassBody
         { ($$=$6)->super->name = unbox($3); 
           $$->super->args.assign($4, true);
           $$->super->bases.assign($5, true); }
     | NewOpt "class" TOK_NAME CtorArgs CtorArgs BaseClassesOpt ClassBody
         { ($$=$7)->super->name = unbox($3);
           $$->super->args.assign($4, true);
           $$->super->lastArgs.assign($5, true);
           $$->super->bases.assign($6, true); }
     ;

/* for now, just allow "new" but don't interpret it */
NewOpt: /* empty */          {}
      | "new"                {}
      ;

/*
 * I contemplated making both kinds of forms end in semicolon, but then
 * realized that for consistency I should also put semis after the
 * inner ctor bodies, which I didn't want to do.  So, I'll stick with the
 * general schema that entities are followed either by brace-delimited
 * bodies or a single ";".  I can always put extensions inside the braces,
 * so the lack of syntax after "}" won't be a problem later.
 */
/* yields TF_class */
ClassBody: "{" ClassMembersOpt "}" /* no ";", see above */
             { $$=$2; }
         | ";"
             { $$ = new (y_pool) TF_class(DBG_INFO_ARG0_FIRST  y_pool,new (y_pool) ASTClass(DBG_INFO_ARG0_FIRST  y_pool,"(placeholder)", NULL, NULL, NULL, NULL), NULL); }
         ;

/* yields TF_class */
/* does this by making an empty one initially, and then adding to it */
ClassMembersOpt
  : /* empty */
      { $$ = new (y_pool) TF_class(DBG_INFO_ARG0_FIRST  y_pool,new (y_pool) ASTClass(DBG_INFO_ARG0_FIRST  y_pool,"(placeholder)", NULL, NULL, NULL, NULL), NULL); }
  | ClassMembersOpt "->" TOK_NAME CtorArgsOpt BaseClassesOpt ";"
      { ($$=$1)->ctors.append(DBG_INFO_ARG0_FIRST  new (y_pool) ASTClass(DBG_INFO_ARG0_FIRST  y_pool,unbox($3), $4, NULL, $5, NULL)); }
  | ClassMembersOpt "->" TOK_NAME CtorArgsOpt BaseClassesOpt "{" CtorMembersOpt "}"
      { ($$=$1)->ctors.append(DBG_INFO_ARG0_FIRST  new (y_pool) ASTClass(DBG_INFO_ARG0_FIRST  y_pool,unbox($3), $4, NULL, $5, $7)); }
  | ClassMembersOpt Annotation
      { ($$=$1)->super->decls.append(DBG_INFO_ARG0_FIRST  $2); }
  ;

/* empty ctor args can have parens or not, at user's discretion */  
/* yields ASTList<CtorArg> */
CtorArgsOpt
  : /* empty */
      { $$ = new (y_pool) ASTList<CtorArg>(DBG_INFO_ARG0_FIRST  y_pool); }
  | CtorArgs
      { $$ = $1; }
  ;

/* yields ASTList<CtorArg> */
CtorArgs
  : "(" ")"
      { $$ = new (y_pool) ASTList<CtorArg>(DBG_INFO_ARG0_FIRST  y_pool); }
  | "(" CtorArgList ")"
      { $$ = $2; }
  ;

/* yields ASTList<CtorArg> */
CtorArgList: Arg
               { $$ = new (y_pool) ASTList<CtorArg>(DBG_INFO_ARG0_FIRST  y_pool);
                 {
                   string tmp = unbox($1);
                   $$->append(DBG_INFO_ARG0_FIRST  parseCtorArg(tmp));
                 }
               }
           | CtorArgList "," Arg
               { ($$=$1)->append(DBG_INFO_ARG0_FIRST  parseCtorArg(unbox($3))); }
           ;

/* yields string */
Arg: ArgWord
       { $$ = $1; }
   | Arg ArgWord
       { $$ = appendStr($1, $2); }   // dealloc's $1, $2
   ;

/* yields string */
ArgWord
  : TOK_NAME         { $$ = appendStr($1, box(" ")); }
  | TOK_INTLIT       { $$ = appendStr($1, box(" ")); }
  | "<" ArgList ">"  { $$ = appendStr(box("<"), appendStr($2, box(">"))); }
  | "*"              { $$ = box("*"); }
  | "&"              { $$ = box("&"); }
  | "="              { $$ = box("="); }
  | TOK_CLASS        { $$ = box("class "); }    /* special b/c is ast spec keyword */
  ;

/* yields string, and may have commas inside */
ArgList: Arg
           { $$ = $1; }
       | Arg "," ArgList
           { $$ = appendStr($1, appendStr(box(","), $3)); }
       ;

/* yields ASTList<Annotation> */
CtorMembersOpt
  : /* empty */
      { $$ = new (y_pool) ASTList<Annotation>(DBG_INFO_ARG0_FIRST  y_pool); }
  | CtorMembersOpt Annotation
      { ($$=$1)->append(DBG_INFO_ARG0_FIRST  $2); }
  ;

/* yields Annotation */
Annotation
  : AccessMod Embedded
      { $$ = new (y_pool) UserDecl(DBG_INFO_ARG0_FIRST  y_pool,$1, unbox($2), ""); }
  | AccessMod TOK_EMBEDDED_CODE "=" TOK_EMBEDDED_CODE ";"
      { $$ = new (y_pool) UserDecl(DBG_INFO_ARG0_FIRST  y_pool,$1, unbox($2), unbox($4)); }
  | CustomCode
      { $$ = $1; }
  ;

/* yields CustomCode */
CustomCode
  : "custom" TOK_NAME Embedded
      { $$ = new (y_pool) CustomCode(DBG_INFO_ARG0_FIRST  y_pool,unbox($2), unbox($3)); }
  ;

/* yields string */
Embedded
  : TOK_EMBEDDED_CODE ";"
      { $$ = $1; }
  | "{" TOK_EMBEDDED_CODE "}"
      { $$ = $2; }
  ;

/* yields AccessCtl */
Public
  : "public"        { $$ = AC_PUBLIC; }
  | "private"       { $$ = AC_PRIVATE; }
  | "protected"     { $$ = AC_PROTECTED; }
  | "ctor"          { $$ = AC_CTOR; }
  | "dtor"          { $$ = AC_DTOR; }
  | "pure_virtual"  { $$ = AC_PUREVIRT; }
  ;

/* yield AccessMod */
AccessMod: Public
             { $$ = new (y_pool) AccessMod(DBG_INFO_ARG0_FIRST  y_pool,$1, NULL); }
         | Public "(" StringList ")"
             { $$ = new (y_pool) AccessMod(DBG_INFO_ARG0_FIRST  y_pool,$1, $3); }
         ;

/* yield ASTList<string> */
StringList: TOK_NAME
              { $$ = new (y_pool) ASTList<string>(DBG_INFO_ARG0_FIRST  y_pool,$1); }
          | StringList "," TOK_NAME
              { ($$=$1)->append(DBG_INFO_ARG0_FIRST  $3); }
          ;

/* yields TF_verbatim */
Verbatim: "verbatim" Embedded
            { $$ = new (y_pool) TF_verbatim(DBG_INFO_ARG0_FIRST  y_pool,unbox($2)); }
        | "impl_verbatim" Embedded
            { $$ = new (y_pool) TF_impl_verbatim(DBG_INFO_ARG0_FIRST  y_pool,unbox($2)); }
        ;

/* yields TF_option */
Option: "option" TOK_NAME OptionArgs ";"
      { $$ = new (y_pool) TF_option(DBG_INFO_ARG0_FIRST  y_pool,unbox($2), $3); }
      ;
      
/* yields ASTList<string> */
OptionArgs: /*empty*/
              { $$ = new (y_pool) ASTList<string>(DBG_INFO_ARG0_FIRST  y_pool); }
          | OptionArgs TOK_NAME
              { ($$=$1)->append(DBG_INFO_ARG0_FIRST  $2); }
          ;

/* yields TF_enum */
Enum: "enum" TOK_NAME "{" EnumeratorSeq "}"
        { $$ = new (y_pool) TF_enum(DBG_INFO_ARG0_FIRST  y_pool,unbox($2), $4); }
    | "enum" TOK_NAME "{" EnumeratorSeq "," "}"
        { $$ = new (y_pool) TF_enum(DBG_INFO_ARG0_FIRST  y_pool,unbox($2), $4); }
    ;

/* yields ASTList<string> */
EnumeratorSeq: Enumerator
                 { $$ = new (y_pool) ASTList<string>(DBG_INFO_ARG0_FIRST  y_pool,$1); }
             | EnumeratorSeq "," Enumerator
                 { ($$=$1)->append(DBG_INFO_ARG0_FIRST  $3); }
             ;

/* yields string */
Enumerator: TOK_NAME
              { $$ = $1; }
          ;

/* yields ASTList<BaseClass> */
BaseClassesOpt: /* empty */
                  { $$ = new (y_pool) ASTList<BaseClass>(DBG_INFO_ARG0_FIRST  y_pool); }
              | ":" BaseClassSeq
                  { $$ = $2; }
              ;

/* yields ASTList<BaseClass> */
BaseClassSeq: BaseClass
                { $$ = new (y_pool) ASTList<BaseClass>(DBG_INFO_ARG0_FIRST  y_pool,$1); }
            | BaseClassSeq "," BaseClass
                { ($$=$1)->append(DBG_INFO_ARG0_FIRST  $3); }
            ;

/* yields AccessCtl */
BaseAccess
  : "public"        { $$ = AC_PUBLIC; }
  | "private"       { $$ = AC_PRIVATE; }
  | "protected"     { $$ = AC_PROTECTED; }
  ;

/* yields BaseClass */
BaseClass: BaseAccess TOK_NAME
             { $$ = new (y_pool) BaseClass(DBG_INFO_ARG0_FIRST  y_pool,$1, unbox($2)); }
         ;

%%

/* ----------------- extra C code ------------------- */

