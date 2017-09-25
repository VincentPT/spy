#include "UserSpyClient.h"
#include "SpyClientUtils.hpp"

#include <filesystem>
#include <string>

using namespace std;
using namespace std::experimental::filesystem::v1;

UserSpyClient::UserSpyClient() {
}

UserSpyClient::~UserSpyClient() {

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
