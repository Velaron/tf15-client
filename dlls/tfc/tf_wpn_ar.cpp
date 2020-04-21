#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "tf_defs.h"

LINK_ENTITY_TO_CLASS( tf_weapon_ar, CTFAutoRifle )

void CTFAutoRifle::Spawn( void )
{
	pev->classname = MAKE_STRING( "tf_weapon_autorifle" );
	Precache();
	m_iId = WEAPON_AUTO_RIFLE;
	m_iDefaultAmmo = 5;
	pev->solid = SOLID_TRIGGER;
}

void CTFAutoRifle::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfc_sniper.mdl" );
	PRECACHE_MODEL( "models/p_sniper.mdl" );
	PRECACHE_MODEL( "models/p_sniper2.mdl" );
	PRECACHE_SOUND( "weapons/sniper.wav" );
	m_usFireAutoRifle = PRECACHE_EVENT( 1, "events/wpn/tf_ar.sc" );
}

int CTFAutoRifle::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = "buckshot";
	p->pszName = STRING( pev->classname );
	if ( m_pPlayer )
		p->iMaxAmmo1 = m_pPlayer->maxammo_shells;
	else
		p->iMaxAmmo1 = 75;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = -1;
	p->iSlot = 2;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_AUTO_RIFLE;
	p->iWeight = 10;
	return 1;
}

void CTFAutoRifle::Holster( void )
{
	m_fInReload = 0;
	m_pPlayer->m_flNextAttack = 0.5f;
	SendWeaponAnim( ARIFLE_HOLSTER, 1 );
}

void CTFAutoRifle::PrimaryAttack( void )
{
	Vector vecSrc, vecEnd;
	TraceResult tr;
	edict_t *pent;
	CBaseEntity *pEntity;

	if ( m_pPlayer->ammo_shells <= 0 )
	{
		m_flNextPrimaryAttack = GetNextAttackDelay( 5.0f );
		m_flTimeWeaponIdle = 1.0f;
		SendWeaponAnim( SRIFLE_IDLE, 1 );
		PlayEmptySound();
	}
	else
	{
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireAutoRifle, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->m_iWeaponVolume = 600;
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_flTimeWeaponIdle = 0.2f;
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		vecSrc = m_pPlayer->GetGunPosition() - gpGlobals->v_up * 2.0f;
		vecEnd = vecSrc + gpGlobals->v_forward * 8192.0f;
		UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

		if ( tr.flFraction != 1.0f )
		{
			pent = ENT( 0 );
			
			if ( tr.pHit || pent != 0 )
			{
				pEntity = CBaseEntity::Instance( tr.pHit );

				if ( pEntity )
				{
					ClearMultiDamage();
					pEntity->TraceAttack( m_pPlayer->pev, 8, gpGlobals->v_forward, &tr, DMG_BULLET );
					ApplyMultiDamage( pev, m_pPlayer->pev );
				}
			}
		}

		m_pPlayer->ammo_shells--;
		m_flNextPrimaryAttack = GetNextAttackDelay( 0.1f );
	}
}

BOOL CTFAutoRifle::Deploy( void )
{
	return DefaultDeploy( "models/v_tfc_sniper.mdl", "models/p_sniper.mdl", ARIFLE_DRAW, "autosniper", 1 );
}

void CTFAutoRifle::WeaponIdle( void )
{
	ResetEmptySound();

	if ( m_flTimeWeaponIdle < 0.0f )
	{
		SendWeaponAnim( ARIFLE_IDLE, 1 );
		m_flTimeWeaponIdle = 12.5f;
	}
}

int CTFAutoRifle::AddToPlayer( CBasePlayer *pPlayer )
{
	if( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, ENT( pPlayer->pev ) );
		WRITE_BYTE( m_iId );
		MESSAGE_END();
		return true;
	}

	return false;
}