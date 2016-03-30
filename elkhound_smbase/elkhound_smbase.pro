TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ../smbase/autofile.cc \
    ../smbase/bflatten.cc \
    ../smbase/bit2d.cc \
    ../smbase/bitarray.cc \
    ../smbase/boxprint.cc \
    ../smbase/breaker.cpp \
    ../smbase/crc.cpp \
    ../smbase/datablok.cpp \
    ../smbase/exc.cpp \
    ../smbase/flatten.cc \
    ../smbase/growbuf.cc \
    ../smbase/hashline.cc \
    ../smbase/hashtbl.cc \
    ../smbase/missing.cpp \
    ../smbase/mysig.cc \
    ../smbase/nonport.cpp \
    ../smbase/point.cc \
    ../smbase/pprint.cc \
    ../smbase/smregexp.cc \
    ../smbase/srcloc.cc \
    ../smbase/srcloc.test.cc \
    ../smbase/srcloc.test2.cc \
    ../smbase/str.cpp \
    ../smbase/strdict.cc \
    ../smbase/strhash.cc \
    ../smbase/stringset.cc \
    ../smbase/strtokp.cpp \
    ../smbase/strutil.cc \
    ../smbase/svdict.cc \
    ../smbase/syserr.cpp \
    ../smbase/taillist_test.cc \
    ../smbase/tarrayqueue.cc \
    ../smbase/testarray.cc \
    ../smbase/testcout.cc \
    ../smbase/testmalloc.cc \
    ../smbase/tobjlist.cc \
    ../smbase/tobjpool.cc \
    ../smbase/trace.cc \
    ../smbase/trdelete.cc \
    ../smbase/tsobjlist.cc \
    ../smbase/vdtllist.cc \
    ../smbase/voidlist.cc \
    ../smbase/vptrmap.cc \
    ../smbase/warn.cpp \
    ../smbase/cycles.c \
    ../smbase/gprintf.c \
    ../smbase/malloc.c \
    ../smbase/malloc_stub.c \
    ../smbase/mypopen.c \
    ../smbase/tmalloc.c \
    ../smbase/unixutil.c \
    ../smbase/int.cc

DISTFILES += \
    ../smbase/SMBASE.IDE \
    ../smbase/test.input \
    ../smbase/libsmbase.a \
    ../smbase/testcout \
    ../smbase/autofile.o \
    ../smbase/bflatten.o \
    ../smbase/bit2d.o \
    ../smbase/bitarray.o \
    ../smbase/boxprint.o \
    ../smbase/breaker.o \
    ../smbase/crc.o \
    ../smbase/cycles.o \
    ../smbase/datablok.o \
    ../smbase/exc.o \
    ../smbase/flatten.o \
    ../smbase/gprintf.o \
    ../smbase/growbuf.o \
    ../smbase/hashline.o \
    ../smbase/hashtbl.o \
    ../smbase/malloc_stub.o \
    ../smbase/missing.o \
    ../smbase/mypopen.o \
    ../smbase/mysig.o \
    ../smbase/nonport.o \
    ../smbase/point.o \
    ../smbase/pprint.o \
    ../smbase/smregexp.o \
    ../smbase/srcloc.o \
    ../smbase/str.o \
    ../smbase/strdict.o \
    ../smbase/strhash.o \
    ../smbase/stringset.o \
    ../smbase/strtokp.o \
    ../smbase/strutil.o \
    ../smbase/svdict.o \
    ../smbase/syserr.o \
    ../smbase/trace.o \
    ../smbase/trdelete.o \
    ../smbase/unixutil.o \
    ../smbase/vdtllist.o \
    ../smbase/voidlist.o \
    ../smbase/vptrmap.o \
    ../smbase/warn.o \
    ../smbase/check-malloc-trace.pl \
    ../smbase/codepatch.pl \
    ../smbase/configure.pl \
    ../smbase/depend.pl \
    ../smbase/run-flex.pl \
    ../smbase/scan-depends.pl \
    ../smbase/sm_config.pm \
    ../smbase/config.status \
    ../smbase/config.summary \
    ../smbase/configure \
    ../smbase/depend.sh \
    ../smbase/do_strhash_test \
    ../smbase/make-preproc \
    ../smbase/smbase.gif \
    ../smbase/index.html \
    ../smbase/trace.html \
    ../smbase/license.txt \
    ../smbase/smbase.dsp \
    ../smbase/string.txt \
    ../smbase/autofile.d \
    ../smbase/bflatten.d \
    ../smbase/bit2d.d \
    ../smbase/bitarray.d \
    ../smbase/boxprint.d \
    ../smbase/breaker.d \
    ../smbase/crc.d \
    ../smbase/datablok.d \
    ../smbase/exc.d \
    ../smbase/flatten.d \
    ../smbase/growbuf.d \
    ../smbase/hashline.d \
    ../smbase/hashtbl.d \
    ../smbase/missing.d \
    ../smbase/nonport.d \
    ../smbase/point.d \
    ../smbase/pprint.d \
    ../smbase/smregexp.d \
    ../smbase/srcloc.d \
    ../smbase/str.d \
    ../smbase/strdict.d \
    ../smbase/strhash.d \
    ../smbase/stringset.d \
    ../smbase/strtokp.d \
    ../smbase/strutil.d \
    ../smbase/svdict.d \
    ../smbase/syserr.d \
    ../smbase/trace.d \
    ../smbase/trdelete.d \
    ../smbase/vdtllist.d \
    ../smbase/voidlist.d \
    ../smbase/vptrmap.d \
    ../smbase/warn.d \
    ../smbase/Makefile \
    ../smbase/Makefile.in

