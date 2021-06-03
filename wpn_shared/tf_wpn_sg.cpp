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

LINK_ENTITY_TO_CLASS( tf_weapon_shotgun, CTFShotgun )

void CTFShotgun::Spawn( void )
{
	Precache();
	m_iId = WEAPON_TF_SHOTGUN;
	m_iDefaultAmmo = 8;
	m_iMaxClipSize = 8;
	m_iShellsReloaded = 1;
	m_fReloadTime = 0.25f;
	m_flPumpTime = 1000.0f;
	pev->solid = SOLID_TRIGGER;
}

void CTFShotgun::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfc_12gauge.mdl" );
	PRECACHE_MODEL( "models/p_smallshotgun.mdl" );
	PRECACHE_MODEL( "models/p_shotgun2.mdl" );
	PRECACHE_SOUND( "weapons/dbarrel1.wav" );
	PRECACHE_SOUND( "weapons/sbarrel1.wav" );
	PRECACHE_SOUND( "weapons/reload1.wav" );
	PRECACHE_SOUND( "weapons/reload3.wav" );
	PRECACHE_SOUND( "weapons/357_cock1.wav" );
	PRECACHE_SOUND( "weapons/scock1.wav" );
	PRECACHE_SOUND( "weapons/shotgn2.wav" );
	m_iShell = PRECACHE_MODEL( "models/shotgunshell.mdl" );
	m_usFireShotgun = PRECACHE_EVENT( 1, "events/wpn/tf_sg.sc" );
	m_usReloadShotgun = PRECACHE_EVENT( 1, "events/wpn/tf_sgreload.sc" );
	m_usPumpShotgun = PRECACHE_EVENT( 1, "events/wpn/tf_sgpump.sc" );
}

int CTFShotgun::GetItemInfo( ItemInfo *p )
{
	p->iSlot = 1;
	p->iPosition = 3;
	p->pszAmmo1 = "buckshot";
	p->pszName = STRING( pev->classname );
	p->iAmmo1 = m_pPlayer ? m_pPlayer->maxammo_shells : 200;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = m_iMaxClipSize;
	p->iId = m_iId = WEAPON_TF_SHOTGUN;
	p->iFlags = 0;
	p->iWeight = 15;
	return 1;
}

BOOL CTFShotgun::Deploy( void )
{
	return DefaultDeploy( "models/v_tfc_12gauge.mdl", "models/p_smallshotgun.mdl", SHOTGUN_DRAW, "shotgun", 1 );
}

void CTFShotgun::PrimaryAttack( void )
{
	Vector p_vecSrc, p_vecDirShooting, p_vecSpread;

	if ( m_iClip <= 0 )
	{
		Reload();

		if ( m_iClip == 0 )
			PlayEmptySound();
	}
	else
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireShotgun, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		p_vecSrc = m_pPlayer->GetGunPosition();
		p_vecDirShooting = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
		p_vecSpread = Vector( 0.04f, 0.04f, 0.0f );
		m_pPlayer->FireBullets( 6, p_vecSrc, p_vecDirShooting, p_vecSpread, 2048.0f, BULLET_PLAYER_TF_BUCKSHOT, 0, 4, 0 );

		if ( !m_iClip && m_pPlayer->ammo_shells <= 0 )
			m_pPlayer->SetSuitUpdate( "!HEV_AMO0", false, SUIT_REPEAT_OK );

		m_iClip--;
		m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
		m_fInSpecialReload = 0;
		m_flTimeWeaponIdle = 5.0f;
		m_pPlayer->tfstate &= ~TFSTATE_RELOADING;
		m_flPumpTime = 0.5f;
		m_flNextPrimaryAttack = GetNextAttackDelay( 0.5f );
	}
}

void CTFShotgun::ItemPostFrame( void )
{
	if ( m_flPumpTime < 0.0f )
	{
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usPumpShotgun, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_flPumpTime = 1000.0f;
	}

	CBasePlayerWeapon::ItemPostFrame();
}

BOOL CTFShotgun::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
		WRITE_BYTE( m_iId );
		MESSAGE_END();

		current_ammo = &m_pPlayer->ammo_shells;

		return TRUE;
	}

	return FALSE;
}

void CTFShotgun::WeaponIdle( void )
{
	ResetEmptySound();

	if ( m_flTimeWeaponIdle < 0.0f )
	{
		if ( m_fInSpecialReload )
		{
			if ( m_iClip == m_iMaxClipSize || m_pPlayer->ammo_shells < m_iShellsReloaded )
			{
				PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usPumpShotgun, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 1, 0 );
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = 1.5f;
				return;
			}

			if ( m_iClip || m_pPlayer->ammo_shells >= m_iShellsReloaded )
			{
				Reload();
				return;
			}
		}

		int iRand = UTIL_SharedRandomLong( m_pPlayer->random_seed, 0, 20 );

		if ( iRand == 5 * ( iRand / 5 ) )
		{
			if ( iRand == 20 * ( iRand / 20 ) )
			{
				m_flTimeWeaponIdle = ( 20.0f / 9.0f );
				SendWeaponAnim( SHOTGUN_IDLE );
			}
			else
			{
				m_flTimeWeaponIdle = ( 20.0f / 9.5f );
				SendWeaponAnim( SHOTGUN_IDLE4 );
			}
		}
		else
		{
			m_flTimeWeaponIdle = 5.0f;
			SendWeaponAnim( SHOTGUN_IDLE_DEEP );
		}
	}
}

