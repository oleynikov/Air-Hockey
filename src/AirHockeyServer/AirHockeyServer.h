#pragma once

#include <stdlib.h>												//	atoi
#include "../o__O/Events/Events.h"
#include "../o__O/SocketServer/SocketServer.h"
#include "../o__O/String/String.h"
#include "../Player/Player.h"
#include "../Game/Game.h"

class AirHockeyServer
{

	private:
	
						//	Members
						std::vector<Player*>					players;
						std::vector<Game*>						games;
						pthread_mutex_t							playersMutex;
						pthread_mutex_t							gamesMutex;

						//	Player methods
						std::vector<Player*>::iterator			PlayerItr ( const int playerSocketFD );
						Player*									PlayerPtr ( const int playerSocketFD );
						void									PlayerAdd ( const int playerSocketFD );
						bool									PlayerDirectionSet ( const int playerSocketFD , const PlayerDirection playerDirection );
						bool									PlayerDelete ( const int playerSocketFD );
						
						//	Game methods
						std::vector<Game*>::iterator			GameItr ( const Player& playerRfr );
						std::vector<Game*>::iterator			GameItr ( const Game* gamePtr );
						Game*									GamePtr ( const Player& playerRfr );
						Game*									GameAwaitingEnemy ( void );
						bool									GameStart ( unsigned int playerSocketFD );
						bool									GameDelete ( Game& gameRfr );
				
	public:
	
						//	Constructor
																AirHockeyServer ( void );
																
						//	Event handlers
						void									OnClientConnectHandler ( void* data );
						void									OnClientDisconnectHandler ( void* data );
						void									OnClientRequestHandler ( void* data );
						void									OnGameEndHandler ( void* data );
	
};