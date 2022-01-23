//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef __VGUI_INT_H__
#define __VGUI_INT_H__

extern "C"
{
	void VGui_Startup();
	void VGui_Shutdown();

	//Only safe to call from inside subclass of Panel::paintBackground
	void VGui_ViewportPaintBackground( int extents[4] );
}

#endif // __VGUI_INT_H__
