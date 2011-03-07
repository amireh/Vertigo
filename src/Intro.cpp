#include "Intro.h"
#include "EventManager.h"
#include "NetworkManager.h"
#include "Puppet.h"
#include "Deck.h"
#include "Spell.h"

using namespace Ogre;
namespace Pixy
{
	
	Intro* Intro::mIntro;
	
	GAME_STATE Intro::getId() const { return STATE_INTRO; }
	
	void Intro::enter( void ) {
		
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Intro");
		
		// init engines
		mGfxEngine = GfxEngine::getSingletonPtr();
		mGfxEngine->setup();
				
		mUIEngine = UIEngine::getSingletonPtr();
		mUIEngine->setup();
			
		// grab CEGUI handle
		mUISystem = &CEGUI::System::getSingleton();
 
		mLog->infoStream() << "Initialized successfully.";
		
	}

	
	void Intro::exit( void ) {
				
		delete mUIEngine;
		delete mGfxEngine;
		
		mLog->infoStream() << "---- Exiting Intro State ----";
		delete mLog;
		mLog = 0;
		 
	}
	
	void Intro::update( unsigned long lTimeElapsed ) {
		
		mGfxEngine->update(lTimeElapsed);
		mUIEngine->update(lTimeElapsed);
		
	}
	
	void Intro::keyPressed( const OIS::KeyEvent &e )
	{
	
		mUISystem->injectKeyDown(e.key);
		mUISystem->injectChar(e.text);
		
	}
	
	void Intro::keyReleased( const OIS::KeyEvent &e ) {
		
		mUISystem->injectKeyUp(e.key);
		
		switch (e.key) {
			case OIS::KC_ESCAPE:
				this->requestShutdown();
				break;
			case OIS::KC_SPACE:
				//fireLoginEvt();
				break;
		}
		
	}
	
	void Intro::mouseMoved( const OIS::MouseEvent &e )
	{
		// Update CEGUI with the mouse motion
		mUISystem->injectMouseMove(e.state.X.rel, e.state.Y.rel);
	}
	
	void Intro::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		mUISystem->injectMouseButtonDown(convertButton(id));
	}
	
	void Intro::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		mUISystem->injectMouseButtonUp(convertButton(id));
	}
	
	void Intro::pause( void ) {
	}
	
	void Intro::resume( void ) {
	}
		
	Intro* Intro::getSingletonPtr( void ) {
		if( !mIntro )
		    mIntro = new Intro();
		
		return mIntro;
	}
	
	Intro& Intro::getSingleton( void ) {
		return *getSingletonPtr();
	}

} // end of namespace
