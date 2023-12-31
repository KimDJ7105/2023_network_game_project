#pragma once

constexpr int SERVERPORT = 9000;
constexpr int BUFSIZE = 256;

// Object ID
constexpr int PLAYER = 0;
constexpr int BULLET = 1;
constexpr int TERRAIN = 2;
constexpr int TREE = 3;

// Key ID
constexpr int KEY_UP = 0;
constexpr int KEY_DOWN = 1;
constexpr int MOUSE_UP = 2;
constexpr int MOUSE_DOWN = 3;

constexpr int KEY_W = 0;
constexpr int KEY_S = 1;
constexpr int KEY_A = 2;
constexpr int KEY_D = 3;
constexpr int KEY_SPACE = 4;

constexpr int MOUSE_LEFT = 10;
constexpr int MOUSE_RIGHT = 11;

constexpr int RECV_DONE = 100;

// Packet ID
constexpr int SC_CREATE_OBJECT = 0;
constexpr int SC_MOVE_OBJECT = 1;
constexpr int SC_DELETE_OBJECT = 2;
constexpr int SC_COLLISION_OBJECT = 3;
constexpr int CS_PLAYER_INPUT = 4;


#pragma pack(push, 1)
#include <D3Dcompiler.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
// 패킷 구조체

//s -> c
struct sc_create_object_packet {
	int object_type;
	XMFLOAT4X4 matrix;
};

struct sc_object_transform_packet {
	int object_id;
	XMFLOAT3 position;
	XMFLOAT3 rotate;
	XMFLOAT3 scale;
	XMFLOAT4X4 matrix;

	bool isActive;
};

struct sc_delete_object_packet {
	int object_id;

};

struct sc_collision_object_packet {
	int object_id1;
	int object_id2;
};

//c -> s
struct cs_player_input_packet {
	int input_event;
};

typedef struct event {
	int client_id;
	int event_id;
	int state;
	POINT mouseAxis;
} EVENT;

#pragma pack(pop)