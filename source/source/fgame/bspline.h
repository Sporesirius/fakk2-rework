//-----------------------------------------------------------------------------
//
//  $Logfile:: /fakk2_code/fakk2_new/fgame/bspline.h                          $
// $Revision:: 9                                                              $
//   $Author:: Steven                                                         $
//     $Date:: 7/26/00 8:37a                                                  $
//
// Copyright (C) 1998 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
// $Log:: /fakk2_code/fakk2_new/fgame/bspline.h                               $
// 
// 9     7/26/00 8:37a Steven
// Fixed a bad memory bug.
// 
// 8     7/26/00 12:46a Markd
// fixed potential crash bug, I hope
// 
// 7     7/25/00 11:32p Aldie
// Made some changes to the memory system and fixed a memory allocation bug in
// Z_TagMalloc.  Also changed a lot of classes to subclass from Class.
// 
// 6     5/25/00 7:52p Markd
// 2nd pass save game stuff
// 
// 5     5/24/00 3:14p Markd
// first phase of save/load games
// 
// 4     1/05/00 7:25p Jimdose
// made angle functions all use the same coordinate system
// AngleVectors now returns left instead of right
// no longer need to invert pitch
// 
// 3     11/09/99 8:08p Markd
// revamped some functions because of changes to camera system
// 
// 2     11/05/99 5:55p Markd
// Added user-interface to camera system
// 
// 1     9/10/99 10:53a Jimdose
// 
// 1     9/08/99 3:15p Aldie
// 
// 10    9/01/99 9:05p Jimdose
// added setthread event
// 
// 9     9/01/99 8:00p Jimdose
// added triggertarget event
// 
// 8     9/01/99 6:15p Jimdose
// added SetThread and SetTriggerTarget
// 
// DESCRIPTION:
// Uniform non-rational bspline class.
// 

#ifndef __BSPLINE_H__
#define __BSPLINE_H__

#include "g_local.h"
#include "Vector.h"

typedef enum
	{
	SPLINE_NORMAL,
	SPLINE_LOOP,
	SPLINE_CLAMP
	} splinetype_t;

class BSplineControlPoint : public Class 
	{
   private:
      float          roll;
      Vector         position;
      Vector         orientation;
      float          speed;

	public:
							BSplineControlPoint();
							BSplineControlPoint( Vector pos, Vector orient, float speed );
							BSplineControlPoint( Vector pos );
		void				Clear( void );
		void				Set( Vector pos );
		void				Set( Vector pos, float speed );
		void				Set( Vector pos, Vector orient, float speed );
		void				Get( Vector& pos, Vector& orient, float& speed );
		void				Get( Vector& pos );
      Vector         *GetPosition( void );
      Vector         *GetOrientation( void );
      float          *GetRoll( void );
      float          *GetSpeed( void );
		void		      operator=( BSplineControlPoint &point );
      virtual void   Archive( Archiver &arc );
	};

inline void BSplineControlPoint::Archive
	(
	Archiver &arc
	)

   {
   arc.ArchiveVector( &position );
   arc.ArchiveVector( &orientation );
   arc.ArchiveFloat( &speed );
   arc.ArchiveFloat( &roll );
   }

inline void BSplineControlPoint::operator=
	(
	BSplineControlPoint &point
	)

	{
   position = point.position;
   orientation = point.orientation;
   speed = point.speed;
   roll = point.roll;
	}

inline BSplineControlPoint::BSplineControlPoint()
	{
   roll = 0;
   speed = 1;
	}

inline BSplineControlPoint::BSplineControlPoint
   ( 
   Vector pos 
   )

	{
   speed = 1;
   position = pos;
	}

inline BSplineControlPoint::BSplineControlPoint
   ( 
   Vector pos, 
   Vector orient,
   float speed
   )

	{
   position = pos;
	orient.AngleVectors( &orientation, NULL, NULL );
   roll = orient[ ROLL ];
   if ( roll > 180 )
      {
      roll -= 360;
      }
   if ( roll < -180 )
      {
      roll += 360;
      }
   this->speed = speed;
	}

inline void BSplineControlPoint::Clear
   ( 
   void 
   )

	{
   roll = 0;
   position = "0 0 0";
   vec_zero.AngleVectors( &orientation, NULL, NULL );
   speed = 1.0f;
	}

inline void BSplineControlPoint::Set
   ( 
   Vector pos 
   )

	{
   speed = 1;
   position = pos;
	}

inline void BSplineControlPoint::Set
   ( 
   Vector pos,
   float pointspeed
   )

	{
   speed = pointspeed;
   position = pos;
	}

inline void BSplineControlPoint::Set
   ( 
   Vector pos, 
   Vector orient,
   float speed
   )

	{
   position = pos;
	orient.AngleVectors( &orientation, NULL, NULL );
   roll = orient[ ROLL ];
   if ( roll > 180 )
      {
      roll -= 360;
      }
   if ( roll < -180 )
      {
      roll += 360;
      }
   this->speed = speed;
	}

