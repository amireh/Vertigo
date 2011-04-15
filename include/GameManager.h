/* -----------------------------------------------
 *  Filename: GameManager.h
 *  Date Created: ??/2009
 *
 *  Original Author:
 *      IMPORTANT : GET AUTHOR NAME FROM OGRE WIKI
 *
 *  Last Update:
 *      Date:   20/4/2009
 *      By:     Ahmad Amireh
 * ----------------------------------------------- */

#ifndef H_GameManager_H
#define H_GameManager_H

#include "Pixy.h"

// OGRE
#include <Ogre.h>
#include <OgreConfigFile.h>
#include <OgreWindowEventUtilities.h>
#include <OgreException.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
#include "OSX/macUtils.h"
#endif

#include "InputManager.h"

#include "log4cpp/Category.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/SimpleLayout.hh"

#include "map"

namespace Pixy
{
  typedef std::map<std::string, std::string> tPixySettings;
	class GameState;
	/*! \class GameManager
	 *  \brief
	 *  Runs and handles the flow between Pixy::GameState s, 
	 *	and acts as an input listener and dispatcher. 
	 */
	class GameManager : public OIS::KeyListener, OIS::MouseListener ,  public Ogre::WindowEventListener {
	public:
		~GameManager( void );
		
		//! Loads StateGame game state to start the game.
		/*! 
		 *	
		 */
		void startGame();
		
		//! Shuts down current game state and loads another.
		/*! 
		 *	@param
		 *	inState		Instance of GameState to be loaded.
		 */
		void changeState(GameState *inState);
		
		//! Loads a game state while another is running, which gets paused.
		/*! 
		 *	@param
		 *	inState		Instance of GameState to be loaded.
		 *
		 *	\note
		 *	Current running state must have a functional implementation
		 *	of GameState::pause() for this context switching to work properly.
		 */
		void pushState(GameState *inState);
		
		//! Reloads an earlier running GameState and shuts down the current one.
		/*! 
		 *	\note
		 *	If the stack contains only one state, the system will shutdown
		 *	upon calling this method.
		 */
		void popState();
		
		//!	Shuts down the system, consequently shutting down all running game states.
		/*! 
		 *	\note
		 *	
		 */
		void requestShutdown();
		
		//! Retrieves a pointer to the Singleton instance of this class.
		/*!
		 *	@return
		 *	Pointer to the instance of this Singleton class.
		 */
		static GameManager* getSingletonPtr();
		static GameManager& getSingleton();
		
		GameState* currentState();
		
		tPixySettings& getSettings();
		void applyNewSettings(tPixySettings& inSettings);
		
		bool shuttingDown();
	private:
		GameManager();
		GameManager(const GameManager&) {}
		GameManager& operator=(const GameManager&);
		
		//! Prepares Ogre for use by the game components
		/*! 
		 *	\note
		 *	This method is called internally within startGame().
		 *
		 *	Ogre Root, RenderWindow, Resource Groups, and SceneManager
		 *	are set up here.
		 */
		bool configureGame();
		
		void loadRenderSystems();
		
		//! Loads resources for use by the Ogre engine
		/*! 
		 *	\note
		 *	This method is called internally within startGame().
		 *
		 *	Parses configuration scripts and re/stores settings.
		 */
		void setupResources(std::string inResourcesPath);
		
		//! OIS key input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		bool keyPressed( const OIS::KeyEvent &e );

		//! OIS key input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */
		bool keyReleased( const OIS::KeyEvent &e );
		
		//! OIS mouse input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */	
		bool mouseMoved( const OIS::MouseEvent &e );

		//! OIS mouse input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */	
		bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );

		//! OIS mouse input event handler/dispatcher method
		/*! 
		 *	\note
		 *	Events received here are dispatched to the
		 *	current running GameState for processing.
		 */		
		bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		
		/*! Starts off the logger
		 *
		 */
		void initLogger();
		
		void loadConfig();
		void saveConfig();
		
		Ogre::Root			*mRoot;
		Ogre::RenderWindow	*mRenderWindow;
		InputManager		*mInputMgr;
		
		GameState* mCurrentState;
		unsigned long lTimeLastFrame, lTimeCurrentFrame, lTimeSinceLastFrame;
		
		//! do we want to shutdown?
		bool fShutdown;
		std::vector<GameState*> mStates;
		static GameManager *mGameManager;
		log4cpp::Category* mLog;
		
		tPixySettings mSettings;
		
		std::string mConfigPath;
	};
} // end of namespace

#endif
