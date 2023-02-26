#include "Storage.h"
#include <stdlib.h>
#include <iostream>
using namespace std;
/* ---------- Constructors ---------- */

Storage::Storage()
{
	x = 0;
	y = 0;
	room_number = 0;
	empty = false;
	cooldown = COOLDOWN_TIME;
}

Storage::Storage(int lx, int ly, int room_number_location)
{
	x = lx;
	y = ly;
	room_number = room_number_location;
	empty = false;
	cooldown = COOLDOWN_TIME;
}

/* ---------- Getters ---------- */

int Storage::GetX()
{
	return x;
}

int Storage::GetY()
{
	return y;
}

bool Storage::isEmpty()
{
	return empty;
}

int Storage::GetRoomNumber()
{
	return room_number;
}

int Storage::GetCooldown()
{
	return cooldown;
}

/* ---------- Setters ---------- */

void Storage::SetX(int lx)
{
	x = lx;
}

void Storage::SetY(int ly)
{
	y = ly;
}

void Storage::SetEmpty(bool isEmpty)
{
	empty = isEmpty;
	if (isEmpty)
		cooldown = COOLDOWN_TIME;
}

void Storage::SetRoomNumber(int room_number_location)
{
	room_number = room_number_location;
}

bool Storage::UpdateCooldown()
{
	if (cooldown > 0 && empty)
	{
		cooldown--;
	}
	else
	{
		cooldown = COOLDOWN_TIME;
		empty = false;
	}
	return false;
}