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
#pragma once
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "pm_materials.h"

#define PLAYER_FATAL_FALL_SPEED      1024                                                                  // approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED   580                                                                   // approx 20 feet
#define DAMAGE_FOR_FALL_SPEED        (float)100 / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED ) // damage per unit per second.
#define PLAYER_MIN_BOUNCE_SPEED      200
#define PLAYER_FALL_PUNCH_THRESHHOLD (float)350 // won't punch player's screen/make scrape noise unless player falling at least this fast.

//
// Player PHYSICS FLAGS bits
//
#define PFLAG_ONLADDER   ( 1 << 0 )
#define PFLAG_ONSWING    ( 1 << 0 )
#define PFLAG_ONTRAIN    ( 1 << 1 )
#define PFLAG_ONBARNACLE ( 1 << 2 )
#define PFLAG_DUCKING    ( 1 << 3 ) // In the process of ducking, but totally squatted yet
#define PFLAG_USING      ( 1 << 4 ) // Using a continuous entity
#define PFLAG_OBSERVER   ( 1 << 5 ) // player is locked in stationary cam mode. Spectators can move, observers can't.

//
// generic player
//
//-----------------------------------------------------
//This is Half-Life player entity
//-----------------------------------------------------
#define CSUITPLAYLIST 4 // max of 4 suit sentences queued up at any time

#define SUIT_GROUP    TRUE
#define SUIT_SENTENCE FALSE

#define SUIT_REPEAT_OK     0
#define SUIT_NEXT_IN_30SEC 30
#define SUIT_NEXT_IN_1MIN  60
#define SUIT_NEXT_IN_5MIN  300
#define SUIT_NEXT_IN_10MIN 600
#define SUIT_NEXT_IN_30MIN 1800
#define SUIT_NEXT_IN_1HOUR 3600

#define CSUITNOREPEAT 32

#define SOUND_FLASHLIGHT_ON  "items/flashlight1.wav"
#define SOUND_FLASHLIGHT_OFF "items/flashlight1.wav"

#define TEAM_NAME_LENGTH 16

typedef enum
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1
} PLAYER_ANIM;

#define MAX_ID_RANGE     2048
#define SBAR_STRING_SIZE 128

enum sbar_data
{
	SBAR_ID_TARGETNAME = 1,
	SBAR_ID_TARGETHEALTH,
	SBAR_ID_TARGETARMOR,
	SBAR_ENG_BUILDINGAMMO,
	SBAR_TEAM1SCORE,
	SBAR_TEAM2SCORE,
	SBAR_END
};

#define CHAT_INTERVAL 1.0f

class CBasePlayer : public CBaseMonster
{
public:
	// Spectator camera
	void Observer_FindNextPlayer( bool bReverse );
	void Observer_HandleButtons();
	void Observer_SetMode( int iMode );
	void Observer_CheckTarget();
	void Observer_CheckProperties();

	EHANDLE m_hObserverTarget;
	float m_flNextObserverInput;
	int m_iObserverWeapon;   // weapon of current tracked target
	int m_iObserverLastMode; // last used observer mode

	int IsObserver() { return pev->iuser1; };

	int random_seed; // See that is shared between client & server for shared weapons code

	int m_iPlayerSound;     // the index of the sound list slot reserved for this player
	int m_iTargetVolume;    // ideal sound volume.
	int m_iWeaponVolume;    // how loud the player's weapon is right now.
	int m_iExtraSoundTypes; // additional classification for this weapon's sound
	int m_iWeaponFlash;     // brightness of the weapon flash
	float m_flStopExtraSoundTime;

	float m_flFlashLightTime; // Time until next battery draw/Recharge
	int m_iFlashBattery;      // Flashlight Battery Draw

	int m_afButtonLast;
	int m_afButtonPressed;
	int m_afButtonReleased;

	edict_t *m_pentSndLast; // last sound entity to modify player room type
	float m_flSndRoomtype;  // last roomtype set by sound entity
	float m_flSndRange;     // dist from player to sound entity

	float m_flFallVelocity;

	int m_rgItems[MAX_ITEMS];
	int m_fKnownItem; // True when a new item needs to be added
	int m_fNewAmmo;   // True when a new item has been added

