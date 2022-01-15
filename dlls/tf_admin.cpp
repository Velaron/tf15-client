#include "tf_defs.h"
#include "extdll.h"
#include "gamerules.h"
#include "cbase.h"
#include "util.h"
#include "cdll_dll.h"
#include "player.h"

void CBasePlayer::AdminAccess( char *pPassword )
{
	char *s;

	if ( pPassword && *pPassword )
	{
		s = tfc_adminpwd.string;
		if ( s && *s )
		{
			is_admin = 1;
			ClientPrint( pev, HUD_PRINTNOTIFY, "#Admin_access" );
		}
		else
		{
			ClientPrint( pev, HUD_PRINTNOTIFY, "#Admin_badpassword" );
		}
	}
	else
	{
		ClientPrint( pev, HUD_PRINTNOTIFY, "#Admin_nopassword" );
	}
}

void CBasePlayer::Admin_CountPlayers( void )
{
	int d = TeamFortress_GetNoPlayers();
	char *param1 = UTIL_dtos1( d );

	ClientPrint( pev, HUD_PRINTNOTIFY, "#Adminoplayers", param1 );
	if ( number_of_teams > 0.0 )
	{
		int NoPlayers = TeamFortress_TeamGetNoPlayers( 1 );
		char *param2 = UTIL_dtos2( NoPlayers );
		ClientPrint( pev, HUD_PRINTNOTIFY, "#Adminoteamplayers", UTIL_dtos1( 1 ), param2 );
		if ( number_of_teams <= 1.0 )
		{
		LABEL_3:
			if ( number_of_teams <= 2.0 )
			{
				goto LABEL_4;
				goto LABEL_8;
			}
		}
	}
	else if ( number_of_teams <= 1.0 )
	{
		goto LABEL_3;
	}
	int NoPlayers2 = TeamFortress_TeamGetNoPlayers( 2 );
	char *param3 = UTIL_dtos2( NoPlayers2 );
	ClientPrint( pev, HUD_PRINTNOTIFY, "#Adminoteamplayers", UTIL_dtos1( 2 ), param3 );
	if ( number_of_teams <= 2.0 )
	{
	LABEL_4:
		if ( number_of_teams <= 3.0 )
			return;
	LABEL_9:
		int NoPlayers3 = TeamFortress_TeamGetNoPlayers( 4 );
		char *param4 = UTIL_dtos2( NoPlayers3 );
		ClientPrint( pev, HUD_PRINTNOTIFY, "#Adminoteamplayers", UTIL_dtos1( 4 ), param4 );
		return;
	}
LABEL_8:
	int NoPlayers4 = TeamFortress_TeamGetNoPlayers( 3 );
	char *param5 = UTIL_dtos2( NoPlayers4 );
	ClientPrint( pev, HUD_PRINTNOTIFY, "#Adminoteamplayers", UTIL_dtos1( 3 ), param5 );
	if ( number_of_teams > 3.0 )
		goto LABEL_9;
}

void CBasePlayer::Admin_ListIPs( void )
{
	int playerIndex;
	CBasePlayer *pIndex;

	if ( TeamFortress_GetNoPlayers() <= 1 )
	{
		ClientPrint( pev, HUD_PRINTNOTIFY, "#Admin_gameempty" );
	}
	else
	{

		if ( gpGlobals->maxClients > 0 )
		{
			do
			{
				pIndex = (CBasePlayer *)UTIL_PlayerByIndex( playerIndex );
				if ( pIndex )
				{
					ClientPrint( pev, HUD_PRINTNOTIFY, "#Admin_listip",
					&gpGlobals->pStringBase[UTIL_PlayerByIndex( playerIndex )->pev->netname],
					&gpGlobals->pStringBase[UTIL_PlayerByIndex( playerIndex )->ip] );
					++playerIndex;
				}
			} while ( gpGlobals->maxClients >= playerIndex );
		}
		ClientPrint( pev, HUD_PRINTNOTIFY, "#Admin_endlist" );
	}
}

