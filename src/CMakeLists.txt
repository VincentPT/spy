cmake_minimum_required (VERSION 3.2)
project(spy)

# spy engine
add_subdirectory(spy)

# user spies lib
add_subdirectory(spylib)
add_subdirectory(spylib2)
add_subdirectory(spylib3)
add_subdirectory(user-spy-lib)

# samples
add_subdirectory(samples/predefined-functions-1)
add_subdirectory(samples/predefined-functions-2)
add_subdirectory(samples/predefined-functions-3)
add_subdirectory(samples/dynamic-functions)

# the app used to inject the dll
add_subdirectory(samples/albinomouse)