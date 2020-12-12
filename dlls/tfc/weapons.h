/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef WEAPONS_H
#define WEAPONS_H

#include "effects.h"

class CBasePlayer;
extern int gmsgWeapPickup;

// constant items
#define ITEM_HEALTHKIT		1
#define ITEM_ANTIDOTE		2
#define ITEM_SECURITY		3
#define ITEM_BATTERY		4

#define WEAPON_CROWBAR		1

#define WEAPON_ALLWEAPONS		(~(1<<WEAPON_SUIT))

#define WEAPON_SUIT				31	// ?????

#define MAX_WEAPONS			32

#define MAX_NORMAL_BATTERY	100

// weapon weight factors (for auto-switching)   (-1 = noswitch)
#define CROWBAR_WEIGHT		0

// weapon clip/carry ammo capacities
#define URANIUM_MAX_CARRY		100
#define	_9MM_MAX_CARRY			250
#define _357_MAX_CARRY			36
#define BUCKSHOT_MAX_CARRY		125
#define BOLT_MAX_CARRY			50
#define ROCKET_MAX_CARRY		5
#define HANDGRENADE_MAX_CARRY	10
#define SATCHEL_MAX_CARRY		5
#define TRIPMINE_MAX_CARRY		5
#define SNARK_MAX_CARRY			15
#define HORNET_MAX_CARRY		8
#define M203_GRENADE_MAX_CARRY	10

// the maximum amount of ammo each weapon's clip can hold
#define WEAPON_NOCLIP			-1

// The amount of ammo given to a player by an ammo item.
#define AMMO_URANIUMBOX_GIVE	20
#define AMMO_GLOCKCLIP_GIVE		GLOCK_MAX_CLIP
#define AMMO_357BOX_GIVE		PYTHON_MAX_CLIP
#define AMMO_MP5CLIP_GIVE		MP5_MAX_CLIP
#define AMMO_CHAINBOX_GIVE		200
#define AMMO_M203BOX_GIVE		2
#define AMMO_BUCKSHOTBOX_GIVE	12
#define AMMO_CROSSBOWCLIP_GIVE	CROSSBOW_MAX_CLIP
#define AMMO_RPGCLIP_GIVE		RPG_MAX_CLIP
#define AMMO_URANIUMBOX_GIVE	20
#define AMMO_SNARKBOX_GIVE		5

enum {
	BULLET_NONE = 0,
	BULLET_PLAYER_9MM = 1,
	BULLET_PLAYER_MP5 = 2,
	BULLET_PLAYER_357 = 3,
	BULLET_PLAYER_BUCKSHOT = 4,
	BULLET_PLAYER_CROWBAR = 5,
	BULLET_PLAYER_TF_BUCKSHOT = 6,
	BULLET_PLAYER_TF_ASSAULT = 7,
	BULLET_MONSTER_9MM = 8,
	BULLET_MONSTER_MP5 = 9,
	BULLET_MONSTER_12MM = 10
};

#define ITEM_FLAG_SELECTONEMPTY		1
#define ITEM_FLAG_NOAUTORELOAD		2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY	4
#define ITEM_FLAG_LIMITINWORLD		8
#define ITEM_FLAG_EXHAUSTIBLE		16 // A player can totally exhaust their ammo supply and lose this weapon

#define WEAPON_IS_ONTARGET 0x40

typedef struct
{
	int iSlot;
	int iPosition;
	const char *pszAmmo1; // ammo 1 type
	int iAmmo1; // max ammo 1
	const char *pszAmmo2; // ammo 2 type
	int iAmmo2; // max ammo 2
	const char *pszName;
	int iMaxClip;
	int iId;
	int iFlags;
	int iWeight; // this value used to determine this weapon's importance in autoselection.
} ItemInfo;

typedef struct
{
	const char *pszName;
	int iId;
} AmmoInfo;

