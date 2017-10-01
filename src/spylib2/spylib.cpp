#include "spylib.h"
#include "spylib_interfaces.h"
#include "spy_interfaces.h"

#include <Windows.h>
#include <iostream>

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
#ifdef _M_X64
ReturnData __cdecl showArguments(char a, short b, int c, __int64 d, float& e, const double* f, DummyStruct& g) {
#else
ReturnData __cdecl showArguments(char a, short b, int c, __int64& d, float& e, const double* f, DummyStruct& g) {
#endif //_M_X64
	
	// print the arguments value
	cout << __FUNCTION__ " is called with following arguments:" << endl;
	cout << "a =" << (int)a << endl;
	cout << "b =" << b << endl;
	cout << "c =" << c << endl;
	cout << "d =" << d << endl;
	cout << "e =" << e << endl;
	cout << "f =" << *f << endl;
	cout << "g = {" << g.val1 << ", " << g.val2 << "}" << endl;

	// change one value
	e = 1234.0f;

	ReturnData returnData;
	returnData.sizeOfCustomData = 0;
	returnData.customData = nullptr;
	returnData.returnCode = 0;

	return returnData;
}

extern "C" {

	/**
	*   brief an export function that will be executed in spy engine, when a spy app send command LOAD_PREDEFINED_FUNCTIONS to the engine
	*	
	*   getPredefinedFunctionCount should return number predefined function that the user want to loaded to the engine
	**/
	SPYLIB_API int getPredefinedFunctionCount() {
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
	SPYLIB_API int loadPredefinedFunctions(void* context, FSetPredefinedFunction fx, CustomCommandId cmdBase) {
		fx(context, (CustomCommandId)UserCommandId::ReadDummyTree, nullptr);
		fx(context, (CustomCommandId)UserCommandId::GetInjectedProcessName, getProcessPath);
		fx(context, (CustomCommandId)UserCommandId::ShowArguments, showArguments);
		return 0;
	}
}