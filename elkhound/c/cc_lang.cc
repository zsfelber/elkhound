// cc_lang.cc            see license.txt for copyright and terms of use
// code for cc_lang.h, and default language settings

// at the moment there are laughably few settings -- that
// will change!

#include "cc_lang.h"     // this module

void CLang::ANSI_C()
{
  tagsAreTypes = false;
  recognizeCppKeywords = false;
}

void CLang::ANSI_Cplusplus()
{
  tagsAreTypes = true;
  recognizeCppKeywords = true;
}

