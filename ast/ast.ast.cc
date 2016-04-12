// ast.ast.cc
// *** DO NOT EDIT ***
// generated automatically by astgen, from ast.ast

#include "ast.ast.h"      // this module


// ------------------ ASTSpecFile -------------------
// *** DO NOT EDIT ***
ASTSpecFile::~ASTSpecFile()
{
  //forms.deleteAllOwning();
}

void ASTSpecFile::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, ASTSpecFile);

  PRINT_LIST(ToplevelForm, forms);
}

ASTSpecFile *ASTSpecFile::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
  deepness--; listDeepness--;
  ASTSpecFile *ret = new (pool) ASTSpecFile(
    DBG_INFO_ARG_FWD_FIRST  pool, cloneASTList(pool, forms, deepness, listDeepness)
  );
  return ret;
}


// ------------------ ToplevelForm -------------------
// *** DO NOT EDIT ***
ToplevelForm::~ToplevelForm()
{
}

char const * const ToplevelForm::kindNames[ToplevelForm::NUM_KINDS] = {
  "TF_verbatim",
  "TF_impl_verbatim",
  "TF_class",
  "TF_option",
  "TF_custom",
  "TF_enum",
};

void ToplevelForm::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
}

DEFN_AST_DOWNCASTS(ToplevelForm, TF_verbatim, TF_VERBATIM)

TF_verbatim::~TF_verbatim()
{
}

void TF_verbatim::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, TF_verbatim);

  ToplevelForm::debugPrint(DBG_INFO_ARG_FWD_FIRST  os, indent, subtreeName);

  PRINT_STRING(code);
}

TF_verbatim *TF_verbatim::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
    deepness--; listDeepness--;
  TF_verbatim *ret = new (pool) TF_verbatim(
    DBG_INFO_ARG_FWD_FIRST  pool, code
  );
  return ret;
}

DEFN_AST_DOWNCASTS(ToplevelForm, TF_impl_verbatim, TF_IMPL_VERBATIM)

TF_impl_verbatim::~TF_impl_verbatim()
{
}

void TF_impl_verbatim::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, TF_impl_verbatim);

  ToplevelForm::debugPrint(DBG_INFO_ARG_FWD_FIRST  os, indent, subtreeName);

  PRINT_STRING(code);
}

TF_impl_verbatim *TF_impl_verbatim::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
    deepness--; listDeepness--;
  TF_impl_verbatim *ret = new (pool) TF_impl_verbatim(
    DBG_INFO_ARG_FWD_FIRST  pool, code
  );
  return ret;
}

DEFN_AST_DOWNCASTS(ToplevelForm, TF_class, TF_CLASS)

TF_class::~TF_class()
{
  delete super;
  //ctors.deleteAllOwning();
}

void TF_class::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, TF_class);

  ToplevelForm::debugPrint(DBG_INFO_ARG_FWD_FIRST  os, indent, subtreeName);

  PRINT_SUBTREE(super);
  PRINT_LIST(ASTClass, ctors);
}

TF_class *TF_class::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
    deepness--; listDeepness--;
  TF_class *ret = new (pool) TF_class(
    DBG_INFO_ARG_FWD_FIRST  pool,
    ((deepness>=0)&&super)? super->clone(DBG_INFO_ARG_FWD_FIRST  pool, deepness, listDeepness) : super,
    cloneASTList(pool, ctors, deepness, listDeepness)
  );
  return ret;
}

DEFN_AST_DOWNCASTS(ToplevelForm, TF_option, TF_OPTION)

TF_option::~TF_option()
{
  while (args.isNotEmpty()) {
    args.removeFirst();
  }
}

void TF_option::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, TF_option);

  ToplevelForm::debugPrint(DBG_INFO_ARG_FWD_FIRST  os, indent, subtreeName);

  PRINT_STRING(name);
  PRINT_LIST(string, args);
}

TF_option *TF_option::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
    deepness--; listDeepness--;
  TF_option *ret = new (pool) TF_option(
    DBG_INFO_ARG_FWD_FIRST  pool, name,
    shallowCloneASTList(pool, args)
  );
  return ret;
}

