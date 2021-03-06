#include "Spy.h"
#include <Shlwapi.h>
#include <Windows.h>

#include <iostream>
#include <vector>
#include <list>

#include "CustomCommandInvoker.h"
#include "CustomCommandManager.h"
#include "spy_interfaces.h"

// internal command function handlers
int freeBufferCommand(FreeBufferCmdData* commandData);
int loadPredifnedFunctions(LoadPredefinedCmdData* param);
int invokeCustomCommand(CustomCommandCmdData* commandData);
int LoadCustomFunctions(LoadCustomFunctionsCmdData* commandData);
int unloadModule(UnloadModuleCmdData* commandData);
int getFunctionPtr(GetFunctionPtrCmdData* commandData);
int getModule(GetModuleCmdData* commandData);
int getModulePath(GetModulePathCmdData* commandData);

// using namespaces
using namespace std;

// extern variables declaration
extern int _runner_count;

// global variables
CustomFunctionManager customFunctionManager;

// macro definition
#define GetPredefinedFunctionCountName "getPredefinedFunctionCount"
#define LoadPredefinedFunctionsName "loadPredefinedFunctions"

// internal structures
struct SetPredefinedCommandContext {
	ModuleInfo* moduleInfo;
	CustomCommandId commandBase;
	unsigned short loadedFunctionCount;
};

extern "C" {
	SPY_API DWORD spyRoot(BaseCmdData* param) {
		switch (param->commandId)
		{
		case CommandId::CUSTOM_COMMAND:
			return invokeCustomCommand((CustomCommandCmdData*)param);
		case CommandId::FREE_BUFFER:
			return freeBufferCommand((FreeBufferCmdData*)param);
		case CommandId::LOAD_PREDEFINED_FUNCTIONS:
			return loadPredifnedFunctions((LoadPredefinedCmdData*)param);
		case CommandId::LOAD_CUSTOM_FUNCTIONS:
			return LoadCustomFunctions((LoadCustomFunctionsCmdData*)param);
		case CommandId::UNLOAD_MODULE:
			return unloadModule((UnloadModuleCmdData*)param);
		case CommandId::GET_CUSTOM_FUNCTION_PTR:
			return getFunctionPtr((GetFunctionPtrCmdData*)param);
		case CommandId::GET_MODULE:
			return getModule((GetModuleCmdData*)param);
		case CommandId::GET_MODULE_PATH:
			return getModulePath((GetModulePathCmdData*)param);
		default:
			break;
		}

		return -1;
	}
}

int freeBufferCommand(FreeBufferCmdData* commandData) {
	if (commandData->commandSize != sizeof(FreeBufferCmdData)) {
		cout << "Invalid free buffer command" << std::endl;
		return -1;
	}
	cout << "deallocated buffer:" << commandData->buffer << std::endl;
	free(commandData->buffer);
	//VirtualFree(commandData->buffer, commandData->bufferSize, MEM_RELEASE);

	return 0;
}

inline CustomCommandId addCustomFunction(void* pFunc, const char* sFunctionName) {
	return customFunctionManager.addCustomFunction(pFunc, sFunctionName);
}

int setCustomFunction(SetPredefinedCommandContext* context, CustomCommandId cmdId, void* pFunc) {
	if (context == nullptr) {
		cout << "setCustomFunction must be call with suppiled context of function " LoadPredefinedFunctionsName << std::endl;
		return -1;
	}
	ModuleInfo* moduleInfo = context->moduleInfo;
	cmdId = cmdId + context->commandBase;

	if (!customFunctionManager.setCustomFunction(cmdId, pFunc, "predefined function")) {
		return -1;
	}

	moduleInfo->commandListRef->push_back(cmdId);
	cout << "Command " << cmdId << " loaded!" << std::endl;
	context->loadedFunctionCount++;
	return 0;
}

inline void* getCustomFunction(CustomCommandId cmdId) {
	return customFunctionManager.getFunctionAddress(cmdId);
}


