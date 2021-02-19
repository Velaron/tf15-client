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

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "entity_types.h"
#include "usercmd.h"
#include "pm_defs.h"
#include "pm_materials.h"

#include "eventscripts.h"
#include "ev_tfc.h"

#include "r_efx.h"
#include "event_api.h"
#include "event_args.h"
#include "in_defs.h"

#include <string.h>

#include "r_studioint.h"
#include "com_model.h"

#include "progdefs.h"

extern engine_studio_api_t IEngineStudio;

extern "C" char PM_FindTextureType( char* name );

extern "C" int PM_GetPhysEntInfo( int ent );

void V_PunchAxis( int axis, float punch );
void VectorAngles( const float* forward, float* angles );

extern "C" float anglemod( float a );

struct eventnode_s
{
	event_args_t* data;
	eventnode_s* prev;
	eventnode_s* next;
};
typedef eventnode_s eventnode_t;

static int tracerCount[32];
static pmtrace_s g_tr_decal[32];
static pmtrace_t* gp_tr_decal[32];
qboolean g_bACSpinning[33];
extern float g_flSpinDownTime[33];
extern float g_flSpinUpTime[33];
extern cvar_t* cl_lw;

extern globalvars_t* gpGlobals;
float g_flNextEventListThink;
float g_nTime;
eventnode_t* g_pEventListHead;

const Vector g_vRed = Vector( 255.0f, 0.0f, 0.0f );
const Vector g_vBlue = Vector( 0.0f, 0.0f, 255.0f );
const Vector g_vYellow = Vector( 255.0f, 255.0f, 0.0f );
const Vector g_vGreen = Vector( 0.0f, 255.0f, 0.0f );
const Vector g_vBlack = Vector( 0.0f, 0.0f, 0.0f );

extern "C"
{
	void EV_TFC_Axe( struct event_args_s* args );
	void EV_FireTFCShotgun( struct event_args_s* args );
	void EV_FireTFCSuperShotgun( struct event_args_s* args );
	void EV_ReloadTFCShotgun( struct event_args_s* args );
	void EV_PumpTFCShotgun( struct event_args_s* args );
	void EV_FireTFCNailgun( struct event_args_s* args );
	void EV_FireTFCSuperNailgun( struct event_args_s* args );
	void EV_FireTFCAutoRifle( struct event_args_s* args );
	void EV_TFC_Gas( struct event_args_s* args );
	void EV_TFC_DoorGoUp( struct event_args_s* args );
	void EV_TFC_DoorGoDown( struct event_args_s* args );
	void EV_TFC_DoorHitTop( struct event_args_s* args );
	void EV_TFC_DoorHitBottom( struct event_args_s* args );
	void EV_TFC_Explosion( struct event_args_s* args );
	void EV_TFC_Grenade( struct event_args_s* args );
	void EV_TFC_NormalGrenade( struct event_args_s* args );
	void EV_TFC_FireRPG( struct event_args_s* args );
	void EV_FireTFCSniper( struct event_args_s* args );
	void EV_TFC_SniperHit( struct event_args_s* args );
	void EV_TFC_FireIC( struct event_args_s* args );
	void EV_TFC_NailgrenadeNail( struct event_args_s* args );
	void EV_TFC_GrenadeLauncher( struct event_args_s* args );
	void EV_TFC_PipeLauncher( struct event_args_s* args );
	void EV_TFC_NormalShot( struct event_args_s* args );
	void EV_TFC_SuperShot( struct event_args_s* args );
	void EV_TFC_SteamShot( struct event_args_s* args );
	void EV_TFC_EngineerGrenade( struct event_args_s* args );
	void EV_TFC_Concussion( struct event_args_s* args );
	void EV_TFC_Assault_WindUp( struct event_args_s* args );
	void EV_TFC_Assault_WindDown( struct event_args_s* args );
	void EV_TFC_Assault_Start( struct event_args_s* args );
	void EV_TFC_Assault_Fire( struct event_args_s* args );
	void EV_TFC_Assault_Spin( struct event_args_s* args );
	void EV_TFC_Assault_StartSpin( struct event_args_s* args );
	void EV_TFC_AxeDecal( struct event_args_s* args );
	void EV_TFC_BuildingEvent( event_args_t* args );
	void EV_TFC_NapalmFire( struct event_args_s* args );
	void EV_TFC_MirvGrenadeMain( struct event_args_s* args );
	void EV_TFC_MirvGrenade( struct event_args_s* args );
	void EV_TFC_NapalmBurn( struct event_args_s* args );
	void EV_TFC_EMP( struct event_args_s* args );
	void EV_TFC_Flame_Fire( struct event_args_s* args );
	void EV_TFC_Railgun( struct event_args_s* args );
	void EV_TFC_Tranquilizer( struct event_args_s* args );
	void EV_TFC_NailGrenade( struct event_args_s* args );
	void EV_TFC_Knife( event_args_s* args );
	void EV_TFC_Gibs( event_args_s* args );
	void EV_Benchmark( event_args_t* args );
	void EV_TrainPitchAdjust( struct event_args_s* args );
}

#define VECTOR_CONE_1DEGREES  Vector( 0.00873, 0.00873, 0.00873 )
#define VECTOR_CONE_2DEGREES  Vector( 0.01745, 0.01745, 0.01745 )
#define VECTOR_CONE_3DEGREES  Vector( 0.02618, 0.02618, 0.02618 )
#define VECTOR_CONE_4DEGREES  Vector( 0.03490, 0.03490, 0.03490 )
#define VECTOR_CONE_5DEGREES  Vector( 0.04362, 0.04362, 0.04362 )
#define VECTOR_CONE_6DEGREES  Vector( 0.05234, 0.05234, 0.05234 )
#define VECTOR_CONE_7DEGREES  Vector( 0.06105, 0.06105, 0.06105 )
#define VECTOR_CONE_8DEGREES  Vector( 0.06976, 0.06976, 0.06976 )
#define VECTOR_CONE_9DEGREES  Vector( 0.07846, 0.07846, 0.07846 )
#define VECTOR_CONE_10DEGREES Vector( 0.08716, 0.08716, 0.08716 )
#define VECTOR_CONE_15DEGREES Vector( 0.13053, 0.13053, 0.13053 )
#define VECTOR_CONE_20DEGREES Vector( 0.17365, 0.17365, 0.17365 )

float EV_TFC_PlayTextureSound( int idx, pmtrace_t* ptr, float* vecSrc, float* vecEnd, int iBulletType )
{
	char chTextureType;
	float fvol, fvolbar;
	char* rgsz[4];
	int cnt;
	float fattn;
	int entity;
	const char* pTextureName;
	char texname[64];
	char szbuffer[64];

	chTextureType = '\0';
	entity = gEngfuncs.pEventAPI->EV_IndexFromTrace( ptr );

	if ( entity >= 1 && entity <= gEngfuncs.GetMaxClients() )
	{
		chTextureType = CHAR_TEX_FLESH;
	}
	else if ( entity == 0 )
	{
		pTextureName = gEngfuncs.pEventAPI->EV_TraceTexture( ptr->ent, vecSrc, vecEnd );

		if ( pTextureName )
		{
			strcpy( texname, pTextureName );
			pTextureName = texname;

			if ( *pTextureName == '-' || *pTextureName == '+' )
			{
				pTextureName += 2;
			}

			if ( *pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ' )
			{
				pTextureName++;
			}

			strcpy( szbuffer, pTextureName );
			szbuffer[CBTEXTURENAMEMAX - 1] = '\0';

			chTextureType = PM_FindTextureType( szbuffer );
		}
	}

	switch ( chTextureType )
	{
	default:
	case CHAR_TEX_CONCRETE:
		fvol = 0.9f;
		fvolbar = 0.6f;
		fattn = ATTN_NORM;
		rgsz[0] = "player/pl_step1.wav";
		rgsz[1] = "player/pl_step2.wav";
		cnt = 2;
		break;
	case CHAR_TEX_METAL:
		fvol = 0.9f;
		fvolbar = 0.3f;
		fattn = ATTN_NORM;
		rgsz[0] = "player/pl_metal1.wav";
		rgsz[1] = "player/pl_metal2.wav";
		cnt = 2;
		break;
	case CHAR_TEX_DIRT:
		fvol = 0.9f;
		fvolbar = 0.1f;
		fattn = 0.8f;
		rgsz[0] = "player/pl_dirt1.wav";
		rgsz[1] = "player/pl_dirt2.wav";
		rgsz[2] = "player/pl_dirt3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_VENT:
		fvol = 0.5f;
		fvolbar = 0.3f;
		fattn = ATTN_NORM;
		rgsz[0] = "player/pl_duct1.wav";
		rgsz[1] = "player/pl_duct1.wav";
		cnt = 2;
		break;
	case CHAR_TEX_GRATE:
		fvol = 0.9f;
		fvolbar = 0.5f;
		fattn = ATTN_NORM;
		rgsz[0] = "player/pl_grate1.wav";
		rgsz[1] = "player/pl_grate4.wav";
		cnt = 2;
		break;
	case CHAR_TEX_TILE:
		fvol = 0.8f;
		fvolbar = 0.2f;
		fattn = ATTN_NORM;
		rgsz[0] = "player/pl_tile1.wav";
		rgsz[1] = "player/pl_tile3.wav";
		rgsz[2] = "player/pl_tile2.wav";
		rgsz[3] = "player/pl_tile4.wav";
		cnt = 4;
		break;
	case CHAR_TEX_SLOSH:
		fvol = 0.9f;
		fvolbar = 0.0f;
		fattn = ATTN_NORM;
		rgsz[0] = "player/pl_slosh1.wav";
		rgsz[1] = "player/pl_slosh3.wav";
		rgsz[2] = "player/pl_slosh2.wav";
		rgsz[3] = "player/pl_slosh4.wav";
		cnt = 4;
		break;
	case CHAR_TEX_WOOD:
		fvol = 0.9f;
		fvolbar = 0.2f;
		fattn = ATTN_NORM;
		rgsz[0] = "debris/wood1.wav";
		rgsz[1] = "debris/wood2.wav";
		rgsz[2] = "debris/wood3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_GLASS:
	case CHAR_TEX_COMPUTER:
		fvol = 0.8f;
		fvolbar = 0.2f;
		fattn = ATTN_NORM;
		rgsz[0] = "debris/glass1.wav";
		rgsz[1] = "debris/glass2.wav";
		rgsz[2] = "debris/glass3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_FLESH:
		if ( iBulletType == BULLET_PLAYER_CROWBAR )
		{
			return 0.0f;
		}

		fvol = VOL_NORM;
		fvolbar = 0.2f;
		fattn = ATTN_NORM;
		rgsz[0] = "weapons/bullet_hit1.wav";
		rgsz[1] = "weapons/bullet_hit2.wav";
		cnt = 2;
		break;
	}

	if ( chTextureType == CHAR_TEX_COMPUTER && ptr->fraction != 1.0f && gEngfuncs.pfnRandomLong( 0, 1 ) == 1 )
	{
		switch ( gEngfuncs.pfnRandomLong( 0, 1 ) )
		{
		case 0:
			gEngfuncs.pEventAPI->EV_PlaySound( 0, ptr->endpos, 6, "buttons/spark5.wav", 0.8f, ATTN_NORM, 0, PITCH_NORM );
			break;
		case 1:
			gEngfuncs.pEventAPI->EV_PlaySound( 0, ptr->endpos, 6, "buttons/spark6.wav", 0.8f, ATTN_NORM, 0, PITCH_NORM );
			break;
		}
	}

	gEngfuncs.pEventAPI->EV_PlaySound( 0, ptr->endpos, CHAN_STATIC, rgsz[gEngfuncs.pfnRandomLong( 0, cnt - 1 )], fvol, fattn, 0, 96 + gEngfuncs.pfnRandomLong( 0, 15 ) );

	return fvolbar;
}

char* EV_TFC_DamageDecal( int entity, int bitsDamageType )
{
	physent_t* pe;
	static char decalname[32];

	pe = gEngfuncs.pEventAPI->EV_GetPhysent( entity );
	*decalname = '\0';

	if ( pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) )
	{
		if ( pe->rendermode != kRenderNormal && pe->rendermode != kRenderTransAlpha )
		{
			sprintf( decalname, "{bproof1" );
		}
		else
		{
			sprintf( decalname, "{shot%i", gEngfuncs.pfnRandomLong( 0, 4 ) + 1 );
		}
	}

	return decalname;
}

