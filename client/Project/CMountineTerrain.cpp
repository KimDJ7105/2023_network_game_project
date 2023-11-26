#include "stdafx.h"
#include "CMountineTerrain.h"
#include "SyncObject.h"

CMountineTerrain::CMountineTerrain() : CTerrain("Image/terrain.raw", TERRAIN)
{
	AddComponet(new SyncObject(this));
}

CMountineTerrain::~CMountineTerrain()
{
}
