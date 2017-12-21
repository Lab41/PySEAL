# Makefile for SEAL examples
BIN_DIR=../bin
LIB_DIR=../bin
INCLUDE_DIR=../SEAL
SRCS=main.cpp
SEALRUN=$(BIN_DIR)/sealexamples
CXX=g++
CXXFLAGS=-march=native -O3 -DHAVE_CONFIG_H -std=c++11 -pthread
LDFLAGS=

# GCC-5 fails at compiling std::to_string without #define _GLIBCXX_USE_C99
ifeq ($(shell $(CXX) --version | grep ^$(CXX) | sed 's/^.* \([0-9]\)\..*/\1/'), 5)
	CXXFLAGS += -D_GLIBCXX_USE_C99
endif

.PHONY : all clean

all : $(SEALRUN)

$(SEALRUN) : $(SRCS)
	@-mkdir -p $(dir $@)
	$(CXX) $(SRCS) $(CXXFLAGS) $(LDFLAGS) $(addprefix -I,$(INCLUDE_DIR)) $(addprefix -L,$(LIB_DIR)) -lseal -o $@

clean :
	@-rm -f $(OBJS) $(SEALRUN)