// load custom functions function
// all custom functions must be set here
int loadPredifnedFunctions(LoadPredefinedCmdData* param) {
	if (param->commandSize < sizeof(LoadPredefinedCmdData)) {
		cout << "Invalid custom command:" << (unsigned short)param->commandId << std::endl;
		return -1;
	}

	ReturnData& returnData = param->returnData;

	returnData.customData = nullptr;
	returnData.sizeOfCustomData = 0;

	char* dllFile = param->dllName;

	// first try load the dll file as it is, dll file can be an absolute path or a relative path
	HMODULE hModule = LoadLibrary(dllFile);

	// if dll cannot be load...
	if (hModule == NULL) {
		// if the dll file is a relative path, we find it in location of root dll
		HMODULE hModuleRoot = GetModuleHandle(SPY_ROOT_DLL_NAME);
		if (hModuleRoot == nullptr) {
			cout << "Name of spy root dll is changed." << std::endl;
			return -1;
		}
		char path[MAX_PATH];
		int pathLen;
		if (!(pathLen = GetModuleFileName(hModuleRoot, path, MAX_PATH))) {
			cout << "Cannot find location of root dll path" << std::endl;
			return -1;
		}

		// remove root dll name to get the directory
		path[pathLen - strlen(SPY_ROOT_DLL_NAME)] = 0;

		string moduleFullPath(path);
		moduleFullPath.append(dllFile);
		hModule = LoadLibrary(moduleFullPath.c_str());
		if (hModule == nullptr) {
			cout << "Cannot find location of spy lib dll (" << dllFile << ")" << std::endl;
			return -1;
		}
	}
	auto GetPredefinedFunctionCount = (FGetPredefinedFunctionCount)GetProcAddress(hModule, GetPredefinedFunctionCountName);
	if (GetPredefinedFunctionCount == nullptr) {
		FreeLibrary(hModule);
		cout << dllFile << " does not contain function " GetPredefinedFunctionCountName << std::endl;
		return -1;
	}
	auto LoadPredefinedFunctions = (FLoadPredefinedFunctions)GetProcAddress(hModule, LoadPredefinedFunctionsName);
	if (LoadPredefinedFunctions == nullptr) {
		FreeLibrary(hModule);
		cout << dllFile << " does not contain function " LoadPredefinedFunctionsName << std::endl;
		return -1;
	}
	auto moduleInfo = customFunctionManager.createModuleContainer(hModule, dllFile);

	returnData.sizeOfCustomData = sizeof(LoadPredefinedReturnData);
	auto pLoadPredefinedReturnData = (LoadPredefinedReturnData*)malloc(returnData.sizeOfCustomData);
	returnData.customData = (char*)pLoadPredefinedReturnData;
	pLoadPredefinedReturnData->hModule = hModule;
	pLoadPredefinedReturnData->moduleId = moduleInfo->moduleId;

	int iNumberOfPredefinedFunction = GetPredefinedFunctionCount();
	// check if there is no predefined functions need to be loaded
	if (iNumberOfPredefinedFunction <= 0) {
		return MAKE_RESULT_OF_LOAD_PREDEFINED_FUNC(0, CUSTOM_COMMAND_END);
	}

	SetPredefinedCommandContext context;
	context.moduleInfo = moduleInfo;
	context.commandBase = (CustomCommandId)customFunctionManager.getCommandCount();
	context.loadedFunctionCount = 0;

	// add space to store predefined functions
	customFunctionManager.addFunctionSpace(iNumberOfPredefinedFunction);
	// call the funcion the load the predefined function to the engine
	int iRes = LoadPredefinedFunctions(&context, (FSetPredefinedFunction)setCustomFunction, context.commandBase);
	if (iRes != 0) {
		// unload all loadded function of the module
		customFunctionManager.unloadModule(moduleInfo->moduleId);
		cout << LoadPredefinedFunctionsName " return a cancelled value(" << iRes << "the loading result will be discard" << std::endl;		
		return MAKE_RESULT_OF_LOAD_PREDEFINED_FUNC(0, CUSTOM_COMMAND_END);
	}

	return MAKE_RESULT_OF_LOAD_PREDEFINED_FUNC(context.loadedFunctionCount, context.commandBase);
}

