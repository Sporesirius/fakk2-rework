//-----------------------------------------------------------------------------
//
//  $Logfile:: /fakk2_code/fakk2_new/fgame/soundman.cpp                       $
// $Revision:: 15                                                             $
//   $Author:: Markd                                                          $
//     $Date:: 3/02/00 7:05p                                                  $
//
// Copyright (C) 1997 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
// $Log:: /fakk2_code/fakk2_new/fgame/soundman.cpp                            $
// 
// 15    3/02/00 7:05p Markd
// Fixed soundman bug with highlighting facets
// 
// 14    3/02/00 6:55p Markd
// Added reverb triggers
// 
// 13    1/22/00 12:42p Jimdose
// got rid of calls to vec3()
// 
// 12    1/19/00 10:26a Markd
// added snd globaltranslate command
// 
// 11    1/03/00 11:40a Markd
// fixed typo in saved file
// 
// 10    12/20/99 4:28p Markd
// Fixed music trigger bugs and music bugs
// 
// 9     12/20/99 3:00p Markd
// fixed more music system bugs
// 
// 8     12/20/99 11:41a Markd
// Fixed a music trigger issue
// 
// 7     12/01/99 12:19p Markd
// added snd_type and changed some of the entity colors
// 
// 6     11/12/99 7:15p Markd
// Put in better distinguishing code for one time MusicTriggers
// 
// 5     11/12/99 6:52p Markd
// fixed up sound manager saving and loading
// 
// 4     11/12/99 4:29p Markd
// Fixed music trigger origin problem and also added dir arrow for music
// triggers
// 
// 3     11/12/99 11:48a Markd
// Added default bounding box to music triggers
// 
// 2     11/12/99 11:47a Markd
// Added sound manager support and retro-fitted TriggerSpeaker and TriggerMusic
// to work with everything
// 
// 1     11/11/99 10:59a Markd
// 
//
// DESCRIPTION:
// Sound Manager
//

#include "g_local.h"
#include "entity.h"
#include "trigger.h"
#include "player.h"
#include "soundman.h"

SoundManager SoundMan;

/******************************************************************************

  Sound Manager

******************************************************************************/

Event EV_SoundManager_AddSpeaker
	(
	"addspeaker", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Add a new sound where the player is standing."
	);
Event EV_SoundManager_AddRandomSpeaker
	(
	"addrandomspeaker", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Add a new sound where the player is standing."
	);
Event EV_SoundManager_AddMusicTrigger
	(
	"addmusictrigger", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Add a new music trigger where the player is standing."
	);
Event EV_SoundManager_AddReverbTrigger
	(
	"addreverbtrigger", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Add a new reverb trigger where the player is standing."
	);
Event EV_SoundManager_Replace
	( 
	"replace", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Replace the current sound position with the player's."
	);
Event EV_SoundManager_Delete
	( 
	"delete", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Delete the current sound."
	);
Event EV_SoundManager_MovePlayer
	( 
	"moveplayer", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Move the player to the current sound position."
	);
Event EV_SoundManager_Next
	( 
	"next", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Go to the next sound."
	);
Event EV_SoundManager_Previous
	( 
	"prev", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Go to the previous sound."
	);

Event EV_SoundManager_Show
	( 
	"show", 
	EV_CONSOLE,
   "E",
   "path",
   "Show all the sounds."
	);

Event EV_SoundManager_ShowingSounds
	( 
	"_showing_sounds", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Internal event for showing the sounds."
	);
Event EV_SoundManager_Hide
	( 
	"hide", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Hides the sounds."
	);

Event EV_SoundManager_Save
	( 
	"save", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Saves the sounds."
	);
Event EV_SoundManager_UpdateInput
	( 
	"updateinput", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Updates the current sound with user interface values."
	);

Event EV_SoundManager_Reset
	( 
	"reset", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Resets the state of all sounds and triggers."
	);

Event EV_SoundManager_GlobalTranslate
	( 
	"globaltranslate", 
	EV_CONSOLE,
   "v",
   "translate_amount",
   "Translates all sounds and triggers by specified amount."
	);

Event EV_SoundManager_SwitchFacet
	( 
	"switchfacet", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Switch the current facet that we are editing."
	);

Event EV_SoundManager_PreviewReverb
	( 
	"previewreverb", 
	EV_CONSOLE,
   NULL,
   NULL,
   "Test out the current reverb settings."
	);

Event EV_SoundManager_ResetReverb
	( 
	"resetreverb", 
	EV_CONSOLE,
   NULL,
   NULL,
   "reset the reverb settings to a normal."
	);

CLASS_DECLARATION( Listener, SoundManager, NULL )
	{
		//chrissstrahl - code incompatibility fix
		// added & on right side (multiple occurences fixed)
		{ &EV_SoundManager_AddSpeaker,			&AddSpeaker },
		{ &EV_SoundManager_AddRandomSpeaker,	&AddRandomSpeaker },
		{ &EV_SoundManager_AddMusicTrigger,		&AddMusicTrigger },
		{ &EV_SoundManager_AddReverbTrigger,	&AddReverbTrigger },
		{ &EV_SoundManager_Replace,				&Replace },
		{ &EV_SoundManager_Delete,				&Delete },
		{ &EV_SoundManager_MovePlayer,			&MovePlayer },
		{ &EV_SoundManager_Next,				&Next },
		{ &EV_SoundManager_Previous,			&Previous },
		{ &EV_SoundManager_Show,				&Show },
		{ &EV_SoundManager_ShowingSounds,		&ShowingSounds },
		{ &EV_SoundManager_Hide,				&Hide },
		{ &EV_SoundManager_Save,				&Save },
		{ &EV_SoundManager_UpdateInput,			&UpdateEvent },
		{ &EV_SoundManager_Reset,				&ResetEvent },
		{ &EV_SoundManager_GlobalTranslate,		&GlobalTranslateEvent },
		{ &EV_SoundManager_SwitchFacet,			&SwitchFacetEvent },
		{ &EV_SoundManager_PreviewReverb,		&PreviewReverbEvent },
		{ &EV_SoundManager_ResetReverb,			&ResetReverbEvent },
		{ NULL,	NULL }
	};

