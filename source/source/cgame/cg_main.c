//-----------------------------------------------------------------------------
//
//  $Logfile:: /fakk2_code/fakk2_new/cgame/cg_main.c                          $
// $Revision:: 51                                                             $
//   $Author:: Aldie                                                          $
//     $Date:: 7/24/00 9:58p                                                  $
//
// Copyright (C) 1998 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
// $Log:: /fakk2_code/fakk2_new/cgame/cg_main.c                               $
// 
// 51    7/24/00 9:58p Aldie
// Fix for backtile
// 
// 50    7/24/00 11:52a Aldie
// Added cg_effectdetail for reducing number of effects
// 
// 49    7/16/00 11:01p Markd
// Added updateLoadingScreen calls
// 
// 48    7/15/00 12:32p Aldie
// Fixed a bug for restarting tempmodels on a save game
// 
// 47    7/10/00 11:54p Markd
// added exit level code
// 
// 46    7/07/00 2:58p Markd
// fixed alias bug
// 
// 45    6/28/00 4:36p Markd
// added debug footsteps code
// 
// 44    6/27/00 2:34p Markd
// clear out swipes when restarting
// 
// 43    6/26/00 9:36p Markd
// cleared out blood marks at the beginning of the level and also made sure
// that snapshots from previous games are not sent to the cgame
// 
// 42    6/17/00 1:54p Markd
// Added server restarted code
// 
// 41    6/14/00 12:14p Markd
// more intel compiler bug fixes
// 
// 40    6/09/00 10:27a Markd
// got lagometer and CG_2D operations functioning
// 
// 39    5/20/00 5:14p Markd
// Added ITEM special effects
// 
// 38    5/16/00 6:24p Markd
// added cg_traceinfo support
// 
// 37    5/11/00 11:06a Steven
// Changed the water ripple shader.
// 
// 36    5/08/00 6:12p Markd
// Added lightstyle support for the client
// 
// 35    5/02/00 7:20p Markd
// changed camera defaults
// 
// 34    3/31/00 11:48a Markd
// Added sky_portal control
// 
// 33    3/06/00 8:07p Markd
// cleaned up unused cvar's
// 
// 32    3/06/00 1:16p Markd
// fixed bug where sounds were overflowing their configstring range
// 
// 31    3/04/00 12:23p Markd
// Fixed some lightstyle bugs
// 
// 30    3/04/00 11:45a Markd
// Added light style support and malloc and free to the cgame
// 
// 29    2/21/00 7:03p Markd
// Added skyalpha support
// 
// 28    2/14/00 10:09a Markd
// Tweaked targeting reticles
// 
// 27    2/11/00 3:22p Aldie
// Changed swipe code a bit, and added cg_hidetempmodels cvar.
// 
// 26    2/08/00 6:24p Markd
// removed unused mediashader 
// 
// 25    1/28/00 6:11p Jimdose
// added CG_GetRendererConfig
// 
// 24    1/27/00 9:06a Markd
// Added watermark
// 
// 23    1/06/00 11:10p Jimdose
// removed unused commands
// 
// 22    12/11/99 5:51p Markd
// First wave of bug fixes after q3a gold merge
// 
// 21    12/11/99 3:37p Markd
// q3a gold checkin, first time
// 
// 20    10/29/99 7:23p Steven
// Moved sound stuff into snapshot.
// 
// 19    10/07/99 9:56a Markd
// made fov come out of playerstate, not cvar
// 
// 18    10/05/99 6:01p Aldie
// Added headers
//
// DESCRIPTION:
// Init functions for the cgame

#include "cg_local.h"

#ifdef _WIN32
#include <windows.h>
#endif

clientGameImport_t	cgi;

cg_t		  cg;
cgs_t		  cgs;
centity_t  cg_entities[MAX_GENTITIES];

