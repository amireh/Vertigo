// Sphere.cpp
#include "Sphere.h"
#include "Utility.h"

namespace Pixy
{
	
    Sphere::Sphere()
    {
		mLog = new log4cpp::FixedContextCategory(CLIENT_LOG_CATEGORY, "Sphere");
        mLog->infoStream() << "created";
    };

	Sphere::~Sphere()
	{
	    mLog->infoStream() <<"destructed";
		
		if (mLog) {
			delete mLog;
			mLog = 0;
		}
	};
	
	void Sphere::live() {};
	void Sphere::die() {};
	
	void Sphere::render() {};
	void Sphere::update() { };
	
	void Sphere::copyFrom(const Sphere& src) {
	};

} // end of namespace