// this function use to invoke the corresponding custom function with the custom data
int invokeCustomCommand(CustomCommandCmdData* commandData) {
	if (commandData->commandSize != sizeof(CustomCommandCmdData) + (commandData->paramCount - 1) * sizeof(void*)) {
		cout << "Invalid custom command:" << commandData->commandSize << std::endl;
		return -1;
	}

	// get function pointer
	void* fx = getCustomFunction(commandData->customCommandId);
	if (fx == nullptr) {
		cout << "custom command '" << (unsigned short)commandData->customCommandId << "' has been not set" << std::endl;
		return -1;
	}

	if (_runner_count <= commandData->paramCount) {
		cout << "custom command " << commandData->paramCount << " parameters has been not supported" << std::endl;
		return -1;
	}

	CustomCommandInvoker invoker(fx, commandData->paramCount);
	commandData->returnData = invoker.invoke(commandData->params);
	return 0;
}

int LoadCustomFunctions(LoadCustomFunctionsCmdData* commandData) {
	if (commandData->commandSize < sizeof(LoadCustomFunctionsCmdData)) {
		cout << "Invalid load custom command" << std::endl;
		return -1;
	}

	int bufferSize = commandData->commandSize - sizeof(LoadCustomFunctionsCmdData) + sizeof(LoadCustomFunctionsCmdData::fNames);
	if (commandData->fNames[bufferSize - 1] != 0) {
		cout << "Invalid load custom command" << std::endl;
		return -1;
	}
	char* dllFile = commandData->fNames;
	char* endBuffer = dllFile + bufferSize;
	auto c = dllFile + strlen(dllFile) + 1;

	// first try load the dll file as it is, dll file can be an absolute path or a relative path
	HMODULE hModule = LoadLibrary(dllFile);
	vector<CustomCommandId> commandIds;
	commandData->returnData.sizeOfCustomData = 0;
	commandData->returnData.customData = nullptr;

	// if dll cannot be load...
	if (hModule == NULL) {
		// if the dll file is not a relative path, we don't process more
		if (!PathIsRelative(dllFile)) {
			return 0;
		}
		// if the dll file is a relative path, we find it in location of root dll
		HMODULE hModuleRoot = GetModuleHandle(SPY_ROOT_DLL_NAME);
		if (hModuleRoot == nullptr) {
			cout << "Name of spy root dll is changed." << std::endl;
			return -1;
		}
		char path[MAX_PATH];
		int pathLen;
		if (!(pathLen = GetModuleFileName(hModuleRoot, path, MAX_PATH))) {
			cout << "Cannot find location of root dll path" << std::endl;
			return -1;
		}

		// remove root dll name to get the directory
		path[pathLen - strlen(SPY_ROOT_DLL_NAME)] = 0;

		string moduleFullPath(path);
		moduleFullPath.append(dllFile);
		hModule = LoadLibrary(moduleFullPath.c_str());
		if (hModule == nullptr) {
			cout << "Cannot find location of " << dllFile << std::endl;
			return 0;
		}
	}

	auto moduleInfo = customFunctionManager.createModuleContainer(hModule, dllFile);

	// load procs address from the loaded dll file
	void* pFunc;
	CustomCommandId customCommandId;
	while (c < endBuffer)
	{
		while (c < endBuffer && (*c == ' ' || *c == '\t') || *c == 0) {
			c++;
		}
		if (c >= endBuffer) {
			break;
		}

		pFunc = GetProcAddress(hModule, c);
		if (pFunc) {
			customCommandId = addCustomFunction(pFunc, c);
			if (customCommandId != CUSTOM_COMMAND_END && moduleInfo) {
				moduleInfo->commandListRef->push_back(customCommandId);
				cout << "Command " << c << " <--> " << customCommandId << " loaded!" << std::endl;
			}
		}
		else {
			customCommandId = CUSTOM_COMMAND_END;
		}

		commandIds.push_back(customCommandId);

		c += strlen(c) + 1;
	}
	ReturnData& returnData = commandData->returnData;	
	returnData.sizeOfCustomData = (int)(sizeof(LoadCustomFunctionsReturnData) - sizeof(LoadCustomFunctionsReturnData::cmdIds) + commandIds.size() * sizeof(CustomCommandId));

	// allocate memory for return data, spy client should be responsible to free the memory after using
	LoadCustomFunctionsReturnData* pCustomFunctionsReturnData = (LoadCustomFunctionsReturnData*)malloc(returnData.sizeOfCustomData);
	returnData.customData = (char*)pCustomFunctionsReturnData;

	pCustomFunctionsReturnData->hModule = hModule;
	pCustomFunctionsReturnData->moduleId = moduleInfo->moduleId;
	memcpy_s(&pCustomFunctionsReturnData->cmdIds[0], commandIds.size() * sizeof(CustomCommandId), commandIds.data(), commandIds.size() * sizeof(CustomCommandId));

	return 0;
}

