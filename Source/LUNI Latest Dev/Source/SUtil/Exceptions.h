#pragma once

#include <exception>

// These are exceptions if there is a pointer to NULL
struct NullPointerException : public std::exception {
	NullPointerException(); // Create an exception
	NullPointerException(const char* _msg); // Create an exception with a message
};