DEFN_AST_DOWNCASTS(ToplevelForm, TF_custom, TF_CUSTOM)

TF_custom::~TF_custom()
{
  delete cust;
}

void TF_custom::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, TF_custom);

  ToplevelForm::debugPrint(DBG_INFO_ARG_FWD_FIRST  os, indent, subtreeName);

  PRINT_SUBTREE(cust);
}

TF_custom *TF_custom::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
    deepness--; listDeepness--;
  TF_custom *ret = new (pool) TF_custom(
    DBG_INFO_ARG_FWD_FIRST  pool, ((deepness>=0)&&cust)? cust->clone(DBG_INFO_ARG_FWD_FIRST  pool, deepness, listDeepness) : cust
  );
  return ret;
}

DEFN_AST_DOWNCASTS(ToplevelForm, TF_enum, TF_ENUM)

TF_enum::~TF_enum()
{
  while (enumerators.isNotEmpty()) {
    enumerators.removeFirst();
  }
}

void TF_enum::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, TF_enum);

  ToplevelForm::debugPrint(DBG_INFO_ARG_FWD_FIRST  os, indent, subtreeName);

  PRINT_STRING(name);
  PRINT_LIST(string, enumerators);
}

TF_enum *TF_enum::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
  TF_enum *ret = new (pool) TF_enum(
    DBG_INFO_ARG_FWD_FIRST  pool, name,
    shallowCloneASTList(pool, enumerators)
  );
  return ret;
}


// ------------------ ASTClass -------------------
// *** DO NOT EDIT ***
ASTClass::~ASTClass()
{
  //args.deleteAllOwning();
  //lastArgs.deleteAllOwning();
  //bases.deleteAllOwning();
  //decls.deleteAllOwning();
}

void ASTClass::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, ASTClass);

  PRINT_STRING(name);
  PRINT_LIST(CtorArg, getArgs());
  PRINT_LIST(CtorArg, getLastArgs());
  PRINT_LIST(BaseClass, bases);
  PRINT_LIST(Annotation, decls);
}

ASTClass *ASTClass::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
  ASTClass *ret = new (pool) ASTClass(
    DBG_INFO_ARG_FWD_FIRST  pool, name,
    cloneASTList(pool, args, deepness, listDeepness),
    cloneASTList(pool, lastArgs, deepness, listDeepness),
    cloneASTList(pool, bases, deepness, listDeepness),
    cloneASTList(pool, decls, deepness, listDeepness)
  );
  return ret;
}


// ------------------ AccessMod -------------------
// *** DO NOT EDIT ***
AccessMod::~AccessMod()
{
  while (mods.isNotEmpty()) {
    mods.removeFirst();
  }
}

void AccessMod::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, AccessMod);

  PRINT_GENERIC(acc);
  PRINT_LIST(string, mods);
}

AccessMod *AccessMod::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
  AccessMod *ret = new (pool) AccessMod(
    DBG_INFO_ARG_FWD_FIRST  pool, acc,
    shallowCloneASTList(pool, mods)
  );
  return ret;
}


// ------------------ Annotation -------------------
// *** DO NOT EDIT ***
Annotation::~Annotation()
{
}

char const * const Annotation::kindNames[Annotation::NUM_KINDS] = {
  "UserDecl",
  "CustomCode",
};

void Annotation::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
}

DEFN_AST_DOWNCASTS(Annotation, UserDecl, USERDECL)

UserDecl::~UserDecl()
{
  delete amod;
}

void UserDecl::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, UserDecl);

  Annotation::debugPrint(DBG_INFO_ARG_FWD_FIRST  os, indent, subtreeName);

  PRINT_SUBTREE(amod);
  PRINT_STRING(code);
  PRINT_STRING(init);
}

UserDecl *UserDecl::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
  UserDecl *ret = new (pool) UserDecl(
    DBG_INFO_ARG_FWD_FIRST  pool, ((deepness>=0)&&amod)? amod->clone(DBG_INFO_ARG_FWD_FIRST  pool, deepness, listDeepness) : amod,
    code,
    init
  );
  return ret;
}

