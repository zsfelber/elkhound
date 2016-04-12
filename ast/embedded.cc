// embedded.cc            see license.txt for copyright and terms of use
// code for embedded.h

#include "embedded.h"     // EmbeddedLang

EmbeddedLang::EmbeddedLang(ReportError *e)
  : err(e? e : &simpleReportError),
    text(DBG_INFO_ARG0 ),
    exprOnly(false),
    isDeclaration(false)
{}

EmbeddedLang::~EmbeddedLang()
{}