	unsigned int m_afPhysicsFlags; // physics flags - set when 'normal' physics should be revisited or overriden
	float m_fNextSuicideTime;      // the time after which the player can next use the suicide command

	float m_fDontPackItemsUntil;
	float m_fNextTeamOrClassChange;

	// these are time-sensitive things that we keep track of
	float m_flTimeStepSound;  // when the last stepping sound was made
	float m_flTimeWeaponIdle; // when to play another weapon idle animation.
	float m_flSwimTime;       // how long player has been underwater
	float m_flDuckTime;       // how long we've been ducking
	float m_flWallJumpTime;   // how long until next walljump

	float m_flSuitUpdate;                        // when to play next suit update
	int m_rgSuitPlayList[CSUITPLAYLIST];         // next sentencenum to play for suit update
	int m_iSuitPlayNext;                         // next sentence slot for queue storage;
	int m_rgiSuitNoRepeat[CSUITNOREPEAT];        // suit sentence no repeat list
	float m_rgflSuitNoRepeatTime[CSUITNOREPEAT]; // how long to wait before allowing repeat
	int m_lastDamageAmount;                      // Last damage taken
	float m_tbdPrev;                             // Time-based damage timer

	float m_flgeigerRange; // range to nearest radiation source
	float m_flgeigerDelay; // delay per update of range msg to client
	int m_igeigerRangePrev;
	int m_iStepLeft;                        // alternate left/right foot stepping sound
	char m_szTextureName[CBTEXTURENAMEMAX]; // current texture name we're standing on
	char m_chTextureType;                   // current texture type

	int m_idrowndmg;      // track drowning damage taken
	int m_idrownrestored; // track drowning damage restored

	int m_bitsHUDDamage; // Damage bits for the current fame. These get sent to
	                     // the hude via the DAMAGE message
	BOOL m_fInitHUD;     // True when deferred HUD restart msg needs to be sent
	BOOL m_fGameHUDInitialized;
	int m_iTrain;   // Train control position
	BOOL m_fWeapon; // Set this to FALSE to force a reset of the current weapon HUD info

	EHANDLE m_pTank;   // the tank which the player is currently controlling,  NULL if no tank
	float m_fDeadTime; // the time at which the player died  (used in PlayerDeathThink())

	BOOL m_fNoPlayerSound; // a debugging feature. Player makes no sound if this is true.
	BOOL m_fLongJump;      // does this player have the longjump module?

	int m_iGLClip;

	float m_tSneaking;
	int m_iUpdateTime;    // stores the number of frame ticks before sending HUD update messages
	int m_iClientHealth;  // the health currently known by the client.  If this changes, send a new
	int m_iClientBattery; // the Battery currently known by the client.  If this changes, send a new
	int m_iHideHUD;       // the players hud weapon info is to be hidden
	int m_iClientHideHUD;
	int m_iFOV;       // field of view
	int m_iClientFOV; // client's known FOV

	// usable player items
	CBasePlayerItem *m_rgpPlayerItems[MAX_ITEM_TYPES];
	CBasePlayerItem *m_pActiveItem;
	CBasePlayerItem *m_pClientActiveItem; // client version of the active item
	CBasePlayerItem *m_pLastItem;

	const char *m_pszLastItem;

	// shared ammo slots
	int m_rgAmmo[MAX_AMMO_SLOTS];
	int m_rgAmmoLast[MAX_AMMO_SLOTS];

	Vector m_vecAutoAim;
	BOOL m_fOnTarget;
	int m_iDeaths;

	int m_iClientDeaths;
	int m_iClientFrags;
	int m_iClientPlayerClass;
	int m_iClientTeam;
	int m_iClientItems;

	float m_iRespawnFrames; // used in PlayerDeathThink() to make sure players can always respawn

	int m_iTimeCheckAllowed;

	int m_lastx, m_lasty; // These are the previous update's crosshair angles, DON"T SAVE/RESTORE

	int m_iConcussion;
	int m_iClientConcussion;
	int m_iConcStartVal;
	float m_flConcStartTime;
	float m_flConcDuration;

	int m_iBeingTeleported;

