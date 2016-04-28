/* grammar.y            see license.txt for copyright and terms of use
 * parser for grammar files with new ast format */


/* C declarations */
%{

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
  LIT_STR_2(SL_UNKNOWN,      /* unknown location */               \
                PARAM->lexer.strtable.add(str))

// return a locstring with same location info as something else
// (passed as a pointer to a SourceLocation)
#define sameloc(otherLoc, str)                                        \
  LIT_STR_2(otherLoc->loc, PARAM->lexer.strtable.add(str))

// return a locstring with same location info as something else
// (passed as a pointer to a SourceLocation)
#define strloc(otherLoc, str)                                        \
  LIT_STR_2(otherLoc, PARAM->lexer.strtable.add(str))

// interpret the word into an associativity kind specification
AssocKind whichKind(LocString * /*owner*/ kind);

%}

// %parse-param %lex-param %param https://lwn.net/Articles/561990/
%param {void* parseParam}

/* ================== bison declarations =================== */
// don't use globals
//bison2 : %pure_parser -> bison3
%define api.pure full

/* ===================== tokens ============================ */
/* tokens that have many lexical spellings */
%token <num> TOK_INTEGER
%token <str> TOK_NAME
%token <str> TOK_STRING
%token <str> TOK_LIT_CODE

/* punctuators */
%token TOK_LBRACE "{"
%token TOK_RBRACE "}"
%token TOK_COLON ":"
%token TOK_COLON_COLON "::"
%token TOK_SEMICOLON ";"
%token <loc> TOK_ARROW "->"
%token <loc> TOK_TRAVERSE "~>"
%token <loc> TOK_VALIDATE "="
%token <loc> TOK_TOKENS ">"
%token TOK_LPAREN "("
%token TOK_RPAREN ")"
%token TOK_COMMA ","

/* keywords */

%token TOK_START "start"
%token TOK_SYMBOL "symbol"
%token TOK_LEXER "lexer"
%token TOK_TERMINALS "terminals"
%token TOK_TOKEN "token"
%token TOK_NONTERM "nonterm"
%token TOK_FUN "fun"
%token TOK_VERBATIM "verbatim"
%token TOK_IMPL_VERBATIM "impl_verbatim"
%token TOK_PRECEDENCE "precedence"
%token TOK_OPTION "option"
%token TOK_EXPECT "expect"
%token TOK_CONTEXT_CLASS "context_class"
%token TOK_SUBSETS "subsets"
%token TOK_DELETE "delete"
%token TOK_REPLACE "replace"
%token TOK_TREE "tree"
%token <loc> TOK_NULL "null"
%token TOK_FORBID_NEXT "forbid_next"
%token TOK_PARSE_ERROR "#parse"
%token TOK_START_RULE "$start"

// left, right, nonassoc: they're not keywords, since "left" and "right"
// are common names for RHS elements; instead, we parse them as names
// and interpret them after lexing


/* ===================== types ============================ */
/* all pointers are owner pointers */
%union {
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
  MarkedAction *markedAct;
  ASTList<MarkedAction> *markedActs;
  Name3 *name3;
}

%type <num> StartSymbol
%type <str> Type Action
%type <markedAct> MarkedAction
%type <markedActs> MarkedActions

%type <topFormList> TopFormList
%type <topForm> TopForm ContextClass Verbatim Option Terminals Nonterminal
%type <start> Start
%type <str> StartS
%type <str> StartL

%type <termDecls> TermDecls
%type <termDecl> TerminalDecl
%type <termTypes> TermTypes
%type <termType> TermType
%type <precSpecs> Precedence PrecSpecs
%type <stringList> NameOrStringList
%type <str> NameOrString

%type <specFuncs> SpecFuncs
%type <specFunc> SpecFunc
%type <stringList> FormalsOpt Formals Subsets

%type <absProdDecls> Productions
%type <absProdDecl> Production
%type <treeProdDecls> TreeValidations 
%type <treeProdDecl> TreeValidation0 TreeValidation 
%type <name3> Name3 Name3N
%type <rhsList> RHS
%type <rhsElt> RHSElt
%type <rhsList> RHS2
%type <rhsElt> RHSElt2


/* ===================== productions ======================= */
%%

