//-----------------------------------------------------------------------------
//
//  $Logfile:: /fakk2_code/fakk2_new/cgame/cg_consolecmds.c                   $
// $Revision:: 9                                                              $
//   $Author:: Markd                                                          $
//     $Date:: 7/22/00 5:50p                                                  $
//
// Copyright (C) 1998 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
// $Log:: /fakk2_code/fakk2_new/cgame/cg_consolecmds.c                        $
// 
// 9     7/22/00 5:50p Markd
// added flushtikis support
// 
// 8     4/26/00 9:05p Markd
// Added client and cgame class commands
// 
// 7     10/18/99 1:59p Aldie
// Lots of fixes for beams and stuff
// 
// 6     10/13/99 3:26p Aldie
// Various fixes for particles, beams and lensflares
// 
// 5     10/06/99 3:23p Steven
// Added a dumpevents command.
// 
// 4     10/05/99 6:01p Aldie
// Added headers
//
// DESCRIPTION:
// text commands typed in at the local console, or executed by a key binding

#include "cg_local.h"

void CG_TargetCommand_f( void );

/*
=================
CG_SizeUp_f

Keybinding command
=================
*/
static void CG_SizeUp_f (void) {
	cgi.Cvar_Set("viewsize", va("%i",(int)(cg_viewsize->integer+10)));
}


/*
=================
CG_SizeDown_f

Keybinding command
=================
*/
static void CG_SizeDown_f (void) {
	cgi.Cvar_Set("viewsize", va("%i",(int)(cg_viewsize->integer-10)));
}


/*
=============
CG_Viewpos_f

Debugging command to print the current position
=============
*/
static void CG_Viewpos_f (void) {
	cgi.Printf("(%i %i %i) : %i\n", (int)cg.refdef.vieworg[0],
		(int)cg.refdef.vieworg[1], (int)cg.refdef.vieworg[2], 
		(int)cg.refdefViewAngles[YAW]);
}

#if 0
static void CG_ScoresDown_f( void ) {
	// don't constantly send requests, or we would overflow
	if ( cg.showScores && cg.scoresRequestTime + 2000 > cg.time ) {
		return;
	}
	cg.scoresRequestTime = cg.time;
	cgi.SendClientCommand( "score" );

	if ( !cg.showScores ) {
		// don't display anything until first score returns
		cg.showScores = qtrue;
		cg.numScores = 0;
	}
}

static void CG_ScoresUp_f( void ) {
	cg.showScores = qfalse;
	cg.scoreFadeTime = cg.time;
}

static void CG_InfoDown_f( void ) {
	cg.showInformation = qtrue;
}

static void CG_InfoUp_f( void ) {
	cg.showInformation = qfalse;
}


/*
=============================================================================

  MODEL TESTING

The viewthing and gun positioning tools from Q2 have been integrated and
enhanced into a single model testing facility.

Model viewing can begin with either "testmodel <modelname>" or "testgun <modelname>".

The names must be the full pathname after the basedir, like 
"models/weapons/v_launch/tris.md3" or "players/male/tris.md3"

Testmodel will create a fake entity 100 units in front of the current view
position, directly facing the viewer.  It will remain immobile, so you can
move around it to view it from different angles.

Testgun will cause the model to follow the player around and supress the real
view weapon model.  The default frame 0 of most guns is completely off screen,
so you will probably have to cycle a couple frames to see it.

"nextframe", "prevframe", "nextskin", and "prevskin" commands will change the
frame or skin of the testmodel.  These are bound to F5, F6, F7, and F8 in
q3default.cfg.

Note that none of the model testing features update while the game is paused, so
it may be convenient to test with deathmatch set to 1 so that bringing down the
console doesn't pause the game.

=============================================================================
*/

/*
=================
CG_TestModel_f

Creates an entity in front of the current position, which
can then be moved around
=================
*/
void CG_TestModel_f (void) {
	vec3_t		angles;

	memset( &cg.testModelEntity, 0, sizeof(cg.testModelEntity) );
	if ( cgi.Argc() < 2 ) {
		return;
	}

	Q_strncpyz (cg.testModelName, cgi.Argv( 1 ), MAX_QPATH );
	cg.testModelEntity.hModel = cgi.R_RegisterModel( cg.testModelName );

	if ( cgi.Argc() == 3 ) {
		cg.testModelEntity.backlerp = atof( cgi.Argv( 2 ) );
		cg.testModelEntity.frame = 1;
		cg.testModelEntity.oldframe = 0;
	}
	if (! cg.testModelEntity.hModel ) {
		cgi.Printf( "Can't register model\n" );
		return;
	}

	VectorMA( cg.refdef.vieworg, 100, cg.refdef.viewaxis[0], cg.testModelEntity.origin );

	angles[PITCH] = 0;
	angles[YAW] = 180 + cg.refdefViewAngles[1];
	angles[ROLL] = 0;

	AnglesToAxis( angles, cg.testModelEntity.axis );
	cg.testGun = qfalse;
}

