
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

cmake_minimum_required (VERSION 4.0)

project (Game)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Profile;" CACHE STRING "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ------------- COMPILE DEFINITIONS -------------

add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
add_compile_definitions($<$<CONFIG:Debug>:GAME_DEBUG>)

add_compile_definitions($<$<CONFIG:Profile>:_ITERATOR_DEBUG_LEVEL=0>)
add_compile_definitions($<$<CONFIG:Profile>:GAME_DEBUG>)

add_compile_definitions($<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>)

add_definitions(-DGAME_CONFIGURATION="$<CONFIGURATION>")

if (WIN32)
    add_compile_definitions(GAME_PLATFORM_WINDOWS=1)
endif()
if(APPLE)
    add_compile_definitions(GAME_PLATFORM_OSX=1)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_definitions(GAME_COMPILER_CLANG=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_definitions(GAME_COMPILER_GNU=1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_definitions(GAME_COMPILER_MSVC=1)
else()
    add_compile_definitions(GAME_COMPILER_UNKOWN=1)
endif()


# ------------- LANGUAGE -------------

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++23")
set(CMAKE_SUPPRESS_REGENERATION true)

if(MSVC)
    add_definitions("/MP")
    add_compile_options(/GR-)
	add_compile_definitions(_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)
endif()

# ------------- SRC -------------

file(GLOB SOURCES 
src/common/*.cpp
src/platform/*.cpp
src/math/*.cpp
src/io/*.cpp
src/data/*.cpp
src/memory/*.cpp
src/serialization/*.cpp
src/input/*.cpp
src/gfx/*.cpp
src/gfx/backend/*.cpp
src/gfx/common/*.cpp
src/resources/*.cpp
src/app/*.cpp
)

file(GLOB HEADERS 
src/common/*.hpp
src/platform/*.hpp
src/math/*.hpp
src/io/*.hpp
src/data/*.hpp
src/memory/*.hpp
src/input/*.hpp
src/gfx/*.hpp
src/gfx/backend/*.hpp
src/gfx/common/*.hpp
src/gui/*.hpp
src/app/*.hpp
src/resources/*.hpp
src/vendor/stb/*.h
src/vendor/moodycamel/*.h
src/vendor/nhlohmann/*.hpp
)

if(WIN32)
file(GLOB_RECURSE SHADERS
assets/engine/shaders/*.hlsl
)

file(GLOB PLATFORM_SOURCES 
src/platform/win32/*.cpp
src/gfx/backend/dx12/*.cpp
src/gfx/backend/dx12/sdk/*.cpp
src/gfx/backend/dx12/sdk/*.c
src/gfx/backend/dx12/sdk/Residency/*.cpp
src/gfx/backend/dx12/sdk/Residency/*.c
)

file(GLOB PLATFORM_HEADERS
src/gfx/backend/dx12/*.hpp
src/gfx/backend/dx12/sdk/*.hpp
src/gfx/backend/dx12/sdk/*.h
src/gfx/backend/dx12/sdk/Residency/*.hpp
src/gfx/backend/dx12/sdk/Residency/*.h
)
set(ICON ${PROJECT_SOURCE_DIR})

endif()

if(APPLE)

set(ICON ${PROJECT_SOURCE_DIR})

endif()

# ------------- TARGET -------------

if(WIN32)

# prevent FXC from compiling hlsl sources automatically.
set_source_files_properties(${SHADERS} PROPERTIES HEADER_FILE_ONLY ON)

	add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${SHADERS} ${PLATFORM_HEADERS} ${PLATFORM_SOURCES} ${ICON})
else()
	add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${PLATFORM_HEADERS} ${PLATFORM_SOURCES} ${ICON})
endif()
include(${CMAKE_SOURCE_DIR}/utils/cmake/executable_settings.cmake)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/deps/phmap/include)

# ------------- DEPS -------------
add_subdirectory(deps/lz4-1.9.4)

if(WIN32)

include(deps/dxc/link.cmake)
link_dxc()

target_link_libraries(${PROJECT_NAME} 
			PRIVATE d3d12.lib
			PRIVATE dxgi.lib
			PRIVATE dxguid.lib
		)

endif()

target_link_libraries(${PROJECT_NAME} 
PUBLIC lz4
)

set_target_properties(lz4 PROPERTIES FOLDER "Dependencies")

add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/assets")

if(WIN32)
add_custom_command(
TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/deps/dx12_bin" "${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/")
endif()

include(${CMAKE_SOURCE_DIR}/utils/cmake/common_functions.cmake)
group_sources(HEADERS SOURCES SHADERS PLATFORM_HEADERS PLATFORM_SOURCES)
group_generated()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CustomTargets")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

