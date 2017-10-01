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
	const char* spyLib = "spylib2.dll";
	ModuleId moduleId = INVALID_MODULE_ID;
	int ret = spyClient.loadPredefinedFunctions(spyLib, &moduleId);
	if (moduleId == INVALID_MODULE_ID) {
		spyClient.uninject();
		cout << "failed to inject predefined spy lib" << endl;
		return -1;
	}

	int loadedFunctionCount = GET_NUMBER_OF_LOAD_PREDEFINED_FUNC(ret);
	CustomCommandId commandBase = GET_BASE_OF_LOAD_PREDEFINED_FUNC(ret);

	// print the spy results
	cout << "number of loaded function " << loadedFunctionCount << endl;
	cout << "custom commands begin at " << commandBase << endl;

	// store command id base and use it to invoke inside the UserSpyClient methods
	spyClient.setPredefinedCommandIdBase(commandBase);

	// call the first command in remote process
	string treeStr;
	bool blRes = spyClient.readDummyTree(nullptr, treeStr);
	if (blRes) {
		cout << "Dummy tree: " << treeStr << endl;
	}

	// call the second command in remote process
	string injectedModulePath = spyClient.getInjectedProcessPath();
	cout << "injected process path: " << injectedModulePath << endl;

	// call the third command in remote process
	float e = 4.0f;
	if (spyClient.showArguments(0, 1, 2, 3, e, 5.0f, { 6, 7 })) {
		cout << "showArguments returns an error" << endl;
	}
	else {
		cout << "f was changed to " << e << endl;
	}

	// uninject the custom lib
	if (spyClient.unloadModule(moduleId) != 0) {
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

