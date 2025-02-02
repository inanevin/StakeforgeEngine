
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

project (Stakeforge)

set(CMAKE_CONFIGURATION_TYPES "Debug;DebugEditor;Release;ReleaseEditor;" CACHE STRING "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SKIP_INSTALL_RULES ON)
set(SFG_GAME_NAME "MyGameProject" CACHE STRING "Name of the game project")

# ------------- COMPILE DEFINITIONS -------------
add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
add_compile_definitions($<$<CONFIG:Debug>:SFG_DEBUG>)

add_compile_definitions($<$<CONFIG:DebugEditor>:_ITERATOR_DEBUG_LEVEL=1>)
add_compile_definitions($<$<CONFIG:DebugEditor>:SFG_DEBUG>)
add_compile_definitions($<$<CONFIG:DebugEditor>:SFG_EDITOR>)
add_compile_definitions($<$<CONFIG:DebugEditor>:_DEBUG>)

if(MSVC)
    add_compile_options($<$<CONFIG:DebugEditor>:/MDd>)
endif()

add_compile_definitions($<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>)

add_compile_definitions($<$<CONFIG:ReleaseEditor>:_ITERATOR_DEBUG_LEVEL=0>)
add_compile_definitions($<$<CONFIG:ReleaseEditor>:SFG_EDITOR>)

if(MSVC)
    add_compile_options($<$<CONFIG:ReleaseEditor>:/MD>)
endif()

add_definitions(-DSFG_CONFIGURATION="$<CONFIGURATION>")
add_definitions(-DSFG_APPNAME="${SFG_GAME_NAME}")

if (WIN32)
    add_compile_definitions(SFG_PLATFORM_WINDOWS=1)
endif()
if(APPLE)
    add_compile_definitions(SFG_PLATFORM_OSX=1)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_definitions(SFG_COMPILER_CLANG=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_definitions(SFG_COMPILER_GNU=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_definitions(SFG_COMPILER_MSVC=1)
else()
    add_compile_definitions(SFG_COMPILER_UNKOWN=1)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SUPPRESS_REGENERATION true)

if(MSVC)
    # Dynamic runtime library
    add_definitions("/MP")

	# Disable RTTI
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")

	add_compile_definitions(_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)
endif()


set(CMAKE_CXX_FLAGS_DEBUGEDITOR "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUGEDITOR "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUGEDITOR "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUGEDITOR "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")


set(CMAKE_CXX_FLAGS_RELEASEEDITOR "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELEASEEDITOR "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASEEDITOR "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASEEDITOR "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")


#--------------------------------------------------------------------
# Sub directories
#--------------------------------------------------------------------

add_subdirectory(StakeforgeSDK)
add_subdirectory(StakeforgeEditor)
add_subdirectory(StakeforgeGame)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CustomTargets")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${SFG_GAME_NAME})

