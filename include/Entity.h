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

namespace Pixy
{
	/*! \class Entity Entity.h "src/Entity.h"
	 *
	 * \brief Defines base attributes and actions for GameObjects.
	 *
	 * \note Please note that for the creation of GameObjects 
	 * refer to their respective Factories, do NOT use this
	 * directly.
	 */
	class Entity
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
		
		
		
		/*!
		 * @return Entity's in-game name.
		 */
		std::string getName() const;
		
		
		/*!
		 * @return Entity's current health points.
		 */
		int getHP() const;
		
				
		//! Prints out information about this Entity to Log ofstream; used for debugging
		virtual void printInfo();
		
		protected:

		int idObject;                       //! Unique entity id
		std::string mName;					//! Entity's in-game name
		int mHP;                            //! Entity's health stat
		
		//! helper method for copy/assignment methods
		//! copies all data from src and sets it into this entity
		virtual void copyFrom(const Entity& src);
		
		log4cpp::Category* mLog;
	}; // end of Entity class
} // end of Pixy namespace
#endif
