cmake_minimum_required(VERSION 2.8.3)

# Setup
project(VolumeRayCasting)
set(APPNAME Voraca)

# Make release build
set(CMAKE_BUILD_TYPE Release)

# Module path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake CACHE PATH "Project specific path. Set manually if it was not found.")

# Used to find data within c++ code
set(DATA_PATH ${PROJECT_SOURCE_DIR}/data CACHE PATH "Project specific path. Set manually if it was not found.")
add_definitions(-DDATA_PATH="${DATA_PATH}")

# Directory of implementation
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

# Directory of external code
set(EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/externals")

# Including
include_directories("${SRC_DIR}")
include_directories("${EXTERNAL_DIR}")

# Collect source files
file(GLOB_RECURSE SOURCES
	"src/*.cpp")

file(GLOB_RECURSE HEADERS
	"src/*.h")

# Add GLM
file(GLOB_RECURSE GLM
	"${EXTERNAL_DIR}/GLM/glm/*.hpp")
include_directories("${EXTERNAL_DIR}/GLM")

# Add OpenGLLoader
file(GLOB_RECURSE OPEN_GL_LOADER
	"${EXTERNAL_DIR}/OpenGLLoader/*")

# Add RapidXML
file(GLOB_RECURSE RAPID_XML
	"${EXTERNAL_DIR}/RapidXML/*.hpp")

# Add PicoPNG
file(GLOB_RECURSE PICO_PNG
	"${EXTERNAL_DIR}/PicoPNG/*")

# Collect all code
set(ALL_CODE
	${SOURCES}
	${HEADERS}
	${GLM}
	${OPEN_GL_LOADER}
	${RAPID_XML}
	${PICO_PNG})

# Filtering for Visual Studio
if(MSVC)

	# http://stackoverflow.com/questions/9701387/cmake-source-group-multiple-files
	foreach(f ${ALL_CODE})
		# Get the path of the file relative to ${PROJECT_SOURCE_DIR},
		# then alter it (not compulsory)
		file(RELATIVE_PATH SRCGR "${CMAKE_CURRENT_LIST_DIR}" ${f})
		set(SRCGR "${LIBNAME}/${SRCGR}")

		# Extract the folder, ie remove the filename part
		string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

		# Source_group expects \\ (double antislash), not / (slash)
		string(REPLACE / \\ SRCGR ${SRCGR})
		source_group("${SRCGR}" FILES ${f})
	endforeach()

endif(MSVC)

### WINDOWS ###
IF(WIN32)

	# GLFW 3
	set(GLFW3_DIRECTORY "${EXTERNAL_DIR}/GLFW")
	include_directories("${GLFW3_DIRECTORY}/include")

	IF(MSVC10)
		set(GLFW3_LIBRARIES_DIRECTORY "${GLFW3_DIRECTORY}/lib-vc2012")
	ELSEIF(MSVC11)
		set(GLFW3_LIBRARIES_DIRECTORY "${GLFW3_DIRECTORY}/lib-vc2013")
	ELSE()
		set(GLFW3_LIBRARIES_DIRECTORY "${GLFW3_DIRECTORY}/lib-vc2015")
	ENDIF()

	find_library(GLFW3_LIBRARIES
		NAMES glfw glfw3
		HINTS "${GLFW3_LIBRARIES_DIRECTORY}")

### LINUX ###
ELSE(WIN32)

	# GLFW 3
	find_package(PkgConfig REQUIRED)
	pkg_search_module(GLFW3 REQUIRED glfw3)
	include_directories(${GLFW3_INCLUDE_DIR})

ENDIF(WIN32)

### GENERAL ###

# AntTweakBar
find_package(AntTweakBar REQUIRED)
include_directories(${ANT_TWEAK_BAR_INCLUDE_PATH})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# Creation of executeable
add_executable(${APPNAME} ${ALL_CODE})

# Linking
target_link_libraries(${APPNAME} ${OPENGL_LIBRARIES} ${GLFW3_LIBRARIES} ${ANT_TWEAK_BAR_LIBRARIES})

# Dynamic linked libraries
IF(WIN32)

	# TODO: should be somehow done or connected to cmake module for ant tweak bar
	# Copy dlls
	IF(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)")
	add_custom_command(TARGET ${APPNAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${PROJECT_SOURCE_DIR}/externals/AntTweakBar/lib/AntTweakBar.dll"
		"${PROJECT_BINARY_DIR}/AntTweakBar.dll")
	ELSE()
		add_custom_command(TARGET ${APPNAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${PROJECT_SOURCE_DIR}/externals/AntTweakBar/lib/AntTweakBar64.dll"
			"${PROJECT_BINARY_DIR}/AntTweakBar64.dll")
	ENDIF()

ENDIF(WIN32)