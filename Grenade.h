#pragma once

#include "Bullet.h"

const int NUM_BULLETS = 16;

class Grenade
{
private:
	Bullet* bullets[NUM_BULLETS];
	double x, y;
	bool isExploded;
	int team_color;
public:
	Grenade(double x, double y);
	Grenade(double x, double y, int team);
	~Grenade();
	int getTeam() { return team_color; };
	void setIsExploded(bool state);
	bool getIsExploded();
	int explode(int maze[MSZ][MSZ], int color = WALL, int enemyx = 0, int enemyy = 0);
	int move(int maze[MSZ][MSZ]);
	void show();
	void SimulateExplosion(int maze[MSZ][MSZ],double security_map[MSZ][MSZ],double damage);
};

