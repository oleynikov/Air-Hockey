#include "Time.h"

using namespace o__O;

				Time::Time ( void )
{
	
	seconds = 0;
	nseconds = 0;
	
}

				Time::Time ( long seconds , long nseconds )
{
	
	this->seconds = seconds;
	this->nseconds = nseconds;
	
	
}

Time			Time::operator+ ( Time time ) const
{
	
	//	Суммируем время
	Time timeResult ( seconds+time.seconds , nseconds+time.nseconds );
	
	//	Нормализуем время и возвращаем его
	return Time::Normalize ( timeResult );

}

Time			Time::operator- ( Time time ) const
{
	
	//	Вычитаем время
	Time timeResult ( seconds-time.seconds , nseconds-time.nseconds );
	
	//	Нормализуем время и возвращаем его
	return Time::Normalize ( timeResult );

}

Time			Time::operator* ( unsigned long multiplier ) const
{

	Time timeResult;
	
	if ( multiplier > 0 )
	{
		
		timeResult = *this;

		for ( unsigned long key = 1 ; key < multiplier ; ++key )
		{
			
			timeResult.seconds += seconds;
			timeResult.nseconds += nseconds;
			
		}
		
		timeResult = Time::Normalize ( timeResult );
		
	}
	
	return timeResult;
	
}

Time			Time::Now ( void )
{
	
	timespec timespecCurrent;
	clock_gettime ( CLOCK_MONOTONIC , &timespecCurrent );
	
	Time timeCurrent ( timespecCurrent.tv_sec , timespecCurrent.tv_nsec );
	return timeCurrent;
	
}

Time			Time::Normalize ( const Time time )
{
	
	//	Приведение времени к нормальной форме:
	//	1. Число наносекунд по модулю должно быть меньше, чем BILLION
	//	2. Числа наносекнд и секунд не должны быть разных знаков.
	
	Time result = time;
	
	if ( result.nseconds >= BILLION || result.nseconds <= -1 * BILLION )
	{
		int secondsExcess = result.nseconds / BILLION;
		result.seconds += secondsExcess;
		result.nseconds -= secondsExcess * BILLION;
	}
	
	if ( result.seconds > 0 && result.nseconds < 0 )
	{
		result.seconds--;
		result.nseconds += BILLION;
	}
	else if ( result.seconds < 0 && result.nseconds > 0 )
	{
		result.seconds++;
		result.nseconds -= BILLION;
	}
	
	return result;

}

bool			Time::Passed ( const Time timeCheck )
{
	
	Time timeDelta = Time::Now() - timeCheck;
	
	if ( timeDelta.nseconds >= 0 )
	{
		
		return true;
		
	}
	
	return false;
	
}

bool			Time::Passed ( const Time timeBegin , const Time timeInterval )
{
	
	Time timeFinish = timeBegin + timeInterval;
	Time timeDelta = Time::Now() - timeFinish;
	
	if ( timeDelta.nseconds >= 0 )
	{
		
		return true;
		
	}

	return false;

}