#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "tf_defs.h"

LINK_ENTITY_TO_CLASS( tf_weapon_railgun, CTFRailgun )

void CTFRailgun::Spawn( void )
{
	pev->classname = MAKE_STRING(" tf_weapon_railgun" );
	Precache();
	SET_MODEL( ENT( pev ), "models/w_gauss.mdl" );
	m_iId = WEAPON_LASER;
	m_iDefaultAmmo = 17;
	pev->solid = SOLID_TRIGGER;
}

void CTFRailgun::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfc_railgun.mdl" );
	PRECACHE_MODEL( "models/w_gauss.mdl" );
	PRECACHE_MODEL( "models/p_9mmhandgun.mdl" );
	PRECACHE_MODEL( "models/p_9mmhandgun2.mdl" );
	PRECACHE_SOUND( "weapons/railgun.wav" );
	m_usFireRail = PRECACHE_EVENT( 1, "events/wpn/tf_rail.sc" );
}

int CTFRailgun::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = "9mm";
	p->pszName = STRING( pev->classname );
	if ( m_pPlayer )
		p->iAmmo1 = m_pPlayer->maxammo_nails;
	else
		p->iAmmo1 = 50;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = -1;
	p->iSlot = 1;
	p->iPosition = 2;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_LASER;
	p->iWeight = 10;
	return 1;
}

BOOL CTFRailgun::Deploy( void )
{
	return DefaultDeploy( "models/v_tfc_railgun.mdl", "models/p_9mmhandgun.mdl", RAIL_DRAW, "onehanded", 1 );
}

void CTFRailgun::WeaponIdle( void )
{
	ResetEmptySound();

	if ( m_flTimeWeaponIdle <= 0.0f )
	{
		m_flTimeWeaponIdle = 12.5f;
		SendWeaponAnim( RAIL_IDLE, 1 );
	}
}

void CTFRailgun::PrimaryAttack( void )
{
	Vector p_vecOrigin, p_vecAngles;

	if ( m_pPlayer->ammo_nails <= 0 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.2;
	}
	else
	{
		m_pPlayer->m_iWeaponVolume = 600;
		m_pPlayer->m_iWeaponFlash = 256;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireRail, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		p_vecOrigin = m_pPlayer->GetGunPosition() + gpGlobals->v_right * 2.0f + gpGlobals->v_up * -4.0f;
		p_vecAngles = m_pPlayer->pev->v_angle;
		//CTFNailgunNail::CreateRailgunNail( &p_vecOrigin, &p_vecAngles, m_pPlayer, this );
		m_pPlayer->ammo_nails--;
		m_flTimeWeaponIdle = 12.5f;
		m_flNextPrimaryAttack = GetNextAttackDelay( 0.4f );
	}
}
