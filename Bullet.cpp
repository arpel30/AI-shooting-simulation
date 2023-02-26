#include "Bullet.h"
#include <stdlib.h>
#include <math.h>
#include "glut.h"
#include <iostream>

using namespace std;
const double BULLET_SIZE = 0.5;
Bullet::Bullet()
{
	angle_direction = (rand() % 360)*3.14 / 180; // in radians
	is_fired = false;
}

Bullet::Bullet(double xx, double yy)
{
	angle_direction = (rand() % 360)*3.14 / 180; // in radians
	is_fired = false;
	x = xx + BULLET_SIZE;
	y = yy + BULLET_SIZE;

}
//atan math.h
Bullet::Bullet(double xx, double yy, double angle)
{
	angle_direction = angle; // in radians
	is_fired = false;
	x = xx;
	y = yy;

}


Bullet::~Bullet()
{
}

bool Bullet::checkCollision(int maze[MSZ][MSZ], int color, int* enemy_color, int enemyx, int enemyy) {
	*enemy_color = GREEN_SOLD == color ? RED_SOLD : GREEN_SOLD;
	if((x <= enemyx + 1.5 && x >= enemyx - 1.5) && (y <= enemyy + 1.5 && y >= enemyy - 1.5))
		return true;
	return false;
}

int Bullet::move(int maze[MSZ][MSZ], int color, int enemyx, int enemyy, bool grenade)
{
	// check that current x,y is in SPACE otherwise stop moving bullet
	
	int enemy_color, tmp;
	if (checkCollision(maze, color, &enemy_color, enemyx, enemyy)) {
		if(!grenade)
			is_fired = false;
		return enemy_color;
	}
	if (maze[(int)y][(int)x] == SPACE || maze[(int)y][(int)x] == color ||
		maze[(int)y][(int)x] == AMMO || maze[(int)y][(int)x] == HEALTH)
	{
		tmp = maze[(int)y][(int)x];
		int iter = 2000;
		double speed = 2;
		double dx, dy;
		for (double i = 0; i < speed; i += speed / iter)
		{
			dx = cos(angle_direction);
			dy = sin(angle_direction);
			// update x,y
			x += dx * speed / iter;
			y += dy * speed / iter;
		}
	}
	else
	{
		if (!grenade)
			is_fired = false;
		return maze[(int)y][(int)x];
	}

	return tmp;
}

int Bullet::checkPath(int maze[MSZ][MSZ], int color, int* hitx, int* hity, int enemyx, int enemyy)
{
	// check that current x,y is in SPACE otherwise stop return what we hit
	while (maze[(int)y][(int)x] == SPACE || maze[(int)y][(int)x] == color || 
		maze[(int)y][(int)x] == AMMO || maze[(int)y][(int)x] == HEALTH)
	{
		double speed = 0.001;
		double dx, dy;
		dx = cos(angle_direction);
		dy = sin(angle_direction);
		// update x,y
		x += dx * speed;
		y += dy * speed;
		int enemy_color;
		if (checkCollision(maze, color, &enemy_color, enemyx, enemyy))
			return enemy_color;
	}
	*hitx = (int)x;
	*hity = (int)y;
	return maze[(int)y][(int)x];


}

void Bullet::show()
{
	glColor3d(0, 0, 0); // black
	glBegin(GL_POLYGON);
		glVertex2d(x, y + BULLET_SIZE);
		glVertex2d(x+ BULLET_SIZE, y );
		glVertex2d(x, y - BULLET_SIZE);
		glVertex2d(x- BULLET_SIZE, y );
	glEnd();
	
}

void Bullet::SimulateFire(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], double damage)
{
	while (is_fired) 
	{
		if (maze[(int)y][(int)x] == WALL)
			is_fired = false;
		else
		{
			security_map[(int)y][(int)x] += damage;
			if (security_map[(int)y][(int)x] > 1) {
				security_map[(int)y][(int)x] = 1;
			}
			double speed = 0.1;
			double dx, dy;
			dx = cos(angle_direction);
			dy = sin(angle_direction);
			// update x,y
			x += dx * speed;
			y += dy * speed;

		}

	}
}
