#
# Copyright (c) 2016 Alibek Omarov
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
#

cmake_minimum_required(VERSION 3.5)
project(tf15-client)

set(CMAKE_CXX_STANDARD 11)

# Install custom module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(EXTRAS_DIR "${CMAKE_CURRENT_BINARY_DIR}/extras")

# Force XP toolchain for Visual Studio
include(VSForceXPToolchain)

#--------------
# USER DEFINES \
################\
option(USE_VOICEMGR "Enable VOICE MANAGER." OFF)

option(BUILD_CLIENT "Build client dll" ON)
option(BUILD_SERVER "Build server dll" OFF)
option(BUILD_MENU "Build menu dll" ON)
option(BUILD_VGUI "Build vgui" ON)
option(BUILD_PMAN "Build particleman" OFF)

option(GOLDSOURCE_SUPPORT "Build goldsource compatible client library" OFF)
option(GOLDSOURCE_DEFAULT_FLAGS "Build with default flags from Valve's Makefile" OFF)

set(GAME_DIR "tfc" CACHE STRING "Gamedir path")
#set(SERVER_INSTALL_DIR "dlls" CACHE STRING "Where put server dll")
set(CLIENT_INSTALL_DIR "cl_dlls" CACHE STRING "Where put client dll")
#set(SERVER_LIBRARY_NAME "tfc" CACHE STRING "Library name for Linux/MacOS/Windows")

#-----------------
# MAIN BUILD CODE \
###################\

# Xash3D FWGS Library Naming Scheme compliance
# see documentation: https://github.com/FWGS/xash3d-fwgs/blob/master/Documentation/library-naming.md
include(LibraryNaming)

if(MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--add-stdcall-alias")
endif()

# add_compile_options for older cmake versions
if(${CMAKE_VERSION} VERSION_LESS "3.0.2")
	macro(add_compile_options)
		set(list_var "${ARGV}")
		foreach(arg IN LISTS list_var)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${arg}")
		endforeach()
	endmacro()
endif()

if(NOT MSVC)
	#add_compile_options(-Wempty-body) # GCC/Clang flag
	add_compile_options(-Wreturn-type) # GCC/Clang flag
else()
	add_definitions(-D_CRT_SILENCE_NONCONFORMING_TGMATH_H -D_CRT_SECURE_NO_WARNINGS)
endif()


if(BUILD_CLIENT)
	add_subdirectory(cl_dll)
endif()

if(BUILD_SERVER)
	add_subdirectory(dlls)
endif()

if(BUILD_MENU)
	add_subdirectory(3rdparty/mainui_cpp)
endif()

if(BUILD_VGUI)
	add_subdirectory(3rdparty/vgui_dll)
	if(ANDROID)
		set(XASH_SDK ${CMAKE_CURRENT_SOURCE_DIR})
		add_subdirectory(3rdparty/vgui_support)
	endif()
endif()

if(BUILD_PMAN)
	add_definitions(-DUSE_PARTICLEMAN)
	add_subdirectory(3rdparty/particleman)
endif()

include(GenerateExtras)

if(WIN32)
	set(CPACK_GENERATOR "ZIP")
	set(CPACK_PACKAGE_FILE_NAME "TF15Client-${CMAKE_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
else()
	set(CPACK_GENERATOR "TGZ")
	set(CPACK_PACKAGE_FILE_NAME "TF15Client-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

include(CPack)