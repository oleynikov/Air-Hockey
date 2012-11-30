#include "Timer/Timer.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace o__O;

class TimerUser
{
	private:
		int i;
	
	public:
		TimerUser()
		{
			Time timerInterval ( 0 , 300000000 );
			Timer timer ( timerInterval , 60 );
			timer.AddEventListener ( TIMER_EVENT_TICK , this , &TimerUser::TimerTickHandler );
			timer.Start();
			
			while ( 1 )
			{
			
			}
		}
		
		
		void TimerTickHandler ( void* eventData )
		{
			int tickCurrent = *((int*)eventData)-1;
			std::cout << "Tick #" << tickCurrent << "\n";
		};
		
};


int main()
{

	TimerUser* tu = new TimerUser();
	
	return 1;
	
};
