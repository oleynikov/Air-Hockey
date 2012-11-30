#pragma once

#include "../o__O/Events/Events.h"
#include "../o__O/Math/Math.h"
#include "../Stage/Stage.h"
#include "../Player/Player.h"
#include <iostream>

enum BallMoveResult
{
	OK,
	HOST_SCORED,
	CLIENT_SCORED
};

enum BallEvent
{
	
	BALL_EVENT_MOVE,
	BALL_EVENT_SCORE
	
};

class Ball : public o__O::EventDispatcher
{

	public:
	
		static const		char						WIDTH;
		static const		char						HEIGHT;
		static const		char						SPEED_X_INITIAL;
		static const		char						SPEED_Y_INITIAL;
		static const		float						POSITION_X_MIN;
		static const		float						POSITION_X_MAX;
		static const		float						POSITION_Y_MIN;
		static const		float						POSITION_Y_MAX;
		static const		float						POSITION_Y_TOP_GOAL;
		static const		float						POSITION_Y_BOTTOM_GOAL;
		
							void						GetPosition ( float* position );
							bool						SetPosition ( const float x = 0 , const float y = 0 );
							
														Ball ( void );
							char						Move ( void );
							void						Stop ( void );
							void						Reset ( void );



							float 						speedX;
							float						speedY;
							float						positionX;
							float						positionY;
		
	private:
	

							
};