#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "tf_defs.h"

BOOL CTF_Map;
float autokick_kills;
BOOL birthday;
float cb_ceasefire_time;
float cb_prematch_time;
BOOL cease_fire;
BOOL christmas;
int civilianteams;
float clan_scores_dumped;
float deathmsg;
float fOldCeaseFire;
float fOldPrematch;
float flNextEqualisationCalc;
float flagem_checked;
int g_iOrderedTeams[5];
CBaseEntity *g_pLastSpawns[5];
int illegalclasses[5];
BOOL initial_cease_fire;
BOOL last_cease_fire;
float last_id;
BOOL no_cease_fire_text;
float num_world_flames;
float number_of_teams;
float old_grens;
Vector rgbcolors[5];

char *sClassCfgs[] = 
{
	"",
	"exec scout.cfg\n",
	"exec sniper.cfg\n",
	"exec soldier.cfg\n",
	"exec demoman.cfg\n",
	"exec medic.cfg\n",
	"exec hwguy.cfg\n",
	"exec pyro.cfg\n",
	"exec spy.cfg\n",
	"exec engineer.cfg\n",
	"",
	"exec civilian.cfg\n",
};

char *sClassModels[] = 
{
	"",
	"scout",
	"sniper",
	"soldier",
	"demo",
	"medic",
	"hvyweapon",
	"pyro",
	"spy",
	"engineer",
	"",
	"civilian"
};

char *sClassNames[] =
{
	"Observer",
	"Scout",
	"Sniper",
	"Soldier",
	"Demoman",
	"Medic",
	"HWGuy",
	"Pyro",
	"Spy",
	"Engineer",
	"RandomPC",
	"Civilian"
};

char *sGrenadeNames[] = 
{
	"",
	"Normal",
	"Concussion",
	"Nail",
	"Mirv",
	"Napalm",
	"",
	"Gas",
	"EMP",
	"Caltrop",
	""
};

char *sNewClassModelFiles[] =
{
	NULL,
	"models/player/scout/scout.mdl",
	"models/player/sniper/sniper.mdl",
	"models/player/soldier/soldier.mdl",
	"models/player/demo/demo.mdl",
	"models/player/medic/medic.mdl",
	"models/player/hvyweapon/hvyweapon.mdl",
	"models/player/pyro/pyro.mdl",
	"models/player/spy/spy.mdl",
	"models/player/engineer/engineer.mdl",
	"models/player.mdl",
	"models/player/civilian/civilian.mdl"
};

char *sOldClassModelFiles[] =
{
	NULL,
	"models/player/scout/scout2.mdl",
	"models/player/sniper/sniper2.mdl",
	"models/player/soldier/soldier2.mdl",
	"models/player/demo/demo2.mdl",
	"models/player/medic/medic2.mdl",
	"models/player/hvyweapon/hvyweapon2.mdl",
	"models/player/pyro/pyro2.mdl",
	"models/player/spy/spy2.mdl",
	"models/player/engineer/engineer2.mdl",
	"models/player.mdl",
	"models/player/civilian/civilian.mdl"
};

char *sTeamSpawnNames[] = 
{
	"",
	"ts1",
	"ts2",
	"ts3",
	"ts4"
};

float spy_off;
string_t team_menu_string;
string_t team_names[5];
float teamadvantage[5];
int teamallies[5];
team_color_t teamcolors[5][12];
int teamfrags[5];
int teamlives[5];
int teammaxplayers[5];
int teamscores[5];
int toggleflags;
static const char *pHallucinationSounds1[4];