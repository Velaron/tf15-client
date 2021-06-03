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

LINK_ENTITY_TO_CLASS( tf_weapon_axe, CTFAxe )

void CTFAxe::Spawn( void )
{
	Precache();
	m_iId = WEAPON_AXE;
	m_iClip = WEAPON_NOCLIP;
	pev->solid = SOLID_TRIGGER;
}

void CTFAxe::Precache( void )
{
	PRECACHE_MODEL( "models/v_umbrella.mdl" );
	PRECACHE_MODEL( "models/p_umbrella.mdl" );
	PRECACHE_MODEL( "models/v_tfc_crowbar.mdl" );
	PRECACHE_MODEL( "models/p_crowbar.mdl" );
	PRECACHE_MODEL( "models/p_crowbar2.mdl" );

	PRECACHE_SOUND( "weapons/cbar_hit1.wav" );
	PRECACHE_SOUND( "weapons/cbar_hit2.wav" );
	PRECACHE_SOUND( "weapons/cbar_hitbod1.wav" );
	PRECACHE_SOUND( "weapons/cbar_hitbod2.wav" );
	PRECACHE_SOUND( "weapons/cbar_hitbod3.wav" );
	PRECACHE_SOUND( "weapons/cbar_miss1.wav" );

	m_usAxe = PRECACHE_EVENT( 1, "events/wpn/tf_axe.sc" );
	m_usAxeDecal = PRECACHE_EVENT( 1, "events/wpn/tf_axedecal.sc" );

	classid = 3;
}

int CTFAxe::GetItemInfo( ItemInfo *p )
{
	p->iSlot = 0;
	p->iPosition = 3;
	p->pszAmmo1 = NULL;
	p->iAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->pszName = STRING( pev->classname );
	p->iMaxClip = WEAPON_NOCLIP;
	p->iId = m_iId = WEAPON_AXE;
	p->iFlags = 0;
	p->iWeight = 0;
	return 1;
}

void CTFAxe::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = 0.5f;
	SendWeaponAnim( CROWBAR_HOLSTER );
}

void CTFAxe::Smack( void )
{
	PlayDecal();
}

void CTFAxe::PlayAnim( int iAnimType )
{
}

void CTFAxe::PlaySound( int iSoundType, float fSoundData )
{
}

void CTFAxe::PlayDecal( void )
{
}

BOOL CTFAxe::AxeHit( CBaseEntity *pTarget, Vector p_vecDir, TraceResult *ptr )
{
	Vector vecTargetAngles;
	ClearMultiDamage();

	if ( m_pPlayer->pev->playerclass == PC_SPY && FClassnameIs( pTarget->pev, "player" ) )
	{
		UTIL_MakeVectors( pTarget->pev->v_angle );
		vecTargetAngles = gpGlobals->v_right;
		UTIL_MakeVectors( m_pPlayer->pev->v_angle );

		if ( vecTargetAngles.x * gpGlobals->v_forward.y - vecTargetAngles.y * gpGlobals->v_forward.x <= 0.0f )
		{
			pTarget->TraceAttack( m_pPlayer->pev, 40.0f, p_vecDir, ptr, DMG_CLUB );
		}
		else
		{
			if ( m_bHullHit )
			{
				pTarget->TraceAttack( m_pPlayer->pev, 120.0f, p_vecDir, ptr, DMG_CLUB );
			}
			else
			{
				pTarget->TraceAttack( m_pPlayer->pev, 60.0f, p_vecDir, ptr, DMG_CLUB );
			}
		}
	}
	else
	{
		pTarget->TraceAttack( m_pPlayer->pev, 20.0f, p_vecDir, ptr, DMG_CLUB );
	}

	ApplyMultiDamage( m_pPlayer->pev, m_pPlayer->pev );
	return TRUE;
}

