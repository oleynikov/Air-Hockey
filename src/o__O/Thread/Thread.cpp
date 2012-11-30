#include "Thread.h"

using namespace o__O;

				Thread::Thread ( void ) : state ( THREAD_STATE_UNBORN )
{

	/*
	
		Конструктор контролируемого потока
		
	*/
	
	

}

void			Thread::SetState ( const ThreadState state )
{

	/*
	
		Установка состояния потока
		
	*/
	
	
	
	this->state = state;
	
}

ThreadState		Thread::GetState ( void ) const
{

	/*
	
		Получение состояния потока
		
	*/
	
	
	
	return this->state;
	
}

pthread_t&		Thread::GetThread ( void )
{

	/*
	
		Получение ссылки на поток
		
	*/
	
	
	
	return this->thread;
	
}