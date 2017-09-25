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

bool UserSpyClient::inject(const char* processName) {
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

	bool blRes = SpyClient::inject(processName, rootSpyPath, dependencies);
	return blRes;
}

string UserSpyClient::getInjectedProcessPath() {
	string res;
	auto handler = [&res](ReturnData& returnData) {
		res = returnData.customData;
	};
	executeCommandAndFreeCustomData(this, (CustomCommandId)UserCommandId::GetInjectedProcessName + _predefinedBase, handler);
	return res;
}

int UserSpyClient::getMinElmInRange(int min, int max) {
	int res = max + 1;
	auto handler = [&res](ReturnData& returnData) {
		if (returnData.returnCode == (int)ReturnCode::Success) {
			res = (int)(size_t)(returnData.customData);
		}
	};
	executeCommand(this, (CustomCommandId)UserCommandId::GetMinElmInRange + _predefinedBase, handler, min, max);
	return res;
}