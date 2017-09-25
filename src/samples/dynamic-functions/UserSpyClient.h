#pragma once
#include "SpyClient.h"
#include "spylib_interfaces.h"

#include <functional>
#include <string>
#include <map>

class UserSpyClient : public SpyClient {
	std::map<std::string,CustomCommandId> _dynamicCmdIdMap;
	std::map<std::string, HMODULE> _spyLibMap;
public:
	UserSpyClient();
	~UserSpyClient();

	bool inject(const char* processName);
	bool loadDynamicFunctions(const char* dllFile);
	bool unloadModulesAndStopMonitor();

	std::string getInjectedProcessPath();
	int getMinElmInRange(int min, int max);
};