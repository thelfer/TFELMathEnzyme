Basic Installation
==================

These are generic installation instructions.

cmake attempts to guess correct values for various system-dependent
variables used during compilation.  It uses those values to create 
a `Makefile` in each directory of the package.

The simplest way to compile this package is:

1. `cd` to the directory containing the package's source code and type
  `cmake .` to configure the package for your system. This will build
  `TFELMathEnzyme` in place.

   However, we strongly recommend building `TFELMathEnzyme` out of the source
   tree as follows:
   
   - Create a `build` directory et go in this directory
   - Call `cmake` to configure the directory

   Useful options are listed below.

   Running `cmake` takes awhile.  While running, it prints some
   messages telling which features it is checking for.

2. Type `make` to compile the package.

3. Optionally, type `ctest` to run any self-tests that come with
  the package.

4. Type `make install` to install the headers and any data files and
  documentation.

5. You can remove the program binaries and object files from the source
  code directory by typing `make clean`.

`cmake` usefull variables
=======================

- `CMAKE_BUILD_TYPE`    : two values are supported `Release` and `Debug`
- `CASTEM_INSTALL_PATH` : specify where the castem has been installed
- `Enzyme_DIR`          : path to where `Enzyme` is installed
- `TFEL_DIR`            : path to where `TFEL` is installed

`cmake` typical usage
=====================

~~~~{.bash}
mkdir build
cd build
export CXXFLAGS="-O2 -Wall -W -pedantic"
cmake ../TFELMathEnzyme/                                                   \
      -DEnzyme_DIR=/home/th202608/codes/enzyme/master/install-llvm-18.1.8/ \
      -DTFEL_DIR=/home/th202608/codes/tfel/master/install-python-3.10.12
~~~~