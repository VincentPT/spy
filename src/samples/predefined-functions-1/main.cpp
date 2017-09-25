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

	UserSpyClient spyClient;
	if (spyClient.inject(argv[1]) == false) {
		cout << " failed to start monitor for process " << argv[1] << endl;
		return -1;
	}

	HMODULE hSpyLib;
	int ret = spyClient.loadPredefinedFunctions("spylib.dll", &hSpyLib);
	int loadedFunctionCount = GET_NUMBER_OF_LOAD_PREDEFINED_FUNC(ret);
	CustomCommandId commandBase = GET_BASE_OF_LOAD_PREDEFINED_FUNC(ret);

	cout << "number of loaded function " << loadedFunctionCount << endl;
	cout << "custom commands begin at " << commandBase << endl;
	
	if (hSpyLib != nullptr && spyClient.unloadModule(hSpyLib) != 0) {
		cout << "failed to uninject spylib.dll" << endl;
	}

	if (!spyClient.uninject()) {
		cout << "failed to stop monitor for process " << argv[1] << endl;
	}

	return 0;
}

