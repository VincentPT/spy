#pragma once

// user enum's declaration
enum class UserCommandId : unsigned short {
	//ReadDummyTree = 0,
	
	// the user command ids must be defined before PredefinedCommandCount
	PredefinedCommandCount
};

// user structure's declaration

struct RawBuffer {
	unsigned int size;
	char data[1];
};