#include "GameState.h"

namespace Pixy
{
	/*CEGUI::MouseButton GameState::convertButton(OIS::MouseButtonID buttonID)
	{
		switch (buttonID)
		{
			case OIS::MB_Left:
				return CEGUI::LeftButton;
				
			case OIS::MB_Right:
				return CEGUI::RightButton;
				
			case OIS::MB_Middle:
				return CEGUI::MiddleButton;
				
			default:
				return CEGUI::LeftButton;
		}
	};*/

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
	
} // end of namespace Pixy