/* The actions in this file simply build an Abstract Syntax Tree (AST)
 * for later processing. */


/* start symbol */
/* yields: int (dummy value) */
StartSymbol: TopFormList     
               { ((ParseParams*)parseParam)->treeTop = new (__pool) GrammarAST($1); $$=0; }
           ;

/* yields: ASTList<TopForm> */
TopFormList: /*empty*/              { $$ = new (__pool) ASTList<TopForm>; }
           | TopFormList TopForm    { ($$=$1)->append($2); }
           ;
           
/* yields: TopForm */
TopForm: ContextClass               { $$ = $1; }
       | Verbatim                   { $$ = $1; }
       | Option                     { $$ = $1; }
       | Start                      { $$ = $1; }
       | Terminals                  { $$ = $1; }
       | Nonterminal                { $$ = $1; }
       ;
 
/* yields: TopForm (always TF_context) */
ContextClass: "context_class" TOK_LIT_CODE ";"
                { $$ = new (__pool) TF_context($2); }
            ;

/* yields: TopForm (always TF_verbatim) */
Verbatim: "verbatim" TOK_LIT_CODE          { $$ = new (__pool) TF_verbatim(false, $2); }
        | "impl_verbatim" TOK_LIT_CODE     { $$ = new (__pool) TF_verbatim(true, $2); }
        ;

/* yields: TopForm (always TF_option) */
/* options without specified values default to a value of 1 */
Option: "option" TOK_NAME ";"              { $$ = new (__pool) TF_option($2, 1); }
      | "option" TOK_NAME TOK_INTEGER ";"  { $$ = new (__pool) TF_option($2, $3); }
      ;

Start:  "start" "{" StartS StartL "}"        { $$ = new (__pool) TF_start($3, $4); }
      | "start" "{" StartL StartS "}"        { $$ = new (__pool) TF_start($4, $3); }
      ;

StartS : /* empty */                         { $$ = NOLOC_NULL(); } 
      | "symbol" TOK_NAME ";"                { $$ = $2; }
      ;

StartL : /* empty */                         { $$ = NOLOC_NULL(); }
      | "lexer" TOK_NAME  ";"                { $$ = $2; }
      ;

/* ------ terminals ------ */
/*
 * the terminals are the grammar symbols that appear only on the RHS of
 * forms; they are the output of the lexer; the Terminals list declares
 * all of the terminals that will appear in the rules
 */
/* yields: TopForm (always TF_terminals) */
Terminals: "terminals" "{" TermDecls TermTypes Precedence "}"
             { $$ = new (__pool) TF_terminals($3, $4, $5); }
         ;

/* yields: ASTList<TermDecl> */
TermDecls: /* empty */                             { $$ = new (__pool) ASTList<TermDecl>; }
         | TermDecls TerminalDecl                  { ($$=$1)->append($2); }
         ;

/* each terminal has an integer code which is the integer value the
 * lexer uses to represent that terminal.  it is followed by a
 * canonical name, and an optional alias; the name/alias appears in
 * the forms, rather than the integer code itself */
/* yields: TermDecl */
TerminalDecl: TOK_INTEGER ":" TOK_NAME ";"
                { $$ = new (__pool) TermDecl($1, $3, sameloc($3, "")); }
            | TOK_INTEGER ":" TOK_NAME TOK_STRING ";"
                { $$ = new (__pool) TermDecl($1, $3, $4); }
            ;

/* yields: LocString */
Type: TOK_LIT_CODE                    { $$ = $1; }
    | /* empty */                     { $$ = NOLOC_NULL(); }
    ;

/* yields: ASTList<TermType> */
TermTypes: /* empty */                { $$ = new (__pool) ASTList<TermType>; }
         | TermTypes TermType         { ($$=$1)->append($2); }
         ;

/* yields: TermType */
TermType: "token" Type TOK_NAME ";"
            { $$ = new (__pool) TermType($3, $2, new (__pool) ASTList<SpecFunc>); }
        | "token" Type TOK_NAME "{" SpecFuncs "}"
            { $$ = new (__pool) TermType($3, $2, $5); }
        ;

