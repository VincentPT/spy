#pragma once
#include "SpyClient.h"
#include "spylib_interfaces.h"

#include <functional>
#include <string>

class UserSpyClient : public SpyClient {
	CustomCommandId _predefinedBase;
public:
	UserSpyClient();
	~UserSpyClient();

	void setPredefinedCommandIdBase(int predefinedBase);

	bool inject(const char* processName);
	std::string getInjectedProcessPath();
	bool readDummyTree(void* address, std::string& result);
	int showArguments(char a, short b, int c, __int64 d, float& e, double f, const DummyStruct& g);
};