void CG_TestModelNextFrame_f (void) {
	cg.testModelEntity.frame++;
	cgi.Printf( "frame %i\n", cg.testModelEntity.frame );
}

void CG_TestModelPrevFrame_f (void) {
	cg.testModelEntity.frame--;
	cgi.Printf( "frame %i\n", cg.testModelEntity.frame );
}

void CG_TestModelNextSkin_f (void) {
	cg.testModelEntity.skinNum++;
	cgi.Printf( "skin %i\n", cg.testModelEntity.skinNum );
}

void CG_TestModelPrevSkin_f (void) {
	cg.testModelEntity.skinNum--;
	if ( cg.testModelEntity.skinNum < 0 ) {
		cg.testModelEntity.skinNum = 0;
	}
	cgi.Printf( "skin %i\n", cg.testModelEntity.skinNum );
}

void CG_AddTestModel (void) {
	// re-register the model, because the level may have changed
	cg.testModelEntity.hModel = cgi.R_RegisterModel( cg.testModelName );
	if (! cg.testModelEntity.hModel ) {
		cgi.Printf ("Can't register model\n");
		return;
	}
	cgi.R_AddRefEntityToScene( &cg.testModelEntity );
}


#endif

/*
=================
CG_FlushModels_f
=================
*/
void CG_FlushModels_f( void )
   {
   int i;

   // clear any tiki handles to bad values
   memset( &cgs.model_tiki, -1, sizeof( cgs.model_tiki ) );

   // reset all emitters
   CG_FlushCommandManager();

   // flush all the tikis
   cgi.FlushAll();

   // go through all the models and process them
	for ( i = CS_MODELS ; i < MAX_MODELS ; i++ ) 
      {
      CG_ProcessConfigString( i );
		}
   }



typedef struct {
	char	*cmd;
	void	(*function)(void);
} consoleCommand_t;

static consoleCommand_t	commands[] = {
//	{ "testmodel", CG_TestModel_f },
//	{ "nextframe", CG_TestModelNextFrame_f },
//	{ "prevframe", CG_TestModelPrevFrame_f },
//	{ "nextskin", CG_TestModelNextSkin_f },
//	{ "prevskin", CG_TestModelPrevSkin_f },
	{ "viewpos", CG_Viewpos_f },
//	{ "+scores", CG_ScoresDown_f },
//	{ "-scores", CG_ScoresUp_f },
//	{ "+info", CG_InfoDown_f },
//	{ "-info", CG_InfoUp_f },
	{ "sizeup", CG_SizeUp_f },
	{ "sizedown", CG_SizeDown_f },
	{ "cg_eventlist", CG_EventList_f },
	{ "cg_eventhelp", CG_EventHelp_f },
	{ "cg_dumpevents", CG_DumpEventHelp_f },
	{ "cg_pendingevents", CG_PendingEvents_f },
	{ "cg_classlist", CG_ClassList_f },
	{ "cg_classtree", CG_ClassTree_f },
   { "cg_classevents", CG_ClassEvents_f },
   { "cg_dumpclassevents", CG_DumpClassEvents_f },
   { "cg_dumpallclasses", CG_DumpAllClasses_f },
   { "testemitter", CG_TestEmitter_f },
   { "dumpemitter", CG_DumpEmitter_f },
   { "flushtikis", CG_FlushModels_f }
};


/*
=================
CG_ConsoleCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
qboolean CG_ConsoleCommand( void ) {
	const char	*cmd;
	int		i;

	cmd = cgi.Argv(0);

	for ( i = 0 ; i < sizeof( commands ) / sizeof( commands[0] ) ; i++ ) {
		if ( !Q_stricmp( cmd, commands[i].cmd ) ) {
			commands[i].function();
			return qtrue;
		}
	}

	return qfalse;
}


/*
=================
CG_InitConsoleCommands

Let the client system know about all of our commands
so it can perform tab completion
=================
*/
void CG_InitConsoleCommands( void ) {
	int		i;

	for ( i = 0 ; i < sizeof( commands ) / sizeof( commands[0] ) ; i++ ) {
		cgi.AddCommand( commands[i].cmd );
	}
}