/* yields: ASTList<PrecSpec> */
Precedence: /* empty */                      { $$ = new (__pool) ASTList<PrecSpec>; }
          | "precedence" "{" PrecSpecs "}"   { $$ = $3; }
          ;

/* yields: ASTList<PrecSpec> */
PrecSpecs: /* empty */
             { $$ = new (__pool) ASTList<PrecSpec>; }
         | PrecSpecs TOK_NAME TOK_INTEGER NameOrStringList ";"
             { ($$=$1)->append(new (__pool) PrecSpec(whichKind($2), $3, $4)); }
         ;

/* yields: ASTList<LocString> */
NameOrStringList: /* empty */                     { $$ = new (__pool) ASTList<LocString>; }
                | NameOrStringList NameOrString   { ($$=$1)->append($2); }
                ;

/* yields: LocString */
NameOrString: TOK_NAME       { $$ = $1; }
            | TOK_STRING     { $$ = $1; }
            ;


/* ------ specification functions ------ */
/* yields: ASTList<SpecFunc> */
SpecFuncs: /* empty */                { $$ = new (__pool) ASTList<SpecFunc>; }
         | SpecFuncs SpecFunc         { ($$=$1)->append($2); }
         ;

/* yields: SpecFunc */
SpecFunc: TOK_FUN TOK_NAME "(" FormalsOpt ")" TOK_LIT_CODE
            { $$ = new (__pool) SpecFunc($2, $4, $6); }
        ;

/* yields: ASTList<LocString> */
FormalsOpt: /* empty */               { $$ = new (__pool) ASTList<LocString>; }
          | Formals                   { $$ = $1; }
          ;

/* yields: ASTList<LocString> */
Formals: TOK_NAME                     { $$ = new (__pool) ASTList<LocString>($1); }
       | Formals "," TOK_NAME         { ($$=$1)->append($3); }
       ;


/* ------ nonterminals ------ */
/*
 * a nonterminal is a grammar symbol that appears on the LHS of forms;
 * the body of the Nonterminal declaration specifies the the RHS forms,
 * attribute info, etc.
 */
/* yields: TopForm (always TF_nonterm) */
Nonterminal: "nonterm" Type TOK_NAME Production
               { $$ = new (__pool) TF_nonterm($3, $2, new (__pool) ASTList<SpecFunc>,
                                     new (__pool) ASTList<AbstractProdDecl>($4), NULL); }
           | "nonterm" Type TOK_NAME "{" SpecFuncs Productions Subsets "}"
               { $$ = new (__pool) TF_nonterm($3, $2, $5, $6, $7); }
           ;

/* yields: ASTList<ProdDecl> */
Productions: /* empty */                   { $$ = new (__pool) ASTList<AbstractProdDecl>; }
           | Productions Production        { ($$=$1)->append($2); }
           ;

/* yields: ProdDecl */
Production: "->" RHS Action                { $$ = new (__pool) ProdDecl($1, PDK_NEW, $2, $3, NOLOC_NULL(), NOLOC_NULL()); }
          | "replace" "->" RHS Action      { $$ = new (__pool) ProdDecl($2, PDK_REPLACE,$3, $4, NOLOC_NULL(), NOLOC_NULL()); }
          | "delete"  "->" RHS ";"         { $$ = new (__pool) ProdDecl($2, PDK_DELETE, $3, NOLOC_NULL(), NOLOC_NULL(), NOLOC_NULL()); }
          | "tree" "->" TreeValidation0    { $$ = $3; }
          ;

TreeValidation0: Name3 "=" "(" TreeValidations ")" Action MarkedActions { $$ = new (__pool) TreeProdDecl($2, PDK_TRAVERSE_VAL, NULL, $6, $1->name.clone(), sameloc((&$1->name), ""), $1->label.clone(), $1->tag.clone(), $4, $7); }
          | Name3 ">" RHS2  Action MarkedActions                        { $$ = new (__pool) TreeProdDecl($2, PDK_TRAVERSE_TKNS, $3,  $4, $1->name.clone(), sameloc((&$1->name), ""), $1->label.clone(), $1->tag.clone(), NULL, $5); }
          | Name3 "~>" RHS Action MarkedActions                         { $$ = new (__pool) TreeProdDecl($2, PDK_TRAVERSE_GR, $3,    $4, $1->name.clone(), sameloc((&$1->name), ""), $1->label.clone(), $1->tag.clone(), NULL, $5); }
          ;

