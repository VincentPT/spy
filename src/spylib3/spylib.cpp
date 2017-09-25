#include "spylib.h"
#include "spylib_interfaces.h"
#include "spy_interfaces.h"

#include <Windows.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

ReturnData __cdecl getProcessPath() {
	char* buffer = (char*)malloc(MAX_PATH);
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	cout << "current process path: " << buffer << endl;

	ReturnData returnData;
	returnData.sizeOfCustomData = MAX_PATH;
	returnData.customData = buffer;

	return returnData;
}

ReturnData __cdecl getMinElemenInRange(int min, int max) {
	ReturnCode returnCode = ReturnCode::Fail;
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
				returnCode = ReturnCode::Success;
			}
		}

		val = min + delta_min;
	}

	ReturnData returnData;
	returnData.sizeOfCustomData = 0;
	returnData.customData = (char*)(size_t)val;
	returnData.returnCode =(int)returnCode;

	return returnData;
}

extern "C" {

	/**
	*   brief an export function that will be executed in spy engine, when a spy app send command LOAD_PREDEFINED_FUNCTIONS to the engine
	*	
	*   getPredefinedFunctionCount should return number predefined function that the user want to loaded to the engine
	**/
	SPYLIB_API int __stdcall getPredefinedFunctionCount() {
		return (int)UserCommandId::PredefinedCommandCount;
	}

	/**
	*   brief an export function that will be executed in spy engine, when a spy app send command LOAD_PREDEFINED_FUNCTIONS to the engine
	*
	*   params
	*		context: context pass by spy engine, user spy library should pass it back to spy engine when use fx to set a custom command id
	*		fx     : a function pointer in spy engine, uer spy library should use it to load a custom command to spy engine
	*		cmdBase: a command id base for predefined commands id will build up, spy library may store this value if need,
	*				note that, this value is also returned to spy client from LOAD_PREDEFINED_FUNCTIONS commands 
	* 
	*   loadPredefinedFunctions should return zero to the engine know that loaded function should be kept in the engine for using in future
	*                           or nonzero to notify that the engine should discard the loadding result
	**/
	SPYLIB_API int __stdcall loadPredefinedFunctions(void* context, FSetPredefinedFunction fx, CustomCommandId cmdBase) {
		fx(context, (CustomCommandId)UserCommandId::ReadDummyTree, nullptr);
		fx(context, (CustomCommandId)UserCommandId::GetInjectedProcessName, getProcessPath);
		fx(context, (CustomCommandId)UserCommandId::GetMinElmInRange, getMinElemenInRange);
		return 0;
	}
}