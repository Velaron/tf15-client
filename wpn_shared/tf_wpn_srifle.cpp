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

LINK_ENTITY_TO_CLASS( tf_weapon_sniperrifle, CTFSniperRifle )

void CTFSniperRifle::Spawn( void )
{
	m_fAimedDamage = 0.0f;
	Precache();
	m_iId = WEAPON_SNIPER_RIFLE;
	m_iDefaultAmmo = 5;
	m_fNextAimBonus = -1.0f;
	pev->solid = SOLID_TRIGGER;
}

void CTFSniperRifle::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfc_sniper.mdl" );
	PRECACHE_MODEL( "models/p_sniper.mdl" );
	PRECACHE_MODEL( "models/p_sniper2.mdl" );
	PRECACHE_SOUND( "ambience/rifle1.wav" );
	m_usFireSniper = PRECACHE_EVENT( 1, "events/wpn/tf_sniper.sc" );
	m_usSniperHit = PRECACHE_EVENT( 1, "events/wpn/tf_sniperhit.sc" );
}

int CTFSniperRifle::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = "buckshot";
	p->pszName = STRING( pev->classname );
	if ( m_pPlayer )
		p->iAmmo1 = m_pPlayer->maxammo_shells;
	else
		p->iAmmo1 = 75;
	p->pszAmmo2 = 0;
	p->iAmmo2 = -1;
	p->iMaxClip = -1;
	p->iSlot = 1;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_SNIPER_RIFLE;
	p->iFlags = 0;
	p->iWeight = 10;
	return 1;
}

void CTFSniperRifle::SecondaryAttack( void )
{
	if ( m_fInZoom )
	{
		m_pPlayer->m_iFOV = 0;
		m_pPlayer->pev->fov = 0.0f;
		m_fInZoom = false;
	}
	else
	{
		m_pPlayer->m_iFOV = 20;
		m_pPlayer->pev->fov = 20.0f;
		m_fInZoom = true;
	}

	pev->nextthink = gpGlobals->time + 0.1f;
	m_flNextSecondaryAttack = 0.3f;
}

void CTFSniperRifle::Holster( int skiplocal )
{
	m_fInReload = 0;

	if ( m_fInZoom )
		SecondaryAttack();

	m_pPlayer->tfstate &= ~TFSTATE_AIMING;
	m_pPlayer->TeamFortress_SetSpeed();

	if ( m_pSpot )
	{
		m_pSpot->Killed( NULL, GIB_NORMAL );
		m_pSpot = NULL;
	}

	m_fAimedDamage = 0.0f;
	m_pPlayer->m_flNextAttack = 0.5f;
	SendWeaponAnim( SRIFLE_HOLSTER, 1 );
}

void CTFSniperRifle::Reload( void )
{

}

BOOL CTFSniperRifle::Deploy( void )
{
	if ( DefaultDeploy( "models/v_tfc_sniper.mdl", "models/p_sniper.mdl", SRIFLE_DRAW, "autosniper", 1 ) )
	{
		m_pSpot = NULL;
		m_flTimeWeaponIdle = 0.5f;
		m_fAimedDamage = 0.0f;
		m_iSpotActive = false;
		return TRUE;
	}

	return FALSE;
}

int CTFSniperRifle::AddToPlayer( CBasePlayer *pPlayer )
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

void CTFSniperRifle::UpdateSpot( void )
{
	Vector vecSrc, vecEnd;
	TraceResult tr;

	if ( m_iSpotActive )
	{
		if ( !m_pSpot )
		{
			m_pSpot = CLaserSpot::CreateSpot();
			m_pSpot->pev->flags |= FL_SKIPLOCALHOST;
			m_pSpot->pev->owner = ENT( m_pPlayer->pev );
		}

		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		vecSrc = m_pPlayer->GetGunPosition();
		vecEnd = vecSrc + gpGlobals->v_forward * 8192.0f;
		UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );
		UTIL_SetOrigin( m_pSpot->pev, tr.vecEndPos );
		m_pSpot->pev->renderamt = 0.25f * m_fAimedDamage + 150.0f;
	}
}

void CTFSniperRifle::WeaponIdle( void )
{
	if ( m_iSpotActive )
		UpdateSpot();

	if ( m_flTimeWeaponIdle < 0.0f && !( m_pPlayer->tfstate & TFSTATE_AIMING ) )
	{
		SendWeaponAnim( SRIFLE_IDLE, 1 );
		m_flTimeWeaponIdle = 12.5f;
	}
}