cvar_t *cg_animSpeed;
cvar_t *cg_debugFootsteps;
cvar_t *cg_debugAnim;
cvar_t *cg_debugAnimWatch;
cvar_t *cg_errorDecay;
cvar_t *cg_nopredict;
cvar_t *cg_showmiss;
cvar_t *cg_addMarks;
cvar_t *cg_viewsize;
cvar_t *cg_3rd_person;
cvar_t *cg_syncronousClients;
cvar_t *cg_stats;
cvar_t *cg_hidetempmodels;
cvar_t *paused;
cvar_t *r_lerpmodels;
cvar_t *cg_stereoSeparation;
cvar_t *cg_lagometer;
cvar_t *cg_cameraheight;
cvar_t *cg_cameradist;
cvar_t *cg_camerascale;
cvar_t *cg_cameraverticaldisplacement;
cvar_t *cg_traceinfo;
cvar_t *cg_shadows;
cvar_t *developer;

/*
=================
CG_RegisterCvars
=================
*/
void CG_RegisterCvars( void ) 
   {
   cvar_t * temp;

	cg_viewsize          = cgi.Cvar_Get( "viewsize",            "100", CVAR_ARCHIVE );
	cg_addMarks          = cgi.Cvar_Get( "cg_marks",            "1", CVAR_ARCHIVE );
	cg_animSpeed         = cgi.Cvar_Get( "cg_animspeed",        "1", CVAR_CHEAT );
	cg_debugAnim         = cgi.Cvar_Get( "cg_debuganim",        "0", CVAR_CHEAT );
   cg_debugAnimWatch    = cgi.Cvar_Get( "cg_debuganimwatch",   "0", CVAR_CHEAT );
	cg_errorDecay        = cgi.Cvar_Get( "cg_errordecay",       "100", 0 );
	cg_nopredict         = cgi.Cvar_Get( "cg_nopredict",        "0", 0 );
	cg_showmiss          = cgi.Cvar_Get( "cg_showmiss",         "0", 0 );
	cg_stats             = cgi.Cvar_Get( "cg_stats",            "0", 0 );
	cg_hidetempmodels    = cgi.Cvar_Get( "cg_hidetempmodels",   "0", 0 );
	cg_syncronousClients = cgi.Cvar_Get( "g_syncronousClients", "0", 0 );
   cg_stereoSeparation  = cgi.Cvar_Get( "cg_stereosep",        "0.4", CVAR_ARCHIVE ); 
   cg_lagometer         = cgi.Cvar_Get( "cg_lagometer",        "0", 0 ); 
   paused               = cgi.Cvar_Get( "paused",              "0", 0 );
   r_lerpmodels         = cgi.Cvar_Get( "r_lerpmodels",        "1", 0 );
	cg_3rd_person        = cgi.Cvar_Get( "cg_3rd_person",       "1", 0 );
   cg_cameraheight      = cgi.Cvar_Get ("cg_cameraheight", "18", CVAR_ARCHIVE);
	cg_cameradist        = cgi.Cvar_Get ("cg_cameradist" , "120", CVAR_ARCHIVE);
	cg_cameraverticaldisplacement = cgi.Cvar_Get ("cg_cameraverticaldisplacement" , "-2", CVAR_ARCHIVE);
	cg_camerascale       = cgi.Cvar_Get ("cg_camerascale" , "0.3", CVAR_ARCHIVE);
	cg_traceinfo         = cgi.Cvar_Get ("cg_traceinfo" , "0", CVAR_ARCHIVE);
	cg_debugFootsteps    = cgi.Cvar_Get( "cg_debugfootsteps",   "0", CVAR_CHEAT );

	cg_shadows           = cgi.Cvar_Get( "cg_shadows", "1", 0 );
	developer			   = cgi.Cvar_Get( "developer", "0", 0 );

	// see if we are also running the server on this machine
	temp                 = cgi.Cvar_Get( "sv_running", "0", 0 );
	cgs.localServer      = temp->integer;
   }

