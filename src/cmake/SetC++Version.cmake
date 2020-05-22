# A macro to set the C++ version
# Based on https://stackoverflow.com/questions/10851247/how-to-activate-c-11-in-cmake

# Copyright (C) 2017, 2020 University College London
# Author Kris Thielemans

# sets minimum C++ version (sorry for the name of the macro)
macro(UseCXX VERSION)
 if (VERSION)
    if (CMAKE_CXX_STANDARD)
       if (${CMAKE_CXX_STANDARD} GREATER ${VERSION})
           if (${CMAKE_CXX_STANDARD} STREQUAL 98 OR ${VERSION} STREQUAL 98)
               message(WARNING "UseCXX macro does not cope well with version 98. Using ${VERSION}")
           else()
               set(VERSION ${CMAKE_CXX_STANDARD})
	       message(STATUS "CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD} was already more recent than version ${VERSION} asked. Keeping the former.")
           endif()
	 endif()
    endif()
    if (CMAKE_VERSION VERSION_LESS "3.1")
    message(WARNING "Your CMake version is older than v3.1. Attempting to set C++ version to ${VERSION} with compiler flags but this might fail. Please upgrade your CMake.")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_EXTENSIONS)
      set (CMAKE_CXX_FLAGS "-std=gnu++${VERSION} ${CMAKE_CXX_FLAGS}")
    else()
      set (CMAKE_CXX_FLAGS "-std=c++${VERSION} ${CMAKE_CXX_FLAGS}")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD ${VERSION})
  endif ()
 endif()
endmacro(UseCXX)