void EV_TFC_GunshotDecalTrace( pmtrace_t* pTrace, char* name )
{
	int iRand;

	if ( name && *name && pTrace->fraction != 1.0f )
	{
		gEngfuncs.pEfxAPI->R_BulletImpactParticles( pTrace->endpos );
		iRand = gEngfuncs.pfnRandomLong( 0, 0x7FFF );

		if ( iRand < ( 0x7fff / 2 ) )
		{
			switch ( iRand % 5 )
			{
			case 0:
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pTrace->endpos, 0, "weapons/ric1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
				break;
			case 1:
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pTrace->endpos, 0, "weapons/ric2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
				break;
			case 2:
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pTrace->endpos, 0, "weapons/ric3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
				break;
			case 3:
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pTrace->endpos, 0, "weapons/ric4.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
				break;
			case 4:
				gEngfuncs.pEventAPI->EV_PlaySound( -1, pTrace->endpos, 0, "weapons/ric5.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
				break;
			}
		}

		EV_TFC_DecalTrace( pTrace, name );
	}
}

void EV_TFC_DecalGunshot( pmtrace_t* pTrace, int iBulletType )
{
	physent_t* pe;

	pe = gEngfuncs.pEventAPI->EV_GetPhysent( pTrace->ent );

	if ( pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) )
	{
		switch ( iBulletType )
		{
		case BULLET_PLAYER_CROWBAR:
			EV_TFC_DecalTrace( pTrace, EV_TFC_DamageDecal( pTrace->ent, 0 ) );
			break;
		case BULLET_PLAYER_TF_ASSAULT:
			//	EV_TFC_GunshotDecalTrace( pTrace, "hwshot1" );
			//	break;
		default:
			EV_TFC_GunshotDecalTrace( pTrace, EV_TFC_DamageDecal( pTrace->ent, 0 ) );
			break;
		}
	}
}

void EV_TFC_DecalTrace( pmtrace_t* pTrace, const char* name )
{
	physent_t* pe;

	if ( name && *name )
	{
		pe = gEngfuncs.pEventAPI->EV_GetPhysent( pTrace->ent );

		if ( pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) && CVAR_GET_FLOAT( "r_decals" ) != 0.0f )
		{
			gEngfuncs.pEfxAPI->R_DecalShoot( gEngfuncs.pEfxAPI->Draw_DecalIndex( gEngfuncs.pEfxAPI->Draw_DecalIndexFromName( (char*)name ) ),
			                                 gEngfuncs.pEventAPI->EV_IndexFromTrace( pTrace ), 0, pTrace->endpos, 0 );
		}
	}
}

int EV_TFC_CheckTracer( int idx, float* vecSrc, float* end, float* forward, float* right, int iBulletType, int iTracerFreq, int* tracerCount )
{
	Vector vecTracerSrc, offset;

	if ( iTracerFreq != 0 && ( ( *tracerCount )++ % iTracerFreq ) == 0 )
	{
		if ( EV_IsPlayer( idx ) )
		{
			offset = Vector( 0, 0, -4 );

			for ( int i = 0; i < 3; i++ )
			{
				vecTracerSrc[i] = vecSrc[i] + offset[i] + right[i] * 2.0f + forward[i] * 16.0f;
			}
		}
		else
		{
			VectorCopy( vecSrc, vecTracerSrc );
		}

		if ( iBulletType != BULLET_PLAYER_TF_BUCKSHOT )
		{
			EV_CreateTracer( vecTracerSrc, end );
		}
	}

	return iTracerFreq != 1;
}

int EV_TFC_ShouldShowBlood( int color )
{
	if ( color != DONT_BLEED )
	{
		if ( color == BLOOD_COLOR_RED )
		{
			return CVAR_GET_FLOAT( "violence_hblood" ) != 0.0f;
		}
		else
		{
			return CVAR_GET_FLOAT( "violence_ablood" ) != 0.0f;
		}
	}

	return 0;
}

void EV_TFC_RandomBloodVector( float* direction )
{
	direction[0] = gEngfuncs.pfnRandomFloat( -1.0f, 1.0f );
	direction[1] = gEngfuncs.pfnRandomFloat( -1.0f, 1.0f );
	direction[2] = gEngfuncs.pfnRandomFloat( 0.0f, 1.0f );
}

/*
================
FireBullets

Go to the trouble of combining multiple pellets into a single damage call.
================
*/
void EV_TFC_FireBullets( int idx, float* forward, float* right, float* up, int cShots, float* vecSrc, float* vecDirShooting, float* vecSpread, float flDistance, int iBulletType, int iTracerFreq, int* tracerCount, int iDamage )
{
	pmtrace_t tr;
	int tracer, iShot;
	int cMultiGunShots = 0;
	//	char decalname[32];
	//	int decal_index[5];

	for ( iShot = 1; iShot <= cShots; iShot++ )
	{
		Vector vecDir, vecEnd;
		float x, y, z;

		do
		{
			x = gEngfuncs.pfnRandomFloat( -0.5f, 0.5f ) + gEngfuncs.pfnRandomFloat( -0.5f, 0.5f );
			y = gEngfuncs.pfnRandomFloat( -0.5f, 0.5f ) + gEngfuncs.pfnRandomFloat( -0.5f, 0.5f );
			z = x * x + y * y;
		} while ( z > 1 );

		for ( int i = 0; i < 3; i++ )
		{
			vecDir[i] = vecDirShooting[i] + x * vecSpread[0] * right[i] + y * vecSpread[1] * up[i];
			vecEnd[i] = vecSrc[i] + flDistance * vecDir[i];
		}

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( 0, 1 );
		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetSolidPlayers( idx - 1 );
		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );

		tracer = EV_TFC_CheckTracer( idx, vecSrc, tr.endpos, forward, right, iBulletType, iTracerFreq, tracerCount );

		if ( tr.fraction != 1.0f )
		{
			if ( iDamage == 0 )
			{
				switch ( iBulletType )
				{
				case BULLET_PLAYER_BUCKSHOT:
					if ( !tracer )
					{
						EV_TFC_DecalGunshot( &tr, iBulletType );
					}
					break;
				default:
					if ( !tracer )
					{
						EV_TFC_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
						EV_TFC_DecalGunshot( &tr, iBulletType );
					}
					break;
				}
			}
			else
			{
				EV_TFC_TraceAttack( idx, iDamage, vecDir, &tr );
				EV_TFC_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
				EV_TFC_DecalGunshot( &tr, iBulletType );
			}
		}

		gEngfuncs.pEventAPI->EV_PopPMStates();
	}

	// Velaron: TODO

	/*
		if ( cShots != 1 )
		{
			for ( int i = 0; i < 5; i++ )
			{
				sprintf( decalname, "{shot%i", i + 1 );
				decal_index[i] = gEngfuncs.pEfxAPI->Draw_DecalIndexFromName( decalname );
			}

			gEngfuncs.pEfxAPI->R_MultiGunshot( vecSrc, vecDirShooting, vecSpread, cShots - 1, 5, decal_index );
		}
	*/
}

#define SND_CHANGE_PITCH ( 1 << 7 )

void EV_TrainPitchAdjust( event_args_t* args )
{
	int idx;
	vec3_t origin;

	unsigned short us_params;
	int noise;
	float m_flVolume;
	int pitch;
	int stop;

	char sz[256];

	idx = args->entindex;

	VectorCopy( args->origin, origin );

	us_params = (unsigned short)args->iparam1;
	stop = args->bparam1;

	m_flVolume = (float)( us_params & 0x003f ) / 40.0;
	noise = (int)( ( ( us_params ) >> 12 ) & 0x0007 );
	pitch = (int)( 10.0 * (float)( ( us_params >> 6 ) & 0x003f ) );

	switch ( noise )
	{
	case 1:
		strcpy( sz, "plats/ttrain1.wav" );
		break;
	case 2:
		strcpy( sz, "plats/ttrain2.wav" );
		break;
	case 3:
		strcpy( sz, "plats/ttrain3.wav" );
		break;
	case 4:
		strcpy( sz, "plats/ttrain4.wav" );
		break;
	case 5:
		strcpy( sz, "plats/ttrain6.wav" );
		break;
	case 6:
		strcpy( sz, "plats/ttrain7.wav" );
		break;
	default:
		strcpy( sz, "" );
		return;
	}

	if ( stop )
	{
		gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, sz );
	}
	else
	{
		gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_STATIC, sz, m_flVolume, ATTN_NORM, SND_CHANGE_PITCH, pitch );
	}
}