HEADERS += \
    ../smbase/array.h \
    ../smbase/arraymap.h \
    ../smbase/arrayqueue.h \
    ../smbase/astlist.h \
    ../smbase/autofile.h \
    ../smbase/bflatten.h \
    ../smbase/bit2d.h \
    ../smbase/bitarray.h \
    ../smbase/boxprint.h \
    ../smbase/breaker.h \
    ../smbase/ckheap.h \
    ../smbase/crc.h \
    ../smbase/cycles.h \
    ../smbase/datablok.h \
    ../smbase/exc.h \
    ../smbase/flatten.h \
    ../smbase/gprintf.h \
    ../smbase/growbuf.h \
    ../smbase/hashline.h \
    ../smbase/hashtbl.h \
    ../smbase/macros.h \
    ../smbase/missing.h \
    ../smbase/mypopen.h \
    ../smbase/mysig.h \
    ../smbase/nonport.h \
    ../smbase/objlist.h \
    ../smbase/objmap.h \
    ../smbase/objpool.h \
    ../smbase/objstack.h \
    ../smbase/ohashtbl.h \
    ../smbase/okhasharr.h \
    ../smbase/okhashtbl.h \
    ../smbase/oobjmap.h \
    ../smbase/owner.h \
    ../smbase/pair.h \
    ../smbase/point.h \
    ../smbase/pprint.h \
    ../smbase/ptrmap.h \
    ../smbase/sm_flexlexer.h \
    ../smbase/smregexp.h \
    ../smbase/sobjlist.h \
    ../smbase/sobjset.h \
    ../smbase/sobjstack.h \
    ../smbase/srcloc.h \
    ../smbase/str.h \
    ../smbase/strdict.h \
    ../smbase/strhash.h \
    ../smbase/stringset.h \
    ../smbase/strobjdict.h \
    ../smbase/strsobjdict.h \
    ../smbase/strtokp.h \
    ../smbase/strutil.h \
    ../smbase/svdict.h \
    ../smbase/syserr.h \
    ../smbase/taillist.h \
    ../smbase/test.h \
    ../smbase/thashtbl.h \
    ../smbase/trace.h \
    ../smbase/trdelete.h \
    ../smbase/typ.h \
    ../smbase/unixutil.h \
    ../smbase/vdtllist.h \
    ../smbase/voidlist.h \
    ../smbase/vptrmap.h \
    ../smbase/warn.h \
    ../smbase/xassert.h \
    ../smbase/xobjlist.h \
    ../smbase/storage.h \
    ../smbase/int.h

