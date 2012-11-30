#include "Player.h"

using namespace o__O;



//	Инициализируем статические члены класса
const char						Player::WIDTH = 35;
const char						Player::HEIGHT = 5;
const char						Player::SPEED = 10;
const float						Player::POSITION_X_MIN = Stage::GAP_X;
const float						Player::POSITION_X_MAX = Stage::HEIGHT - Stage::GAP_X - Player::HEIGHT;



								Player::Player ( const int socketFD ) : socketFD ( socketFD ) , status ( PLAYER_STATUS_IDLE )
{

	/*
	
		Конструктор игрока
		
	*/
	
	
	

}

								Player::~Player ( void )
{
	
	/*
	
		Уничтожение игрока
		
	*/
	

	
	//	Сообщаем об уничтожении игрока
	DispatchEvent ( PLAYER_EVENT_LEAVE , this );
	
}

void							Player::Reset()
{

	/*
	
		Возвращение игрока в начальное состояниие
		
	*/
	
	
	
	direction = PLAYER_DIRECTION_NONE;
	position = Stage::WIDTH / 2 - Player::WIDTH / 2;
	
}

bool							Player::Move()
{

	/*
	
		Подвинуть игрока в соответствии с направлением
		
	*/
	
	
	
	//	Если игрок движется
	if ( direction != PLAYER_DIRECTION_NONE )
	{
		//	Если направление "PLAYER_DIRECTION_LEFT"
		if ( direction == PLAYER_DIRECTION_LEFT )
		{
		
			//	Двигаем влево
			position -= Player::SPEED;
			
		}
		
		//	Если направление "PLAYER_DIRECTION_RIGHT"
		else if ( direction == PLAYER_DIRECTION_RIGHT )
		{

			//	Двигаем вправо
			position += Player::SPEED;
			
		}
		
		//	Проверяем выход за рамки
		EscapeCheck();
	
		//	Игрок подвинулся
		return true;
		
	}
	
	//	Игрок неподвижен
	return false;
	
}

void							Player::EscapeCheck()
{

	/*
	
		Проверка выхода за рамки сцены
		
	*/
	
	
	
	if ( position < Player::POSITION_X_MIN )
	{
	
		position = Player::POSITION_X_MIN;
		direction = PLAYER_DIRECTION_NONE;
		
	}
	
	else if ( position > Player::POSITION_X_MAX )
	{
	
		position = Player::POSITION_X_MAX;
		direction =PLAYER_DIRECTION_NONE;
		
	}
	
}

PlayerStatus					Player::GetStatus ( void ) const
{

	/*
	
		Получение статуса игрока
		
	*/
	
	
	
	return status;

}

float							Player::GetPosition ( void ) const
{

	/*
	
		Получение позиции игрока
		
	*/
	
	
	
	return position;

}

bool							Player::SetStatus ( const PlayerStatus playerStatus )
{

	/*
	
		Изменение статуса игрока
		
	*/
	
	
	
	
	//	Если статус указан правильно
	if ( playerStatus >= 0 && playerStatus < PLAYER_STATUS_COUNT )
	{
	
		//	Устанавливаем статус
		status = playerStatus;
	
		//	Статус установлен
		return true;
	
	}
	
	//	Статус указан неправильно
	return false;
	
}

bool							Player::SetPosition ( const unsigned int playerPosition )
{

	/*
	
		Установка позиции игрока
		
	*/
	
	
	//	Проверяем переданное значение абсциссы
	if ( playerPosition >= Player::POSITION_X_MIN && playerPosition <= Player::POSITION_X_MAX )
	{
	
		//	Устанавливаем значение
		position = playerPosition;
		
		//	Значение установлено
		return true;
	
	}
	
	//	Значение указано неправильно
	return false;
	
}

bool							Player::SetDirection ( const PlayerDirection playerDirection )
{

	/*
	
		Изменить направление движения игрока
		
	*/
	

	
	//	Если игрок в игре, активен и направление указано правильно
	if (
			status == PLAYER_STATUS_IN_GAME_ACTIVE
				&&
			playerDirection >= 0
				&&
			playerDirection < PLAYER_DIRECTION_COUNT
		)
	{
		
		//	Устанавливаем направление
		direction = playerDirection;
		return true;
		
	}
	
	//	Направление указано неверно
	return false;
	
}