// Items that the player has in their inventory that they can use
class CBasePlayerItem : public CBaseAnimating
{
public:
	virtual void SetObjectCollisionBox( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	virtual int AddToPlayer( CBasePlayer *pPlayer );	// return TRUE if the item you want the item added to the player inventory
	virtual int AddDuplicate( CBasePlayerItem *pItem ) { return FALSE; }	// return TRUE if you want your duplicate removed from world
	void EXPORT DestroyItem( void );
	void EXPORT DefaultTouch( CBaseEntity *pOther );	// default weapon touch
	void EXPORT FallThink( void );// when an item is first spawned, this think is run to determine when the object has hit the ground.
	void EXPORT Materialize( void );// make a weapon visible and tangible
	void EXPORT AttemptToMaterialize( void );  // the weapon desires to become visible and tangible, if the game rules allow for it
	CBaseEntity *Respawn( void );// copy a weapon
	void FallInit( void );
	void CheckRespawn( void );
	virtual int GetItemInfo( ItemInfo *p ) { return 0; };	// returns 0 if struct not filled out
	virtual BOOL CanDeploy( void ) { return TRUE; };
	virtual BOOL Deploy( void ) { return TRUE; }; // returns is deploy was successful

	virtual BOOL CanHolster( void ) { return TRUE; };// can this weapon be put away right now?
	virtual void Holster( int skiplocal = 0 );
	virtual void UpdateItemInfo( void ) { return; };

	virtual void ItemPreFrame( void ) { return; }		// called each frame by the player PreThink
	virtual void ItemPostFrame( void ) { return; }		// called each frame by the player PostThink

	virtual void Drop( void );
	virtual void Kill( void );
	virtual void AttachToPlayer( CBasePlayer *pPlayer );

	virtual int PrimaryAmmoIndex() { return -1; };
	virtual int SecondaryAmmoIndex() { return -1; };

	virtual int UpdateClientData( CBasePlayer *pPlayer ) { return 0; }

	virtual CBasePlayerItem *GetWeaponPtr( void ) { return NULL; };

	static ItemInfo ItemInfoArray[MAX_WEAPONS];
	static AmmoInfo AmmoInfoArray[MAX_AMMO_SLOTS];

	CBasePlayer *m_pPlayer;
	CBasePlayerItem *m_pNext;
	int		m_iId;												// WEAPON_???

	virtual int iItemSlot( void ) { return 0; }			// return 0 to MAX_ITEMS_SLOTS, used in hud

	int			iItemPosition( void ) { return ItemInfoArray[m_iId].iPosition; }
	const char *pszAmmo1( void ) { return ItemInfoArray[m_iId].pszAmmo1; }
	int			iMaxAmmo1( void ) { return ItemInfoArray[m_iId].iAmmo1; }
	const char *pszAmmo2( void ) { return ItemInfoArray[m_iId].pszAmmo2; }
	int			iMaxAmmo2( void ) { return ItemInfoArray[m_iId].iAmmo2; }
	const char *pszName( void ) { return ItemInfoArray[m_iId].pszName; }
	int			iMaxClip( void ) { return ItemInfoArray[m_iId].iMaxClip; }
	int			iWeight( void ) { return ItemInfoArray[m_iId].iWeight; }
	int			iFlags( void ) { return ItemInfoArray[m_iId].iFlags; }

	// int		m_iIdPrimary;										// Unique Id for primary ammo
	// int		m_iIdSecondary;										// Unique Id for secondary ammo
};

// inventory items that
class CBasePlayerWeapon : public CBasePlayerItem
{
public:
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	// generic weapon versions of CBasePlayerItem calls
	virtual int AddToPlayer( CBasePlayer *pPlayer );
	virtual int AddDuplicate( CBasePlayerItem *pItem );

	virtual int ExtractAmmo( CBasePlayerWeapon *pWeapon ); //{ return TRUE; };			// Return TRUE if you can add ammo to yourself when picked up
	virtual int ExtractClipAmmo( CBasePlayerWeapon *pWeapon );// { return TRUE; };			// Return TRUE if you can add ammo to yourself when picked up

