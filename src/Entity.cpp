// Entity.cpp
#include "Entity.h"
#include "Utility.h"
//using namespace Ogre;
namespace Pixy
{

	Entity::~Entity()
	{
		if (mLog) {
			delete mLog;
			mLog = 0;
		}
	};

    Entity::Entity()
    {
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Entity");
		
		idObject = -1;
		mName = "Unnamed";
		mHP = 1;
		
	};

	void Entity::setObjectId(int inIdEntity)
	{
		idObject = inIdEntity;
	};


	void Entity::setName(std::string inName)
	{
		mName = inName;
	};

	void Entity::setHP(int inHealth)
	{
		mHP = inHealth;
	};




	int Entity::getObjectId() const { return idObject; };
	std::string Entity::getName() const { return mName; };
	int Entity::getHP() const { return mHP; };

    Entity::Entity(const Entity& src)
    {
        copyFrom(src);
    };

    Entity& Entity::operator=(const Entity& rhs)
    {
        // check for self-assignment
        if (this == &rhs)
        {
            return (*this);
        }

        copyFrom(rhs);

        return (*this);
    };

    void Entity::copyFrom(const Entity& src)
    {
        // handle the Entity copying part
        idObject = src.idObject;
        mName = src.mName;
        mHP = src.mHP;
    };

    void Entity::printInfo()
    {

        mLog->debugStream() << "Name : " << mName;
        mLog->debugStream() << "Object id : " << idObject;
        mLog->debugStream() << "Health : " << mHP;

    };

    void Entity::live()
    {
        mLog->infoStream() << mName << " is alive!";
    }
	
    void Entity::die()
    {
        mLog->infoStream() << mName << " is dead!";
    }

} // end of namespace
