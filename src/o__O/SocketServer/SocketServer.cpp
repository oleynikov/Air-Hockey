#include "SocketServer.h"

using namespace o__O;

		SocketServer::SocketServer ( const std::string ip , const int port ) : ip ( ip ) , port ( port )
{

	//	Создаем сокет
	SocketCreate();
	
	//	Привязываем сокет к IP и порту
	SocketBind();
	
	//	Запускаем прослушивание сокета
	SocketListen();
	
	//	Запускаем рабочий поток сервера для ассинхронной прослушки сокета
	pthread_create ( &threadAccept , NULL , SocketServer::SocketAccept , this );
	
}

void	SocketServer::SocketCreate ( )
{
	
	socketFD = socket ( AF_INET , SOCK_STREAM , 0 );
	
	if ( socketFD < 0 )
	{

		perror ( "socket" );

	}
	
	else
	{

		//	Включаем повторное использование адресов
		int reuseAddress = 1;
		setsockopt ( socketFD , SOL_SOCKET , SO_REUSEADDR , &reuseAddress , sizeof(reuseAddress) );
		std::cout << "Socket created successfully. Socket FD is " << socketFD << "\n";

	}
	
}

void	SocketServer::SocketBind ( )
{
	/*
		bind a socket to a port @ a local address
	*/
	
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons ( port );
	address.sin_addr.s_addr = inet_addr ( ip.data() );

	if ( bind ( socketFD , (sockaddr*)&address , sizeof ( address ) ) < 0 )
	{
		
		perror ( "bind" );
		
	}
	
	else
	{
		
		std::cout << "Socket bound successfully to the " << ip << ":" << port << " address\n";
		
	}
	
}

void	SocketServer::SocketListen ( )
{
	
	/*
		start to listen socket
	*/
	
	listen ( socketFD , 5 );
	std::cout << "Socket is beeing listened now\n";
	
}

void*	SocketServer::SocketAccept ( void* serverPtr )
{

	SocketServer* server = static_cast<SocketServer*>(serverPtr);
	
	// Запускаем бесконечный цикл
	while ( 1 )
	{
		
		//	Принимаем соединение из очереди
		int socketFD = accept ( server->socketFD , NULL , NULL );
		
		//	Если произошла ошибка
		if ( socketFD < 0 )
		{
			
			perror ( "accept" );
			
		}
		
		//	В случае успешного соединения
		else
		{
			
			//	Извещаем слушателей о подключении клиента
			SocketServer::Log ( socketFD ,  "C O N N E C T E D" );
			ClientRequest clientRequest = { socketFD , "" };
			server->DispatchEvent ( SOCKET_SERVER_EVENT_CLIENT_CONNECTED , &clientRequest );
			
			//	Запускаем поток клиента и передаем ему ссылку на сервер и ФД сокетад
			Client client = { server , socketFD };
			pthread_t clientThread;
			pthread_create ( &clientThread , NULL , SocketServer::SocketRead, &client );
			
		}
		
	}
	
}

void*	SocketServer::SocketRead ( void* clientPtr )
{

	Client* client = static_cast<Client*>(clientPtr);
	SocketServer* server = client->serverPtr;
	int socketFD = client->socketFD;

	//	Буфера данных сокета и его размер
	char input[256];
	int inputLength;
	
	//	Запускаем бесконечный цикл, ожидающий данные на сокете
	while ( 1 )
	{
		
		//	Очищаем буфер данных сокета
		memset ( static_cast<void*>(&input) , '\0' , sizeof ( input ) );
		
		//	Считываем данные из сокета
		inputLength = read ( socketFD , static_cast<void*>(&input) , 255 );
		
		//	Ошибка считывания
		if ( inputLength < 0 )
		{
			perror ( "read" );
		}
		
		//	Клиент отключился
		else if ( inputLength == 0 || input[0] == '\0' )
		{
			
			SocketServer::Log ( socketFD ,  "D I S C O N N E C T E D" );
			ClientRequest clientRequest = { socketFD , "" };
			server->DispatchEvent ( SOCKET_SERVER_EVENT_CLIENT_DISCONNECTED , &clientRequest );
			pthread_exit( NULL );
			
		}
		
		//	Новые данные на сокете
		else
		{

			SocketServer::Log ( socketFD ,  "---> " + std::string(input) );
			ClientRequest clientRequest = { socketFD , input };
			server->DispatchEvent ( SOCKET_SERVER_EVENT_CLIENT_SENT_REQUEST , &clientRequest );
			
		}
		
	}
	
}

void	SocketServer::Send ( int socketFD , std::string message )
{
	
	int bytesWritten = write ( socketFD , message.c_str() , message.size() + 1 );

	if ( bytesWritten < 0 )
	{
		perror ( "write" );
	}
	
	else
	{
		SocketServer::Log ( socketFD , "<--- " + message );
	}
	
}

void	SocketServer::Log ( int socketFD , std::string message )
{

	sockaddr address;
	socklen_t addressLength = sizeof ( address );
	
	if ( getpeername ( socketFD , &address , &addressLength ) == 0 )
	{
		
		sockaddr_in* addressInternet = reinterpret_cast<sockaddr_in*>(&address);
		std::string peerIp = inet_ntoa ( addressInternet->sin_addr );
		int peerPort = ntohs ( addressInternet->sin_port );
		//std::cout << peerIp << ":" << peerPort << " " << message << "\n";
		
	}

}
