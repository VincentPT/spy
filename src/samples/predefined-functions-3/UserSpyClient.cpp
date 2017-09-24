#include "UserSpyClient.h"
#include "SpyClientUtils.hpp"
#include "spylib_interfaces.h"

#include <filesystem>
#include <string>

using namespace std;
using namespace std::experimental::filesystem::v1;

UserSpyClient::UserSpyClient() : _predefinedBase(-1) {
}

UserSpyClient::~UserSpyClient() {
}

void UserSpyClient::setPredefinedCommandIdBase(int predefinedBase) {
	_predefinedBase = (CustomCommandId)predefinedBase;
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

string UserSpyClient::getInjectedProcessPath() {
	string res;
	auto handler = [&res](char*& buffer) {
		res = buffer;
	};
	readCustomObject<char*>(this, (CustomCommandId)UserCommandId::GetInjectedProcessName + _predefinedBase, handler);
	return res;
}

int UserSpyClient::getMinElmInRange(int min, int max) {
	int res = max + 1;
	auto handler = [&res](int& val) {
		res = val;
	};
	readCustomObject2<int>(this, (CustomCommandId)UserCommandId::GetMinElmInRange + _predefinedBase, handler, min, max);
	return res;
}