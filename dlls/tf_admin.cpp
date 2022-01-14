#include    "tf_defs.h"
#include    "extdll.h"
#include    "gamerules.h"
#include    "cbase.h"
#include    "util.h"
#include    "cdll_dll.h"

void CBasePlayer::Admin_Access( CBasePlayer const *pPlayer , char *pPassword )
{
    char s;
    
    if( pPassword && *pPassword )
    {
        s = tfc_adminpwd.string;
        if( s && *s )
        {
            pPlayer->is_admin;
            ClientPrint( pPlayer->pev, HUD_PRINTNOTIFY, "#Admin_access" );
        }
        else
        {
            ClientPrint( pPlayer->pev, HUD_PRINTNOTIFY, "#Admin_badpassword" );
        }
    }
    else
    {
        ClientPrint( pPlayer->pev, HUD_PRINTNOTIFY, "#Admin_nopassword" );
    }
}

void CBasePlayer::Admin_CountPlayers( CBasePlayer const *pPlayer )
{
    int d = TeamFortress_GetNoPlayers();
    char *param1 = UTIL_dtos1( d );

    ClientPrint( *pPlayer->pev, HUD_PRINTNOTIFY, "#Adminoplayers", param1 );
    if( number_of_teams > 0.0 )
    {
        int NoPlayers = TeamFortress_TeamGetNoPlayers( 1 );
        char *param2 = UTIL_dtos2( NoPlayers );
        ClientPrint( *pPlayer->pev, HUD_PRINTNOTIFY, "#Adminoteamplayers", UTIL_dtos1(1), param2 );
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
    ClientPrint( *pPlayer->pev, HUD_PRINTNOTIFY, "#Adminoteamplayers", UTIL_dtos1(2), param3 );
    if( number_of_teams <= 2.0 )
    {
LABEL_4:
        if( number_of_teams <= 3.0 )
            return;
LABEL_9:
        int NoPlayers3 = TeamFortress_TeamGetNoPlayers( 4 );
        char *param4 = UTIL_dtos2( NoPlayers3 );
        ClientPrint( *pPlayer->pev, HUD_PRINTNOTIFY, "#Adminoteamplayers", UTIL_dtos1(4), param4 );
        return;
    }
LABEL_8:
    int NoPlayers4 = TeamFortress_TeamGetNoPlayers( 3 );
    char *param5 = UTIL_dtos2( NoPlayers4 );
    ClientPrint( *pPlayer->pev, HUD_PRINTNOTIFY, "#Adminoteamplayers", UTIL_dtos1(3), param5 );
    if( number_of_teams > 3.0 )
        goto LABEL_9;
}

void CBasePlayer::Admin_ListIPs( CBasePlayer const *pPlayer )
{
    if( TeamFortress_GetNoPlayers() <= 1 )
    {
        ClientPrint( pPlayer->pev, HUD_PRINTNOTIFY, "#Admin_gameempty" );
    }
    else
    {
        int playerIndex;

        if( gpGlobals->maxClients > 0 )
        {
            do
            {
                CBasePlayer *pIndex = UTIL_PlayerByIndex( playerIndex );
                if( pIndex )
                {
                    ClientPrint( pPlayer->pev, HUD_PRINTNOTIFY, "#Admin_listip", 
                    &gpGlobals->pStringBase[UTIL_PlayerByIndex( playerIndex )->pev->netname],
                    &gpGlobals->pStringBase[UTIL_PlayerByIndex( playerIndex )->ip] );
                ++playerIndex;
                }
            }
            while( gpGlobals->maxClients >= playerIndex );
        }
        ClientPrint( pPlayer->pev, HUD_PRINTNOTIFY, "#Admin_endlist" );
    }
}

void CBasePlayer::Admin_CycleDeal( CBasePlayer const *pPlayer )
{
    CBasePlayer *pEntity;

    if( TeamFortress_GetNoPlayers() <= 1 )
    {
        ClientPrint( pPlayer->pev, HUD_PRINTNOTIFY, "#Admin_gameempty" );
        EHANDLE::operator = ( &pPlayer->admin_use, pEntity);
        pPlayer->admin_mode 0;
    }
    else
    {
        int playerIndex;
        if( EHANDLE::operator = ( &pPlayer->admin_use ) )
        {
            playerIndex = g_engfuncs.pfnIndexOfEdict( CBaseEntity *EHANDLE::operator->pev->pContainingEntity ) + 1;
        }
        if ( gpGlobals->maxClients < playerIndex )
        {
LABEL_7:
            pPlayer->admin_mode = 0;
            ClientPrint( pPlayer->pev, HUD_PRINTNOTIFY, "#Admin_endlist" );
        }
        else
        {
            while( 1 )
            {
                pEntity = UTIL_PlayerByIndex( playerIndex );
                EHANDLE::operator = ( &pPlayer->admin_use, pEntity);
                if( EHANDLE::operator int( &pPlayer->admin_use ) )
                {
                    if( EHANDLE::operator CBaseEntity *( &pPlayer->admin_use) != pPlayer )
                        break;
                }
                if( gpGlobals->maxClients < ++playerIndex )
                    goto LABEL_7;
            }
            pPlayer->admin_mode = 1;
            const char s = gpGlobals->pStringBase;
            ClientPrint( pPlayer->pev, HUD_PRINTNOTIFY, "#Admin_kickban", &s[EHANDLE::operator int( &pPlayer->admin_use )->pev-netname], 
            &s[EHANDLE::operator int( &pPlayer->admin_use )->ip] );  
        }
    }
}

void CBasePlayer::Admin_DoKick( void )
{
        p_admin_use = &pPlayer->admin_use;
    if( EHANDLE::operator int( &pPlayer->admin_use ) )
    {
        const char pStringBase = gpGlobals->pStringBase;
        string_t netname = pPlayer->pev->netname;
        UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "#Admin_kick", &pStringBase[CBaseEntity *EHANDLE::operator->( p_admin_use )->pev->netname], 
        &pStringBase[netname] );

        char *TeamName = "SPECTATOR";

        if( EHANDLE::operator->(p_admin_use)->team_no )
        {
            TeamName = GetTeamName( EHANDLE::operator->(p_admin_use)->team_no )
        }

        char *TeamName2 = "SPECTATOR";
        const char *s = gpGlobals->pStringBase;

        if( pPlayer->team_no )
            TeamName2 = GetTeamName( pPlayer->team_no );
        
        UTIL_LogPrintf( "\"%s<%i><%s><%s>\" triggered \"Admin_Kick\" against \"%s<%i><%s><%s>\"\n", 
        &gpGlobals->pStringBase[pPlayer->pev->netname],
        g_engfuncs.pfnGetPlayerUserId( pPlayer->pev->pContiningEntity ),
        g_engfuncs.pfnGetPlayerAuthId( pPlayer->pev->pContiningEntity ),
        TeamName2,
        &s[EHANDLE::operator->(p_admin_use)->pev->netname],
        g_engfuncs.pfnGetPlayerUserId( EHANDLE::operator->(p_admin_use)->pev->pContiningEntity ),
        v21,
        TeamName );

        if( g_engfuncs.pfnGetPlayerUserId( EHANDLE::operator CBaseEntity *(p_admin_use)->pev->pContainingEntity != -1 ) )
        {
            g_engfuncs.pfnServerCommand( UTIL_VarArgs( "kick # %d\n", 
            g_engfuncs.pfnGetPlayerUserId( EHANDLE::operator CBaseEntity *(p_admin_use)->pev->pContainingEntity ) )
        }

        pPlayer->admin_mode = 0;
        EHANDLE::operator = ( p_admin_use, 0 );
    }
}

void CBasePlayer::Admin_DoBan( CBasePlayer const *pPlayer )
{
    p_admin_use = &pPlayer->admin_use;
    if( EHANDLE::operator int( &pPlayer->admin_use ) )
    {
        const char pStringBase = gpGlobals->pStringBase;
        string_t netname = pPlayer->pev->netname;
        UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "#Admin_ban", &pStringBase[CBaseEntity *EHANDLE::operator->( p_admin_use )->pev->netname], 
        &pStringBase[netname] );

        char *TeamName = "SPECTATOR";

        if( EHANDLE::operator->(p_admin_use)->team_no )
        {
            TeamName = GetTeamName( EHANDLE::operator->(p_admin_use)->team_no )
        }

        char *TeamName2 = "SPECTATOR";
        const char *s = gpGlobals->pStringBase;

        if( pPlayer->team_no )
            TeamName2 = GetTeamName( pPlayer->team_no );
        
        UTIL_LogPrintf( "\"%s<%i><%s><%s>\" triggered \"Admin_Ban\" against \"%s<%i><%s><%s>\"\n", 
        &gpGlobals->pStringBase[pPlayer->pev->netname],
        g_engfuncs.pfnGetPlayerUserId( pPlayer->pev->pContiningEntity ),
        g_engfuncs.pfnGetPlayerAuthId( pPlayer->pev->pContiningEntity ),
        TeamName2,
        &s[EHANDLE::operator->(p_admin_use)->pev->netname],
        g_engfuncs.pfnGetPlayerUserId( EHANDLE::operator->(p_admin_use)->pev->pContiningEntity ),
        v18,
        TeamName );

        BanPlayer( EHANDLE::operator CBaseEntity *(p_admin_use) );
        pPlayer->admin_mode = 0;
        EHANDLE::operator = ( p_admin_use, 0 );
    }
}

