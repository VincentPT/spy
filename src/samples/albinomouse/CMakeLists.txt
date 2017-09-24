cmake_minimum_required (VERSION 3.2)
project(albino-mouse)

SET (MAIN_SOURCES
    main.cpp
)

SET (SOURCES ${MAIN_SOURCES})

# define project's build target with project's source files
add_executable(${PROJECT_NAME} ${SOURCES})

SET (INSTALL_DEST ${CMAKE_BINARY_DIR}/bin/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/${PROJECT_NAME}.exe DESTINATION ${INSTALL_DEST}/debug CONFIGURATIONS Debug)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Release/${PROJECT_NAME}.exe DESTINATION ${INSTALL_DEST}/release CONFIGURATIONS Release)