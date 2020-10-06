//-----------------------------------------------------------------------------
//
//  $Logfile:: /fakk2_code/fakk2_new/cgame/cg_listener.cpp                    $
// $Revision:: 2                                                              $
//   $Author:: Aldie                                                          $
//     $Date:: 9/10/99 5:24p                                                  $
//
// Copyright (C) 1998 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
// $Log:: /fakk2_code/fakk2_new/cgame/cg_listener.cpp                         $
// 
// 2     9/10/99 5:24p Aldie
// Merge code
// 
// 2     9/09/99 3:30p Aldie
// Merge
// 
// 1     9/08/99 3:35p Aldie
// 
// 5     7/29/99 3:32p Aldie
// Updated to new class system
//
// DESCRIPTION:
//

#include "cg_local.h"
#include "cg_listener.h"

cvar_t *cg_numevents;
cvar_t *cg_showevents;
cvar_t *cg_eventlimit;
cvar_t *cg_timeevents;
cvar_t *cg_watch;

Vector vec_zero="0 0 0";

Event EV_Remove( "immediateremove" );
Event EV_ScriptRemove( "remove" );

typedef struct eventcache_s
	{
	Listener *obj;
	Event		*event;
	float		time;

	struct eventcache_s *next;
	struct eventcache_s *prev;
	} eventcache_t;

#define MAX_EVENTS 2000

extern "C"
	{
	eventcache_t	Events[ MAX_EVENTS ];
	int				numEvents = 0;
	cvar_t			*g_numevents;
	};

eventcache_t FreeEventHead;
eventcache_t *FreeEvents = &FreeEventHead;
eventcache_t EventQueueHead;
eventcache_t *EventQueue = &EventQueueHead;

Container<str *> *Event::commandList = NULL;
Container<int> *Event::flagList = NULL;
Container<int> *Event::sortedList = NULL;
qboolean Event::dirtylist = qfalse;

qboolean IsNumeric
	(
	const char *str
	)

	{
	int len;
	int i;
	qboolean dot;

	if ( *str == '-' )
		{
		str++;
		}

	dot = qfalse;
	len = strlen( str );
	for( i = 0; i < len; i++ )
		{
		if ( !isdigit( str[ i ] ) )
			{
			if ( ( str[ i ] == '.' ) && !dot )
				{
				dot = qtrue;
				continue;
				}
			return qfalse;
			}
		}

	return qtrue;
	}

Event NullEvent;

CLASS_DECLARATION( Class, Event, NULL )
	{
		{ NULL, NULL }
	};

int Event::NumEventCommands
	(
	void
	)

	{
	if ( commandList )
		{
		// Add 1 since container gives the inclusive number of objects
		return commandList->NumObjects() + 1;
		}

	return 0;
	}

int Event::compareEvents
	(
	const void *arg1,
	const void *arg2
	)

	{
	int ev1;
	int ev2;

	ev1 = *( int * )arg1;
	ev2 = *( int * )arg2;

	return stricmp( commandList->ObjectAt( ev1 )->c_str(), commandList->ObjectAt( ev2 )->c_str() );
	}

void Event::SortEventList
	(
	void
	)

	{
	dirtylist = qfalse;

	if ( sortedList && commandList )
		{
#ifndef EXPORT_TEMPLATE
      qsort( ( void * )sortedList->AddressOfObjectAt( 1 ),
         ( size_t )sortedList->NumObjects(),
         sizeof( int ), compareEvents );
#else
		sortedList->Sort( compareEvents );
#endif
		}
	}

inline int Event::FindEvent
	(
	const char *name
	)

	{
	int eventnum;
   int index;
	int l;
	int r;
	int diff;

   assert( name );
   if ( !name )
      {
      return 0;
      }

	if ( !commandList )
		{
		return 0;
		}

	if ( dirtylist )
		{
		SortEventList();
		}

   l = 1;
   r = sortedList->NumObjects();
   while( r >= l )
      {
      index = ( l + r ) >> 1;
		eventnum = sortedList->ObjectAt( index );
      diff = stricmp( name, commandList->ObjectAt( eventnum )->c_str() );
      if ( diff < 0 )
         {
         r = index - 1;
         }
      else if ( diff > 0 )
         {
         l = index + 1;
         }
      else
         {
         return eventnum;
         }
      }

   return 0;
	}