Player *SoundManager_GetPlayer
   (
   void
   )

   {
   assert( g_entities[ 0 ].entity && g_entities[ 0 ].entity->isSubclassOf( Player ) );
   if ( !g_entities[ 0 ].entity || !( g_entities[ 0 ].entity->isSubclassOf( Player ) ) )
      {
      gi.Printf( "No player found.\n" );
      return NULL;
      }

   return ( Player * )g_entities[ 0 ].entity;
   }

SoundManager::SoundManager()
   {
   currentFacet = 0;
   Reset();
   }

void SoundManager::Reset
   (
   void
   )
   {
   current = NULL;
   soundList.ClearObjectList();
   }

void SoundManager::UpdateUI
   (
   void
   )
   {
   if ( current )
      {
      gi.cvar_set( "snd_multifaceted", "" );
      gi.cvar_set( "snd_currentfacet", "" );
      gi.cvar_set( "snd_onetime", "0" );
      gi.cvar_set( "snd_useangles", "0" );
      gi.cvar_set( "snd_yaw", "0" );

      gi.cvar_set( "snd_origin", va( "%.2f %.2f %.2f", current->origin[ 0 ], current->origin[ 1 ], current->origin[ 2 ] ) );
      gi.cvar_set( "snd_targetname", current->targetname.c_str() );
      if ( current->isSubclassOf( TriggerSpeaker ) )
         {
         TriggerSpeaker * speaker;

         speaker = ( TriggerSpeaker * )current;
         if ( speaker->volume != DEFAULT_VOL )
            {
            gi.cvar_set( "snd_volume", va( "%.1f", speaker->volume ) );
            }
         else
            {
            gi.cvar_set( "snd_volume", "Default" );
            }
         if ( speaker->min_dist != DEFAULT_MIN_DIST )
            {
            gi.cvar_set( "snd_mindist", va( "%.1f", speaker->min_dist ) );
            }
         else
            {
            gi.cvar_set( "snd_mindist", "Default" );
            }

         // setup the sound
         gi.cvar_set( "ui_pickedsound", speaker->Noise().c_str() );

         if ( current->isSubclassOf( RandomSpeaker ) )
            {
            RandomSpeaker * random;

            random = ( RandomSpeaker * )current;
            gi.cvar_set( "snd_mindelay", va( "%.1f", random->mindelay ) );
            gi.cvar_set( "snd_maxdelay", va( "%.1f", random->maxdelay ) );
            gi.cvar_set( "snd_channel", va( "%d", random->channel ) );
            gi.cvar_set( "snd_chance", va( "%.1f", random->chance ) );
            gi.cvar_set( "snd_type", "RandomSpeaker" );
            }
         else
            {
            gi.cvar_set( "snd_type", "Speaker" );
            }
         }
      else if ( current->isSubclassOf( TriggerMusic )  || current->isSubclassOf( TriggerReverb ) )
         {
         int      multiFaceted;
         Trigger  *trigger;

         trigger = ( Trigger * )current;
         gi.cvar_set( "snd_width", va( "%.0f", trigger->maxs[ 0 ] ) );
         gi.cvar_set( "snd_length", va( "%.0f", trigger->maxs[ 1 ] ) );
         gi.cvar_set( "snd_height", va( "%.0f", trigger->maxs[ 2 ] ) );
         if ( trigger->UsingTriggerDir() )
            {
            gi.cvar_set( "snd_useangles", "1" );
            }
         else
            {
            gi.cvar_set( "snd_useangles", "0" );
            }
         gi.cvar_set( "snd_yaw", va( "%.0f", trigger->angles[ 1 ] ) );
         multiFaceted = trigger->GetMultiFaceted();
         if ( multiFaceted )
            {
            if ( multiFaceted == 1 )
               {
               gi.cvar_set( "snd_multifaceted", "North/South" );
               if ( currentFacet )
                  {
                  gi.cvar_set( "snd_currentfacet", "South" );
                  }
               else
                  {
                  gi.cvar_set( "snd_currentfacet", "North" );
                  }
               }
            else
               {
               gi.cvar_set( "snd_multifaceted", "East/West" );
               if ( currentFacet )
                  {
                  gi.cvar_set( "snd_currentfacet", "West" );
                  }
               else
                  {
                  gi.cvar_set( "snd_currentfacet", "East" );
                  }
               }
            }
         else
            {
            gi.cvar_set( "snd_multifaceted", "Not" );
            }

         if ( current->isSubclassOf( TriggerMusic ) )
            {
            TriggerMusic * music;

            music = ( TriggerMusic * )trigger;

            gi.cvar_set( "snd_type", "MusicTrigger" );
            if ( music->oneshot )
               {
               gi.cvar_set( "snd_onetime", "1" );
               }
            else
               {
               gi.cvar_set( "snd_onetime", "0" );
               }

            if ( !currentFacet )
               {
               gi.cvar_set( "snd_currentmood", music->current.c_str() );
               gi.cvar_set( "snd_fallbackmood", music->fallback.c_str() );
               }
            else
               {
               gi.cvar_set( "snd_currentmood", music->altcurrent.c_str() );
               gi.cvar_set( "snd_fallbackmood", music->altfallback.c_str() );
               }
            }
         else if ( current->isSubclassOf( TriggerReverb ) )
            {
            TriggerReverb * reverb;

            reverb = ( TriggerReverb * )trigger;

            gi.cvar_set( "snd_type", "ReverbTrigger" );
            if ( reverb->oneshot )
               {
               gi.cvar_set( "snd_onetime", "1" );
               }
            else
               {
               gi.cvar_set( "snd_onetime", "0" );
               }

            if ( !currentFacet )
               {
               gi.cvar_set( "snd_reverbtypedisplay", EAXMode_NumToName( reverb->reverbtype ) );
               gi.cvar_set( "snd_reverbtype", va( "%d", reverb->reverbtype ) );
               gi.cvar_set( "snd_reverblevel", va( "%.2f", reverb->reverblevel ) );
               }
            else
               {
               gi.cvar_set( "snd_reverbtypedisplay", EAXMode_NumToName( reverb->altreverbtype ) );
               gi.cvar_set( "snd_reverbtype", va( "%d", reverb->altreverbtype ) );
               gi.cvar_set( "snd_reverblevel", va( "%.2f", reverb->altreverblevel ) );
               }
            }
         }
      if ( EventPending( EV_SoundManager_ShowingSounds ) )
         {
         gi.cvar_set( "snd_hiddenstate", "visible" );
         }
      else
         {
         gi.cvar_set( "snd_hiddenstate", "hidden" );
         }
      gi.cvar_set( "snd_speakernum", va( "%d", soundList.IndexOfObject( current ) - 1 ) );
      }
   }

