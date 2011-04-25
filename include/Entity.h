/*
 *  This file is part of Vertigo.
 *
 *  Vertigo - a cross-platform arcade game powered by Ogre3D.
 *  Copyright (C) 2011  Ahmad Amireh <ahmad@amireh.net>
 * 
 *  Vertigo is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Vertigo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Vertigo.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
  
  typedef enum {
    FIRE,
    ICE
  } SHIELD;
	
	/*! \class Entity Entity.h "src/Entity.h"
	 *
	 * \brief 
	 *  Defines base attributes and actions for Entities.
	 */
	class Entity
	{
		
	public:
	
		/*! Default ctor.
		 *  Initializes all attributes with default settings and allocates
		 *  any resources that this entity will need.
		 *
		 *  \remark
		 *  Some resources such as particle systems and sound effects will be
		 *  allocated and initialised here. However, their state remains idle
		 *  until the Entity becomes alive with live().
		 */
		Entity();
		Entity& operator=(const Entity& rhs);
		Entity(const Entity& src);
		
		/*! \brief
		 *  Cleans up any allocated resources.
		 */
		virtual ~Entity();

    /*! \brief
     *  Fires up the Entity's physical and graphical components and changes
     *  state to alive.
     */
		virtual void live()=0;
		/*! \brief
		 *  Possibly unplugs the Entity's physical and renderable components.
		 *  Behaviour can differ between children.
		 */
		virtual void die()=0;
		
		/*! \brief
		 *  Returns the state of this Entity.
		 */
		inline bool dead() const {
		  return fDead;
		};
				
		virtual void update(unsigned long lTimeElapsed)=0;
		
		/*! \brief
		 *  Defines how the Entity reacts when it collides with another.
		 */
		virtual void collide(Entity* obj);
		
		/*! \brief
		 *  Entity names are not unique and are no identifiers, mainly used for debugging.
     *
		 * @param inName Entity name to set.
		 */
		inline void setName(const std::string inName) {
		  mName = inName;
		};
				
		/*!
		 * @return Entity's id.
		 */
		inline int getObjectId() const {
		  return idObject;
		};
		
		/*! \brief
		 *  Entity names are not unique and are no identifiers, mainly used for debugging.
		 *
		 * @return Entity's in-game name.
		 */
		inline std::string& getName() {
		  return mName;
		};
		
		/*! \brief
		 *  An Entity's master node is the parent of all the renderable parts:
		 *  the mesh, particle systems, and anything else.
		 */
		inline Ogre::SceneNode* getMasterNode() {
		  return mMasterNode;
		};
		
		/*! \brief
		 *  The Entity's SceneNode is the node that contains the Ogre::Entity and its mesh.
		 */
		inline void _attachSceneNode(Ogre::SceneNode* inNode) {
		  mSceneNode = inNode;
		};
		inline Ogre::SceneNode* getSceneNode() {
		  return mSceneNode;
		};
		
		/*! \brief
		 *  Every Pixy::Entity has an Ogre::Entity that defines its mesh. 
		 */
		inline void _attachSceneObject(Ogre::MovableObject* inObject) {
		  mSceneObject = inObject;
		};
		inline Ogre::MovableObject* getSceneObject() {
		  return mSceneObject;
		};
		
		/*! \brief
		 *  Returns the name of the mesh resource used by this Entity.
		 */
		inline std::string& getMesh() {
		  return mMesh;
		};
		
		/*! \brief
	   *  Called by Level when spawning Duettes to oppose their directions.
	   */
	  inline void setDirection(Vector3 inDirection) {
	    mLog->debugStream() << "setting direction to " << inDirection.x << ", " << inDirection.y << ", " << inDirection.z;
	    mDirection = inDirection;
	  };
	  
		inline Vector3& getDirection() {
		  return mDirection;
		};
		inline void setMoveSpeed(const Real inSpeed) {
		  mMoveSpeed = inSpeed;
		};
		inline void setMaxSpeed(const float inSpeed) {
		  mMaxSpeed = inSpeed;
		};
		inline Real getMoveSpeed() const {
		  return mMoveSpeed;
		};
		inline float getMaxSpeed() const {
		  return mMaxSpeed;
		};
		
		inline btRigidBody* getRigidBody() {
		  return mPhyxBody;
		};
		
	protected:

    static long idCounter;
    
		int idObject;                       //! Unique entity id
		std::string mName;					//! Entity's in-game name
		bool fDead;
		
		bool fHasSfx; // toggler for sound effects
		bool fHasFx; // toggler for visual effects
		
		std::string mMesh;					//! Entity's in-game name		
		Ogre::SceneNode         *mSceneNode, *mMasterNode;
		Ogre::MovableObject     *mSceneObject;
		btCollisionShape		*mPhyxShape;
		MotionState				*mPhyxMS;
		btRigidBody				*mPhyxBody;
		
		Real mMoveSpeed;
		float mMaxSpeed;
		Vector3 mDirection;
		Vector3 mPosition;
		
		//! helper method for copy/assignment methods
		//! copies all data from src and sets it into this entity
		virtual void copyFrom(const Entity& src);
		
		log4cpp::Category* mLog;
	}; // end of Entity class
} // end of Pixy namespace
#endif