int Event::FindEvent
	(
	str &name
	)

	{
   return FindEvent( name.c_str() );
	}

void Event::ListCommands
	(
   const char *mask
	)

	{
   str name;
   int flags;
	int eventnum;
   int num;
	int i;
	int n;
   int l;
   int p;
   int hidden;
   str text;

	if ( !commandList )
		{
      cgi.Printf( "No events.\n" );
		return;
		}

	if ( dirtylist )
		{
		SortEventList();
		}

   l = 0;
   if ( mask )
      {
      l = strlen( mask );
      }

   hidden = 0;
   num = 0;
   n = sortedList->NumObjects();
   for( i = 1; i <= n; i++ )
      {
		eventnum = sortedList->ObjectAt( i );
      name = commandList->ObjectAt( eventnum )->c_str();
      flags = flagList->ObjectAt( eventnum );

      if ( flags & EV_HIDE )
         {
         hidden++;
         continue;
         }

      if ( mask && strnicmp( name.c_str(), mask, l ) )
         {
         continue;
         }

      num++;

      text = "   ";
      p = 0;
      if ( flags & EV_CONSOLE )
         {
         text[ p++ ] = '*';
         }
      if ( flags & EV_CHEAT )
         {
         text[ p++ ] = 'C';
         }

      cgi.Printf( "%4d : %s%s\n", eventnum, text.c_str(), name.c_str() );
      }

   cgi.Printf( "\n* = console command.\nC = cheat command.\n\n"
      "Printed %d of %d total commands.\n", num, n - hidden );

/* GAMEFIX
   if ( developer->integer && hidden )
      {
      cgi.Printf( "Suppressed %d commands.\n", hidden );
      }
*/
	}

void Event::initCommandList
	(
	void
	)

	{
	int flags;
   str *n;

	flags = 0;
	commandList = new Container<str *>;

   n = new str( "NULL" );
	NullEvent.eventnum = commandList->AddObject( n );

	flagList = new Container<int>;
	flagList->AddObject( flags );

	sortedList = new Container<int>;
	sortedList->AddObject( NullEvent.eventnum );

	dirtylist = qfalse;

	NullEvent.data = NULL;
	NullEvent.info.inuse = 0;
	NullEvent.info.source = EV_FROM_CODE;
	NullEvent.info.flags = 0;
	NullEvent.info.linenumber = 0;
	}

Event::Event()
	{
	info.inuse = 0;
	info.source = EV_FROM_CODE;
	info.flags = 0;
	info.linenumber = 0;
	threadnum = -1;
	eventnum = 0;
	data = NULL;
   name = NULL;
	}

Event::Event
	(
	int num
	)

	{
	if ( !commandList )
		{
		initCommandList();
		}

	assert( ( num > 0 ) && num <= commandList->NumObjects() );

   if ( ( num <= 0 ) || ( num > commandList->NumObjects() ) )
      {
      num = 0;
      name = NULL;
   	info.flags = 0;
      }
   else
      {
      name = commandList->ObjectAt( num )->c_str();
	   info.flags = flagList->ObjectAt( num );
      }

	eventnum = num;
	data = NULL;
	info.inuse = 0;
	info.source = EV_FROM_CODE;
	info.linenumber = 0;
	threadnum = -1;
	}

Event::Event
	(
	Event &ev
	)

	{
	int num;
	int i;

	eventnum = ( int )ev;
	assert( ( eventnum > 0 ) && eventnum <= commandList->NumObjects() );
	data = NULL;

   name = commandList->ObjectAt( eventnum )->c_str();
	info.inuse = 0;
	info.source = ev.info.source;
	info.flags = ev.info.flags;
	info.linenumber = ev.info.linenumber;
	threadnum = ev.threadnum;

	if ( ev.data )
		{
		num = ev.data->NumObjects();

		data = new Container<str>;
		data->Resize( num );

		for( i = 1; i <= num; i++ )
			{
			data->AddObject( ev.data->ObjectAt( i ) );
			}
		}
	}

