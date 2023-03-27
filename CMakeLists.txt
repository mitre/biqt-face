# #######################################################################
# NOTICE
#
# This software (or technical data) was produced for the U.S. Government
# under contract, and is subject to the Rights in Data-General Clause
# 52.227-14, Alt. IV (DEC 2007).
#
# Copyright 2019 The MITRE Corporation. All Rights Reserved.
# ####################################################################### 

cmake_minimum_required(VERSION 3.1)
project(BIQTFace)
set(CMAKE_CXX_STANDARD 11)

OPTION(BUILD_SHARED_LIBS "Builds shared libraries for certain dependencies. Recommended: ON" ON)
OPTION(BUILD_STATIC_LIBS "Builds static libraries for certain dependencies. Recommended: OFF" OFF)

if(NOT WIN32)
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  set(CMAKE_CXX_FLAGS "-g -fPIC")
endif()

# BUILD THE FACE LIBRARY FILE #################################################
if(DEFINED ENV{BIQT_HOME})
	if(BIQT_HOME)
		message(WARNING "Both -DBIQT_HOME and a BIQT_HOME environment variable are defined. Using -DBIQT_HOME (${BIQT_HOME}).")
	else()
	  set(BIQT_HOME "$ENV{BIQT_HOME}")
  endif()
endif()

if(NOT BIQT_HOME)
	message(FATAL_ERROR "BIQT_HOME must be defined as an environment variable or passed to CMake as a parameter using -DBIQT_HOME.")
else()
  file(TO_CMAKE_PATH ${BIQT_HOME} BIQT_HOME)
endif()

if(NOT OPENBR_DIR)
	message(FATAL_ERROR "-DOPENBR_DIR must be defined.")
else()
  file(TO_CMAKE_PATH ${OPENBR_DIR} OPENBR_DIR)
endif()

if(CMAKE_INSTALL_PREFIX)
	# Ignore CMAKE_INSTALL_PREFIX, but let the user know about it.
	message(WARNING "Overriding provided CMAKE_INSTALL_PREFIX with ${BIQT_HOME}/providers.")
endif()

set(CMAKE_INSTALL_PREFIX "${BIQT_HOME}/providers")

# BUILD THE FACE LIBRARY FILE #################################################
find_package(OpenCV REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Network)
find_library(OPENBR NAMES openbr libopenbr HINTS ${OPENBR_DIR} ${OPENBR_DIR}/lib)
find_library(JSONCPP NAMES libjsoncpp jsoncpp HINTS ${BIQT_HOME}/lib ${BIQT_HOME}/lib64 ${BIQT_HOME}/bin 
                                                    ${BIQT_HOME}/../../lib ${BIQT_HOME}/../../lib64 ${BIQT_HOME}/../../bin)
find_library(BIQTAPI NAMES libbiqtapi biqtapi HINTS ${BIQT_HOME}/lib ${BIQT_HOME}/lib64 ${BIQT_HOME}/bin 
                                                    ${BIQT_HOME}/../../lib ${BIQT_HOME}/../../lib64 ${BIQT_HOME}/../../bin)
                                                    
if(NOT JSONCPP)
	message(FATAL_ERROR "Failed to find library: jsoncpp.")
endif()

if(NOT BIQTAPI)
	message(FATAL_ERROR "Failed to find library: biqtapi.")
endif()

if(NOT OPENBR)
	message(FATAL_ERROR "Failed to find library: openbr.")
endif()

file(GLOB SOURCE_FILES "src/*.cpp")

include_directories("include" ${OPENBR_DIR}/include ${OpenCV_INCLUDE_DIRS} ${BIQT_HOME}/include ${BIQT_HOME}/../../include ${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})
add_library(BIQTFace SHARED ${SOURCE_FILES})

target_link_libraries(BIQTFace ${JSONCPP} ${OPENBR} ${BIQTAPI} ${OpenCV_LIBS} ${CMAKE_DL_LIBS} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Network_LIBRARIES})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/descriptor.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS   BIQTFace        DESTINATION "${CMAKE_PROJECT_NAME}")
install(FILES     descriptor.json DESTINATION "${CMAKE_PROJECT_NAME}")
install(DIRECTORY config          DESTINATION "${CMAKE_PROJECT_NAME}")

if(WIN32)
  include(${CMAKE_CURRENT_SOURCE_DIR}/CopyWindowsLibraries.cmake)
  if(DEPENDENCY_DLLS)
    install(FILES ${DEPENDENCY_DLLS} DESTINATION "${CMAKE_PROJECT_NAME}")
  endif()
endif()