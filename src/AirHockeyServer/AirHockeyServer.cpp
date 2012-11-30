#include "AirHockeyServer.h"

using namespace o__O;

									AirHockeyServer::AirHockeyServer ( void )
{

	/*
	
		Сервер игры "Air Hockey"
		
	*/
	
	
	
	//	Инициализируем мьютексы массивов игроков и игр
	pthread_mutex_init ( &playersMutex , NULL );
	pthread_mutex_init ( &gamesMutex , NULL );

	//	Запускаем сокет-сервер
	o__O::SocketServer* socketServer = new SocketServer ( "88.198.237.65" , 44444 );

	//	Подписываемся на события сокет-сервера
	socketServer->AddEventListener ( SOCKET_SERVER_EVENT_CLIENT_CONNECTED , this , &AirHockeyServer::OnClientConnectHandler );
	socketServer->AddEventListener ( SOCKET_SERVER_EVENT_CLIENT_DISCONNECTED , this , &AirHockeyServer::OnClientDisconnectHandler );
	socketServer->AddEventListener ( SOCKET_SERVER_EVENT_CLIENT_SENT_REQUEST , this , &AirHockeyServer::OnClientRequestHandler );

	//	Приостанавливаем выполнение программы
	while ( 1 )
	{

	}

}

void								AirHockeyServer::OnClientConnectHandler ( void* data )
{

	/*
	
		Клиент подключился к серверу игры
		
	*/
	
	
	
	//	Конвертируем данные события в запрос игрока
	ClientRequest* playerRequest = static_cast <ClientRequest*> ( data );
	
	//	Берем дескриптор сокета игрока
	int playerSocketFD = playerRequest->socketFD;

	//	Создаем игрока
	PlayerAdd ( playerSocketFD );

}

void								AirHockeyServer::OnClientDisconnectHandler ( void* data )
{

	/*
	
		Клиент отключился от сервера игры
		
	*/
	

	
	//	Конвертируем данные события в запрос игрока
	ClientRequest* playerRequest = static_cast <ClientRequest*> ( data );
	
	//	Берем дескриптор сокета игрока
	int playerSocketFD = playerRequest->socketFD;
	
	//	Удаляем игрока
	PlayerDelete ( playerSocketFD );
	
}

void								AirHockeyServer::OnClientRequestHandler ( void* data )
{
	
	//	Берем данные из запроса
	ClientRequest*	clientRequest = static_cast <ClientRequest*> ( data );
	int				clientSocketFD = clientRequest->socketFD;
	std::string		clientRequestRaw = clientRequest->data;
	
	//	Если пользователь запросил файл политики безопасности
	if ( clientRequestRaw == "<policy-file-request/>" )
	{
	
		//	Отдаем ему файл политики безопасности
		SocketServer::Send ( clientSocketFD , "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE cross-domain-policy SYSTEM \"http://www.adobe.com/xml/dtds/cross-domain-policy.dtd\"><cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"44444\"/></cross-domain-policy>" );
	
	}
	
	//	В случае других запросов
	else
	{
		
		//	Разбиваем запрос разделителем "/"
		std::vector<std::string> clientRequestSplit;
		String::Split ( clientRequestSplit , clientRequestRaw , '/' );\
		
		//	Смотрим тип запроса
		int requestType = atoi ( clientRequestSplit[0].data() );
		
		//	Разбираем возможные типы запросов
		switch ( requestType )
		{
		
			//	Запрос на начало игры
			case 0:
			{
			
				//	Начинаем игру
				GameStart ( clientSocketFD );
				break;
				
			}
			
			//	Запрос на изменение направления движения игрока
			case 1:
			{
			
				int playerDirectionInt = atoi ( clientRequestSplit[1].data() );
				PlayerDirection playerDirection = static_cast <PlayerDirection> ( playerDirectionInt );
				PlayerDirectionSet ( clientSocketFD , playerDirection );
				break;
				
			}
			
		}
		
	}
	
}