Event::Event
	(
	Event *ev
	)

	{
	int num;
	int i;

   assert( ev );
   if ( !ev )
      {
      Class::error( "Event", "NULL Event\n" );
      }

	eventnum = ( int )*ev;
	assert( ( eventnum > 0 ) && eventnum <= commandList->NumObjects() );
	data = NULL;
   name = commandList->ObjectAt( eventnum )->c_str();
	info.inuse = 0;
	info.source = ev->info.source;
	info.flags = ev->info.flags;
	info.linenumber = ev->info.linenumber;
	threadnum = ev->threadnum;
	if ( ev->data )
		{
		num = ev->data->NumObjects();

		data = new Container<str>;
		data->Resize( num );

		for( i = 1; i <= num; i++ )
			{
			data->AddObject( ev->data->ObjectAt( i ) );
			}
		}
	}

Event::Event
	(
	const char *command,
	int flags
	)

	{
	str c;
   str *t;

	if ( !commandList )
		{
		initCommandList();
		}

	c = command;
	eventnum = FindEvent( c );
	if ( !eventnum )
		{
      t = new str( c );
		eventnum = commandList->AddObject( t );
		// check for default flags
		if ( flags == -1 )
			{
			flags = 0;
			}
		flagList->AddObject( ( int )flags );
		sortedList->AddObject( eventnum );
		dirtylist = qtrue;
		}

   // Use the name stored in the command list in case the string passed in
   // is not in static memory.
   name = commandList->ObjectAt( eventnum )->c_str();

	data = NULL;
	info.inuse = 0;
	info.source = EV_FROM_CODE;
	info.linenumber = 0;
	threadnum = -1;

	// If flags have changed, let the user know.  It's probably a development bug.
	int &flagobj = flagList->ObjectAt( eventnum );

	// check for default flags
	if ( flags == -1 )
		{
		flags = flagobj;
		}

	assert( flags == flagobj );
	if ( flags != flagobj )
		{
		// Flags not equal.  Use combined value.
		flagobj |= flags;
		}

	info.flags = flagobj;
	}

Event::Event
	(
	str &command,
	int flags
	)

	{
   str *t;

	if ( !commandList )
		{
		initCommandList();
		}

	eventnum = FindEvent( command );
	if ( !eventnum )
		{
      t = new str( command );
		eventnum = commandList->AddObject( t );
		// check for default flags
		if ( flags == -1 )
			{
			flags = 0;
			}
		flagList->AddObject( flags );
		sortedList->AddObject( eventnum );
		dirtylist = qtrue;
		}

   // Use the name stored in the command list since the string passed in
   // is not in static memory.
   name = commandList->ObjectAt( eventnum )->c_str();
   data = NULL;
	info.inuse = 0;
	info.source = EV_FROM_CODE;
	info.linenumber = 0;
	threadnum = -1;

	// If flags have changed, let the user know.  It's probably a development bug.
	int &flagobj = flagList->ObjectAt( eventnum );

	// check for default flags
	if ( flags == -1 )
		{
		flags = flagobj;
		}

	assert( flags == flagobj );
	if ( flags != flagobj )
		{
		// Flags not equal.  Use combined value.
		flagobj |= flags;
		}

	info.flags = flagobj;
	}

 Event::~Event()
	{
	if ( data )
		{
		delete data;
		data = NULL;
		}
	}

void Event::Error
	(
	const char *fmt,
	...
	)

	{
	va_list			argptr;
	char				text[ 1024 ];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

   cgi.Printf( "Client: '%s' : %s\n", getName().c_str(), text );
	}

const char *Event::GetString
	(
	int pos
	)

	{
	const char *text;

	if ( !data || ( pos < 1 ) || ( data->NumObjects() < pos ) )
		{
		Error( "Index %d out of range.", pos );
		return "";
		}

	text = data->ObjectAt( pos ).c_str();
	assert( text );

   /* GAMEFIX
	var = Director.GetExistingVariable( text );
	if ( var )
		{
		return var->stringValue();
		}
   */
	return text;
	}

