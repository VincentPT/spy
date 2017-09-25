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
	if (spyClient.inject(remoteProcessName) == false) {
		cout << " failed to start monitor for process " << remoteProcessName << endl;
		return -1;
	}

	// inject custom spy library which implement predefined functions
	const char* spyLib = "spylib3.dll";
	HMODULE hSpyLib;
	int ret = spyClient.loadPredefinedFunctions(spyLib, &hSpyLib);
	int loadedFunctionCount = GET_NUMBER_OF_LOAD_PREDEFINED_FUNC(ret);
	CustomCommandId commandBase = GET_BASE_OF_LOAD_PREDEFINED_FUNC(ret);

	// print the spy results
	cout << "number of loaded function " << loadedFunctionCount << endl;
	cout << "custom commands begin at " << commandBase << endl;

	// store command id base and use it to invoke inside the UserSpyClient methods
	spyClient.setPredefinedCommandIdBase(commandBase);

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
	
	// uninject the custom lib
	if (spyClient.unloadModule(hSpyLib) != 0) {
		cout << "failed to uninject " << spyLib << endl;
	}
	else {
		cout << "uninjected module from remote process " << spyLib << endl;
	}

	// stop the monitor target process and also uninject spy engine dll from the process
	if (!spyClient.uninject()) {
		cout << "failed to stop monitor for process " << argv[1] << endl;
	}
	else {
		cout << "stop monitor process " << remoteProcessName << " successfully" << endl;
	}

	return 0;
}

