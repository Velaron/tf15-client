#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

#define TF_DEFS_ONLY
#include "tf_defs.h"

LINK_ENTITY_TO_CLASS( tf_weapon_flamethrower, CTFFlamethrower )

void CTFFlamethrower::Spawn( void )
{
	Precache();
	m_iId = WEAPON_FLAMETHROWER;
	SET_MODEL( ENT( pev ), "models/w_egon.mdl" );
	m_iDefaultAmmo = 50;
	pev->solid = SOLID_TRIGGER;
}

int CTFFlamethrower::GetItemInfo( ItemInfo *p )
{
	p->iSlot = 3;
	p->iPosition = 2;
	p->pszAmmo1 = "uranium";
	if ( m_pPlayer )
		p->iAmmo1 = m_pPlayer->maxammo_cells;
	else
		p->iAmmo1 = 200;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->pszName = STRING( pev->classname );
	p->iMaxClip = -1;
	p->iId = m_iId = WEAPON_FLAMETHROWER;
	p->iFlags = 0;
	p->iWeight = 20;
	return 1;
}

void CTFFlamethrower::Holster( void )
{
	m_fInReload = 0;
	m_pPlayer->m_flNextAttack = 0.5f;
	SendWeaponAnim( FLAME_HOLSTER, 1 );
}

void CTFFlamethrower::PrimaryAttack( void )
{
	Vector p_vecOrigin, p_vecAngles;

	if ( m_pPlayer->ammo_cells <= 0 )
	{
		PlayEmptySound();
	}
	else
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireFlame, 0.0f, m_pPlayer->pev->origin, m_pPlayer->pev->angles, 0.0f, 0.0f, 0, 0, m_pPlayer->pev->waterlevel > 2, 0 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		p_vecOrigin = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16.0f + gpGlobals->v_right * 8.0f + gpGlobals->v_up * -8.0f;
		p_vecAngles = m_pPlayer->pev->v_angle;

		if ( m_pPlayer->pev->waterlevel > 2 )
		{
			m_flNextPrimaryAttack = GetNextAttackDelay( 1.0f );
			m_flTimeWeaponIdle = 1.0f;
		}
		else
		{
			//CTFFlamethrowerBurst::CreateBurst( &p_vecOrigin, &p_vecAngles, m_pPlayer, this );
			m_flNextPrimaryAttack = GetNextAttackDelay( 0.15f );
			m_flTimeWeaponIdle = 0.15f;
		}

		m_pPlayer->ammo_cells--;
	}
}

BOOL CTFFlamethrower::Deploy( void )
{
	return DefaultDeploy( "models/v_flame.mdl", "models/p_egon.mdl", FLAME_DRAW, "egon", 1 );
}

void CTFFlamethrower::Precache( void )
{
	PRECACHE_MODEL( "models/w_egon.mdl" );
	PRECACHE_MODEL( "models/v_flame.mdl" );
	PRECACHE_MODEL( "models/p_egon.mdl" );
	PRECACHE_MODEL( "models/p_egon2.mdl" );
	PRECACHE_MODEL( "models/w_9mmclip.mdl" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );
	PRECACHE_SOUND( "weapons/flmfire2.wav" );
	UTIL_PrecacheOther( "tf_flamethrower_burst" );
	m_usFireFlame = PRECACHE_EVENT( 1, "events/wpn/tf_flame.sc" );
}

void CTFFlamethrower::WeaponIdle( void )
{
	ResetEmptySound();

	if ( m_flTimeWeaponIdle <= 0.0f )
	{
		if ( UTIL_SharedRandomLong( m_pPlayer->random_seed, 0, 4 ) )
		{
			m_flTimeWeaponIdle = 12.5f;
			SendWeaponAnim( FLAME_IDLE1, 1 );
		}
		else
		{
			m_flTimeWeaponIdle = 3.0f;
			SendWeaponAnim( FLAME_FIDGET1, 1 );
		}
	}
}

int CTFFlamethrower::AddToPlayer( CBasePlayer *pPlayer )
{
	if( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
		WRITE_BYTE( m_iId );
		MESSAGE_END();
		return true;
	}

	return false;
}
