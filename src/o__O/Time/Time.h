#pragma once

#include <time.h>

#define BILLION 1000000000

namespace o__O
{
	
	class Time
	{
		
		public:
		
							//	Public members
							long				seconds;
							long				nseconds;
							
							//	Interface
												Time ( void );
												Time ( long seconds , long nseconds );
							Time				operator+ ( Time time ) const;
							Time				operator- ( Time time ) const;
							Time				operator* ( unsigned long multiplier ) const;
		
							//	Static interface
			static			Time				Now ( void );
			static			Time				Normalize ( const Time time );
			static			bool				Passed ( Time timeCheck );
			static			bool				Passed ( Time timeBegin, Time interval );
		
	};
	
}