TreeValidations: /* empty */                                           { $$ = new (__pool) ASTList<TreeProdDecl>; }
          | TreeValidations TreeValidation                             { ($$=$1)->append($2); }
          ;

TreeValidation: Name3 ";" MarkedActions                                 { $$ = new (__pool) TreeProdDecl($1->name.loc, PDK_TRAVERSE_VAL,  NULL, NOLOC_NULL(), $1->name.clone(),
                                                                         sameloc((&$1->name), ""), $1->label.clone(), $1->tag.clone(), NULL, $3); }
          | Name3N ";" MarkedActions                                    { $$ = new (__pool) TreeProdDecl($1->name.loc, PDK_TRAVERSE_NULL, NULL, NOLOC_NULL(), $1->name.clone(),
                                                                         sameloc(($1->name.clone()), ""), $1->label.clone(), $1->tag.clone(), NULL, $3); }
          | TreeValidation0                                            { $$ = $1; }
          ;

Name3:    TOK_NAME                                                     { $$=new (__pool) Name3(sameloc($1, NULL), sameloc($1, NULL), $1); }
          | TOK_NAME ":" TOK_NAME                                      { $$=new (__pool) Name3(sameloc($1, NULL), $1,                $3); }
          | TOK_NAME "::" TOK_NAME                                     { $$=new (__pool) Name3($1,                sameloc($1, NULL), $3); }
          | TOK_NAME "::" TOK_NAME ":" TOK_NAME                        { $$=new (__pool) Name3($1,                $3,                $5); }
          ;

Name3N:   "null" ":" TOK_NAME                                          { $$=new (__pool) Name3(strloc($1, NULL),  strloc($1, NULL),  $3); }
          | TOK_NAME "::" "null" ":" TOK_NAME                          { $$=new (__pool) Name3($1,                strloc($3, NULL),  $5); }
          ;

/* yields: LocString */
Action: TOK_LIT_CODE                       { $$ = $1; }
      | ";"                                { $$ = NOLOC_NULL(); }
      ;

MarkedActions: /*empty*/                   { $$ = new (__pool) ASTList<MarkedAction>; }
      | MarkedActions MarkedAction         { ($$=$1)->append($2); }
      ;

MarkedAction: "#parse" TOK_LIT_CODE        { $$ = new (__pool) MarkedAction(ERR_PARSE, NOLOC_NULL(), $2); }
      | "$start" TOK_LIT_CODE              { $$ = new (__pool) MarkedAction(START_RULE, NOLOC_NULL(), $2); }
      ;

/* yields: ASTList<RHSElt> */
RHS: /* empty */                           { $$ = new (__pool) ASTList<RHSElt>; }
   | RHS RHSElt                            { ($$=$1)->append($2); }
   ;

RHS2: /* empty */                           { $$ = new (__pool) ASTList<RHSElt>; }
   | RHS2 RHSElt2                           { ($$=$1)->append($2); }
   ;

/*
 * each element on the RHS of a form can have a tag, which appears before a
 * colon (':') if present; the tag is required if that symbol's attributes
 * are to be referenced anywhere in the actions or conditions for the form
 */
/* yields: RHSElt */
RHSElt: RHSElt2
      | "precedence" "(" NameOrString ")"    { $$ = new (__pool) RH_prec($3); }
      | "forbid_next" "(" NameOrString ")"   { $$ = new (__pool) RH_forbid($3); }
      ;

RHSElt2:  TOK_NAME                { $$ = new (__pool) RH_name(sameloc($1, ""), $1); }
      | TOK_NAME ":" TOK_NAME   { $$ = new (__pool) RH_name($1, $3); }
      | TOK_STRING              { $$ = new (__pool) RH_string(sameloc($1, ""), $1); }
      | TOK_NAME ":" TOK_STRING { $$ = new (__pool) RH_string($1, $3); }
      ;
        
/* yields: ASTList<LocString> */
Subsets: /*empty*/                  { $$ = NULL; }
       | "subsets" Formals ";"      { $$ = $2; }
       ;


%%
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