int EV_TFC_IsAllyTeam( int iTeam1, int iTeam2 )
{
	int team_ally[5];

	for ( int i = 0; i < 5; i++ )
	{
		team_ally[i] = atoi( gEngfuncs.PhysInfo_ValueForKey( "ta" ) ) >> ( 5 * i ) & 0x1F;
	}

	if ( !( ( team_ally[iTeam1] >> ( iTeam2 - 1 ) ) & 1 ) )
	{
		return iTeam1 == iTeam2;
	}

	return 1;
}

int EV_TFC_PlayCrowbarAnim( int iAnimType )
{
	static int m_iSwing;
	int iAnim;

	m_iSwing++;

	if ( iAnimType == ANIM_HIT )
	{
		switch ( m_iSwing % 2 )
		{
		case 0: iAnim = CROWBAR_ATTACK2HIT; break;
		case 1: iAnim = CROWBAR_ATTACK3HIT; break;
		}
	}
	else
	{
		switch ( m_iSwing % 3 )
		{
		case 0: iAnim = CROWBAR_ATTACK1MISS; break;
		case 1: iAnim = CROWBAR_ATTACK2MISS; break;
		case 2: iAnim = CROWBAR_ATTACK3MISS; break;
		}
	}

	return iAnim;
}

enum soundtypes_e
{
	SOUND_MISS,
	SOUND_HIT_BODY,
	SOUND_HIT_WALL,
};

void EV_TFC_Axe( event_args_t* args )
{
	int idx;
	float fSoundData;
	int classid;
	int ent;
	int iAnim;
	pmtrace_t tr, tr2;
	Vector vecSrc, vecEnd;
	Vector right, forward, up;
	Vector origin, angles;
	unsigned short m_usAxeDecal;

	idx = args->entindex;
	classid = args->iparam1;

	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	AngleVectors( angles, forward, right, up );
	EV_GetGunPosition( args, vecSrc, origin );
	VectorMA( vecSrc, 32.0f, forward, vecEnd );

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( 0, 1 );
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers( idx - 1 );
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_NORMAL, -1, &tr );

	if ( tr.fraction >= 1.0f )
	{
		gEngfuncs.pEventAPI->EV_SetTraceHull( 1 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_NORMAL, -1, &tr2 );

		if ( tr2.fraction < 1.0f )
		{
			tr = tr2;
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
	gp_tr_decal[idx - 1] = NULL;

	if ( tr.fraction >= 1.0f )
	{
		EV_TFC_PlayAxeAnim( idx, classid, 0 );
		EV_TFC_PlayAxeSound( idx, classid, origin, 0, 0.0f );
		return;
	}

	g_tr_decal[idx - 1] = tr;
	gp_tr_decal[idx - 1] = &g_tr_decal[idx - 1];

	if ( EV_IsLocal( idx ) )
	{
		EV_TFC_PlayAxeAnim( idx, classid, 1 );
	}

	ent = gEngfuncs.pEventAPI->EV_IndexFromTrace( &tr );

	switch ( classid )
	{
	case 0:
		EV_TFC_AxeHit( idx, origin, forward, right, ent, forward, &tr );
		break;
	case 1:
		EV_TFC_TraceAttack( idx, 20.0f, forward, &tr );
		break;
	case 2:
		gp_tr_decal[idx - 1] = NULL;
		EV_TFC_Medkit( idx, origin, forward, right, ent, forward, &tr );
		break;
	case 3:
		EV_TFC_AxeHit( idx, origin, forward, right, ent, forward, &tr );
		break;
	}

	if ( gp_tr_decal[idx - 1] )
	{
		m_usAxeDecal = gEngfuncs.pEventAPI->EV_PrecacheEvent( 1, "events/wpn/tf_axedecal.sc" );
		gEngfuncs.pEventAPI->EV_PlaybackEvent( idx, 0, m_usAxeDecal, 0.2f, origin, angles, 0.0f, 0.0f, 0, 0, 0, 0 );
	}

	if ( EV_IsPlayer( ent ) )
	{
		EV_TFC_PlayAxeSound( idx, classid, origin, SOUND_HIT_BODY, 0.0f );
	}
	else
	{
		fSoundData = gEngfuncs.GetMaxClients() < 2 ? EV_TFC_PlayTextureSound( idx, &tr, vecSrc, vecSrc, BULLET_PLAYER_CROWBAR ) : 1.0f;
		EV_TFC_PlayAxeSound( idx, classid, origin, SOUND_HIT_WALL, fSoundData );
	}
}

void EV_FireTFCShotgun( event_args_t* args )
{
	int idx;
	int shell;
	Vector origin, angles, velocity;
	Vector up, right, forward;
	Vector ShellVelocity, ShellOrigin;
	Vector vecSrc, vecAiming;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex( "models/shotgunshell.mdl" );

	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
	AngleVectors( angles, forward, right, up );

	EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0f, -12.0f, 4.0f );

	if ( EV_IsLocal( idx ) )
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( SHOTGUN_FIRE, 2 );
		V_PunchAxis( 0, -2.0f );
	}

	EV_EjectBrass( ShellOrigin, ShellVelocity, angles.y, shell, TE_BOUNCE_SHELL );

	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/sbarrel1.wav", gEngfuncs.pfnRandomFloat( 0.95f, 1.0f ), ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 31 ) + 93 );
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_TFC_FireBullets( idx, forward, right, up, 6, vecSrc, vecAiming, Vector( 0.04f, 0.04f, 0.0f ), 2048.0f, BULLET_PLAYER_TF_BUCKSHOT, 0, &tracerCount[idx - 1], 4 );
}

void EV_ReloadTFCShotgun( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( SHOTGUN_RELOAD, 2 );
	}

	if ( gEngfuncs.pfnRandomLong( 0, 1 ) )
	{
		gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_ITEM, "weapons/reload3.wav", VOL_NORM, ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 31 ) + 85 );
	}
	else
	{
		gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_ITEM, "weapons/reload1.wav", VOL_NORM, ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 31 ) + 85 );
	}
}

void EV_PumpTFCShotgun( event_args_t* args )
{
	int full;
	int idx;
	Vector origin;

	full = args->bparam1;
	idx = args->entindex;

	VectorCopy( args->origin, origin );

	if ( full && EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( SHOTGUN_PUMP, 2 );
	}

	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/scock1.wav", VOL_NORM, ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 31 ) + 95 );
}

void EV_FireTFCNailgun( event_args_t* args )
{
	int idx;
	int shell;
	Vector origin, angles;
	Vector up, right, forward;
	Vector ShellVelocity, ShellOrigin;
	Vector vecSrc, vecAiming, vecVelocity;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex( "models/nail.mdl" );

	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( NAILGUN_FIRE1, 2 );
	}

	EV_GetGunPosition( args, ShellOrigin, origin );
	VectorMA( ShellOrigin, -4.0f, up, ShellOrigin );
	VectorMA( ShellOrigin, 2.0f, right, ShellOrigin );
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/airgun_1.wav", gEngfuncs.pfnRandomFloat( 0.95f, 1.0f ), ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 31 ) + 93 );
	VectorScale( forward, 1000.0f, vecVelocity );
	EV_TFC_ClientProjectile( ShellOrigin, vecVelocity, shell, idx, 6, EV_TFC_NailTouch );

	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -1.0f );
	}
}

void EV_TFC_NailTouch( struct tempent_s* ent, pmtrace_t* ptr )
{
	physent_t* pe;
	char decalname[32];

	pe = gEngfuncs.pEventAPI->EV_GetPhysent( ptr->ent );

	if ( pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) )
	{
		sprintf( decalname, "{shot%i", gEngfuncs.pfnRandomLong( 1, 5 ) );

		if ( ptr->fraction != 1.0 )
		{
			EV_TFC_GunshotDecalTrace( ptr, decalname );
		}
	}
}

void EV_FireTFCSuperNailgun( event_args_t* args )
{
	int idx;
	int shell;
	Vector origin, angles;
	Vector up, right, forward;
	Vector ShellVelocity, ShellOrigin;
	Vector vecSrc, vecAiming, vecVelocity;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex( "models/nail.mdl" );

	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( NAILGUN_FIRE1, 2 );
	}

	EV_GetGunPosition( args, ShellOrigin, origin );
	VectorMA( ShellOrigin, -4.0f, up, ShellOrigin );
	VectorMA( ShellOrigin, 2.0f, right, ShellOrigin );
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/spike2.wav", VOL_NORM, ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 15 ) + 94 );
	VectorScale( forward, 1000.0f, vecVelocity );
	EV_TFC_ClientProjectile( ShellOrigin, vecVelocity, shell, idx, 6, EV_TFC_NailTouch );

	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -1.0f );
	}
}

void EV_FireTFCSuperShotgun( event_args_t* args )
{
	int idx;
	int shell;
	Vector origin, angles, velocity;
	Vector up, right, forward;
	Vector ShellVelocity, ShellOrigin;
	Vector vecSrc, vecAiming;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex( "models/shotgunshell.mdl" );

	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
	AngleVectors( angles, forward, right, up );
	EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0f, -12.0f, 4.0f );

	if ( EV_IsLocal( idx ) )
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( SHOTGUN_FIRE, 2 );
		V_PunchAxis( 0, -4.0f );
	}

	EV_EjectBrass( ShellOrigin, ShellVelocity, angles.y, shell, TE_BOUNCE_SHELL );

	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/shotgn2.wav", gEngfuncs.pfnRandomFloat( 0.95f, 1.0f ), ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 31 ) + 93 );
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	EV_TFC_FireBullets( idx, forward, right, up, 14, vecSrc, vecAiming, Vector( 0.04f, 0.04f, 0.0f ), 2048.0f, BULLET_PLAYER_TF_BUCKSHOT, 4, &tracerCount[idx - 1], 4 );
}

