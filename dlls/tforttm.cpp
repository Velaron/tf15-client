#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "tf_defs.h"

extern int gmsgTeamScore;

char *sTNameCvars[] = {
	"team1",
	"team2",
	"team3",
	"team4",
	"t1",
	"t2",
	"t3",
	"t4",
	"Blue",
	"Red",
	"Yellow",
	"Green"
};

char *GetTeamName( int tno )
{
	char *result = "";

	if ( tno > 0 )
	{
		if ( team_names[tno] )
			result = (char *)STRING( team_names[tno] );
		else
			result = sTNameCvars[tno + 7];
	}

	return result;
}

void TeamFortress_TeamSetColor( int tno )
{
	// Blue
	teamcolors[1][PC_SCOUT] = { 153, 139 };
	teamcolors[1][PC_SNIPER] = { 153, 145 };
	teamcolors[1][PC_SOLDIER] = { 153, 130 };
	teamcolors[1][PC_DEMOMAN] = { 153, 145 };
	teamcolors[1][PC_MEDIC] = { 153, 140 };
	teamcolors[1][PC_HVYWEAP] = { 148, 138 };
	teamcolors[1][PC_PYRO] = { 140, 145 };
	teamcolors[1][PC_SPY] = { 150, 145 };
	teamcolors[1][PC_ENGINEER] = { 140, 148 };
	teamcolors[1][PC_RANDOM] = { 150, 0 };
	teamcolors[1][PC_CIVILIAN] = { 150, 140 };

	// Red
	teamcolors[2][PC_SCOUT] = { 255, 10 };
	teamcolors[2][PC_SNIPER] = { 255, 10 };
	teamcolors[2][PC_SOLDIER] = { 250, 58 };
	teamcolors[2][PC_DEMOMAN] = { 255, 20 };
	teamcolors[2][PC_MEDIC] = { 255, 250 };
	teamcolors[2][PC_HVYWEAP] = { 255, 25 };
	teamcolors[2][PC_PYRO] = { 250, 25 };
	teamcolors[2][PC_SPY] = { 250, 240 };
	teamcolors[2][PC_ENGINEER] = { 5, 250 };
	teamcolors[2][PC_RANDOM] = { 250, 0 };
	teamcolors[2][PC_CIVILIAN] = { 250, 240 };

	// Yellow
	teamcolors[3][PC_SCOUT] = { 45, 35 };
	teamcolors[3][PC_SNIPER] = { 45, 35 };
	teamcolors[3][PC_SOLDIER] = { 45, 35 };
	teamcolors[3][PC_DEMOMAN] = { 45, 35 };
	teamcolors[3][PC_MEDIC] = { 45, 35 };
	teamcolors[3][PC_HVYWEAP] = { 45, 40 };
	teamcolors[3][PC_PYRO] = { 45, 35 };
	teamcolors[3][PC_SPY] = { 45, 35 };
	teamcolors[3][PC_ENGINEER] = { 45, 5 };
	teamcolors[3][PC_RANDOM] = { 45, 0 };
	teamcolors[3][PC_CIVILIAN] = { 45, 35 };

	// Green
	teamcolors[4][PC_SCOUT] = { 100, 90 };
	teamcolors[4][PC_SNIPER] = { 80, 90 };
	teamcolors[4][PC_SOLDIER] = { 100, 40 };
	teamcolors[4][PC_DEMOMAN] = { 100, 90 };
	teamcolors[4][PC_MEDIC] = { 100, 90 };
	teamcolors[4][PC_HVYWEAP] = { 100, 90 };
	teamcolors[4][PC_PYRO] = { 100, 50 };
	teamcolors[4][PC_SPY] = { 100, 90 };
	teamcolors[4][PC_ENGINEER] = { 100, 90 };
	teamcolors[4][PC_RANDOM] = { 100, 0 };
	teamcolors[4][PC_CIVILIAN] = { 100, 90 };

	rgbcolors[0] = { 255.0f, 255.0f, 255.0f };
	rgbcolors[1] = { 0.0f, 0.0f, 255.0f };
	rgbcolors[2] = { 255.0f, 0.0f, 0.0f };
	rgbcolors[3] = { 255.0f, 255.0f, 30.0f };
	rgbcolors[4] = { 0.0f, 255.0f, 0.0f };
}

// Velaron: TODO
int TeamFortress_TeamGetNoPlayers( int tno )
{
	return 0;
}

// Velaron: TODO
BOOL TeamFortress_SortTeams( void )
{
	int bGotScore[5];

	memset( bGotScore, 0, sizeof( bGotScore ) );
	memset( g_iOrderedTeams, 0, sizeof( g_iOrderedTeams ) );

	if ( number_of_teams < 1.0f )
		return TRUE;
}

void TeamFortress_TeamIncreaseScore( int tno, int scoretoadd )
{
	if ( tno <= 0 )
		return;
	
	teamscores[tno] += scoretoadd;

	MESSAGE_BEGIN( MSG_ALL, gmsgTeamScore );
	WRITE_STRING( GetTeamName( tno ) );
	WRITE_SHORT( teamscores[tno] );
	WRITE_SHORT( 0 );
	MESSAGE_END();
}

// Velaron: TODO
void CalculateTeamEqualiser( void )
{
	
}