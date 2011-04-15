#include "GameState.h"

namespace Pixy
{

	void GameState::changeState( GameState *state ) {
		GameManager::getSingletonPtr()->changeState( state );
	}

	void GameState::pushState( GameState *state ) {
		GameManager::getSingletonPtr()->pushState( state );
	}

	void GameState::popState( void ) {
		GameManager::getSingletonPtr()->popState();
	}

	void GameState::requestShutdown( void ) {
		GameManager::getSingletonPtr()->requestShutdown();
	}
	
	bool GameState::running() { return fRunning; };
	
} // end of namespace Pixy
