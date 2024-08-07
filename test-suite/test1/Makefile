CPPFLAGS=-std=c++20 $(shell tfel-config --includes) -I /home/th202608/codes/tfel-math-enzyme/master/src/TFELMathEnzyme/include
CXXFLAGS1=-fno-vectorize -fno-slp-vectorize -fno-unroll-loops
CXXFLAGS2=-O3
OPTFLAGS=-load-pass-plugin=/home/th202608/codes/enzyme/master/install-llvm-18.1.8/lib/LLVMEnzyme-18.so
LDFLAGS=$(shell tfel-config --libs --math --tests) -lstdc++ -lm

.PHONY = all clean test.exe

all: test.exe

test.exe: test.cxx
	clang $(CPPFLAGS) $(CXXFLAGS1) test.cxx -S -emit-llvm -o input.ll $(CXXFLAGS1)
	opt input.ll $(OPTFLAGS) --passes=enzyme  -o output.ll -S
	opt output.ll $(CXXFLAGS2) -o output_opt.ll -S
	clang output_opt.ll -o test.exe $(LDFLAGS)

clean:
	$(RM) input.ll output.ll output_opt.ll tfel-math-enzyme.xml test.exe
