//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#if !defined ( EV_TFCH )
#define EV_TFCH

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

enum animtypes_e {
	ANIM_MISS = 0,
	ANIM_HIT = 1
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

enum ac_e {
	AC_IDLE1 = 0,
	AC_IDLE2 = 1,
	AC_SPINUP = 2,
	AC_SPINDOWN = 3,
	AC_FIRE = 4,
	AC_DEPLOY = 5,
	AC_HOLSTER = 6
};

enum flamethrower_e {
	FLAME_IDLE1 = 0,
	FLAME_FIDGET1 = 1,
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

enum railgun_e {
	RAIL_IDLE = 0,
	RAIL_FIRE = 1,
	RAIL_DRAW = 2,
	RAIL_HOLSTER = 3
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

enum medikit_e {
	MEDIKIT_IDLE_SHORT = 0,
	MEDIKIT_IDLE_LONG = 1,
	MEDIKIT_USE_SHORT = 2,
	MEDIKIT_USE_LONG = 3,
	MEDIKIT_HOLSTER = 4,
	MEDIKIT_DRAW = 5
};

enum knife_e {
	KNIFE_IDLE1 = 0,
	KNIFE_IDLE2 = 1,
	KNIFE_ATTACK1 = 2,
	KNIFE_ATTACK2 = 3,
	KNIFE_DRAW = 4,
	KNIFE_HOLSTER = 5
};

enum spanner_e {
	SPANNER_IDLE = 0,
	SPANNER_ATTACK1 = 1,
	SPANNER_ATTACK2 = 2,
	SPANNER_USE1 = 3,
	SPANNER_DRAW = 4,
	SPANNER_HOLSTER = 5
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

void EV_TFC_GunshotDecalTrace( pmtrace_t *pTrace, char *decalName );
void EV_TFC_DecalGunshot( pmtrace_t *pTrace, int iBulletType );
int EV_TFC_CheckTracer( int idx, float *vecSrc, float *end, float *forward, float *right, int iBulletType, int iTracerFreq, int *tracerCount );
void EV_TFC_FireBullets( int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, int iDamage );
void EV_TFC_NailTouch( struct tempent_s *ent, pmtrace_t *ptr );
void EV_TFC_Explode( float *org, int dmg, pmtrace_t *pTrace, float fExplosionScale );
void EV_TFC_BloodDrips( float *origin, int amount );
void EV_TFC_TraceAttack( int idx, float *vecDir, pmtrace_t *ptr, float flDamage );
float EV_TFC_WaterLevel( float *position, float minz, float maxz );
void EV_TFC_RailDie( struct particle_s *particle );
int EV_TFC_IsAlly( int idx1, int idx2 );
void EV_TFC_TranqNailTouch( tempent_s *ent, pmtrace_t *ptr );
void EV_TFC_PlayAxeSound( int idx, int classid, float *origin, int iSoundType, float fSoundData );
void EV_TFC_AxeHit( int idx, float *origin, float *forward, float *right, int entity, float *vecDir, pmtrace_t *ptr );
int EV_TFC_Medkit( int idx, float *origin, float *forward, float *right, int entity, float *vecDir, pmtrace_t *ptr );
tempent_s* EV_TFC_CreateGib( float *origin, float *attackdir, int multiplier, int ishead );
void EV_TFC_GibTouchCallback( tempent_s *ent, pmtrace_t *ptr );
void EV_TFC_GibCallback( tempent_s *ent, float frametime, float currenttime );
void EV_TFC_BloodDecalTrace( pmtrace_t *pTrace, int bloodColor );
int EV_TFC_PlayCrowbarAnim( int iAnimType );
char *EV_TFC_LookupDoorSound( int type, int index );

extern cvar_t *cl_gibcount;
extern cvar_t *cl_giblife;
extern cvar_t *cl_gibvelscale;
extern cvar_t *cl_localblood;

#endif