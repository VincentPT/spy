#include "UserSpyClient.h"
#include "SpyClientUtils.hpp"
#include "spylib_interfaces.h"

#include <filesystem>
#include <iostream>
#include <string>

using namespace std;
using namespace std::experimental::filesystem::v1;

#define GetProcessPath "getProcessPath"
#define GetMinElemenInRange "getMinElemenInRange"

UserSpyClient::UserSpyClient() {
}

UserSpyClient::~UserSpyClient() {
}

bool UserSpyClient::startMonitorProcess(const char* processName) {
	auto hCurrentProcessBase = GetModuleHandleA(NULL);
	string currentProcesssFilePath(256, ' ');

	if (GetModuleFileNameA(hCurrentProcessBase, &currentProcesssFilePath[0], (DWORD)currentProcesssFilePath.size()) == FALSE) {
		return false;
	}
	
	path thePath(currentProcesssFilePath);
	string processParentPath = thePath.parent_path().u8string();

	string rootSpyPath = processParentPath + "\\" SPY_ROOT_DLL_NAME;

	// list all dependencies dll here
	list<string> dependencies = {
	};

	bool blRes = SpyClient::startMonitorProcess(processName, rootSpyPath, dependencies);
	return blRes;
}

bool UserSpyClient::unloadModulesAndStopMonitor() {	
	HMODULE hModule;
	for (auto& elm : _spyLibMap) {
		hModule = elm.second;
		if(unloadModule(hModule) != 0) {
			cout << "failed to uninject " << elm.first << endl;
		}
		else {
			cout << "uninjected module from remote process " << elm.first << endl;
		}
	}
	if (!stopMonitorProcess()) {
		cout << "failed to stop monitor for process " << getProcessName() << endl;
	}
	else {
		cout << "stop monitor process " << getProcessName() << " successfully" << endl;
	}

	return true;
}

bool UserSpyClient::loadDynamicFunctions(const char* dllFile) {
	const char* functions[] = {
		GetProcessPath,
		GetMinElemenInRange,
	};

	constexpr int functionCount = sizeof(functions) / sizeof(functions[0]);
	// check any function is duplicated or not
	for (int i = 0; i < functionCount; i++) {
		auto it = _dynamicCmdIdMap.find(functions[i]);
		if (it != _dynamicCmdIdMap.end()) {
			cout << "duplicated function found " << functions[i] << endl;
			return false;
		}
	}

	HMODULE module;
	list<CustomCommandId> cmdIds;

	int iRes = loadCustomDynamicFunctions(dllFile, functions, functionCount, cmdIds, &module);
	if (iRes) {
		cout << "Cannot load functions in dll file " << dllFile << endl;
		return false;
	}

	// store the loaded module 
	_spyLibMap[dllFile] = module;

	if (cmdIds.size() != (size_t)functionCount) {
		cout << "number of command return by loadCustomDynamicFunctions is mismatch with number of function" << dllFile << endl;
		return false;
	}

	// map the function name with return comamnd ids
	auto it = cmdIds.begin();
	for (int i = 0; i < functionCount; i++) {
		_dynamicCmdIdMap[functions[i]] = *it++;
	}

	return true;
}

string UserSpyClient::getInjectedProcessPath() {
	string res;
	auto handler = [&res](char*& buffer) {
		res = buffer;
	};

	auto it = _dynamicCmdIdMap.find(GetProcessPath);
	if (it != _dynamicCmdIdMap.end()) {
		readCustomObject<char*>(this, it->second, handler);
	}
	return res;
}

int UserSpyClient::getMinElmInRange(int min, int max) {
	int res = max + 1;
	auto handler = [&res](int& val) {
		res = val;
	};
	auto it = _dynamicCmdIdMap.find(GetMinElemenInRange);
	if (it != _dynamicCmdIdMap.end()) {
		readCustomObject2<int>(this, it->second, handler, min, max);
	}
	
	return res;
}