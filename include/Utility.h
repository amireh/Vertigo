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
 
#ifndef H_Utility_H
#define H_Utility_H
#include <exception>
#include <stdexcept>
#include <typeinfo>
#include "Pixy.h"
namespace Pixy {
	
	class BadConversion : public std::runtime_error {
	public:
		inline BadConversion(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	
	class BadStream : public std::runtime_error {
	public:
		inline BadStream(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	
	class UnknownEvent : public std::runtime_error {
	public:
		inline UnknownEvent(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	
	class UnknownEventSender : public std::runtime_error {
	public:
		inline UnknownEventSender(const std::string& s)
		: std::runtime_error(s)
		{ }
	};	
	
	class BadEvent : public std::runtime_error {
	public:
		inline BadEvent(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	
	class UnassignedProfile : public std::runtime_error {
	public:
		inline UnassignedProfile(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	
	class UnknownSpell : public std::runtime_error {
	public:
		inline UnknownSpell(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	
	template<typename T>
	inline std::string stringify(const T& x)
	{
		std::ostringstream o;
		if (!(o << x))
			throw BadConversion(std::string("stringify(")
								+ typeid(x).name() + ")");
		return o.str();
	}
	
	// helper; converts an integer-based type to a string
	template<typename T>
	inline void convert(const std::string& inString, T& inValue,
						bool failIfLeftoverChars = true)
	{
		std::istringstream _buffer(inString);
		char c;
		if (!(_buffer >> inValue) || (failIfLeftoverChars && _buffer.get(c)))
			throw BadConversion(inString);
	}
	
	template<typename T>
	inline T convertTo(const std::string& inString,
					   bool failIfLeftoverChars = true)
	{
		T _value;
		convert(inString, _value, failIfLeftoverChars);
		return _value;
	}
	
	class Utility {
	public:
	static inline log4cpp::Category& getLogger() {
		return log4cpp::Category::getInstance(CLIENT_LOG_CATEGORY);
	}

	};
}

#endif