void EV_FireTFCAutoRifle( event_args_t* args )
{
	int idx;
	pmtrace_t tr;
	Vector origin, angles;
	Vector up, right, forward;
	Vector vecSrc, vecAiming, vecEnd;

	idx = args->entindex;

	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( ARIFLE_FIRE, 2 );
	}

	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/sniper.wav", 0.9f, ATTN_NORM, 0, PITCH_NORM );
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	VectorMA( vecSrc, 8192.0f, vecAiming, vecEnd );
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers( idx - 1 );
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );

	if ( tr.fraction != 1.0 )
	{
		EV_TFC_TraceAttack( idx, 8.0f, vecAiming, &tr );
		EV_TFC_PlayTextureSound( idx, &tr, vecSrc, vecEnd, BULLET_PLAYER_357 );
		EV_TFC_DecalGunshot( &tr, BULLET_PLAYER_357 );
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
}

void EV_TFC_TraceAttack( int idx, float flDamage, float* vecDir, pmtrace_t* ptr, int bitsDamageType )
{
	int entity;
	Vector vecOrigin, dir;

	if ( !cl_localblood || cl_localblood->value != 0.0f )
	{
		VectorScale( vecDir, -4.0, dir );
		vecOrigin = ptr->endpos - dir;
		entity = gEngfuncs.pEventAPI->EV_IndexFromTrace( ptr );

		if ( EV_IsPlayer( entity ) && !EV_TFC_IsAlly( idx, entity ) )
		{
			EV_TFC_SpawnBlood( vecOrigin, BLOOD_COLOR_RED, flDamage );
		}
	}
}

void EV_TFC_BloodDrips( float* origin, int color, int amount )
{
	float scale;

	if ( EV_TFC_ShouldShowBlood( color ) && amount )
	{
		if ( gEngfuncs.GetMaxClients() > 1 )
		{
			amount *= 2;
		}
		scale = Q_min( Q_max( 3.0f, amount / 10.0f ), 16.0f );

		gEngfuncs.pEfxAPI->R_BloodSprite( origin, color, gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/bloodspray.spr" ),
		                                  gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/blood.spr" ), scale );
	}
}

void EV_TFC_BubbleTrail( float* from, float* to, int count, int modelindex )
{
	float maxz, maxza, maxzb;

	maxz = from[2] + 256.0f;
	maxza = EV_TFC_WaterLevel( from, from[2], maxz ) - from[2];

	if ( maxza < 8.0f )
	{
		maxzb = to[2] + 256.0f;

		if ( EV_TFC_WaterLevel( to, to[2], maxzb ) - to[2] > 8.0f )
		{
			maxza = EV_TFC_WaterLevel( to, to[2], maxzb ) - from[2];
		}
	}

	count = Q_min( count, 255 );
	gEngfuncs.pEfxAPI->R_BubbleTrail( from, to, maxza, modelindex, count, 8.0f );
}

void EV_TFC_ClientProjectile( float* vecOrigin, float* vecVelocity, short sModelIndex, int iOwnerIndex,
                              int iLife, void ( *hitcallback )( struct tempent_s* ent, struct pmtrace_s* ptr ) )
{
	gEngfuncs.pEfxAPI->R_Projectile( vecOrigin, vecVelocity, sModelIndex, iLife, iOwnerIndex, hitcallback );
}

void EV_TFC_Assault_WindUp( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	g_flSpinDownTime[idx - 1] = 0.0f;
	g_flSpinUpTime[idx - 1] = gEngfuncs.GetClientTime() + 3.5f;
	g_bACSpinning[idx - 1] = 0;

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( AC_SPINUP, 2 );
	}

	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, "weapons/asscan2.wav" );
	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, "weapons/asscan4.wav" );
	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_WEAPON, "weapons/asscan3.wav" );
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/asscan1.wav", 0.98f, ATTN_NORM, 0, 125 );
}

void EV_TFC_Assault_WindDown( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	if ( g_flSpinUpTime[idx - 1] == 0.0f )
	{
		g_flSpinDownTime[idx - 1] = gEngfuncs.GetClientTime() + 3.0f;
	}

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( AC_SPINDOWN, 2 );
	}

	g_bACSpinning[idx - 1] = FALSE;

	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, "weapons/asscan2.wav" );
	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, "weapons/asscan4.wav" );
	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_WEAPON, "weapons/asscan1.wav" );

	if ( !args->bparam1 )
	{
		gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/asscan3.wav", 0.98f, ATTN_NORM, 0, 125 );
	}
}

void EV_TFC_Assault_Start( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	g_flSpinDownTime[idx - 1] = 0.0f;
	g_flSpinUpTime[idx - 1] = 0.0f;
	g_bACSpinning[idx - 1] = FALSE;

	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, "weapons/asscan2.wav" );
	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, "weapons/asscan4.wav" );
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_STATIC, "weapons/asscan2.wav", 0.98f, ATTN_NORM, 0, 125 );
}

void EV_TFC_Assault_Fire( event_args_t* args )
{
	int idx;
	int oddammo;
	int shell;
	Vector ShellOrigin, ShellVelocity;
	Vector up, right, forward;
	Vector vecSrc, vecAiming;
	Vector origin, angles, velocity;
	Vector vecSpread;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex( "models/shell.mdl" );
	oddammo = args->bparam1;

	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( AC_FIRE, 2 );
	}

	g_bACSpinning[idx - 1] = FALSE;

	if ( oddammo )
	{
		EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 15.0f, -25.0f, 6.0f );
		EV_EjectBrass( ShellOrigin, ShellVelocity, angles.y, shell, TE_BOUNCE_SHELL );
	}

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	for ( int i = 0; i < 3; i++ )
	{
		vecSrc[i] = right[i] * 2.0f + up[i] * -4.0f + vecSrc[i];
	}

	vecSpread = Vector( 0.1f, 0.1f, 0.0f );

	EV_TFC_FireBullets( idx, forward, right, up, 5, vecSrc, vecAiming, vecSpread, 8192.0f, BULLET_PLAYER_TF_ASSAULT, 8, &tracerCount[idx - 1], 8 );
}

void EV_TFC_Assault_Spin( event_args_t* args )
{
	int idx;

	idx = args->entindex;

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( AC_FIRE, 2 );
	}

	g_bACSpinning[idx - 1] = TRUE;
}

void EV_TFC_Assault_StartSpin( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	g_bACSpinning[idx - 1] = TRUE;

	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, "weapons/asscan2.wav" );
	gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, "weapons/asscan4.wav" );
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_STATIC, "weapons/asscan4.wav", 0.98f, ATTN_NORM, 0, 125 );
}

void EV_TFC_Gas( event_args_t* args )
{
	Vector origin;

	VectorCopy( args->origin, origin );

	gEngfuncs.pEfxAPI->R_ParticleBurst( origin, 240, 195, 2.0f );
}

void EV_TFC_DoorGoUp( event_args_t* args )
{
	const char* sound;

	sound = EV_TFC_LookupDoorSound( 0, args->iparam1 );
	gEngfuncs.pEventAPI->EV_StopSound( -1, CHAN_STATIC, sound );
	gEngfuncs.pEventAPI->EV_PlaySound( -1, args->origin, CHAN_STATIC, sound, VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
}

void EV_TFC_DoorGoDown( event_args_t* args )
{
	const char* sound;

	sound = EV_TFC_LookupDoorSound( 0, args->iparam1 );
	gEngfuncs.pEventAPI->EV_StopSound( -1, CHAN_STATIC, sound );
	gEngfuncs.pEventAPI->EV_PlaySound( -1, args->origin, CHAN_STATIC, sound, VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
}

void EV_TFC_DoorHitTop( event_args_t* args )
{
	const char* sound;

	sound = EV_TFC_LookupDoorSound( 0, args->iparam1 );
	gEngfuncs.pEventAPI->EV_StopSound( -1, CHAN_STATIC, sound );
	sound = EV_TFC_LookupDoorSound( 1, args->iparam1 );
	gEngfuncs.pEventAPI->EV_PlaySound( -1, args->origin, CHAN_STATIC, sound, VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
}

void EV_TFC_DoorHitBottom( event_args_t* args )
{
	const char* sound;

	sound = EV_TFC_LookupDoorSound( 0, args->iparam1 );
	gEngfuncs.pEventAPI->EV_StopSound( -1, CHAN_STATIC, sound );
	sound = EV_TFC_LookupDoorSound( 1, args->iparam1 );
	gEngfuncs.pEventAPI->EV_PlaySound( -1, args->origin, CHAN_STATIC, sound, VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
}

void EV_TFC_Explode( float* org, int dmg, float fExplosionScale, qboolean bDontSpark, pmtrace_t* pTrace, int bitsDamageType )
{
	Vector origin;
	float fireball;
	qboolean explosion_outside;

	VectorCopy( org, origin );

	if ( pTrace->fraction != 1.0 )
	{
		fireball = Q_max( dmg - 24, 0 ) * 0.6f;
		VectorMA( pTrace->endpos, fireball, pTrace->plane.normal, origin );
	}

	explosion_outside = gEngfuncs.PM_PointContents( origin, NULL ) == CONTENT_SOLID && gEngfuncs.PM_PointContents( org, NULL ) == CONTENT_SOLID;

	if ( fExplosionScale == 0.0f )
	{
		fExplosionScale = ( dmg - 50 ) * 0.6f;
	}

	gEngfuncs.pEfxAPI->R_Explosion( origin, gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/explode01.spr" ),
	                                fExplosionScale * 0.1f, 15.0f, TE_EXPLFLAG_NOADDITIVE | TE_EXPLFLAG_NODLIGHTS );

	if ( !explosion_outside )
	{
		if ( gEngfuncs.pfnRandomFloat( 0.0f, 1.0f ) > 0.5f )
		{
			EV_TFC_DecalTrace( pTrace, "{scorch2" );
		}
		else
		{
			EV_TFC_DecalTrace( pTrace, "{scorch1" );
		}
	}

	switch ( gEngfuncs.pfnRandomLong( 0, 2 ) )
	{
	case 0:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_VOICE, "weapons/debris1.wav", 0.55f, ATTN_NORM, 0, PITCH_NORM );
		break;
	case 1:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_VOICE, "weapons/debris2.wav", 0.55f, ATTN_NORM, 0, PITCH_NORM );
		break;
	case 2:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_VOICE, "weapons/debris3.wav", 0.55f, ATTN_NORM, 0, PITCH_NORM );
		break;
	}

	if ( ( explosion_outside || gEngfuncs.PM_PointContents( org, NULL ) == CONTENT_WATER ) && !bDontSpark )
	{
		for ( int i = 0; i < gEngfuncs.pfnRandomLong( 0, 3 ); i++ )
		{
			gEngfuncs.pEfxAPI->R_SparkShower( origin );
		}
	}
}

void EV_TFC_ExplodeFire( float* org, int dmg, qboolean bDontSpark, pmtrace_t* pTrace, int bitsDamageType )
{
	EV_TFC_Explode( org, dmg, 0.0f, bDontSpark, pTrace, bitsDamageType );
}

void EV_TFC_Explosion( event_args_t* args )
{
	pmtrace_t tr;
	Vector vecEnd, vecSpot;
	Vector origin;

	VectorCopy( args->origin, origin );
	VectorCopy( origin, vecSpot );
	VectorCopy( origin, vecEnd );

	vecSpot.z += 8.0f;
	vecEnd.z -= 32.0f;

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSpot, vecEnd, PM_STUDIO_BOX, -1, &tr );
	gEngfuncs.pEventAPI->EV_PopPMStates();

	EV_TFC_Explode( origin, 120, 0.0f, TRUE, &tr, 0 );
}

