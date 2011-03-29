#include "StatePause.h"
#include "GameManager.h"

namespace Pixy
{
	
	StatePause* StatePause::mStatePause;
	
	GAME_STATE StatePause::getId() const { return STATE_PAUSE; }
	
	void StatePause::enter( void ) {
	}
	
	void StatePause::exit( void ) {
	}
	
	void StatePause::keyPressed( const OIS::KeyEvent &e )	{
	}
	
	void StatePause::keyReleased( const OIS::KeyEvent &e ) {
		switch (e.key) {	  
			case OIS::KC_P:
				  GameManager::getSingleton().popState();
				break;
		}
	}
	
	void StatePause::mouseMoved( const OIS::MouseEvent &e )	{
	}
	
	void StatePause::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	}
	
	void StatePause::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	}
	
	void StatePause::pause( void ) {
	}
	
	void StatePause::resume( void ) {
	}
	
	StatePause* StatePause::getSingletonPtr( void ) {
		if( !mStatePause )
		    mStatePause = new StatePause();
		
		return mStatePause;
	}
	
	StatePause& StatePause::getSingleton( void ) {
		return *getSingletonPtr();
	}
	
	void StatePause::update( unsigned long lTimeElapsed ) {
	}
	
} // end of namespace
