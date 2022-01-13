#include <time.h>

#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "tf_defs.h"

// Velaron: TODO
BOOL ActivationSucceeded( CBaseEntity *Goal, CBasePlayer *AP, CBaseEntity *ActivatingGoal )
{
	return FALSE;
}

void ParseTFServerSettings( void )
{
	time_t aclock;
	tm *ptm;
	CBaseEntity *pEntity;

	toggleflags &= ~TFLAG_CHEATCHECK;

	time( &aclock );
	ptm = localtime( &aclock );

	if ( ptm->tm_mon == 11 && ptm->tm_mday == 25 )
		christmas = 1;

	if ( tfc_birthday.value != 0.0f || ( ptm->tm_mon == 7 && ptm->tm_mday == 24 ) )
	{
		birthday = 1;

		pEntity = CBaseEntity::Create( "timer", g_vecZero, g_vecZero, NULL );
		pEntity->weapon = 10;
		pEntity->SetThink( &CBaseEntity::Timer_Birthday );
		pEntity->pev->nextthink = gpGlobals->time + 60.0f;
	}

	cb_prematch_time = tfc_clanbattle_prematch.value * 60.0f;
	if ( cb_prematch_time == 0.0f )
		cb_prematch_time = tfc_prematch.value * 60.0f;
	
	if ( tfc_clanbattle.value != 0.0f )
	{
		clan_scores_dumped = 0.0f;

		cb_ceasefire_time = tfc_clanbattle_ceasefire.value * 60.0f;
		if ( cb_ceasefire_time != 0.0f )
		{
			last_cease_fire = 1;
			initial_cease_fire = 1;
			cease_fire = 1;
		}
	}

	autokick_kills = tfc_autokick_kills.value;

	if ( tfc_fragscoring.value == 0.0f )
		toggleflags &= ~TFLAG_FRAGSCORING;
	else
		toggleflags |= TFLAG_FRAGSCORING;

	gpGlobals->teamplay = teamplay.value;
}