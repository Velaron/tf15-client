//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef __VGUI_LOADTGA_H__
#define __VGUI_LOADTGA_H__

#include <VGUI_BitmapTGA.h>

vgui::BitmapTGA *vgui_LoadTGA( char const *pFilename );
vgui::BitmapTGA *vgui_LoadTGANoInvertAlpha( char const *pFilename );

#endif // __VGUI_LOADTGA_H__
