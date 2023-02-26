#pragma once 

#include "Ammo.h"
#include "Health.h"
#include "Grenade.h"
#include "Bullet.h"
#include "Cell.h"
#include <vector>


using namespace std;

const int START_BULLETS = 10;
const int START_GRENADES = 3;
const int START_HEALTH = 100;
const int NUM_SOLDIERS = 3;

// for random behaviour
const int MIN_AMMO_VAL = 1;
const int MAX_AMMO_VAL = START_BULLETS / 2 + 2;
const int MIN_HEALTH_VAL = START_HEALTH * 0.1;
const int MAX_HEALTH_VAL = START_HEALTH * 0.8;

class Soldier
{
private:
	bool isInRoom;
	int locationX;
	int locationY;
	int health;
	int bullets_ammo;
	int grenade_ammo;
	int room_index;
	int color;
	int cooldown;

	int fight_or_flight; // if health is smaller - run
	int min_ammo;

	// squire
	bool squire;
	int carry_health;
	int health_needs[2];
	int ammo_needs[2];

	int destx, desty;
	int health_index, ammo_index, enemy_index, help_index;
	double shot_dist;

	Cell *start, *target;
	vector<Cell*> path;

	Bullet* pb;
	Grenade* pg;

public:
	Soldier();
	Soldier(int y, int x, int room, int team_color, bool lsquire=false);
	int GetLocationX();
	int GetLocationY();
	int GetAmmoIndex();
	int GetEnemyIndex();
	int GetHealthIndex();
	int GetDesty();
	int GetDestx();
	int GetHealth();
	int GetBulletsAmmo();
	int GetGrenadeAmmo();
	int GetColor();
	void Heal();
	Cell* GetStart();
	Cell* GetTarget();
	int GetRoomIndex();
	Bullet* GetBullet() { return pb; };
	Grenade* GetGrenade() { return pg; };
	int GetCarryHealth() { return carry_health; };
	bool IsSquire() { return squire; };
	void kill() { pb = nullptr; pg = nullptr; };

	void SetColor(int team_color);
	void SetRoomIndex(int index);
	void SetLocationX(int lx);
	void SetLocationY(int ly);
	void SetHealth(int health);
	void UpdateHealth(int health_points);
	void UpdateBulletsAmmo(int num_of_bullets);
	void UpdateGrenadeAmmo(int num_of_grenades);
	void UpdateNeeds(int index, int type, int amount);
	int CompareNeeds(Soldier* myTeam[3], int* soldier);
	int FindTargetPoint(Storage* storages[5], int size);
	void NotifySquire(Soldier* mySquire, int myIndex, int type);
	void MakeDecisionSoldier(Storage* health[5], Storage* ammo[5], Soldier* opponents[3], int size, int num_opponents, int maze[MSZ][MSZ], Soldier* mySquire, int myIndex);
	bool SoldierInRange(Soldier* sold, int y, int x);
	bool CheckSoldiers(Soldier* myTeam[3], int y, int x, int* index);
	void HelpSoldier(Soldier* myTeam[3], int y, int x);
	void MakeDecisionSquire(Storage* health[5], Storage* ammo[5], Soldier* opponents[3], int size, int num_opponents, int maze[MSZ][MSZ]);
	void MakeDecision(Storage* health[5], Storage* ammo[5], Soldier* opponents[3], int size, int num_opponents, int maze[MSZ][MSZ], Soldier* myTeam[3], int myIndex);
	void Fight(Soldier* opponents[3], int num_opponents, int maze[MSZ][MSZ]);
	bool CheckPath(Soldier* opponents[3], int num_opponents, int maze[MSZ][MSZ]);
	bool IsClear(Soldier* opponent, int maze[MSZ][MSZ], int enemy_dist);
};