void EV_TFC_Grenade( event_args_t* args )
{
	pmtrace_t tr;
	Vector vecEnd, vecSpot;
	Vector origin;

	VectorCopy( args->origin, origin );
	VectorCopy( origin, vecSpot );
	VectorCopy( origin, vecEnd );

	vecSpot.z += 8.0f;
	vecEnd.z -= 32.0f;

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSpot, vecEnd, PM_STUDIO_BOX, -1, &tr );
	gEngfuncs.pEventAPI->EV_PopPMStates();
	EV_TFC_Explode( origin, 120, 0.0f, TRUE, &tr, 0 );
}

void EV_TFC_NormalGrenade( event_args_t* args )
{
	pmtrace_t tr;
	Vector vecEnd, vecSpot;
	Vector origin;

	VectorCopy( args->origin, origin );
	VectorCopy( origin, vecSpot );
	VectorCopy( origin, vecEnd );

	vecSpot.z += 8.0f;
	vecEnd.z -= 32.0f;

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSpot, vecEnd, PM_STUDIO_BOX, -1, &tr );
	gEngfuncs.pEventAPI->EV_PopPMStates();
	EV_TFC_Explode( origin, 180, 0.0f, TRUE, &tr, 0 );
}

void EV_TFC_FireRPG( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( RPG_FIRE, 2 );
	}

	gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_WEAPON, "weapons/rocketfire1.wav", 0.9f, ATTN_NORM, 0, PITCH_NORM );
	gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_WEAPON, "weapons/glauncher.wav", 0.7f, ATTN_NORM, 0, PITCH_NORM );

	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -5.0f );
	}
}

void EV_FireTFCSniper( event_args_t* args )
{
	int idx;
	physent_t* pe;
	pmtrace_t tr;
	int iDamage;
	Vector origin, angles;
	Vector up, right, forward;
	Vector vecSrc, vecDir, vecEnd;

	idx = args->entindex;
	iDamage = args->iparam1;

	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( SRIFLE_FIRE, 2 );
	}

	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "ambience/rifle1.wav", 0.9f, ATTN_NORM, 0, PITCH_NORM );
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecDir );
	VectorMA( vecSrc, 2.0f, up, vecSrc );
	VectorMA( vecSrc, 8192.0f, vecDir, vecEnd );

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( 0, 1 );
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers( idx - 1 );
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );
	gEngfuncs.pEventAPI->EV_PopPMStates();

	if ( tr.fraction != 1.0f )
	{
		if ( cl_localblood && cl_localblood->value != 0.0f )
		{
			EV_TFC_TraceAttack( idx, iDamage, vecDir, &tr );
		}

		EV_TFC_PlayTextureSound( idx, &tr, vecSrc, vecEnd, BULLET_PLAYER_357 );

		pe = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent );

		if ( pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) )
		{
			EV_TFC_DecalGunshot( &tr, BULLET_PLAYER_357 );
		}
	}
}

void EV_TFC_SniperHit( event_args_s* args )
{
	int idx;
	float volume;
	char* sound;
	Vector origin;

	idx = args->entindex;
	volume = args->iparam1 / 100.0f;

	VectorCopy( args->origin, origin );

	if ( args->bparam2 )
	{
		sound = "common/bodysplat.wav";
		volume = 1.0f;
	}
	else if ( args->bparam1 )
	{
		sound = "weapons/bullet_hit2.wav";
	}
	else
	{
		sound = "weapons/ric3.wav";
	}

	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_STATIC, sound, volume, ATTN_NORM, 0, PITCH_NORM );
}

void EV_TFC_FireIC( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( RPG_HOLSTER, 2 );
	}

	gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_WEAPON, "weapons/sgun1.wav", 0.9f, ATTN_NORM, 0, PITCH_NORM );

	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -5.0f );
	}
}

void EV_TFC_NailgrenadeNail( event_args_t* args )
{
	int nail;
	Vector origin, angles, forward, velocity, org;

	VectorCopy( args->origin, org );
	angles = Vector( 0.0f, ( args->iparam1 & 0x7FF ) * 0.25f, 0.0f );
	nail = gEngfuncs.pEventAPI->EV_FindModelIndex( "models/nail.mdl" );

	for ( int i = 0; i < 5; i++ )
	{
		angles.y = anglemod( args->fparam1 + angles.y );
		AngleVectors( angles, forward, NULL, NULL );
		VectorMA( org, 12.0f, forward, origin );
		VectorScale( forward, 1000.0f, velocity );
		EV_TFC_ClientProjectile( origin, velocity, nail, ( args->iparam1 >> 11 ) & 0x1F, 6, EV_TFC_NailTouch );

		if ( gEngfuncs.pfnRandomLong( 0, 5 ) == 0 )
		{
			if ( gEngfuncs.pfnRandomLong( 0, 1 ) == 0 )
			{
				gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_WEAPON, "weapons/airgun_1.wav", VOL_NORM, ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 15 ) + 94 );
			}
			else
			{
				gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_WEAPON, "weapons/spike2.wav", VOL_NORM, ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 15 ) + 94 );
			}
		}
	}
}

void EV_TFC_GrenadeLauncher( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( GL_FIRE, 2 );
	}

	gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_WEAPON, "weapons/glauncher.wav", 0.7f, ATTN_NORM, 0, PITCH_NORM );

	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -2.0f );
	}
}

void EV_TFC_PipeLauncher( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( PL_FIRE, 2 );
	}

	gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_WEAPON, "weapons/glauncher.wav", 0.7f, ATTN_NORM, 0, PITCH_NORM );

	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -2.0f );
	}
}

void EV_TFC_NormalShot( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_ITEM, "items/medshot4.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
}

void EV_TFC_SuperShot( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_ITEM, "items/medshot5.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
}

void EV_TFC_SteamShot( event_args_t* args )
{
	int idx;
	Vector origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_ITEM, "items/steamburst1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
}

void EV_TFC_AxeDecal( event_args_t* args )
{
	int idx;
	pmtrace_t* tr;
	physent_t* pe;

	idx = args->entindex;
	EV_TFC_DecalGunshot( gp_tr_decal[idx - 1], BULLET_PLAYER_CROWBAR );
	gp_tr_decal[idx - 1] = NULL;
}

void EV_TFC_EngineerGrenade( event_args_t* args )
{
	pmtrace_t tr;
	Vector vecEnd, vecSpot;
	Vector origin;

	VectorCopy( args->origin, origin );
	VectorCopy( origin, vecSpot );
	VectorCopy( origin, vecEnd );

	vecSpot.z += 8.0f;
	vecEnd.z -= 32.0f;

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSpot, vecEnd, PM_STUDIO_BOX, -1, &tr );
	gEngfuncs.pEventAPI->EV_PopPMStates();

	EV_TFC_Explode( origin, 180, 78.0f, TRUE, &tr, 0 );
}

void EV_TFC_Concussion( event_args_t* args )
{
	int wave;
	pmtrace_t tr;
	Vector vecEnd, vecSpot;
	Vector origin;

	wave = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/shockwave.spr" );
	VectorCopy( args->origin, origin );
	VectorCopy( origin, vecSpot );
	VectorCopy( origin, vecEnd );

	vecSpot.z += 8.0f;
	vecEnd.z -= 32.0f;

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSpot, vecEnd, PM_STUDIO_BOX, -1, &tr );
	gEngfuncs.pEventAPI->EV_PopPMStates();

	switch ( gEngfuncs.pfnRandomLong( 0, 2 ) )
	{
	case 0:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_VOICE, "weapons/concgren_blast1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
		break;
	case 1:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_VOICE, "weapons/concgren_blast2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
		break;
	case 2:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_VOICE, "weapons/concgren_blast3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
		break;
	}

	VectorCopy( origin, vecSpot );
	VectorCopy( origin, vecEnd );

	vecSpot.z += 16.0f;
	vecEnd.z += 600.0f + 16.0f;

	gEngfuncs.pEfxAPI->R_BeamCirclePoints( TE_BEAMCYLINDER, vecSpot, vecEnd, wave, 0.2f, 70.0f, 0.0f, 1.0f, 0.0f, 0, 0.0f, 1.0f, 1.0f, 1.0f );
}

void EV_TFC_MirvGrenadeMain( event_args_t* args )
{
	EV_TFC_NormalGrenade( args );
}

void EV_TFC_MirvGrenade( event_args_t* args )
{
	EV_TFC_NormalGrenade( args );
}

void EV_TFC_NapalmFire( event_args_t* args )
{
	pmtrace_t tr;
	Vector vecSpot, vecEnd;
	Vector origin;

	VectorCopy( args->origin, origin );
	VectorCopy( origin, vecSpot );
	VectorCopy( origin, vecEnd );

	vecSpot.z += 8.0f;
	vecEnd.z -= 32.0f;

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSpot, vecEnd, PM_STUDIO_BOX, -1, &tr );
	gEngfuncs.pEventAPI->EV_PopPMStates();

	EV_TFC_Explode( origin, 20, 30.0f, TRUE, &tr, 0 );
}

