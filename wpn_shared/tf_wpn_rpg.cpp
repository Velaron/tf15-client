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

LINK_ENTITY_TO_CLASS( tf_weapon_rpg, CTFRpg )

void CTFRpg::Spawn( void )
{
	Precache();
	m_iId = WEAPON_ROCKET_LAUNCHER;
	SET_MODEL( ENT( pev ), "models/w_rpg.mdl" );
	m_iDefaultAmmo = 50;
	m_fReloadTime = 1.25f;
	pev->solid = SOLID_TRIGGER;
}

int CTFRpg::GetItemInfo( ItemInfo* p )
{
	p->pszAmmo1 = "rockets";
	p->pszName = STRING( pev->classname );
	if ( m_pPlayer )
		p->iAmmo1 = m_pPlayer->maxammo_rockets;
	else
		p->iAmmo1 = 50;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = 4;
	p->iSlot = 4;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_ROCKET_LAUNCHER;
	p->iFlags = 0;
	p->iWeight = 20;
	return 1;
}

void CTFRpg::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->tfstate &= ~TFSTATE_RELOADING;
	m_fInSpecialReload = 0;
	m_pPlayer->m_flNextAttack = 0.5f;
	SendWeaponAnim( RPG_HOLSTER );
}

void CTFRpg::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfc_rpg.mdl" );
	PRECACHE_MODEL( "models/p_srpg.mdl" );
	PRECACHE_MODEL( "models/p_rpg2.mdl" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );
	PRECACHE_SOUND( "weapons/rocketfire1.wav" );
	PRECACHE_SOUND( "weapons/glauncher.wav" );
	UTIL_PrecacheOther( "tf_rpg_rocket" );
	m_usFireRPG = PRECACHE_EVENT( 1, "events/wpn/tf_rpg.sc" );
}

void CTFRpg::Reload( void )
{
	if ( m_pPlayer->ammo_rockets )
	{
		if ( m_iClip != 4 && m_flNextReload <= 0.0f && m_flNextPrimaryAttack <= 0.0f )
		{
			if ( m_fInSpecialReload )
			{
				if ( m_fInSpecialReload == 1 )
				{
					if ( m_flTimeWeaponIdle <= gpGlobals->time )
					{
						m_fInSpecialReload = 2;
						SendWeaponAnim( RPG_RELOAD );
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
				SendWeaponAnim( RPG_RELOAD_START );
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

void CTFRpg::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle <= 0.0f )
	{
		if ( m_iClip )
		{
			if ( !m_fInSpecialReload )
			{
				if ( m_pPlayer->current_ammo )
				{
					m_flTimeWeaponIdle = 3.0f;
					ResetEmptySound();
					SendWeaponAnim( m_iClip < 1 ? RPG_FIDGET_UL : RPG_FIDGET );
				}
				else
				{
					m_flTimeWeaponIdle = 1.0f;
				}
				return;
			}

			if ( m_iClip == 4 )
			{
				SendWeaponAnim( RPG_RELOAD_END );
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = 1.5f;
				return;
			}
		}
		else if ( !m_fInSpecialReload )
		{
			if ( m_pPlayer->ammo_rockets > 0 )
			{
				Reload();
				return;
			}

			if ( m_pPlayer->current_ammo )
			{
				m_flTimeWeaponIdle = 3.0f;
				ResetEmptySound();
				SendWeaponAnim( m_iClip < 1 ? RPG_FIDGET_UL : RPG_FIDGET );
			}
			else
			{
				m_flTimeWeaponIdle = 1.0f;
			}
			return;
		}

		if ( m_pPlayer->ammo_rockets > 0 )
		{
			Reload();
			return;
		}

		SendWeaponAnim( RPG_RELOAD_END );
		m_fInSpecialReload = 0;
		m_flTimeWeaponIdle = 1.5f;
		return;
	}
}

BOOL CTFRpg::Deploy( void )
{
	if ( m_iClip )
		return DefaultDeploy( "models/v_tfc_rpg.mdl", "models/p_srpg.mdl", RPG_DRAW, "rpg", 1 );
	else
		return DefaultDeploy( "models/v_tfc_rpg.mdl", "models/p_srpg.mdl", RPG_DRAW_UL, "rpg", 1 );
}

int CTFRpg::AddToPlayer( CBasePlayer* pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
		WRITE_BYTE( m_iId );
		MESSAGE_END();
		return true;
	}

	return false;
}

void CTFRpg::PrimaryAttack( void )
{
	Vector p_vecOrigin, p_vecAngles;

	if ( m_iClip )
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireRPG, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		p_vecOrigin = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16.0f + gpGlobals->v_right * 8.0f + gpGlobals->v_up * -8.0f;
		p_vecAngles = m_pPlayer->pev->v_angle;
		//CTFRpgRocket::CreateRpgRocket( &p_vecOrigin, &p_vecAngles, m_pPlayer, this );
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		m_fInSpecialReload = 0;
		m_pPlayer->tfstate &= ~TFSTATE_RELOADING;
		m_iClip--;
		m_flTimeWeaponIdle = 0.8f;
		m_flNextPrimaryAttack = GetNextAttackDelay( 0.8f );
		ResetEmptySound();
	}
	else
	{
		PlayEmptySound();
		Reload();
	}
}