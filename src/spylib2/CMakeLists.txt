cmake_minimum_required (VERSION 3.2)
project(spylib2 C CXX)

SET (SOURCES spylib.cpp spylib.h spylib_interfaces.h)

# define project's build target with project's source files
add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../spy)

# add macros
add_definitions(-DSPYLIB_EXPORTS)

SET (INSTALL_DEST ${CMAKE_BINARY_DIR}/bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/${PROJECT_NAME}.dll DESTINATION ${INSTALL_DEST}/debug CONFIGURATIONS Debug)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Release/${PROJECT_NAME}.dll DESTINATION ${INSTALL_DEST}/release CONFIGURATIONS Release)