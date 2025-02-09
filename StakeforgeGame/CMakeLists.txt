
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
project(${SFG_GAME_NAME})

# -------------- FILES -------------

file (GLOB_RECURSE SOURCES
src/Game.cpp
)

file (GLOB_RECURSE HEADERS
include/Game.hpp
)

if(WIN32)
	file (GLOB_RECURSE PLATFORM_SOURCES
	src/Platform/Win32/*.cpp
	)

	file (GLOB_RECURSE PLATFORM_HEADERS
	include/Platform/Win32/*.hpp
	)
endif()

if(APPLE)
	file (GLOB_RECURSE PLATFORM_SOURCES
	src/Platform/OSX/*.mm
	)

	file (GLOB_RECURSE PLATFORM_HEADERS
	include/Platform/OSX/*.hpp
	)
endif()


# -------------- EXE -------------

if(WIN32)
set(ICON ${PROJECT_SOURCE_DIR}/_Resources/Game.rc)
endif()

if(APPLE)
set(ICON ${PROJECT_SOURCE_DIR}/_Resources/Game.icns)
endif()

if(WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${PLATFORM_HEADERS} ${PLATFORM_SOURCES} ${ICON})
else()
	add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${PLATFORM_HEADERS} ${PLATFORM_SOURCES} ${ICON})
endif()


# -------------- SETTINGS -------------

include(${CMAKE_SOURCE_DIR}/Utils/CMake/ExecutableSettings.cmake)

# -------------- TARGET -------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} 
PUBLIC StakeforgeSDK
$<$<CONFIG:DebugEditor>:StakeforgeEditor>
$<$<CONFIG:ReleaseEditor>:StakeforgeEditor>
)


# -------------- PROJECT STRUCTURE -------------

include(${CMAKE_SOURCE_DIR}/Utils/CMake/CommonFunctions.cmake)
group_sources(HEADERS SOURCES PLATFORM_HEADERS PLATFORM_SOURCES)
group_generated()