inline void BSplineControlPoint::Get
   ( 
   Vector& pos 
   )
	{
   pos = position;
	}

inline Vector *BSplineControlPoint::GetPosition
   ( 
   void
   )

	{
   return &position;
	}

inline void BSplineControlPoint::Get
   ( 
   Vector& pos, 
   Vector& orient,
   float& speed
   )

	{
   pos = position;
   orient = orientation;
   speed = this->speed;
	}

inline Vector *BSplineControlPoint::GetOrientation
   ( 
   void
   )
	{
   return &orientation;
	}

inline float *BSplineControlPoint::GetRoll
   ( 
   void
   )
	{
   return &roll;
	}

inline float *BSplineControlPoint::GetSpeed
   ( 
   void
   )
	{
   return &speed;
	}

class BSpline : public Class 
	{
	private:
		BSplineControlPoint  *control_points;
		int				      num_control_points;
      int                  loop_control_point;
		splinetype_t	      curvetype;
      qboolean             has_orientation;

		float  			EvalNormal( float u, Vector &pos, Vector& orient );
		float          EvalLoop( float u, Vector &pos, Vector& orient );
		float 			EvalClamp( float u, Vector &pos, Vector& orient );

	public:
							BSpline();
							~BSpline();
							BSpline( Vector *control_points_, int num_control_points_, splinetype_t type );
							BSpline( Vector *control_points_, Vector *control_orients_, float *control_speeds_, int num_control_points_, splinetype_t type );
		void		      operator=( BSpline &spline );
		void				SetType( splinetype_t type );
		int   			GetType( void );
		void				Clear( void );
		void				Set( Vector *control_points_, int num_control_points_, splinetype_t type );
		void				Set( Vector *control_points_, Vector *control_orients_, float *control_speeds_, int num_control_points_, splinetype_t type );
		void				AppendControlPoint( const Vector& new_control_point );
		void				AppendControlPoint( const Vector& new_control_point, const float& speed );
		void				AppendControlPoint( const Vector& new_control_point, const Vector& new_control_orient, const float& speed );
		Vector			Eval( float u );
		float  			Eval( float u, Vector& pos, Vector& orient );

		void				DrawControlSegments( void );
		void				DrawCurve( int num_subdivisions );
		void				DrawCurve( Vector offset, int num_subdivisions );

      void           SetLoopPoint( const Vector& pos );

		float				EndPoint( void );

		// return the index of the control point picked or -1 if none.
		int				PickControlPoint( const Vector& window_point, float pick_size );

		Vector			*GetControlPoint( int id );
		void           GetControlPoint( int id, Vector& pos, Vector& orient, float& speed );
		void				SetControlPoint( int id, const Vector& new_control_point );
		void				SetControlPoint( int id, const Vector& new_control_point, const Vector& new_control_orient, const float& speed );
      virtual void   Archive( Archiver &arc );
	};

inline BSpline::BSpline()
	{
   has_orientation = false;
	control_points = NULL;
	num_control_points = 0;
   loop_control_point = 0;
	curvetype = SPLINE_NORMAL;
	}

inline BSpline::~BSpline()
	{
   if ( control_points )
      {
      delete [] control_points;
      control_points = NULL;
      }
	}

inline BSpline::BSpline
	(
	Vector *control_points_,
	int num_control_points_,
	splinetype_t type
	)

	{
   has_orientation = false;
	control_points = NULL;
	num_control_points = 0;
   loop_control_point = 0;
	curvetype = SPLINE_NORMAL;

   Set( control_points_, num_control_points_, type );
	}

inline BSpline::BSpline
	(
	Vector *control_points_,
	Vector *control_orients_,
	float  *control_speeds_,
	int num_control_points_,
	splinetype_t type
	)

	{
   has_orientation = false;
	control_points = NULL;
	num_control_points = 0;
   loop_control_point = 0;
	curvetype = SPLINE_NORMAL;

   Set( control_points_, control_orients_, control_speeds_, num_control_points_, type );
	}

inline void BSpline::operator=
	(
	BSpline &spline
	)

	{
   int i;

   Clear();
   num_control_points = spline.num_control_points;
   loop_control_point = spline.loop_control_point;
   curvetype = spline.curvetype;
   has_orientation = spline.has_orientation;

   if ( num_control_points )
      {
	   control_points = new BSplineControlPoint[num_control_points];
	   assert( control_points );
      for ( i = 0; i < num_control_points ; i++ )
         control_points[ i ] = spline.control_points[ i ];
      }
   else
      {
      control_points = NULL;
      }
	}

inline void BSpline::SetType
	( 
	splinetype_t type
	)

	{
	curvetype = type;
	}