void CTFSniperRifle::PrimaryAttack( void )
{
	Vector vecSrc, vecEnd;
	Vector anglesAim;
	TraceResult tr;
	edict_t *pent;
	CBaseEntity *pEntity;

	if ( m_pPlayer->ammo_shells <= 0 )
		PlayEmptySound();
	else
	{
		m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usFireSniper, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, m_fAimedDamage, 0, 0, 0 );
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		m_flTimeWeaponIdle = 0.5f;
		anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
		UTIL_MakeVectors( anglesAim );
		vecSrc = m_pPlayer->GetGunPosition() - gpGlobals->v_up * 2.0f;
		vecEnd = vecSrc + gpGlobals->v_forward * 8192.0f;
		UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

		if ( tr.flFraction == 1.0f )
			PLAYBACK_EVENT_FULL( FEV_SERVER | FEV_HOSTONLY, ENT( m_pPlayer->pev ), m_usSniperHit, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 25, 0, 0, 0 );
		else
		{
			pent = ENT( 0 );

			if ( tr.pHit || pent )
			{
				pEntity = CBaseEntity::Instance( pent );

				if ( pEntity )
				{
					ClearMultiDamage();
					pEntity->TraceAttack( m_pPlayer->pev, m_fAimedDamage, gpGlobals->v_forward, &tr, DMG_BULLET );

					if ( m_pPlayer->deathtype )
						deathtype = m_pPlayer->deathtype;

					ApplyMultiDamage( pev, m_pPlayer->pev );
					m_pPlayer->deathtype = 0;
					//STRING( pEntity->pev->classname );
					//Velaron: TODO
				}
			}
		}

	}
}

void CTFSniperRifle::ItemPostFrame()
{
	ItemInfo info;

	GetItemInfo( &info );

	if ( !( m_pPlayer->pev->button & IN_ATTACK2 ) || m_flNextSecondaryAttack > 0.0f )
	{
		if ( m_pPlayer->m_afButtonReleased & IN_ATTACK && m_flNextPrimaryAttack <= 0.0f )
		{
			if ( m_pPlayer->tfstate & TFSTATE_AIMING )
			{
				m_pPlayer->tfstate &= ~TFSTATE_AIMING;
				m_pPlayer->TeamFortress_SetSpeed();

				if ( m_pPlayer->pev->flags & FL_ONGROUND )
					PrimaryAttack();
				else
					ClientPrint( m_pPlayer->pev, HUD_PRINTCENTER, "#Sniper_cantfire" );

				m_iSpotActive = false;

				if ( m_pSpot )
					m_pSpot->Killed( NULL, GIB_NEVER );

				m_pSpot = NULL;
				m_fAimedDamage = 0.0f;
			}
		}
		else if ( m_pPlayer->pev->button & IN_ATTACK && m_flNextPrimaryAttack <= 0.0f )
		{
			if ( m_pPlayer->tfstate & TFSTATE_AIMING )
			{
				m_iSpotActive = true;
				if ( m_fAimedDamage < 400.0f )
				{
					m_fAimedDamage -= m_fNextAimBonus * 50.0f;
					m_fNextAimBonus = 0.0f;
				}
				UpdateSpot();
			}
			else if ( sqrt( m_pPlayer->pev->velocity.x * m_pPlayer->pev->velocity.x + m_pPlayer->pev->velocity.y * m_pPlayer->pev->velocity.y ) <= 50.0f )
			{
				m_iSpotActive = true;
				m_fAimedDamage = 50.0f;
				m_fNextAimBonus = 0.0f;
				m_pPlayer->tfstate |= TFSTATE_AIMING;
				m_pPlayer->TeamFortress_SetSpeed();
				SendWeaponAnim( SRIFLE_AIM, 1 );
				m_flTimeWeaponIdle = 1000.0f;
			}
		}
		else if ( !( m_pPlayer->pev->button & IN_ATTACK | IN_ATTACK2 ) )
		{
			m_fFireOnEmpty = false;
			if ( IsUseable() || m_flNextPrimaryAttack >= 0.0f )
			{
				if ( !m_iClip && !( info.iFlags & ITEM_FLAG_NOAUTORELOAD ) && m_flNextPrimaryAttack < 0.0f )
				{
					Reload();
					return;
				}
			}
			else if ( !( info.iFlags & ITEM_FLAG_NOAUTOSWITCHEMPTY ) && UTIL_GetNextBestWeapon( m_pPlayer, this ) )
			{
				m_flNextPrimaryAttack = GetNextAttackDelay( 0.3f );
				return;
			}

			WeaponIdle();
			return;
		}

		if ( !ShouldWeaponIdle() )
			return;

		WeaponIdle();
		return;
	}

	if ( info.pszAmmo2 )
	{
		if ( m_pPlayer->m_rgAmmo[SecondaryAmmoIndex()] )
		{
			m_fFireOnEmpty = true;
		}
	}

	SecondaryAttack();
	m_pPlayer->pev->button &= ~IN_ATTACK2;

	if ( ShouldWeaponIdle() )
	{
		WeaponIdle();
		return;
	}
}
