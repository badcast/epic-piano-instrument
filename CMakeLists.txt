cmake_minimum_required(VERSION 3.10)

project(PianoInstrument LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

option(USE_SYSRONIN "Use system installed ronin engine" ON)

find_package(PkgConfig REQUIRED)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

if(WIN32)
  add_executable(PianoInstrument WIN32 ${HEADERS} ${SOURCES})
else()
  add_executable(PianoInstrument ${HEADERS} ${SOURCES})
endif()

if(NOT USE_SYSRONIN)
  set(RONIN_NO_CONFIG ON)
  add_subdirectory(RoninEngine)
  target_link_libraries(PianoInstrument PRIVATE RoninEngine)
else()
  pkg_check_modules(RoninEngine REQUIRED RoninEngine)
  target_include_directories(PianoInstrument
                             PRIVATE ${RoninEngine_INCLUDE_DIRS})
  target_link_libraries(PianoInstrument PRIVATE ${RoninEngine_LIBRARIES})
endif()

# set the executable name
set_target_properties(PianoInstrument PROPERTIES OUTPUT_NAME
                                                 epic-piano-instrument)

get_target_property(OUTPUT_NAME PianoInstrument OUTPUT_NAME)

# INSTALLS

include(GNUInstallDirs)

if(CMAKE_BUILD_TYPE STREQUAL "Release"
   OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"
   OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

  set(EPIC_PIANO_DATA_DIR ${CMAKE_INSTALL_PREFIX}/share/${OUTPUT_NAME})

  target_compile_definitions(PianoInstrument
                             PUBLIC DATA_NOTES_DIR="${EPIC_PIANO_DATA_DIR}")

  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/data/
          DESTINATION ${EPIC_PIANO_DATA_DIR})

  install(FILES "${CMAKE_BINARY_DIR}/bin/${OUTPUT_NAME}"
          DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
else()
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
endif()
