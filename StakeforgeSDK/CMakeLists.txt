
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

file (GLOB SOURCES
src/Core/*.cpp
src/Data/*.cpp
src/IO/*.cpp
src/Math/*.cpp
src/Memory/*.cpp
src/Serialization/*.cpp
src/System/*.cpp
src/Resources/*.cpp
src/Type/*.cpp
src/Gfx/*.cpp

include/SFG/Vendor/LZ4/lz4.cpp

)

file (GLOB HEADERS
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
include/SFG/Resources/*.hpp
include/SFG/Gfx/*.hpp
include/SFG/Gfx/Common/*.hpp
include/SFG/Gfx/Commands/*.hpp

include/SFG/Vendor/LZ4/lz4.h
include/SFG/Vendor/readerwriterqueue/*.h
include/SFG/Vendor/json/*.hpp
include/SFG/Vendor/tinygltf/*.h
include/SFG/Vendor/stb/*.h
include/SFG/Vendor/parallel_hashmap/*.h

)

if(WIN32)

file (GLOB PLATFORM_HEADERS
include/SFG/Platform/Win32/*.hpp
include/SFG/Vendor/vulkan/*.h
include/SFG/Gfx/Backend/Vulkan/*.hpp
)

file (GLOB PLATFORM_SOURCES
src/Platform/Win32/*.cpp
src/Gfx/Backend/Vulkan/*.cpp
include/SFG/Vendor/vulkan/*.cpp
)

endif()

if(APPLE)

file (GLOB PLATFORM_HEADERS
include/SFG/Platform/OSX/*.hpp
include/SFG/Gfx/Backend/Metal/*.hpp

)

file (GLOB PLATFORM_SOURCES
src/Platform/OSX/*.mm
src/Gfx/Backend/Metal/*.mm
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

if(APPLE)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Dependencies/slang/OSX/include/)
endif()

# -------------- DEPENDENCIES -------------

if(WIN32)
	find_package(Vulkan REQUIRED)
	target_link_libraries(${PROJECT_NAME} 
		PRIVATE Vulkan::Vulkan
	)
endif()

if(APPLE)
	target_link_libraries(${PROJECT_NAME}
		PUBLIC "-framework Metal"
		# PUBLIC "-framework AppKit"
		PUBLIC "-framework Foundation"
		PUBLIC "-framework QuartzCore"
		# PUBLIC "-framework Carbon"
	)
endif()

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

# Slang
# set(SLANG_ENABLE_DXIL OFF CACHE BOOL "" FORCE)
# set(SLANG_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
# set(SLANG_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
# set(SLANG_ENABLE_SLANG_RHI OFF CACHE BOOL "" FORCE)
# add_subdirectory(Dependencies/slang)

target_precompile_headers(${PROJECT_NAME}
PUBLIC
	include/SFG/Stakeforge.hpp
)

target_link_libraries(${PROJECT_NAME} 
	PUBLIC Jolt
)

if(APPLE)

set(SLANG_LIB_DIR ${PROJECT_SOURCE_DIR}/Dependencies/slang/OSX/lib/)
find_library(SLANG_LIB slang PATHS ${SLANG_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SLANG_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC "-framework AppKit")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SLANG_LIB_DIR}/libslang.dylib
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/
)
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
