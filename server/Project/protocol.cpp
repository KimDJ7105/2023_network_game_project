#include "stdafx.h"

sc_create_object_packet::sc_create_object_packet() : sc_parent_packet(0) 
{ 
	object_type = -1; matrix = Matrix4x4::Identity(); 
}

sc_create_object_packet::sc_create_object_packet(int type, XMFLOAT4X4 _matrix) : sc_parent_packet(0), object_type(type), matrix(_matrix)
{

}

sc_object_transform_packet::sc_object_transform_packet() : sc_parent_packet(1) 
{
	object_id = -1; matrix = Matrix4x4::Identity(); 
}

sc_delete_object_packet::sc_delete_object_packet(int id) : sc_parent_packet(2), object_id(id)
{
}
