//-----------------------------------------------------------------------------
//
//  $Logfile:: /fakk2_code/fakk2_new/cgame/cg_swipe.cpp                       $
// $Revision:: 12                                                             $
//   $Author:: Markd                                                          $
//     $Date:: 6/27/00 2:34p                                                  $
//
// Copyright (C) 1998 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
// $Log:: /fakk2_code/fakk2_new/cgame/cg_swipe.cpp                            $
// 
// 12    6/27/00 2:34p Markd
// clear out swipes when restarting
// 
// 11    6/14/00 12:14p Markd
// more intel compiler bug fixes
// 
// 10    5/15/00 2:19p Aldie
// Added new tempmodel system and added player accumulated pain
// 
// 9     5/03/00 4:20p Markd
// Only turn off swipes when swipes were active
// 
// 8     3/27/00 1:50p Aldie
// Made swipes work for multiple models
// 
// 7     3/04/00 6:03p Aldie
// Made commandManager static
// 
// 6     2/07/00 7:38p Aldie
// Fixed swiping, also various weapons fixes for sword and for sling
// 
// 5     1/26/00 6:36p Aldie
// Fix the swipe
// 
// 4     12/09/99 10:52a Jimdose
// got tags working with torso and crossblended animations
// 
// 3     11/01/99 4:11p Jimdose
// made tags work with bone controllers
// 
// 2     10/05/99 6:01p Aldie
// Added headers
//
// DESCRIPTION:
// General swiping functions

#include "cg_commands.h"

swipething_t m_swipes[MAX_SWIPES];
extern int current_entity_number;
extern refEntity_t *current_entity;
extern int current_tiki;
extern centity_t *current_centity;

void ClientGameCommandManager::SwipeOn
   (
   Event *ev
   )

   {
   // Make a swipe for this if available
   int i;
   int freeswipe = -1;
   str tagname_start,tagname_end;
   qhandle_t shader;

   if ( ev->NumArgs () != 4 )
      return;

   shader         = cgi.R_RegisterShader( ev->GetString( 1 ) );
   tagname_start  = ev->GetString( 2 );
   tagname_end    = ev->GetString( 3 );
   
   for ( i=0; i < MAX_SWIPES; i++ )
      {
      swipething_t &swipe = m_swipes[i];

      if ( !swipe.enabled || 
            (
            ( swipe.entitynum == current_entity_number ) && 
            ( tagname_start == swipe.tagname_start ) && 
            ( tagname_end == swipe.tagname_end ) && 
            ( shader == swipe.shader )
            )
         )
         {
         freeswipe = i;
         break;
         }
      }

   if ( freeswipe == -1 )
      return;

   swipething_t &swipe = m_swipes[freeswipe];

   if ( !swipe.enabled )
      {
      swipe.Init ();
      swipe.enabled = qtrue;
      swipe.entitynum = current_entity_number;
      }

   swipe.shader = cgi.R_RegisterShader( ev->GetString( 1 ) );
   swipe.tagname_start = ev->GetString ( 2 );
   swipe.tagname_end   = ev->GetString ( 3 );
   swipe.life = ev->GetFloat ( 4 ) * 1000.f;

   if ( current_centity )
      current_centity->clientFlags |= CF_UPDATESWIPE;
   }

void ClientGameCommandManager::SwipeOff
   (
   Event *ev 
   )

   {
   qboolean was_enabled;
   int i;

   was_enabled = qfalse;
   for ( i=0; i < MAX_SWIPES; i++ )
      {
      swipething_t &swipe = m_swipes[i];

      if ( swipe.enabled && swipe.entitynum == current_entity_number )
         {
         swipe.enabled = qfalse;
         was_enabled = qtrue;
         }
      }

   if ( was_enabled )
      {
      //cgi.R_SwipeBegin ( 0.f, 0.f, -1 );
      //cgi.R_SwipeEnd ();

      if ( current_centity )
         current_centity->clientFlags &= ~CF_UPDATESWIPE;
      }
   }

