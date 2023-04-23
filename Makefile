prefix=/usr/local
INSTALL=install
REVISION=$(shell git show --format='%h' -s)

CONF=Debug

CND_BUILDDIR=build
CND_CONF=$(CONF)
CND_PLATFORM=CLang-Linux
PCHDIR=inc/$(CND_BUILDDIR)/$(CND_CONF)/$(CND_PLATFORM)

vpath %.h.gch $(PCHDIR)

MKDIR=mkdir
UNAME_S=$(shell uname -s)
UNAME_P=$(shell uname -p)
UNAME_M=$(shell uname -m)

COPYRIGHT=(C) 2023 SuitableApp
PACKAGE_BUGREPORT=$(shell awk '/Packager:/ {for(i=2;i<NF;++i){printf("%s ",$$i)}print $$NF}' xtru.spec)
VENDOR=$(shell awk '/Vendor:/ {for(i=2;i<NF;++i){printf("%s ",$$i)}print $$NF}' xtru.spec)

TITLE_OF_XTRU=$(shell awk '/Summary:/ {for(i=2;i<NF;++i){printf("%s ",$$i)}print $$NF}' xtru.spec)
VERSION_OF_XTRU=$(shell awk '/Version:/ { print $$2}' xtru.spec)

TITLE_OF_MPX=Multiplexed Processes eXecuter
VERSION_OF_MPX=5.3.0

ifeq ($(UNAME_S),SunOS)
PLATFORM_CXXFLAGS=-D_GLIBCXX_USE_C99_MATH
# -Wno-deprecated-declarations   # unnecessary for boost_1_68_0
SUN_SDK_HOME=$(subst /bin/CC,,$(shell which CC))
PKG=$(shell awk -F= '/PKG=/ { print $$2}' pkginfo)
PKG_BASE_DIR=$(shell awk -F= '/BASEDIR=/ { print $$2}' pkginfo)/$(PKG)
PLATFORM_OCCI_LDFLAGS=-L$(SUN_SDK_HOME)/lib/compilers/$(shell isainfo -n) -lCrunG3
else
CC=clang
CCC=clang++
CXX=clang++
# CC=ccache gcc
# CCC=ccache g++
# CXX=ccache g++
PLATFORM_CXXFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0 -gdwarf-4
PLATFORM_OCCI_LDFLAGS=
PKG_BASE_DIR=${prefix}
endif

ifeq ($(UNAME_M),x86_64)
PLATFORM_FROF_LDFLAGS=-lprofiler -lunwind -ltcmalloc
else
PLATFORM_FROF_LDFLAGS=-lprofiler -lunwind
endif

ifeq ($(lastword $(CC)),clang)
PLATFORM_FROF_LDFLAGS+= -fuse-ld=lld
endif

PCH_OBJECTS=pslib.h.gch

ifeq ($(CONF),Release)
OPTIMIZING=-O3 -DNDEBUG
else
OPTIMIZING=-O0
endif

ifeq ($(lastword $(CXX)),clang++)
OPTIMIZING+=-g -fstandalone-debug
else
OPTIMIZING+=-g
endif

.SUFFIXES: .h.gch

.PHONY: all

.PHONY: clean

CPPFLAGS=-MMD -isystem $${OCCI_INC_PATH}

ifeq ($(lastword $(CXX)),clang++)
PCH_OPTS=-include-pch $(PCHDIR)/${PCH_OBJECTS}
else
PCH_OPTS=-I$(PCHDIR)
endif

CXXFLAGS=$(OPTIMIZING) -pthread -Wall $(PLATFORM_CXXFLAGS)

LDFLAGS=-Wall -rdynamic -lstdc++ -lm -ldl -lpthread $(PLATFORM_FROF_LDFLAGS)

LIB_BOOST=-lboost_{date_time,iostreams,regex,serialization,system,thread,program_options,filesystem}-mt

LIB_MPX=$(LIB_BOOST)

LIB_XTRU=$(LIB_BOOST) -L$${OCCI_LIB_PATH} -locci -lclntsh $(PLATFORM_OCCI_LDFLAGS)