void CTFAxe::PrimaryAttack( void )
{
	Vector vecSrc, vecEnd;
	TraceResult tr;
	CBaseEntity *pEntity;
	BOOL bHit;

	m_bHullHit = FALSE;
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	vecSrc = m_pPlayer->GetGunPosition();
	vecEnd = vecSrc + gpGlobals->v_forward * 32.0f;
	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );
	PLAYBACK_EVENT_FULL( FEV_NOTHOST, ENT( m_pPlayer->pev ), m_usAxe, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, classid, 0, 0, 0 );

	m_flTimeWeaponIdle = 5.0f;
	m_flNextPrimaryAttack = GetNextAttackDelay( 0.4f );
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	if ( tr.flFraction >= 1.0f )
	{
		m_bHullHit = TRUE;
		return;
	}

	pEntity = CBaseEntity::Instance( tr.pHit );
	bHit = AxeHit( pEntity, gpGlobals->v_forward, &tr );

	if ( bHit && pEntity && pEntity->Classify() && pEntity->Classify() != CLASS_MACHINE )
	{
		m_pPlayer->m_iWeaponVolume = 128;

		if ( !pEntity->IsAlive() )
			return;
	}

	m_trHit = tr;
	SetThink( &CTFAxe::Smack );
	pev->nextthink = gpGlobals->time + 0.2f;
	m_pPlayer->m_iWeaponVolume = 512;
}

BOOL CTFAxe::Deploy( void )
{
	if ( m_pPlayer->pev->playerclass == PC_CIVILIAN )
		return DefaultDeploy( "models/v_umbrella.mdl", "models/p_umbrella.mdl", CROWBAR_DRAW, "crowbar", 1 );
	else
		return DefaultDeploy( "models/v_tfc_crowbar.mdl", "models/p_crowbar.mdl", CROWBAR_DRAW, "crowbar", 1 );
}

LINK_ENTITY_TO_CLASS( tf_weapon_spanner, CTFSpanner )

void CTFSpanner::Spawn( void )
{
	Precache();
	m_iId = WEAPON_SPANNER;
	m_iClip = -1;
	pev->solid = SOLID_TRIGGER;
	classid = 1;
}

void CTFSpanner::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfc_spanner.mdl" );
	PRECACHE_MODEL( "models/p_spanner.mdl" );
	PRECACHE_MODEL( "models/p_spanner2.mdl" );
	m_usAxe = PRECACHE_EVENT( 1, "events/wpn/tf_axe.sc" );
	m_usAxeDecal = PRECACHE_EVENT( 1, "events/wpn/tf_axedecal.sc" );
}

void CTFSpanner::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = 0.5f;
	SendWeaponAnim( SPANNER_HOLSTER );
}

int CTFSpanner::GetItemInfo( ItemInfo *p )
{
	p->iSlot = 0;
	p->iPosition = 2;
	p->pszAmmo1 = "uranium";
	p->iAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->pszName = STRING( pev->classname );
	p->iMaxClip = -1;
	p->iId = m_iId = WEAPON_SPANNER;
	p->iFlags = 0;
	p->iWeight = 0;
	return 1;
}

BOOL CTFSpanner::Deploy( void )
{
	return DefaultDeploy( "models/v_tfc_spanner.mdl", "models/p_spanner.mdl", SPANNER_DRAW, "crowbar", 1 );
}

void CTFSpanner::PlayAnim( int iAnimType )
{
	SendWeaponAnim( iAnimType ? SPANNER_USE1 : SPANNER_ATTACK1 );
}

void CTFSpanner::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle <= 0.0f )
	{
		m_flTimeWeaponIdle = 12.5f;
		SendWeaponAnim( SPANNER_IDLE );
	}
}

BOOL CTFSpanner::AxeHit( CBaseEntity *pTarget, Vector p_vecDir, TraceResult *ptr )
{
	if ( pTarget->goal_activation & TFGA_SPANNER )
	{
		pTarget->do_triggerwork = 1;
		pTarget->Use( m_pPlayer, m_pPlayer, USE_TOGGLE, 8.0f );
		pTarget->do_triggerwork = 0;
		return TRUE;
	}
	else if ( m_pPlayer->IsAlly( pTarget ) )
	{
		return pTarget->EngineerUse( m_pPlayer ) != 0;
	}
	else
	{
		ClearMultiDamage();
		pTarget->TraceAttack( m_pPlayer->pev, 20.0f, p_vecDir, ptr, DMG_CLUB );
		ApplyMultiDamage( m_pPlayer->pev, m_pPlayer->pev );
		return TRUE;
	}
}

LINK_ENTITY_TO_CLASS( tf_weapon_knife, CTFKnife )