	virtual int AddWeapon( void ) { ExtractAmmo( this ); return TRUE; };	// Return TRUE if you want to add yourself to the player

	// generic "shared" ammo handlers
	BOOL AddPrimaryAmmo( int iCount, char *szName, int iMaxClip, int iMaxCarry );
	BOOL AddSecondaryAmmo( int iCount, char *szName, int iMaxCarry );

	virtual void UpdateItemInfo( void ) {};	// updates HUD state

	int m_iPlayEmptySound;
	int m_fFireOnEmpty;		// True when the gun is empty and the player is still holding down the
							// attack key(s)
	virtual BOOL PlayEmptySound( void );
	virtual void ResetEmptySound( void );

	virtual void SendWeaponAnim( int iAnim, int skiplocal = 1, int body = 0 );  // skiplocal is 1 if client is predicting weapon animations

	virtual BOOL CanDeploy( void );
	virtual BOOL IsUseable( void );
	BOOL DefaultDeploy( const char *szViewModel, const char *szWeaponModel, int iAnim, const char *szAnimExt, int skiplocal = 0, int body = 0 );
	int DefaultReload( int iClipSize, int iAnim, float fDelay, int body = 0 );

	virtual void ItemPostFrame( void );	// called each frame by the player PostThink
	// called by CBasePlayerWeapons ItemPostFrame()
	virtual void PrimaryAttack( void ) { return; }				// do "+ATTACK"
	virtual void SecondaryAttack( void ) { return; }			// do "+ATTACK2"
	virtual void Reload( void ) { return; }						// do "+RELOAD"
	virtual void WeaponTick() {}								// Always called at beginning of ItemPostFrame. - Solokiller
	virtual void WeaponIdle( void ) { return; }					// called when no buttons pressed
	virtual int UpdateClientData( CBasePlayer *pPlayer );		// sends hud info to client dll, if things have changed
	virtual void RetireWeapon( void );
	virtual BOOL ShouldWeaponIdle( void ) { return FALSE; };
	virtual void Holster( int skiplocal = 0 );
	virtual BOOL UseDecrement( void ) { return FALSE; };

	int	PrimaryAmmoIndex();
	int	SecondaryAmmoIndex();

	void PrintState( void );

	virtual CBasePlayerItem *GetWeaponPtr( void ) { return (CBasePlayerItem *)this; };
	float GetNextAttackDelay( float delay );

	float	m_flNextPrimaryAttack;								// soonest time ItemPostFrame will call PrimaryAttack
	float	m_flNextSecondaryAttack;							// soonest time ItemPostFrame will call SecondaryAttack
	float	m_flTimeWeaponIdle;									// soonest time ItemPostFrame will call WeaponIdle
	float	m_flNextReload;
	float	m_flPumpTime;
	float	m_fReloadTime;
	float 	m_fAimedDamage;
	float 	m_fNextAimBonus;
	float 	m_fInZoom;
	int 	m_iWeaponState;
	int		m_fInReload;										// Are we in the middle of a reload;
	int		m_fInSpecialReload;									// Are we in the middle of a reload for the shotguns
	int		m_iPrimaryAmmoType;									// "primary" ammo index into players m_rgAmmo[]
	int		m_iSecondaryAmmoType;								// "secondary" ammo index into players m_rgAmmo[]
	int		m_iClip;											// number of shots left in the primary weapon clip, -1 it not used
	int		m_iClientClip;										// the last version of m_iClip sent to hud dll
	int		m_iClientWeaponState;								// the last version of the weapon state sent to hud dll (is current weapon, is on target)
	int		m_iDefaultAmmo;										// how much ammo you get when you pick up this weapon as placed by a level designer.

	// hle time creep vars
	float	m_flPrevPrimaryAttack;
	float	m_flLastFireTime;
};

class CBasePlayerAmmo : public CBaseEntity
{
public:
	virtual void Spawn( void );
	void EXPORT DefaultTouch( CBaseEntity *pOther ); // default weapon touch
	virtual BOOL AddAmmo( CBaseEntity *pOther ) { return TRUE; };

