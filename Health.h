#pragma once
#include "Storage.h"

const int HEALTH_STOCK = 30;

class Health : public Storage
{

public:
	Health();
	Health(int lx, int ly, int rnumber);
	
};

