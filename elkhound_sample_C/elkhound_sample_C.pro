TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ../elkhound/c/bcparse.cc \
    ../elkhound/c/c.ast.gen.cc \
    ../elkhound/c/cc_flags.cc \
    ../elkhound/c/cc_lang.cc \
    ../elkhound/c/c_env.cc \
    ../elkhound/c/cparse.cc \
    ../elkhound/c/c_type.cc \
    ../elkhound/c/c_variable.cc \
    ../elkhound/c/exprequal.cc \
    ../elkhound/c/exprvisit.cc \
    ../elkhound/c/lexer1.cc \
    ../elkhound/c/lexer1yy.cc \
    ../elkhound/c/lexer2.cc \
    ../elkhound/c/main.cc \
    ../elkhound/c/parssppt.cc \
    ../elkhound/c/paths.cc \
    ../elkhound/c/postorder.cc \
    ../elkhound/c/stubs.cc \
    ../elkhound/c/tcheck.cc \
    ../elkhound/c/treeout.cc

DISTFILES += \
    ../elkhound/c/testcout \
    ../elkhound/c/c.ast.gen.o \
    ../elkhound/c/cc_flags.o \
    ../elkhound/c/cc_lang.o \
    ../elkhound/c/c_env.o \
    ../elkhound/c/cparse.o \
    ../elkhound/c/c_type.o \
    ../elkhound/c/c_variable.o \
    ../elkhound/c/exprequal.o \
    ../elkhound/c/exprvisit.o \
    ../elkhound/c/lexer1.o \
    ../elkhound/c/lexer1yy.o \
    ../elkhound/c/lexer2.o \
    ../elkhound/c/parssppt.o \
    ../elkhound/c/paths.o \
    ../elkhound/c/postorder.o \
    ../elkhound/c/stubs.o \
    ../elkhound/c/tcheck.o \
    ../elkhound/c/treeout.o \
    ../elkhound/c/configure.pl \
    ../elkhound/c/config.status \
    ../elkhound/c/config.summary \
    ../elkhound/c/configure \
    ../elkhound/c/c.ast.gen.d \
    ../elkhound/c/cc_flags.d \
    ../elkhound/c/cc_lang.d \
    ../elkhound/c/c_env.d \
    ../elkhound/c/cparse.d \
    ../elkhound/c/c_type.d \
    ../elkhound/c/c_variable.d \
    ../elkhound/c/exprequal.d \
    ../elkhound/c/exprvisit.d \
    ../elkhound/c/lexer1.d \
    ../elkhound/c/lexer1yy.d \
    ../elkhound/c/lexer2.d \
    ../elkhound/c/parssppt.d \
    ../elkhound/c/paths.d \
    ../elkhound/c/postorder.d \
    ../elkhound/c/stubs.d \
    ../elkhound/c/tcheck.d \
    ../elkhound/c/treeout.d \
    ../elkhound/c/Makefile \
    ../elkhound/c/Makefile.in

HEADERS += \
    ../elkhound/c/bcparse.h \
    ../elkhound/c/c.ast \
    ../elkhound/c/c.ast.gen.h \
    ../elkhound/c/c.gr \
    ../elkhound/c/cc_flags.h \
    ../elkhound/c/cc_lang.h \
    ../elkhound/c/c_env.h \
    ../elkhound/c/cparse.h \
    ../elkhound/c/c_type.h \
    ../elkhound/c/c_variable.h \
    ../elkhound/c/exprequal.h \
    ../elkhound/c/exprvisit.h \
    ../elkhound/c/lexer1.h \
    ../elkhound/c/lexer1.lex \
    ../elkhound/c/lexer2.h \
    ../elkhound/c/parssppt.h \
    ../elkhound/c/paths.h \
    ../elkhound/c/treeout.h

