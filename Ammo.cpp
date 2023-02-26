#include "Ammo.h"
#include "Storage.h"
#include <stdlib.h>

/* ---------- Constructors ---------- */
/**/
Ammo::Ammo()
{
	x = 0;
	y = 0;
	room_number = 0;
	cooldown = COOLDOWN_TIME;
}

Ammo::Ammo(int lx, int ly, int room_number_location)
{
	x = lx;
	y = ly;
	room_number = room_number_location;
	cooldown = COOLDOWN_TIME;
}
