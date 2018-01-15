#################################################
# CMake file for building sensor calibration tools
# @author Benjamin Boitrelle
# Copyright (c) DESY
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 FATAL_ERROR )
#################################################

# project name
PROJECT(analyseChipscopeData CXX)

# Setup the build environment
LIST ( APPEND CMAKE_CXX_FLAGS "-fPIC -O2 -std=c++14 -Wall -Werror -pedantic -g" )
LIST ( APPEND CMAKE_LD_FLAGS "-fPIC -O2" )

# Additional packages to be searched for by cmake
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

SET(INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" CACHE PATH "Prefix prepended to install directories")
SET(CMAKE_INSTALL_PREFIX "${INSTALL_PREFIX}" CACHE INTERNAL "Prefix prepended to install directories" FORCE)

SET(src/analyseChipscopeData.cpp)

# Depends on ROOT for plotting:
FIND_PACKAGE(ROOT REQUIRED)

# Include root package properties
INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )
LINK_LIBRARIES( ${ROOT_LIBRARIES} ${ROOT_ROOFIT_LIBRARY} )
ADD_DEFINITIONS ( ${ROOT_DEFINITIONS} )

FILE(
  GLOB_RECURSE SOURCE_FILES
  src/*
)

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR} )

ADD_EXECUTABLE(analyseChipscopeData src/analyseChipscopeData.cpp ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ROOT_LIBRARY})
