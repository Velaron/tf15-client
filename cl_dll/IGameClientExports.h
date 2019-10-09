//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef IGAMECLIENTEXPORTS_H
#define IGAMECLIENTEXPORTS_H

#ifdef _WIN32
#pragma once
#endif

#include "../common/interface.h"

#ifdef _WIN32
#define CLIENT_DLLNAME "cl_dlls/client.dll"
#elif defined(OSX)
#define CLIENT_DLLNAME "cl_dlls/client.dylib"
#elif defined(LINUX)
#define CLIENT_DLLNAME "cl_dlls/client.so"
#endif

class IGameClientExports : public IBaseInterface
{
public:
	//virtual const char *GetServerHostName() = 0;
	//virtual bool IsPlayerGameVoiceMuted(int playerIndex) = 0;
	//virtual void MutePlayerGameVoice(int playerIndex) = 0;
	//virtual void UnmutePlayerGameVoice(int playerIndex) = 0;

	virtual char *LocaliseTextString( const char *msg ) = 0;
};

#define GAMECLIENTEXPORTS_INTERFACE_VERSION "GameClientExports001"


#endif