DEFN_AST_DOWNCASTS(Annotation, CustomCode, CUSTOMCODE)

CustomCode::~CustomCode()
{
}

void CustomCode::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, CustomCode);

  Annotation::debugPrint(DBG_INFO_ARG_FWD_FIRST  os, indent, subtreeName);

  PRINT_STRING(qualifier);
  PRINT_STRING(code);
}

CustomCode *CustomCode::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
  CustomCode *ret = new (pool) CustomCode(
    DBG_INFO_ARG_FWD_FIRST  pool, qualifier,
    code
  );
  return ret;
}


// ------------------ CtorArg -------------------
// *** DO NOT EDIT ***
CtorArg::~CtorArg()
{
}

void CtorArg::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, CtorArg);

  PRINT_BOOL(isOwner);
  PRINT_STRING(type);
  PRINT_STRING(name);
  PRINT_STRING(defaultValue);
}

CtorArg *CtorArg::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
  CtorArg *ret = new (pool) CtorArg(
    DBG_INFO_ARG_FWD_FIRST  pool, isOwner,
    type,
    name,
    defaultValue
  );
  return ret;
}


// ------------------ BaseClass -------------------
// *** DO NOT EDIT ***
BaseClass::~BaseClass()
{
}

void BaseClass::debugPrint(DBG_INFO_FORMAL_FIRST  std::ostream &os, int indent, char const *subtreeName) const
{
  PRINT_HEADER(subtreeName, BaseClass);

  PRINT_GENERIC(access);
  PRINT_STRING(name);
}

BaseClass *BaseClass::clone(DBG_INFO_FORMAL_FIRST  str::StoragePool &pool, int deepness, int listDeepness) const
{
  BaseClass *ret = new (pool) BaseClass(
    DBG_INFO_ARG_FWD_FIRST  pool, access,
    name
  );
  return ret;
}


// *** DO NOT EDIT ***


#include "strutil.h"      // stringToupper

string toString(AccessCtl acc)
{
  char const *arr[] = {
    "public",
    "private",
    "protected",
    "ctor",
    "dtor",
    "pure_virtual"
  };
  STATIC_ASSERT(TABLESIZE(arr) == NUM_ACCESSCTLS);
  xassert((unsigned)acc < NUM_ACCESSCTLS);
  return string(arr[acc]);
}

string ASTClass::classKindName() const
{
  string ret = stringToupper(name);
  if (ret == name) {
    // this simplemindedly avoids collisions with itself, and I think
    // it even avoids collisions with other classes, since if they would
    // collide with this, they'd collide with themselves too, and hence
    // get an extra "KIND_" prepended..
    ret &= "KIND_";
  }
  return ret;
}

bool AccessMod::hasMod(char const *mod) const
{
  FOREACH_ASTLIST(string, mods, iter) {
    if (iter.data()->equals(mod)) {
      return true;
    }
  }
  return false;      // not found
}

bool AccessMod::hasModPrefix(char const *mod) const
{
  string mod0(mod);
  FOREACH_ASTLIST(string, mods, iter) {
    rostring i = *iter.data();
    if (prefixEquals(i, mod0)) {
      return true;
    }
  }
  return false;      // not found
}

string AccessMod::getModSuffixFromPrefix(char const *mod) const
{
  string mod0(DBG_INFO_ARG0_FIRST mod);
  string ret(DBG_INFO_ARG0);
  bool found = false;
  FOREACH_ASTLIST(string, mods, iter) {
    rostring s = *iter.data();
    if (prefixEquals(s, mod0)) {
      if (found) {
        xfailure(stringc << "two modifiers with this prefix found " << mod);
      }
      int len = strlen(mod);
      ret = s.substring(len, s.length()-len);
      found = true;
    }
  }
  if (!found) {
    xfailure(stringc << "no such prefix found " << mod);
  }
  return ret;
}



