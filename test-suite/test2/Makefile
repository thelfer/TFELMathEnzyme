CPPFLAGS=-std=c++20 $(shell tfel-config --includes) -I /home/tom/codes/tfel-math-enzyme/master/install/include
CXXFLAGS1=-fno-vectorize -fno-slp-vectorize -fno-unroll-loops
CXXFLAGS2=-O3
OPTFLAGS=-load-pass-plugin=/home/tom/codes/enzyme/master/install-llvm-17/lib/LLVMEnzyme-17.so
LDFLAGS=$(shell tfel-config --libs --math --tests)

.PHONY = all clean test.exe

all: test.exe

test.exe: test.cxx
	echo $(LDFLAGS)
	clang++-17 $(CPPFLAGS) test.cxx -S -emit-llvm -o input.ll $(CXXFLAGS1)
	opt-17 input.ll $(OPTFLAGS) --passes=enzyme  -o output.ll -S
	opt-17 output.ll $(CXXFLAGS2) -o output_opt.ll -S
	clang++-17 output_opt.ll -o test.exe $(LDFLAGS)

clean:
	$(RM) input.ll output.ll output_opt.ll tfel-math-enzyme-2.xml test.exe
