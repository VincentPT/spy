#pragma once
#include "spy_interfaces.h"
#include <functional>

template<class T>
class ScopeAutoFunction {
	T _fx;
public:
	
	ScopeAutoFunction(const T& fx) {
		_fx = fx;
	}
	
	ScopeAutoFunction(T&& fx) {
		_fx = fx;
	}

	~ScopeAutoFunction() {
		_fx();
	}
};

class SpyClient;

template <class T, class ...Args>
int readCustomObject(SpyClient* spyClient, CustomCommandId customCmdId, const std::function<void(T&)>& handler, Args...args) {
	constexpr int nParam = sizeof...(args);
	constexpr size_t paramSize = nParam * sizeof(void*);
	char commandRaw[sizeof(CustomCommandCmdData) + paramSize - sizeof(CustomCommandCmdData::params)];

	CustomCommandCmdData& customData = *(CustomCommandCmdData*)&commandRaw[0];
	customData.commandSize = sizeof(commandRaw);
	customData.commandId = CommandId::CUSTOM_COMMAND;
	customData.customCommandId = customCmdId;
	customData.paramCount = nParam;

	{
		void* params[] = { (void*)(size_t)args..., nullptr };
		memcpy_s(&customData.params[0], paramSize, &params[0], paramSize);
	}

	ReturnData& returnData = customData.returnData;
	int iRes;
	DWORD invokeResult;
	iRes = spyClient->sendCommandToRemoteThread((BaseCmdData*)&customData, true, &invokeResult);
	if (iRes != 0) {
		return iRes;
	}
	if (invokeResult != 0) {
		return invokeResult;
	}

	T rawData;
	iRes = spyClient->readCustomCommandResult(&returnData, (void**)&rawData);
	if (iRes != 0) {
		return iRes;
	}

	handler(rawData);

	if (rawData != nullptr) {
		free(rawData);
	}

	int freeBufferRes = spyClient->freeCustomCommandResult(&returnData);
	return iRes;
}


template <class T, class ...Args>
int readCustomObject2(SpyClient* spyClient, CustomCommandId customCmdId, const std::function<void(T&)>& handler, Args...args) {
	constexpr int nParam = sizeof...(args);
	constexpr size_t paramSize = nParam * sizeof(void*);
	char commandRaw[sizeof(CustomCommandCmdData) + paramSize - sizeof(CustomCommandCmdData::params)];

	CustomCommandCmdData& customData = *(CustomCommandCmdData*)&commandRaw[0];
	customData.commandSize = sizeof(commandRaw);
	customData.commandId = CommandId::CUSTOM_COMMAND;
	customData.customCommandId = customCmdId;
	customData.paramCount = nParam;

	{
		void* params[] = { (void*)(size_t)args..., nullptr };
		memcpy_s(&customData.params[0], paramSize, &params[0], paramSize);
	}

	ReturnData& returnData = customData.returnData;
	int iRes;
	DWORD invokeResult;
	iRes = spyClient->sendCommandToRemoteThread((BaseCmdData*)&customData, true, &invokeResult);
	if (iRes != 0) {
		return iRes;
	}
	if (invokeResult != 0) {
		return invokeResult;
	}

	T raw = (T)(size_t)returnData.customData;
	handler(raw);
	return iRes;
}