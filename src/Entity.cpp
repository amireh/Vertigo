// Entity.cpp
#include "Entity.h"
#include "Utility.h"
//using namespace Ogre;
namespace Pixy
{

  long Entity::idCounter = 0;
  int Entity::sphereCollidesWith = COL_OBSTACLES | COL_WALLS;
  int Entity::obstacleCollidesWith = COL_SPHERE | COL_WALLS;
  
	Entity::~Entity()
	{
		/*if (mLog) {
			delete mLog;
			mLog = 0;
		}*/
	};

    Entity::Entity()
    {
		//mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Entity");
		
		idObject = ++idCounter;
		mName = "Unnamed";
		mHP = 1;
		mMoveSpeed = 0;
		mDirection = Vector3::ZERO;
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
	std::string& Entity::getName() { return mName; };
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
        //mLog->infoStream() << mName << " is alive!";
    }
	
    void Entity::die()
    {
        //mLog->infoStream() << mName << " is dead!";
    }
	
	void Entity::attachSceneNode(Ogre::SceneNode* inNode) { mSceneNode = inNode; }
    void Entity::attachSceneObject(Ogre::MovableObject* inObject) { mSceneObject = inObject; }
    Ogre::SceneNode* Entity::getSceneNode() { return mSceneNode; }
    Ogre::MovableObject* Entity::getSceneObject() { return mSceneObject; }
	
	void Entity::setMesh(std::string inMesh) {
		mMesh = inMesh;
	}
	std::string Entity::getMesh() {
		return mMesh;
	}
	
	Vector3& Entity::getDirection() {
		return mDirection;
	}
	Real Entity::getMoveSpeed() {
		return mMoveSpeed;
	}
	//btCollisionObject* Entity::getObject() { return mObject; };
	btRigidBody* Entity::getRigidBody() { return mPhyxBody; };
	MotionState* Entity::getMotionState() { return mPhyxMS; };
	btCollisionShape* Entity::getCollisionShape() { return mPhyxShape; };
	
	bool Entity::dead() { return fDead; };
	ENTITY_TYPE Entity::type() { return mType; };
	
	void Entity::collide(Entity* target) {
	  mLog->warnStream() << "parent collide() called, children MUST override";
	}
	
	Ogre::SceneNode* Entity::getMasterNode() {
	  return mMasterNode;
	}
	
	void Entity::setMoveSpeed(const Real inSpeed) { mMoveSpeed = inSpeed; };
} // end of namespace
