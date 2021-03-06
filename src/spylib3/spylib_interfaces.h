#pragma once

// user enum's declaration
enum class UserCommandId : unsigned short {
	ReadDummyTree = 0,
    GetInjectedProcessName,
	GetMinElmInRange,
	
    // the user command ids must be defined before PredefinedCommandCount
	PredefinedCommandCount
};

enum class ReturnCode : int {
	Success = 0,
	Fail
};


// user structure's declaration
struct RawBuffer {
	unsigned int size;
	char data[1];
};