void CTFKnife::Spawn( void )
{
	Precache();
	m_iClip = -1;
	m_iId = WEAPON_KNIFE;
	pev->solid = SOLID_TRIGGER;
	classid = 0;
}

void CTFKnife::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfc_knife.mdl" );
	PRECACHE_MODEL( "models/p_knife.mdl" );
	PRECACHE_MODEL( "models/p_knife2.mdl" );
	m_usAxe = PRECACHE_EVENT( 1, "events/wpn/tf_axe.sc" );
	m_usAxeDecal = PRECACHE_EVENT( 1, "events/wpn/tf_axedecal.sc" );
	m_usKnife = PRECACHE_EVENT( 1, "events/wpn/tf_knife.sc" );
}

void CTFKnife::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = 0.5f;
	SendWeaponAnim( KNIFE_HOLSTER );
}

int CTFKnife::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = NULL;
	p->pszName = STRING( pev->classname );
	p->iAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iSlot = 0;
	p->iPosition = 4;
	p->iFlags = 0;
	p->iMaxClip = -1;
	p->iId = m_iId = WEAPON_KNIFE;
	p->iWeight = 0;
	return 1;
}

BOOL CTFKnife::Deploy( void )
{
	return DefaultDeploy( "models/v_tfc_knife.mdl", "models/p_knife.mdl", KNIFE_DRAW, "knife", 1 );
}

void CTFKnife::PlayAnim( int iAnimType )
{
	if ( iAnimType == 0 || iAnimType == 1 ) //Velaron: ANIM_HIT
	{
		SendWeaponAnim( KNIFE_ATTACK1 );
	}
}

void CTFKnife::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle <= 0.0f )
	{
		m_flTimeWeaponIdle = 7.5f;
		SendWeaponAnim( RANDOM_LONG( KNIFE_IDLE1, KNIFE_IDLE2 ) );
	}
}

LINK_ENTITY_TO_CLASS( tf_weapon_medikit, CTFMedikit )

void CTFMedikit::Spawn( void )
{
	Precache();
	m_iClip = -1;
	m_iId = WEAPON_MEDIKIT;
	pev->solid = SOLID_TRIGGER;
	classid = 2;
}

void CTFMedikit::Precache( void )
{
	PRECACHE_MODEL( "models/v_tfc_medkit.mdl" );
	PRECACHE_MODEL( "models/p_medkit.mdl" );
	PRECACHE_MODEL( "models/p_medkit2.mdl" );
	m_usAxe = PRECACHE_EVENT( 1, "events/wpn/tf_axe.sc" );
	m_usAxeDecal = PRECACHE_EVENT( 1, "events/wpn/tf_axedecal.sc" );
	m_usNormalShot = PRECACHE_EVENT( 1, "events/wpn/tf_mednormal.sc" );
	m_usSuperShot = PRECACHE_EVENT( 1, "events/wpn/tf_medsuper.sc" );
	m_usSteamShot = PRECACHE_EVENT( 1, "events/wpn/tf_medsteam.sc" );
}

void CTFMedikit::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = 0.5f;
	SendWeaponAnim( MEDIKIT_HOLSTER );
}

int CTFMedikit::GetItemInfo( ItemInfo *p )
{
	p->pszAmmo1 = NULL;
	p->iAmmo1 = -1;
	p->pszName = STRING( pev->classname );
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = -1;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_MEDIKIT;
	p->iWeight = 0;
	return 1;
}

BOOL CTFMedikit::Deploy( void )
{
	return DefaultDeploy( "models/v_tfc_medkit.mdl", "models/p_medkit.mdl", MEDIKIT_DRAW, "medkit", 1 );
}

void CTFMedikit::PlayAnim( int iAnimType )
{
	SendWeaponAnim( iAnimType ? MEDIKIT_USE_LONG : MEDIKIT_USE_SHORT );
}

void CTFMedikit::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle <= 0.0f )
	{
		m_flTimeWeaponIdle = 7.5f;
		SendWeaponAnim( RANDOM_LONG( MEDIKIT_IDLE_SHORT, MEDIKIT_IDLE_LONG ) );
	}
}

BOOL CTFMedikit::AxeHit( CBaseEntity *pTarget, Vector *p_vecDir, TraceResult *ptr )
{
	//if
	return true;
}