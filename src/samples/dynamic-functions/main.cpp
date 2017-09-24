#include "UserSpyClient.h"

#include <iostream>
#include <filesystem>
#include <string>

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
	if (spyClient.startMonitorProcess(remoteProcessName) == false) {
		cout << " failed to start monitor for process " << remoteProcessName << endl;
		return -1;
	}

	const char* spyLib = "user-spy-lib.dll";
	// inject custom spy library by user spy client
	// the functions is defined inside loadDynamicFunctions method
	if (spyClient.loadDynamicFunctions(spyLib) == false) {
		spyClient.stopMonitorProcess();
		return -1;
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