void *Event::GetPointer
	(
	int pos
	)

	{
	const char *text;

	if ( !data || ( pos < 1 ) || ( data->NumObjects() < pos ) )
		{
		Error( "Index %d out of range.", pos );
		return 0;
		}

	text = data->ObjectAt( pos ).c_str();
	assert( text );

   if ( !IsNumeric( text ) )
		{
		Error( "Expecting a numeric value but found '%s'.", text );
		return 0;
		}

	return (void *)atoi( text );
	}

int Event::GetInteger
	(
	int pos
	)

	{
	const char *text;

	if ( !data || ( pos < 1 ) || ( data->NumObjects() < pos ) )
		{
		Error( "Index %d out of range.", pos );
		return 0;
		}

	text = data->ObjectAt( pos ).c_str();
	assert( text );

   /* GAMEFIX
	var = Director.GetExistingVariable( text );
	if ( var )
		{
		if ( !IsNumeric( var->stringValue() ) )
			{
			Error( "Variable %s contains non-numeric value '%s'", text, var->stringValue() );
			}
		return var->intValue();
		}
   */

	if ( !IsNumeric( text ) )
		{
		Error( "Expecting a numeric value but found '%s'.", text );
		return 0;
		}

	return atoi( text );
	}

double Event::GetDouble
	(
	int pos
	)

	{
	const char *text;

	if ( !data || ( pos < 1 ) || ( data->NumObjects() < pos ) )
		{
		Error( "Index %d out of range.", pos );
		return 0;
		}

	text = data->ObjectAt( pos ).c_str();
	assert( text );

	/* GAMEFIX
   var = Director.GetExistingVariable( text );
	if ( var )
		{
		if ( !IsNumeric( var->stringValue() ) )
			{
			cgi.Error( "Variable %s contains non-numeric value '%s'", text, var->stringValue() );
			}
		return ( double )var->floatValue();
		}
   */

	if ( !IsNumeric( text ) )
		{
		Error( "Expecting a numeric value but found '%s'.", text );
		return 0;
		}

	return ( double )atof( text );
	}

float Event::GetFloat
	(
	int pos
	)

	{
	const char *text;

	if ( !data || ( pos < 1 ) || ( data->NumObjects() < pos ) )
		{
		Error( "Index %d out of range.", pos );
		return 0;
		}

	text = data->ObjectAt( pos ).c_str();
	assert( text );

   /* GAMEFIX
	var = Director.GetExistingVariable( text );
	if ( var )
		{
		if ( !IsNumeric( var->stringValue() ) )
			{
			cgi.Error( "Variable %s contains non-numeric value '%s'", text, var->stringValue() );
			}
		return var->floatValue();
		}
   */

	if ( !IsNumeric( text ) )
		{
		Error( "Expecting a numeric value but found '%s'.", text );
		return 0;
		}

	return atof( text );
	}

Vector Event::GetVector
	(
	int pos
	)

	{
	const char *text;

	if ( !data || ( pos < 1 ) || ( data->NumObjects() < pos ) )
		{
		Error( "Index %d out of range.", pos );
		return vec_zero;
		}

	text = data->ObjectAt( pos ).c_str();
	assert( text );

   /* GAMEFIX
	var = Director.GetExistingVariable( text );
	if ( var )
		{
		text = var->stringValue();
		}
   */

	// Check if this is a ()-based vector
	// we accept both, but using parenthesis we can determine if it is a vector or not
	if ( text[ 0 ] == '(' )
		{
		return Vector( &text[ 1 ] );
		}

	// give an cgi.Error, but try converting it anyways
	Error( "Vector '%s' does not include '(' ')'.", text );
	return Vector( text );
	}

CLASS_DECLARATION( Class, Listener, NULL )
	{
	   { &EV_Remove,				Listener::Remove },
		{ &EV_ScriptRemove,		Listener::ScriptRemove },
		{ NULL, NULL }
	};


void Listener::Remove
	(
	Event *e
	)

	{
	delete this;
	}

void Listener::ScriptRemove
	(
	Event *e
	)

	{
	// Forces the remove to be done at a safe time
	PostEvent( EV_Remove, 0 );
	}