void SoundManager::UpdateRandomSpeaker
   (
   RandomSpeaker * speaker
   )
   {
   cvar_t * cvar;
   Vector tempvec;

   if ( !speaker )
      {
      return;
      }

   // get origin
   cvar = gi.cvar( "snd_origin", "", 0 );
   sscanf( cvar->string, "%f %f %f", &tempvec[ 0 ], &tempvec[ 1 ], &tempvec[ 2 ] );
   speaker->setOrigin( tempvec );

   // get targetname
   cvar = gi.cvar( "snd_targetname", "", 0 );
   speaker->SetTargetName( cvar->string );

   // get volume
   cvar = gi.cvar( "snd_volume", "", 0 );
   if ( str( cvar->string ) == "Default" )
      {
      speaker->SetVolume( DEFAULT_VOL );
      }
   else
      {
      speaker->SetVolume( cvar->value );
      }

   // get min_dist
   cvar = gi.cvar( "snd_mindist", "", 0 );
   if ( str( cvar->string ) == "Default" )
      {
      speaker->SetMinDist( DEFAULT_MIN_DIST );
      }
   else
      {
      speaker->SetMinDist( cvar->value );
      }

   // get mindelay
   cvar = gi.cvar( "snd_mindelay", "", 0 );
   speaker->SetMinDelay( cvar->value );

   // get maxdelay
   cvar = gi.cvar( "snd_maxdelay", "", 0 );
   speaker->SetMaxDelay( cvar->value );

   // get chance
   cvar = gi.cvar( "snd_chance", "", 0 );
   speaker->SetChance( cvar->value );

   // get sound
   cvar = gi.cvar( "ui_pickedsound", "", 0 );
   if ( cvar->string[ 0 ] )
      {
      speaker->SetNoise( cvar->string );
      speaker->ScheduleSound();
      }
   }


void SoundManager::UpdateSpeaker
   (
   TriggerSpeaker * speaker
   )
   {
   cvar_t * cvar;
   Vector tempvec;

   if ( !speaker )
      {
      return;
      }

   //
   // make sure the speaker is an ambient speaker
   //
   speaker->ambient = true;

   // get origin
   cvar = gi.cvar( "snd_origin", "", 0 );
   sscanf( cvar->string, "%f %f %f", &tempvec[ 0 ], &tempvec[ 1 ], &tempvec[ 2 ] );
   speaker->setOrigin( tempvec );

   // get targetname
   cvar = gi.cvar( "snd_targetname", "", 0 );
   speaker->SetTargetName( cvar->string );

   // get volume
   cvar = gi.cvar( "snd_volume", "", 0 );
   if ( str( cvar->string ) == "Default" )
      {
      speaker->SetVolume( DEFAULT_VOL );
      }
   else
      {
      speaker->SetVolume( cvar->value );
      }

   // get min_dist
   cvar = gi.cvar( "snd_mindist", "", 0 );
   if ( str( cvar->string ) == "Default" )
      {
      speaker->SetMinDist( DEFAULT_MIN_DIST );
      }
   else
      {
      speaker->SetMinDist( cvar->value );
      }

   // get sound
   cvar = gi.cvar( "ui_pickedsound", "", 0 );
   if ( cvar->string[ 0 ] )
      {
      speaker->SetNoise( cvar->string );
      speaker->StartSound();
      }
   }

void SoundManager::UpdateTriggerMusic
   (
   TriggerMusic * music
   )
   {
   cvar_t   *cvar;
   str      current;
   str      fallback;
   str      faceted;
   Vector   tempvec;

   if ( !music )
      {
      return;
      }

   //
   // go to a known state
   //

   music->SetOneShot( false );
   // no angle use
   music->useTriggerDir = false;
   // make sure it doesn't trigger
   music->triggerable = false;

   // get origin
   cvar = gi.cvar( "snd_origin", "", 0 );
   sscanf( cvar->string, "%f %f %f", &tempvec[ 0 ], &tempvec[ 1 ], &tempvec[ 2 ] );
   music->setOrigin( tempvec );

   // get targetname
   cvar = gi.cvar( "snd_targetname", "", 0 );
   music->SetTargetName( cvar->string );

   // get width
   cvar = gi.cvar( "snd_width", "", 0 );
   music->mins[ 0 ] = -cvar->value;
   music->maxs[ 0 ] = cvar->value;

   // get length
   cvar = gi.cvar( "snd_length", "", 0 );
   music->mins[ 1 ] = -cvar->value;
   music->maxs[ 1 ] = cvar->value;

   // get height
   cvar = gi.cvar( "snd_height", "", 0 );
   music->mins[ 2 ] = 0;
   music->maxs[ 2 ] = cvar->value;

   music->setSize( music->mins, music->maxs );

   // get current multi faceted ness
   cvar = gi.cvar( "snd_multifaceted", "", 0 );
   faceted = cvar->string;

   if ( faceted == "North/South" )
      {
      music->SetMultiFaceted( 1 );
      }
   else if ( faceted == "East/West" )
      {
      music->SetMultiFaceted( 2 );
      }
   else
      {
      music->SetMultiFaceted( 0 );
      }

   // get current mood
   cvar = gi.cvar( "snd_currentmood", "", 0 );
   current = cvar->string;

   // get fallback mood
   cvar = gi.cvar( "snd_fallbackmood", "", 0 );
   fallback = cvar->string;

   if ( music->multiFaceted && currentFacet )
      {
      music->SetAltMood( current, fallback );
      }
   else
      {
      music->SetMood( current, fallback );
      }

   // get onetime
   cvar = gi.cvar( "snd_onetime", "", 0 );
   if ( cvar->integer )
      {
      music->SetOneShot( true );
      }

   // get yaw
   cvar = gi.cvar( "snd_yaw", "", 0 );
   music->angles[ YAW ] = cvar->value;
   music->setAngles();

   // get useangles
   cvar = gi.cvar( "snd_useangles", "", 0 );
   if ( cvar->integer )
      {
      music->SetTriggerDir( music->angles[ YAW ] );
      }
   UpdateUI();
   }

