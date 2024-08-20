cmake_minimum_required(VERSION 3.12)

find_package(Enzyme REQUIRED CONFIG)

message("LLVM Version: " ${Enzyme_LLVM_VERSION_MAJOR})
message("Found LLVM at: " ${Enzyme_LLVM_BINARY_DIR})

# C and C++ Compilers can only be configured before project(...)
set(CMAKE_C_COMPILER "${Enzyme_LLVM_BINARY_DIR}/bin/clang")
set(CMAKE_CXX_COMPILER "${Enzyme_LLVM_BINARY_DIR}/bin/clang++")

project(tfel-math-enzyme LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(GNUInstallDirs)
find_package(TFELTests REQUIRED HINTS "${TFEL_DIR}/share/tfel/cmake")
find_package(TFELMath REQUIRED HINTS "${TFEL_DIR}/share/tfel/cmake")

add_subdirectory(include)
add_subdirectory(src)