#if 0
void Listener::FloatVar
	(
	Event &e,
	float *var,
	float defaultvalue
	)

	{
	}

void Listener::IntVar
	(
	Event &e,
	int *var,
	float defaultvalue
	)

	{
	}

void Listener::StringVar
	(
	Event &e,
	str *var,
	const char *defaultvalue
	)

	{
	}

void Listener::StringVar
	(
	Event &e,
	char **var,
	const char *defaultvalue
	)

	{
	}

void Listener::VectorVar
	(
	Event &e,
	Vector *var,
	Vector defaultvalue
	)

	{
	}
#endif

qboolean Listener::ValidEvent
	(
	Event &e
	)

	{
	ClassDef	*c;
	int		ev;

	ev = ( int )e;

	c = this->classinfo();
   assert( ( ev >= 0 ) && ( ev < c->numEvents ) );
	if ( ( ev < 0 ) || ( ev >= c->numEvents ) )
		{
      warning( "ValidEvent", "Event '%s' out of response range for class '%s'.  "
         "Event probably invalid or allocated late.\n", e.getName().c_str(), getClassname() );
		return qfalse;
		}

	return ( qboolean )( c->responseLookup[ ev ] != NULL );
   }

qboolean Listener::ValidEvent
	(
	const char *name
	)

	{
	ClassDef	*c;
	int		ev;

   ev = Event::FindEvent( name );

	c = this->classinfo();
   assert( ( ev >= 0 ) && ( ev < c->numEvents ) );
	if ( ( ev < 0 ) || ( ev >= c->numEvents ) )
		{
      warning( "ValidEvent", "Event '%s' out of response range for class '%s'.  "
         "Event probably invalid or allocated late.\n", name, getClassname() );
		return qfalse;
		}

	return ( qboolean )( c->responseLookup[ ev ] != NULL );
	}

qboolean Listener::EventPending
	(
	Event &ev
	)

	{
	eventcache_t *event;
	int eventnum;

	assert( EventQueue );
	assert( EventQueue->next );

	event = EventQueue->next;

	eventnum = ( int )ev;
	while( event != EventQueue )
		{
		if ( ( event->obj == this ) && ( (int)*event->event == eventnum ) )
			{
			return qtrue;
			}
		event = event->next;
		}

	return qfalse;
   }

inline qboolean Listener::CheckEventFlags
	(
	Event *event
	)

	{
   /* GAMEFIX
	// Special handling of console events
	if ( event->GetSource() == EV_FROM_CONSOLE )
		{
		if ( !( event->info.flags & (EV_CONSOLE|EV_CHEAT) ) )
			{
			if ( isSubclassOf( Entity ) )
				{
				Entity *ent;

				ent = ( Entity * )this;
            gi.SendServerCommand( ent->edict, "print \"Command not available from console.\n\"" );
				}

			// don't process
			return qfalse;
			}

		// don't allow console cheats during deathmatch unless the server says it's ok.
      if ( ( event->info.flags & EV_CHEAT ) && deathmatch->integer && !sv_cheats->integer )
			{
			if ( isSubclassOf( Entity ) )
				{
				Entity *ent;

				ent = ( Entity * )this;
            gi.SendServerCommand( ent->edict, "print \"You must run the server with '+set cheats 1' to enable this command.\n\"" );
				}

			// don't process
			return qfalse;
			}
		}
   */
	// ok to process
	return qtrue;
	}