void SoundManager::UpdateTriggerReverb
   (
   TriggerReverb * reverb
   )
   {
   cvar_t   *cvar;
   int      reverbtype;
   float    reverblevel;
   str      faceted;
   Vector   tempvec;

   if ( !reverb )
      {
      return;
      }

   //
   // go to a known state
   //

   reverb->SetOneShot( false );
   // no angle use
   reverb->useTriggerDir = false;
   // make sure it doesn't trigger
   reverb->triggerable = false;

   // get origin
   cvar = gi.cvar( "snd_origin", "", 0 );
   sscanf( cvar->string, "%f %f %f", &tempvec[ 0 ], &tempvec[ 1 ], &tempvec[ 2 ] );
   reverb->setOrigin( tempvec );

   // get targetname
   cvar = gi.cvar( "snd_targetname", "", 0 );
   reverb->SetTargetName( cvar->string );

   // get width
   cvar = gi.cvar( "snd_width", "", 0 );
   reverb->mins[ 0 ] = -cvar->value;
   reverb->maxs[ 0 ] = cvar->value;

   // get length
   cvar = gi.cvar( "snd_length", "", 0 );
   reverb->mins[ 1 ] = -cvar->value;
   reverb->maxs[ 1 ] = cvar->value;

   // get height
   cvar = gi.cvar( "snd_height", "", 0 );
   reverb->mins[ 2 ] = 0;
   reverb->maxs[ 2 ] = cvar->value;

   reverb->setSize( reverb->mins, reverb->maxs );

   // get current multi faceted ness
   cvar = gi.cvar( "snd_multifaceted", "", 0 );
   faceted = cvar->string;

   if ( faceted == "North/South" )
      {
      reverb->SetMultiFaceted( 1 );
      }
   else if ( faceted == "East/West" )
      {
      reverb->SetMultiFaceted( 2 );
      }
   else
      {
      reverb->SetMultiFaceted( 0 );
      }

   // get reverb type
   cvar = gi.cvar( "snd_reverbtype", "", 0 );
   reverbtype = cvar->integer;

   // get reverb level
   cvar = gi.cvar( "snd_reverblevel", "", 0 );
   reverblevel = cvar->value;

   if ( reverb->multiFaceted && currentFacet )
      {
      reverb->SetAltReverb( reverbtype, reverblevel );
      }
   else
      {
      reverb->SetReverb( reverbtype, reverblevel );
      }

   // get onetime
   cvar = gi.cvar( "snd_onetime", "", 0 );
   if ( cvar->integer )
      {
      reverb->SetOneShot( true );
      }

   // get yaw
   cvar = gi.cvar( "snd_yaw", "", 0 );
   reverb->angles[ YAW ] = cvar->value;
   reverb->setAngles();

   // get useangles
   cvar = gi.cvar( "snd_useangles", "", 0 );
   if ( cvar->integer )
      {
      reverb->SetTriggerDir( reverb->angles[ YAW ] );
      }
   UpdateUI();
   }

void SoundManager::UpdateEvent
   (
   Event *ev
   )
   {
   if ( !current )
      {
      return;
      }
   if ( current->isSubclassOf( RandomSpeaker ) )
      {
      UpdateRandomSpeaker( ( RandomSpeaker * )current );
      }
   else if ( current->isSubclassOf( TriggerSpeaker ) )
      {
      UpdateSpeaker( ( TriggerSpeaker * )current );
      }
   else if ( current->isSubclassOf( TriggerMusic ) )
      {
      UpdateTriggerMusic( ( TriggerMusic * )current );
      }
   else if ( current->isSubclassOf( TriggerReverb ) )
      {
      UpdateTriggerReverb( ( TriggerReverb * )current );
      }
   }

void SoundManager::AddSpeaker
   (
   Event *ev
   )

   {
   Player *player;
   Vector ang;
   Vector pos;

   player = SoundManager_GetPlayer();
   if ( player )
      {
      player->GetPlayerView( &pos, &ang );

      current = new TriggerSpeaker;
      current->setOrigin( pos );
      current->setAngles( ang );

      soundList.AddUniqueObject( current );

      Show();
      }
   UpdateUI();
   }

void SoundManager::AddRandomSpeaker
   (
   Event *ev
   )

   {
   Player *player;
   Vector ang;
   Vector pos;

   player = SoundManager_GetPlayer();
   if ( player )
      {
      player->GetPlayerView( &pos, &ang );

      current = new RandomSpeaker;
      current->setOrigin( pos );
      current->setAngles( ang );

      soundList.AddUniqueObject( current );

      Show();
      }
   UpdateUI();
   }

