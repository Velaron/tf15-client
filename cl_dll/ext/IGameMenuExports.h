#pragma once

#ifndef GAME_MENU_INT_H
#define GAME_MENU_INT_H

#include "interface.h"
#include "../mainui/font/FontRenderer.h"

#if defined( _WIN32 )
#define MAINUI_DLLNAME "cl_dlls/menu.dll"
#elif defined( OSX )
#define MAINUI_DLLNAME "cl_dlls/menu.dylib"
#elif defined( __ANDROID__ )
#define MAINUI_DLLNAME "libmenu.so"
#else
#define MAINUI_DLLNAME "cl_dlls/menu.so"
#endif

class IGameMenuExports : public IBaseInterface
{
public:
	virtual bool Initialize( CreateInterfaceFn factory ) = 0;
	virtual HFont BuildFont( CFontBuilder &builder ) = 0;
	virtual const char *L( const char *szStr ) = 0;
	virtual bool IsActive( void ) = 0;
	virtual bool IsMainMenuActive( void ) = 0;
	virtual void Key( int key, int down ) = 0;
	virtual void MouseMove( int x, int y ) = 0;
	virtual void GetCharABCWide( HFont font, int ch, int &a, int &b, int &c ) = 0;
	virtual int GetFontTall( HFont font ) = 0;
	virtual int GetCharacterWidth( HFont font, int ch, int charH ) = 0;
	virtual void GetTextSize( HFont font, const char *text, int *wide, int *height, int size = -1 ) = 0;
	virtual int GetTextHeight( HFont font, const char *text, int size = -1 ) = 0;
	virtual int DrawCharacter( HFont font, int ch, int x, int y, int charH, const unsigned int color, bool forceAdditive = false ) = 0;

	// new methods
	virtual void ShowUpdateDialog( void ) = 0;
};

#define GAMEMENUEXPORTS_INTERFACE_VERSION "GameMenuExports001"

#endif // GAME_MENU_INT_H