qboolean Listener::ProcessEvent
	(
	Event *event
	)

	{
	ClassDef *c;
	int		ev;
	int		i;

	// Prevent overflow of the inuse count
	if ( event->info.inuse >= MAX_EVENT_USE )
		{
		cgi.Error( ERR_DROP, "ProcessEvent : Event usage overflow on '%s' event.  Possible infinite loop.\n", event->getName().c_str() );
		}

   if ( cg_showevents->integer )
		{
		int n;
		str text;

		text = va( "%.1f: %s", (float)cgi.Milliseconds()/1000.0f, this->getClassname() );
		text += event->getName().c_str();
		n = event->NumArgs();
		for( i = 1; i <= n; i++ )
			{
			text += va( " %s", event->GetToken( i ) );
			}

		text += "\n";
		}

	ev = ( int )*event;
	c = this->classinfo();
	if ( ev >= c->numEvents	)
		{
      event->Error( "Event out of response range for class '%s'.  Event probably invalid or allocated late.\n", getClassname() );
		return qfalse;
		}

	if ( c->responseLookup[ ev ] )
		{
      int start;
      int end;

		event->info.inuse++;

      if ( !cg_timeevents->integer )
         {
         // only process the event if we allow it
		   if ( CheckEventFlags( event ) )
			   {
			   ( this->**c->responseLookup[ ev ] )( event );
			   }
         }
      else
         {
         start = cgi.Milliseconds();

		   // only process the event if we allow it
		   if ( CheckEventFlags( event ) )
			   {
			   ( this->**c->responseLookup[ ev ] )( event );
			   }

         end = cgi.Milliseconds();

         if ( cg_timeevents->integer == 1 )
            {
            cgi.Printf( "'%s' : %d\n", event->getName().c_str(), end - start );
            }
         else
            {
            // GAMEFIX G_DebugPrintf( "'%s' : %d\n", event->getName().c_str(), end - start );
            }
         }

		// Prevent an event from being freed twice, in case it's been re-used
		event->info.inuse--;
		if ( !event->info.inuse )
			{
			delete event;
			}

      return qtrue;
		}

   if ( !event->info.inuse )
		{
		delete event;
		}

	return qfalse;
	}

void Listener::PostEvent
	(
	Event *ev,
	float time
	)

	{
	eventcache_t *newevent;
	eventcache_t *event;

	if ( LL_Empty( FreeEvents, next, prev ) )
		{
		cgi.Error( ERR_DROP, "PostEvent : No more free events on '%s' event.\n", ev->getName().c_str() );
		return;
		}

	newevent = FreeEvents->next;
	LL_Remove( newevent, next, prev );

	// Probably don't have this many events, but it's a good safety precaution
	if ( ev->info.inuse >= MAX_EVENT_USE )
		{
		cgi.Error( ERR_DROP, "PostEvent : Event usage overflow on '%s' event.  Possible infinite loop.\n", ev->getName().c_str() );
		return;
		}

	ev->info.inuse++;

	newevent->obj		= this;
	newevent->event	= ev;
	newevent->time		= ( ( float )cgi.Milliseconds() / 1000.0f ) + time;

	event = EventQueue->next;
	while( ( event != EventQueue ) && ( newevent->time >= event->time ) )
		{
		event = event->next;
		}

	LL_Add( event, newevent, next, prev );
	numEvents++;
	}

qboolean Listener::PostponeEvent
	(
	Event &ev,
	float time
	)

	{
	eventcache_t *event;
	eventcache_t *node;
	int eventnum;

	assert( EventQueue );
	assert( EventQueue->next );

	eventnum = ( int )ev;

	event = EventQueue->next;
	while( event != EventQueue )
		{
		if ( ( event->obj == this ) && ( ( int )*event->event == eventnum ) )
			{
			event->time	+= time;

			node = event->next;
			while( ( node != EventQueue ) && ( event->time >= node->time ) )
				{
				node = node->next;
				}

			LL_Remove( event, next, prev );
			LL_Add( node, event, next, prev );

			return qtrue;
			}
		event = event->next;
		}

	return qfalse;
	}

void Listener::CancelEventsOfType
	(
	Event *ev
	)

	{
	eventcache_t *event;
	eventcache_t *next;
	int eventnum;

	assert( EventQueue );
	assert( EventQueue->next );

	event = EventQueue->next;

	eventnum = (int)*ev;
	while( event != EventQueue )
		{
		next = event->next;
		if ( ( event->obj == this ) && ( (int)*event->event == eventnum ) )
			{
			delete event->event;
			event->event = NULL;
			LL_Remove( event, next, prev );
			LL_Add( FreeEvents, event, next, prev );
			numEvents--;
			}
		event = next;
		}
	}

