#include "Common.h"
#include "SUtil\Kbhit.h"
#include <iomanip>

using namespace std;

// This converts raw hex data to a readable ASCII string
string RawDataToString(uchar* data, uint size, bool onlyraw) {
	// Initialize the ostringstream buffer
	ostringstream buffer;

	// If onlyraw is false, print "Data in bytes: "
	if (!onlyraw ) buffer << "Data in bytes: ";

	// Copy the data into the stringstream
	for (uint i = 0; i < size; i++) {
		if (!onlyraw) if (i % 16 == 0) buffer << "\n\t\t";
		else if (!onlyraw) buffer << " ";
		buffer << setw(2) << hex << setfill('0') << (int)data[i];
	}

	if (!onlyraw) buffer << "\n\n";

	// Return the stringstream as a std::string
	return buffer.str();
}

// Server QuitError
void QuitError(const string& msg) {
	cout << "Terminated " << msg << endl << "PRESS ENTER TO QUIT" << endl;
	while (!_kbhit());
	exit(1);
}