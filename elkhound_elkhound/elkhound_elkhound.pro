TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ../elkhound/asockind.cc \
    ../elkhound/cyctimer.cc \
    ../elkhound/emitcode.cc \
    ../elkhound/forbid.gr.gen.cc \
    ../elkhound/genml.cc \
    ../elkhound/glr.cc \
    ../elkhound/glrmain.cc \
    ../elkhound/gramanl.cc \
    ../elkhound/gramast.ast.gen.cc \
    ../elkhound/gramast.cc \
    ../elkhound/gramexpl.cc \
    ../elkhound/gramlex.yy.cc \
    ../elkhound/grammar.cc \
    ../elkhound/grampar.cc \
    ../elkhound/grampar.tab.cc \
    ../elkhound/mlsstr.cc \
    ../elkhound/parsetables.cc \
    ../elkhound/parsstub.cc \
    ../elkhound/ptreeact.cc \
    ../elkhound/ptreenode.cc \
    ../elkhound/ssxmain.cc \
    ../elkhound/trcptr.cc \
    ../elkhound/trivbison.cc \
    ../elkhound/trivlex.cc \
    ../elkhound/trivmain.cc \
    ../elkhound/useract.cc \
    ../elkhound/tcheck/addrglob.c \
    ../elkhound/tcheck/exprpath.c \
    ../elkhound/tcheck/init.c \
    ../elkhound/tcheck/loops.c \
    ../elkhound/tcheck/morepaths.c

DISTFILES += \
    ../elkhound/parser.files \
    ../elkhound/libelkhound.a \
    ../elkhound/elkhound \
    ../elkhound/testcout \
    ../elkhound/asockind.o \
    ../elkhound/cyctimer.o \
    ../elkhound/emitcode.o \
    ../elkhound/genml.o \
    ../elkhound/glr.o \
    ../elkhound/gramast.ast.gen.o \
    ../elkhound/gramexpl.o \
    ../elkhound/gramlex.yy.o \
    ../elkhound/grammar.o \
    ../elkhound/grampar.o \
    ../elkhound/grampar.tab.o \
    ../elkhound/mlsstr.o \
    ../elkhound/parsetables.o \
    ../elkhound/ptreeact.o \
    ../elkhound/ptreenode.o \
    ../elkhound/testprog.o \
    ../elkhound/useract.o \
    ../elkhound/configure.pl \
    ../elkhound/find-extra-deps \
    ../elkhound/make-lrtable-graph \
    ../elkhound/make-tok \
    ../elkhound/make-trivparser.pl \
    ../elkhound/perf \
    ../elkhound/config.status \
    ../elkhound/config.summary \
    ../elkhound/configure \
    ../elkhound/mkdist \
    ../elkhound/parse-linux \
    ../elkhound/parse-tests \
    ../elkhound/regrtest \
    ../elkhound/test-all-config \
    ../elkhound/test-ambig-perf \
    ../elkhound/test-bad \
    ../elkhound/test-mlsstr-on-many \
    ../elkhound/algorithm.html \
    ../elkhound/faq.html \
    ../elkhound/index.html \
    ../elkhound/manual.html \
    ../elkhound/tutorial.html \
    ../elkhound/extradep.mk \
    ../elkhound/ffollow.gr \
    ../elkhound/ffollow_ext.gr \
    ../elkhound/forbid.gr \
    ../elkhound/grammar.txt \
    ../elkhound/license.txt \
    ../elkhound/parsgen.txt \
    ../elkhound/readme.txt \
    ../elkhound/tmp.txt \
    ../elkhound/asockind.d \
    ../elkhound/cyctimer.d \
    ../elkhound/emitcode.d \
    ../elkhound/genml.d \
    ../elkhound/glr.d \
    ../elkhound/gramast.ast.gen.d \
    ../elkhound/gramexpl.d \
    ../elkhound/gramlex.yy.d \
    ../elkhound/grammar.d \
    ../elkhound/grampar.d \
    ../elkhound/grampar.tab.d \
    ../elkhound/mlsstr.d \
    ../elkhound/parsetables.d \
    ../elkhound/ptreeact.d \
    ../elkhound/ptreenode.d \
    ../elkhound/useract.d \
    ../elkhound/Makefile \
    ../elkhound/Makefile.in \

HEADERS += \
    ../elkhound/asockind.h \
    ../elkhound/cdecl2.gr \
    ../elkhound/cyctimer.h \
    ../elkhound/emitcode.h \
    ../elkhound/flatutil.h \
    ../elkhound/forbid.gr.gen.h \
    ../elkhound/genml.h \
    ../elkhound/glr.h \
    ../elkhound/glrconfig.h \
    ../elkhound/glrconfig.h.in \
    ../elkhound/gramanl.h \
    ../elkhound/gramast.ast \
    ../elkhound/gramast.ast.gen.h \
    ../elkhound/gramast.h \
    ../elkhound/gramlex.lex \
    ../elkhound/grammar.h \
    ../elkhound/grampar.codes.h \
    ../elkhound/grampar.h \
    ../elkhound/grampar.tab.h \
    ../elkhound/grampar.y \
    ../elkhound/lexerint.h \
    ../elkhound/mlsstr.h \
    ../elkhound/ownerspec.h \
    ../elkhound/parsetables.h \
    ../elkhound/ptreeact.h \
    ../elkhound/ptreenode.h \
    ../elkhound/rcptr.h \
    ../elkhound/skeleton.gr \
    ../elkhound/ssxnode.h \
    ../elkhound/trivbison.h \
    ../elkhound/trivlex.h \
    ../elkhound/useract.h \
    ../elkhound/util.h \

INCLUDEPATH += ../smbase \
    ../ast \
    ../elkhound \
    ../elkhound/c \
