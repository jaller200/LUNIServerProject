#include "Exceptions.h"

using namespace std;

NullPointerException::NullPointerException() { exception::exception(); }
NullPointerException::NullPointerException(const char* _msg) { exception::exception(_msg); }