void ClientGameCommandManager::Swipe
   (
   Event *ev
   )

   {
   // This does a swipe effect on the entity
   int            i,swipenum;
   orientation_t  or;
   int            tagnum_start,tagnum_end;
   qboolean       add_cnt_point = qfalse;
   vec3_t         tmp;

   assert( current_entity );
   assert( current_tiki != -1 );

   if ( !current_entity || ( current_tiki == -1 ) )
      {
      return;
      }

   // Let's find us a new (or current) swipe
   for ( swipenum=0; swipenum < MAX_SWIPES; swipenum++ )
      {
      swipething_t &swipe = m_swipes[swipenum];

      if ( swipe.enabled && swipe.entitynum == current_entity_number )
         {         
         while ( swipe.num_live_swipes > 1 )
            {
            i = ( swipe.first_swipe + 1 ) % MAX_SWIPE_POINTS;
            swipepoint_t &swipepoint = swipe.swipepoints[i];

            if ( swipepoint.time < cg.time - swipe.life )
               {
               // then let's delete the previous
               swipe.first_swipe = ( swipe.first_swipe + 1 ) % MAX_SWIPE_POINTS;
               swipe.num_live_swipes--;
               }
            else
               {
               break;
               }
            }

         if ( swipe.num_live_swipes == 1 )
            {
            swipepoint_t &swipepoint = swipe.swipepoints[swipe.first_swipe];
            if ( swipepoint.time < cg.time - swipe.life )
               {
               swipe.num_live_swipes = 0;
               }
            }

         swipe.cntPoint.time = cg.time;
         VectorCopy ( current_entity->origin, swipe.cntPoint.points[0] );

         tagnum_start = cgi.Tag_NumForName( current_tiki, swipe.tagname_start.c_str() );
         or = cgi.Tag_LerpedOrientation( current_tiki, current_entity, tagnum_start );

         // Clear out the points
         VectorClear ( tmp );
         VectorClear ( swipe.cntPoint.points[0] );
         VectorClear ( swipe.cntPoint.points[1] );

         if ( ev->NumArgs() > 0 )
            VectorCopy( ev->GetVector( 1 ), tmp );
         else
            VectorCopy ( current_entity->origin, tmp );
   
         // Transform the origin of the tag by the axis of the entity and add it to the origin of the entity
         for ( i = 0; i < 3; i++ )
            {
            VectorMA ( tmp, or.origin[i], current_entity->axis[i], tmp );
            }

         // Copy tmp into the startpoint
         VectorCopy ( tmp, swipe.cntPoint.points[0] );

         tagnum_end = cgi.Tag_NumForName( current_tiki, swipe.tagname_end.c_str() );
         or = cgi.Tag_LerpedOrientation( current_tiki, current_entity, tagnum_end );

         if ( ev->NumArgs() > 0 )
            VectorCopy( ev->GetVector( 1 ), tmp );
         else
            VectorCopy ( current_entity->origin, tmp );
   
         // Transform the origin of the tag by the axis of the entity and add it to the origin of the entity
         for ( i = 0; i < 3; i++ )
            {
            VectorMA ( tmp, or.origin[i], current_entity->axis[i], tmp );
            }

         // Copy tmp into the startpoint
         VectorCopy ( tmp, swipe.cntPoint.points[1] );

         if ( swipe.num_live_swipes == 0 )
            add_cnt_point = qtrue;
         else 
            {
            float deltime = swipe.life / float ( MAX_SWIPES );
            swipepoint_t &lastpoint = swipe.swipepoints[ ( swipe.first_swipe + swipe.num_live_swipes - 1 ) % MAX_SWIPE_POINTS];

            if ( swipe.cntPoint.time - lastpoint.time >= deltime )
               add_cnt_point = qtrue;
            }

         cgi.R_SwipeBegin ( cg.time, swipe.life, swipe.shader );

         if ( add_cnt_point )
            {
            swipepoint_t &swipepoint = swipe.swipepoints[ ( swipe.first_swipe + swipe.num_live_swipes ) % MAX_SWIPE_POINTS];

            swipepoint = swipe.cntPoint;
      
            if ( swipe.num_live_swipes == MAX_SWIPE_POINTS )
               swipe.first_swipe = ( swipe.first_swipe + 1 ) % MAX_SWIPE_POINTS;
            else
               swipe.num_live_swipes++;
            }

         for ( i = 0; i != swipe.num_live_swipes; i++ )
            {
            swipepoint_t &swipepoint = swipe.swipepoints[ ( swipe.first_swipe + i ) % MAX_SWIPE_POINTS];

            cgi.R_SwipePoint ( swipepoint.points[0], swipepoint.points[1], swipepoint.time );
            }

         if ( !add_cnt_point )
            cgi.R_SwipePoint ( swipe.cntPoint.points[0], swipe.cntPoint.points[1], swipe.cntPoint.time );

         cgi.R_SwipeEnd ();
         }
      }
   }

void ClientGameCommandManager::ClearSwipes
   (
   void
   )

   {
   int i;

   for ( i=0; i < MAX_SWIPES; i++ )
      {
      swipething_t &swipe = m_swipes[i];

      swipe.enabled = qfalse;
      }

   cgi.R_SwipeBegin ( 0.f, 0.f, -1 );
   cgi.R_SwipeEnd ();
   }


/*
===================
CG_ClearSwipes

This is called at startup and for tournement restarts
===================
*/
void CG_ClearSwipes( void ) 
   {
   commandManager.ClearSwipes();
   }
