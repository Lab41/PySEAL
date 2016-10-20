# Makefile for SEAL examples
BIN_DIR=../bin
LIB_DIR=/usr/local/lib/SEAL ../bin
INCLUDE_DIR=/usr/local/include/SEAL ../SEAL
SRCS=main.cpp
SEALRUN=$(BIN_DIR)/sealexamples
CXX=g++
CXXFLAGS=-O3 -std=c++11
LDFLAGS=

.PHONY : all clean

all : $(SEALRUN)

$(SEALRUN) : $(SRCS)
	@-mkdir -p $(dir $@)
	$(CXX) $(SRCS) $(CXXFLAGS) $(LDFLAGS) $(addprefix -I,$(INCLUDE_DIR)) $(addprefix -L,$(LIB_DIR)) -lseal -o $@

clean :
	@-rm -f $(OBJS) $(SEALRUN)
	@-rmdir --ignore-fail-on-non-empty $(BIN_DIR)
