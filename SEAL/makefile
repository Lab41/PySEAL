# Makefile for the SEAL library
CXXFLAGS=-Wall -Wno-unused-function -std=c++11 -O3 -I.
UTIL_DIR=util
BIN_DIR=../bin
UTIL_SRCS=clipnormal.cpp common.cpp computation.cpp mempool.cpp \
	modulus.cpp polyarith.cpp polyarithmod.cpp polycore.cpp polyextras.cpp \
	polyfftmult.cpp polyfftmultmod.cpp polymodulus.cpp uintarith.cpp \
	uintarithmod.cpp uintcore.cpp uintextras.cpp
SRCS=bigpoly.cpp bigpolyarith.cpp biguint.cpp chooser.cpp decryptor.cpp \
	encoder.cpp encryptionparams.cpp encryptor.cpp evaluationkeys.cpp \
	evaluator.cpp keygenerator.cpp randomgen.cpp simulator.cpp utilities.cpp \
	$(addprefix $(UTIL_DIR)/,$(UTIL_SRCS))
OBJS=$(SRCS:.cpp=.o)
SEALLIB=$(BIN_DIR)/libseal.a

.PHONY: all clean

all: $(SEALLIB)

$(SEALLIB): $(OBJS)
	-mkdir -p $(BIN_DIR)
	ar rcs $(SEALLIB) $(OBJS)

clean:
	-rm -f $(OBJS) $(SEALLIB)