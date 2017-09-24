#pragma once

#include "spy_interfaces.h"

#include <Windows.h>

#include <memory>
#include <map>
#include <list>
#include <string>
#include <vector>

struct FunctionInfo
{
	std::string functionName;
	void* address;
};

typedef std::shared_ptr<std::list<CustomCommandId>> CommandListRef;

struct ModuleInfo
{
	std::string moduleName;
	CommandListRef commandListRef;	
};

class CustomFunctionManager {
	typedef std::map<HMODULE, ModuleInfo> ModuleHandleMap;	
	ModuleHandleMap _moduleHandleMap;
	std::vector<FunctionInfo> _customFuncs;

public:
	CustomFunctionManager();
	virtual ~CustomFunctionManager();

	ModuleInfo* createModuleContainer(HMODULE hModule, const std::string& moduleName);
	ModuleInfo* getModuleContainer(HMODULE);
	bool unloadModule(HMODULE);

	const std::string& getFunctionName(CustomCommandId cmdId) const;
	void* getFunctionAddress(CustomCommandId cmdId) const;
	CustomCommandId addCustomFunction(void* address, const char* functionName);
	bool setCustomFunction(CustomCommandId cmdId, void* address, const char* functionName);
	int addFunctionSpace(int functionCount);
	int getCommandCount() const;
};