void Listener::CancelPendingEvents
	(
	void
	)

	{
	eventcache_t *event;
	eventcache_t *next;

	assert( EventQueue );
	assert( EventQueue->next );

	event = EventQueue->next;

	while( event != EventQueue )
		{
		next = event->next;
		if ( event->obj == this )
			{
			delete event->event;
			event->event = NULL;
			LL_Remove( event, next, prev );
			LL_Add( FreeEvents, event, next, prev );
			numEvents--;
			}
		event = next;
		}
	}

qboolean Listener::ProcessPendingEvents
	(
	void
	)

	{
	eventcache_t *event;
	qboolean processedEvents;
	float t;

	assert( EventQueue );
	assert( EventQueue->next );
	assert( EventQueue->prev );

	processedEvents = qfalse;

	t = ( ( float )cgi.Milliseconds() / 1000.0f ) + 0.001;

	event = EventQueue->next;
	while( event != EventQueue )
		{
		assert( event );
		assert( event->event );
		assert( event->obj );

		if ( event->time > t )
			{
			break;
			}

		if ( event->obj != this )
			{
			// traverse normally
			event = event->next;
			}
		else
			{
			LL_Remove( event, next, prev );
			numEvents--;

			assert( event->obj );

			// ProcessEvent increments the inuse count, so decrement it since we've already incremented it in PostEvent
			event->event->info.inuse--;

			event->obj->ProcessEvent( event->event );

			event->event = NULL;
			LL_Add( FreeEvents, event, next, prev );

			// start over, since can't guarantee that we didn't process any previous or following events
			event = EventQueue->next;

			processedEvents = qtrue;
			}
		}

	return processedEvents;
   }

Listener::~Listener()
	{
	CancelPendingEvents();
	}

void CG_ClearEventList
	(
	void
	)

	{
	int i;
	eventcache_t *e;

	LL_Reset( FreeEvents, next, prev );
	LL_Reset( EventQueue, next, prev );
	memset( Events, 0, sizeof( Events ) );

	for( e = &Events[ 0 ], i = 0; i < MAX_EVENTS; i++, e++ )
		{
		LL_Add( FreeEvents, e, next, prev );
		}

	numEvents = 0;
	}

void CG_ProcessPendingEvents
	(
	void
	)

	{
	eventcache_t *event;
	float t;
   int num;
   int maxevents;

	assert( EventQueue );
	assert( EventQueue->next );
	assert( EventQueue->prev );

   maxevents = ( int )cg_eventlimit->integer;

   num = 0;
	t = ( ( float )cgi.Milliseconds() / 1000.0f ) + 0.001;
	while( !LL_Empty( EventQueue, next, prev ) )
		{
		event = EventQueue->next;

		assert( event );
		assert( event->event );
		assert( event->obj );

		if ( event->time > t )
			{
			break;
			}

		LL_Remove( event, next, prev );
		numEvents--;

		assert( event->obj );

		// ProcessEvent increments the inuse count, so decrement it since we've already incremented it in PostEvent
      assert( event->event->info.inuse > 0 );
		event->event->info.inuse--;

		event->obj->ProcessEvent( event->event );

		event->event = NULL;
		LL_Add( FreeEvents, event, next, prev );

      // Don't allow ourselves to stay in here too long.  An abnormally high number
      // of events being processed is evidence of an infinite loop of events.
      num++;
      if ( num > maxevents )
         {
         cgi.Printf( "Event overflow.  Possible infinite loop in script.  "
            "Enable g_showevents to trace.  Aborting frame.\n" );
         break;
         }
		}
	}

void CG_InitEvents
	(
	void
	)

   {
	cg_numevents   = cgi.Cvar_Get( "cg_numevents", "0", 0 );
	cg_showevents  = cgi.Cvar_Get( "cg_showevents", "0", 0 );
   cg_eventlimit  = cgi.Cvar_Get( "cg_eventlimit", "1500", 0 );
   cg_timeevents  = cgi.Cvar_Get( "cg_timeevents", "0", 0 );
   cg_watch       = cgi.Cvar_Get( "cg_watch", "0", 0 );

	BuildEventResponses();
	CG_ClearEventList();
   CG_InitCommandManager();
	// Sort the list before we go on since we won't be adding any more events
	Event::SortEventList();
	}