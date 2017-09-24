cmake_minimum_required (VERSION 3.2)
project(predefined-functions-1)

SET (MAIN_SOURCES
    main.cpp
    UserSpyClient.h
    UserSpyClient.cpp
)

SET (SPY_FRAMEWORKS_SOURCE
    ../../spy-client/SpyClient.cpp
    ../../spy-client/SpyClient.h
    ../../spy-client/SpyClientUtils.hpp
)

SET (SOURCES ${MAIN_SOURCES} ${SPY_FRAMEWORKS_SOURCE})

# [Visual studio specific]group the source into solution folder
source_group(spy-client FILES ${SPY_FRAMEWORKS_SOURCE})

# define project's build target with project's source files
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../spy-client)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../spylib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../spy)

add_dependencies(spylib spy-engine)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/spy/$<CONFIGURATION>/spy-engine.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/spylib/$<CONFIGURATION>/spylib.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/)

SET (INSTALL_DEST ${CMAKE_BINARY_DIR}/bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/${PROJECT_NAME}.exe DESTINATION ${INSTALL_DEST}/debug CONFIGURATIONS Debug)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Release/${PROJECT_NAME}.exe DESTINATION ${INSTALL_DEST}/release CONFIGURATIONS Release)