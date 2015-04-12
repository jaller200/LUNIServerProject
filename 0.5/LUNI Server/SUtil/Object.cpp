#pragma once

#include "Object.h"
#include <typeinfo>
#include <sstream>

bool Object::Equals(Object& o) const { return this == &o; }

const char* Object::ToString() const {
	std::ostringstream oss;
	oss << GetType() << "@" << this;
	return oss.str().c_str();
}

int Object::GetHashCode() const { return (int)this; }

const char* Object::GetType() const { return typeid(*this).name(); }