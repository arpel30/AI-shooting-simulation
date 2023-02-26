#pragma once

const int COOLDOWN_TIME = 50;
class Storage
{
protected:
	int x;
	int y;
	int room_number;
	bool empty;
	int cooldown;

public:
	Storage();
	Storage(int lx, int ly, int lroom_number);

	int GetX();
	int GetY();
	bool isEmpty();
	int GetRoomNumber();
	int GetCooldown();

	void SetX(int lx);
	void SetY(int ly);
	void SetEmpty(bool isEmpty);
	void SetRoomNumber(int room_number_location);
	bool UpdateCooldown();
};