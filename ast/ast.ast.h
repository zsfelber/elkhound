// ast.ast.h
// *** DO NOT EDIT ***
// generated automatically by astgen, from ast.ast

#ifndef AST_AST_H
#define AST_AST_H

#include "asthelp.h"        // helpers for generated code

// fwd decls
class ASTSpecFile;
class ToplevelForm;
class TF_verbatim;
class TF_impl_verbatim;
class TF_class;
class TF_option;
class TF_custom;
class TF_enum;
class ASTClass;
class AccessMod;
class Annotation;
class UserDecl;
class CustomCode;
class CtorArg;
class BaseClass;


// *** DO NOT EDIT ***

  #include "str.h"         // string

  // this signals to ast.hand.cc that ast.ast.cc is nonempty,
  // so none of the bootstrap code in ast.hand.cc should be used
  #define GENERATED_AST_PRESENT

// *** DO NOT EDIT ***
class ASTSpecFile : public str::Storeable {
public:      // data
  ASTList <ToplevelForm > forms;

public:      // funcs
  ASTSpecFile(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, ASTList <ToplevelForm > *_forms) : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), forms(DBG_INFO_ARG_FWD_FIRST  _forms, true/*move*/) {
  }
  ~ASTSpecFile();

  char const *kindName() const { return "ASTSpecFile"; }

  ASTSpecFile *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

  void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

};



// *** DO NOT EDIT ***
class ToplevelForm : public str::Storeable {
public:      // data

public:      // funcs
  ToplevelForm(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool) : Storeable(DBG_INFO_ARG_FWD_FIRST  pool) {
  }
  virtual ~ToplevelForm();

  enum Kind { TF_VERBATIM, TF_IMPL_VERBATIM, TF_CLASS, TF_OPTION, TF_CUSTOM, TF_ENUM, NUM_KINDS };
  virtual Kind kind() const = 0;

  static char const * const kindNames[NUM_KINDS];
  char const *kindName() const { return kindNames[kind()]; }

  DECL_AST_DOWNCASTS(TF_verbatim, TF_VERBATIM)
  DECL_AST_DOWNCASTS(TF_impl_verbatim, TF_IMPL_VERBATIM)
  DECL_AST_DOWNCASTS(TF_class, TF_CLASS)
  DECL_AST_DOWNCASTS(TF_option, TF_OPTION)
  DECL_AST_DOWNCASTS(TF_custom, TF_CUSTOM)
  DECL_AST_DOWNCASTS(TF_enum, TF_ENUM)

  virtual ToplevelForm* clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const=0;

  virtual void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

};

class TF_verbatim : public ToplevelForm {
public:      // data
  string code;

public:      // funcs
  TF_verbatim(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, string _code) : ToplevelForm(DBG_INFO_ARG_FWD_FIRST  pool), code(_code) {
  }
  virtual ~TF_verbatim();

  virtual Kind kind() const { return TF_VERBATIM; }
  enum { TYPE_TAG = TF_VERBATIM };

  virtual void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

  virtual TF_verbatim *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

};

class TF_impl_verbatim : public ToplevelForm {
public:      // data
  string code;

public:      // funcs
  TF_impl_verbatim(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, string _code) : ToplevelForm(DBG_INFO_ARG_FWD_FIRST  pool), code(_code) {
  }
  virtual ~TF_impl_verbatim();

  virtual Kind kind() const { return TF_IMPL_VERBATIM; }
  enum { TYPE_TAG = TF_IMPL_VERBATIM };

  virtual void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

  virtual TF_impl_verbatim *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

};

class TF_class : public ToplevelForm {
public:      // data
  ASTClass *super;
  ASTList <ASTClass > ctors;

public:      // funcs
  TF_class(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, ASTClass *_super, ASTList <ASTClass > *_ctors) : ToplevelForm(DBG_INFO_ARG_FWD_FIRST  pool), super(_super), ctors(DBG_INFO_ARG_FWD_FIRST  _ctors, true/*move*/) {
  }
  virtual ~TF_class();