void Admin_CeaseFire( void )
{
    bool cease_fire;

    if( cease_fire )
    {
        cease_fire = false;
        UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "#Game_resumefire" ) );
        UTIL_LogPrintf( "World triggered \"Resume_Fire \"\n" );

        if( gpGlobals->maxClients > 0 )
        {
            int playerIndex;
            do
            {
                CBasePlayer *pIndex = UTIL_PlayerByIndex( playerIndex );
                if( pIndex )
                {
                    pIndex->pev->iuser4 = 0;
                    pIndex->tfstate &= ~0x10000u;
                    pIndex->immune_to_check = gpGlobals->time + 10.0;
                    CBasePlayer::TeamFortress_SetSpeed( pIndex );
                }
                ++playerIndex;
            } while ( gpGlobals->maxclients >= playerIndex );
            
        }
    }
    else
    {
        cease_fire = true;
        UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "#Admin_ceasefire" ) );
        if( gpGlobals->maxClients > 1 )
        {
            int playerIndex2;
            do
            {
                CBasePlayer *pIndex2 = UTIL_PlayerByIndex( playerIndex2 );
                if( gpGlobals->maxClients > 0 )
                {
                    pIndex2->pev->iuser4 = 0;
                    pIndex2->tfstate &= ~0x10000u;
                    pIndex2->immune_to_check = gpGlobals->time + 10.0;
                    CBasePlayer::TeamFortress_SetSpeed( pIndex2 );
                }
            } while ( gpGlobals->maxclients >= playerIndex2 );
            
        }
    }
}

