//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef __VOICE_VGUI_TWEAKDLG_H__
#define __VOICE_VGUI_TWEAKDLG_H__

class CMenuPanel;

// Returns true if the tweak dialog is currently up.
bool IsTweakDlgOpen();

// Returns a global instance of the tweak dialog.
CMenuPanel *GetVoiceTweakDlg();

#endif // __VOICE_VGUI_TWEAKDLG_H__
