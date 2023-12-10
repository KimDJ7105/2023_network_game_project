#pragma once
#include "protocol.h"
#include <vector>

class NetworkConverter {
public:
	NetworkConverter() {
		buffer = nullptr;
		bufferSize = 0;
	}
	~NetworkConverter() {
		delete buffer;
	}

public:

	void AddBufferIndex(char* indexBuf, int size){AddBuffer(indexBuf, size);}
	void AddBufferSize(char* sizeBuf, int size) { AddBuffer(sizeBuf, size); }

	template<class T>
	void AddBuffer(const vector<T>& list)
	{
		int size = list.size();
		AddBufferSize((char*)&size);
		for (auto v : list)
			AddBuffer((char*)&v, sizeof(T));
	}
	
	void AddBuffer(char* buf, int size)
	{
		char* resultStore;
		if (buffer != nullptr)
		{
			resultStore = new char[bufferSize + size];
			memcpy(resultStore, buffer, bufferSize);
			memcpy(resultStore + std::strlen(buffer), buf, size);
			delete buffer;
			buffer = resultStore;
		}
		else
			buffer = buf;
		bufferSize += size;

	}
	char* GetBuffer() { return buffer; }
	int GetBufferSize() { return bufferSize; }

public:

	void Send(SOCKET sock)
	{
		send(sock, (char*)&bufferSize, sizeof(int), 0);
		send(sock, buffer, bufferSize, 0);
	}

	void Recv(SOCKET sock)
	{
		recv(sock, (char*)&bufferSize, sizeof(int), 0);
		delete buffer;
		buffer = new char[bufferSize];
		recv(sock, buffer, bufferSize, 0);
	}

public:
	template<class PTOROCOL>
	vector<PTOROCOL>* ReadBuffer()
	{
		if (bufferSize <= 0) return nullptr;

		int index = -1;
		int size = -1;
		char* pack;

		memcpy((char*)&index, buffer, sizeof(int));
		memcpy((char*)&size, buffer + (int)sizeof(int), sizeof(int));
		pack = new char[size];
		memcpy(pack, buffer + (int)sizeof(int) * 2, size);

		switch (index)
		{
		case SC_CREATE_OBJECT:
			return &GetPackListToBuffer<sc_create_object_packet>(pack, size);
		default:
			return nullptr;
		}
	}

private:
	template<class PROTOCOL>
	vector<PROTOCOL> GetPackListToBuffer(char* buf, int size)
	{
		vector<PROTOCOL> packList;
		packList.insert(packList.end(), reinterpret_cast<PROTOCOL*>(buf), reinterpret_cast<PROTOCOL*>(buf + size));
		return packList;
	}

private:
	char* buffer;
	int bufferSize = 0;
};