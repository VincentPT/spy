#pragma once
#include "SpyClient.h"
#include "spylib_interfaces.h"

#include <functional>

class UserSpyClient : public SpyClient {
public:
	UserSpyClient();
	~UserSpyClient();

	bool inject(const char* processName);
};