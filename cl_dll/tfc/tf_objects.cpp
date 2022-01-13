#include "../hud.h"
#include "../cl_util.h"
#include "../demo.h"
#include "../common/demo_api.h"
#include "bench.h"

#define TF_DEFS_ONLY
#include "tf_defs.h"

int HUD_NeedSpot( int *damage );
void HUD_GetLastOrg( float *org );
int HUD_CreateSniperDot( int damage, Vector p_viewangles, Vector p_origin, float *dotorigin );

extern Vector g_aimorg;

void Game_AddObjects( void )
{
	int damage;
	qboolean tfc;
	Vector origin, viewangles;
	Vector p_dot, p_target;
	unsigned char buf[32];

	tfc = atoi( gEngfuncs.PhysInfo_ValueForKey( "tfc" ) );

	if ( gEngfuncs.GetLocalPlayer() && tfc && ( gEngfuncs.GetLocalPlayer()->curstate.playerclass == PC_SNIPER || Bench_InStage( 3 ) ) )
	{
		if ( HUD_NeedSpot( &damage ) )
		{
			gEngfuncs.GetViewAngles( viewangles );
			HUD_GetLastOrg( origin );

			if ( Bench_InStage( 3 ) )
				damage = 250;

			if ( HUD_CreateSniperDot( damage, viewangles, origin, p_dot ) )
			{
				p_target = g_aimorg;
				Bench_SpotPosition( p_dot, p_target );
			}

			if ( gEngfuncs.pDemoAPI->IsRecording() )
			{
				*(int *)buf = TRUE;
				*(int *)&buf[4] = damage;
				*(Vector *)&buf[8] = viewangles;
				*(Vector *)&buf[20] = origin;
				Demo_WriteBuffer( TYPE_SNIPERDOT, 32, buf );
			}
		}
		else if ( gEngfuncs.pDemoAPI->IsRecording() )
		{
			*(int *)buf = FALSE;
			Demo_WriteBuffer( TYPE_SNIPERDOT, 4, buf );
		}
	}

	if ( gEngfuncs.pDemoAPI->IsPlayingback() && g_demosniper )
	{
		HUD_CreateSniperDot( g_demosniperdamage, g_demosniperangles, g_demosniperorg, p_dot );
	}
}