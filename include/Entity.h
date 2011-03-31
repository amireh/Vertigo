
/* -----------------------------------------------
 *  Filename: Entity.h
 *  Date Created: ??/2009
 *
 *  Original Author:
 *      Ahmad Amireh ( ahmad.amireh@gmail.com )
 *
 *  Last Update:
 *      Date:   20/4/2009
 *      By:     Ahmad Amireh
 * ----------------------------------------------- */

#ifndef H_Entity_H
#define H_Entity_H

#ifndef H_Pixy_H
#include "Pixy.h"
#endif

#include <Ogre.h>
#include <btBulletDynamicsCommon.h>
#include "MotionState.h"

#define BIT(x) (1<<(x))

using Ogre::Vector3;
using Ogre::Real;
namespace Pixy
{
	
  enum collisiontypes {
    COL_NOTHING = 0, // collide with nothing (for our terrain)
    COL_SPHERE = BIT(0), // collide with the sphere
    COL_OBSTACLES = BIT(1), // collide with obstacles
    COL_WALLS = BIT(2) // collide with walls
  };
  
  typedef enum {
    FIRE,
    ICE
  } SHIELD;

  typedef enum {
    SPHERE,
    OBSTACLE
  } ENTITY_TYPE;
	
	/*! \class Entity Entity.h "src/Entity.h"
	 *
	 * \brief Defines base attributes and actions for GameObjects.
	 *
	 * \note Please note that for the creation of GameObjects 
	 * refer to their respective Factories, do NOT use this
	 * directly.
	 */
	class Entity/* : public btCollisionObject*/
	{
		
	public:
		/*! Default ctor.
		 *!  Initializes all attributes with default settings.
		 */
		Entity();
		Entity& operator=(const Entity& rhs);
		Entity(const Entity& src);
		virtual ~Entity();

		virtual void live();
		virtual void die();

		//! Sets the id of the entity.
		/*!
		 * @param inIdEntity Entity id
		 */
		void setObjectId(const int inIdEntity);
		
		//! Sets entity in-game name.
		/*!
		 * @param inName Entity name to set.
		 */
		void setName(const std::string inName);
				
		//! Sets entity health points.
		/*!
		 * @param inHealth Entity health stat to set.
		 */
		void setHP(const int inHealth);

		
		/*!
		 * @return Entity's id.
		 */
		int getObjectId() const;
		
		
		virtual void update(unsigned long lTimeElapsed)=0;
		
		/*!
		 * @return Entity's in-game name.
		 */
		std::string& getName();
		
		
		/*!
		 * @return Entity's current health points.
		 */
		int getHP() const;
		
		bool dead();
		
				
		//! Prints out information about this Entity to Log ofstream; used for debugging
		virtual void printInfo();
		
		virtual Ogre::SceneNode* getMasterNode();
		
		virtual void attachSceneNode(Ogre::SceneNode* inNode);
		virtual Ogre::SceneNode* getSceneNode();
		
		virtual void attachSceneObject(Ogre::MovableObject* inObject);
		virtual Ogre::MovableObject* getSceneObject();
		
		virtual void setMesh(std::string inMesh);
		virtual std::string getMesh();
		
		Vector3& getDirection();
		Real getMoveSpeed();
		void setMoveSpeed(const Real inSpeed);
		
		//virtual btCollisionObject* getObject();
		virtual btRigidBody* getRigidBody();
		virtual MotionState* getMotionState();
		virtual btCollisionShape* getCollisionShape();
		
		ENTITY_TYPE type();
		
		virtual void collide(Entity* obj);
		
	protected:

    static long idCounter;
		int idObject;                       //! Unique entity id
		std::string mName;					//! Entity's in-game name
		int mHP;                            //! Entity's health stat
		bool fDead;
		
		bool fHasSfx; // toggler for sound effects
		bool fHasFx; // toggler for visual effects
		
		ENTITY_TYPE mType;
		std::string mMesh;					//! Entity's in-game name		
		Ogre::SceneNode         *mSceneNode, *mMasterNode;
		Ogre::MovableObject     *mSceneObject;
    Ogre::ParticleSystem* mFireTrail;
    Ogre::ParticleSystem* mIceSteam;
		btCollisionShape		*mPhyxShape;
		//btCollisionObject   *mObject;
		MotionState				*mPhyxMS;
		btRigidBody				*mPhyxBody;
		
		Real mMoveSpeed;
		Vector3 mDirection;
		Vector3 mPosition;
		
		//! helper method for copy/assignment methods
		//! copies all data from src and sets it into this entity
		virtual void copyFrom(const Entity& src);

    static int sphereCollidesWith;
    static int obstacleCollidesWith;
		
		log4cpp::Category* mLog;
	}; // end of Entity class
} // end of Pixy namespace
#endif
