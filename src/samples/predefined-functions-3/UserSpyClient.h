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

	bool startMonitorProcess(const char* processName);
	std::string getInjectedProcessPath();
	int getMinElmInRange(int min, int max);
};