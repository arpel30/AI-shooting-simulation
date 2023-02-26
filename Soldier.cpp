#include "Soldier.h"
#include <stdlib.h>
#include <iostream>
#include "Room.h"
#include "glut.h"
#include "Grenade.h"
#include "Bullet.h"
#include <time.h>

const int SHOOTING_COOLDOWN = 20;
const int GRENADE_DISTANCE = 10; //10
const int BULLET_DISTANCE = 20; //5
const int BULLET_BASE_DAMAGE = 60;
const int BULLET_DISTANCE_DAMAGE = 50;
const int GRENADE_DISTANCE_DAMAGE = 20;
const int GRENADE_BASE_DAMAGE = 40;
/* ---------- Constructors ---------- */

Soldier::Soldier()
{
	locationX = 0;
	locationY = 0;
	health = START_HEALTH;
	bullets_ammo = START_BULLETS;
	grenade_ammo = START_GRENADES;
	isInRoom = true;
	room_index = 0;
	color = SPACE;
	start = nullptr;
	target = nullptr;
	pb = nullptr;
	pg = nullptr;
	//srand(time(NULL));
}

Soldier::Soldier(int y, int x, int room, int team_color, bool lsquire)
{
	locationX = x;
	locationY = y;
	health = START_HEALTH;
	if (lsquire) {
		bullets_ammo = 0;
		grenade_ammo = 0;
	}
	else {
		bullets_ammo = START_BULLETS;
		grenade_ammo = START_GRENADES;
	}
	
	isInRoom = true;
	room_index = room;
	color = team_color;
	start = nullptr;
	target = nullptr;
	pb = nullptr;
	pg = nullptr;
	cooldown = 0;
	squire = lsquire;
	if (squire) {
		ammo_needs[0] = 0;
		ammo_needs[1] = 0;
		health_needs[0] = 0;
		health_needs[1] = 0;
	}

	// generate random behaviour
	fight_or_flight = MIN_HEALTH_VAL + rand() % (MAX_HEALTH_VAL - MIN_HEALTH_VAL);
	min_ammo = MIN_AMMO_VAL + rand() % (MAX_AMMO_VAL - MIN_AMMO_VAL);
	//cout << "min_ammo: " << min_ammo << ", fight_or_flight: " << fight_or_flight << "\n";
}



/* ---------- Getters ---------- */

int Soldier::GetLocationX()
{
	return locationX;
}

int Soldier::GetLocationY()
{
	return locationY;
}

int Soldier::GetDesty()
{
	return desty;
}

int Soldier::GetDestx()
{
	return destx;
}

int Soldier::GetAmmoIndex()
{
	return ammo_index;
}

int Soldier::GetHealthIndex()
{
	return health_index;
}

int Soldier::GetEnemyIndex()
{
	return enemy_index;
}

int Soldier::GetHealth()
{
	return health;
}

int Soldier::GetBulletsAmmo()
{
	return bullets_ammo;
}

int Soldier::GetGrenadeAmmo()
{
	return grenade_ammo;
}

int Soldier::GetColor()
{
	return color;
}

Cell* Soldier::GetStart()
{
	return start;
}

Cell* Soldier::GetTarget()
{
	cout << "target: " << target->getCol() << "," << target->getRow() << "\n";
	return target;
}

int Soldier::GetRoomIndex()
{
	return room_index;
}

/* ---------- Setters ---------- */

void Soldier::SetColor(int team_color)
{
	color = team_color;
}

void Soldier::SetRoomIndex(int index)
{
	room_index = index;
}

void Soldier::SetLocationX(int lx)
{
	locationX = lx;
}

void Soldier::SetLocationY(int ly)
{
	locationY = ly;
}

void Soldier::SetHealth(int health)
{
	this->health = health;
}

void Soldier::Heal() {
	if (carry_health <= 0)
		return;
	int missing_health = START_HEALTH - health;
	int healing = missing_health < carry_health ? missing_health : carry_health;
	health += healing;
	carry_health -= healing;
	cout << "team " << color << ": squire healed himself with " << healing << " health.\n";
}