	CBaseEntity *Respawn( void );
	void EXPORT Materialize( void );
};

extern DLL_GLOBAL	short	g_sModelIndexLaser;// holds the index for the laser beam
extern DLL_GLOBAL	const char *g_pModelNameLaser;

extern DLL_GLOBAL	short	g_sModelIndexLaserDot;// holds the index for the laser beam dot
extern DLL_GLOBAL	short	g_sModelIndexFireball;// holds the index for the fireball
extern DLL_GLOBAL	short	g_sModelIndexSmoke;// holds the index for the smoke cloud
extern DLL_GLOBAL	short	g_sModelIndexWExplosion;// holds the index for the underwater explosion
extern DLL_GLOBAL	short	g_sModelIndexBubbles;// holds the index for the bubbles model
extern DLL_GLOBAL	short	g_sModelIndexBloodDrop;// holds the sprite index for blood drops
extern DLL_GLOBAL	short	g_sModelIndexBloodSpray;// holds the sprite index for blood spray (bigger)

extern void ClearMultiDamage( void );
extern void ApplyMultiDamage( entvars_t *pevInflictor, entvars_t *pevAttacker );
extern void AddMultiDamage( entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType );

extern void DecalGunshot( TraceResult *pTrace, int iBulletType );
extern void SpawnBlood( Vector vecSpot, int bloodColor, float flDamage );
extern int DamageDecal( CBaseEntity *pEntity, int bitsDamageType );
extern void RadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType );

typedef struct
{
	CBaseEntity *pEntity;
	float			amount;
	int				type;
} MULTIDAMAGE;

extern MULTIDAMAGE gMultiDamage;

#define LOUD_GUN_VOLUME			1000
#define NORMAL_GUN_VOLUME		600
#define QUIET_GUN_VOLUME		200

#define	BRIGHT_GUN_FLASH		512
#define NORMAL_GUN_FLASH		256
#define	DIM_GUN_FLASH			128

#define BIG_EXPLOSION_VOLUME	2048
#define NORMAL_EXPLOSION_VOLUME	1024
#define SMALL_EXPLOSION_VOLUME	512

#define	WEAPON_ACTIVITY_VOLUME	64

#define VECTOR_CONE_1DEGREES	Vector( 0.00873, 0.00873, 0.00873 )
#define VECTOR_CONE_2DEGREES	Vector( 0.01745, 0.01745, 0.01745 )
#define VECTOR_CONE_3DEGREES	Vector( 0.02618, 0.02618, 0.02618 )
#define VECTOR_CONE_4DEGREES	Vector( 0.03490, 0.03490, 0.03490 )
#define VECTOR_CONE_5DEGREES	Vector( 0.04362, 0.04362, 0.04362 )
#define VECTOR_CONE_6DEGREES	Vector( 0.05234, 0.05234, 0.05234 )
#define VECTOR_CONE_7DEGREES	Vector( 0.06105, 0.06105, 0.06105 )
#define VECTOR_CONE_8DEGREES	Vector( 0.06976, 0.06976, 0.06976 )
#define VECTOR_CONE_9DEGREES	Vector( 0.07846, 0.07846, 0.07846 )
#define VECTOR_CONE_10DEGREES	Vector( 0.08716, 0.08716, 0.08716 )
#define VECTOR_CONE_15DEGREES	Vector( 0.13053, 0.13053, 0.13053 )
#define VECTOR_CONE_20DEGREES	Vector( 0.17365, 0.17365, 0.17365 )

