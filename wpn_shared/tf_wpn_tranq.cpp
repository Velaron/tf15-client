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

LINK_ENTITY_TO_CLASS( tf_weapon_tranq, CTFTranq )

void CTFTranq::Spawn( void )
{
	pev->classname = MAKE_STRING( "tf_weapon_tranq" );
	Precache();
	m_iId = WEAPON_TRANQ;
	m_iDefaultAmmo = 17;
	pev->solid = SOLID_TRIGGER;
}

void CTFTranq::Precache()
{
	PRECACHE_MODEL( "models/v_tfc_pistol.mdl" );
	PRECACHE_MODEL( "models/p_spygun.mdl" );
	PRECACHE_MODEL( "models/p_9mmhandgun2.mdl" );
	PRECACHE_SOUND( "weapons/dartgun.wav" );
	m_usFireTranquilizer = PRECACHE_EVENT( 1, "events/wpn/tf_tranq.sc" );
}

int CTFTranq::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = "buckshot";
	p->pszName = STRING( pev->classname );
	p->iAmmo1 = m_pPlayer ? m_pPlayer->maxammo_shells : 200;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = -1;
	p->iSlot = 1;
	p->iPosition = 4;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_TRANQ;
	p->iWeight = 10;
	return 1;
}

void CTFTranq::WeaponIdle( void )
{
	int iRand;

	ResetEmptySound();

	if ( m_flTimeWeaponIdle <= 0.0f )
	{
		iRand = UTIL_SharedRandomLong( m_pPlayer->random_seed, 0, 3 );

		if ( iRand == 3 * ( iRand / 3 ) )
		{
			m_flTimeWeaponIdle = 3.0625f;
			SendWeaponAnim( TRANQ_IDLE3 );
		}
		else if ( iRand + 1 == 3 * ( ( iRand + 1 ) / 3 ) )
		{
			m_flTimeWeaponIdle = 3.75f;
			SendWeaponAnim( TRANQ_IDLE1 );
		}
		else
		{
			m_flTimeWeaponIdle = 2.5f;
			SendWeaponAnim( TRANQ_IDLE2 );
		}
	}
}

BOOL CTFTranq::Deploy( void )
{
	pev->body = 1;

	if ( DefaultDeploy( "models/v_tfc_pistol.mdl", "models/p_spygun.mdl", TRANQ_DRAW, "onehanded", 1 ) )
	{
		SendWeaponAnim( TRANQ_DRAW, 0 );
		return TRUE;
	}

	return FALSE;
}

void CTFTranq::PrimaryAttack( void )
{
	Vector p_vecOrigin, p_vecAngles;

	if ( m_pPlayer->ammo_shells <= 0 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = GetNextAttackDelay( 0.2f );
	}
	else
	{
		m_pPlayer->m_iWeaponVolume = 600;
		m_pPlayer->m_iWeaponFlash = 256;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireTranquilizer, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		p_vecOrigin = m_pPlayer->GetGunPosition();
		p_vecAngles = m_pPlayer->pev->v_angle;
		//CTFNailgunNail::CreateTranqNail( &p_vecOrigin, &p_vecAngles, m_pPlayer, this );
		m_pPlayer->ammo_shells--;

		if ( m_pPlayer->ammo_nails < 0 )
			m_pPlayer->ammo_nails = 0;

		m_flTimeWeaponIdle = 12.5f;
		m_flNextPrimaryAttack = GetNextAttackDelay( 1.5f );
	}
}