void Soldier::UpdateHealth(int health_points)
{

	if (health_points > 0 && !squire) {
		cout << "team " << color << ": soldier healed with " << health_points << " health.\n";
	}
	else if (health_points < 0 && !squire)
	{
		cout << "team " << color << ": soldier took hit of " << health_points << " health.\n";
	}
	else if(health_points < 0 && squire){
		cout << "team " << color << ": squire took hit of " << health_points << " health.\n";
	}

	if (squire && health_points > 0) {
		carry_health += health_points;
		if (carry_health > START_HEALTH) {
			health += carry_health - START_HEALTH;
			carry_health = START_HEALTH;
		}
	}
	else {
		health += health_points;
	}
	if (health > START_HEALTH)
		health = START_HEALTH;
	if (health <= 0) {
		health = 0;
		if(squire)
			cout << "team " << color << ": squire is dead.\n";
		else
			cout << "team " << color << ": soldier is dead.\n";
	}
}

void Soldier::UpdateBulletsAmmo(int num_of_bullets)
{
	bullets_ammo += num_of_bullets;
	if (bullets_ammo > START_BULLETS)
		bullets_ammo = START_BULLETS;
}

void Soldier::UpdateGrenadeAmmo(int num_of_grenades)
{
	grenade_ammo += num_of_grenades;
	if (grenade_ammo > START_GRENADES)
		grenade_ammo = START_GRENADES;
}

void Soldier::UpdateNeeds(int index, int type, int amount)
{
	switch (type)
	{
	case AMMO:
		ammo_needs[index] = amount;
		break;
	case HEALTH:
		health_needs[index] = amount;
		break;
	}
}



/* ---------- Game Functions ---------- */

/* Find the closer ammo or health point to soldier */
int Soldier::FindTargetPoint(Storage* storages[5], int size)
{
	/* Calculate the heuristic of the two options */
	int i, closestIndex=-1;
	double tmp, h = MSZ*5;
	for (i = 0; i < size; i++)
	{
		tmp = sqrt(pow(((double)locationY - (double)storages[i]->GetY()), 2) + pow((double)locationX - (double)storages[i]->GetX(), 2));
		if (tmp < h && !storages[i]->isEmpty()) {
			closestIndex = i;
			h = tmp;
		}
	}
	if (closestIndex >= 0) {
		destx = storages[closestIndex]->GetX();
		desty = storages[closestIndex]->GetY();
	}
	return closestIndex;
}

void Soldier::NotifySquire(Soldier* mySquire, int myIndex, int type)
{
	int amount = 0;
	switch (type)
	{
	case HEALTH:
		amount = START_HEALTH - health;
		break;
	case AMMO:
		amount = START_BULLETS - bullets_ammo;
		break;
	}
	mySquire->UpdateNeeds(myIndex, type, amount);
}

void Soldier::MakeDecisionSoldier(Storage* healths[5], Storage* ammos[5], Soldier* opponents[3], int size, int num_opponents, int maze[MSZ][MSZ], Soldier* mySquire, int myIndex)
{
	if (cooldown > 0)
		cooldown--;
	if (pb) {
		int hit = pb->move(maze, color, opponents[enemy_index]->GetLocationX(), opponents[enemy_index]->GetLocationY());
		pb->show();
		if (hit == opponents[enemy_index]->GetColor()) {
			opponents[enemy_index]->UpdateHealth(-(BULLET_DISTANCE_DAMAGE / shot_dist + BULLET_BASE_DAMAGE));
		}
		if (!pb->getIsFired())
			pb = nullptr;
	}
	if (pg) {
		int hit = pg->explode(maze, color, opponents[enemy_index]->GetLocationX(), opponents[enemy_index]->GetLocationY());
		pg->show();
		if (hit == opponents[enemy_index]->GetColor()) {
			opponents[enemy_index]->UpdateHealth(-(GRENADE_DISTANCE_DAMAGE / shot_dist + GRENADE_BASE_DAMAGE));
		}
		if (!pg->getIsExploded())
			pg = nullptr;
	}

	if (health <= fight_or_flight) {
		if (mySquire->GetHealth() > 0) {
			NotifySquire(mySquire, myIndex, HEALTH);
			destx = mySquire->GetLocationX();
			desty = mySquire->GetLocationY();
		}
		else {
			health_index = FindTargetPoint(healths, size);
			if (health_index < 0) {
				Fight(opponents, num_opponents, maze);
			}
		}
	}
	else if (bullets_ammo <= min_ammo) {
		
		if (mySquire->GetHealth() > 0) {
			NotifySquire(mySquire, myIndex, AMMO);
			destx = mySquire->GetLocationX();
			desty = mySquire->GetLocationY();
		}
		else {
			ammo_index = FindTargetPoint(ammos, size);
			if (ammo_index < 0) {
				Fight(opponents, num_opponents, maze);
			}
		}
	}
	else {
		Fight(opponents, num_opponents, maze);
	}
}