//=========================================================
// CWeaponBox - a single entity that can store weapons
// and ammo.
//=========================================================
class CWeaponBox : public CBaseEntity
{
	void Precache( void );
	void Spawn( void );
	void Touch( CBaseEntity *pOther );
	void KeyValue( KeyValueData *pkvd );
	BOOL IsEmpty( void );
	int  GiveAmmo( int iCount, const char *szName, int iMax, int *pIndex = NULL );
	void SetObjectCollisionBox( void );

public:
	void EXPORT Kill( void );
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	BOOL HasWeapon( CBasePlayerItem *pCheckItem );
	BOOL PackWeapon( CBasePlayerItem *pWeapon );
	BOOL PackAmmo( int iszName, int iCount );

	CBasePlayerItem *m_rgpPlayerItems[MAX_ITEM_TYPES];// one slot for each

	int m_rgiszAmmo[MAX_AMMO_SLOTS];// ammo names
	int	m_rgAmmo[MAX_AMMO_SLOTS];// ammo quantities

	int m_cAmmoTypes;// how many ammo types packed into this box (if packed by a level designer)
};

#ifdef CLIENT_DLL
bool bIsMultiplayer( void );
void LoadVModel( const char *szViewModel, CBasePlayer *m_pPlayer );
#endif

class CGrenade : public CBaseMonster
{
public:
	void Spawn( void );

	typedef enum { SATCHEL_DETONATE = 0, SATCHEL_RELEASE } SATCHELCODE;

	static CGrenade *ShootTimed( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time );
	static CGrenade *ShootContact( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	static CGrenade *ShootSatchelCharge( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	static void UseSatchelCharges( entvars_t *pevOwner, SATCHELCODE code );

	void Explode( Vector vecSrc, Vector vecAim );
	void Explode( TraceResult *pTrace, int bitsDamageType );
	void EXPORT Smoke( void );

	void EXPORT BounceTouch( CBaseEntity *pOther );
	void EXPORT SlideTouch( CBaseEntity *pOther );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void EXPORT DangerSoundThink( void );
	void EXPORT PreDetonate( void );
	void EXPORT Detonate( void );
	void EXPORT DetonateUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT TumbleThink( void );

	virtual void BounceSound( void );
	virtual int	BloodColor( void ) { return DONT_BLEED; }
	virtual void Killed( entvars_t *pevInflictor, entvars_t *pevAttacker, int iGib );

	BOOL m_fRegisteredSound;// whether or not this grenade has issued its DANGER sound to the world sound list yet.
};

class CCrowbar : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 1; }
	void EXPORT SwingAgain( void );
	void EXPORT Smack( void );
	int GetItemInfo( ItemInfo *p );
	void PrimaryAttack( void );
	int Swing( int fFirst );
	BOOL Deploy( void );
	void Holster( int skiplocal = 0 );

	int m_iSwing;
	TraceResult m_trHit;
};

class CLaserSpot : public CBaseEntity
{
	void Spawn( void );
	void Precache( void );
	int	ObjectCaps( void ) { return FCAP_DONT_SAVE; }

public:
	void Suspend( float flSuspendTime );
	void EXPORT Revive( void );
	static CLaserSpot *CreateSpot( void );
};

enum crowbar_e {
	CROWBAR_IDLE = 0,
	CROWBAR_DRAW = 1,
	CROWBAR_HOLSTER = 2,
	CROWBAR_ATTACK1HIT = 3,
	CROWBAR_ATTACK1MISS = 4,
	CROWBAR_ATTACK2MISS = 5,
	CROWBAR_ATTACK2HIT = 6,
	CROWBAR_ATTACK3MISS = 7,
	CROWBAR_ATTACK3HIT = 8
};

class CTFAxe : public CCrowbar
{
public:
	virtual void Spawn( void );
	virtual void Precache( void );
	void Smack( void );
	virtual BOOL AxeHit( CBaseEntity *pTarget, Vector p_vecDir, TraceResult *ptr );
	virtual int GetItemInfo( ItemInfo *p );
	virtual void PlayAnim( int iAnimType );
	virtual void PlaySound( int iSoundType, float fSoundData );
	virtual void PlayDecal( void );
	virtual void PrimaryAttack( void );
	virtual BOOL Deploy( void );
	virtual void Holster( int skiplocal = 0 );
	virtual BOOL IsUseable( void ) { return TRUE; }

