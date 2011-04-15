#include "Pause.h"
#include "GameManager.h"

namespace Pixy
{
	
	Pause* Pause::mPause;
	
	GAME_STATE Pause::getId() const { return STATE_PAUSE; }
	
	void Pause::enter( void ) {
	}
	
	void Pause::exit( void ) {
	}
	
	void Pause::keyPressed( const OIS::KeyEvent &e )	{
	}
	
	void Pause::keyReleased( const OIS::KeyEvent &e ) {
		switch (e.key) {	 
 			case OIS::KC_ESCAPE:
				this->requestShutdown();
				break;
			case OIS::KC_P:
				  GameManager::getSingleton().popState();
				break;
		}
	}
	
	void Pause::mouseMoved( const OIS::MouseEvent &e )	{
	}
	
	void Pause::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	}
	
	void Pause::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
	}
	
	void Pause::pause( void ) {
	}
	
	void Pause::resume( void ) {
	}
	
	Pause* Pause::getSingletonPtr( void ) {
		if( !mPause )
		    mPause = new Pause();
		
		return mPause;
	}
	
	Pause& Pause::getSingleton( void ) {
		return *getSingletonPtr();
	}
	
	void Pause::update( unsigned long lTimeElapsed ) {
	}
	
	bool Pause::areFxEnabled() { return true; }
  bool Pause::areSfxEnabled() { return true; }
  
  void Pause::dontUpdateMe(Engine* inEngine) { };
} // end of namespace
