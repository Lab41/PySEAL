# Makefile for SEAL examples
BIN_DIR=../bin
LIB_DIR=../bin
INCLUDE_DIR=../SEAL
SRCS=main.cpp
SEALRUN=$(BIN_DIR)/sealexamples
CXX=g++
CXXFLAGS=-march=native -O3 -std=c++11
LDFLAGS=

.PHONY : all clean

all : $(SEALRUN)

$(SEALRUN) : $(SRCS)
	@-mkdir -p $(dir $@)
	$(CXX) $(SRCS) $(CXXFLAGS) $(LDFLAGS) $(addprefix -I,$(INCLUDE_DIR)) $(addprefix -L,$(LIB_DIR)) -lseal -o $@

clean :
	@-rm -f $(OBJS) $(SEALRUN)