int Soldier::CompareNeeds(Soldier* myTeam[3], int* soldier) {
	if (health_needs[0] >= health_needs[1] && health_needs[0] > 0) {
		*soldier = 0;
		return HEALTH;
	}
	else if (health_needs[0] < health_needs[1]) {
		*soldier = 1;
		return HEALTH;
	}
	else {
		if (ammo_needs[0] >= ammo_needs[1] && ammo_needs[0] > 0) {
			*soldier = 0;
			return AMMO;
		}
		else if (ammo_needs[0] < ammo_needs[1]) {
			*soldier = 1;
			return AMMO;
		}
		else {
			return -1;
		}
	}
}

bool Soldier::SoldierInRange(Soldier* sold, int y, int x)
{
	int soldx = sold->GetLocationX();
	int soldy = sold->GetLocationY();
	return (soldx >= x-5 && soldx <= x+5 && 
		soldy >= y - 5 && soldy <= y + 5);
}

bool Soldier::CheckSoldiers(Soldier* myTeam[3], int y, int x, int* index)
{
	if (SoldierInRange(myTeam[0], y, x) && myTeam[0]->GetHealth() > 0) {
		*index = 0;
		return true;
	}

	if (SoldierInRange(myTeam[1], y, x) && myTeam[1]->GetHealth() > 0) {
		*index = 1;
		return true;
	}
	*index = -1;
	return false;
}

void Soldier::HelpSoldier(Soldier* myTeam[3], int y, int x)
{
	int current_index;
	if (help_index == -1) {
		return;
	}
	else if(CheckSoldiers(myTeam, y, x, &current_index))
	{
		int bullet_amount = START_BULLETS - myTeam[current_index]->GetBulletsAmmo();
		int grenades_amount = START_GRENADES - myTeam[current_index]->GetGrenadeAmmo();
		int health_amount = START_HEALTH - myTeam[current_index]->GetHealth();
		
		bullet_amount = bullets_ammo > bullet_amount ? bullet_amount : bullets_ammo;
		grenades_amount = grenade_ammo > grenades_amount ? grenades_amount : grenade_ammo;
		health_amount = carry_health > health_amount ? health_amount : carry_health;
		
		myTeam[current_index]->UpdateBulletsAmmo(bullet_amount);
		myTeam[current_index]->UpdateGrenadeAmmo(grenades_amount);
		myTeam[current_index]->UpdateHealth(health_amount);

		bullets_ammo -= bullet_amount;
		grenade_ammo -= grenades_amount;
		carry_health -= health_amount;
		if(!(health_amount == 0 && bullet_amount == 0))
			cout << "team " << color << ": squire helped " << current_index << " with: " << bullet_amount << " bullets & " << health_amount << " health.\n";
	}
}

void Soldier::MakeDecisionSquire(Storage* healths[5], Storage* ammos[5], Soldier* myTeam[3], int size, int num_opponents, int maze[MSZ][MSZ])
{
	int soldier=-1;
	HelpSoldier(myTeam, desty, destx);
	if (health < START_HEALTH/2) {
		health_index = FindTargetPoint(healths, size);
		Heal();
	}
	switch (CompareNeeds(myTeam, &soldier)) {
	case AMMO:
		if(ammo_needs[soldier] > bullets_ammo) // collect more ammo
			ammo_index = FindTargetPoint(ammos, size);
		else { // go give tyhe soldier ammo
			destx = myTeam[soldier]->GetLocationX();
			desty = myTeam[soldier]->GetLocationY();
		}
		break;

	case HEALTH:
		if (health_needs[soldier] > carry_health) // collect more health
			health_index = FindTargetPoint(healths, size);
		else { // go give tyhe soldier health
			destx = myTeam[soldier]->GetLocationX();
			desty = myTeam[soldier]->GetLocationY();
		}
		break;
	case -1:
		health_index = FindTargetPoint(healths, size);
		if (carry_health > 80 || health_index == -1) { // if no health storages or we already collected enough
			ammo_index = FindTargetPoint(ammos, size);
		}
		break;
	}
	if (health < START_HEALTH)
		Heal();
	help_index = soldier;
}

