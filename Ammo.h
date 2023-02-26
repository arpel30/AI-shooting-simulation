#pragma once
#include "Storage.h"
const int BULLETS_STOCK = 5;
const int GRENADE_STOCK = 1;

class Ammo : public Storage
{

public:
	Ammo();
	Ammo(int lx, int ly, int lroom_number);

};