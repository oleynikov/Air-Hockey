#include "Game.h"

using namespace o__O;



//	Инициализируем статические члены класса
const int						Game::GOALS_TO_WIN = 1;
const Time						Game::UPDATE_TIMEOUT = Time ( 0 , 200000000 );
const Time						Game::TIME_SYNC_TIMEOUT = Time ( 1 , 0 );
const Time						Game::AFTER_GOAL_PAUSE = Time ( 1 , 0 );
const Time						Game::BEFORE_ROUND_START_PAUSE = Time ( 2 , 0 );



								Game::Game ( Player* playerPtr )
{

	//	Инициализируем мьютекс игры
	pthread_mutex_init ( &masterMutex , NULL );
	
	//	Сохраняем указатель на хоста игры и подписываемся на его лив
	host = playerPtr;
	host->AddEventListener ( PLAYER_EVENT_LEAVE , this , &Game::OnPlayerLeaveHandler );

	//	Ставим заглушки на клиента
	client = NULL;

	//	Изменяем статус игры на `ОЖИДАЕТ_ПРОТИВНИКА`
	SetStatus ( GAME_STATUS_AWAITING_ENEMY );

	//	Изменяем статус хоста на `ОЖИДАЕТ_ПРОТИВНИКА`
	host->SetStatus ( PLAYER_STATUS_AWAITING_ENEMY );
	
	//	Извещаем клиента о создании игры
	SocketServer::Send ( host->socketFD , "0" );
	
}

								Game::~Game ( void )
{
	
	/*
	
		Деструктор игры
		
	*/
	
	
	
	//	Удаляем подписку у хоста
	host->RemoveEventListener ( PLAYER_EVENT_LEAVE , this , &Game::OnPlayerLeaveHandler );
	
	//	Если есть клиент
	if ( client )
	{

		//	Удаляем подписку у клиента
		client->RemoveEventListener ( PLAYER_EVENT_LEAVE , this , &Game::OnPlayerLeaveHandler );

	}

}

void							Game::Join ( Player& player )
{

	/*
	
		Клиент присоединился к созданной игре
		
	*/
	
	
	
	//	Сохраняем указатель на игрока-клиента и подписываемся на его выход из игры
	client = &player;
	client->AddEventListener ( PLAYER_EVENT_LEAVE , this , &Game::OnPlayerLeaveHandler );

	//	Инициализируем таймер синхронизации времени, подписываемся на него
	timeSyncTimer = Timer ( Game::TIME_SYNC_TIMEOUT );
	timeSyncTimer.AddEventListener ( TIMER_EVENT_TICK , this , &Game::OnClockSynchTimerHandler );

	//	Инициализируем таймер обновления игрового состояния и подписываемся на него
	updateTimer = Timer ( Game::UPDATE_TIMEOUT );
	updateTimer.AddEventListener ( TIMER_EVENT_TICK , this , &Game::OnGameStateUpdateTimerHandler );

	//	Обнуляем счет
	score[0] = 0;
	score[1] = 0;
	
	//	Перезагружаем игру
	Reset();
	
	//	Сообщаем игрокам, что игра началась
	SocketServer::Send ( host->socketFD , "1" );
	SocketServer::Send ( client->socketFD , "1" );
	
}

void							Game::DispatchUpdate ( void )
{

	/*
	
		Рассылка обновленного состояния игры
		
	*/
	
	
	
	//	Позиции игроков
	std::string hostX = String::FTS ( host->GetPosition() );
	std::string clientX = String::FTS ( client->GetPosition() );
	
	//	Позиция шара
	std::string ballX = String::FTS ( ball.positionX );
	std::string ballY = String::FTS ( ball.positionY );
	
	//	Время обновления
	Time timeNextUpdate = Time::Now() + Game::UPDATE_TIMEOUT;
	std::string timeFinish = String::ITS( timeNextUpdate.seconds ) + ":" + String::ITS( timeNextUpdate.nseconds );
	
	//	Текст запроса на обновление
	std::string hostUpdateData = "4/" + hostX + ":" + clientX + ":" + ballX + ":" + ballY + ":" + timeFinish;
	std::string clientUpdateData = "4/" + clientX + ":" + hostX + ":" + ballX + ":" + ballY + ":" + timeFinish;
	
	//	Отправляем обновление
	SocketServer::Send ( host->socketFD , hostUpdateData );
	SocketServer::Send ( client->socketFD , clientUpdateData );
	
}

