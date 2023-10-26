//Server Port
constexpr int PORT_NUM = 9000;

//Object ID
constexpr int PLAYER 0;
constexpr int ENEMY 1;
constexpr int BULLET 2;
constexpr int MISSILE 3;
constexpr int TREE 4;

// Packet ID
// ----Server to Client
constexpr int SC_CREATE_OBJECT 0
constexpr int SC_MOVE_OBJECT 1
constexpr int SC_DELETE_OBJECT 2
constexpr int SC_COLLISION_OBJECT 3

// ----Client to Server
constexpr int CS_PLAYER_MOVE 0
constexpr int CS_PLAYER_SHOT 1


//Packet
#pragma pack (push, 1)

//Server to client
struct SC_CREATE_OBJECT_PACKET{
	int packet_type;
	int type;
	float pos; //must changed	
};

struct SC_MOVE_OBJECT_PACKET {
	int packet_type;
	int object_id;
	float pos; //must changed
};

struct SC_DELETE_OBJECT_PACKET {
	int packet_type;
	int object_id;
};

struct SC_COLLISION_OBJECT_PACKET {
	int packet_type;
	int object_id1;
	int object_id2;
};
//client to server
struct CS_PLAYER_MOVE_PACKET {
	int packet_type;
	int pressed_key; // must changed
};

struct CS_PLAYER_SHOT_PACKET {
	int packet_type;
	float degree; //must changed
}

#pragma pack (pop)