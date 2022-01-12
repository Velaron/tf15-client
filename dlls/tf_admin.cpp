#include    "tf_defs.h"
#include	"extdll.h"
#include    "gamerules.h"
#include	"cbase.h"
#include    "util.h"
#include    "cdll_dll.h"

void Admin_CeaseFire( void )
{
    bool cease_fire;

    if( cease_fire )
    {
        cease_fire = false;
        UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "#Game_resumefire" ) );
        UTIL_LogPrintf( "World triggered \"Resume_Fire \"\n" );

        if( gpGlobals->maxClients > 1 )
        {
            int i;
            do
            {
                CBasePlayer *piPlayer = UTIL_PlayerByIndex( i );
                if(  )
                {
                    ->pev->iuser4 = 0;
                    = gpGlobals->time + 10.0;
                    ->tfstate &= ~0x10000u;
                    ->immune_to_check =;
                    CBasePlayer::TeamFortress_SetSpeed( );
                }
                ++
            } while ( gpGlobals->maxclients >= );
            
        }
    }
    else
    {
        cease_fire = true;
        UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "#Admin_ceasefire" ) );
        if( gpGlobals->maxClients > 1 )
        {
            int j;
            do
            {
                CBasePlayer *pjPlayer = UTIL_PlayerByIndex( j );
                if(  )
                {
                    ->pev->iuser4 = 0;
                    = gpGlobals->time + 10.0;
                    ->tfstate &= ~0x10000u;
                    ->immune_to_check =;
                    CBasePlayer::TeamFortress_SetSpeed( );
                }
            } while ( gpGlobals->maxclients >= );
            
        }
    }
}

void BanPlayer( CBaseEntity *pTarget )
{
    const char *name;

    void(*pfnServerCommand)( char *text );
    pfnServerCommand = g_engfuncs.pfnServerCommand;
    = g_engfuncs.pfnGetPlayerAuthId( pTarget->pev->pContainingEntity );
    = UTIL_VarArgs( "banid 30 %s kick\n", name );
    pfnServerCommand(  );
    = g_engfuncs.pfnServerCommand;
    = UTIL_VarArgs( "addip 30 %s\n", &gpGlobals->pStringBase[pTarget->ip] );

    = g_engfunc.pfnGetPlayerUserId( pTarget->pev->pContainingEntity );
    if( != -1 )
    {
        = g_engfuncs.pfnServerCommand;
        = UTIL_VarArgs( "kick # %d\n",  );

    }
}

void CBasePlayer::Admin_Access( CBasePlayer const *pPlayer , char *pPassword )
{
    char s;
    
    if( pPassword && *pPassword )
    {
        s = tfc_adminpwd.string;
        if( s && *s )
        {
            pPlayer->is_admin;
            ClientPrint( pPlayer->pev, HUD_PRINTCENTER, "#Admin_access" );
        }
        else
        {
            ClientPrint( pPlayer->pev, HUD_PRINTCENTER, "#Admin_badpassword" );
        }
    }
    else
    {
        ClientPrint( pPlayer->pev, HUD_PRINTCENTER, "#Admin_nopassword" );
    }
}

void CBasePlayer::Admin_Changelevel( CBasePlayer const *pPlayer )
{
    const char *mapname;
    char sz[256];

    if( g_engfuncs.pnfCmd_Argc() == 2 )
    {
        mapname = g_engfuncs.pnfCmd_Argv();
        sprintf( sz, "changelevel %s\n", mapname);
        g_engfuncs.pfnServerCommand( sz );
    }
}

