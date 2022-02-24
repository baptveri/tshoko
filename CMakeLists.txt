PROJECT (file_saver)

cmake_minimum_required(VERSION 3.19)

SET(CMAKE_CXX_STANDARD 17)

MESSAGE(STATUS "Configurationtype: ${CMAKE_CONFIGURATION_TYPES}")
SET(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "Configuration types" FORCE)

# Set Variables
SET(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
SET(LIB_DIR ${CMAKE_SOURCE_DIR}/3rdparty)

INCLUDE_DIRECTORIES(${SRC_DIR})

ADD_LIBRARY(file_saver STATIC 
            src/modules/file_saver/file_saver.c
            src/common/cmath.c
            src/utils/wav.c)

ADD_SUBDIRECTORY(test)