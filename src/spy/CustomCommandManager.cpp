#include "CustomCommandManager.h"
#include <iostream>

using namespace std;

string g_dummyFunctionName;
static ModuleId g_moduleIdSeed = 0;

CustomFunctionManager::CustomFunctionManager() {
}

CustomFunctionManager::~CustomFunctionManager() {
}

ModuleInfo* CustomFunctionManager::createModuleContainer(HMODULE hModule, const std::string& moduleName) {
	ModuleInfo dummy;
	dummy.moduleName = moduleName;
	dummy.moduleId = g_moduleIdSeed++;
	dummy.hModule = hModule;

	auto it = _moduleHandleMap.insert( make_pair(dummy.moduleId, dummy) );
	ModuleInfo* pModueInfo = &it.first->second;
	pModueInfo->commandListRef = make_shared<std::list<CustomCommandId>>();
	return &(it.first->second);
}

ModuleInfo* CustomFunctionManager::getModuleContainer(ModuleId moduleId) {
	auto it = _moduleHandleMap.find(moduleId);
	if (it != _moduleHandleMap.end()) {
		return &(it->second);
	}
	return nullptr;
}

bool CustomFunctionManager::unloadModule(ModuleId moduleId) {
	auto it = _moduleHandleMap.find(moduleId);
	if (it == _moduleHandleMap.end()) {
		return false;
	}

	ModuleInfo& moduleInfo = it->second;
	if (!FreeLibrary(moduleInfo.hModule)) {
		return false;
	}

	for (auto it = moduleInfo.commandListRef->begin(); it != moduleInfo.commandListRef->end(); it++) {
		auto& functionInfo = _customFuncs[*it];
		functionInfo.address = nullptr;
	}

	_moduleHandleMap.erase(it);
	return true;
}

const string& CustomFunctionManager::getFunctionName(CustomCommandId cmdId) const {
	if ((size_t)cmdId >= _customFuncs.size()) {
		cout << "Out of range" << std::endl;
		return g_dummyFunctionName;
	}
	const FunctionInfo& functionInfo = _customFuncs[(size_t)cmdId];
	return functionInfo.functionName;
}

void* CustomFunctionManager::getFunctionAddress(CustomCommandId cmdId) const {
	if ((size_t)cmdId >= _customFuncs.size()) {
		cout << "Out of range" << std::endl;
		return nullptr;
	}
	const FunctionInfo& functionInfo = _customFuncs[(size_t)cmdId];
	return functionInfo.address;
}

CustomCommandId CustomFunctionManager::addCustomFunction(void* address, const char* functionName) {
	if (_customFuncs.size() >= (size_t)CUSTOM_COMMAND_END - 1) {
		cout << "The custom library reachs maximum number of function" << std::endl;
		return CUSTOM_COMMAND_END;
	}
	FunctionInfo functionInfo;
	functionInfo.address = address;
	if (functionName != nullptr) {
		functionInfo.functionName = functionName;
	}

	_customFuncs.push_back(functionInfo);
	return (CustomCommandId)(_customFuncs.size() - 1);
}

bool CustomFunctionManager::setCustomFunction(CustomCommandId cmdId, void* address, const char* functionName) {
	if ((size_t)cmdId >= _customFuncs.size()) {
		cout << "Invalid id" << std::endl;
		return false;
	}
	if (address == nullptr) {
		cout << "Invalid function address" << std::endl;
		return false;
	}
	FunctionInfo& functionInfo = _customFuncs[(size_t)cmdId];
	functionInfo.address = address;
	functionInfo.functionName = functionName;

	return true;
}

int CustomFunctionManager::addFunctionSpace(int functionCount) {
	int availableSpace = min((int)(CUSTOM_COMMAND_END - (int)_customFuncs.size()), functionCount);
	if (availableSpace < 0) return 0;

	if (availableSpace < functionCount) {
		cout << "there is only " << availableSpace << " seat(s) left in function container" << std::endl;		
	}

	_customFuncs.resize(availableSpace + _customFuncs.size());

	return availableSpace;
}

int CustomFunctionManager::getCommandCount() const {
	return (int)_customFuncs.size();
}