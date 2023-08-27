cmake_minimum_required(VERSION 3.10)

project(PianoInstrument LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(PkgConfig REQUIRED)

set(RONIN_NO_CONFIG ON)
add_subdirectory(RoninEngine)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(PianoInstrument ${HEADERS} ${SOURCES})

target_link_libraries(PianoInstrument PUBLIC RoninEngine)

# set the executable name
set_target_properties(PianoInstrument PROPERTIES OUTPUT_NAME
                                                 epic-piano-instrument)

# create link resources "data:resources"
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(_DATA_DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data")
if(NOT EXISTS "${_DATA_DESTINATION}")
  message(STATUS "Linking data to \"${_DATA_DESTINATION}\"")
  file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/resources/data"
       "${_DATA_DESTINATION}" SYMBOLIC)
else()
  message(STATUS "Symbolic link exists - not required")
endif()
unset(_DATA_DESTINATION)