void CTFShotgun::Reload( void )
{
	if ( m_pPlayer->ammo_shells >= m_iShellsReloaded )
	{
		if ( m_iClip != m_iMaxClipSize && m_flNextReload <= 0.0f && m_flNextPrimaryAttack <= 0.0f )
		{
			if ( m_fInSpecialReload )
			{
				if ( m_fInSpecialReload == 1 )
				{
					if ( m_flTimeWeaponIdle <= 0.0f )
					{
						m_fInSpecialReload = 2;
						PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usReloadShotgun, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
						m_flNextReload = m_fReloadTime;
						m_flTimeWeaponIdle = m_fReloadTime;
					}
				}
				else
				{
					m_iClip += m_iShellsReloaded;
					m_pPlayer->ammo_shells -= m_iShellsReloaded;
					m_fInSpecialReload = 1;
					m_pPlayer->tfstate &= ~TFSTATE_RELOADING;
				}
			}
			else
			{
				SendWeaponAnim( SHOTGUN_START_RELOAD );
				m_pPlayer->tfstate |= TFSTATE_RELOADING;
				m_fInSpecialReload = 1;
				m_pPlayer->m_flNextAttack = 0.1f;
				m_flTimeWeaponIdle = 0.1f;
				m_flNextSecondaryAttack = 0.1f;
				m_flNextPrimaryAttack = GetNextAttackDelay( 0.1f );
			}
		}
	}
}

LINK_ENTITY_TO_CLASS( tf_weapon_supershotgun, CTFSuperShotgun )

void CTFSuperShotgun::Spawn( void )
{
	Precache();
	m_iId = WEAPON_SUPER_SHOTGUN;
	m_flPumpTime = 1000.0f;
	pev->solid = SOLID_TRIGGER;
	m_iDefaultAmmo = 16;
	m_iMaxClipSize = 16;
	m_fReloadTime = 0.375f;
	m_iShellsReloaded = 2;
}

void CTFSuperShotgun::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfc_shotgun.mdl" );
	PRECACHE_MODEL( "models/p_shotgun.mdl" );
	PRECACHE_MODEL( "models/p_shotgun2.mdl" );
	PRECACHE_SOUND( "weapons/dbarrel1.wav" );
	PRECACHE_SOUND( "weapons/sbarrel1.wav" );
	PRECACHE_SOUND( "weapons/reload1.wav" );
	PRECACHE_SOUND( "weapons/reload3.wav" );
	PRECACHE_SOUND( "weapons/357_cock1.wav" );
	PRECACHE_SOUND( "weapons/scock1.wav" );
	PRECACHE_SOUND( "weapons/shotgn2.wav" );
	m_iShell = PRECACHE_MODEL( "models/shotgunshell.mdl" );
	m_usFireSuperShotgun = PRECACHE_EVENT( 1, "events/wpn/tf_ssg.sc" );
	m_usReloadShotgun = PRECACHE_EVENT( 1, "events/wpn/tf_sgreload.sc" );
	m_usPumpShotgun = PRECACHE_EVENT( 1, "events/wpn/tf_sgpump.sc" );
}

int CTFSuperShotgun::GetItemInfo( ItemInfo *p )
{
	p->iSlot = 2;
	p->iPosition = 2;
	p->pszAmmo1 = "buckshot";
	p->pszName = STRING( pev->classname );
	p->iAmmo1 = m_pPlayer ? m_pPlayer->maxammo_shells : 200;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = m_iMaxClipSize;
	p->iId = m_iId = WEAPON_SUPER_SHOTGUN;
	p->iFlags = 0;
	p->iWeight = 15;
	return 1;
}

BOOL CTFSuperShotgun::Deploy( void )
{
	return DefaultDeploy( "models/v_tfc_shotgun.mdl", "models/p_shotgun.mdl", SHOTGUN_DRAW, "shotgun", 1 );
}

void CTFSuperShotgun::PrimaryAttack( void )
{
	Vector p_vecSrc, p_vecDirShooting, p_vecSpread;

	if ( m_iClip <= 0 )
	{
		Reload();

		if ( m_iClip == 0 )
			PlayEmptySound();
	}
	else if ( m_iClip == 1 )
	{
		CTFShotgun::PrimaryAttack();
	}
	else
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireSuperShotgun, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		p_vecSrc = m_pPlayer->GetGunPosition();
		p_vecDirShooting = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
		p_vecSpread = Vector( 0.04f, 0.04f, 0.0f );
		m_pPlayer->FireBullets( 14, p_vecSrc, p_vecDirShooting, p_vecSpread, 2048.0f, BULLET_PLAYER_TF_BUCKSHOT, 4, 4, 0 );

		if ( !m_iClip && m_pPlayer->ammo_shells <= 0 )
			m_pPlayer->SetSuitUpdate( "!HEV_AMO0", 0, SUIT_REPEAT_OK );

		m_iClip -= 2;
		m_fInSpecialReload = 0;
		m_flTimeWeaponIdle = 5.0f;
		m_pPlayer->tfstate &= ~TFSTATE_RELOADING;
		m_flPumpTime = 0.7f;
		m_flNextPrimaryAttack = GetNextAttackDelay( 0.7f );
	}
}
