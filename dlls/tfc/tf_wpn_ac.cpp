#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "tf_defs.h"

LINK_ENTITY_TO_CLASS( tf_weapon_ac, CTFAssaultC )

void CTFAssaultC::Spawn()
{
	pev->classname = MAKE_STRING( "tf_weapon_ac" );
	Precache();
	m_iId = 4;
	m_iDefaultAmmo = 25;
	m_iWeaponState = 0;
	pev->solid = SOLID_TRIGGER;
}

void CTFAssaultC::Precache()
{
	PRECACHE_MODEL("models/v_tfac.mdl");
	PRECACHE_MODEL("models/p_mini.mdl");
	PRECACHE_MODEL("models/p_mini2.mdl");
	m_iShell = PRECACHE_MODEL("models/shell.mdl");
	PRECACHE_SOUND("weapons/357_cock1.wav");
	PRECACHE_SOUND("weapons/asscan1.wav");
	PRECACHE_SOUND("weapons/asscan2.wav");
	PRECACHE_SOUND("weapons/asscan3.wav");
	PRECACHE_SOUND("weapons/asscan4.wav");
	m_usWindUp = PRECACHE_EVENT(1, "events/wpn/tf_acwu.sc");
	m_usWindDown = PRECACHE_EVENT(1, "events/wpn/tf_acwd.sc");
	m_usFire = PRECACHE_EVENT(1, "events/wpn/tf_acfire.sc");
	m_usStartSpin = PRECACHE_EVENT(1, "events/wpn/tf_acsspin.sc");
	m_usSpin = PRECACHE_EVENT(1, "events/wpn/tf_acspin.sc");
	m_usACStart = PRECACHE_EVENT(1, "events/wpn/tf_acstart.sc");
}

int CTFAssaultC::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "buckshot";

	if ( m_pPlayer )
		p->iMaxAmmo1 = m_pPlayer->maxammo_shells;
	else
		p->iMaxAmmo1 = 200;
	
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = -1;
	p->iSlot = 4;
	p->iPosition = 3;
	p->iFlags = 0;
	p->iId = WEAPON_ASSAULT_CANNON;
	p->iWeight = 15;
	return 1;
}

BOOL CTFAssaultC::Deploy()
{
	return DefaultDeploy( "models/v_tfac.mdl", "models/p_mini.mdl", AC_DEPLOY, "ac", 1 );
}

int CTFAssaultC::AddToPlayer( CBasePlayer *pPlayer )
{
	if( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, ENT( pPlayer->pev ) );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

void CTFAssaultC::Holster()
{
	PLAYBACK_EVENT_FULL( FEV_NOTHOST | FEV_RELIABLE | FEV_GLOBAL, ENT( m_pPlayer->pev ), m_usWindDown, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, 0.0f, 0, 0, 1, 0 );
	m_iWeaponState = 0;
	//m_pPlayer->tfstate &= 0xFFFFF7FF;
	//m_pPlayer->TeamFortress_SetSpeed();
	m_fInReload = 0;
	m_flTimeWeaponIdle = 2.0f;
	m_pPlayer->m_flNextAttack = 0.1f;
	SendWeaponAnim( AC_HOLSTER, 1 );
}

void CTFAssaultC::WeaponIdle( void )
{
	ResetEmptySound();

	if( m_flTimeWeaponIdle <= 0.0f )
	{
		if( m_iWeaponState )
		{
			PLAYBACK_EVENT_FULL( FEV_NOTHOST | FEV_RELIABLE | FEV_GLOBAL, ENT( m_pPlayer->pev ), m_usWindDown, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
			m_iWeaponState = 0;
			//m_pPlayer->tfstate &= 0xFFFFF7FF;
			//m_pPlayer->TeamFortress_SetSpeed();
			m_flTimeWeaponIdle = 2.0f;
		}
		else
		{
			SendWeaponAnim( !UTIL_SharedRandomLong( m_pPlayer->random_seed, 0, 1 ), 1 );
			m_flTimeWeaponIdle = 12.5f;
		}
	}
}

void CTFAssaultC::PrimaryAttack()
{
	switch( m_iWeaponState )
	{
		case 1:
			if ( m_flNextPrimaryAttack <= 0.0f )
			{
				PLAYBACK_EVENT_FULL( FEV_NOTHOST | FEV_RELIABLE | FEV_GLOBAL, ENT( m_pPlayer->pev ), m_usACStart, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
				m_iWeaponState = 2;
				m_flNextPrimaryAttack = GetNextAttackDelay( 0.1f );
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1f;
			}
			return;
		case 2:
		{
			if ( m_pPlayer->ammo_shells <= 0 )
			{
				PLAYBACK_EVENT_FULL( FEV_NOTHOST | FEV_RELIABLE | FEV_GLOBAL, ENT( m_pPlayer->pev ), m_usStartSpin, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
				m_iWeaponState = 3;
				m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
				pev->effects &= ~EF_MUZZLEFLASH;
			}
			else
			{
				Fire();
			}
			
			m_flNextPrimaryAttack = GetNextAttackDelay( 0.1f );
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1f;
			return;
		}
		case 3:
		{
			if ( m_pPlayer->ammo_shells <= 0 )
			{
				PLAYBACK_EVENT_FULL( FEV_NOTHOST | FEV_RELIABLE | FEV_GLOBAL, ENT( m_pPlayer->pev ), m_usSpin, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
				m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
			}
			else
			{
				m_iWeaponState = 1;
			}
			
			m_flNextPrimaryAttack = GetNextAttackDelay( 0.1f );
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1f;
			return;
		}
	}

	if( m_pPlayer->pev->button & IN_ATTACK )
	{
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usWindUp, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_iWeaponState = 1;
		//m_pPlayer->tfstate |= 0x800u;
		//m_pPlayer->TeamFortress_SetSpeed();
	}

	m_flNextPrimaryAttack = GetNextAttackDelay( 0.5f );
	m_flTimeWeaponIdle = 0.6f;
}

void CTFAssaultC::Fire()
{
	Vector p_vecSrc, vecAiming, vecSpread;

	if( m_flNextPrimaryAttack > 0.0f )
		return;

	PLAYBACK_EVENT_FULL( FEV_NOTHOST | FEV_UPDATE, ENT( m_pPlayer->pev ), m_usWindUp, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, 0.0f, 0, 0, m_pPlayer->ammo_shells & 1, 0 );
	m_pPlayer->m_iWeaponVolume = 600;
	m_pPlayer->m_iWeaponFlash = 256;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	p_vecSrc = gpGlobals->v_up * -4.0f + gpGlobals->v_right * 2.0f + m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs;
	vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	vecSpread = Vector( 0.1f, 0.1f, 0.0f );
	FireBullets( 5, p_vecSrc, vecAiming, vecSpread, 8192.0f, BULLET_PLAYER_TF_ASSAULT, 8, 7, NULL );
	m_pPlayer->ammo_shells--;
}