bool							Game::CollisionCheck ( void )
{

	/*
	
		Провера коллизий
		
	*/
	
	
	
	//	Позиция шара
	float ballPosition[2];
	ball.GetPosition ( ballPosition );
	
	float ballPositionX = ballPosition[0];
	float ballPositionY = ballPosition[1];
	float hostPosition = host->GetPosition();
	float clientPosition = Stage::WIDTH - Player::WIDTH - client->GetPosition();
	
	
	//	Коллизии шара с хостом
	if	(
			( ballPositionX + Ball::WIDTH >= hostPosition )		//	Шар правее левого края платформы
				&&
			( ballPositionX <= hostPosition + Player::WIDTH )	//	Шар левее правого края платформы
				&&
			( ballPositionY >= Ball::POSITION_Y_MAX )			//	Шар ниже платформы
				&&
			( ball.speedY > 0 )									//	Шар движется вниз
		)
	{
	
		ball.positionY = Ball::POSITION_Y_MAX;
		ball.speedY *= -1;
	
	}

	// Коллизия шара с клиентом
	if	(
			( ballPositionX + Ball::WIDTH >= clientPosition )
				&&
			( ballPositionX <= clientPosition + Player::WIDTH )
				&&
			( ball.positionY <= Ball::POSITION_Y_MIN )
				&&
			( ball.speedY < 0 )
		)
	{
	
		ball.positionY = Ball::POSITION_Y_MIN;
		ball.speedY *= -1;
	
	}
	
	//	Проверка на гол
	if ( ball.positionY >= Ball::POSITION_Y_BOTTOM_GOAL )
	{
	
		//	Хост забил гол
		ScoreIncrease ( PLAYER_ROLE_CLIENT );
		return false;
		
	}
	
	else if ( ball.positionY <= Ball::POSITION_Y_TOP_GOAL )
	{

		//	Клиент забил гол
		ScoreIncrease ( PLAYER_ROLE_HOST );
		return false;
	
	}
	
	//	Гол не был забит
	return true;
	
}

void							Game::ScoreIncrease ( PlayerRole playerRole )
{

	/*
	
		Увеличение счета
		
	*/
	
	
	
	//	Ставим статус игры
	SetStatus ( GAME_STATUS_SCORED );
	
	//	Увеличиваем счет
	score[playerRole] += 1;
	
	
	//	Сообщение об изменении счета
	std::string scoreUpdateRequest = "5/" + String::ITS ( score[0] ) + ":" + String::ITS ( score[1] );
	
	//	Извещаем игроков об изменении счета
	SocketServer::Send ( host->socketFD , scoreUpdateRequest );
	SocketServer::Send ( client->socketFD , scoreUpdateRequest );
	
	//	Ставим на паузу
	Pause ( Game::AFTER_GOAL_PAUSE );

}

void							Game::Pause ( Time duration )
{

	/*
	
		Поставить игру на паузу на duration времени
		
	*/
	
	
	
	//	Ставим статусы игроков
	host->SetStatus ( PLAYER_STATUS_IN_GAME_PAUSED );
	client->SetStatus ( PLAYER_STATUS_IN_GAME_PAUSED );
	
	//	Останавливаем таймеры обновления
	updateTimer.Stop();
	timeSyncTimer.Stop();
	
	//	Запускаем статус паузы и подписываемся на окончание паузы
	pauseTimer = Timer ( Game::AFTER_GOAL_PAUSE , 1 );
	pauseTimer.AddEventListener ( TIMER_EVENT_COMPLETE , this , &Game::OnPauseEndHandler );
	pauseTimer.Start();

}

void							Game::Reset ( void )
{

	/*
	
		Перезагрузка игры
		
	*/
	
	
	
	//	Меняем статус игры на "GAME_STATUS_ACTIVE"
	SetStatus ( GAME_STATUS_ACTIVE );

	//	Меняем статусы игроков на "PLAYER_STATUS_IN_GAME_ACTIVE" и перезагружаем их
	host->SetStatus ( PLAYER_STATUS_IN_GAME_ACTIVE );
	host->Reset();
	
	client->SetStatus ( PLAYER_STATUS_IN_GAME_ACTIVE );
	client->Reset();
	
	//	Сбрасываем состояние шара
	ball.Reset();

	//	Запускаем таймеры обновления
	timeSyncTimer.Start ( true );
	updateTimer.Start( true );

}

void							Game::OnGameStateUpdateTimerHandler ( void* data )
{

	/*
		
		Обработчик события таймера обновления состояния игры
		
	*/
	
	
	
	//	Если игра активна
	if ( status == GAME_STATUS_ACTIVE )
	{
	
		//	Двигаем шар
		ball.Move();

		//	Двигаем игроков
		host->Move();
		client->Move();
		
		if ( /*CollisionCheck()*/ 1 )
		{
		
			//	Рассылаем обновление
			DispatchUpdate();
			
		}
		
	}

}

