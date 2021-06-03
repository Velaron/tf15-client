#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "tf_gamerules.h"

#include "skill.h"
#include "game.h"
#include "items.h"
#ifndef NO_VOICEGAMEMGR
#include "voice_gamemgr.h"
#endif
#include "tf_defs.h"

#define MAX_INTERMISSION_TIME 120

extern int gmsgViewMode;
extern int gmsgTeamInfo;

extern float g_flIntermissionStartTime;

extern cvar_t mp_chattime;

extern DLL_GLOBAL BOOL g_fGameOver;

CTeamFortress::CTeamFortress( void )
{
	CHalfLifeTeamplay::CHalfLifeMultiplay();
#ifndef NO_VOICEGAMEMGR
	m_VoiceGameMgr.Init( &g_GameMgrHelper, gpGlobals->maxClients );
#endif
}

void CTeamFortress::Think( void )
{
#ifndef NO_VOICEGAMEMGR
	m_VoiceGameMgr->Update( gpGlobals->frametime );
#endif
	CHalfLifeMultiplay::Think();
}

BOOL CTeamFortress::IsTeamplay( void )
{
	return gpGlobals->teamplay;
}

void CTeamFortress::PlayerSpawn( CBasePlayer *pPlayer )
{
	pPlayer->pev->weapons |= ( 1 << WEAPON_SUIT );
}

BOOL CTeamFortress::ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
#ifndef NO_VOICEGAMEMGR
	if ( m_VoiceGameMgr.ClientCommand( pPlayer, pcmd ) )
		return TRUE;
#endif
	return CHalfLifeTeamplay::ClientCommand( pPlayer, pcmd );
}

void CTeamFortress::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
	char *s;

	pPlayer->TeamFortress_SetSkin();

	pPlayer->exec_scripts = 0;

	s = g_engfuncs.pfnInfoKeyValue( infobuffer, "ec" );
	if ( s && s[0] && ( !strcmp( s, "1" ) || !strcmp( s, "on" ) ) )
	{
		pPlayer->exec_scripts = 1;
	}

	s = g_engfuncs.pfnInfoKeyValue( infobuffer, "exec_class" );
	if ( s && s[0] && ( !strcmp( s, "1" ) || !strcmp( s, "on" ) ) )
	{
		pPlayer->exec_scripts = 1;
	}

	pPlayer->exec_map_scripts = 0;

	s = g_engfuncs.pfnInfoKeyValue( infobuffer, "em" );
	if ( s && s[0] && ( !strcmp( s, "1" ) || !strcmp( s, "on" ) ) )
	{
		pPlayer->exec_map_scripts = 1;
	}

	s = g_engfuncs.pfnInfoKeyValue( infobuffer, "exec_map" );
	if ( s && s[0] && ( !strcmp( s, "1" ) || !strcmp( s, "on" ) ) )
	{
		pPlayer->exec_map_scripts = 1;
	}

	pPlayer->display_class_briefing = 0;
	pPlayer->take_screenshots = 0;

	s = g_engfuncs.pfnInfoKeyValue( infobuffer, "take_sshot" );
	if ( s && s[0] && ( !strcmp( s, "1" ) || !strcmp( s, "on" ) ) )
	{
		pPlayer->take_screenshots = 1;
	}

	s = g_engfuncs.pfnInfoKeyValue( infobuffer, "ts" );
	if ( s && s[0] && ( !strcmp( s, "1" ) || !strcmp( s, "on" ) ) )
	{
		pPlayer->take_screenshots = 1;
	}

	pPlayer->local_blood = 1;

	s = g_engfuncs.pfnInfoKeyValue( infobuffer, "cl_lb" );
	if ( s && s[0] )
	{
		pPlayer->local_blood = atoi( s ) != 0;
	}
}

int CTeamFortress::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_GUN_NO;
}

int CTeamFortress::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_ALL;
}

float CTeamFortress::FlItemRespawnTime( CItem *pItem )
{
	return gpGlobals->time + pItem->m_flRespawnTime;
}

BOOL CTeamFortress::CanHaveItem( CBasePlayer *pPlayer, CItem *pItem )
{
	if ( !pPlayer->is_feigning && !( pPlayer->tfstate & ( TFSTATE_CANT_MOVE | TFSTATE_AIMING ) ) && cb_prematch_time <= gpGlobals->time )
	{
		return ActivationSucceeded( pItem, pPlayer, NULL );
	}

	return FALSE;
}

const char *CTeamFortress::GetGameDescription( void )
{
	return "TF Classic";
}

const char *CTeamFortress::GetTeamID( CBaseEntity *pEntity )
{
	return GetTeamName( pEntity->team_no );
}

void CTeamFortress::InitHUD( CBasePlayer *pl )
{
	CHalfLifeTeamplay::InitHUD( pl );

	UTIL_LogPrintf( "\"%s<%i><%s><>\" entered the game\n",
	                STRING( pl->pev->netname ),
	                GETPLAYERUSERID( pl->edict() ),
	                GETPLAYERAUTHID( pl->edict() ) );

	UTIL_LogPrintf( "\"%s<%i><%s><>\" joined team \"SPECTATOR\"\n",
	                STRING( pl->pev->netname ),
	                GETPLAYERUSERID( pl->edict() ),
	                GETPLAYERAUTHID( pl->edict() ) );

	for ( int i = 2; i <= number_of_teams; i++ )
	{
		TeamFortress_TeamIncreaseScore( i, 0 );
	}

	pl->TeamFortress_ExecMapScript();

	pl->m_bDisplayedMOTD = 0;

	if ( CVAR_GET_FLOAT( "sv_cheats" ) == 0.0f )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgViewMode, NULL, pl->edict() );
		MESSAGE_END();
	}
}

BOOL CTeamFortress::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128] )
{
#ifndef NO_VOICEGAMEMGR
	m_VoiceGameMgr.ClientConnected( pEntity );
#endif

	if ( !g_bFirstClient )
	{
		g_bFirstClient = TRUE;
		ParseTFServerSettings();
	}

	MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
	WRITE_BYTE( ENTINDEX( pEntity ) );
	WRITE_STRING( "" );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_ONE, SVC_STUFFTEXT, NULL, pEntity );
	WRITE_STRING( UTIL_VarArgs( "cl_forwardspeed %d\ncl_backspeed %d\ncl_sidespeed %d\n", 400, 400, 400 ) );
	MESSAGE_END();

	return CHalfLifeMultiplay::ClientConnected( pEntity, pszName, pszAddress, szRejectReason );
}

void CTeamFortress::GoToIntermission( void )
{
	if ( g_fGameOver )
		return;

	MESSAGE_BEGIN( MSG_ALL, SVC_INTERMISSION );
	MESSAGE_END();

	DumpClanScores();
	clan_scores_dumped = 1.0f;

	// bounds check
	int time = (int)CVAR_GET_FLOAT( "mp_chattime" );
	if ( time < 1 )
		CVAR_SET_STRING( "mp_chattime", "1" );
	else if ( time > MAX_INTERMISSION_TIME )
		CVAR_SET_STRING( "mp_chattime", UTIL_dtos1( MAX_INTERMISSION_TIME ) );

	m_flIntermissionEndTime = gpGlobals->time + ( (int)mp_chattime.value );
	g_flIntermissionStartTime = gpGlobals->time;

	g_fGameOver = TRUE;
	m_iEndIntermissionButtonHit = FALSE;
}