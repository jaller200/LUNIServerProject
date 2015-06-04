#pragma once

#include "Common.h"

class Object {
	public:
		// Check to see whether an object is equal to another object
		virtual bool Equals(Object& o) const;

		// Convert an object to a string
		virtual const char* ToString() const;

		// Get the hash code of an object
		virtual int GetHashCode() const;

		// Get the type of an object
		virtual const char* GetType() const;

		// Object destructor
		virtual destructor Object() {}
};