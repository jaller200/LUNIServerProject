#include "Common.h"
#include "SUtil\Kbhit.h"
#include <iomanip>

using namespace std;

string RawDataToString(uchar* data, uint size, bool onlyraw) {
	ostringstream buffer;
	if (!onlyraw ) buffer << "data in bytes:";
	for (uint i = 0; i < size; i++) {
		if (!onlyraw) if (i % 16 == 0) buffer << "\n\t\t";
		else if (!onlyraw) buffer << " ";
		buffer << setw(2) << hex << setfill('0') << (int)data[i];
	}
	if (!onlyraw) buffer << "\n\n";
	return buffer.str();
}

void QuitError(const string& msg) {
	cout << "Terminated " << msg << endl << "PRESS ENTER TO QUIT" << endl;
	while (!_kbhit());
	exit(1);
}