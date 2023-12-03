#include "NetworkConverter.h"

NetworkConverter::NetworkConverter()
{
}

NetworkConverter::~NetworkConverter()
{
}

void NetworkConverter::AddBuffer(char* index, char* size, char* buf)
{
	//buffer.insert(buffer.end(), index, index + std::strlen(index));
	//buffer.insert(buffer.end(), size, size + std::strlen(size));
	//buffer.insert(buffer.end(), buf, buf + std::strlen(buf));

	memcpy(buffer + std::strlen(buffer), index, sizeof(int));
	memcpy(buffer + std::strlen(buffer), size, sizeof(int));
	memcpy(buffer + std::strlen(buffer), buf, std::strlen(buf));
}