	BOOL m_bHullHit;
	unsigned short m_usAxe;
	unsigned short m_usAxeDecal;
	int classid;
};

enum knife_e {
	KNIFE_IDLE1 = 0,
	KNIFE_IDLE2 = 1,
	KNIFE_ATTACK1 = 2,
	KNIFE_ATTACK2 = 3,
	KNIFE_DRAW = 4,
	KNIFE_HOLSTER = 5
};

class CTFKnife : public CTFAxe
{
public:
	void Spawn( void );
	void Precache( void );
	void Holster( void );
	int GetItemInfo( ItemInfo *p );
	BOOL Deploy( void );
	void WeaponIdle( void );

private:
	void PlayAnim( int iAnimType );
	unsigned short m_usKnife;
};

enum spanner_e {
	SPANNER_IDLE = 0,
	SPANNER_ATTACK1 = 1,
	SPANNER_ATTACK2 = 2,
	SPANNER_USE1 = 3,
	SPANNER_DRAW = 4,
	SPANNER_HOLSTER = 5
};

class CTFSpanner : public CTFAxe
{
public:
	void Spawn( void );
	void Precache( void );
	void Holster( void );
	int GetItemInfo( ItemInfo *p );
	BOOL Deploy( void );
	void WeaponIdle( void );
	BOOL AxeHit( CBaseEntity *pTarget, Vector p_vecDir, TraceResult *ptr );

private:
	void PlayAnim( int iAnimType );
};

enum medikit_e {
	MEDIKIT_IDLE_SHORT = 0,
	MEDIKIT_IDLE_LONG = 1,
	MEDIKIT_USE_SHORT = 2,
	MEDIKIT_USE_LONG = 3,
	MEDIKIT_HOLSTER = 4,
	MEDIKIT_DRAW = 5
};

class CTFMedikit : public CTFAxe
{
public:
	void Spawn( void );
	void Precache( void );
	void Holster( void );
	int GetItemInfo( ItemInfo *p );
	BOOL Deploy( void );
	void PlayAnim( int iAnimType );
	void WeaponIdle( void );
	BOOL AxeHit( CBaseEntity *pTarget, Vector *p_vecDir, TraceResult *ptr );

private:
	unsigned short m_usNormalShot;
	unsigned short m_usSuperShot;
	unsigned short m_usSteamShot;
};

enum srifle_e {
	SRIFLE_IDLE = 0,
	SRIFLE_AIM = 1,
	SRIFLE_FIRE = 2,
	SRIFLE_DRAW = 3,
	SRIFLE_HOLSTER = 4,
	ARIFLE_IDLE = 5,
	ARIFLE_FIRE = 6,
	ARIFLE_DRAW = 7,
	ARIFLE_HOLSTER = 8
};

class CTFSniperRifle : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 3; }
	int GetItemInfo( ItemInfo *p );
	void ItemPostFrame( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	int AddToPlayer( CBasePlayer *pPlayer );
	BOOL Deploy( void );
	void Holster( int skiplocal = 0 );
	void Reload( void );
	void WeaponIdle( void );
	void UpdateSpot( void );

	CLaserSpot *m_pSpot;
	BOOL m_iSpotActive;

private:
	unsigned short m_usFireSniper;
	unsigned short m_usSniperHit;
};

class CTFAutoRifle : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo( ItemInfo *p );
	void Holster( void );
	void PrimaryAttack( void );
	BOOL Deploy( void );
	void WeaponIdle( void );
	int AddToPlayer( CBasePlayer *pPlayer );
	int iItemSlot( void ) { return 3; };

private:
	unsigned short m_usFireAutoRifle;
};

enum shotgun_e {
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE = 1,
	SHOTGUN_FIRE2 = 2,
	SHOTGUN_RELOAD = 3,
	SHOTGUN_PUMP = 4,
	SHOTGUN_START_RELOAD = 5,
	SHOTGUN_DRAW = 6,
	SHOTGUN_HOLSTER = 7,
	SHOTGUN_IDLE4 = 8,
	SHOTGUN_IDLE_DEEP = 9
};

