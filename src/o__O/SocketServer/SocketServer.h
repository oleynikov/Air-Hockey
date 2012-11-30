#pragma once

#include "../Events/Events.h"
#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

namespace o__O
{

	enum SocketServerEvent
	{
		SOCKET_SERVER_EVENT_CLIENT_CONNECTED,
		SOCKET_SERVER_EVENT_CLIENT_DISCONNECTED,
		SOCKET_SERVER_EVENT_CLIENT_SENT_REQUEST
	};
	
	class SocketServer : public EventDispatcher
	{
		private:
							std::string		ip;										//	IP адрес сервера
							unsigned int	port;									//	Порт сервера
							unsigned int	socketFD;								//	Серверный сокет
							pthread_t		threadAccept;							//	Рабочий поток
			
							void			SocketCreate ( void );
							void			SocketBind ( void );
							void			SocketListen ( void );
			static			void*			SocketAccept ( void* );
			static			void*			SocketRead ( void* );
			
		public:
											SocketServer ( const std::string , const int );
			static			void			Send ( int , std::string );
			static			void			Log ( int , std::string );
	};

	struct Client
	{
		SocketServer* serverPtr;
		int socketFD;
	};
	
	struct ClientRequest
	{
		int socketFD;
		std::string data;
	};
	
}