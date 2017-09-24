#include <iostream>
#include <string>
#include <vector>

#include <conio.h>
#include <random>

using namespace std;

vector<int> _gobalArray;

extern "C" {
	__declspec(dllexport) vector<int>& getGlobalArray() {
		return _gobalArray;
	}
}

void buildGlobalArray() {
	auto& arr = getGlobalArray();

	random_device rd;   // non-deterministic generator  
	mt19937 gen(rd());  // to seed mersenne twister. 

						// random size of array
	uniform_int_distribution<> sizeDist(1, 10); // distribute results between 1 and 10 inclusive.  
	uniform_int_distribution<> elmDist(-10, 10); // distribute results between -10 and 10 inclusive.

	arr.resize(sizeDist(gen));

	cout << "generated array: {";
	for (size_t i = 0; i < arr.size(); i++) {
		arr[i] = elmDist(gen);
		cout << arr[i] << ",";
	}
	cout << "}" << endl;
}

int main(int argc, char* argv[]) {	
	buildGlobalArray();
	cout << "press any key to exit" << endl;
	_getch();
	return 0;
}