class CTFShotgun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo( ItemInfo *p );
	BOOL Deploy( void );
	void PrimaryAttack( void );
	void ItemPostFrame( void );
	int AddToPlayer( CBasePlayer *pPlayer );
	void WeaponIdle( void );
	void Reload( void );

	int m_iShell;
	int m_iMaxClipSize;
	int m_iShellsReloaded;
	unsigned short m_usReloadShotgun;
	unsigned short m_usPumpShotgun;

private:
	unsigned short m_usFireShotgun;
};

class CTFSuperShotgun : public CTFShotgun
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo( ItemInfo *p );
	BOOL Deploy( void );
	void PrimaryAttack( void );

private:
	unsigned short m_usFireSuperShotgun;
};

enum nailgun_e {
	NAILGUN_LONGIDLE = 0,
	NAILGUN_IDLE1 = 1,
	NAILGUN_LAUNCH = 2,
	NAILGUN_RELOAD = 3,
	NAILGUN_DEPLOY = 4,
	NAILGUN_FIRE1 = 5,
	NAILGUN_FIRE2 = 6,
	NAILGUN_FIRE3 = 7
};

class CTFNailgun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo( ItemInfo *p );
	void PrimaryAttack( void );
	BOOL Deploy( void );
	void WeaponIdle( void );
	int AddToPlayer( CBasePlayer *pPlayer );
	int iItemSlot( void ) { return 2; };

private:
	unsigned short m_usFireNailGun;
};

class CTFSuperNailgun : public CTFNailgun
{
public:
	int GetItemInfo( ItemInfo *p );
	void Precache( void );
	void PrimaryAttack( void );
	BOOL Deploy( void );
	int iItemSlot( void ) { return 4; };

private:
	unsigned short m_usFireSuperNailGun;
};

enum gl_e {
	GL_IDLE = 0,
	PL_IDLE = 1,
	GL_FIRE = 2,
	PL_FIRE = 3,
	GL_RELOAD_DN = 4,
	GL_RELOAD_UP = 5,
	PL_RELOAD_DN = 6,
	PL_RELOAD_UP = 7,
	GL_DRAW = 8,
	PL_DRAW = 9,
	GL_HOLSTER = 10,
	PL_HOLSTER = 11
};

class CTFGrenadeLauncher : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo( ItemInfo *p );
	void Holster( void );
	BOOL Deploy( void );
	void Reload( void );
	void WeaponIdle( void );
	int AddToPlayer( CBasePlayer *pPlayer );
	void PrimaryAttack( void );
	int iItemSlot( void ) { return 3; };

	int m_iAnim_Holster;
	int m_iAnim_Idle;
	int m_iAnim_ReloadDown;
	int m_iAnim_ReloadUp;
	int m_iAnim_Deploy;
	unsigned short m_usFireGL;
};

class CTFPipebombLauncher : public CTFGrenadeLauncher
{
public:
	void Spawn( void );
	int GetItemInfo( ItemInfo *p );
	void PrimaryAttack( void );
	int iItemSlot( void ) { return 3; };
};

enum flamethrower_e {
	FLAME_IDLE = 0,
	FLAME_FIDGET = 1,
	FLAME_NONE1 = 2,
	FLAME_NONE2 = 3,
	FLAME_NONE3 = 4,
	FLAME_FIRE1 = 5,
	FLAME_FIRE2 = 6,
	FLAME_FIRE3 = 7,
	FLAME_FIRE4 = 8,
	FLAME_DRAW = 9,
	FLAME_HOLSTER = 10
};

class CTFFlamethrower : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	int GetItemInfo( ItemInfo *p );
	BOOL CanHolster( void ) { return TRUE; };
	void Holster( void );
	void PrimaryAttack( void );
	BOOL Deploy( void );
	void Precache( void );
	void WeaponIdle( void );
	int AddToPlayer( CBasePlayer *pPlayer );
	int iItemSlot( void ) { return 5; };

