# Try to find AntTweakBar library and include path.
#
# Once done this will define:
# ANT_TWEAK_BAR_FOUND
# ANT_TWEAK_BAR_INCLUDE_PATH
# ANT_TWEAK_BAR_LIBRARIES

IF (WIN32)

	FIND_PATH( ANT_TWEAK_BAR_INCLUDE_PATH AntTweakBar.h
		PATHS
		"${PROJECT_SOURCE_DIR}/externals/AntTweakBar/include")
		
	IF(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)")
		find_library(ANT_TWEAK_BAR_LIBRARIES
			NAMES AntTweakBar
			HINTS "${PROJECT_SOURCE_DIR}/externals/AntTweakBar/lib")
	ELSE()
		find_library(ANT_TWEAK_BAR_LIBRARIES
			NAMES AntTweakBar64
			HINTS "${PROJECT_SOURCE_DIR}/externals/AntTweakBar/lib")
	ENDIF()
		
ELSE (WIN32)

	FIND_PATH(ANT_TWEAK_BAR_INCLUDE_PATH AntTweakBar.h
		PATHS
		"/usr/local/include"
		"/usr/X11/include"
		"/usr/include")

	FIND_LIBRARY(ANT_TWEAK_BAR_LIBRARIES AntTweakBar
		PATHS
		"/usr/local"
		"/usr/X11"
		"/usr"
		PATH_SUFFIXES
		"lib64"
		"lib"
		"dylib")
		
ENDIF (WIN32)

SET(ANT_TWEAK_BAR_FOUND "NO")
IF (ANT_TWEAK_BAR_INCLUDE_PATH AND ANT_TWEAK_BAR_LIBRARIES)
	SET(ANT_TWEAK_BAR_FOUND "YES")
ENDIF (ANT_TWEAK_BAR_INCLUDE_PATH AND ANT_TWEAK_BAR_LIBRARIES)
