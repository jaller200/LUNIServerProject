#pragma once

#include "Object.h"
#include <typeinfo>
#include <sstream>

// Check to see if an object equals another object
bool Object::Equals(Object& o) const { return this == &o; }

// Convert object to a string by getting type
const char* Object::ToString() const {
	std::ostringstream oss;
	oss << GetType() << "@" << this;
	return oss.str().c_str();
}

// Get the hash code of an object
int Object::GetHashCode() const { return (int)this; }

// Get the type of an object by figuring out at run-time
const char* Object::GetType() const { return typeid(*this).name(); }