void CBasePlayer::CheckAutoKick( CBasePlayer *pPlayer )
{
    if( pPlayer->teamkills < autokick_kills || autokick_kills == 0.0 )
    {
        if( autokick_kills != 0.0 )
        {
            if( pPlayer->teamkills == autokick_kills - 1.0 )
            {
                ClientPrint( pPlayer->pev, HUD_PRINTCENTER, "#Akick_warn"  );
                ClientPrint( pPlayer->pev, HUD_PRINTNOTIFY, "#Akick_warn"  );
            }
            CBaseEntity *pStartEntity = UTIL_FindEntityByClassname( 0, "ak_timer" );
            if( !pStartEntity )
                goto LABEL_5;
            do
            {
                bool pS = pStartEntity->pev->owner == pPlayer->pev->pContainingEntity;
                pStartEntity = UTIL_FindEntityByClassname( pStartEntity, "ak_timer" )
            }
            while( pStartEntity && !pS );
            if( !pS )
            {
LABEL_5:
                CBaseEntity *Timer = CBaseEntity::CreateTimer( pPLayer, 0 );
                Timer->pev->classname = "ak_timer" - gpGlobals->pStringBase;
                Timer->m_pfnThink.__pfn = CBaseEntity::Timer_AutokickThink;
                Timer->m_pfnThink.__delta = 0;
                Timer->pev->nextthink = gpGlobals->time + tfc_autokick_time.value;
            }
        }
    }
    else
    {
        char *TeamName = "SPECTATOR";
        UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "#Akick_kicked", &gpGlobals->pStringBase[pPlayer->pev->netname] );
        if( pPlayer->team_no )
            TeamName = GetTeamName( pPlayer->team_no );
        UTIL_LogPrintf( "\"<-1><><>\" triggered \"Auto_Kick_For_Tk\" against \"%s<%i><%s><%s>\"\n",
        &gpGlobals->pStringBase[pPlayer->pev->netname],
        g_engfuncs.pfnGetPlayerUserId( pPlayer->pev->pContainingEntity ),
        g_engfuncs.pfnGetPlayerAuthId( pPlayer->pev->pContainingEntity ),
        TeamName );
        BanPlayer( pPlayer );
    }
}   

void KickPlayer( CBaseEntity *pTarget )
{
    if( g_engfuncs.pfnGetPlayerUserId( pTarget->pev->pContainingEntity != -1 ) )
    {
        g_engfuncs.pfnServerCommand( UTIL_VarArgs( "kick # %d\n", g_engfuncs.pfnGetPlayerUserId( pTarget->pev->pContainingEntity ) );
    }
}

void BanPlayer( CBaseEntity *pTarget )
{
    g_engfuncs.pfnServerCommand( UTIL_VarArgs( "banid 30 %s kick\n", g_engfuncs.pfnGetPlayerAuthId( pTarget->pev->pContainingEntity ) ) );
    g_engfuncs.pfnServerCommand( UTIL_VarArgs( "addip 30 %s\n", &gpGlobals->pStringBase[pTarget->ip] ) );
    if( g_engfuncs.pfnGetPlayerUserId( pTarget->pev->pContainingEntity ) != -1 )
    {
        g_engfuncs.pfnServerCommand( UTIL_VarArgs( "kick # %d\n", g_engfuncs.pfnGetPlayerUserId( pTarget->pev->pContainingEntity ) ) );
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