	int m_nCustomSprayFrames; // Custom clan logo frames for this player
	float m_flNextDecalTime;  // next time this player can spray a decal

	BOOL m_bDisplayedMOTD;

	BOOL m_bSentBuildingEvents;

	char m_MenuStringBuffer[512];
	int m_MenuSelectionBuffer;
	float m_MenuUpdateTime;

	char m_SbarString0[SBAR_STRING_SIZE];
	char m_SbarString1[SBAR_STRING_SIZE];
	char m_SbarString2[SBAR_STRING_SIZE];

	virtual void Spawn( void );
	virtual void PainSound( void );

	//virtual void Think( void );
	virtual void Jump( void );
	virtual void Duck( void );
	virtual void PreThink( void );
	virtual void PostThink( void );
	virtual int TakeHealth( float flHealth, int bitsDamageType );
	virtual void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType );
	virtual int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	virtual void Killed( entvars_t *pevInflictor, entvars_t *pevAttacker, int iGib );
	virtual Vector BodyTarget( const Vector &posSrc ) { return Center() + pev->view_ofs * RANDOM_FLOAT( 0.5, 1.1 ); }; // position to shoot at
	virtual void StartSneaking( void ) { m_tSneaking = gpGlobals->time - 1; }
	virtual void StopSneaking( void ) { m_tSneaking = gpGlobals->time + 30; }
	virtual BOOL IsSneaking( void ) { return m_tSneaking <= gpGlobals->time; }
	virtual BOOL IsAlive( void ) { return ( pev->deadflag == DEAD_NO ) && pev->health > 0; }
	virtual BOOL ShouldFadeOnDeath( void ) { return FALSE; }
	virtual BOOL IsPlayer( void ) { return TRUE; } // Spectators should return FALSE for this, they aren't "players" as far as game logic is concerned

	virtual BOOL IsNetClient( void ) { return TRUE; } // Bots should return FALSE for this, they can't receive NET messages
	                                                  // Spectators should return TRUE for this

	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	void RenewItems( void );
	void PackDeadPlayerItems( void );
	void RemoveAllItems( BOOL removeSuit );
	BOOL SwitchWeapon( CBasePlayerItem *pWeapon );
	BOOL SwitchWeapon( const char *szWeaponName );

	// JOHN:  sends custom messages if player HUD data has changed  (eg health, ammo)
	void UpdateClientData( void );
	void UpdateLowPriorityClientData( void );

	static TYPEDESCRIPTION m_playerSaveData[];

	// Player is moved across the transition by other means
	virtual int ObjectCaps( void ) { return CBaseMonster ::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual void Precache( void );
	BOOL IsOnLadder( void );
	BOOL FlashlightIsOn( void );
	void FlashlightTurnOn( void );
	void FlashlightTurnOff( void );

	void UpdatePlayerSound( void );
	virtual void DeathSound( void );

	virtual int Classify( void );
	void SetAnimation( PLAYER_ANIM playerAnim );
	void SetWeaponAnimType( const char *szExtention );

	char m_szAnimExtention[32];
	int m_iCurrentAnimationState;
	int m_iCurrentAnimationSequence;
	char m_szSavedAnimExtention[32];

	// custom player functions
	virtual void ImpulseCommands( void );
	void CheatImpulseCommands( int iImpulse );

	void StartDeathCam( void );
	void StartObserver( Vector vecPosition, Vector vecViewAngle );

	virtual int AddPlayerItem( CBasePlayerItem *pItem );
	virtual int RemovePlayerItem( CBasePlayerItem *pItem );
	void DropPlayerItem( char *pszItemName );
	BOOL HasPlayerItem( CBasePlayerItem *pCheckItem );
	BOOL HasNamedPlayerItem( const char *pszItemName );
	BOOL HasWeapons( void ); // do I have ANY weapons?
	void SelectPrevItem( int iItem );
	void SelectNextItem( int iItem );
	void SelectLastItem( void );
	void SelectItem( const char *pstr );
	void ItemPreFrame( void );
	void ItemPostFrame( void );
	void GiveNamedItem( const char *szName );
	void EnableControl( BOOL fControl );

	virtual int GiveAmmo( int iCount, const char *szName, int iMax, int *pIndex );
	BOOL GiveTFAmmo( int shells, int nails, int rockets, int cells );
	void SendAmmoUpdate( void );

	void WaterMove( void );
	void EXPORT PlayerDeathThink( void );
	void PlayerUse( void );

	void CheckSuitUpdate();
	void SetSuitUpdate( const char *name, int fgroup, int iNoRepeat );
	void UpdateGeigerCounter( void );
	void CheckTimeBasedDamage( void );

	void UpdateStepSound( void );
	void PlayStepSound( int step, float fvol );

	virtual BOOL FBecomeProne( void );
	virtual void BarnacleVictimBitten( entvars_t *pevBarnacle );
	virtual void BarnacleVictimReleased( void );
	static int GetAmmoIndex( const char *psz );
	int AmmoInventory( int iAmmoIndex );
	virtual int Illumination( void );

	void Spy_DisguiseExternalWeaponModel( void );
	void Spy_ResetExternalWeaponModel( void );

	void ResetAutoaim( void );
	Vector GetAutoaimVector( float flDelta );
	Vector AutoaimDeflection( Vector &vecSrc, float flDist, float flDelta );

	void ForceClientDllUpdate( void ); // Forces all client .dll specific data to be resent to client.

	void DeathMessage( entvars_t *pevKiller );

	void SetCustomDecalFrames( int nFrames );
	int GetCustomDecalFrames( void );

	void CheckPowerups( void );
	void PlayerTouch( CBaseEntity *pOther );
	void UpdateClientCommandMenu( void );
	BOOL PositionAvailable( void );
	BOOL CantChange( int iClassNo, BOOL showMessage );
	void ChangeClass( int iClassNo );
	void TeamFortress_SetEquipment( void );
	void SetLastWeapon( void );
	void TeamFortress_SetHealth( void );
	void TeamFortress_SetSpeed( void );
	void TeamFortress_SetSkin( void );

	char *m_pszSavedWeaponModel;
	int m_iTeamToDisguiseAs;
	float m_DB_LastTimingTestTime;
	int m_nFirstSpawn;
	string_t delayed_spawn_message;
	int forced_spawn;
	int no_sentry_message;
	int no_dispenser_message;
	int m_iPipebombCount;
	int m_iAmmoboxCount;
	int m_iMaxArmor;

	int no_entry_teleporter_message;
	int no_exit_teleporter_message;

	int m_iFadeDirection;
	float m_flFadeAmount;
	float m_flGagTime;
	float m_flLastTalkTime;
	int m_cSpamPoints;

	// Player ID
	void TeamFortress_InitStatusBar( void );
	void TeamFortress_UpdateStatusBar( void );

	CBaseEntity *GetTeleporter( int type );

	BOOL IsLegalClass( int pc );
	void TeamFortress_DisplayLegalClasses( void );
	void TeamFortress_PrintClassName( int pc );
	BOOL TeamFortress_TeamPutPlayerInTeam( void );
	BOOL TeamFortress_TeamSet( int tno );
	void TeamFortress_ExecClassScript( void );
	void TeamFortress_ExecMapScript( void );
	void TeamFortress_TeamShowMemberClasses( void );
	virtual void TeamFortress_CalcEMPDmgRad( float &damage, float &radius );
	virtual void TeamFortress_TakeEMPBlast( entvars_t *pevGren );
	virtual void TeamFortress_EMPRemove( void );
	virtual void TeamFortress_TakeConcussionBlast( entvars_t *pevGren, float bouncemax );
	virtual void TeamFortress_Concuss( entvars_t *pevGren );

	void ExplodeOldPipebomb( BOOL bAll, BOOL bForceDetonation );

	void RemovePipebombs( void );
	void Engineer_RemoveBuildings( void );
	void TeamFortress_RemoveBuildings( void );
	void Ignite( int tno );
	void TF_AddFrags( int iFrags );

	void ResetMenu( void );
	void Player_Menu( void );
	void Menu_MapBriefing( void );
	void Menu_Team( void );
	void Menu_Class( void );
	void Menu_RepeatHelp( void );
	void Menu_DoBindings_Input( float inp );
	void Menu_Input( float inp );
	void Menu_Intro( void );
	void Menu_ClassHelp( void );
	void Menu_ClassHelp_Input( float inp );
	void Menu_Spy( void );
	void Menu_Spy_Input( float inp );
	void Menu_Spy_Skin( void );
	void Menu_Spy_Skin_Input( float inp );
	void Menu_Spy_Color( void );
	void Menu_Spy_Color_Input( float inp );
	void Menu_Engineer( void );
	void Menu_Engineer_Input( float inp );
	void Menu_EngineerFix_Dispenser( void );
	void Menu_EngineerFix_Dispenser_Input( float inp );
	void Menu_EngineerFix_SentryGun( void );
	void Menu_EngineerFix_SentryGun_Input( float inp );
	void Menu_EngineerFix_Mortar( void );
	void Menu_EngineerFix_Mortar_Input( float inp );
	void Menu_Dispenser( void );
	void Menu_Dispenser_Input( float inp );
	
	void CleanupOnPlayerDisconnection( void );
	void TeamFortress_RemoveTimers( void );
	void TeamFortress_SetupRespawn( BOOL bSuicided );
	void TeamFortress_CheckClassStats( void );
	void TeamFortress_PrimeGrenade( int iWhichType );
	void TeamFortress_ThrowPrimedGrenade( void );

	void TeamFortress_SetDetpack( int iTimer );
	void TeamFortress_DetpackStop( void );
	BOOL TeamFortress_RemoveDetpacks( void );
	void TeamFortress_RemoveLiveGrenades( void );
	void TeamFortress_RemoveRockets( void );
	void RemoveOwnedEnt( const char *pEntName );
	void TeamFortress_EngineerBuild( int iBuildingNo );
	void TeamFortress_Build( int iBuildingID );

	void Spy_RemoveDisguise( void );

	void TeamFortress_SpyFeignDeath( BOOL bSilent );

	void ForceRespawn( void );
	void DropGoalItems( void );
	void UseSpecialSkill( void );
	void ClientHearVox( const char *pSentence );
	void ConnectDuringBattle( void );
	void CreateBattleID( void );
	void CreateGhost( void );
	BOOL CanFeign( void );
	void TeamFortress_SpyGoUndercover( void );
	void TeamFortress_SpyCalcName( void );

	void TeamFortress_SpyChangeColor( int iTeamNo );
	void TeamFortress_SpyChangeSkin( int iClass );
	void TeamFortress_SpyDisguiseEnemy( BOOL bEnemy, int iClass );
	void TeamFortress_SpyDisguise( int iTeam, int iClass );

	void CheckUncoveringSpies( CBasePlayer *pPlayer );
	virtual BOOL EngineerUse( CBasePlayer *pPlayer );
	void TeamFortress_Discard( void );
	void RemoveOldAmmobox( void );
	void TeamFortress_SaveMe( void );
	void TeamFortress_DisplayDetectionItems( void );

	void DisplayLocalItemStatus( CBaseEntity *pGoal );
	void AdminAccess( const char *pPassword );
	void Admin_CountPlayers( void );
	void Admin_ListIPs( void );
	void Admin_CycleDeal( void );
	void Admin_DoKick( void );
	void Admin_DoBan( void );
	void CheckAutoKick( void );
	void Admin_Changelevel( void );
	void TeleporterEffectThink( void );
	const char *DB_GetPlayerClassName( void );

	float m_flNextSBarUpdateTime;
	int m_izSBarState[SBAR_END];
	float m_flStatusBarDisappearDelay;

	EHANDLE m_hLastIDTarget;
	BOOL m_bUpdatedCommandMenu;
	int m_iClientIsFeigning;
	int m_iClientIsDetpacking;
	int m_iClientDetpackAmmo;
	int m_iClientBuildState;
	int m_iClientRandomPC;
	float m_flTeleporterEffectEndTime;

	void SetPrefsFromUserinfo( char *infobuffer );
};

#define AUTOAIM_2DEGREES  0.0348994967025
#define AUTOAIM_5DEGREES  0.08715574274766
#define AUTOAIM_8DEGREES  0.1391731009601
#define AUTOAIM_10DEGREES 0.1736481776669

extern int gmsgHudText;
extern BOOL gInitHUD;

#endif // __PLAYER_H__