void							Game::OnClockSynchTimerHandler ( void* eventData )
{

	/*
	
		Обработчик таймера сихронизации времени
		
	*/
	
	
	
	//	Текущее время сервера
	Time timeCurrent = Time::Now();
	std::string secondsCurrent = String::ITS( timeCurrent.seconds );
	std::string nSecondsCurrent = String::ITS( timeCurrent.nseconds );
	
	//	Формируем текст запроса
	std::string timeUpdateRequest = "3/" + secondsCurrent + ":" + nSecondsCurrent;
	
	//	Отправляем игрокам обновленное время
	SocketServer::Send ( host->socketFD , timeUpdateRequest );
	SocketServer::Send ( client->socketFD , timeUpdateRequest );
	
}

void							Game::OnPlayerLeaveHandler ( void* data )
{

	/*
	
		Обработчик события выхода игрока
		
	*/

	
	
	//	Кастуем указатель на игрока
	Player* playerPtr = static_cast <Player*> ( data );
	
	//	Если игрок принадлежит этой игре
	if ( HasPlayer ( playerPtr ) )
	{
	
		//	Если игрок - хост
		if ( playerPtr == host )
		{
		
			//	Если есть клиент
			if ( client )
			{

				//	Извещаем клиента об окончании игры и ставим статус бездействия
				o__O::SocketServer::Send ( client->socketFD , "2/0" );
				client->SetStatus ( PLAYER_STATUS_IDLE );
				
			}
		
			//	Устанавливаем стус игры
			SetStatus ( GAME_STATUS_HOST_LEFT );
		
		}
		
		//	Если игрок - клиент
		else
		{
			
			//	Извещаем хоста об окончании игры и ставим статус бездействия
			o__O::SocketServer::Send ( host->socketFD , "2/0" );
			host->SetStatus ( PLAYER_STATUS_IDLE );
			
			//	Устанавливаем стус игры
			SetStatus ( GAME_STATUS_CLIENT_LEFT );
			
		}
		
		//	Сообщаем об окончании игры
		DispatchEvent ( GAME_EVENT_END , this );

	}

}

void							Game::OnPauseEndHandler ( void* eventData )
{

	/*
	
		Пауза кончилась
		
	*/
	
	
	
	//	Смотрим статус игры
	GameStatus gameStatus = GetStatus();
	
	//	Если кончилась пауза после забитого гола
	if ( gameStatus == GAME_STATUS_SCORED )
	{
	
		//	Если счет игроков не превышает максимального
		if ( score[0] < Game::GOALS_TO_WIN && score[1] < Game::GOALS_TO_WIN )
		{

			//	Ставим игру на паузу перед началом нового раунда
			SetStatus ( GAME_STATUS_BEFORE_ROUND_START_PAUSE );
			Pause ( Game::BEFORE_ROUND_START_PAUSE );
			
		}
		
		//	Игра окончена по счету
		else
		{
		
			//	Сообщение об окончании игры по очкам
			std::string gameEndRequest = "2/1";
			
			//	Меняем статусы игроков на "PLAYER_STATUS_IDLE" и перезагружаем их
			host->SetStatus ( PLAYER_STATUS_IDLE );
			client->SetStatus ( PLAYER_STATUS_IDLE );

			//	Извещаем игроков об окончании игры
			SocketServer::Send ( host->socketFD , gameEndRequest );
			SocketServer::Send ( client->socketFD , gameEndRequest );

			//	Сообщаем об окончании игры
			DispatchEvent ( GAME_EVENT_END , this );
			
		}
		
	}
	
	//	Если кончилась пауза перед началом раунда
	else if ( gameStatus == GAME_STATUS_BEFORE_ROUND_START_PAUSE )
	{
	
		//	Начинаем раунд
		Reset();
		
	}

}

GameStatus						Game::GetStatus ( void ) const
{

	return status;
	
}

bool							Game::SetStatus ( const GameStatus gameStatus )
{

	/*
	
		Установка статуса игры
		
	*/
	
	
	
	
	//	Если статус указан правильно
	if ( gameStatus >= 0 && gameStatus < GAME_STATUS_COUNT )
	{
	
		//	Устанавливаем статус
		status = gameStatus;
	
		//	Статус установлен
		return true;
	
	}
	
	//	Статус указан неправильно
	return false;

}

bool							Game::HasPlayer ( const Player* playerPtr ) const
{

	/*
	
		Участвует ли игрок в этой игре?
		
	*/
	
	
	
	if ( host->socketFD == playerPtr->socketFD || client->socketFD == playerPtr->socketFD )
	{
		
		return true;
		
	}
	
	return false;

}
