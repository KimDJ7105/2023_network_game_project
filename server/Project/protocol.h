#pragma once

constexpr int SERVERPORT = 9000;
constexpr int BUFSIZE = 256;

// Object ID
constexpr int PLAYER = 0;
constexpr int BULLET = 1;
constexpr int TERRAIN = 2;

// Key ID
constexpr int KEY_UP = 0;
constexpr int KEY_DOWN = 1;
constexpr int KEY_LEFT = 2;
constexpr int KEY_RIGHT = 3;

constexpr int MOUSE_LEFT = 11;

// Packet ID
constexpr int SC_CREATE_OBJECT = 0;
constexpr int SC_MOVE_OBJECT = 1;
constexpr int SC_DELETE_OBJECT = 2;
constexpr int SC_COLLISION_OBJECT = 3;
constexpr int CS_PLAYER_INPUT = 4;


#pragma pack(push, 1)

// 패킷 구조체

//s -> c
struct sc_create_object_packet {
	int object_type;
	XMFLOAT4X4 matrix;

	sc_create_object_packet();
	sc_create_object_packet(int type, XMFLOAT4X4 _matrix);
};

struct sc_object_transform_packet {
	int object_id;
	XMFLOAT4X4 matrix;

	sc_object_transform_packet();
	sc_object_transform_packet(int id, XMFLOAT4X4 m) : object_id(id), matrix(m) {}
};

struct sc_delete_object_packet {
	int object_id;

	sc_delete_object_packet(int id = -1);
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
	int event_id;
	int client_id;
	POINT mouseAxis;
} EVENT;

#pragma pack(pop)