//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef IGAMECLIENTEXPORTS_H
#define IGAMECLIENTEXPORTS_H

#include "../common/interface.h"

class IGameClientExports : public IBaseInterface
{
public:
	//virtual const char *GetServerHostName() = 0;
	//virtual bool IsPlayerGameVoiceMuted(int playerIndex) = 0;
	//virtual void MutePlayerGameVoice(int playerIndex) = 0;
	//virtual void UnmutePlayerGameVoice(int playerIndex) = 0;

	virtual char *LocaliseTextString( const char *msg ) = 0;
	virtual int GetRandomClass( void ) = 0;
	virtual char **GetTeamNames( void ) = 0;
	virtual int GetNumberOfTeams( void ) = 0;
	virtual int GetPlayerClass( void ) = 0;
};

#define GAMECLIENTEXPORTS_INTERFACE_VERSION "GameClientExports001"

#endif