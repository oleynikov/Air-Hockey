#include "Timer.h"

using namespace o__O;

					Timer::Timer ( void )
{

	/*
	
		Конструктор по умолчанию
		
	*/
	
	

	//	Инициализируем таймер.
	//	В конструкторе по умолчанию это необходимо для корректной
	//	отработки деструктора класса (блокировка мьютекса)
	Initiate();

}

					Timer::Timer ( const Time tickInterval , const unsigned long tickLimit )
{

	/*
	
		Основной конструктор
		
	*/
	
	
	
	//	Инициализируем таймер
	Initiate();

	// Устанавливаем интервал срабатывания и лимит срабатываний
	this->tickInterval = Time::Normalize ( tickInterval );
	this->tickLimit = tickLimit;
	
}

					Timer::Timer ( const Timer& timer )
{

	/*
	
		Конструктор копирования таймера
		
	*/
	
	
	
	//	Инициализируем таймер
	Initiate();

	// Устанавливаем интервал срабатывания и лимит срабатываний
	tickInterval = Time::Normalize ( timer.tickInterval );
	tickLimit = timer.tickLimit;
	
}

Timer&				Timer::operator = ( const Timer& timer )
{

	/*
	
		= operator
		
	*/
	
	
	
	if ( this != &timer )
	{

		//	Инициализируем таймер
		Initiate();
		
		// Устанавливаем интервал срабатывания и лимит срабатываний
		tickInterval = Time::Normalize ( timer.tickInterval );
		tickLimit = timer.tickLimit;
		
	}
	
	return *this;
	
}

					Timer::~Timer ( void )
{

	/*
	
		Деструктор таймера
		
	*/


	
	//	Блокируем мьютекс
	pthread_mutex_lock ( &mutex );
	
	//	Проверяем, запущен ли поток "Routine". Он может быть еще не запущен или же уже остановлен
	if ( thread.GetState() == THREAD_STATE_ALIVE )
	{
	
		//	Если Routine запущен, возможны два случая:
		//
		//	1. 	Деструктор вызывается из потока, отличного от "Routine".
		//		В этом случае, все просто: можно завершить "Routine" с помощью "pthread_cancel()".
		//
		//	2.	Деструктор вызывается из потока "Routine" (в случае если удаление будет
		//		в обработчике события таймера. В этом случае все гораздо сложнее...
	

		//	Проверяем, из какого потока вызван дескриптор. Пробуем присоединить "Routine"
		int routineJoin = pthread_tryjoin_np ( thread.GetThread() , NULL );
		
		//	Если этот код вызовется из Routine вернется ошибка EDEADLK
		if ( routineJoin == EDEADLK )
		{
		
			//	Завершаем поток. Поток будет завершен сразу после выполнения всех обработчиков.
			pthread_cancel ( pthread_self() );
		
		}
		
		//	Если этот код вызовется НЕ из Routine вернется EBUSY
		else if ( routineJoin == EBUSY )
		{
			
			//	Завершаем поток
			pthread_cancel ( thread.GetThread() );
			
			//	Ждем пока поток завершится
			pthread_join ( thread.GetThread() , NULL );
		
		}

	}
	
	//	Отпускаем мьютекс
	pthread_mutex_unlock ( &mutex );

}

bool				Timer::Initiate ( void )
{
	
	// Инициализируем рекурсивный мьютекс
	pthread_mutexattr_t mutexAttrType;
	pthread_mutexattr_init ( &mutexAttrType );
	pthread_mutexattr_settype ( &mutexAttrType , PTHREAD_MUTEX_RECURSIVE );
	pthread_mutex_init ( &mutex , &mutexAttrType );
	
	// Устанавливаем флаги, обнуляем переменные
	active = false;
	tickCurrent = 0;
	
	return true;
	
}

void				Timer::Launch ( const bool instantly )
{

	/*
	
		ЗАПУСК ТАЙМЕРА
		
	*/
	
	
	//	Ставим флаг запуска таймера
	active = true;
	
	//	Тикаем сразу, если требуется
	if ( instantly )
	{

		Tick();

	}
	
	//	Или устанавливаем время начала работы таймера
	else
	{

		tickLastTime = Time::Now();

	}
	
	//	Поток "Routine" может все еще работать в данный момент
	//	Например, если после вызова "Stop()" сразу был вызван "Start()",
	//	И поток еще не успел завершиться. Необходима проверка!!!!
	//	Если поток "Routine" не запущен
	if ( thread.GetState() != THREAD_STATE_ALIVE )
	{
	
		//	Создаем поток `Routine`
		pthread_create ( &thread.GetThread() , NULL , Timer::Routine , this );
		
		//	Ждем пока запустится поток!!!!
		//	Иначе может получиться, последнее условие сработает снова
		//	И будут запущены ДВА дочерних потока! Это недопустимо!
		while ( thread.GetState() != THREAD_STATE_ALIVE )
		{
		
		}
		
	}

}

void				Timer::Tick ( void )
{
	
	// Обновляем время тика, инкрементируем счетчик таймера
	tickLastTime = Time::Now();
	tickCurrent++;
	
	// Отсылаем событие тика
	DispatchEvent ( TIMER_EVENT_TICK , &tickCurrent );
	
	// Проверяем, не превышен ли лимит тиков таймера
	if ( tickCurrent == tickLimit && tickLimit > 0 )
	{

		// Останавливаем таймер
		Stop();
		
		// Отсылаем событие окончания работы
		DispatchEvent ( TIMER_EVENT_COMPLETE , &tickCurrent );
		
	}
	
}