void CBasePlayer::Admin_CycleDeal( void )
{
	CBasePlayer *pEntity;

	if ( TeamFortress_GetNoPlayers() <= 1 )
	{
		ClientPrint( pev, HUD_PRINTNOTIFY, "#Admin_gameempty" );
		admin_use = pEntity;
		admin_mode = 0;
	}
	else
	{
		int playerIndex;
		if ( admin_use )
		{
			playerIndex = ENTINDEX( admin_use.Get() ) + 1;
		}
		if ( gpGlobals->maxClients < playerIndex )
		{
		LABEL_7:
			admin_mode = 0;
			ClientPrint( pev, HUD_PRINTNOTIFY, "#Admin_endlist" );
		}
		else
		{
			while ( 1 )
			{
				pEntity = (CBasePlayer *)UTIL_PlayerByIndex( playerIndex );
				admin_use = pEntity;
				if ( admin_use )
				{
					if ( admin_use != this )
						break;
				}
				if ( gpGlobals->maxClients < ++playerIndex )
					goto LABEL_7;
			}
			admin_mode = 1;
			ClientPrint( pev, HUD_PRINTNOTIFY, "#Admin_kickban", STRING( admin_use->pev->netname ),
			STRING( admin_use->ip ) );
		}
	}
}

void CBasePlayer::Admin_DoKick( void )
{
	if ( admin_use )
	{
		string_t netname = pev->netname;
		UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "#Admin_kick", STRING( admin_use->pev->netname ),
		STRING( netname ) );

		char *TeamName = "SPECTATOR";

		if ( admin_use->team_no )
		{
			TeamName = GetTeamName( admin_use->team_no );
		}

		char *TeamName2 = "SPECTATOR";

		if ( team_no )
			TeamName2 = GetTeamName( team_no );

		UTIL_LogPrintf( "\"%s<%i><%s><%s>\" triggered \"Admin_Kick\" against \"%s<%i><%s><%s>\"\n",
		STRING( pev->netname ),
		GETPLAYERUSERID( pev->pContainingEntity ),
		GETPLAYERAUTHID( pev->pContainingEntity ),
		TeamName2,
		STRING(admin_use->pev->netname),
		GETPLAYERUSERID( ENT( admin_use->pev ) ),
		GETPLAYERAUTHID( ENT( admin_use->pev ) ),
		TeamName );

		if ( GETPLAYERUSERID( admin_use->pev->pContainingEntity ) != -1 )
		{
			SERVER_COMMAND( UTIL_VarArgs( "kick # %d\n",
			GETPLAYERUSERID( admin_use->pev->pContainingEntity ) ) );
		}
		admin_mode = 0;
		admin_use = 0;
	}
}

void CBasePlayer::Admin_DoBan( void )
{
    if ( admin_use )
	{
		string_t netname = pev->netname;
		UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "#Admin_kick", STRING( admin_use->pev->netname ),
		STRING( netname ) );

		char *TeamName = "SPECTATOR";

		if ( admin_use->team_no )
		{
			TeamName = GetTeamName( admin_use->team_no );
		}

		char *TeamName2 = "SPECTATOR";
		const char *s = gpGlobals->pStringBase;

		if ( team_no )
			TeamName2 = GetTeamName( team_no );


		char *TeamName2 = "SPECTATOR";
		const char *s = gpGlobals->pStringBase;

		if ( team_no )
			TeamName2 = GetTeamName( team_no );

		UTIL_LogPrintf( "\"%s<%i><%s><%s>\" triggered \"Admin_Ban\" against \"%s<%i><%s><%s>\"\n",
		STRING(pev->netname),
		GETPLAYERUSERID( pev->pContainingEntity ),
		GETPLAYERAUTHID( pev->pContainingEntity ),
		TeamName2,
		STRING(admin_use->pev->netname),
		GETPLAYERUSERID( ENT( admin_use->pev ) ),
		GETPLAYERAUTHID( ENT( admin_use->pev ) ),
		TeamName );

		BanPlayer( admin_use );
		admin_mode = 0;
		admin_use = 0;
	}
}

