#include "spylib.h"
#include "spylib_interfaces.h"
#include "spy_interfaces.h"

extern "C" {

	SPYLIB_API int __stdcall getPredefinedFunctionCount() {
		return (int)UserCommandId::PredefinedCommandCount;
	}

	SPYLIB_API int __stdcall loadPredefinedFunctions(void* context, FSetPredefinedFunction fx, CustomCommandId beginPredefinedCmdId) {
		return 0;
	}
}