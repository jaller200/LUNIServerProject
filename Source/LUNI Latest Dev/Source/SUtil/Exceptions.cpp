#include "Exceptions.h"

using namespace std;

// Create two exceptions
NullPointerException::NullPointerException() { exception::exception(); }
NullPointerException::NullPointerException(const char* _msg) { exception::exception(_msg); }