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

cmake_minimum_required(VERSION 3.10)
project(client)

set(CLDLL_LIBRARY client)

include(CheckIncludeFile)
check_include_file("tgmath.h" HAVE_TGMATH_H)
if(HAVE_TGMATH_H)
	add_definitions(-DHAVE_TGMATH_H)
endif()

add_definitions(-DTF15CLIENT_ADDITIONS)
add_definitions(-DCLIENT_WEAPONS -DCLIENT_DLL)

if(NOT MSVC)
	add_compile_options(-fno-exceptions) # GCC/Clang flag
	add_compile_options(-Wno-write-strings) # GCC/Clang flag
	add_compile_options(-fvisibility=hidden) # GCC/Clang flag
	add_definitions(-D_LINUX -DLINUX) # It seems enough for all non-Win32 systems
	add_definitions(-Dstricmp=strcasecmp -Dstrnicmp=strncasecmp)
	if(NOT MINGW)
		add_definitions(-D_snprintf=snprintf -D_vsnprintf=vsnprintf)
	endif()
else()
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
endif()

if(GOLDSOURCE_SUPPORT)
	add_definitions(-DGOLDSOURCE_SUPPORT)
endif()

if(GOLDSOURCE_DEFAULT_FLAGS)
	add_definitions(-DPOSIX -D_POSIX -DGNUC)
	add_compile_options(-fpermissive -fno-strict-aliasing -fexpensive-optimizations)
	add_compile_options(-Werror=return-type -w -gdwarf-2 -g2 -march=pentium-m -mfpmath=387)
	add_link_options(-gdwarf-2 -g2)
endif()

set(CLDLL_SOURCES
	../wpn_shared/tf_wpn_ac.cpp
	../wpn_shared/tf_wpn_ar.cpp
	../wpn_shared/tf_wpn_axe.cpp
	../wpn_shared/tf_wpn_flame.cpp
	../wpn_shared/tf_wpn_gl.cpp
	../wpn_shared/tf_wpn_ic.cpp
	../wpn_shared/tf_wpn_ng.cpp
	../wpn_shared/tf_wpn_railgun.cpp
	../wpn_shared/tf_wpn_rpg.cpp
	../wpn_shared/tf_wpn_sg.cpp
	../wpn_shared/tf_wpn_srifle.cpp
	../wpn_shared/tf_wpn_tranq.cpp
	ev_tfc.cpp
	tfc/tf_baseentity.cpp
	tfc/tf_events.cpp
	tfc/tf_objects.cpp
	tfc/tf_weapons.cpp
	../public/interface.cpp
	ammo.cpp
	ammo_secondary.cpp
	ammohistory.cpp
	battery.cpp
	cdll_int.cpp
	com_weapons.cpp
	death.cpp
	demo.cpp
	entity.cpp
	ev_common.cpp
	events.cpp
	flashlight.cpp
	GameStudioModelRenderer.cpp
	geiger.cpp
	health.cpp
	hud.cpp
	hud_bench.cpp
	hud_benchtrace.cpp
	hud_msg.cpp
	hud_redraw.cpp
	hud_servers.cpp
	hud_spectator.cpp
	hud_update.cpp
	in_camera.cpp
	input.cpp
	input_goldsource.cpp
	input_mouse.cpp
	input_xash3d.cpp
	interpolation.cpp
	menu.cpp
	message.cpp
	../common/parsemsg.cpp
	../pm_shared/pm_debug.c
	../pm_shared/pm_math.c
	../pm_shared/pm_shared.c
	saytext.cpp
	status_icons.cpp
	statusbar.cpp
	studio_util.cpp
	StudioModelRenderer.cpp
	text_message.cpp
	train.cpp
	tri.cpp
	util.cpp
	view.cpp
	../game_shared/vgui_checkbutton2.cpp
	../game_shared/vgui_grid.cpp
	../game_shared/vgui_helpers.cpp
	../game_shared/vgui_listbox.cpp
	../game_shared/vgui_loadtga.cpp
	../game_shared/vgui_scrollbar2.cpp
	../game_shared/vgui_slider2.cpp
	../game_shared/voice_banmgr.cpp
	vgui_ClassMenu.cpp
	vgui_ConsolePanel.cpp
	vgui_ControlConfigPanel.cpp
	vgui_CustomObjects.cpp
	vgui_int.cpp
	vgui_MOTDWindow.cpp
	vgui_SchemeManager.cpp
	vgui_ScorePanel.cpp
	vgui_ServerBrowser.cpp
	vgui_SpectatorPanel.cpp
	vgui_TeamFortressViewport.cpp
	vgui_teammenu.cpp
	../game_shared/voice_status.cpp)

include_directories(. hl/ ../dlls ../dlls/tfc ../common ../engine ../pm_shared ../game_shared ../public)

if(ANDROID OR CMAKE_SIZEOF_VOID_P EQUAL 8 OR VITA)
    include_directories(../3rdparty/vgui_dll/include)
elseif(NOT BUILD_VGUI)
    SET(CMAKE_SKIP_RPATH TRUE)
    link_directories(${CMAKE_SOURCE_DIR}/3rdparty/vgui_support/vgui-dev/lib)
    include_directories(${CMAKE_SOURCE_DIR}/3rdparty/vgui_support/vgui-dev/include)
endif()

add_library(${CLDLL_LIBRARY} MODULE ${CLDLL_SOURCES})

if(GOLDSOURCE_SUPPORT)
	target_link_libraries(${CLDLL_LIBRARY} ${CMAKE_DL_LIBS})
endif()

if(WIN32)
	target_link_libraries(${CLDLL_LIBRARY} user32.lib winmm.lib ws2_32.lib)
endif()

if(ANDROID OR CMAKE_SIZEOF_VOID_P EQUAL 8 OR VITA)
    target_link_libraries(${CLDLL_LIBRARY} vgui_support)
    if(VITA)
        target_link_libraries(${CLDLL_LIBRARY} -Wl,--whole-archive vgui -Wl,--no-whole-archive)
    else()
        target_link_libraries(${CLDLL_LIBRARY} vgui)
    endif()
elseif(NOT BUILD_VGUI)
    if (WIN32)
        add_library(vgui SHARED IMPORTED)
        set_property(TARGET vgui PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/3rdparty/vgui_support/vgui-dev/lib/win32_vc6/vgui.dll")
        set_property(TARGET vgui PROPERTY IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/3rdparty/vgui_support/vgui-dev/lib/win32_vc6/vgui.lib")
        target_link_libraries(${CLDLL_LIBRARY} vgui)
    elseif(APPLE)
        target_link_libraries(${CLDLL_LIBRARY} "-Wl,--no-undefined -L${CMAKE_SOURCE_DIR}/3rdparty/vgui_support/vgui-dev/lib vgui.dylib")
    else()
        target_link_libraries(${CLDLL_LIBRARY} :vgui.so)
    endif()
endif()

set_target_postfix(${CLDLL_LIBRARY})

install(TARGETS ${CLDLL_LIBRARY}
        DESTINATION "${GAME_DIR}/${CLIENT_INSTALL_DIR}/"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE)

# Install PDB file on Windows
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:${CLDLL_LIBRARY}>
            DESTINATION "${GAME_DIR}/${CLIENT_INSTALL_DIR}/" OPTIONAL)
endif()