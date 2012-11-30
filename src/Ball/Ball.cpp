#include "Ball.h"

using namespace o__O;



//	�������������� ����������� ����� ������
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
	
		����������� ����
		
	*/
	
	
	
	
}

void						Ball::Stop ( )
{

	/*
	
		��������� ����
		
	*/
	
	
	
	speedX = 0;
	speedY = 0;
	
}

void						Ball::Reset ( )
{

	/*
	
		����� ��������� ����
		
	*/
	
	
	
	//	���������� ��� �� ����� �����
	positionX = Stage::WIDTH / 2 - Ball::WIDTH / 2;
	positionY = Stage::HEIGHT / 2 - Ball::HEIGHT / 2;
	
	//	���������� �������� ����
	speedX = Ball::SPEED_X_INITIAL;
	speedY = Ball::SPEED_Y_INITIAL;
	
	//	������������ ��������� ��������
	//	50% ��� �������� ���� ��������� �� ���������������
	Math::CoinFlip() ? speedX *= -1 : NULL;
	Math::CoinFlip() ? speedY *= -1 : NULL;
	
}

char						Ball::Move ( void )
{

	/*
	
		������� ���
		
	*/
	
	
	
	//	������� ��� �� ���������� ��������
	positionX += speedX;
	positionY += speedY;
	
	//	�������� � ������ ������
	if ( positionX > Ball::POSITION_X_MAX )
	{
	
		positionX = Ball::POSITION_X_MAX;
		speedX *= -1;
		
	}
	
	//	�������� � ����� ������
	else if ( positionX < Ball::POSITION_X_MIN )
	{
	
		positionX = Ball::POSITION_X_MIN;
		speedX *= -1;
		
	}
	
}

void						Ball::GetPosition ( float* position )
{

	/*
	
		��������� ������� ����
		
	*/
	
	
	
	position [0] = positionX;
	position [1] = positionY;
	
}

bool						Ball::SetPosition ( const float x , const float y )
{

	/*
	
		��������� ������� ����
		
	*/
	
	
	
	positionX = x;
	positionY = y;
	


}