void EV_TFC_NapalmBurn( event_args_t* args )
{
	int sprite;
	Vector origin;

	sprite = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/playerflame.spr" );
	VectorCopy( args->origin, origin );

	gEngfuncs.pEfxAPI->R_FireField( origin, 100, sprite, 12, TEFIRE_FLAG_PLANAR | TEFIRE_FLAG_SOMEFLOAT, 0.8f );
}

void EV_TFC_EMP( event_args_t* args )
{
	int wave;
	pmtrace_t tr;
	Vector vecEnd, vecSpot;
	Vector origin;

	wave = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/shockwave.spr" );
	VectorCopy( args->origin, origin );
	VectorCopy( origin, vecSpot );
	VectorCopy( origin, vecEnd );

	vecSpot.z += 8.0f;
	vecEnd.z -= 32.0f;

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSpot, vecEnd, PM_STUDIO_BOX, -1, &tr );
	gEngfuncs.pEventAPI->EV_PopPMStates();

	gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_VOICE, "weapons/emp_1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );

	VectorCopy( origin, vecSpot );
	VectorCopy( origin, vecEnd );

	vecSpot.z += 16.0f;
	vecEnd.z += 600.0f + 16.0f;

	gEngfuncs.pEfxAPI->R_BeamCirclePoints( TE_BEAMCYLINDER, vecSpot, vecEnd, wave, 0.2f, 70.0f, 0.0f, 1.0f, 0.0f, 0, 0.0f, 1.0f, 1.0f, 1.0f );
}

void EV_TFC_Flame_Fire( event_args_t* args )
{
	int idx;
	int shell, bubble;
	Vector ShellOrigin;
	Vector BubbleSpot;
	Vector vecVelocity;
	Vector up, right, forward;
	Vector origin, angles;
	int underwater;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/fthrow.spr" );
	bubble = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/bubble.spr" );
	underwater = args->bparam1;

	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( FLAME_FIRE1, 2 );
	}

	EV_GetGunPosition( args, ShellOrigin, origin );
	VectorMA( ShellOrigin, -8.0f, up, ShellOrigin );
	VectorMA( ShellOrigin, 8.0f, right, ShellOrigin );
	VectorMA( ShellOrigin, 16.0f, forward, ShellOrigin );

	if ( !underwater )
	{
		VectorScale( forward, 600.0f, vecVelocity );
		EV_TFC_ClientProjectile( ShellOrigin, vecVelocity, shell, idx, 1, NULL );
		gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/flmfire2.wav", 0.9f, ATTN_NORM, 0, PITCH_NORM );
		return;
	}

	VectorMA( ShellOrigin, 128.0f, forward, BubbleSpot );
	EV_TFC_BubbleTrail( ShellOrigin, BubbleSpot, 4, bubble );
}

//Velaron: TODO
float EV_TFC_WaterLevel( float* position, float minz, float maxz )
{
	float result; // fst7
	float v5;     // fst7
	float v6;     // fst5
	float diff;   // [esp+28h] [ebp-24h]
	float minz_1; // [esp+38h] [ebp-14h]

	minz_1 = minz;
	result = minz;

	if ( gEngfuncs.PM_PointContents( position, NULL ) == CONTENTS_WATER )
	{
		minz_1 = maxz;
		result = maxz;
		diff = minz;

		v5 = maxz - minz;
		if ( v5 > 1.0 )
		{
			while ( 1 )
			{
				minz_1 = v5 * 0.5 + diff;

				if ( gEngfuncs.PM_PointContents( position, NULL ) == CONTENTS_WATER )
				{
					result = minz_1;
					diff = minz_1;
				}
				else
				{
					result = minz_1;
					maxz = minz_1;
				}
				v6 = maxz - diff;
				if ( v6 <= 1.0 )
					break;
				v5 = v6;
			}
		}
		else
		{
			result = minz_1;
		}
	}

	return result;
}

void EV_TFC_Railgun( event_args_t* args )
{
	int idx;
	pmtrace_t tr;
	Vector ShellOrigin;
	Vector vecSrc, vecEnd;
	Vector tracerVelocity;
	Vector right, up, forward;
	Vector origin, angles;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( RAIL_FIRE, 2 );
	}

	EV_GetGunPosition( args, ShellOrigin, origin );
	VectorMA( ShellOrigin, -4.0f, up, ShellOrigin );
	VectorMA( ShellOrigin, 2.0f, right, ShellOrigin );

	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/railgun.wav", 0.9f, ATTN_NORM, 0, PITCH_NORM );

	VectorCopy( ShellOrigin, vecSrc );
	VectorMA( vecSrc, 4096.0f, forward, vecEnd );

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers( idx - 1 );
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );

	VectorScale( forward, 1500.0f, tracerVelocity );
	gEngfuncs.pEfxAPI->R_UserTracerParticle( ShellOrigin, tracerVelocity, Length( tr.endpos - vecSrc ) / 1500.0f, 2, 1.0f, idx, EV_TFC_RailDie );
	gEngfuncs.pEventAPI->EV_PopPMStates();

	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -2.0f );
	}
}

void EV_TFC_RailDie( particle_s* particle )
{
	physent_t* pe;
	int entity;
	pmtrace_t tr;
	Vector forward, back;
	Vector vecSrc, vecEnd;

	if ( particle == NULL )
	{
		return;
	}

	VectorCopy( particle->vel, forward );
	VectorNormalize( forward );

	VectorCopy( forward, back );
	VectorInverse( back );

	VectorMA( particle->org, 128.0f, back, vecSrc );
	VectorMA( vecSrc, 256.0f, forward, vecEnd );

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers( particle->context - 1 );
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );

	if ( tr.ent != -1 )
	{
		pe = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent );
		entity = gEngfuncs.pEventAPI->EV_IndexFromTrace( &tr );

		if ( !EV_IsPlayer( entity ) || EV_TFC_IsAlly( entity, particle->context ) )
		{
			EV_TFC_GunshotDecalTrace( &tr, EV_TFC_DamageDecal( entity, 0 ) );
		}
		else
		{
			EV_TFC_SpawnBlood( tr.endpos, BLOOD_COLOR_RED, 25.0f );
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
}

int EV_TFC_IsAlly( int idx1, int idx2 )
{
	if ( EV_TFC_GetTeamIndex( idx1 ) < 1 || EV_TFC_GetTeamIndex( idx1 ) > 4
	     || EV_TFC_GetTeamIndex( idx2 ) > 1 || EV_TFC_GetTeamIndex( idx2 ) > 4 )
	{
		return 0;
	}

	if ( gEngfuncs.GetLocalPlayer() )
	{
		return EV_TFC_IsAllyTeam( EV_TFC_GetTeamIndex( idx1 ), EV_TFC_GetTeamIndex( idx2 ) );
	}

	return 0;
}

void EV_TFC_Tranquilizer( event_args_t* args )
{
	int idx;
	int shell;
	Vector ShellOrigin, vecVelocity;
	Vector right, up, forward;
	Vector origin, angles;

	idx = args->entindex;
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex( "models/nail.mdl" );
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( TRANQ_SHOOT, 1 );
	}

	EV_GetGunPosition( args, ShellOrigin, origin );
	VectorMA( ShellOrigin, -4.0f, up, ShellOrigin );
	VectorMA( ShellOrigin, 2.0f, right, ShellOrigin );
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/dartgun.wav", 0.9f, ATTN_NORM, 0, PITCH_NORM );
	VectorScale( forward, 1500.0f, vecVelocity );
	EV_TFC_ClientProjectile( ShellOrigin, vecVelocity, shell, idx, 6, EV_TFC_TranqNailTouch );

	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -2.0f );
	}
}

void EV_TFC_TranqNailTouch( tempent_s* ent, pmtrace_t* ptr )
{
	physent_t* pe;
	int entity;
	char decalname[32];

	entity = gEngfuncs.pEventAPI->EV_IndexFromTrace( ptr );

	if ( !EV_IsPlayer( entity ) )
	{
		pe = gEngfuncs.pEventAPI->EV_GetPhysent( ptr->ent );

		if ( pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) )
		{
			if ( ptr->fraction != 1.0f )
			{
				sprintf( decalname, "{shot%i", gEngfuncs.pfnRandomLong( 1, 5 ) );
				EV_TFC_GunshotDecalTrace( ptr, decalname );
			}
		}
	}
}

void EV_TFC_NailGrenade( event_args_t* args )
{
	pmtrace_t tr;
	Vector vecEnd, vecSpot;
	Vector origin;

	VectorCopy( args->origin, origin );
	VectorCopy( origin, vecSpot );
	VectorCopy( origin, vecEnd );

	vecSpot.z += 8.0f;
	vecEnd.z -= 32.0f;

	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSpot, vecEnd, PM_STUDIO_BOX, -1, &tr );
	gEngfuncs.pEventAPI->EV_PopPMStates();

	EV_TFC_Explode( origin, 180, 78.0f, TRUE, &tr, 0 );
}

void EV_TFC_GibCallback( tempent_s* ent, float frametime, float currenttime )
{
	switch ( ent->entity.curstate.playerclass )
	{
	case 0:
		if ( Length( ent->entity.baseline.origin ) == 0.0f )
		{
			ent->entity.curstate.playerclass = 1;
			ent->entity.baseline.fuser1 = cl_giblife->value;
		}
		ent->die = gEngfuncs.GetClientTime() + 1.0f;
		break;
	case 1:
		ent->entity.baseline.fuser1 -= frametime;
		if ( ent->entity.baseline.fuser1 <= 0.0f )
		{
			ent->entity.curstate.playerclass = 2;
		}
		ent->die = gEngfuncs.GetClientTime() + 1.0f;
		break;
	case 2:
		ent->entity.curstate.playerclass = 3;
		ent->entity.curstate.renderamt = 255;
		ent->entity.curstate.rendermode = kRenderFxPulseFast;
		ent->die = gEngfuncs.GetClientTime() + 2.5f;
		break;
	case 3:
		if ( ent->entity.curstate.renderamt <= 7 )
		{
			ent->entity.curstate.playerclass = 4;
			ent->entity.curstate.renderamt = 0;
			ent->die = gEngfuncs.GetClientTime() + 0.2f;
		}
		else
		{
			ent->entity.curstate.renderamt = ent->entity.curstate.renderamt - frametime * 70.0f;
			ent->die = gEngfuncs.GetClientTime() + 1.0f;
		}
		break;
	}
}

