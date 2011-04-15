/* -----------------------------------------------
 *  Filename: GameState.h
 *  Date Created: ??/2009
 *
 *  Original Author:
 *      IMPORTANT : GET AUTHOR NAME FROM OGRE WIKI
 *
 *  Last Update:
 *      Date:   20/4/2009
 *      By:     Ahmad Amireh
 * ----------------------------------------------- */

#ifndef H_GameState_H
#define H_GameState_H

#ifndef H_GameManager_H
#include "GameManager.h"
#endif
#ifndef H_InputManager_H
#include "InputManager.h"
#endif

namespace Pixy
{
    /*! \class GameState
     *  \brief
     *  Defines general behaviour of game states such as the Main Menu, and Combat.
     */
  class Engine;
	class GameState {
		
	public:
		~GameState(){ };
		
		virtual void enter( void )  = 0;
		virtual void exit( void )   = 0;
		
		virtual void pause( void )  = 0;
		virtual void resume( void ) = 0;
		virtual void update( unsigned long lTimeElapsed ) = 0;
		
		virtual void keyPressed( const OIS::KeyEvent &e ) = 0;
		virtual void keyReleased( const OIS::KeyEvent &e ) = 0;
		
		virtual void mouseMoved( const OIS::MouseEvent &e ) = 0;
		virtual void mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id )  = 0;
		virtual void mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) = 0;
		
		void changeState( GameState *state );
		void pushState( GameState *state );
		void popState( void );
		void requestShutdown( void );
		
    virtual bool areFxEnabled()=0;
    virtual bool areSfxEnabled()=0;
		virtual GAME_STATE getId() const=0;
		virtual void dontUpdateMe(Engine* inEngine)=0;
		virtual bool running();
	protected:
		GameState(){ };
		log4cpp::Category* mLog;
		bool fRunning;
	private:
		
		GameState(const GameState&) { fRunning = false; mLog = 0; }
		GameState& operator=(const GameState&);
    };
} // end of namespace
#endif
