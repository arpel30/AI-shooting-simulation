#include "Health.h"
#include "Storage.h"
#include <stdlib.h>

Health::Health()
{

}

Health::Health(int lx, int ly, int rnumber) {
	x = lx;
	y = ly;
	room_number = rnumber;
	cooldown = COOLDOWN_TIME;
}