void Admin_CeaseFire( void )
{
	bool cease_fire;
    CBasePlayer *pIndex;

	if ( cease_fire )
	{
		cease_fire = false;
		UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "#Game_resumefire" ) );
		UTIL_LogPrintf( "World triggered \"Resume_Fire \"\n" );

		if ( gpGlobals->maxClients > 0 )
		{
			int playerIndex;
			do
			{
				pIndex =(CBasePlayer *) UTIL_PlayerByIndex( playerIndex );
				if ( pIndex )
				{
					pIndex->pev->iuser4 = 0;
					pIndex->tfstate &= ~0x10000u;
					pIndex->immune_to_check = gpGlobals->time + 10.0;
					pIndex->TeamFortress_SetSpeed( );
				}
				++playerIndex;
			} while ( gpGlobals->maxClients >= playerIndex );
		}
	}
	else
	{
		cease_fire = true;
        CBasePlayer *pIndex2;

		UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "#Admin_ceasefire" ) );
		if ( gpGlobals->maxClients > 1 )
		{
			int playerIndex2;
			do
			{
				pIndex2 =(CBasePlayer *) UTIL_PlayerByIndex( playerIndex2 );
				if ( gpGlobals->maxClients > 0 )
				{
					pIndex2->pev->iuser4 = 0;
					pIndex2->tfstate &= ~0x10000u;
					pIndex2->immune_to_check = gpGlobals->time + 10.0;
					pIndex2->TeamFortress_SetSpeed( );
				}
			} while ( gpGlobals->maxClients >= playerIndex2 );
		}
	}
}

void CBasePlayer::CheckAutoKick( void )
{
	if ( teamkills < autokick_kills || autokick_kills == 0.0 )
	{
		if ( autokick_kills != 0.0 )
		{
			if ( teamkills == autokick_kills - 1.0 )
			{
				ClientPrint( pev, HUD_PRINTCENTER, "#Akick_warn" );
				ClientPrint( pev, HUD_PRINTNOTIFY, "#Akick_warn" );
			}
			CBaseEntity *pStartEntity = UTIL_FindEntityByClassname( 0, "ak_timer" );
			if ( !pStartEntity )
				goto LABEL_5;

            bool pS;
            
			do
			{
				pS = pStartEntity->pev->owner == pev->pContainingEntity;
				pStartEntity = UTIL_FindEntityByClassname( pStartEntity, "ak_timer" );
			} while ( pStartEntity && !pS );
			if ( !pS )
			{
			LABEL_5:
				CBaseEntity *Timer = CBaseEntity::CreateTimer( TF_TIMER_ANY );
				Timer->pev->classname = "ak_timer" - gpGlobals->pStringBase;
				Timer->SetThink( &CBaseEntity::Timer_AutokickThink );
				Timer->pev->nextthink = gpGlobals->time + tfc_autokick_time.value;
			}
		}
	}
	else
	{
		char *TeamName = "SPECTATOR";
		UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "#Akick_kicked", STRING(pev->netname) );
		if ( team_no )
			TeamName = GetTeamName( team_no );
		UTIL_LogPrintf( "\"<-1><><>\" triggered \"Auto_Kick_For_Tk\" against \"%s<%i><%s><%s>\"\n",
		STRING(pev->netname),
		GETPLAYERUSERID( pev->pContainingEntity ),
		GETPLAYERAUTHID( pev->pContainingEntity ),
		TeamName );
		BanPlayer( this );
	}
}

void KickPlayer( CBaseEntity *pTarget )
{
	if ( GETPLAYERUSERID( pTarget->pev->pContainingEntity )!= -1 )
	{
		SERVER_COMMAND( UTIL_VarArgs( "kick # %d\n", GETPLAYERUSERID( pTarget->pev->pContainingEntity ) ) );
	}
}

void BanPlayer( CBaseEntity *pTarget )
{
	SERVER_COMMAND( UTIL_VarArgs( "banid 30 %s kick\n", GETPLAYERAUTHID( pTarget->pev->pContainingEntity ) ) );
	SERVER_COMMAND( UTIL_VarArgs( "addip 30 %s\n", &gpGlobals->pStringBase[pTarget->ip] ) );
	if ( GETPLAYERUSERID( pTarget->pev->pContainingEntity ) != -1 )
	{
		SERVER_COMMAND( UTIL_VarArgs( "kick # %d\n", GETPLAYERUSERID( pTarget->pev->pContainingEntity ) ) );
	}
}

void CBasePlayer::Admin_Changelevel( void )
{
	const char *mapname;
	char sz[256];

	if ( CMD_ARGC() == 2 )
	{
		mapname = CMD_ARGV( 1 );
		sprintf( sz, "changelevel %s\n", mapname );
		SERVER_COMMAND( sz );
	}
}