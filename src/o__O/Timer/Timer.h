#pragma once

#include "../Events/Events.h"
#include "../Thread/Thread.h"
#include "../Time/Time.h"

#include <iostream>
#include <errno.h>

namespace o__O
{
	
	enum TimerState
	{
		
		TIMER_STATE_UNREADY,
		TIMER_STATE_READY,
		TIMER_STATE_ACTIVE
		
	};

	enum TimerEvent
	{
		
		TIMER_EVENT_TICK,
		TIMER_EVENT_COMPLETE,
		TIMER_EVENT_START,
		TIMER_EVENT_PAUSE,
		TIMER_EVENT_STOP
		
	};
	
	class Timer : public EventDispatcher
	{
		
		public:
		
							//	System Interface
												Timer ( void );
												Timer ( const Time tickInterval, const unsigned long tickRepeatLimit = 0 );
												Timer ( const Timer& );
											   ~Timer ( void );
							Timer&			   operator = ( const Timer& );
											   
							//	Control Interface
							bool				Start ( const bool instantly = false );
							bool				Restart ( const bool instantly = false );
							bool				Pause ( void );
							bool				Stop ( void );
							unsigned long		GetTickCurrent ( void );
							unsigned long		GetTickLimit ( void );
							Time				GetTickInterval ( void );

		private:
		
							//	Private properties
							Time				tickInterval;
							unsigned long		tickLimit;
							Thread				thread;
							pthread_mutex_t		mutex;
							pthread_mutex_t		mutexRead;
							unsigned long		tickCurrent;
							Time				tickLastTime;
							bool				active;

							//	Private methods
							bool				Initiate ( void );
							void				Launch ( const bool instantly );
							void				Tick ( void );
							TimerState			GetState ( void ) const;
			static			void*				Routine ( void* timerPointer );

	};
	
}