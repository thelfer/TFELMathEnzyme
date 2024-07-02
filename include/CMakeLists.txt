set(TFEL_MATH_ENZYME_HEADERS
    TFEL/Math/Enzyme/diff.hxx
    TFEL/Math/Enzyme/diff.ixx)

install(FILES ${TFEL_MATH_ENZYME_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TFEL/Math/Enzyme)
