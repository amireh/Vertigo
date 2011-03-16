/*
 *  Utility.h
 *  Elementum
 *
 *  Created by Ahmad Amireh on 2/21/10.
 *  Copyright 2010 JU. All rights reserved.
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
