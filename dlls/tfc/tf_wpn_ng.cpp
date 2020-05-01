#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "tf_defs.h"

LINK_ENTITY_TO_CLASS( tf_weapon_ng, CTFNailgun )

void CTFNailgun::Spawn( void )
{
	//current_ammo = 0;
	Precache();
	m_iId = WEAPON_NAILGUN;
	m_iDefaultAmmo = 50;
	pev->solid = SOLID_TRIGGER;
}

void CTFNailgun::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfc_nailgun.mdl" );
	PRECACHE_MODEL( "models/v_tfc_supernailgun.mdl" );
	PRECACHE_MODEL( "models/p_nailgun.mdl" );
	PRECACHE_MODEL( "models/p_nailgun2.mdl" );	
	PRECACHE_MODEL( "models/p_snailgun.mdl" );	
	PRECACHE_MODEL( "models/p_snailgun2.mdl" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );
	PRECACHE_SOUND( "items/clipinsert1.wav" );
	PRECACHE_SOUND( "items/cliprelease1.wav" );
	PRECACHE_SOUND( "weapons/airgun_1.wav" );
	PRECACHE_SOUND( "weapons/spike2.wav" );
	PRECACHE_SOUND( "weapons/357_cock1.wav" );
	m_usFireNailGun = PRECACHE_EVENT( 1, "events/wpn/tf_nail.sc" );
}

int CTFNailgun::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = "9mm";
	p->pszName = STRING( pev->classname );
	if ( m_pPlayer )
		p->iAmmo1 = m_pPlayer->maxammo_nails;
	else
		p->iAmmo1 = 200;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = 0;
	p->iSlot = 3;
	p->iPosition = 4;
	p->iId = m_iId = WEAPON_NAILGUN;
	p->iFlags = 0;
	p->iWeight = 15;
	return 1;
}

void CTFNailgun::PrimaryAttack( void )
{
	Vector p_vecOrigin, p_vecAngles;

	if( m_pPlayer->ammo_nails <= 0 )
	{
		PlayEmptySound();
	}
	else
	{
		m_pPlayer->m_iWeaponVolume = 1000;
		m_pPlayer->m_iWeaponFlash = 512;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireNailGun, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		p_vecOrigin = m_pPlayer->GetGunPosition() + gpGlobals->v_up * -4.0f + gpGlobals->v_right * 2.0f;
		p_vecAngles = m_pPlayer->pev->v_angle;
		//CTFNailgunNail::CreateNail( false, &p_vecOrigin, &p_vecAngles, m_pPlayer, this, true );
		m_pPlayer->ammo_nails--;

		if ( m_pPlayer->ammo_nails < 0 )
			m_pPlayer->ammo_nails = 0;

		m_flTimeWeaponIdle = 10.0f;
		m_flNextPrimaryAttack = GetNextAttackDelay( 0.1f );
	}
}

BOOL CTFNailgun::Deploy( void )
{
	return DefaultDeploy( "models/v_tfc_nailgun.mdl", "models/p_nailgun.mdl", NAILGUN_DEPLOY, "mp5", 1 );
}

void CTFNailgun::WeaponIdle( void )
{
	ResetEmptySound();

	if ( m_flTimeWeaponIdle < 0.0f )
	{
		m_flTimeWeaponIdle = 15.0f;
		SendWeaponAnim( NAILGUN_IDLE1, 1 );
	}
}

int CTFNailgun::AddToPlayer( CBasePlayer *pPlayer )
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

LINK_ENTITY_TO_CLASS( tf_weapon_superng, CTFSuperNailgun )


int CTFSuperNailgun::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = "9mm";
	p->pszName = STRING( pev->classname );
	if ( m_pPlayer )
		p->iAmmo1 = m_pPlayer->maxammo_nails;
	else
		p->iAmmo1 = 150;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = 0;
	p->iSlot = 3;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_SUPER_NAILGUN;
	p->iFlags = 0;
	p->iWeight = 15;
	return 1;
}

void CTFSuperNailgun::Precache( void )
{
	CTFNailgun::Precache();
	m_usFireSuperNailGun = PRECACHE_EVENT( 1, "events/wpn/tf_snail.sc" );
}

BOOL CTFSuperNailgun::Deploy( void )
{
	return DefaultDeploy( "models/v_tfc_supernailgun.mdl", "models/p_snailgun.mdl", NAILGUN_DEPLOY, "mp5", 1 );
}

void CTFSuperNailgun::PrimaryAttack( void )
{
	Vector p_vecOrigin, p_vecAngles;

	if ( m_pPlayer->ammo_nails <= 0 )
	{
		PlayEmptySound();
	}
	else
	{
		m_pPlayer->m_iWeaponVolume = 1000;
		m_pPlayer->m_iWeaponFlash = 512;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireSuperNailGun, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		p_vecOrigin = m_pPlayer->GetGunPosition() + gpGlobals->v_up * -4.0f + gpGlobals->v_right * 2.0f;
		p_vecAngles = m_pPlayer->pev->v_angle;
		if ( m_pPlayer->ammo_nails <= 3 )
		{
			//CTFNailgunNail::CreateNail( false, &p_vecOrigin, &p_vecAngles, m_pPlayer, this, true );
		}
		else
		{
			//CTFNailgunNail::CreateSuperNail( &p_vecOrigin, &p_vecAngles, m_pPlayer, this );
		}
		m_pPlayer->ammo_nails -= 2;

		if ( m_pPlayer->ammo_nails < 0 )
			m_pPlayer->ammo_nails = 0;

		m_flTimeWeaponIdle = 10.0f;
		m_flNextPrimaryAttack = GetNextAttackDelay( 0.1f );
	}
}