inline int BSpline::GetType
	( 
   void
	)

	{
	return curvetype;
	}

inline float BSpline::EndPoint
	(
	void
	)

	{
	return num_control_points;
	}

inline Vector *BSpline::GetControlPoint
	(
	int id
	)

	{
   assert( id >= 0 );
	assert( id < num_control_points );
   if ( ( id < 0 ) && ( id >= num_control_points ) )
      {
      // probably wrong, but if we're in release mode we have no recourse
      id = 0;
      }
   
   return control_points[ id ].GetPosition();
  	}

inline void BSpline::GetControlPoint
	(
	int id,
   Vector& pos,
   Vector& orient,
   float& speed
	)

	{
   assert( id >= 0 );
	assert( id < num_control_points );
   if ( ( id >= 0 ) && ( id < num_control_points ) )
      {
      control_points[ id ].Get( pos, orient, speed );
      }
	}

inline void BSpline::SetControlPoint
	(
	int id,
	const Vector& new_control_point
	)

	{
   assert( id >= 0 );
	assert( id < num_control_points );
   if ( ( id >= 0 ) && ( id < num_control_points ) )
      {
      control_points[ id ].Set( new_control_point );
      }
	}

inline void BSpline::SetControlPoint
	(
	int id,
	const Vector& new_control_point,
	const Vector& new_control_orient,
   const float& speed
	)

	{
   assert( id >= 0 );
	assert( id < num_control_points );
   if ( ( id >= 0 ) && ( id < num_control_points ) )
      {
      control_points[ id ].Set( new_control_point, new_control_orient, speed );
      }
	}

inline void BSpline::Archive
	(
	Archiver &arc
	)
   {
   int i;

   arc.ArchiveInteger( &num_control_points );
   if ( arc.Loading() )
      {
		if ( num_control_points )
			control_points = new BSplineControlPoint[ num_control_points ];
		else
			control_points = NULL;
      }

   arc.ArchiveInteger( &loop_control_point );

   i = curvetype;
   arc.ArchiveInteger( &i );
   curvetype = ( splinetype_t )i;

   arc.ArchiveBoolean( &has_orientation );
   for( i = 0; i < num_control_points; i++ )
      {
      control_points[ i ].Archive( arc );
      }
   }

extern Event EV_SplinePath_Create;
extern Event EV_SplinePath_Loop;
extern Event EV_SplinePath_Speed;

class SplinePath : public Entity
	{
	protected:
		SplinePath        *owner;
		SplinePath        *next;
      SplinePath        *loop;
      str               loop_name;

		void					CreatePath( Event *ev );
      void              SetLoop( Event *ev );
      void              SetSpeed( Event *ev );
      void              SetTriggerTarget( Event *ev );
      void              SetThread( Event *ev );
      void              SetFov( Event *ev );
      void              SetWatch( Event *ev );
      void              SetFadeTime( Event *ev );

	public:
      float             speed;
      float             fov;
      float             fadeTime;
      qboolean          doWatch;
      str               watchEnt;
      str               triggertarget;
      str               thread;

      CLASS_PROTOTYPE( SplinePath );

								SplinePath();
								~SplinePath();
		SplinePath			*GetNext( void );
		SplinePath			*GetPrev( void );
		SplinePath			*GetLoop( void );
      void              SetFadeTime( float newFadeTime );
      void              SetFov( float theFov );
      void              SetWatch( const char *name );
      void              SetThread( const char *name );
      void              SetTriggerTarget( const char *name );
      void              NoWatch( void );
      str               GetWatch( void );
      float             GetFadeTime( void );
      float             GetFov( void );
		void              SetNext( SplinePath *node );
		void              SetPrev( SplinePath *node );
      virtual void      Archive( Archiver &arc );
	};

typedef SafePtr<SplinePath> SplinePathPtr;

inline void SplinePath::Archive
	(
	Archiver &arc
	)

   {
   Entity::Archive( arc );

   arc.ArchiveObjectPointer( ( Class ** )&owner );
   arc.ArchiveObjectPointer( ( Class ** )&next );
   arc.ArchiveObjectPointer( ( Class ** )&loop );
   arc.ArchiveString( &loop_name );
   arc.ArchiveFloat( &speed );
   arc.ArchiveFloat( &fov );
   arc.ArchiveFloat( &fadeTime );
   arc.ArchiveBoolean( &doWatch );
   arc.ArchiveString( &watchEnt );
   arc.ArchiveString( &thread );
   arc.ArchiveString( &triggertarget );
   if ( arc.Loading() )
      {
      CancelEventsOfType( EV_SplinePath_Create );
      }
   }

inline void SplinePath::SetThread
   (
   const char *name
   )

   {
   thread = name;
   }

inline void SplinePath::SetTriggerTarget
   (
   const char *name
   )

   {
   triggertarget = name;
   }

#endif /* __BSPLINE_H__ */
