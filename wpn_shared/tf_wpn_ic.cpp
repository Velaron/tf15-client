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

LINK_ENTITY_TO_CLASS( tf_weapon_ic, CTFIncendiaryC )

void CTFIncendiaryC::Spawn( void )
{
	Precache();
	m_iId = WEAPON_INCENDIARY;
	SET_MODEL( ENT( pev ), "models/w_rpg.mdl" );
	m_iDefaultAmmo = 50;
	pev->solid = SOLID_TRIGGER;
}

int CTFIncendiaryC::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = "rockets";
	p->pszName = STRING( pev->classname );
	p->iAmmo1 = m_pPlayer ? m_pPlayer->maxammo_rockets : 20;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = -1;
	p->iSlot = 4;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_INCENDIARY;
	p->iFlags = 0;
	p->iWeight = 20;
	return 1;
}

void CTFIncendiaryC::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = 0;
	m_pPlayer->m_flNextAttack = 0.5f;
	SendWeaponAnim( IC_HOLSTER1 );
}

void CTFIncendiaryC::Precache( void )
{
	PRECACHE_MODEL( "models/w_rpg.mdl" );
	PRECACHE_MODEL( "models/v_rpg.mdl" );
	PRECACHE_MODEL( "models/p_rpg.mdl" );
	PRECACHE_MODEL( "models/p_rpg2.mdl" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );
	UTIL_PrecacheOther( "tf_ic_rocket" );
	PRECACHE_SOUND( "weapons/rocketfire1.wav" );
	PRECACHE_SOUND( "weapons/glauncher.wav" );
	m_usFireIC = PRECACHE_EVENT( 1, "events/wpn/tf_ic.sc" );
}

void CTFIncendiaryC::WeaponIdle( void )
{
	ResetEmptySound();

	if ( m_flTimeWeaponIdle <= 0.0f )
	{
		if ( m_pPlayer->ammo_rockets <= 0 )
		{
			m_flTimeWeaponIdle = 1.0f;
		}
		else
		{
			m_flTimeWeaponIdle = 3.0f;
			SendWeaponAnim( IC_FIDGET );
		}
	}
}

BOOL CTFIncendiaryC::Deploy( void )
{
	if ( m_pPlayer->ammo_rockets )
		return DefaultDeploy( "models/v_rpg.mdl", "models/p_rpg.mdl", IC_DRAW1, "rpg", 1 );
	else
		return DefaultDeploy( "models/v_tfc_rpg.mdl", "models/p_rpg.mdl", IC_DRAW_UL, "rpg", 1 );
}

BOOL CTFIncendiaryC::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
		WRITE_BYTE( m_iId );
		MESSAGE_END();

		current_ammo = &m_pPlayer->ammo_rockets;

		return TRUE;
	}

	return FALSE;
}

void CTFIncendiaryC::PrimaryAttack( void )
{
	Vector p_vecOrigin, p_vecAngles;

	if ( m_pPlayer->ammo_rockets <= 0 )
	{
		PlayEmptySound();
	}
	else
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireIC, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		p_vecOrigin = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16.0f + gpGlobals->v_right * 8.0f + gpGlobals->v_up * -8.0f;
		p_vecAngles = m_pPlayer->pev->v_angle;
		CTFIncendiaryCRocket::CreateRpgRocket( p_vecOrigin, p_vecAngles, m_pPlayer, this );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		m_pPlayer->ammo_rockets--;
		m_flTimeWeaponIdle = 1.2f;
		m_flNextPrimaryAttack = GetNextAttackDelay( 1.2f );
	}
}