void Soldier::MakeDecision(Storage* healths[5], Storage* ammos[5], Soldier* opponents[3], int size, int num_opponents, int maze[MSZ][MSZ], Soldier* myTeam[3], int myIndex)
{
	if (squire) {
		destx = locationX;
		desty = locationY;
		MakeDecisionSquire(healths, ammos, myTeam, size, num_opponents, maze);
	}
	else {
		MakeDecisionSoldier(healths, ammos, opponents, size, num_opponents, maze, myTeam[2], myIndex);
	}
}

void Soldier::Fight(Soldier* opponents[3], int num_opponents, int maze[MSZ][MSZ])
{
	/* Calculate the heuristic of the two options */
	int i, closestIndex = -1;
	double tmp, h = MSZ * 5;
	for (i = 0; i < num_opponents; i++)
	{
		tmp = sqrt(pow(((double)locationY - (double)opponents[i]->GetLocationY()), 2) + pow((double)locationX - (double)opponents[i]->GetLocationX(), 2));
		if (tmp < h && opponents[i]->GetHealth()>0) {
			closestIndex = i;
			h = tmp;
		}
	}
	if (!CheckPath(opponents, num_opponents, maze)) {
		destx = opponents[closestIndex]->GetLocationX();
		desty = opponents[closestIndex]->GetLocationY();
	}
	else {
		destx = locationX;
		desty = locationY;
		float radians = atan2(-locationY + opponents[closestIndex]->GetLocationY()+0.5, -locationX + opponents[closestIndex]->GetLocationX() + 0.5);
		if (h < GRENADE_DISTANCE && grenade_ammo > 0) { // && h > BULLET_DISTANCE
			// add && !pg to if 
			if (cooldown == 0) {
				pg = new Grenade(locationX, locationY, color);
				enemy_index = closestIndex;
				grenade_ammo--;
				shot_dist = h + 0.5;
				pg->setIsExploded(true);
				cooldown = SHOOTING_COOLDOWN + rand() % (SHOOTING_COOLDOWN/3);
			}
		}
		else {
			if (!pb && cooldown == 0 && bullets_ammo > 0) {
				pb = new Bullet(locationX, locationY, radians);
				enemy_index = closestIndex;
				bullets_ammo--;
				shot_dist = h + 0.5;
				pb->setIsFired(true);
				cooldown = SHOOTING_COOLDOWN + rand() % (SHOOTING_COOLDOWN / 3);
			}
		}
	}
}

bool Soldier::CheckPath(Soldier* opponents[3], int num_opponents, int maze[MSZ][MSZ])
{
	int i, closestIndex = -1;
	double tmp, h = MSZ * 5;
	for (i = 0; i < num_opponents; i++)
	{
		tmp = sqrt(pow(((double)locationY - (double)opponents[i]->GetLocationY()), 2) + pow((double)locationX - (double)opponents[i]->GetLocationX(), 2));
		if (tmp < h && opponents[i]->GetHealth()>0 && IsClear(opponents[i], maze, tmp)) {
			closestIndex = i;
			h = tmp;
		}
	}

	if (closestIndex > -1){
		if (h < BULLET_DISTANCE || h < GRENADE_DISTANCE) { // change if order - grenade first
			return true;
		}
		else if (h < GRENADE_DISTANCE) {
			if (pg == nullptr)
				pg = new Grenade(locationX, locationY);
			if (!pg->getIsExploded()) {
				pg->setIsExploded(true);
				pg->show();
			}
			pg->explode(maze);
			grenade_ammo--;
			return true;
		}
		else
			return false;
	}
	
	return false;
	
}

bool Soldier::IsClear(Soldier* opponent, int maze[MSZ][MSZ], int enemy_dist) {
	float radians = atan2(-locationY + opponent->GetLocationY(), -locationX + opponent->GetLocationX());
	Bullet* b = new Bullet(locationX, locationY, radians);
	int hitx, hity;
	int hit = b->checkPath(maze, color, &hitx, &hity, opponent->GetLocationX(), opponent->GetLocationY());
	int hit_dist = sqrt(pow(((double)locationY - (double)hity), 2) + pow((double)locationX - (double)hitx, 2));
	if (hit == opponent->GetColor())
		return true;
	else
		return hit_dist > enemy_dist;
		
}