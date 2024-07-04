cmake_minimum_required(VERSION 3.12)
project(tfel-math-enzyme LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(GNUInstallDirs)
find_package(TFELTests REQUIRED HINTS "${TFEL_DIR}/share/tfel/cmake")
find_package(TFELMath REQUIRED HINTS "${TFEL_DIR}/share/tfel/cmake")

add_subdirectory(include)
add_subdirectory(src)