void EV_TFC_BloodDecalTrace( pmtrace_t* pTrace, int bloodColor )
{
	const char* format;
	char decalname[32];

	if ( EV_TFC_ShouldShowBlood( bloodColor ) )
	{
		format = bloodColor == BLOOD_COLOR_RED ? "{blood%i" : "{yblood%i";
		sprintf( decalname, format, gEngfuncs.pfnRandomLong( 0, 5 ) + 1 );
		EV_TFC_DecalTrace( pTrace, decalname );
	}
}

void EV_TFC_GibTouchCallback( tempent_s* ent, pmtrace_t* ptr )
{
	if ( Length( ent->entity.baseline.origin ) > 0.0f && ent->entity.curstate.team > 0 )
	{
		EV_TFC_BloodDecalTrace( ptr, BLOOD_COLOR_RED );
		ent->entity.curstate.team--;
	}
}

void EV_TFC_TraceBleed( float flDamage, float* vecDir, pmtrace_t* ptr, int bitsDamageType, int bloodcolor )
{
	pmtrace_t Bloodtr;
	Vector vecTraceDir, trend;
	float flNoise;
	int cCount;

	if ( bloodcolor != DONT_BLEED && flDamage != 0.0f && bitsDamageType & 0x8000C7 /* Velaron: idk */ )
	{
		if ( flDamage < 10.0f )
		{
			flNoise = 0.1f;
			cCount = 1;
		}
		else if ( flDamage >= 25.0f )
		{
			flNoise = 0.3f;
			cCount = 4;
		}
		else
		{
			flNoise = 0.2f;
			cCount = 2;
		}

		for ( int i = 0; i < cCount; i++ )
		{
			VectorScale( vecDir, -1.0f, vecTraceDir );

			for ( int j = 0; j < 3; j++ )
			{
				vecTraceDir[i] += gEngfuncs.pfnRandomFloat( -flNoise, flNoise );
			}

			VectorMA( ptr->endpos, -172.0f, vecTraceDir, trend );
			gEngfuncs.pEventAPI->EV_PlayerTrace( ptr->endpos, trend, PM_STUDIO_BOX, -1, &Bloodtr );

			if ( Bloodtr.fraction != 1.0f )
			{
				EV_TFC_BloodDecalTrace( &Bloodtr, bloodcolor );
			}
		}
	}
}

void EV_TFC_SpawnBlood( float* vecSpot, int bloodColor, float flDamage )
{
	EV_TFC_BloodDrips( vecSpot, bloodColor, flDamage );
}

int EV_TFC_GetTeamIndex( int idx )
{
	cl_entity_s* pEnt;

	if ( EV_IsPlayer( idx ) && ( pEnt = GetEntity( idx ) ) )
	{
		return pEnt->curstate.team;
	}

	return -1;
}

tempent_s* EV_TFC_CreateGib( float* origin, float* attackdir, int multiplier, int ishead )
{
	int modelindex;
	tempent_s* ent;
	float mins[3], maxs[3];
	float vmultiple;

	model_s* gib = gEngfuncs.CL_LoadModel( "models/hgibs.mdl", &modelindex );

	if ( gib == NULL )
		return NULL;

	vmultiple = multiplier ? ( 4.0f * cl_gibvelscale->value ) : ( vmultiple = 0.7f * cl_gibvelscale->value );

	gEngfuncs.pEventAPI->EV_LocalPlayerBounds( 0, mins, maxs );
	ent = gEngfuncs.pEfxAPI->CL_TentEntAllocCustom( origin, gib, 0, EV_TFC_GibCallback );

	if ( ent )
	{
		ent->entity.curstate.body = ishead ? 0 : gEngfuncs.pfnRandomLong( 1, 5 );
		ent->entity.origin[0] += gEngfuncs.pfnRandomFloat( -1.0f, 1.0f ) * ( ( maxs[0] - mins[0] ) * 0.5f );
		ent->entity.origin[1] += gEngfuncs.pfnRandomFloat( -1.0f, 1.0f ) * ( ( maxs[1] - mins[1] ) * 0.5f );
		ent->entity.origin[2] += gEngfuncs.pfnRandomFloat( -1.0f, 1.0f ) * ( ( maxs[2] - mins[2] ) * 0.5f );

		if ( ishead )
		{
			ent->entity.curstate.velocity[0] = gEngfuncs.pfnRandomFloat( -100.0f, 100.0f );
			ent->entity.curstate.velocity[1] = gEngfuncs.pfnRandomFloat( -100.0f, 100.0f );
			ent->entity.curstate.velocity[2] = gEngfuncs.pfnRandomFloat( 200.0f, 300.0f );
			ent->entity.curstate.angles[0] = gEngfuncs.pfnRandomFloat( 100.0f, 200.0f );
			ent->entity.curstate.angles[1] = gEngfuncs.pfnRandomFloat( 200.0f, 300.0f );
		}
		else
		{
			VectorScale( attackdir, -1.0f, ent->entity.curstate.velocity );
			ent->entity.curstate.velocity[0] += gEngfuncs.pfnRandomFloat( -0.25f, 0.25f ) * gEngfuncs.pfnRandomFloat( 300.0f, 400.0f ) * vmultiple;
			ent->entity.curstate.velocity[1] += gEngfuncs.pfnRandomFloat( -0.25f, 0.25f ) * gEngfuncs.pfnRandomFloat( 300.0f, 400.0f ) * vmultiple;
			ent->entity.curstate.velocity[2] += gEngfuncs.pfnRandomFloat( -0.25f, 0.25f ) * gEngfuncs.pfnRandomFloat( 300.0f, 400.0f ) * vmultiple;
			ent->entity.baseline.angles[0] = gEngfuncs.pfnRandomFloat( 100.0f, 200.0f );
			ent->entity.baseline.angles[1] = gEngfuncs.pfnRandomFloat( 100.0f, 200.0f );
		}

		EV_TFC_GibVelocityCheck( ent->entity.curstate.velocity );
		ent->flags |= ( FTENT_GRAVITY | FTENT_ROTATE | FTENT_COLLIDEWORLD );
		ent->hitcallback = EV_TFC_GibTouchCallback;
		ent->entity.curstate.team = 5;
		ent->die = ent->die + 1.0f;
		VectorCopy( ent->entity.curstate.velocity, ent->entity.baseline.origin );
		ent->entity.curstate.solid = 0;
		ent->entity.curstate.playerclass = 0;
	}

	return ent;
}

void EV_TFC_Gibs( event_args_t* args )
{
	int idx;
	int multiplier, gibcount;
	Vector origin, attackdir;

	idx = args->entindex;
	multiplier = args->iparam1;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, attackdir );

	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "common/bodysplat.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );

	gibcount = cl_gibcount->value;
	gibcount = gibcount > 64 ? 64 : gibcount;

	for ( int x = 0; x < gibcount; x++ )
	{
		EV_TFC_CreateGib( origin, attackdir, multiplier, 0 );
	}

	EV_TFC_CreateGib( origin, attackdir, multiplier, 1 );
}

void EV_TFC_PlayAxeSound( int idx, int classid, float* origin, int iSoundType, float fSoundData )
{
	static char sound[32];

	if ( classid != 2 )
	{
		switch ( iSoundType )
		{
		case SOUND_MISS:
			sprintf( sound, "weapons/cbar_miss1.wav" );
			gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, sound, 1.0f, ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 15 ) + 94 );
			break;
		case SOUND_HIT_BODY:
			sprintf( sound, "weapons/cbar_hitbod%i.wav", gEngfuncs.pfnRandomLong( 1, 3 ) );
			gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, sound, fSoundData, ATTN_NORM, 0, 100 );
			break;
		case SOUND_HIT_WALL:
			sprintf( sound, "weapons/cbar_hit%i.wav", gEngfuncs.pfnRandomLong( 1, 2 ) );
			gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, sound, fSoundData, ATTN_NORM, 0, gEngfuncs.pfnRandomLong( 0, 3 ) + 98 );
			break;
		}
	}
}

void EV_TFC_AxeHit( int idx, float* origin, float* forward, float* right, int entity, float* vecDir, pmtrace_t* ptr )
{
	int iAnim;
	cl_entity_s* pEnt;
	Vector up;

	pEnt = GetEntity( entity - 1 );

	if ( EV_IsPlayer( entity ) && pEnt && pEnt->curstate.playerclass == 8 ) // PC_SPY
	{
		AngleVectors( pEnt->curstate.angles, NULL, NULL, up );

		if ( up[0] * forward[1] - forward[0] * up[1] <= 0.0f )
		{
			EV_TFC_TraceAttack( idx, 40.0f, vecDir, ptr );
			iAnim = KNIFE_ATTACK1;
		}
		else
		{
			EV_TFC_TraceAttack( idx, 120.0f, vecDir, ptr );
			iAnim = KNIFE_ATTACK2;
		}

		if ( EV_IsLocal( idx ) )
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation( iAnim, 2 );
		}
	}
	else
	{
		EV_TFC_TraceAttack( idx, 20.0f, vecDir, ptr );
	}
}

int EV_TFC_Medkit( int idx, float* origin, float* forward, float* right, int entity, float* vecDir, pmtrace_t* ptr )
{
	cl_entity_s* pEnt;

	if ( entity > 0 && entity <= gEngfuncs.GetMaxClients() )
	{
		pEnt = GetEntity( entity - 1 );
		if ( !EV_TFC_IsAlly( idx, entity ) )
		{
			if ( !cl_localblood || cl_localblood->value != 0.0f )
			{
				EV_TFC_TraceAttack( idx, 10.0f, vecDir, ptr );
			}
			if ( pEnt->curstate.playerclass != 5 ) // PC_MEDIC
			{
				gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, 2, "player/death2.wav", VOL_NORM, ATTN_NORM, 0, 100 );
			}
		}
	}
	else
	{
		return 0;
	}

	return 1;
}