  virtual Kind kind() const { return TF_CLASS; }
  enum { TYPE_TAG = TF_CLASS };

  virtual void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

  virtual TF_class *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

  public:  bool hasChildren() const { return ctors.isNotEmpty(); };
};

class TF_option : public ToplevelForm {
public:      // data
  string name;
  ASTList <string > args;

public:      // funcs
  TF_option(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, string _name, ASTList <string > *_args) : ToplevelForm(DBG_INFO_ARG_FWD_FIRST  pool), name(_name), args(DBG_INFO_ARG_FWD_FIRST  _args, true/*move*/) {
  }
  virtual ~TF_option();

  virtual Kind kind() const { return TF_OPTION; }
  enum { TYPE_TAG = TF_OPTION };

  virtual void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

  virtual TF_option *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

};

class TF_custom : public ToplevelForm {
public:      // data
  CustomCode *cust;

public:      // funcs
  TF_custom(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, CustomCode *_cust) : ToplevelForm(DBG_INFO_ARG_FWD_FIRST  pool), cust(_cust) {
  }
  virtual ~TF_custom();

  virtual Kind kind() const { return TF_CUSTOM; }
  enum { TYPE_TAG = TF_CUSTOM };

  virtual void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

  virtual TF_custom *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

};

class TF_enum : public ToplevelForm {
public:      // data
  string name;
  ASTList <string > enumerators;

public:      // funcs
  TF_enum(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, string _name, ASTList <string > *_enumerators) : ToplevelForm(DBG_INFO_ARG_FWD_FIRST  pool), name(_name), enumerators(DBG_INFO_ARG_FWD_FIRST  _enumerators, true/*move*/) {
  }
  virtual ~TF_enum();

  virtual Kind kind() const { return TF_ENUM; }
  enum { TYPE_TAG = TF_ENUM };

  virtual void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

  virtual TF_enum *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

};



// *** DO NOT EDIT ***
class ASTClass : public str::Storeable {
public:      // data
  string name;
  ASTList <CtorArg > args;
  ASTList <CtorArg > lastArgs;
  ASTList <BaseClass > bases;
  ASTList <Annotation > decls;

  int level;
  bool consumed;
  TF_class *parent;
  ASTList <CtorArg > totArgs;
  ASTList <CtorArg > totLastArgs;

public:      // funcs
  ASTClass(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, string _name, ASTList <CtorArg > *_args, ASTList <CtorArg > *_lastArgs, ASTList <BaseClass > *_bases, ASTList <Annotation > *_decls) : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), name(_name), args(DBG_INFO_ARG_FWD_FIRST  _args, true/*move*/), lastArgs(DBG_INFO_ARG_FWD_FIRST  _lastArgs, true/*move*/), bases(DBG_INFO_ARG_FWD_FIRST  _bases, true/*move*/), decls(DBG_INFO_ARG_FWD_FIRST  _decls, true/*move*/),
    level(0), consumed(0), parent(0), totArgs(DBG_INFO_ARG_FWD), totLastArgs(DBG_INFO_ARG_FWD){
  }
  ~ASTClass();

  char const *kindName() const { return "ASTClass"; }

  ASTClass *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

  void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

  public:  string classKindName() const;

  ASTList <CtorArg >& getArgs() { return parent ? totArgs : args; }
  ASTList <CtorArg >& getLastArgs() { return parent ? totLastArgs : lastArgs; }
  ASTList <CtorArg > const & getArgs() const { return parent ? totArgs : args; }
  ASTList <CtorArg > const & getLastArgs() const { return parent ? totLastArgs : lastArgs; }
};



