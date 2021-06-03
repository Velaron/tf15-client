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

LINK_ENTITY_TO_CLASS( tf_weapon_gl, CTFGrenadeLauncher )

void CTFGrenadeLauncher::Spawn( void )
{
	Precache();
	m_iId = WEAPON_GRENADE_LAUNCHER;
	m_iAnim_Deploy = GL_DRAW;
	m_iAnim_Holster = GL_HOLSTER;
	m_iAnim_Idle = GL_IDLE;
	m_iAnim_ReloadDown = GL_RELOAD_DN;
	m_iAnim_ReloadUp = GL_RELOAD_UP;
	m_iDefaultAmmo = 50;
	m_fReloadTime = ( 2.0f / 3.0f );
	pev->solid = SOLID_TRIGGER;
}

void CTFGrenadeLauncher::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfgl.mdl" );
	PRECACHE_MODEL( "models/p_glauncher.mdl" );
	PRECACHE_MODEL( "models/p_glauncher2.mdl" );
	PRECACHE_MODEL( "models/pipebomb.mdl" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );
	PRECACHE_SOUND( "items/clipinsert1.wav" );
	PRECACHE_SOUND( "items/cliprelease1.wav" );
	PRECACHE_SOUND( "weapons/hks1.wav" );
	PRECACHE_SOUND( "weapons/hks2.wav" );
	PRECACHE_SOUND( "weapons/hks3.wav" );
	PRECACHE_SOUND( "weapons/glauncher.wav" );
	PRECACHE_SOUND( "weapons/glauncher2.wav" );
	PRECACHE_SOUND( "weapons/357_cock1.wav" );
	m_usFireGL = PRECACHE_EVENT( 1, "events/wpn/tf_gl.sc" );
	PRECACHE_EVENT( 1, "events/wpn/tf_pipel.sc" );
}

int CTFGrenadeLauncher::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = "rockets";
	p->pszName = STRING( pev->classname );
	p->iAmmo1 = m_pPlayer ? m_pPlayer->maxammo_rockets : 50;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = 6;
	p->iSlot = 3;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_GRENADE_LAUNCHER;
	p->iFlags = 0;
	p->iWeight = 15;
	return 1;
}

void CTFGrenadeLauncher::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_iGLClip = m_iClip;
	m_pPlayer->tfstate &= ~TFSTATE_RELOADING;
	m_fInSpecialReload = 0;
	m_flTimeWeaponIdle = 1000.0f;
	m_pPlayer->m_flNextAttack = 0.5f;
	SendWeaponAnim( m_iAnim_Holster );
}

BOOL CTFGrenadeLauncher::Deploy( void )
{
	return DefaultDeploy( "models/v_tfgl.mdl", "models/p_glauncher.mdl", m_iAnim_Deploy, "mp5", 1 );
}

void CTFGrenadeLauncher::Reload( void )
{
	if ( m_pPlayer->ammo_rockets )
	{
		if ( m_iClip != 6 && m_flNextReload <= 0.0 && m_flNextPrimaryAttack <= 0.0f )
		{
			if ( m_fInSpecialReload )
			{
				if ( m_fInSpecialReload == 1 )
				{
					if ( m_flTimeWeaponIdle <= 0.0 )
					{
						m_fInSpecialReload = 2;
						m_flNextReload = m_fReloadTime;
						m_flTimeWeaponIdle = m_fReloadTime;
					}
				}
				else
				{
					m_iClip++;
					m_pPlayer->ammo_rockets--;
					m_fInSpecialReload = 1;
					m_pPlayer->tfstate &= ~TFSTATE_RELOADING;
				}
			}
			else
			{
				m_fInSpecialReload = 1;
				m_pPlayer->tfstate |= TFSTATE_RELOADING;
				SendWeaponAnim( m_iAnim_ReloadDown );
				m_pPlayer->m_flNextAttack = 0.1f;
				m_flTimeWeaponIdle = 0.1f;
				m_flNextSecondaryAttack = 0.1f;
				m_flNextPrimaryAttack = GetNextAttackDelay( 0.1f );
			}
		}
	}
}

