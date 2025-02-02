
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# This file is a part of: Stakeforge Engine
# https://github.com/inanevin/StakeforgeEngine
# 
# Author: Inan Evin
# http://www.inanevin.com
# 
# Copyright (c) [2025-] [Inan Evin]
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.10...3.31)
project(StakeforgeSDK)

# -------------- FILES -------------

file (GLOB_RECURSE SOURCES
src/Core/*.cpp
src/Data/*.cpp
src/IO/*.cpp
src/Math/*.cpp
src/Memory/*.cpp
src/Serialization/*.cpp
src/System/*.cpp
src/Type/*.cpp

include/SFG/Vendor/LZ4/lz4.cpp

)

file (GLOB_RECURSE HEADERS
include/SFG/*.hpp
include/SFG/Core/*.hpp
include/SFG/Data/*.hpp
include/SFG/IO/*.hpp
include/SFG/Math/*.hpp
include/SFG/Memory/*.hpp
include/SFG/Platform/*.hpp
include/SFG/Serialization/*.hpp
include/SFG/System/*.hpp
include/SFG/Type/*.hpp

include/SFG/Vendor/LZ4/lz4.h
include/SFG/Vendor/readerwriterqueue/*.h
include/SFG/Vendor/json/*.hpp

)

if(WIN32)

file (GLOB_RECURSE PLATFORM_HEADERS
include/SFG/Platform/Win32/*.hpp
)

file (GLOB_RECURSE PLATFORM_SOURCES
src/Platform/Win32/*.cpp
)

endif()

if(APPLE)

file (GLOB_RECURSE PLATFORM_HEADERS
include/SFG/Platform/OSX/*.hpp
)

file (GLOB_RECURSE PLATFORM_SOURCES
src/Platform/OSX/*.mm
)

endif()


# -------------- LIBRARY -------------

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} ${PLATFORM_HEADERS} ${PLATFORM_SOURCES})
add_library(SFG::SDK ALIAS ${PROJECT_NAME}) 

# include(GenerateExportHeader)
# generate_export_header(${PROJECT_NAME} BASE_NAME SFG EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/StakeforgeAPI.hpp)

include(${CMAKE_SOURCE_DIR}/Utils/CMake/Compile.cmake)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Stakeforge")

# -------------- TARGET -------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)


# -------------- DEPENDENCIES -------------

# Jolt
set(CPP_RTTI_ENABLED OFF CACHE BOOL "" FORCE)
set(CPP_EXCEPTIONS_ENABLED_ENABLED OFF CACHE BOOL "" FORCE)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
set(OVERRIDE_CXX_FLAGS OFF CACHE BOOL "" FORCE)
set(OVERRIDE_MSVCCRT OFF CACHE BOOL "" FORCE)
set(GENERATE_DEBUG_SYMBOLS ON CACHE BOOL "" FORCE)
set(INTERPROCEDURAL_OPTIMIZATION OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
add_subdirectory(Dependencies/Jolt/ConfigBuild)

set_property(TARGET Jolt PROPERTY FOLDER Stakeforge/Dependencies)

target_precompile_headers(${PROJECT_NAME}
PUBLIC
	Dependencies/Jolt/Jolt/Jolt.h
	include/SFG/Stakeforge.hpp
)

if(WIN32)
    target_precompile_headers(${PROJECT_NAME} PUBLIC <Windows.h>)
endif()

target_link_libraries(${PROJECT_NAME} 
	PUBLIC Jolt
)

if(APPLE)
	# target_link_libraries(${PROJECT_NAME}
	# 	PUBLIC "-framework AppKit"
	# 	PUBLIC "-framework Foundation"
	# 	PUBLIC "-framework QuartzCore"
	# 	PUBLIC "-framework UniformTypeIdentifiers"
	# )
endif()

if(WIN32)
# target_link_libraries(${PROJECT_NAME} 
# 	PUBLIC user32.lib
# 	PUBLIC Shcore.lib
# )
endif()

#--------------------------------------------------------------------
# Folder structuring
#--------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/Utils/CMake/CommonFunctions.cmake)
group_sources(HEADERS SOURCES PLATFORM_HEADERS PLATFORM_SOURCES)
group_generated()
