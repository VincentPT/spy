#include "UserSpyClient.h"

#include <iostream>
#include <filesystem>
#include <string>
#include <iomanip>

using namespace std;
using namespace std::experimental::filesystem::v1;


int main(int argc, char* argv[]) {
	if (argc < 2) {
		path currentProcessPath(argv[0]);
		cout << "Usage " << currentProcessPath.filename().string() << " <a running processname.exe>" << endl;
		return -1;
	}
	// spy the target process
	const char* remoteProcessName = argv[1];
	UserSpyClient spyClient;
	if (spyClient.inject(remoteProcessName) == false) {
		cout << " failed to start monitor for process " << remoteProcessName << endl;
		return -1;
	}

	const char* spyLib = "user-spy-lib.dll";
	ModuleId moduleId;
	// inject custom spy library by user spy client
	// the functions is defined inside loadDynamicFunctions method
	if (spyClient.loadDynamicFunctions(spyLib, moduleId) == false) {
		spyClient.uninject();
		return -1;
	}

	// show module info after injection
	{
		// show module path
		string modulePath;
		HMODULE hModule;
		int iRes = spyClient.getModulePath(moduleId, modulePath, &hModule);
		if (iRes == 0) {
			cout << "getModulePath return: modulePath = " << modulePath << endl;
			cout << "getModulePath return: hModule = 0x" << std::setfill('0') << std::setw(sizeof(HMODULE) * 2)
				<< std::hex << hModule << endl;
		}
		else {
			cout << "getModulePath return an error(" << iRes << ")" << endl;
		}

		// show module commmands
		list<CustomCommandId> cmdIds;
		spyClient.getModuleData(moduleId, cmdIds, nullptr);
		if (iRes == 0) {
			void* ptr;
			cout << "getModuleData return:" << endl;
			for (auto cmdId : cmdIds) {
				iRes = spyClient.getFunctionPtr(cmdId, &ptr);
				if (iRes == 0) {
					cout << "    " << cmdId << " <--> " << std::setfill('0') << std::setw(sizeof(ptr) * 2) << std::hex << (size_t)ptr << endl;
				}
				else {
					cout << "    " << cmdId << " --> error!!! " << endl;
				}
			}
		}
		else {
			cout << "getModuleData return an error(" << iRes << ")" << endl;
		}
	}


	// call the first command in remote process
	string injectedModulePath = spyClient.getInjectedProcessPath();
	cout << "injected process path: " << injectedModulePath << endl;

	// prepare parameter for second command
	cout << "Now, try to find a min element in a range from an array in the remote process" << endl;
	cout << "input a range [min, max]" << endl;

	int min, max;
	cout << "min:";
	cin >> min;
	cout << "max:";
	cin >> max;

	// call the second command in remote process
	int val = spyClient.getMinElmInRange(min, max);
	if (val > max) {
		cout << "getMinElmInRange return an error" << endl;
	}
	else {
		cout << "min element in range [" << min << ", " << max << "] is " << val << endl;
	}	

	spyClient.unloadModulesAndStopMonitor();

	return 0;
}

