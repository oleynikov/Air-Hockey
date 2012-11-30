#pragma once

#include <pthread.h>

namespace o__O
{
	
	enum ThreadState
	{
		THREAD_STATE_UNBORN,
		THREAD_STATE_ALIVE,
		THREAD_STATE_DEAD
	};
	
	class Thread
	{

		private:
		
							ThreadState			state;
							pthread_t			thread;
		
		public:
		
												Thread ( void );
							void				SetState ( const ThreadState state );
							ThreadState			GetState ( void ) const;
							pthread_t&			GetThread ( void );
	};
	
}