#include "UserSpyClient.h"
#include "SpyClientUtils.hpp"
#include "spylib_interfaces.h"

#include <filesystem>
#include <string>
#include <iostream>

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

bool UserSpyClient::readDummyTree(void* address, std::string& result) {
	auto handler = [&result](char*& buffer) {
		result = buffer;
	};
	int iRes = readCustomObject<char*>(this, (CustomCommandId)UserCommandId::ReadDummyTree + _predefinedBase, handler, (void*)nullptr);
	if (iRes) {
		cout << "Error: command ReadDummyTree return error!" << endl;
	}
	return iRes == 0;
}