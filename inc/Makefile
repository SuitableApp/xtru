# Pre-compiled hedders

# Macros
CND_PLATFORM=CLang-Linux
CND_CONF=${CONF}
CND_BUILDDIR=build
PCHDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

vpath %.h.gch $(PCHDIR)

# Environment 
MKDIR=mkdir
CC=clang
CCC=clang++
CXX=clang++

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

ORACLE_HOME=/usr/lib/oracle/product/12.1.0/client_1

CPPFLAGS=-MMD -Iinc -I$${ORACLE_HOME}/rdbms/public

define static_part
endef

CXXFLAGS=$(OPTIMIZING) -std=c++11 -pthread -Wall -m64

all: $(PCH_OBJECTS)

clean:
	$(RM) -r ${PCHDIR}

%.h.gch:
	$(MKDIR) -p ${PCHDIR}
	@echo "*** PCH $@ is now generating... ***"
	@if [ "$(@D)" == "." ]; \
	then $(COMPILE.cc) -o ${PCHDIR}/$@ ${@:.h.gch=.h} ; \
	 sed -i -e "/include\/boost/d" -e "/oracle\/product/d" ${PCHDIR}/${@:.gch=.d} ; \
	else $(COMPILE.cc) -o $@ ${@F:.h.gch=.h} ; \
	 sed -i -e "/include\/boost/d" -e "/oracle\/product/d" ${@:.gch=.d} ; \
	fi	

%.h.gch: override CPPFLAGS+=-x c++-header

-include ${PCHDIR}/pslib.h.d

