//Server Port
constexpr int PORT_NUM = 9000;

//Object ID
constexpr int PLAYER = 0;
constexpr int ENEMY = 1;
constexpr int BULLET = 2;
constexpr int MISSILE = 3;
constexpr int TREE = 4;

constexpr int TURRET = 0;
constexpr int BODY = 1;

//Key ID
constexpr int KEY_UP = 0;
constexpr int KEY_DOWN = 1;
constexpr int KEY_LEFT = 2;
constexpr int KEY_RIGHT = 3;
constexpr int KEY_SHOT = 4;

// Packet ID
// ----Server to Client
constexpr int SC_CREATE_OBJECT = 0;
constexpr int SC_MOVE_OBJECT = 1;
constexpr int SC_DELETE_OBJECT = 2;
constexpr int SC_COLLISION_OBJECT = 3;
constexpr int SC_PLAYER_ROLE = 4;

// ----Client to Server
constexpr int CS_PLAYER_MOVE = 0;
constexpr int CS_PLAYER_SHOT = 1;


//Packet
#pragma pack (push, 1)

struct sc_parent_paket {
	int packet_type;
};

//Server to client
struct sc_create_object_packet : public sc_parent_paket {
	int object_type;
	float pos; //must changed	
};

struct sc_move_object_packet : public sc_parent_paket {
	int object_id;
	float pos; //must changed
};

struct sc_delete_obejct_packet : public sc_parent_paket {
	int object_id;
};

struct sc_collision_object_packet : public sc_parent_paket {
	int object_id1;
	int object_id2;
};

struct sc_player_role_packet : public sc_parent_paket {
	int role;
}

//client to server
struct cs_player_move_packet : public sc_parent_paket {
	int pressed_key;
};

struct cs_player_shot_packet : public sc_parent_paket {
	float degree; //must changed
}

#pragma pack (pop)