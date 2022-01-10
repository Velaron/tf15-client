#
# Copyright (c) 2015 Pavlo Lavrenenko
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

cmake_minimum_required(VERSION 2.8.12)
project(server)

set(SVDLL_LIBRARY tfc)

include(CheckIncludeFile)
check_include_file("tgmath.h" HAVE_TGMATH_H)
if(HAVE_TGMATH_H)
	add_definitions(-DHAVE_TGMATH_H)
endif()

add_definitions(-DCLIENT_WEAPONS)

if(NOT MSVC)
	add_compile_options(-fno-exceptions) # GCC/Clang flag
	add_compile_options(-Wno-invalid-offsetof) # GCC/Clang flag
	add_compile_options(-fvisibility=hidden) # GCC/Clang flag
	add_definitions(-D_LINUX) # It seems enough for all non-Win32 systems
	add_definitions(-Dstricmp=strcasecmp -Dstrnicmp=strncasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf )
else()
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
endif()

set(SVDLL_SOURCES
	areadef.cpp
	airtank.cpp
	animating.cpp
	animation.cpp
	bmodels.cpp
	buttons.cpp
	cbase.cpp
	client.cpp
	combat.cpp
	crowbar.cpp
#	dbghelpers.cpp
	defaultai.cpp
	demoman.cpp
	dispenser.cpp
	doors.cpp
	effects.cpp
	engineer.cpp
	explode.cpp
	func_break.cpp
	func_tank.cpp
	game.cpp
	gamerules.cpp
#	genericmonster.cpp
	ggrenade.cpp
	globals.cpp
	h_ai.cpp
	h_battery.cpp
	h_cine.cpp
	h_cycler.cpp
	h_export.cpp
	handgrenade.cpp
	healthkit.cpp
	items.cpp
	lights.cpp
#	menu.cpp
	tfortmap.cpp
	monsters.cpp
	monsterstate.cpp
	mortar.cpp
#	mpstubb.cpp
	multiplay_gamerules.cpp
	nodes.cpp
	observer.cpp
	pathcorner.cpp
	plane.cpp
	plats.cpp
	player.cpp
	playermonster.cpp
#	prop.cpp
#	rpg.cpp
	pyro.cpp
	schedule.cpp
	scripted.cpp
	sentry.cpp
	skill.cpp
	sound.cpp
	soundent.cpp
	spectator.cpp
	spy.cpp
	squadmonster.cpp
	subs.cpp
	talkmonster.cpp
	teamplay_gamerules.cpp
	tempmonster.cpp
	tf_admin.cpp
	tf_clan.cpp
	tf_gamerules.cpp
	tf_item.cpp
	tf_sbar.cpp
	tf_wpn_grenades.cpp
	tf_wpn_nails.cpp
	tfort.cpp
	tforttm.cpp
	triggers.cpp
	turret.cpp
	util.cpp
	weapons.cpp
	world.cpp
	xen.cpp
	teleporter.cpp
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
	../pm_shared/pm_debug.c
	../pm_shared/pm_math.c
	../pm_shared/pm_shared.c
)

include_directories (. wpn_shared ../cl_dll ../common ../engine ../pm_shared ../game_shared ../public tfc)

if(USE_VOICEMGR)
	set(SVDLL_SOURCES
		${SVDLL_SOURCES}
		../game_shared/voice_gamemgr.cpp)
else()
	add_definitions(-DNO_VOICEGAMEMGR)
endif()

if(MSVC)
	set(SVDLL_SOURCES
		${SVDLL_SOURCES}
		tfc.def)
endif()

add_library(${SVDLL_LIBRARY} MODULE ${SVDLL_SOURCES})

set_target_properties(${SVDLL_LIBRARY} PROPERTIES
	POSITION_INDEPENDENT_CODE 1)

if(APPLE OR WIN32 OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(SVDLL_NAME "${SERVER_LIBRARY_NAME}")
	set_target_properties(${SVDLL_LIBRARY} PROPERTIES
		OUTPUT_NAME ${SVDLL_NAME}
		PREFIX "")
endif()

install( TARGETS ${SVDLL_LIBRARY}
	DESTINATION "${GAMEDIR}/${SERVER_INSTALL_DIR}/"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
	    GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE)

# Install PDB file on Windows
if(WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:${SVDLL_LIBRARY}>
		DESTINATION "${GAMEDIR}/${SERVER_INSTALL_DIR}/" OPTIONAL)
endif()