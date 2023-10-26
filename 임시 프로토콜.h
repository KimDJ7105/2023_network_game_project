//포트 번호
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
constexpr int SC_OBJECT_MOVE 1
constexpr int SC_DELETE_OBJECT 2
constexpr int SC_COLIISION_OBJECT 3

// ----Client to Server
constexpr int CS_PLAYER_MOVE 0
constexpr int CS_PLAYER_SHOT 1

#pragma pack (push, 1)
struct NAME{
	//member
};
#pragma pack (pop)