void CTFGrenadeLauncher::WeaponIdle( void )
{
	ResetEmptySound();

	if ( m_flTimeWeaponIdle <= 0.0f )
	{
		if ( m_fInSpecialReload )
		{
			if ( m_iClip < 6 && m_pPlayer->ammo_rockets > 0 )
			{
				Reload();
				return;
			}

			SendWeaponAnim( m_iAnim_ReloadUp );
			m_fInSpecialReload = 0;
			m_flTimeWeaponIdle = 1.5f;
			return;
		}

		m_flTimeWeaponIdle = 3.0f;
		SendWeaponAnim( m_iAnim_Idle );
	}
}

BOOL CTFGrenadeLauncher::AddToPlayer( CBasePlayer *pPlayer )
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

void CTFGrenadeLauncher::PrimaryAttack( void )
{
	Vector p_VecOrigin, p_VecAngles;

	if ( m_iClip )
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireGL, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		p_VecOrigin = m_pPlayer->GetGunPosition() + gpGlobals->v_right * 8.0f + gpGlobals->v_up * -16.0f;
		p_VecAngles = m_pPlayer->pev->v_angle;
		// Velaron: TODO
		//CTFGrenade::CreateTFGrenade( &p_VecOrigin, &p_VecAngles, m_pPlayer, this );
		m_fInSpecialReload = 0;
		m_pPlayer->tfstate &= ~TFSTATE_RELOADING;
		m_flNextSecondaryAttack = 0.6f;
		m_iClip--;
		m_flNextPrimaryAttack = 0.6f;
		m_flTimeWeaponIdle = 0.6f;
	}
	else
	{
		SendWeaponAnim( m_iAnim_Idle );
		m_flNextPrimaryAttack = GetNextAttackDelay( 1.0f );
		PlayEmptySound();
		m_fInSpecialReload = 0;
		m_pPlayer->tfstate &= ~TFSTATE_RELOADING;
	}
}

LINK_ENTITY_TO_CLASS( tf_weapon_pl, CTFPipebombLauncher )

void CTFPipebombLauncher::Spawn( void )
{
	Precache();
	m_iId = WEAPON_PIPEBOMB_LAUNCHER;
	m_iDefaultAmmo = 50;
	m_fReloadTime = ( 2.0f / 3.0f );
	pev->solid = SOLID_TRIGGER;
	m_iAnim_Holster = PL_HOLSTER;
	m_iAnim_Idle = PL_IDLE;
	m_iAnim_ReloadDown = PL_RELOAD_DN;
	m_iAnim_ReloadUp = PL_RELOAD_UP;
	m_iAnim_Deploy = PL_DRAW;
	m_usFireGL = PRECACHE_EVENT( 1, "events/wpn/tf_pipel.sc" );
}

int CTFPipebombLauncher::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = "rockets";
	p->pszName = STRING( pev->classname );
	p->iAmmo1 = m_pPlayer ? m_pPlayer->maxammo_rockets : 50;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = 6;
	p->iSlot = 4;
	p->iPosition = 4;
	p->iId = m_iId = WEAPON_PIPEBOMB_LAUNCHER;
	p->iFlags = 0;
	p->iWeight = 15;
	return 1;
}

void CTFPipebombLauncher::PrimaryAttack( void )
{
	Vector p_VecOrigin, p_VecAngles;

	if ( m_iClip )
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireGL, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		p_VecOrigin = m_pPlayer->GetGunPosition() + gpGlobals->v_right * 8.0f + gpGlobals->v_up * -16.0f;
		p_VecAngles = m_pPlayer->pev->v_angle;
		// Velaron: TODO
		//CTFGrenade::CreateTFPipebomb( &p_VecOrigin, &p_VecAngles, m_pPlayer, this );
		m_fInSpecialReload = 0;
		m_pPlayer->tfstate &= ~TFSTATE_RELOADING;
		m_iClip--;
		m_flTimeWeaponIdle = 0.6f;
		m_flNextPrimaryAttack = GetNextAttackDelay( 0.6f );
	}
	else
	{
		Reload();

		if ( !m_pPlayer->ammo_rockets )
		{
			PlayEmptySound();
			SendWeaponAnim( m_iAnim_Idle );
			m_flNextPrimaryAttack = GetNextAttackDelay( 1.0f );
		}
	}
}