/*
=================
CG_RegisterSounds

called during a precache command
=================
*/
void CG_RegisterSounds( void ) {
	int		i;

   // process the global defs  
   for ( i = 0; i < 10; i++ )
      {
      char filename[ MAX_QPATH ];

      Com_sprintf( filename, sizeof( filename ), "global/global%i.scr", i );
      if ( !CG_Command_ProcessFile( filename, qtrue ) )
         {
         break;
         }
      }
}

/*
================
CG_ProcessConfigString
================
*/
void CG_ProcessConfigString( int num ) 
   {
	const char	*str;

	if ( num == CS_SOUNDTRACK ) 
      {
      cgi.UpdateLoadingScreen();
	   str = CG_ConfigString( num );
		cgi.MUSIC_NewSoundtrack( str );
      }
	if ( num == CS_FOGINFO ) 
      {
	   str = CG_ConfigString( num );
      sscanf
         ( 
         str, 
         "%d %f %f %f %f",
         &cg.farplane_cull, 
         &cg.farplane_distance,
         &cg.farplane_color[ 0 ],
         &cg.farplane_color[ 1 ],
         &cg.farplane_color[ 2 ]
         );
      }
	else if ( num == CS_SKYINFO ) 
      {
	   str = CG_ConfigString( num );
      sscanf
         ( 
         str, 
         "%f %d",
         &cg.sky_alpha,
         &cg.sky_portal
         );
      }
	else if ( num == CS_SERVERINFO ) 
      {
		CG_ParseServerinfo();
      }
	else if ( num == CS_LEVEL_START_TIME ) 
      {
	   str = CG_ConfigString( num );
		cgs.levelStartTime = atoi( str );
      }
	else if ( num >= CS_MODELS && num < CS_MODELS+MAX_MODELS ) 
      {
	   str = CG_ConfigString( num );
      if ( str && str[ 0 ] )
         {
		   cgs.model_draw[ num-CS_MODELS ] = cgi.R_RegisterModel( str );
         if ( strstr( str, ".tik" ) )
            {
            cgs.model_tiki[ num-CS_MODELS ] = cgi.TIKI_GetHandle( cgs.model_draw[ num-CS_MODELS ] );
            if ( cgs.model_tiki[ num-CS_MODELS ] >= 0 )
               CG_ProcessInitCommands( cgs.model_tiki[ num-CS_MODELS ] );
            }
         cgi.UpdateLoadingScreen();
         }
	   } 
   else if ( num >= CS_SOUNDS && num < CS_SOUNDS+MAX_SOUNDS ) 
      {
	   str = CG_ConfigString( num );
		if ( str && str[ 0 ] && ( str[0] != '*' ) ) 
         {
			cgs.sound_precache[ num-CS_SOUNDS] = cgi.S_RegisterSound( str );
         cgi.UpdateLoadingScreen();
		   }
      }
   else if ( num >= CS_LIGHTSTYLES && num < CS_LIGHTSTYLES+MAX_LIGHTSTYLES ) 
      {
	   str = CG_ConfigString( num );
      CG_SetLightStyle( num - CS_LIGHTSTYLES, str );
      }
   }

//===================================================================================

