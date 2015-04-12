#pragma once

#include <exception>

struct NullPointerException : public std::exception {
	NullPointerException();
	NullPointerException(const char* _msg);
};