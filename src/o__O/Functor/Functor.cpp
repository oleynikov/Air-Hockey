#include "Events.h"

using namespace o__O;

				EventDispatcher::EventDispatcher ( void )
{
	// Инициализируем рекурсивный мьютекс
	pthread_mutexattr_t mutexType;
	pthread_mutexattr_init ( &mutexType );
	pthread_mutexattr_settype ( &mutexType , PTHREAD_MUTEX_RECURSIVE );
	pthread_mutex_init ( &mutex , &mutexType );
}

				EventDispatcher::~EventDispatcher ( void )
{

	std::map<int,FunctorBase*>::iterator key;

	for ( key=subscriptions.begin() ; key!=subscriptions.end(); ++key )
	{

		delete (*key).second;

	}

};

void			EventDispatcher::DispatchEvent ( int eventId , void* eventData )
{
	
	pthread_mutex_lock ( &mutex );
	
	std::multimap<int,FunctorBase*>::iterator key;
	std::pair <std::multimap<int,FunctorBase*>::iterator,std::multimap<int,FunctorBase*>::iterator> range;

	range = subscriptions.equal_range ( eventId );

	for ( key=range.first ; key!=range.second; ++key )
	{
	
		(*key).second->Invoke ( eventData );

	}
	
	pthread_mutex_unlock ( &mutex );
	
	
	
	
	
	/*
	
	Event event = { eventId , *this , eventData };
	pthread_create ( &thread.GetThread() , NULL , EventDispatcher::DispatchEventRoutine , &event );

	while ( thread.GetState() == THREAD_UNBORN )
	{
		std::cout << thread.GetState() << " ";
	}

	pthread_join ( thread.GetThread() , NULL );
	pthread_mutex_unlock ( &mutex );
*/
}

void*			EventDispatcher::DispatchEventRoutine ( void* eventPtr )
{

	Event event = *((Event*)eventPtr);
	event.dispatcher.thread.SetState ( THREAD_ALIVE );
	
	std::multimap<int,FunctorBase*>::iterator key;
	std::pair <std::multimap<int,FunctorBase*>::iterator,std::multimap<int,FunctorBase*>::iterator> range;
	range = event.dispatcher.subscriptions.equal_range ( event.id );

	for ( key=range.first ; key!=range.second; ++key )
	{
	
		(*key).second->Invoke ( event.data );

	}
	
	event.dispatcher.thread.SetState ( THREAD_DEAD );
	
}