private:
	unsigned short m_usFireFlame;
};

enum rpg_e {
	RPG_IDLE = 0,
	RPG_FIDGET = 1,
	RPG_FIRE = 2,
	RPG_HOLSTER = 3,
	RPG_DRAW = 4,
	RPG_HOLSTER_UL = 5,
	RPG_DRAW_UL = 6,
	RPG_RELOAD_START = 7,
	RPG_RELOAD = 8,
	RPG_RELOAD_END = 9,
	RPG_IDLE_UL = 10,
	RPG_FIDGET_UL = 11
};

class CTFRpg : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	int GetItemInfo( ItemInfo *p );
	BOOL CanHolster( void ) { return 1; };
	void Holster( void );
	void Precache( void );
	void Reload( void );
	void WeaponIdle( void );
	BOOL Deploy( void );
	int AddToPlayer( CBasePlayer *pPlayer );
	void PrimaryAttack( void );
	int iItemSlot( void ) { return 5; };

private:
	unsigned short m_usFireRPG;
};

class CTFIncendiaryC : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	int GetItemInfo( ItemInfo *p );
	BOOL CanHolster( void ) { return 1; };
	void Holster( void );
	void Precache( void );
	void WeaponIdle( void );
	BOOL Deploy( void );
	int AddToPlayer( CBasePlayer *pPlayer );
	void PrimaryAttack( void );
	int iItemSlot( void ) { return 4; };

private:
	unsigned short m_usFireIC;
};

enum ac_e {
	AC_IDLE1 = 0,
	AC_IDLE2 = 1,
	AC_SPINUP = 2,
	AC_SPINDOWN = 3,
	AC_FIRE = 4,
	AC_DEPLOY = 5,
	AC_HOLSTER = 6
};

class CTFAssaultC : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo( ItemInfo *p );
	BOOL Deploy( void );
	int AddToPlayer( CBasePlayer *pPlayer );
	void Holster( void );
	void WeaponIdle( void );
	void Fire( void );
	void PrimaryAttack( void );
	void StartSpin( void );
	void Spin( void );
	void WindUp( void );
	void WindDown( bool bFromHolster );
	int iItemSlot( void ) { return 3; };
	BOOL ShouldWeaponIdle( void ) { return true; };

private:
	int m_iShell;
	unsigned short m_usWindUp;
	unsigned short m_usWindDown;
	unsigned short m_usFire;
	unsigned short m_usStartSpin;
	unsigned short m_usSpin;
	unsigned short m_usACStart;
};

enum railgun_e {
	RAIL_IDLE = 0,
	RAIL_FIRE = 1,
	RAIL_DRAW = 2,
	RAIL_HOLSTER = 3
};

class CTFRailgun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo( ItemInfo *p );
	BOOL Deploy( void );
	void WeaponIdle( void );
	void PrimaryAttack( void );
	int iItemSlot( void ) { return 2; };

private:
	unsigned short m_usFireRail;
};

enum tranq_e {
	TRANQ_IDLE1 = 0,
	TRANQ_IDLE2 = 1,
	TRANQ_IDLE3 = 2,
	TRANQ_SHOOT = 3,
	TRANQ_SHOOT_EMPTY = 4,
	TRANQ_RELOAD = 5,
	TRANQ_RELOAD_NOT_EMPTY = 6,
	TRANQ_DRAW = 7,
	TRANQ_HOLSTER = 8,
	TRANQ_ADD_SILENCER = 9
};

class CTFTranq : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int GetItemInfo( ItemInfo *p );
	void WeaponIdle( void );
	BOOL Deploy( void );
	void PrimaryAttack( void );
	virtual int iItemSlot( void ) { return 2; }

private:
	unsigned short m_usFireTranquilizer;
};

#endif // WEAPONS_H
