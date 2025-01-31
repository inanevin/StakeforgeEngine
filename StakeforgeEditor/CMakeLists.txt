
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
project(StakeforgeEditor)

# -------------- FILES -------------

file (GLOB_RECURSE SOURCES
src/*.cpp
)

file (GLOB_RECURSE HEADERS
include/*.hpp
)

# -------------- LIBRARY -------------

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
add_library(SFG::Editor ALIAS ${PROJECT_NAME}) 

include(../CMake/Compile.cmake)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Stakeforge")

# include(GenerateExportHeader)
# generate_export_header(${PROJECT_NAME} BASE_NAME SFGEDITOR EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/StakeforgeEditorAPI.hpp)

# -------------- SETTINGS -------------

include(${CMAKE_SOURCE_DIR}/CMake/ExecutableSettings.cmake)

# -------------- TARGET -------------

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} 
PUBLIC StakeforgeSDK
)

# -------------- PROJECT STRUCTURE -------------

include(${CMAKE_SOURCE_DIR}/CMake/CommonFunctions.cmake)
group_sources(HEADERS SOURCES)
group_generated()