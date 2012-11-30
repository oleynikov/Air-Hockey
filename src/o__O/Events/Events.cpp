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

	pthread_mutex_lock ( &mutex );
	
	std::map<int,FunctorBase*>::iterator key;

	for ( key=subscriptions.begin() ; key!=subscriptions.end(); ++key )
	{

		delete (*key).second;

	}

	pthread_mutex_unlock ( &mutex );

}

void			EventDispatcher::DispatchEvent ( int eventId , void* eventData )
{

	// Создаем копию массива подписок
	pthread_mutex_lock ( &mutex );
	std::multimap<int,FunctorBase*> subscriptionsTemp = subscriptions;
	pthread_mutex_unlock ( &mutex );
	
	//	Ищем интервал со всеми подписками на событие "eventId"
	typedef std::multimap<int,FunctorBase*>::iterator subscriptionsItr;
	std::pair <subscriptionsItr,subscriptionsItr> range;
	range = subscriptionsTemp.equal_range ( eventId );
	
	//	Перебираем подписки из отобранного интервала
	for ( subscriptionsItr key=range.first ; key!=range.second; ++key )
	{
	
		//	Вызываем функции
		(*key).second->Invoke ( eventData );

	}
	
}