TimerState			Timer::GetState ( void ) const
{

	//	Если таймер запущен
	if ( active )
	{
		
		//	таймер работает
		return TIMER_STATE_ACTIVE;
		
	}
	
	//	Если таймер не запущен
	else
	{
		
		// Если указан интервал срабатывания таймера
		if ( tickInterval.seconds > 0 || tickInterval.nseconds > 0 )
		{
			
			//	Таймер готов к запуску
			return TIMER_STATE_READY;
			
		}

		//	Если интервал срабатывания не указан
		else
		{
			
			// Таймер не готов к запуску
			return TIMER_STATE_UNREADY;
			
		}
		
	}

}

void*				Timer::Routine ( void* timerPointer )
{
	
	/*
	
		Поток таймера
		
	*/


	
	//	Устанавливаем флаг потока, который позволит завершить его МОМЕНТАЛЬНО
	pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS , NULL );
	
	//	"timerPointer" всегда доступен в этой точке, т.к. деструтор таймера не
	//	может быть вызван до этой точки: Launch ждет, пока выполнится следующая строка
	Timer* timer = static_cast <Timer*> (timerPointer);
	
	// Сообщаем, что поток успешно запустился
	timer->thread.SetState ( THREAD_STATE_ALIVE );
	
	while ( 1 )
	{

		pthread_mutex_lock ( &timer->mutex );

		//	Если таймер все еще работает
		if ( timer->active )
		{
			
			//	Если прошло "tickInterval" секунд с момента "tickLastTime"
			if ( Time::Passed ( timer->tickLastTime , timer->tickInterval ) )
			{

				//	Запрещяем завершение потока, пока Tick не отработает
				pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE , NULL );
			
				//	Производим тик
				timer->Tick();

				//	Если вдруг в обработчиках события был вызван деструктор таймера,
				//	разрешаем завершение потока. Все обработчики были вызваны
				pthread_setcancelstate ( PTHREAD_CANCEL_ENABLE , NULL );
				
			}
			
		}
			
		//	Если таймер остановлен
		else
		{
		
			//	Если тик сброшен, сообщаем, что таймер остановлен
			//	Если тик не сброщен, сообщаем, что таймер на паузе
			timer->tickCurrent == 0	?	timer->DispatchEvent ( TIMER_EVENT_PAUSE , NULL )
									:	timer->DispatchEvent ( TIMER_EVENT_PAUSE , &timer->tickCurrent );
			
			//	Меняем состояние потока на "THREAD_STATE_DEAD"
			timer->thread.SetState ( THREAD_STATE_DEAD );
			
			pthread_mutex_unlock ( &timer->mutex );
			
			//	Останавливаем поток
			pthread_exit ( NULL );
			
		}
		
		pthread_mutex_unlock ( &timer->mutex );
		
	}
	
}

bool				Timer::Start ( const bool instantly )
{

	/*
	
		Запус таймера
		
	*/
	
	
	
	pthread_mutex_lock ( &mutex );
	
	//	Если таймер готов к запуску
	if ( GetState() == TIMER_STATE_READY )
	{

		//	Запускаем таймер
		Launch ( instantly );
		
		pthread_mutex_unlock ( &mutex );

		//	Таймер запущен
		return true;
		
	}
	
	pthread_mutex_unlock ( &mutex );

	//	Таймер либо уже запущен либо еще не подготовлен к запуску
	return false;
	
}

bool				Timer::Restart ( const bool instantly )
{

	/*
	
		ПЕРЕЗАПУСК ТАЙМЕРА
		
		Обнуляем счетчик и запускаем таймер
		
	*/
	
	
	
	//	Блокируем мьютес
	pthread_mutex_lock ( &mutex );
	
	//	Смотрим состояние таймера
	int timerState = GetState();
	
	//	Если таймер запущен или готов к запуску
	if ( timerState != TIMER_STATE_UNREADY )
	{

		//	Обнуляем счетчик таймера
		tickCurrent = 0;

		Launch ( instantly );
		
		pthread_mutex_unlock ( &mutex );

		//	Таймер перезапущен
		return true;
		
	}
	
	pthread_mutex_unlock ( &mutex );

	//	Таймер не готов к запуску
	return false;

}

bool				Timer::Pause ( void )
{

	pthread_mutex_lock ( &mutex );
	
	// Если таймер запущен
	if ( GetState() == TIMER_STATE_ACTIVE )
	{

		//	Ставим флаг остановки таймера
		active = false;

		//	Запрос на паузу отправлен
		pthread_mutex_unlock ( &mutex );
		return true;
		
	}
	
	//	Таймер не запущен
	pthread_mutex_unlock ( &mutex );
	return false;
	
}

bool				Timer::Stop ( void )
{

	pthread_mutex_lock ( &mutex );

	// Если таймер запущен
	if ( GetState() == TIMER_STATE_ACTIVE )
	{
		
		//	Ставим флаг остановки таймера
		active = false;
		
		//	Обнуляем счетчик таймера
		tickCurrent = 0;
		
		//	Запрос на остановку отправлен
		pthread_mutex_unlock ( &mutex );
		return true;
	
	}
	
	//	Таймер не запущен
	pthread_mutex_unlock ( &mutex );
	return false;
	
}

unsigned long		Timer::GetTickLimit ( void )
{

	pthread_mutex_lock ( &mutex );
	
	return tickLimit;

	pthread_mutex_unlock ( &mutex );
	
}

Time				Timer::GetTickInterval ( void )
{
	
	pthread_mutex_lock ( &mutex );

	return tickInterval;

	pthread_mutex_unlock ( &mutex );
	
}

unsigned long		Timer::GetTickCurrent ( void )
{

	pthread_mutex_lock ( &mutex );
	
	return tickCurrent;

	pthread_mutex_unlock ( &mutex );
	
}