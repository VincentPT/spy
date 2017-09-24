#include "spylib.h"
#include "spylib_interfaces.h"
#include "spy_interfaces.h"

#include <Windows.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


extern "C" {
	SPYLIB_API ReturnData __cdecl getProcessPath() {
		char* buffer = (char*)malloc(MAX_PATH);
		GetModuleFileNameA(NULL, buffer, MAX_PATH);

		cout << "current process path: " << buffer << endl;

		ReturnData returnData;
		returnData.sizeOfCustomData = MAX_PATH;
		returnData.customData = buffer;

		return returnData;
	}

	SPYLIB_API ReturnData __cdecl getMinElemenInRange(int min, int max) {
		// init with a out of range value, it is asumming for cases of error
		int val = max + 1;

		typedef vector<int>& (*FGetGlobalArray)();
		// get current process module handle
		HMODULE hCurrentProcessModule = GetModuleHandleA(NULL);
		// find the getGlobalArray function
		FGetGlobalArray getGlobalArray = (FGetGlobalArray)GetProcAddress(hCurrentProcessModule, "getGlobalArray");
		if (getGlobalArray == nullptr) {
			cout << "Error: getGlobalArray cannot be found in current process" << endl;
		}
		else {
			auto& arr = getGlobalArray();

			cout << "find min elemen in range[" << min << ", " << max << "] from array {";
			for (size_t i = 0; i < arr.size(); i++) {
				cout << arr[i] << ",";
			}
			cout << "}" << endl;

			int delta_min = max - min + 1;
			for (auto elm : arr) {
				if (elm >= min && elm <= max) {
					if (delta_min > elm - min) {
						delta_min = elm - min;
					}
				}
			}

			val = min + delta_min;
		}

		ReturnData returnData;
		returnData.sizeOfCustomData = 0;
		returnData.customData = (char*)(size_t)val;

		return returnData;
	}
}