#pragma once
#include "protocol.h"
#include <vector>

class NetworkConverter {
public:
	NetworkConverter();
	~NetworkConverter();

public:
	void AddBuffer(char* index, char* size, char* buf);

private:
	char* buffer;
};