void SoundManager::AddMusicTrigger
   (
   Event *ev
   )

   {
   Player *player;
   Vector ang;

   player = SoundManager_GetPlayer();
   if ( player )
      {
      player->GetPlayerView( NULL, &ang );

      current = new TriggerMusic;
      // we grab the origin from the feet of the player
      current->setOrigin( player->origin );
      current->setAngles( ang );
      current->setSize( "-16 -16 0", "16 16 64" );
      // make sure it doesn't trigger
      ( ( TriggerMusic * )current )->triggerable = false;

      soundList.AddUniqueObject( current );

      Show();
      }
   UpdateUI();
   }

void SoundManager::AddReverbTrigger
   (
   Event *ev
   )

   {
   Player *player;
   Vector ang;

   player = SoundManager_GetPlayer();
   if ( player )
      {
      player->GetPlayerView( NULL, &ang );

      current = new TriggerReverb;
      // we grab the origin from the feet of the player
      current->setOrigin( player->origin );
      current->setAngles( ang );
      current->setSize( "-16 -16 0", "16 16 64" );
      // make sure it doesn't trigger
      ( ( TriggerReverb * )current )->triggerable = false;

      soundList.AddUniqueObject( current );

      Show();
      }
   UpdateUI();
   }

void SoundManager::Replace
   (
   Event *ev
   )

   {
   Player *player;
   Vector ang;
   Vector pos;

   player = SoundManager_GetPlayer();
   if ( current && player )
      {
      player->GetPlayerView( &pos, &ang );

      if ( current->isSubclassOf( TriggerMusic ) || current->isSubclassOf( TriggerReverb )  )
         {
         current->setOrigin( player->origin );
         }
      else
         {
         current->setOrigin( pos );
         }
      current->setAngles( ang );
      }
   UpdateUI();
   }

void SoundManager::Delete
   (
   Event *ev
   )

   {
   int index = 0;

   if ( !current )
      return;

   if ( soundList.ObjectInList( current ) )
      {
      index = soundList.IndexOfObject( current );
      // remove the speaker
      soundList.RemoveObject( current );
      }
   current->PostEvent( EV_Remove, 0 );

   if ( ( index > 0 ) && ( index < soundList.NumObjects() ) )
      {
      current = soundList.ObjectAt( index );
      CurrentGainsFocus();
      }
   else
      {
      current = NULL;
      }
   
   UpdateUI();
   }

void SoundManager::MovePlayer
   (
   Event *ev
   )

   {
   Player *player;
   Vector pos;

   player = SoundManager_GetPlayer();
   if ( current && player )
      {
      player->GetPlayerView( &pos, NULL );

      if ( current->isSubclassOf( TriggerMusic ) || current->isSubclassOf( TriggerReverb ) )
         {
         player->setOrigin( current->origin );
         }
      else
         {
         player->setOrigin( current->origin - pos + player->origin );
         }
      player->SetViewAngles( current->angles );
      }
   }

void SoundManager::CurrentLostFocus
   (
   void
   )

   {
   if ( current )
      {
      if ( current->isSubclassOf( TriggerMusic ) || current->isSubclassOf( TriggerReverb ) )
         {
         current->PostEvent( EV_Trigger_SetTriggerable, 0.1f );
         }
      }
   }

void SoundManager::CurrentGainsFocus
   (
   void
   )

   {
   if ( current )
      {
      if ( current->isSubclassOf( TriggerMusic ) || current->isSubclassOf( TriggerReverb ) )
         {
         current->ProcessEvent( EV_Trigger_SetNotTriggerable );
         }
      }
   }

void SoundManager::Next
   (
   Event *ev
   )

   {
   int index;

   currentFacet = 0;

   if ( current )
      {
      //
      // find current sound in container of sounds
      //
      index = soundList.IndexOfObject( current );
      if ( index < soundList.NumObjects() )
         {
         index++;
         }
      else
         {
         index = 1;
         }
      CurrentLostFocus();
      }
   else
      {
      index = 1;
      }

   if ( index <= soundList.NumObjects() )
      {
      current = soundList.ObjectAt( index );
      CurrentGainsFocus();
      UpdateUI();
      }
   }

void SoundManager::Previous
   (
   Event *ev
   )

   {
   int index;

   currentFacet = 0;

   if ( current )
      {
      //
      // find current sound in container of sounds
      //
      index = soundList.IndexOfObject( current );
      if ( index > 1 )
         {
         index--;
         }
      else
         {
         index = soundList.NumObjects();
         }
      CurrentLostFocus();
      }
   else
      {
      index = 1;
      }

   if ( index <= soundList.NumObjects() )
      {
      current = soundList.ObjectAt( index );
      CurrentGainsFocus();
      UpdateUI();
      }
   }

void SoundManager::ResetEvent
   (
   Event *ev
   )
   {
   int i;
   Entity * ent;

   for( i = 1; i <= soundList.NumObjects(); i++ )
      {
      ent = soundList.ObjectAt( i );
      if ( ent->isSubclassOf( TriggerSpeaker ) )
         {
         // nothing to reset
         }
      else if ( ent->isSubclassOf( TriggerMusic ) )
         {
         TriggerMusic * music;

         music = ( TriggerMusic * )ent;
         music->SetOneShot( music->oneshot );
         }
      else if ( ent->isSubclassOf( TriggerReverb ) )
         {
         TriggerReverb * reverb;

         reverb = ( TriggerReverb * )ent;
         reverb->SetOneShot( reverb->oneshot );
         }
      }
   }

void SoundManager::GlobalTranslateEvent
   (
   Event *ev
   )
   {
   int i;
   Entity * ent;
   Vector amount;

   amount = ev->GetVector( 1 );

   for( i = 1; i <= soundList.NumObjects(); i++ )
      {
      ent = soundList.ObjectAt( i );
      ent->addOrigin( amount );
      }
   }

