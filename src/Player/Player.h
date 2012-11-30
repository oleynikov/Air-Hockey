#pragma once

#include "../o__O/Events/Events.h"
#include "../Stage/Stage.h"
#include <iostream>

enum PlayerRole
{

	PLAYER_ROLE_HOST,
	PLAYER_ROLE_CLIENT
	
};

enum PlayerStatus
{
	
	PLAYER_STATUS_IDLE,
	PLAYER_STATUS_AWAITING_ENEMY,
	PLAYER_STATUS_IN_GAME_ACTIVE,
	PLAYER_STATUS_IN_GAME_PAUSED,
	PLAYER_STATUS_COUNT
	
};

enum PlayerDirection
{
	
	PLAYER_DIRECTION_NONE,
	PLAYER_DIRECTION_RIGHT,
	PLAYER_DIRECTION_LEFT,
	PLAYER_DIRECTION_COUNT
	
};

enum PlayerEvent
{

	PLAYER_EVENT_LEAVE,
	PLAYER_EVENT_STATUS_CHANGE

};

class Player : public o__O::EventDispatcher
{
	
	public:

		static const	char					WIDTH;
		static const	char					HEIGHT;
		static const	char					SPEED;
		static const	float					POSITION_X_MIN;
		static const	float					POSITION_X_MAX;
		
						int						socketFD;
						
						//	Interface
												Player ( const int socketFD );
											   ~Player ( void );
						void					Reset();
						bool					Move();
						PlayerStatus			GetStatus ( void ) const;
						float					GetPosition ( void ) const;
						bool					SetDirection ( const PlayerDirection playerDirection );
						bool					SetStatus ( const PlayerStatus playerStatus );
						bool					SetPosition ( const unsigned int playerPosition );
		
	private:
	
						//	Private properties
						PlayerStatus			status;
						PlayerDirection			direction;
						float					position;
						
						//	Private methods
						void					EscapeCheck();
						
};