void CBasePlayer::Admin_CountPlayers( CBasePlayer const *pPlayer )
{
    int d = TeamFortress_GetNoPlayers();
    char *param1 = UTIL_dtos1( d );

    ClientPrint( *pPlayer->pev, HUD_PRINTCENTER, "#Adminoplayers", param1 );
    if( number_of_teams > 0.0 )
    {
        int NoPlayers = TeamFortress_TeamGetNoPlayers( 1 );
        char *param2 = UTIL_dtos2( NoPlayers );
        ClientPrint( *pPlayer->pev, HUD_PRINTCENTER, "#Adminoteamplayers", UTIL_dtos1(1), param2 );
        if( number_of_teams <= 1.0 )
        {
LABEL_3:
            if( number_of_teams <= 2.0 )
            {
                goto LABEL_4;
            goto LABEL_8;
            }
        }
    }
    else if( number_of_teams <= 1.0 )
    {
        goto LABEL_3;
    }
    int NoPlayers2 = TeamFortress_TeamGetNoPlayers( 2 );
    char *param3 = UTIL_dtos2( NoPlayers2 );
    ClientPrint( *pPlayer->pev, HUD_PRINTCENTER, "#Adminoteamplayers", UTIL_dtos1(2), param3 );
    if( number_of_teams <= 2.0 )
    {
LABEL_4:
        if( number_of_teams <= 3.0 )
            return;
LABEL_9:
        int NoPlayers3 = TeamFortress_TeamGetNoPlayers( 4 );
        char *param4 = UTIL_dtos2( NoPlayers3 );
        ClientPrint( *pPlayer->pev, HUD_PRINTCENTER, "#Adminoteamplayers", UTIL_dtos1(4), param4 );
        return;
    }
LABEL_8:
    int NoPlayers4 = TeamFortress_TeamGetNoPlayers( 3 );
    char *param5 = UTIL_dtos2( NoPlayers4 );
    ClientPrint( *pPlayer->pev, HUD_PRINTCENTER, "#Adminoteamplayers", UTIL_dtos1(3), param5 );
    if( number_of_teams > 3.0 )
        goto LABEL_9;
}

void CBasePlayer::Admin_CycleDeal( CBasePlayer const *pPlayer )
{
    CBasePlayer *pEntity;

    if( TeamFortress_GetNoPlayers() <= 1 )
    {
        ClientPrint( pPlayer->pev, HUD_PRINTCENTER, "#Admin_gameempty" );
        EHANDLE::operator = ( &pPlayer->admin_use, pEntity);
        pPlayer->admin_mode 0;
    }
    else
    {
        int i;
        if( EHANDLE::operator = ( &pPlayer->admin_use ) )
        {
            i = g_engfuncs.pfnIndexOfEdict( CBaseEntity *EHANDLE::operator->pev->pContainingEntity ) + 1;
        }
        if ( gpGlobals->maxClients < i )
        {
LABEL_7:
            pPlayer->admin_mode = 0;
            ClientPrint( pPlayer->pev, HUD_PRINTCENTER, "#Admin_endlist" );
        }
        else
        {
            while( 1 )
            {
                pEntity = UTIL_PlayerByIndex( i );
                EHANDLE::operator = ( &pPlayer->admin_use, pEntity);
                if( EHANDLE::operator int( &pPlayer->admin_use ) )
                {
                    if( EHANDLE::operator CBaseEntity *( &pPlayer->admin_use) != pPlayer )
                        break;
                }
                if( gpGlobals->maxClients < ++i )
                    goto LABEL_7:
            }
            pPlayer->admin_mode = 1;
            const char s = gpGlobals->pStringBase;
            ClientPrint( pPlayer->pev, HUD_PRINTCENTER, "#Admin_kickban", &s[EHANDLE::operator int( &pPlayer->admin_use )->pev-netname], &s[EHANDLE::operator int( &pPlayer->admin_use )->ip] );  
        }
    }
}

void CBasePlayer::Admin_DoBan( CBasePlayer const *pPlayer )
{
    p_admin_use = &pPlayer->admin_use;
    if( EHANDLE::operator int( &pPlayer->admin_use ) )
    {
        const char pStringBase = gpGlobals->pStringBase;
        string_t netname = pPlayer->pev->netname;
        UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "#Admin_ban", &pStringBase[CBaseEntity *EHANDLE::operator->( p_admin_use )->pev->netname], &pStringBase[netname] );
        char *TeamName = "SPECTATOR";
        if( EHANDLE::operator->(p_admin_use)->team_no )
        {
            
        }
    }
}

void CBasePlayer::Admin_DoKick( void )
{

}

void CBasePlayer::Admin_ListIPs( void )
{

}

void CBasePlayer::CheckAutoKick( void )
{

}

void KickPlayer( CBaseEntity* )
{

}