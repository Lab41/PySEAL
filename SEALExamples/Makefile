# Makefile for SEAL examples
BIN_DIR=../bin
INCLUDE_DIR=../SEAL
SRCS=main.cpp
SEALRUN=$(BIN_DIR)/sealexamples
CXX=g++
CXXFLAGS=-O3 -std=c++11
LDFLAGS=

.PHONY : all clean

all : $(SEALRUN)

$(SEALRUN) : $(SRCS)
	@-mkdir -p $(dir $@)
	$(CXX) $(SRCS) $(CXXFLAGS) $(LDFLAGS) -I$(INCLUDE_DIR) -L$(BIN_DIR) -lseal -o $@

clean :
	@-rm -f $(OBJS) $(SEALRUN)
	@-rmdir --ignore-fail-on-non-empty $(BIN_DIR)
