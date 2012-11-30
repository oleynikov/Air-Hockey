#pragma once

#include <map>
#include <vector>
#include <iostream>
#include "../Thread/Thread.h"

namespace o__O
{
	
	/*											
												
												
			class FunctorBase					
			class Functor						
												
												
	*/
	
	class FunctorBase
	{

		public:
			virtual void Invoke ( void* data ) const
			{
				
			}
			
	};
	
	template <class type>
	class Functor : public FunctorBase
	{

		private:
			virtual void Invoke ( void* data ) const
			{
				(object->*method)(data);
			}

		public:
			type* object;
			void(type::*method)(void*);

			Functor ( type* object , void(type::*method)(void*) ) : object(object) , method(method)
			{
			
			}

	};
	
	/*												
													
													
			class EventDispatcher					
													
													
	*/
	
	class EventDispatcher
	{

		private:
			std::multimap<int,FunctorBase*> subscriptions;
			pthread_mutex_t mutex;
			Thread thread;
		
			
			static void* DispatchEventRoutine ( void* eventPtr );

		protected:
			void DispatchEvent ( int eventId , void* eventDataPtr );

		public:
			EventDispatcher ( void );
		
			//	Виртуальный деструктор
			virtual ~EventDispatcher ( void );
		
			//	Найти итератор подписки, если она существует
			template <class Listener>
			std::multimap<int,FunctorBase*>::iterator SubscriptionIterator ( int eventId , Functor<Listener>* functor );
			
			//	Добавить подписку
			template <class Listener>
			bool AddEventListener ( int eventId , Listener* listener , void(Listener::*action)(void*) );

			//	Удалить подписку
			template <class Listener>
			bool RemoveEventListener ( int eventId , Listener* listener , void(Listener::*action)(void*) );
			
	};
	
	struct Event
	{
		int id;
		EventDispatcher& dispatcher;
		void* data;
	};

	
	template <class Listener>
	std::multimap<int,FunctorBase*>::iterator EventDispatcher::SubscriptionIterator ( int eventId , Functor<Listener>* functor )
	{
		
		std::multimap<int,FunctorBase*>::iterator key;
		std::pair <std::multimap<int,FunctorBase*>::iterator,std::multimap<int,FunctorBase*>::iterator> range;
		
		range = subscriptions.equal_range ( eventId );
		
		for ( key=range.first ; key!=range.second; ++key )
		{

			Functor<Listener>* functorCheck = dynamic_cast < Functor<Listener>* > ( (*key).second );
			
			if	( functorCheck && functorCheck->object == functor->object && functorCheck->method == functor->method )
			{
				return key;
			}
			
		}
		
		return subscriptions.end();
	}

	template <class Listener>
	bool EventDispatcher::AddEventListener ( int eventId , Listener* listener , void(Listener::*method)(void*) )
	{
		
		pthread_mutex_lock ( &mutex );
		Functor<Listener>* functor = new Functor<Listener> ( listener , method );

		if ( SubscriptionIterator ( eventId , functor ) == subscriptions.end() )
		{

			subscriptions.insert ( std::pair <int,FunctorBase*> ( eventId , functor ) );
			pthread_mutex_unlock ( &mutex );
			return true;

		}

		pthread_mutex_unlock ( &mutex );
		return false;
		
	}

	template <class Listener>
	bool EventDispatcher::RemoveEventListener ( int eventId , Listener* listener , void(Listener::*method)(void*) )
	{

		pthread_mutex_lock ( &mutex );
		Functor<Listener>* functor = new Functor<Listener> ( listener , method );
		std::multimap<int,FunctorBase*>::iterator subscriptionIterator = SubscriptionIterator ( eventId , functor );
		
		if ( subscriptionIterator != subscriptions.end() )
		{
			
			subscriptions.erase ( subscriptionIterator );
			pthread_mutex_unlock ( &mutex );
			return true;
			
		}
		
		pthread_mutex_unlock ( &mutex );
		return false;
		
	}
	
};