int unloadModule(UnloadModuleCmdData* commandData) {
	if (commandData->commandSize != sizeof(UnloadModuleCmdData)) {
		cout << "Invalid load custom command" << std::endl;
		return -1;
	}

	return customFunctionManager.unloadModule(commandData->moduleId) ? 0 : 1;
}

int getFunctionPtr(GetFunctionPtrCmdData* commandData) {
	if (commandData->commandSize != sizeof(GetFunctionPtrCmdData)) {
		cout << "Invalid load custom command" << std::endl;
		return -1;
	}

	commandData->ptr = customFunctionManager.getFunctionAddress(commandData->customCommandId);
	return 0;
}

int getModule(GetModuleCmdData* commandData) {
	if (commandData->commandSize != sizeof(GetModuleCmdData)) {
		cout << "Invalid load custom command" << std::endl;
		return -1;
	}

	auto& returnData = commandData->returnData;
	returnData.customData = nullptr;
	returnData.sizeOfCustomData = 0;
	returnData.returnCode = 0;

	auto moduleInfo = customFunctionManager.getModuleContainer(commandData->moduleId);
	int cmdCount = 0;
	int rawDataSize = sizeof(ModuleData) - sizeof(ModuleData::cmdIds);

	if (moduleInfo != nullptr) {
		auto commandListRef = moduleInfo->commandListRef;
		ModuleData* pModuleData;
		if (commandListRef) {
			cmdCount = (int)moduleInfo->commandListRef->size();
			rawDataSize += cmdCount * sizeof(CustomCommandId);

			pModuleData = (ModuleData*)malloc(rawDataSize);
			auto pCmdId = pModuleData->cmdIds;

			for (auto it = commandListRef->begin(); it != commandListRef->end(); it++) {
				*pCmdId++ = *it;
			}
		}
		else {
			pModuleData = (ModuleData*)malloc(rawDataSize);
		}

		pModuleData->hModule = moduleInfo->hModule;
		pModuleData->commandCount = cmdCount;
		returnData.customData = (char*)pModuleData;
		returnData.sizeOfCustomData = rawDataSize;
	}

	return 0;
}

int getModulePath(GetModulePathCmdData* commandData) {
	if (commandData->commandSize != sizeof(GetModulePathCmdData)) {
		cout << "Invalid load custom command" << std::endl;
		return -1;
	}

	auto& returnData = commandData->returnData;
	returnData.customData = nullptr;
	returnData.sizeOfCustomData = 0;
	returnData.returnCode = 0;

	auto moduleInfo = customFunctionManager.getModuleContainer(commandData->moduleId);
	int cmdCount = 0;

	if (moduleInfo != nullptr) {

		HMODULE hModule = moduleInfo->hModule;
		int bufferSize = MAX_PATH;
		int rawDataSize = sizeof(ModulePathData) - sizeof(ModulePathData::path) + bufferSize;

		ModulePathData* pModuleData;
		pModuleData = (ModulePathData*)malloc(rawDataSize);

		pModuleData->hModule = moduleInfo->hModule;
		pModuleData->bufferSize = bufferSize;
		GetModuleFileNameA(hModule, pModuleData->path, bufferSize);

		returnData.customData = (char*)pModuleData;
		returnData.sizeOfCustomData = rawDataSize;
	}

	return 0;
}