void								AirHockeyServer::OnGameEndHandler ( void* data )
{

	/*
	
		Обработчик события окончания игры
		
	*/
	
	
	
	//	Получаем указатель на завершившуюся игру
	Game* gamePtr = static_cast <Game*> ( data );
	
	//	Удаляем игру
	GameDelete ( *gamePtr );
	
}


/*
			PLAYER
*/

std::vector<Player*>::iterator		AirHockeyServer::PlayerItr ( const int socketFD )
{

	std::vector<Player*>::iterator playerItr;
	
	for ( playerItr = players.begin() ; playerItr < players.end(); ++playerItr )
	{
	
		if ( (*playerItr)->socketFD == socketFD )
		{
		
			return playerItr;
			
		}
		
	}
	
	return players.end();
	
}

Player*								AirHockeyServer::PlayerPtr ( const int socketFD )
{

	pthread_mutex_lock ( &playersMutex );
	
	std::vector<Player*>::iterator playerItr = PlayerItr ( socketFD );
	Player* playerPtr;
	
	playerItr == players.end() ? playerPtr = NULL : playerPtr = *playerItr;
		
	pthread_mutex_unlock ( &playersMutex );
	
	return playerPtr;
	
}

void								AirHockeyServer::PlayerAdd ( const int playerSocketFD )
{

	/*
	
		Создание игрока
		
	*/
	
	
	
	//	Блокируем мьютекс игроков
	pthread_mutex_lock ( &playersMutex );
	
	//	Создаем игрока и добавляем указатель на него в вектор игроков
	players.push_back ( new Player ( playerSocketFD ) );
	
	//	Отпускаем мьютекс игроков
	pthread_mutex_unlock ( &playersMutex );
	
}

bool								AirHockeyServer::PlayerDirectionSet ( const int playerSocketFD , const PlayerDirection playerDirection )
{

	/*
	
		Выполнение запроса игрока на изменение направления движения
		
	*/
	
	
	
	//	Ищем игрока
	Player* playerPtr = PlayerPtr ( playerSocketFD );
	
	//	Если игрок найден и удалось изменить направление движения игрока
	if ( playerPtr && playerPtr->SetDirection ( playerDirection ) )
	{
	
		//	Направление движения изменено
		return true;
			
	}

	//	Направление движения НЕ изменено
	return false;
		
}

bool								AirHockeyServer::PlayerDelete ( const int socketFD )
{
	
	/*
	
			Удаление игрока
			
	*/
	
	
	
	//	Блокируем мьютекс игроков
	pthread_mutex_lock ( &playersMutex );

	//	Вычисляем итератор игрока по дескриптору его сокета
	std::vector<Player*>::iterator playerItr = PlayerItr ( socketFD );

	//	Если итератор найден
	if ( playerItr != players.end() )
	{

		//	Удаляем динамический объект игрока
		delete *playerItr;
		
		//	Удаляем элемент массива игроков
		players.erase ( playerItr );

		//	Отпускаем мьютекс игроков
		pthread_mutex_unlock ( &playersMutex );
		
		//	Игрок удален
		return true;
		
	}
	
	//	Игрок не найден
	return false;
	
}



/*
			GAME
*/

std::vector<Game*>::iterator		AirHockeyServer::GameItr ( const Player& playerRfr )
{

	std::vector<Game*>::iterator gameItr;
	
	for ( gameItr = games.begin() ; gameItr < games.end(); ++gameItr )
	{
		
		if ( (*gameItr)->HasPlayer ( &playerRfr ) )
		{
		
			return gameItr;

		}
		
	}
	
	return games.end();
	
}

std::vector<Game*>::iterator		AirHockeyServer::GameItr ( const Game* gamePtr )
{

	std::vector<Game*>::iterator gameItr;
	
	for ( gameItr = games.begin() ; gameItr < games.end(); ++gameItr )
	{
		
		if ( (*gameItr) == gamePtr )
		{
		
			return gameItr;

		}
		
	}
	
	return games.end();

}

