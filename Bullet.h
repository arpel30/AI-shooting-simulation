#pragma once

#include "Definitions.h"

class Bullet
{
private:
	double x, y;
	double angle_direction;
	bool is_fired;
public:
	Bullet();
	Bullet(double xx, double yy);
	Bullet(double xx, double yy, double angle);
	~Bullet();
	int move(int maze[MSZ][MSZ], int color=WALL, int enemyx=0, int enemyy=0, bool grenade=false);
	int checkPath(int maze[MSZ][MSZ], int color, int* hitx, int* hity, int enemyx, int enemyy);
	void setIsFired(bool status) { is_fired = status; }
	void show();
	bool getIsFired() { return is_fired; }
	bool checkCollision(int maze[MSZ][MSZ], int color, int* enemy_color, int enemyx, int enemyy);
	void SimulateFire(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], double damage);
	
};

