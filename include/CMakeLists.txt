set(TFEL_MATH_ENZYME_HEADERS
    TFEL/Math/Enzyme/diff.hxx
    TFEL/Math/Enzyme/diff.ixx
    TFEL/Math/Enzyme/Variable.hxx
    TFEL/Math/Enzyme/Internals/Enzyme.hxx
    TFEL/Math/Enzyme/Internals/IsTemporary.hxx)

foreach(file ${TFEL_MATH_ENZYME_HEADERS})
  get_filename_component(dir ${file} DIRECTORY)
  install(FILES ${file} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${dir} )
endforeach()