char* EV_TFC_LookupDoorSound( int type, int index )
{
	int idx;
	static char sound[32];

	strcpy( sound, "common/null.wav" );

	idx = (char)index;

	if ( idx > 0 )
	{
		if ( type )
			sprintf( sound, "doors/doorstop%i.wav", idx );
		else
			sprintf( sound, "doors/doormove%i.wav", idx );
	}

	return sound;
}

void EV_TFC_Knife( event_args_t* args )
{
}

const Vector& GetTeamColor( int team_no )
{
	switch ( team_no )
	{
	case 0:
		return g_vBlue;
	case 1:
		return g_vRed;
	case 2:
		return g_vYellow;
	case 3:
		return g_vGreen;
	default:
		return g_vBlack;
	}
}

void EV_TFC_GibVelocityCheck( float* vel )
{
	float scale;

	if ( Length( vel ) > 1500.0f )
	{
		scale = 1500.0f / Length( vel );
		VectorScale( vel, scale, vel );
	}
}

int EV_TFC_PlayKnifeAnim( int iAnimType )
{
	switch ( iAnimType )
	{
	case ANIM_MISS: return KNIFE_ATTACK1;
	case ANIM_HIT: return KNIFE_ATTACK1;
	default: return -1;
	}
}

int EV_TFC_PlayMedkitAnim( int iAnimType )
{
	switch ( iAnimType )
	{
	case ANIM_MISS: return MEDIKIT_USE_SHORT;
	case ANIM_HIT: return MEDIKIT_USE_LONG;
	default: return -1;
	}
}

int EV_TFC_PlaySpannerAnim( int iAnimType )
{
	switch ( iAnimType )
	{
	case ANIM_MISS: return SPANNER_ATTACK1;
	case ANIM_HIT: return SPANNER_USE1;
	default: return -1;
	}
}

void EV_TFC_PlayAxeAnim( int idx, int classid, int iAnimType )
{
	if ( EV_IsLocal( idx ) )
	{
		switch ( classid )
		{
		case 0: gEngfuncs.pEventAPI->EV_WeaponAnimation( EV_TFC_PlayKnifeAnim( iAnimType ), 2 ); break;
		case 1: gEngfuncs.pEventAPI->EV_WeaponAnimation( EV_TFC_PlaySpannerAnim( iAnimType ), 2 ); break;
		case 2: gEngfuncs.pEventAPI->EV_WeaponAnimation( EV_TFC_PlayMedkitAnim( iAnimType ), 2 ); break;
		case 3: gEngfuncs.pEventAPI->EV_WeaponAnimation( EV_TFC_PlayCrowbarAnim( iAnimType ), 2 ); break;
		}
	}
}

int EV_TFC_Spanner( int idx, float* origin, float* forward, float* right, int entity, float* vecDir, pmtrace_t* ptr )
{
	EV_TFC_TraceAttack( idx, 20.0f, vecDir, ptr );
	return 1;
}

// Velaron: todo maybe
//void EV_TFC_BenchmarkWallMark( pmtrace_t *pTrace, char *name )
//void EV_TFC_BenchMarkTrace( pmtrace_t *pTrace )

void EV_Benchmark( event_args_t* args )
{
}

void EV_TFC_BuildingEvent( event_args_t* args )
{
	if ( !args )
	{
		return;
	}
}

void RandomSparkSound( float* origin )
{
	switch ( (int)( gEngfuncs.pfnRandomFloat( 0.0f, 1.0f ) * 6.0f ) )
	{
	case 0:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_STATIC, "buttons/spark1.wav", gEngfuncs.pfnRandomFloat( 0.2f, 0.45f ), ATTN_NORM, 0, PITCH_NORM );
		break;
	case 1:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_STATIC, "buttons/spark2.wav", gEngfuncs.pfnRandomFloat( 0.2f, 0.45f ), ATTN_NORM, 0, PITCH_NORM );
		break;
	case 2:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_STATIC, "buttons/spark3.wav", gEngfuncs.pfnRandomFloat( 0.2f, 0.45f ), ATTN_NORM, 0, PITCH_NORM );
		break;
	case 3:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_STATIC, "buttons/spark4.wav", gEngfuncs.pfnRandomFloat( 0.2f, 0.45f ), ATTN_NORM, 0, PITCH_NORM );
		break;
	case 4:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_STATIC, "buttons/spark5.wav", gEngfuncs.pfnRandomFloat( 0.2f, 0.45f ), ATTN_NORM, 0, PITCH_NORM );
		break;
	case 5:
		gEngfuncs.pEventAPI->EV_PlaySound( -1, origin, CHAN_STATIC, "buttons/spark6.wav", gEngfuncs.pfnRandomFloat( 0.2f, 0.45f ), ATTN_NORM, 0, PITCH_NORM );
		break;
	}
}

void DoSparkSmokeEffect( event_args_t* args )
{
	pmtrace_t tr;
	Vector vecstart, vecend;

	if ( !args )
	{
		return;
	}

	if ( gEngfuncs.pfnRandomFloat( 0.0f, 1.0f ) > 0.8f )
	{
		vecstart = args->origin;
		vecend.x = vecstart.x + gEngfuncs.pfnRandomLong( -50, 50 );
		vecend.y = vecstart.y + gEngfuncs.pfnRandomLong( -50, 50 );
		vecend.z = vecstart.z + gEngfuncs.pfnRandomLong( 15, 50 );

		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecend, vecstart, PM_NORMAL, -1, &tr );
		gEngfuncs.pEventAPI->EV_PopPMStates();

		if ( args->entindex == PM_GetPhysEntInfo( tr.ent ) )
		{
			gEngfuncs.pEfxAPI->R_SparkEffect( tr.endpos, 8, -200, 200 );
			RandomSparkSound( tr.endpos );

			if ( args->iparam1 & 2 )
			{
				tr.endpos.z -= 8.0f;
				gEngfuncs.pEfxAPI->R_Sprite_Smoke( gEngfuncs.pEfxAPI->R_DefaultSprite( tr.endpos, gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/steam1.spr" ), 15.0f ), 0.8f );
			}
		}
	}
}

void DoTeleporterRings( event_args_t* args )
{
	static int iEntryCount = 0, iExitCount = 0;
	Vector vecSpot, vecEnd;
	Vector color;

	if ( !args )
	{
		return;
	}

	if ( args->iparam1 & 4 )
	{
		if ( ++iEntryCount > 6 )
		{
			iEntryCount = 0;
		}
	}
	else
	{
		if ( --iExitCount < 0 )
		{
			iExitCount = 6;
		}
	}

	vecSpot = vecEnd = args->origin;
	vecSpot.z += ( -14 * ( ( args->iparam1 & 4 ) ? iEntryCount : iExitCount ) + 98 );
	vecEnd.z = vecSpot.z + 125.0f;
	color = GetTeamColor( args->iparam2 - 1 );

	gEngfuncs.pEfxAPI->R_BeamCirclePoints( FBEAM_SINENOISE | FBEAM_FADEIN | FBEAM_STARTENTITY, vecSpot, vecEnd,
	                                       gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/shellchrome.spr" ), 0.2f, 10.0f, 0.0f, 1.0f, 0.0f, 0, 0.0f, color.x, color.y, color.z );
}

void DoTeleporterParticles( event_args_t* args )
{
	// Velaron: big todo ( particleman )
	if ( !args )
	{
		return;
	}
}

void PlayTeleporterAmbientSound( event_args_t* args )
{
	if ( !args )
	{
		return;
	}

	if ( ( args->fparam2 == 0.0f || args->fparam2 <= gpGlobals->time ) && gEngfuncs.pfnRandomFloat( 0.0f, 1.0f ) > 1.0f )
	{
		args->fparam2 = gpGlobals->time + 6.0f;
		gEngfuncs.pEventAPI->EV_PlaySound( -1, args->origin, CHAN_STATIC, "misc/teleport_ready.wav", VOL_NORM, 0.5f, 0, PITCH_NORM );
	}
}

void RemoveEvent( eventnode_t* node )
{
	if ( node && g_pEventListHead )
	{
		if ( node == g_pEventListHead )
		{
		}
	}
}

void CheckEventsFinished( eventnode_t* node )
{
	if ( node && !node->data->iparam1 && g_pEventListHead )
	{
		if ( node == g_pEventListHead )
		{
		}
	}
}

void ClearEventList( void )
{
	eventnode_t* pNext;

	while ( g_pEventListHead )
	{
		pNext = g_pEventListHead->next;
		delete g_pEventListHead;
		g_pEventListHead = pNext;
	}

	g_pEventListHead = NULL;
	g_flNextEventListThink = 0.0f;
}

void RunEventList( void )
{
	/*
		eventnode_t *ptr;

		if ( g_flNextEventListThink == 0.0f || g_flNextEventListThink <= gpGlobals->time )
		{
			ptr = g_pEventListHead;
			g_flNextEventListThink = gpGlobals->time + 0.1f;

			if ( !ptr ) { return; }

			while ( true )
			{
				if ( ptr->data->iparam1 & 16 && ( ptr->data->fparam2 == 0.0f || ptr->data->fparam2 <= gpGlobals->time ) )
				{
					if ( gEngfuncs.pfnRandomFloat( 0.0f, 1.0f ) > 0.98f )
					{
						ptr->data->fparam2 = gpGlobals->time + 6.0f;
						gEngfuncs.pEventAPI->EV_PlaySound( -1, ptr->data->origin, CHAN_STATIC, "misc/teleport_ready.wav", VOL_NORM, 0.5f, 0, PITCH_NORM );
					}
				}

				if ( ptr->data->iparam1 & 128 )
				{
					gEngfuncs.pEventAPI->EV_PlaySound( -1, ptr->data->origin, CHAN_STATIC, "misc/teleport_out.wav", VOL_NORM, 0.5f, 0, PITCH_NORM );
					ptr->data->iparam1 &= ~128;
					ptr->data->iparam1 |= 4;
				}

				if ( !( ptr->data->iparam1 & 64 ) )
				{
					if ( !( ptr->data->iparam1 & 32 ) )
					{
						if ( ptr->data->iparam1 & 1 )
						{
							DoSparkSmokeEffect( ptr->data );
						}
					}
				}
			}
		}
	*/
}
