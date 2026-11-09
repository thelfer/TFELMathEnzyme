# \note The 'OPTIONAL' argument is required as the documentation is only
#   generated when the 'doxygen' target is built (which is not the case when
#   BUILD_DOC is OFF)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
  DESTINATION share/doc/tfel-math-enzyme
  COMPONENT doxygen
  OPTIONAL)
install(FILES
  ${PROJECT_SOURCE_DIR}/docs/doxygen/img/background.svg
  DESTINATION share/doc/tfel-math-enzyme/html/img/
  COMPONENT doxygen
  OPTIONAL)
