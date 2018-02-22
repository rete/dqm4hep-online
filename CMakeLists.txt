#################################################
# cmake file for building DQMCore package
# @author Eté Rémi
# Copyright (c) CNRS / IPNL
cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
#################################################

###############################
# project name
project( DQMOnline )

set( ${PROJECT_NAME}_VERSION_MAJOR 1 )
set( ${PROJECT_NAME}_VERSION_MINOR 2 )
SET( ${PROJECT_NAME}_VERSION_PATCH 3 )

###############################
# Require C++11
include( CheckCXXCompilerFlag )

check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)

if( COMPILER_SUPPORTS_CXX11 )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	message( SEND_ERROR "DQMOnline requires C++11 support. Please upgrade your compiler !" )
endif()

###############################
# Dependencies
list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
include( dqm4hep_settings )

###############################
# Define options
option( INSTALL_DOC    "Set to OFF to skip build/install Documentation" OFF )
option( BUILD_HTTP_RC  "Set to OFF to skip build/install http run control plugin" OFF )

# ----- Find ROOT package -----
list( APPEND ROOT_COMPONENT_LIST Core Hist Rint HistPainter Graf Graf3d MathCore Net RIO Tree )
find_package( ROOT COMPONENTS ${ROOT_COMPONENT_LIST} REQUIRED )

# ----- Find DQMCore package -----
find_package( DQMCore REQUIRED )

# ----- Find DQMNet package -----
find_package( DQMNet REQUIRED )

# ----- Include package properties -----
foreach( pkg ROOT DQMCore DQMNet )

  include_directories( ${${pkg}_INCLUDE_DIRS} )
  link_libraries( ${${pkg}_LIBRARIES} )
  add_definitions ( ${${pkg}_DEFINITIONS} )

  list( APPEND ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS ${${pkg}_INCLUDE_DIRS} )
  list( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS ${${pkg}_LIBRARY_DIRS} )
  list( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARIES ${${pkg}_LIBRARIES} )

endforeach()

# ----- Install doc -----
if( INSTALL_DOC )
  find_package( Doxygen )

  if( DOXYGEN_FOUND )
    add_subdirectory( ./doc )
  else()
    message( SEND_ERROR "Could not find doxygen required to build documentation" )
    message( "Please install doxygen or set INSTALL_DOC to OFF" )
    message( "" )
  endif()
endif()

###############################
# Generate DQMOnlineConfig.h
configure_file( "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.h.cmake.in"
    "${PROJECT_SOURCE_DIR}/source/include/${PROJECT_NAME}Config.h" @ONLY )

###############################
# Add source directory
add_subdirectory( ./source )

# INSTALL ( DIRECTORY web DESTINATION .  )

###############################
# display some variables and write them to cache
display_std_variables()

###############################
# generate and install following configuration files
generate_package_configuration_files( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )
