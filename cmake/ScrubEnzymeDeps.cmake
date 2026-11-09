# ~~~
# The Enzyme clang plugin provides the `enzyme` C++ headers (which define
# `enzyme::Const`, `enzyme::Duplicated`, etc.) from an in-memory virtual
# file system mounted at `/enzymeroot`. The compiler is happy with this,
# but it records the mounted paths in the dependency files (`-MD`) of every
# translation unit including `<enzyme/enzyme>`. As these files never exist
# on disk, the build system considers the objects always out of date and
# recompiles everything on every single run.
#
# This script replaces the phantom dependencies by `/dev/null`, which
# exists and is older than anything: targets are then only recompiled when
# something real changed. The compilation itself does not depend on these
# files on disk since the plugin serves them from memory.
#
# Usage: cmake -D DIR=<target object directory> -P ScrubEnzymeDeps.cmake
# ~~~

if(NOT DEFINED DIR)
  message(FATAL_ERROR "the DIR variable is required")
endif()

file(GLOB _enzyme_dep_files "${DIR}/*.o.d" "${DIR}/compiler_depend.make")

foreach(_enzyme_dep_file IN LISTS _enzyme_dep_files)
  file(READ "${_enzyme_dep_file}" _enzyme_dep_file_content)
  string(REGEX REPLACE "/enzymeroot/[A-Za-z0-9/._-]*" "/dev/null"
                       _enzyme_new_content "${_enzyme_dep_file_content}")
  if(NOT _enzyme_new_content STREQUAL _enzyme_dep_file_content)
    file(WRITE "${_enzyme_dep_file}" "${_enzyme_new_content}")
  endif()
endforeach()
