cmake_minimum_required(VERSION 2.8.0)
project(vgui_support)

set(VGUI_SUPPORT_LIB vgui_support)

file(GLOB_RECURSE VGUI_SUPPORT_SRC "*.cpp")

include_directories(
	"../common"
	"../dlls"
	"../vgui_dll/include"
)

add_definitions(-DINTERNAL_VGUI_SUPPORT)

add_library(${VGUI_SUPPORT_LIB} OBJECT ${VGUI_SUPPORT_SRC})