/*
=================
CG_PrepRefresh

Call before entering a new level, or after changing renderers
This function may execute for a couple of minutes with a slow disk.
=================
*/
void CG_PrepRefresh( void )
   {
	int i;

	memset( &cg.refdef, 0, sizeof( cg.refdef ) );

   cgi.R_ClearScene();

   // clear any tiki handles to bad values
   memset( &cgs.model_tiki, -1, sizeof( cgs.model_tiki ) );

	cgi.R_LoadWorldMap( cgs.mapname );

	// register the inline models
	cgs.numInlineModels = cgi.CM_NumInlineModels();

	for ( i=1; i<cgs.numInlineModels; i++ )
      {
		char	         name[10];
		vec3_t			mins, maxs;
		int				j;

		Com_sprintf( name, sizeof(name), "*%i", i );
		cgs.inlineDrawModel[i] = cgi.R_RegisterModel( name );
      cgi.R_ModelBounds( cgs.inlineDrawModel[i], mins, maxs );
		
      for ( j = 0 ; j < 3 ; j++ )
         {
			cgs.inlineModelMidpoints[i][j] = mins[j] + 0.5 * ( maxs[j] - mins[j] );
		   }
	   }
   // register media shaders
   cgs.media.shadowMarkShader    = cgi.R_RegisterShader( "markShadow" );
   cgs.media.wakeMarkShader      = cgi.R_RegisterShader( "ripple.spr" );
   cgs.media.itemRingShader      = cgi.R_RegisterShader( "item_ring" );
   cgs.media.leftTargetShader    = cgi.R_RegisterShader( "left_targeted" );
   cgs.media.rightTargetShader   = cgi.R_RegisterShader( "right_targeted" );
   cgs.media.leftTargetModel     = cgi.R_RegisterModel( "left_targeted.spr" );
   cgs.media.rightTargetModel    = cgi.R_RegisterModel( "right_targeted.spr" );
   cgs.media.lagometerShader     = cgi.R_RegisterShaderNoMip("gfx/2d/blank" );
   cgs.media.levelExitShader     = cgi.R_RegisterShaderNoMip( "textures/menu/exit" );
   cgs.media.pausedShader        = cgi.R_RegisterShaderNoMip( "textures/menu/paused" );
   cgs.media.backTileShader      = cgi.R_RegisterShader( "gfx/2d/backtile" );

   // go through all the configstrings and process them
	for ( i = CS_SYSTEMINFO + 1 ; i < MAX_CONFIGSTRINGS ; i++ ) 
      {
      CG_ProcessConfigString( i );
		}
   }

//===========================================================================

/*
=================
CG_ConfigString
=================
*/
const char *CG_ConfigString( int index ) {
	if ( index < 0 || index >= MAX_CONFIGSTRINGS ) {
		cgi.Error( ERR_DROP, "CG_ConfigString: bad index: %i", index );
	}
	return cgs.gameState.stringData + cgs.gameState.stringOffsets[ index ];
}

//==================================================================


void CG_GetRendererConfig( void )
   {
	// get the rendering configuration from the client system
  	cgi.GetGlconfig( &cgs.glconfig );
	cgs.screenXScale = cgs.glconfig.vidWidth  / 640.0;
	cgs.screenYScale = cgs.glconfig.vidHeight / 480.0;
   }

/*
======================
CG_GameStateReceived

Displays the info screen while loading media
======================
*/
void CG_GameStateReceived( void )
   {	
   const char *s;

   // clear everything
	memset( &cg,         0, sizeof( cg ) );
	memset( cg_entities, 0, sizeof(cg_entities) );

   // clear the light styles
   CG_ClearLightStyles();

	// get the rendering configuration from the client system
   CG_GetRendererConfig();

	// get the gamestate from the client system
	cgi.GetGameState( &cgs.gameState );

	// check version
	s = CG_ConfigString( CS_GAME_VERSION );
	if ( strcmp( s, GAME_VERSION ) ) {
		cgi.Error( ERR_DROP, "Client/Server game mismatch: %s/%s", GAME_VERSION, s );
	}

	s = CG_ConfigString( CS_LEVEL_START_TIME );
	cgs.levelStartTime = atoi( s );

	CG_ParseServerinfo();

	// load the new map
	cgi.CM_LoadMap( cgs.mapname );

	CG_InitMarkPolys();

   CG_RegisterSounds();
   
	CG_PrepRefresh();

	// remove the last loading update
	cg.infoScreenText[0] = 0;
   }  


