#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "tf_defs.h"

float g_fNextPrematchAlert;

// Velaron: TODO
void LogMatchResults( int *iNoPlayers, int *iUnaccountedFrags, int bDraw )
{
	
}

// Velaron: TODO
void DumpClanScores( void )
{
	int iUnaccountedFrags[5];
	int iNoPlayers[5];
	CBaseEntity *pPlayer;

	for ( int i = 0; i < 5; i++ )
		iNoPlayers[i] = TeamFortress_TeamGetNoPlayers( i );
	
	for ( int i = 0; i < gpGlobals->maxClients; i++ )
	{
		pPlayer = UTIL_PlayerByIndex( i );

		if ( pPlayer )
			iUnaccountedFrags[pPlayer->team_no] += pPlayer->real_frags;
	}

	iUnaccountedFrags[4] = teamfrags[4] - iUnaccountedFrags[4];
	UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "#Match_results" );
	LogMatchResults( iNoPlayers, iUnaccountedFrags, 0 ); // sort teams
}

// Velaron: TODO
void Display_Prematch( void )
{
	if ( g_fNextPrematchAlert > gpGlobals->time )
		return;
}

// Velaron: TODO
void Check_Ceasefire( void )
{
}