#include "Grenade.h"

/*
1. init with starting x,y
2. setIsExploded
3. show
4. explode - one step at a time
*/

const int GRENADE_EXPLOSION_DISTANCE = 20;

Grenade::Grenade(double x, double y)
{
	int i;
	double angle,teta = 2*3.14/NUM_BULLETS;
	for (i = 0, angle = 0; i < NUM_BULLETS; i++,angle += teta) 
	{
		bullets[i] = new Bullet(x, y,angle);
	}
	isExploded = false;
}

Grenade::Grenade(double x, double y, int team)
{
	int i;
	double angle, teta = 2 * 3.14 / NUM_BULLETS;
	for (i = 0, angle = 0; i < NUM_BULLETS; i++, angle += teta)
	{
		bullets[i] = new Bullet(x, y, angle);
	}
	isExploded = false;
	team_color = team;
}

Grenade::~Grenade()
{
}

void Grenade::setIsExploded(bool state)
{
	isExploded = state;
	for (int i = 0; i < NUM_BULLETS; i++)
		bullets[i]->setIsFired(state);

}

bool Grenade::getIsExploded()
{
	for (int i = 0; i < NUM_BULLETS; i++) {
		if (bullets[i]->getIsFired()) {
			return true;
		}
	}
	return false;

}

int Grenade::move(int maze[MSZ][MSZ])
{
	return 1;
}

int Grenade::explode(int maze[MSZ][MSZ], int color, int enemyx, int enemyy)
{
	int res = WALL;
	int enemy_color = GREEN_SOLD == color ? RED_SOLD : GREEN_SOLD;
	for (int i = 0; i < NUM_BULLETS; i++) {
		if (bullets[i]->getIsFired()) {
			int tmp = bullets[i]->move(maze, color, enemyx, enemyy, true);
			if (tmp != SPACE && tmp != color &&
				tmp != AMMO && tmp != HEALTH) {
				res = tmp == enemy_color ? enemy_color : res;
				bullets[i]->setIsFired(false);
			}
		}
	}
	return res;
}

void Grenade::show()
{
	for (int i = 0; i < NUM_BULLETS; i++)
		bullets[i]->show();

}

void Grenade::SimulateExplosion(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], double damage)
{
	for (int i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i]->setIsFired(true);
		bullets[i]->SimulateFire( maze,  security_map,  damage);
	}
}