void SoundManager::SwitchFacetEvent
   (
   Event *ev
   )
   {
   if ( current && current->isSubclassOf( Trigger ) )
      {
      Trigger * trigger;

      trigger = ( Trigger * )current;
      if ( trigger->GetMultiFaceted() && !currentFacet )
         {
         currentFacet = 1;
         }
      else
         {
         currentFacet = 0;
         }
      }
   UpdateUI();
   }

void SoundManager::PreviewReverbEvent
   (
   Event *ev
   )
   {
   cvar_t   *cvar;
   int      reverbtype;
   float    reverblevel;

   // get reverb type
   cvar = gi.cvar( "snd_reverbtype", "", 0 );
   reverbtype = cvar->integer;

   // get reverb level
   cvar = gi.cvar( "snd_reverblevel", "", 0 );
   reverblevel = cvar->value;

   if ( g_entities[ 0 ].inuse && g_entities[ 0 ].client )
      {
      Player *client;

      client = ( Player * )g_entities[ 0 ].entity;
      client->SetReverb( reverbtype, reverblevel );
      }
   }

void SoundManager::ResetReverbEvent
   (
   Event *ev
   )
   {
   if ( g_entities[ 0 ].inuse && g_entities[ 0 ].client )
      {
      Player *client;

      client = ( Player * )g_entities[ 0 ].entity;
      client->SetReverb( "Generic", 0 );
      }
   }


void SoundManager::ShowingSounds
   (
   Event *ev
   )

   {
   int i;
   Entity * ent;

   for( i = 1; i <= soundList.NumObjects(); i++ )
      {
      ent = soundList.ObjectAt( i );
      if ( ent->isSubclassOf( TriggerSpeaker ) )
         {
         TriggerSpeaker * speaker;

         speaker = ( TriggerSpeaker * )ent;
         if ( current == ent )
            {
            if ( speaker->volume != DEFAULT_VOL )
               G_DrawDebugNumber( speaker->origin + Vector( 0, 0, 10 ), speaker->volume, 0.5, 0, 1, 0, 2 );
            if ( speaker->min_dist != DEFAULT_MIN_DIST )
               G_DrawDebugNumber( speaker->origin + Vector( 0, 0, 20 ), speaker->min_dist, 0.5, 0, 0, 1, 0 );

            // falloff circles
            if ( speaker->min_dist != DEFAULT_VOL )
               {
               G_DebugCircle( speaker->origin, speaker->min_dist, 0, 1, 1, 1, true );
               G_DebugCircle( speaker->origin, 2*speaker->min_dist, 0, 0.5, 0.5, 0.5, true );
               G_DebugCircle( speaker->origin, 4*speaker->min_dist, 0, 0.25, 0.25, 0.25, true );
               }

            if ( speaker->isSubclassOf( RandomSpeaker ) )
               {
               RandomSpeaker * random;

               G_DebugPyramid( speaker->origin, 24, 1, 1, 0.5, 1 );
               random = ( RandomSpeaker * )ent;

               G_DrawDebugNumber( random->origin + Vector( 0, 0, 30 ), random->mindelay, 0.5, 0.25, 0, 1, 1 );
               G_DrawDebugNumber( random->origin + Vector( 0, 0, 38 ), random->maxdelay, 0.5, 0.25, 0, 1, 1 );
               G_DrawDebugNumber( random->origin + Vector( 0, 0, 46 ), random->chance, 0.5, 0.25, 0, 1, 2 );
               }
            else
               {
               G_DebugPyramid( speaker->origin, 24, 1, 1, 0, 1 );
               }
            }
         else
            {
            if ( speaker->isSubclassOf( RandomSpeaker ) )
               {
               G_DebugPyramid( speaker->origin, 24, 1, 0, 1, 1 );
               }
            else
               {
               G_DebugPyramid( speaker->origin, 24, 1, 0, 0, 1 );
               }
            }
         }
      else if ( ent->isSubclassOf( TriggerMusic ) )
         {
         int            facet;
         TriggerMusic * music;

         music = ( TriggerMusic * )ent;

         if ( current == ent )
            {
            if ( music->oneshot )
               G_DebugBBox( ent->origin, ent->mins, ent->maxs, 0, 1, 1, 1 );
            else
               G_DebugBBox( ent->origin, ent->mins, ent->maxs, 1, 1, 0, 1 );
            }
         else
            {
            if ( music->oneshot )
               G_DebugBBox( ent->origin, ent->mins, ent->maxs, 0, 0.5, 0.5, 1 );
            else
               G_DebugBBox( ent->origin, ent->mins, ent->maxs, 1, 0, 0, 1 );
            }

         if ( music->useTriggerDir )
            {
            Vector org;

            org = ent->origin;
            org[ 2 ] += 0.5f * ent->maxs[ 2 ];

            G_DebugArrow( org, music->GetTriggerDir(), 48, 0.5, 1, 1, 1 );
            }
         facet = music->GetMultiFaceted();
         if ( facet )
            {
            if ( current == ent )
               {
               G_DebugHighlightFacet( ent->origin, ent->mins, ent->maxs, ( facet_t )( ( ( facet - 1 ) << 1 ) + currentFacet ), 1, 1, 1, 1 );
               }
            else
               {
               G_DebugHighlightFacet( ent->origin, ent->mins, ent->maxs, ( facet_t )( ( ( facet - 1 ) << 1 ) + 0 ), 0.6f, 0.3f, 0.2f, 0.1f );
               G_DebugHighlightFacet( ent->origin, ent->mins, ent->maxs, ( facet_t )( ( ( facet - 1 ) << 1 ) + 1 ), 0.6f, 0.3f, 0.2f, 0.1f );
               }
            }
         }
      else if ( ent->isSubclassOf( TriggerReverb ) )
         {
         int            facet;
         TriggerReverb  *reverb;

         reverb = ( TriggerReverb * )ent;

         if ( current == ent )
            {
            if ( reverb->oneshot )
               G_DebugBBox( ent->origin, ent->mins, ent->maxs, 0.5, 0.5, 1, 1 );
            else
               G_DebugBBox( ent->origin, ent->mins, ent->maxs, 0, 0.5, 1, 1 );
            }
         else
            {
            if ( reverb->oneshot )
               G_DebugBBox( ent->origin, ent->mins, ent->maxs, 0.5, 0, 1, 1 );
            else
               G_DebugBBox( ent->origin, ent->mins, ent->maxs, 0, 0, 1, 1 );
            }

         if ( reverb->useTriggerDir )
            {
            Vector org;

            org = ent->origin;
            org[ 2 ] += 0.5f * ent->maxs[ 2 ];

            G_DebugArrow( org, reverb->GetTriggerDir(), 48, 0.5, 1, 1, 1 );
            }
         facet = reverb->GetMultiFaceted();
         if ( facet )
            {
            if ( current == ent )
               {
               G_DebugHighlightFacet( ent->origin, ent->mins, ent->maxs, ( facet_t )( ( ( facet - 1 ) << 1 ) + currentFacet ), 1, 1, 1, 1 );
               }
            else
               {
               G_DebugHighlightFacet( ent->origin, ent->mins, ent->maxs, ( facet_t )( ( ( facet - 1 ) << 1 ) + 0 ), 0.3f, 0.3f, 0.6f, 0.1f );
               G_DebugHighlightFacet( ent->origin, ent->mins, ent->maxs, ( facet_t )( ( ( facet - 1 ) << 1 ) + 1 ), 0.3f, 0.3f, 0.6f, 0.1f );
               }
            }
         }
      }
   PostEvent( EV_SoundManager_ShowingSounds, FRAMETIME );
   }