// *** DO NOT EDIT ***

  // specifies what kind of userdecl this is; pub/priv/prot are uninterpreted
  // class members with the associated access control; ctor and dtor are
  // code to be inserted into the ctor or dtor, respectively
  enum AccessCtl {
    AC_PUBLIC,      // access
    AC_PRIVATE,     //   control
    AC_PROTECTED,   //     keywords
    AC_CTOR,        // insert into ctor
    AC_DTOR,        // insert into dtor
    AC_PUREVIRT,    // declare pure virtual in superclass, and impl in subclass
    NUM_ACCESSCTLS
  };

  // map the enum value to a string like "public"
  string toString(AccessCtl acc);      // defined in ast.cc

// *** DO NOT EDIT ***
class AccessMod : public str::Storeable {
public:      // data
  AccessCtl acc;
  ASTList <string > mods;

public:      // funcs
  AccessMod(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, AccessCtl _acc, ASTList <string > *_mods) : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), acc(_acc), mods(DBG_INFO_ARG_FWD_FIRST  _mods, true/*move*/) {
  }
  ~AccessMod();

  char const *kindName() const { return "AccessMod"; }

  AccessMod *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

  void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

  public:  bool hasMod(char const *mod) const;
  public:  bool hasModPrefix(char const *mod) const;
  public:  string getModSuffixFromPrefix(char const *mod) const;
};



// *** DO NOT EDIT ***
class Annotation : public str::Storeable {
public:      // data

public:      // funcs
  Annotation(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool) : Storeable(pool) {
  }
  virtual ~Annotation();

  enum Kind { USERDECL, CUSTOMCODE, NUM_KINDS };
  virtual Kind kind() const = 0;

  static char const * const kindNames[NUM_KINDS];
  char const *kindName() const { return kindNames[kind()]; }

  DECL_AST_DOWNCASTS(UserDecl, USERDECL)
  DECL_AST_DOWNCASTS(CustomCode, CUSTOMCODE)

  virtual Annotation* clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const=0;

  virtual void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

};

class UserDecl : public Annotation {
public:      // data
  AccessMod *amod;
  string code;
  string init;

public:      // funcs
  UserDecl(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, AccessMod *_amod, string _code, string _init) : Annotation(DBG_INFO_ARG_FWD_FIRST  pool), amod(_amod), code(_code), init(_init) {
  }
  virtual ~UserDecl();

  virtual Kind kind() const { return USERDECL; }
  enum { TYPE_TAG = USERDECL };

  virtual void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

  virtual UserDecl *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

  public:  AccessCtl access() const { return amod->acc; };
};

class CustomCode : public Annotation {
public:      // data
  string qualifier;
  string code;

public:      // funcs
  CustomCode(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, string _qualifier, string _code) : Annotation(DBG_INFO_ARG_FWD_FIRST  pool), qualifier(_qualifier), code(_code) {
     used=false;
  }
  virtual ~CustomCode();

  virtual Kind kind() const { return CUSTOMCODE; }
  enum { TYPE_TAG = CUSTOMCODE };

  virtual void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

  virtual CustomCode *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

  public:  bool used;
};



// *** DO NOT EDIT ***
class CtorArg : public str::Storeable {
public:      // data
  bool isOwner;
  string type;
  string name;
  string defaultValue;

public:      // funcs
  CtorArg(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, bool _isOwner, string _type, string _name, string _defaultValue) : Storeable(DBG_INFO_ARG_FWD_FIRST  pool), isOwner(_isOwner), type(_type), name(_name), defaultValue(_defaultValue) {
  }
  ~CtorArg();

  char const *kindName() const { return "CtorArg"; }

  CtorArg *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

  void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

};



// *** DO NOT EDIT ***
class BaseClass : public str::Storeable {
public:      // data
  AccessCtl access;
  string name;

public:      // funcs
  BaseClass(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, AccessCtl _access, string _name) : Storeable(pool), access(_access), name(_name) {
  }
  ~BaseClass();

  char const *kindName() const { return "BaseClass"; }

  BaseClass *clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness=INT_MAX, int listDeepness=INT_MAX) const;

  void debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName = "tree") const;

};




#endif // AST_AST_H