Game*								AirHockeyServer::GamePtr ( const Player& playerRfr )
{
	pthread_mutex_lock ( &gamesMutex );

	std::vector<Game*>::iterator gameItr = GameItr ( playerRfr );
	Game* gamePtr;
	
	gameItr == games.end() ? gamePtr = NULL : gamePtr = *gameItr;
	
	pthread_mutex_unlock ( &gamesMutex );

	return gamePtr;
	
}

Game*								AirHockeyServer::GameAwaitingEnemy ( void )
{

	/*
	
		Поиск игры, ожидающей присоединения противника
		
	*/
	
	
	
	//	Блокируем мьютекс игр
	pthread_mutex_lock ( &gamesMutex );
	
	//	Перебираем все игры
	for ( std::vector<Game*>::iterator key = this->games.begin() ; key < this->games.end(); ++key )
	{
		
		//	Проверяем статус конкретной игры
		if ( (*key)->GetStatus() == GAME_STATUS_AWAITING_ENEMY )
		{
		
			//	Отпускаем мьютекс игр
			pthread_mutex_unlock ( &gamesMutex );

			//	Игра найдена
			return *key;
			
		}
		
	}
	
	//	Отпускаем мьютекс игр
	pthread_mutex_unlock ( &gamesMutex );
	
	//	Игра НЕ найдена
	return NULL;
	
}

bool 								AirHockeyServer::GameStart ( unsigned int playerSocketFD )
{
	
	/*
	
		Обработка запроса игрока на начало игры
		
	*/
	
	
	
	//	Ищем указатель на игрока по сокету
	Player* playerPtr = PlayerPtr ( playerSocketFD );
	
	//	Если игрок найден и он бездействует
	if ( playerPtr && playerPtr->GetStatus() == PLAYER_STATUS_IDLE )
	{
		
		//	Ищем игру, ожидающую противника
		Game* gameAwaitingEnemyPtr = GameAwaitingEnemy();

		//	Если такая игра найдена
		if ( gameAwaitingEnemyPtr )
		{
			
			//	Присоединяемся к игре
			gameAwaitingEnemyPtr->Join ( *playerPtr );
			
			//	Присоединились к игре
			return true;
			
		}
	   
		//	Если нет игр, ожидающих противника
		else
		{
			
			//	Создаем новую игру
			Game* gamePtr = new Game ( playerPtr );
			
			//	Подписываемся на событие окончания игры
			gamePtr->AddEventListener ( GAME_EVENT_END , this , &AirHockeyServer::OnGameEndHandler );
			
			//	Добавляем игру в массив игр
			pthread_mutex_lock ( &gamesMutex );
			games.push_back ( gamePtr );
			pthread_mutex_unlock ( &gamesMutex );
			
			//	Создали игру
			return true;
			
		}
			
	}
	
	//	Игрок либо не найден, либо уже играет, либо уже ожидает противника
	return false;

}

bool								AirHockeyServer::GameDelete ( Game& gameRfr )
{

	/*
	
		Удаление игры
		
	*/

	
	
	//	Блокируем мьютекс игр
	pthread_mutex_lock ( &gamesMutex );
	
	//	Ищем итератор игры
	std::vector<Game*>::iterator gameItr = GameItr ( &gameRfr );
	
	//	Если игра найдена
	if ( gameItr != games.end() )
	{
	
		//	Удаляем динамически созданный объект Game
		delete *gameItr;
		
		//	Удаляем элемент массива games
		games.erase ( gameItr );

		//	Отпускаем мьютекс игр
		pthread_mutex_unlock ( &gamesMutex );
		
		//	Игра удалена
		return true;
	
	}

	//	Отпускаем мьютекс игр
	pthread_mutex_unlock ( &gamesMutex );
	
	//	Игра не найдена
	return false;
	
}