void SoundManager::Show
   (
   void
   )

   {
   CurrentGainsFocus();
   CancelEventsOfType( EV_SoundManager_ShowingSounds );
   PostEvent( EV_SoundManager_ShowingSounds, FRAMETIME );
   UpdateUI();
   }

void SoundManager::Show
   (
   Event *ev
   )

   {
   Show();
   }

void SoundManager::Hide
   (
   Event *ev
   )

   {
   CurrentLostFocus();
   CancelEventsOfType( EV_SoundManager_ShowingSounds );
   UpdateUI();
   }

void SoundManager::Save
   (
   void
   )
   {
   Entity      *ent;
   str         buf;
	str         filename;
   int         i;

   // get the name of the sound file from the world
   filename = "maps/";
	filename += level.mapname;
	for( i = filename.length() - 1; i >= 0; i-- )
		{
		if ( filename[ i ] == '.' )
			{
			filename[ i ] = 0;
			break;
			}
		}

	filename += ".snd";

   gi.Printf( "Saving soundmanager file to '%s'...\n", filename.c_str() );

   buf = "";
   buf += va( "//\n" );
   buf += va( "// Sound Manager File \"%s\", %d Sound Entities.\n", filename.c_str(), soundList.NumObjects() );
   buf += va( "//\n" );

   //
   // save out normal TriggerSpeakers
   //
   buf += va( "// TriggerSpeakers\n" );
   for( i = 1; i <= soundList.NumObjects(); i++ )
      {
      TriggerSpeaker * speaker;

      ent = soundList.ObjectAt( i );
      if ( !ent->isSubclassOf( TriggerSpeaker ) || ent->isSubclassOf( RandomSpeaker ) )
         continue;

      speaker = ( TriggerSpeaker * )ent;
      //
      // start off the spawn command
      //
      buf += "spawn TriggerSpeaker";
      //
      // set the targetname
      //
      if ( ent->targetname != "" )
         {
         buf += va( " targetname %s", ent->targetname.c_str() );
         }
      //
      // set the origin
      //
      buf += va( " origin \"%.2f %.2f %.2f\"", ent->origin.x, ent->origin.y, ent->origin.z );
      //
      // make the speaker ambient and on
      //
      buf += " spawnflags 1";
      //
      // set the volume
      //
      if ( speaker->volume != DEFAULT_VOL )
         buf += va( " volume %.2f", speaker->volume );
      //
      // set the mindist
      //
      if ( speaker->min_dist != DEFAULT_MIN_DIST )
         buf += va( " min_dist %.1f", speaker->min_dist );
      //
      // save the sound
      //
      buf += va( " sound \"%s\"", speaker->Noise().c_str() );
      //
      // make sure it gets re-added to the sound manager
      //
      buf += " _addtosoundmanager 0";
      buf += va( "\n" );
      }
   //
   // save out RandomSpeakers
   //
   buf += va( "// RandomSpeakers\n" );
   for( i = 1; i <= soundList.NumObjects(); i++ )
      {
      RandomSpeaker * speaker;

      ent = soundList.ObjectAt( i );
      if ( !ent->isSubclassOf( TriggerSpeaker ) || !ent->isSubclassOf( RandomSpeaker ) )
         continue;

      speaker = ( RandomSpeaker * )ent;
      //
      // start off the spawn command
      //
      buf += "spawn RandomSpeaker";
      //
      // set the targetname
      //
      if ( ent->targetname != "" )
         {
         buf += va( " targetname %s", ent->targetname.c_str() );
         }
      //
      // set the origin
      //
      buf += va( " origin \"%.2f %.2f %.2f\"", ent->origin.x, ent->origin.y, ent->origin.z );
      //
      // set the volume
      //
      if ( speaker->volume != DEFAULT_VOL )
         buf += va( " volume %.2f", speaker->volume );
      //
      // set the mindist
      //
      if ( speaker->min_dist != DEFAULT_MIN_DIST )
         buf += va( " min_dist %.1f", speaker->min_dist );
      //
      // set the channel
      //
      buf += va( " channel %d", speaker->channel );
      //
      // set the mindelay
      //
      buf += va( " mindelay %.2f", speaker->mindelay );
      //
      // set the maxdelay
      //
      buf += va( " maxdelay %.2f", speaker->maxdelay );
      //
      // set the chance
      //
      buf += va( " chance %.2f", speaker->chance );
      //
      // save the sound
      //
      buf += va( " sound \"%s\"", speaker->Noise().c_str() );
      //
      // make sure it gets re-added to the sound manager
      //
      buf += " _addtosoundmanager 0";
      buf += va( "\n" );
      }
   //
   // save out TriggerMusic
   //
   buf += va( "// TriggerMusics\n" );
   for( i = 1; i <= soundList.NumObjects(); i++ )
      {
      TriggerMusic * music;

      ent = soundList.ObjectAt( i );
      if ( !ent->isSubclassOf( TriggerMusic ) )
         continue;

      music = ( TriggerMusic * )ent;
      //
      // start off the spawn command
      //
      buf += "spawn TriggerMusic";
      //
      // set the targetname
      //
      if ( ent->targetname != "" )
         {
         buf += va( " targetname %s", ent->targetname.c_str() );
         }
      //
      // set the origin
      //
      buf += va( " origin \"%.2f %.2f %.2f\"", ent->origin.x, ent->origin.y, ent->origin.z );
      //
      // set the angle
      //
      if ( music->useTriggerDir )
         {
         buf += va( " angle %.1f", AngleMod( ent->angles.y ) );
         }
      //
      // set the oneshot
      //
      if ( music->oneshot )
         {
         buf += " oneshot 0";
         }
      //
      // set current
      //
      buf += va( " current %s", music->current.c_str() );

      //
      // set fallback
      //
      buf += va( " fallback %s", music->fallback.c_str() );

      if ( music->GetMultiFaceted() )
         {
         //
         // save out multi faceted
         //
         buf += va( " multifaceted %d", music->GetMultiFaceted() );

         //
         // set alt current
         //
         buf += va( " altcurrent %s", music->altcurrent.c_str() );

         //
         // set alt fallback
         //
         buf += va( " altfallback %s", music->altfallback.c_str() );
         }

      //
      // set mins
      //
      buf += va( " _setmins \"%.2f %.2f %.2f\"", ent->mins.x, ent->mins.y, ent->mins.z );

      //
      // set maxs
      //
      buf += va( " _setmaxs \"%.2f %.2f %.2f\"", ent->maxs.x, ent->maxs.y, ent->maxs.z );
      //
      // make sure it gets re-added to the sound manager
      //
      buf += " _addtosoundmanager 0";
      buf += va( "\n" );
      }

   //
   // save out TriggerReverb
   //
   buf += va( "// TriggerReverbs\n" );
   for( i = 1; i <= soundList.NumObjects(); i++ )
      {
      TriggerReverb * reverb;

      ent = soundList.ObjectAt( i );
      if ( !ent->isSubclassOf( TriggerReverb ) )
         continue;

      reverb = ( TriggerReverb * )ent;
      //
      // start off the spawn command
      //
      buf += "spawn TriggerReverb";
      //
      // set the targetname
      //
      if ( ent->targetname != "" )
         {
         buf += va( " targetname %s", ent->targetname.c_str() );
         }
      //
      // set the origin
      //
      buf += va( " origin \"%.2f %.2f %.2f\"", ent->origin.x, ent->origin.y, ent->origin.z );
      //
      // set the angle
      //
      if ( reverb->useTriggerDir )
         {
         buf += va( " angle %.1f", AngleMod( ent->angles.y ) );
         }
      //
      // set the oneshot
      //
      if ( reverb->oneshot )
         {
         buf += " oneshot 0";
         }
      //
      // set reverb type
      //
      buf += va( " reverbtype %d", reverb->reverbtype );

      //
      // set reverb level
      //
      buf += va( " reverblevel %.2f", reverb->reverblevel );

      if ( reverb->GetMultiFaceted() )
         {
         //
         // save out multi faceted
         //
         buf += va( " multifaceted %d", reverb->GetMultiFaceted() );

         //
         // set alt reverb type
         //
         buf += va( " altreverbtype %d", reverb->altreverbtype );

         //
         // set alt reverb level
         //
         buf += va( " altreverblevel %.2f", reverb->altreverblevel );
         }

      //
      // set mins
      //
      buf += va( " _setmins \"%.2f %.2f %.2f\"", ent->mins.x, ent->mins.y, ent->mins.z );

      //
      // set maxs
      //
      buf += va( " _setmaxs \"%.2f %.2f %.2f\"", ent->maxs.x, ent->maxs.y, ent->maxs.z );
      //
      // make sure it gets re-added to the sound manager
      //
      buf += " _addtosoundmanager 0";
      buf += va( "\n" );
      }

   buf += "end\n";

   gi.FS_WriteFile( filename.c_str(), buf.c_str(), buf.length() + 1 );
   gi.Printf( "done.\n" );
   }

void SoundManager::Save
   (
   Event *ev
   )

   {
   Save();
   }

void SoundManager::Load
   (
   void
   )
   {
	str filename;
   int i;

   // get the name of the sound file from the world
   filename = "maps/";
	filename += level.mapname;
	for( i = filename.length() - 1; i >= 0; i-- )
		{
		if ( filename[ i ] == '.' )
			{
			filename[ i ] = 0;
			break;
			}
		}

	filename += ".snd";

 	// If there isn't a script with the same name as the map, then don't try to load script
	if ( gi.FS_ReadFile( filename.c_str(), NULL, qtrue ) != -1 )
      {
      ScriptThread *thread;

      Reset();

      thread = Director.CreateThread( filename, LEVEL_SCRIPT, NULL );
      if ( thread )
         {
         // start right away
         thread->StartImmediately();
         }
      }
   }

void SoundManager::AddEntity
   (
   Entity * ent
   )
   {
   if ( ent )
      {
      soundList.AddUniqueObject( ent );
      }
   }
