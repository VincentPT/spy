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

bool UserSpyClient::readDummyTree(void* address, std::string& result) {
	auto handler = [&result](ReturnData& returnData) {
		result = returnData.customData;
	};
	int iRes = executeCommandAndFreeCustomData(this, (CustomCommandId)UserCommandId::ReadDummyTree + _predefinedBase, handler, (void*)nullptr);
	if (iRes) {
		cout << "Error: command ReadDummyTree return error!" << endl;
	}
	return iRes == 0;
}

int UserSpyClient::showArguments(char a, short b, int c, __int64 d, float& e, double f, const DummyStruct& g) {
	auto handler = [](ReturnData& returnData) {
	};

	list<void*> allocatedBuffer;
	ScopeAutoFunction autoFreeRemoteData([&allocatedBuffer, this]() {
		for (auto it = allocatedBuffer.begin(); it != allocatedBuffer.end(); it++) {
			if (*it) {
				freeRemoteBuffer(*it);
			}
		}
	});

	// build arguments before execute remote command
	void* ra = (void*)a;
	void* rb = (void*)b;
	void* rc = (void*)(size_t)c;
#ifdef _M_X64
	void* rd = (void*)d;
#else
	void* rd = cloneBufferToRemoteProcess(this, d);
	if (rd == nullptr) return -1;
	allocatedBuffer.push_back(rd);
#endif // _M_X64
	void* re = cloneBufferToRemoteProcess(this, e);
	if (re == nullptr) return -1;
	allocatedBuffer.push_back(re);

	void* rf = cloneBufferToRemoteProcess(this, f);
	if (rf == nullptr) return -1;
	allocatedBuffer.push_back(rf);

	void* rg = cloneBufferToRemoteProcess(this, g);
	if (rg == nullptr) return -1;
	allocatedBuffer.push_back(rg);

	int iRes = executeCommandAndFreeCustomData(this, (CustomCommandId)UserCommandId::ShowArguments + _predefinedBase, handler,
		ra, rb, rc, rd, re, rf, rg);
	if (iRes) {
		cout << "Error: command showArguments return error!" << endl;
	}

	readDataFromRemoteProcess(re, &e, sizeof(e));

	return iRes;
}