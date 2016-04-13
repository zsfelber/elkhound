TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ../ast/agrampar.cc \
    ../ast/agrampar.tab.cc \
    ../ast/ast.ast.cc \
    ../ast/ast.hand.cc \
    ../ast/astgen.cc \
    ../ast/asthelp.cc \
    ../ast/ccsstr.cc \
    ../ast/embedded.cc \
    ../ast/exampletest.cc \
    ../ast/fileloc.cc \
    ../ast/gramlex.cc \
    ../ast/locstr.cc \
    ../ast/reporterr.cc \
    ../ast/strtable.cc \
    ../ast/towner.cc \
    ../ast/xmlhelp.cc \
    ../ast/example.cc \
    ../ast/ext1.cc

DISTFILES += \
    ../ast/configure.pl \
    ../ast/configure \
    ../ast/index.html \
    ../ast/manual.html \
    ../ast/demo.ast \
    ../ast/example.ast \
    ../ast/ext1.ast \
    ../ast/license.txt \
    ../ast/readme.txt \
    ../ast/tiny.ast \
    ../ast/Makefile.in \
    ../ast/ext1.o \
    ../ast/ext1.d

HEADERS += \
    ../ast/agramlex.lex \
    ../ast/agrampar.codes.h \
    ../ast/agrampar.h \
    ../ast/agrampar.tab.h \
    ../ast/agrampar.y \
    ../ast/ast.ast \
    ../ast/ast.ast.h \
    ../ast/ast.hand.h \
    ../ast/asthelp.h \
    ../ast/ccsstr.h \
    ../ast/embedded.h \
    ../ast/fakelist.h \
    ../ast/fileloc.h \
    ../ast/gramlex.h \
    ../ast/locstr.h \
    ../ast/reporterr.h \
    ../ast/strtable.h \
    ../ast/xmlhelp.h \
    ../ast/example.h \
    ../ast/ext1.h \
    ../ast/ast.h

INCLUDEPATH += ../smbase
