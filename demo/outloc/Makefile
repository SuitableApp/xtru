UNAME_S=$(shell uname -s)
UNAME_P=$(shell uname -p)

INCLUDE := -I$(HOME)/include -I../../inc -I$(ORACLE_HOME)/rdbms/public
SRC_DIR := ../../lib/nsStreamLocator
OBJ_DIR := ../../lib/nsStreamLocator
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJS_outloc := $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.cpp=.o)) main.o
DEPS_outloc := $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.cpp=.d)) main.d
OBJS_child := child.o
DEPS_child := child.d

ifeq ($(UNAME_S),SunOS)
PLATFORM_CXXFLAGS=-pthreads -D_GLIBCXX_USE_C99_MATH
SUN_SDK_HOME=$(subst /bin/CC,,$(shell which CC))
ifeq ($(UNAME_P),sparc)
PLATFORM_LDFLAGS=-L$(SUN_SDK_HOME)/lib/compilers/sparcv9 -lCrunG3
else
PLATFORM_LDFLAGS=-L$(SUN_SDK_HOME)/lib/compilers/amd64 -lCrunG3
endif
else
CC=clang
CCC=clang++
CXX=clang++
PLATFORM_LDFLAGS=-lprofiler -ltcmalloc
endif

ifeq ($(CONF),Release)
OPTIMIZING=-O3 -DNDEBUG
else
OPTIMIZING=-O0 -g
endif

ifeq ($(lastword $(CXX)),clang++)
PCH_OPTS= -include-pch ../../inc/build/Debug/CLang-Linux/pslib.h.gch
else
endif

CXXFLAGS= -std=c++17 -Wall -m64 -pipe $(PLATFORM_CXXFLAGS)

all: outloc child

-include $(DEPS_outloc) $(DEPS_child)

outloc:  override LDFLAGS=-Wall -m64 -L$(HOME)/lib -lboost_{system,date_time,thread,program_options,filesystem,iostreams,regex,serialization}-mt \
 -lstdc++ -lm -ldl -lpthread -rdynamic $(PLATFORM_LDFLAGS)

outloc: $(OBJS_outloc) ../../lib/libps.a
	$(LINK.o) -o $@ $^

child:  override LDFLAGS=-Wall -m64 \
 -lstdc++ -lm -ldl -lpthread -rdynamic $(PLATFORM_LDFLAGS)

child:  $(OBJS_child)
	$(LINK.o) -o $@ $^

$(OBJ_DIR)/%.o: override CPPFLAGS=$(INCLUDE) $(OPTIMIZING) $(PCH_OPTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(COMPILE.cc) -MMD -MP $< -o $@

main.o: override CPPFLAGS=$(INCLUDE) $(OPTIMIZING) $(PCH_OPTS)

main.o: main.cpp
	$(COMPILE.cc) -MMD -MP $< -o $@

child.o: child.cpp
	$(COMPILE.cc) -MMD -MP $< -o $@

clean:
	$(RM) outloc $(OBJS_outloc) $(DEPS_outloc) child $(OBJS_child) $(DEPS_child)