OBJS_LIB=$(patsubst %.cpp,%.o,$(wildcard lib/*.cpp lib/sql/*.cpp lib/sql/lite3/*.cpp lib/sql/occi/*.cpp lib/nsStreamLocator/*.cpp lib/system/*.cpp ))

OBJS_XTRU=$(patsubst %.cpp,%.o,$(wildcard app/xtru/*.cpp app/xtru/copydd/*.cpp app/xtru/getdata/*.cpp app/xtru/getmeta/*.cpp ))

OBJS_MPX=$(patsubst %.cpp,%.o,$(wildcard app/mpx/*.cpp ))

OBJS_MKCRD=$(patsubst %.cpp,%.o,$(wildcard app/mkcrd/*.cpp app/mkcrd/nsCompo/*.cpp ))

CONFIG_H=-DPACKAGE_BUGREPORT="\"$(PACKAGE_BUGREPORT)\"" -DCOPYRIGHT="\"$(COPYRIGHT)\""

all: $(PCH_OBJECTS) lib/libps.a build/mkcrd build/mpx build/xtru

NB_WORK=$(shell find app demo inc lib -regex "\(.*\)/\(Debug\|Release\)\(.*\)\.[oda]$$")

clean:
	find inc/ lib/ demo/ app/ \( -name "*.o" -o -name "*.a" -o -name "*.d" \) -exec $(RM) {} +;
	$(RM) -r build/

clean_gch:
	for i in $(PCH_OBJECTS) $(PCH_OBJECTS .gch=.d) ; do find inc -name $$i -exec $(RM) {} \; ; done

clean_nb:
	$(RM) $(NB_WORK)

install:
	${INSTALL} -m755 -d ${prefix}/bin ${prefix}/share/xtru-$(VERSION_OF_XTRU)
	${INSTALL} -m755 -t ${prefix}/bin build/mkcrd build/mpx build/xtru
	${INSTALL} -m644 -t ${prefix}/share/xtru-$(VERSION_OF_XTRU) conf.tar

uninstall:
	$(RM) -r ${prefix}/bin/{xtru,mpx,mkcrd} ${prefix}/share/xtru-$(VERSION_OF_XTRU)

dist_solaris:
	${MAKE} CONF=Release prefix=${PKG} INSTALL=ginstall install
	echo "$(COPYRIGHT)" > copyright
	pkgproto ${PKG} | gsed -e "s|$$(id -un) $$(id -gn)|root bin|g" > prototype
	echo "i pkginfo\ni depend\ni compver\ni copyright\ni request\ni checkinstall\ni postremove\ni preinstall" >> prototype
	echo 'd none /usr ? ? ?' >> prototype
	echo 'd none /usr/local ? ? ?' >> prototype
	echo 'd none /usr/local/bin ? ? ?' >> prototype
	echo 'd none /usr/local/share ? ? ?' >> prototype
	echo 's none /usr/local/bin/mpx=../../../$$BASEDIR/PLM6xtru/bin/mpx' >> prototype
	echo 's none /usr/local/bin/xtru=../../../$$BASEDIR/PLM6xtru/bin/xtru' >> prototype
	echo 's none /usr/local/bin/mkcrd=../../../$$BASEDIR/PLM6xtru/bin/mkcrd' >> prototype
	echo 's none /usr/local/share/xtru-5.3.0=../../../$$BASEDIR/PLM6xtru/share/xtru-5.3.0' >> prototype
	pkgmk -o -b ${PWD} -p ${REVISION} -a ${UNAME_P} -v ${VERSION_OF_XTRU} NAME="$(TITLE_OF_XTRU)" CLASSES=none EMAIL="$(PACKAGE_BUGREPORT)" VENDOR="$(VENDOR)"
	pkgtrans /var/spool/pkg/ ${PKG}.pkg ${PKG}

build/mkcrd: $(OBJS_MKCRD)
	$(MKDIR) -p `dirname $@`
	$(LINK.o) $(OUTPUT_OPTION) $^

build/mpx: $(OBJS_MPX) lib/libps.a
	$(MKDIR) -p `dirname $@`
	$(LINK.o) $(OUTPUT_OPTION) $(LIB_MPX) $^

build/xtru: $(OBJS_XTRU) lib/libps.a
	$(MKDIR) -p `dirname $@`
	$(LINK.o) $(OUTPUT_OPTION) $(LIB_XTRU) $^

app/mkcrd/mkcrd.o: override CPPFLAGS+=-DPACKAGE="\"MKCRD\"" \
	$(CONFIG_H)

app/mpx/mpx.o: override CPPFLAGS+=-DPACKAGE="\"MPX\"" \
	$(CONFIG_H) \
	-DPACKAGE_NAME="\"mpx\"" \
	-DTITLE="\"$(TITLE_OF_MPX)\"" \
	-DVERSION="\"$(VERSION_OF_MPX)\"" \
	-DPKG_BASE_DIR="\"$(PKG_BASE_DIR)\"" \
	-DPKG_DATA_DIR="\"/share/xtru-$(VERSION_OF_XTRU)\""

app/xtru/xtru.o: override CPPFLAGS+=-DPACKAGE="\"XTRU\"" \
	$(CONFIG_H) \
	-DPACKAGE_NAME="\"xtru\"" \
	-DTITLE="\"$(TITLE_OF_XTRU)\"" \
	-DVERSION="\"$(VERSION_OF_XTRU)\"" \
	-DPKG_BASE_DIR="\"$(PKG_BASE_DIR)\"" \
	-DPKG_DATA_DIR="\"/share/xtru-$(VERSION_OF_XTRU)\""

# "$?" is replaced only newly dependencies. In contrast, "$^" is replaced all dependencies.
lib/libps.a: $(OBJS_LIB)
	$(AR) r $@ $^

$(OBJS_XTRU) $(OBJS_LIB): $(PCH_OBJECTS)

build/mkcrd: override LDFLAGS+= -lcrypto

lib/%.o: override CPPFLAGS+= -Iinc $(PCH_OPTS)

app/xtru/%.o: override CPPFLAGS+= -Iapp/xtru -Iinc $(PCH_OPTS)

app/mpx/%.o: override CPPFLAGS+= -Iapp/mpx -Iinc $(PCH_OPTS)

%.h.gch: override CPPFLAGS+= -x c++-header

${PCHDIR}/${PCH_OBJECTS}: inc/$(PCH_OBJECTS:.h.gch=.h)
	$(MKDIR) -p ${PCHDIR}
	@echo "*** PCH $@ is now generating... ***"
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

${PCHDIR}/${PCH_OBJECTS:.gch=.d}: ${PCHDIR}/${PCH_OBJECTS}

-include ${PCHDIR}/${PCH_OBJECTS:.gch=.d}

-include lib/sql/occi/cAttr.d

-include app/xtru/getdata/cUnload.d

-include app/xtru/getdata/cQuery.d

-include app/xtru/getmeta/cGetMeta.d

$(patsubst %.cpp,%.o,$(wildcard app/xtru/getmeta/*.cpp)): app/xtru/getmeta/nsGetMeta.h