/*
======================
CG_ServerRestarted

The server has beeen restarted, adjust our cgame data accordingly
======================
*/
void CG_ServerRestarted( void )
   {	
   int timedelta;
   const char * s;

	s = CG_ConfigString( CS_LEVEL_START_TIME );
	cgs.levelStartTime = atoi( s );

	CG_ParseServerinfo();

	cg.thisFrameTeleport = qtrue;
	cg.lastCameraTime = -1;
   timedelta = cg.oldTime - cg.time;
   // free up any temp models currently spawned
   CG_RestartCommandManager( timedelta );
   // restart beams
   CG_RestartBeams( timedelta );
   // get rid of left over decals from the last game
	CG_InitMarkPolys();
   // clear all the swipes
   CG_ClearSwipes();
   // Reset tempmodels
   CG_ResetTempModels();
   }  

/*
=================
CG_Init

Called after every level change or subsystem restart
=================
*/
void CG_Init( clientGameImport_t *imported, int serverMessageNum, int serverCommandSequence ) {

 	cgi = *imported;

	memset( &cg, 0, sizeof( cg ) );
	memset( &cgs, 0, sizeof( cgs ) );

	cgs.processedSnapshotNum = serverMessageNum;
	cgs.serverCommandSequence = serverCommandSequence;

	CG_RegisterCvars();

   L_InitEvents();

   // init swapping for endian conversion
	Swap_Init ();

	CG_InitializeCommandManager();

	CG_GameStateReceived();

	CG_InitConsoleCommands();
   }

/*
=================
CG_Shutdown

Called before every level change or subsystem restart
=================
*/
void CG_Shutdown( void ) 
   {
   L_ShutdownEvents();
	// some mods may need to do cleanup work here,
	// like closing files or archiving session data
   }


/*
================
GetCGameAPI

The only exported function from this module
================
*/
clientGameExport_t	*GetCGameAPI( void )
   {
	static clientGameExport_t	cge;

	cge.CG_Init             = CG_Init;
   cge.CG_DrawActiveFrame  = CG_DrawActiveFrame;
   cge.CG_Shutdown         = CG_Shutdown;
   cge.CG_ConsoleCommand   = CG_ConsoleCommand;
   cge.CG_GetRendererConfig = CG_GetRendererConfig;
   cge.CG_Draw2D           = CG_Draw2D;

	return &cge;
   }

/*
=====================
CG_DrawActive

Perform all drawing needed to completely fill the screen
=====================
*/
void CG_DrawActive( stereoFrame_t stereoView )
   {
	float		separation;
	vec3_t		baseOrg;

	switch ( stereoView )
      {
	   case STEREO_CENTER:
		   separation = 0;
		   break;
	   case STEREO_LEFT:
		   separation = -cg_stereoSeparation->value / 2;
		   break;
	   case STEREO_RIGHT:
		   separation = cg_stereoSeparation->value / 2;
		   break;
	   default:
		   separation = 0;
		   cgi.Error( ERR_DROP, "CG_DrawActive: Undefined stereoView" );
   	}

	// clear around the rendered view if sized down
	CG_TileClear();

	// offset vieworg appropriately if we're doing stereo separation
	VectorCopy( cg.refdef.vieworg, baseOrg );
	if ( separation != 0 )
      {
		VectorMA( cg.refdef.vieworg, -separation, cg.refdef.viewaxis[1], cg.refdef.vieworg );
	   }

	// draw 3D view
   cgi.R_RenderScene( &cg.refdef );
	
	// restore original viewpoint if running stereo
	if ( separation != 0 )
      {
		VectorCopy( baseOrg, cg.refdef.vieworg );
	   }
   }

#ifndef CGAME_HARD_LINKED
// this is only here so the functions in q_shared.c and bg_*.c can link (FIXME)

void Com_Error( int level, const char *error, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);

	cgi.Error( level, "%s", text);
}

void Com_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	cgi.Printf( "%s", text);
}

#endif


