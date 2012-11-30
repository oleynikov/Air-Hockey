#pragma once

#include "../o__O/String/String.h"
#include "../o__O/Timer/Timer.h"
#include "../o__O/Events/Events.h"
#include "../o__O/SocketServer/SocketServer.h"
#include "../Player/Player.h"
#include "../Ball/Ball.h"

enum GameStatus
{
	
	GAME_STATUS_AWAITING_ENEMY,
	GAME_STATUS_ACTIVE,
	GAME_STATUS_SCORED,
	GAME_STATUS_BEFORE_ROUND_START_PAUSE,
	GAME_STATUS_PAUSED,
	GAME_STATUS_HOST_LEFT,
	GAME_STATUS_CLIENT_LEFT,
	GAME_STATUS_HOST_WON,
	GAME_STATUS_CLIENT_WON,
	GAME_STATUS_ENDED,
	GAME_STATUS_COUNT
	
};

enum GameEvent
{

	GAME_EVENT_END

};

class Game : public o__O::EventDispatcher
{

	public:
	
						//	Interface
												Game ( Player* playerPtr );
											   ~Game ( void );
						void					Join ( Player& player );
						GameStatus				GetStatus ( void ) const;
						bool					HasPlayer ( const Player* playerPtr ) const;
						bool					SetStatus ( const GameStatus gameStatus );

						//	Event handlers
						void					OnGameStateUpdateTimerHandler ( void* eventData );
						void					OnClockSynchTimerHandler ( void* eventData );
						void					OnPlayerLeaveHandler ( void* eventData );
						void					OnPauseEndHandler ( void* eventData );
						
	private:

		static const	int						GOALS_TO_WIN;
		static const	o__O::Time				UPDATE_TIMEOUT;
		static const	o__O::Time				TIME_SYNC_TIMEOUT;
		static const	o__O::Time				AFTER_GOAL_PAUSE;
		static const	o__O::Time				BEFORE_ROUND_START_PAUSE;

						pthread_mutex_t			masterMutex;
						GameStatus				status;
						char					score[2];
						Player*					host;
						Player*					client;
						Ball					ball;
						o__O::Timer				updateTimer;
						o__O::Timer				timeSyncTimer;
						o__O::Timer				pauseTimer;
						
						void					DispatchUpdate ( void );
						bool					CollisionCheck ( void );
						void					ScoreIncrease ( PlayerRole playerRole );
						void					Pause ( o__O::Time duration );
						void					Reset ( void );
						
};