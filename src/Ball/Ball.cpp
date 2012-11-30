#include "Ball.h"

using namespace o__O;



//	Инициализируем статические члены класса
const char					Ball::WIDTH = 5;
const char					Ball::HEIGHT = 5;
const char					Ball::SPEED_X_INITIAL = 3;
const char					Ball::SPEED_Y_INITIAL = 6;
const float					Ball::POSITION_X_MIN = 0;
const float					Ball::POSITION_X_MAX = Stage::WIDTH - Ball::WIDTH;
const float					Ball::POSITION_Y_MIN = Stage::GAP_Y + Player::HEIGHT;
const float					Ball::POSITION_Y_MAX = Stage::HEIGHT - Stage::GAP_Y - Player::HEIGHT - Ball::HEIGHT;
const float					Ball::POSITION_Y_TOP_GOAL = Ball::HEIGHT * ( -1 );
const float					Ball::POSITION_Y_BOTTOM_GOAL = Stage::HEIGHT;


							Ball::Ball ( )
{

	/*
	
		Конструктор шара
		
	*/
	
	
	
	
}

void						Ball::Stop ( )
{

	/*
	
		Остановка шара
		
	*/
	
	
	
	speedX = 0;
	speedY = 0;
	
}

void						Ball::Reset ( )
{

	/*
	
		Сброс состояния шара
		
	*/
	
	
	
	//	Возвращаем шар на центр сцены
	positionX = Stage::WIDTH / 2 - Ball::WIDTH / 2;
	positionY = Stage::HEIGHT / 2 - Ball::HEIGHT / 2;
	
	//	Сбрасываем скорость шара
	speedX = Ball::SPEED_X_INITIAL;
	speedY = Ball::SPEED_Y_INITIAL;
	
	//	рандомизация начальной скорости
	//	50% что скорости шара изменятся на противоположные
	Math::CoinFlip() ? speedX *= -1 : NULL;
	Math::CoinFlip() ? speedY *= -1 : NULL;
	
}

char						Ball::Move ( void )
{

	/*
	
		Двинуть шар
		
	*/
	
	
	
	//	Двигаем шар на расстояние скорости
	positionX += speedX;
	positionY += speedY;
	
	//	Коллизия с правой стеной
	if ( positionX > Ball::POSITION_X_MAX )
	{
	
		positionX = Ball::POSITION_X_MAX;
		speedX *= -1;
		
	}
	
	//	Коллизия с левой стеной
	else if ( positionX < Ball::POSITION_X_MIN )
	{
	
		positionX = Ball::POSITION_X_MIN;
		speedX *= -1;
		
	}
	
}

void						Ball::GetPosition ( float* position )
{

	/*
	
		Получение позиции шара
		
	*/
	
	
	
	position [0] = positionX;
	position [1] = positionY;
	
}

bool						Ball::SetPosition ( const float x , const float y )
{

	/*
	
		Установка позиции шара
		
	*/
	
	
	
	positionX = x;
	positionY = y;
	


}
