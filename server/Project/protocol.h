#pragma once

constexpr int SERVERPORT = 9000;

// Object ID
constexpr int PLAYER = 0;
constexpr int BULLET = 1;

// Key ID
constexpr int KEY_UP = 0;
constexpr int KEY_DOWN = 1;
constexpr int KEY_LEFT = 2;

// Packet ID
constexpr int SC_CREATE_OBJECT = 0;
constexpr int SC_MOVE_OBJECT = 1;
constexpr int SC_DELETE_OBJECT = 2;
constexpr int SC_COLLISION_OBJECT = 3;


#pragma pack(push, 1)

// 패킷 구조체
struct sc_parent_packet {
	int packet_type;
};

struct sc_create_object_packet : public sc_parent_packet {
	int object_type;
	XMVECTOR pos;
};

struct sc_move_object_packet : public sc_parent_packet {
	int object_id;
	XMVECTOR pos;
};

struct sc_delete_object_packet : public sc_parent_packet {
	int object_id;
};

struct sc_player_role_packet : public sc_parent_packet {
	int role;
};

struct cs_player_move_packet : public sc_parent_packet {
	int input_event;
};

struct sc_collision_object_packet : public sc_parent_packet {
	int object_id1;
	int object_id2;
};

#pragma pack(pop)