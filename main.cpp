#include "glut.h"
#include <time.h>
#include <vector>
#include <queue>
#include <iostream>
#include "Cell.h"
#include "Room.h"
#include "Grenade.h"
#include "CompareCells.h"
#include "Health.h"
#include "Ammo.h"
#include "Bullet.h"
#include "Soldier.h"

#include <chrono>
#include <thread>
// sleep function was added for a nice path drawing 

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds


using namespace std;

const int NUM_ROOMS = 10;
const int MAX_ROOM_WIDTH = MSZ / 5;
const int MAX_ROOM_HEIGHT = MSZ / 5;
const int MIN_ROOM_WIDTH = 6;
const int MIN_ROOM_HEIGHT = 6;
const int FPS = 100;

int closest = -1;
int time_counter = 0;

Room* rooms[NUM_ROOMS];
bool empty_rooms[NUM_ROOMS];
Storage* health_storages[NUM_ROOMS / 2];
Storage* ammo_storages[NUM_ROOMS / 2];
Soldier* red_team[3];
Soldier* green_team[3];

int maze[MSZ][MSZ];
double security_map[MSZ][MSZ] = {0};

void InitMaze();
void InitRooms();
void DigTunnels();
void InitTeams();
void create_storage_helper(int* lx, int* ly, int* room_number);
Bullet* pb = nullptr;
Grenade* pg = nullptr;
bool isFinish = true, finishedInit=false, redAlive=true, greenAlive=true;

void init()
{
	glClearColor(0.3, 0.3, 0.3, 0);// color of window background
	//           RED GREEN BLUE

	glOrtho(0, MSZ, 0, MSZ, -1, 1);
	//     left right bottom top near, far

	srand(time(0)); // initialize random seed

	InitMaze();
	InitRooms();
	DigTunnels();
	InitTeams();
}

double calcDist(int x1, int x2, int y1, int y2)
{
	return sqrt(pow(((double)x1 - (double)x2), 2) + pow((double)y1 - (double)y2, 2));
	
}

bool checkDist(int x1, int x2, int y1, int y2)
{
	return calcDist(x1, x2, y1, y2) > 0.4*MSZ;
}

void InitTeams()
{
	int i, room_number, x, y;
	for (i = 0; i < 3; i++)
	{
		do
		{
			create_storage_helper(&x, &y, &room_number);
			red_team[i] = new Soldier(y, x , room_number, RED_SOLD, i==2);
		} while (!empty_rooms[room_number]);
		empty_rooms[room_number] = false;
	}

	for (i = 0; i < 3; i++)
	{
		do
		{
			create_storage_helper(&x, &y, &room_number);
			green_team[i] = new Soldier(y, x, room_number, GREEN_SOLD, i==2);
		} while (!empty_rooms[room_number]);
		empty_rooms[room_number] = false;
	}

	
	finishedInit = true;
}


void FillRoom(int index) 
{
	int i, j;
	int cr, cc, w, h;

	w = rooms[index]->getW();
	h = rooms[index]->getH();
	cr = rooms[index]->getCenterRow();
	cc = rooms[index]->getCenterCol();


	for (i = cr - h / 2; i <= cr + h / 2; i++)
		for (j = cc - w / 2; j <= cc + w / 2; j++)
			maze[i][j] = SPACE;
}

void create_storage_helper(int* lx, int* ly, int* room_number) {
	*room_number = rand() % (NUM_ROOMS);
	int min_x, max_x, min_y, max_y;

	min_y = rooms[*room_number]->getCenterRow() - (rooms[*room_number]->getH() / 2);
	max_y = rooms[*room_number]->getCenterRow() + (rooms[*room_number]->getH() / 2);
	min_x = rooms[*room_number]->getCenterCol() - (rooms[*room_number]->getW() / 2);
	max_x = rooms[*room_number]->getCenterCol() + (rooms[*room_number]->getW() / 2);
	do {
		*lx = min_x + rand() % (max_x - min_x);
		*ly = min_y + rand() % (max_y - min_y);
	} while (maze[*ly][*lx] != SPACE);
}

void create_storage(int type, int index) {
	int lx, ly, room_number;
	create_storage_helper(&lx, &ly, &room_number);
	switch (type) {
	case HEALTH:
		maze[ly][lx] = HEALTH;
		health_storages[index] = new Health(lx, ly, room_number);
		break;
	case AMMO:
		maze[ly][lx] = AMMO;
		ammo_storages[index] = new Ammo(lx, ly, room_number);
		break;
	}
}

void  spawn_storages() {
	int i;
	for (i = 0; i < NUM_ROOMS; i++) {
		if (i < NUM_ROOMS / 2 == 0) {
			create_storage(HEALTH, i);
		}
		else {
			create_storage(AMMO, i - NUM_ROOMS / 2);
		}
	}

}

void hide_empty_storages() {
	int i;
	for (i = 0; i < NUM_ROOMS; i++) {
		if (i < NUM_ROOMS / 2) {
			if (health_storages[i]->isEmpty())
				maze[health_storages[i]->GetY()][health_storages[i]->GetX()] = SPACE;
			else
				maze[health_storages[i]->GetY()][health_storages[i]->GetX()] = HEALTH;
		}
		else {
			int j = i - NUM_ROOMS / 2;
			if (ammo_storages[j]->isEmpty())
				maze[ammo_storages[j]->GetY()][ammo_storages[j]->GetX()] = SPACE;
			else
				maze[ammo_storages[j]->GetY()][ammo_storages[j]->GetX()] = AMMO;
		}
	}
}

void update_storages() {
	int i;
	for (i = 0; i < NUM_ROOMS; i++) {
		if (i < NUM_ROOMS / 2) {
			if (health_storages[i]->UpdateCooldown()) {
				maze[health_storages[i]->GetY()][health_storages[i]->GetX()] = HEALTH;
			}
		}
		else {
			if (ammo_storages[i - NUM_ROOMS / 2]->UpdateCooldown()) {
				maze[ammo_storages[i - NUM_ROOMS / 2]->GetY()][ammo_storages[i - NUM_ROOMS / 2]->GetX()] = AMMO;
			}
		}
	}
}

bool hasOverlapping(int index,int w, int h, int crow, int ccol)
{
	int i;
	int dx, dy;
	bool overlap = false;
	for (i = 0; i < index && !overlap; i++)
	{
		dx = abs(ccol - rooms[i]->getCenterCol());
		dy = abs(crow - rooms[i]->getCenterRow());
		if (dx <w/2 + rooms[i]->getW()/2 +3 && 
				dy< h/2 +rooms[i]->getH()/2+3)
			overlap = true;
	}
	return overlap;
}

void InitRooms() 
{
	int crow, ccol, w, h;
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		// init a room
		do {
			w = MIN_ROOM_WIDTH + rand() % (MAX_ROOM_WIDTH - MIN_ROOM_WIDTH);
			h = MIN_ROOM_HEIGHT + rand() % (MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT);
			crow = 2 + h / 2 + rand() % (MSZ - (5 + h));
			ccol = 2 + w / 2 + rand() % (MSZ - (5 + w));
		} while (hasOverlapping(i, w, h, crow, ccol));
		rooms[i] = new Room(crow, ccol, w, h);
		empty_rooms[i] = true;
		FillRoom(i);
	}

	// set random obstacles
	for (int i = 0; i < 200; i++)
		maze[rand() % MSZ][rand() % MSZ] = WALL;

	spawn_storages();

}

void InitMaze() 
{
	int i, j;
	// setup inner space of maze
	for(i=0;i<MSZ;i++)
		for (j = 0; j < MSZ ; j++)
			maze[i][j] = WALL; 


}

void drawStats(Soldier* sold) {
	int x = sold->GetLocationX();
	int y = sold->GetLocationY();
		
	double ammo = x - 2 + (4 * ((double)sold->GetBulletsAmmo() / START_BULLETS));
	double health = x - 2 + (4 * ((double)sold->GetHealth() / START_HEALTH));
	double grenades;
	if(sold->IsSquire())
		grenades = x - 2 + (4 * ((double)sold->GetCarryHealth() / START_HEALTH));
	else
		grenades = x - 2 + (4 * ((double)sold->GetGrenadeAmmo() / START_GRENADES));
	
	if(ammo > 0){
		glLineWidth(3);
		glBegin(GL_LINES);
		glColor3d(255, 0, 255);
		glVertex2f(x - 2, y + 1.5);
		glVertex2f(ammo, y + 1.5);
		glEnd();
	}

	if (health > 0) {
		glLineWidth(3);
		glBegin(GL_LINES);
		glColor3d(0.0, 0.5, 0.0);
		glVertex2f(x - 2, y + 2);
		glVertex2f(grenades, y + 2);
		glEnd();
	}

	if (grenades > 0) {
		glLineWidth(3);
		glBegin(GL_LINES);
		glColor3d(255, 255, 0);
		glVertex2f(x - 2, y + 2.5);
		glVertex2f(health, y + 2.5);
		glEnd();
	}

}

void RemoveDeadSoldiers()
{
	int i;
	redAlive = false;
	greenAlive = false;
	for (i = 0; i < NUM_SOLDIERS; i++)
	{
		if (red_team[i]->GetHealth() > 0) {
			maze[red_team[i]->GetLocationY()][red_team[i]->GetLocationX()] = RED_SOLD;
			drawStats(red_team[i]);
			redAlive = true;
		}
		else {
			red_team[i]->kill();
			maze[red_team[i]->GetLocationY()][red_team[i]->GetLocationX()] = SPACE;
		}
		if (green_team[i]->GetHealth() > 0) {
			maze[green_team[i]->GetLocationY()][green_team[i]->GetLocationX()] = GREEN_SOLD;
			drawStats(green_team[i]);
			greenAlive = true;
		}
		else {
			green_team[i]->kill();
			maze[green_team[i]->GetLocationY()][green_team[i]->GetLocationX()] = SPACE;
		}
	}
}
void RestorePath(Cell* pCurrent,int start_row, int start_col, int mode, Soldier* s) // mode: 0-digPath, 1-soldier walking
{
	
	while(pCurrent->getParent()!=nullptr)
	{
		if (maze[pCurrent->getRow()][pCurrent->getCol()] == WALL && mode == 0)
			maze[pCurrent->getRow()][pCurrent->getCol()] = SPACE;
		if (pCurrent->getParent()->getParent() == nullptr && mode == 1) {
			switch (maze[pCurrent->getRow()][pCurrent->getCol()]) {
			case AMMO:
				s->UpdateBulletsAmmo(BULLETS_STOCK);
				s->UpdateGrenadeAmmo(GRENADE_STOCK);
				ammo_storages[s->GetAmmoIndex()]->SetEmpty(true);
				break;
			case HEALTH:
				s->UpdateHealth(HEALTH_STOCK);
				health_storages[s->GetHealthIndex()]->SetEmpty(true);
				break;
			case RED_SOLD:
				if (s->IsSquire() && s->GetColor() == RED_SOLD)
				{
					s->HelpSoldier(red_team, pCurrent->getRow(), pCurrent->getCol());
				}
				break;
			case GREEN_SOLD:
				if (s->IsSquire() && s->GetColor() == GREEN_SOLD)
				{
					s->HelpSoldier(green_team, pCurrent->getRow(), pCurrent->getCol());
				}
				break;
			}

			maze[pCurrent->getRow()][pCurrent->getCol()] = s->GetColor();
			s->SetLocationX(pCurrent->getCol());
			s->SetLocationY(pCurrent->getRow());
		}
		pCurrent = pCurrent->getParent();
	}
	if (mode == 1) {
		maze[pCurrent->getRow()][pCurrent->getCol()] = SPACE;
	}
}


// who is the neighbor at nrow ncol? If it is white then paint it gray and add to pq
// If it is gray then check two cases: 
// 1. if F of this neighbor is now better then what was before then we need to update the neighbor
// 2. if it is not better then do nothing
// If it is black do nothing
// If it is Target then we have two cases (actually this is one of the previous cases, white or gray):
// 1. if F of this target is now better then what was before then we need to update the target
// 2. if it is not better then do nothing
void CheckNeighbor(Cell* pCurrent, int nrow, int ncol,
	priority_queue <Cell, vector<Cell>, CompareCells> &pq,
	vector <Cell> &grays, vector <Cell> &blacks, int mode)
{
	if (mode == 1 && maze[pCurrent->getRow()][pCurrent->getCol()] == WALL)
		return;
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	double space_cost = 0.1, wall_cost = 2,cost;
	if (maze[nrow][ncol] == SPACE || maze[nrow][ncol] == RED_SOLD || maze[nrow][ncol] == GREEN_SOLD)
		//cost = space_cost;
		cost = security_map[nrow][ncol];
	else cost = wall_cost;
	// this is the neighbor
	Cell* pn = new Cell(nrow, ncol, pCurrent->getTargetRow(), pCurrent->getTargetCol(),
		pCurrent->getG()+cost, pCurrent);
	// check the color of this neighbor
	it_black = find(blacks.begin(), blacks.end(), *pn);
	if (it_black != blacks.end()) // it was found i.e. it is black
		return;
	// white
	it_gray = find(grays.begin(), grays.end(), *pn);
	if (it_gray == grays.end()) // it wasn't found => it is white
	{
		grays.push_back(*pn); // paint it gray
		pq.push(*pn);
	}
	else // it is gray
	{
		//   new F       ?      old F
		if (pn->getF() < it_gray->getF()) // then update it (F of neighbor)!!!
		{
			// we need toupdate it in two places:
			// 1. in vector grays
			it_gray->setG(pn->getG()); 
			it_gray->setF(pn->getF());
			// 2. in PQ
			// to update a Cell in pq we need to remove it from pq, to update it and to push it back
			vector<Cell> tmp;
			Cell tmpCell; 
			do
			{
				if (pq.empty())
				{
					cout << "ERROR! PQ is empty in update PQ\n";
					exit(1);
				}
				tmpCell = pq.top();
				pq.pop();
				if(!(tmpCell == (*pn))) // do not copy the Cell we were looking for to tmp!(see line 173) 
					tmp.push_back(tmpCell);
			} while (!(tmpCell==(*pn)));
			// now we are aout of do-while because we have found the neighbor in PQ. So change it to *pn.
			pq.push(*pn);
			// now push back all the elements that are in tmp
			while (!tmp.empty())
			{
				pq.push(tmp.back());
				tmp.pop_back();
			}
		}
	}
}

// runs A* from room i to room j
bool DigPath(int y1, int x1, int y2, int x2, int mode, Soldier* s)
{
	if (y1 == y2 && x1 == x2 && mode == 1) {
		
		return true;
	}
	int row, col;
	vector <Cell> grays;
	vector <Cell> blacks;
	priority_queue <Cell,vector<Cell>,CompareCells> pq;
	Cell* pstart = new Cell(y1, x1, y2, x2, 0, nullptr);

	Cell* pCurrent;
	bool targetFound = false;
	// initializes grays and pq
	grays.push_back(*pstart);
	pq.push(*pstart);
	vector<Cell>::iterator it_gray;

	while (!pq.empty())
	{
		pCurrent = new Cell(pq.top());
		pq.pop();
		// If current is actually a target then we stop A*
		if (pCurrent->getRow() == y2 &&
			pCurrent->getCol() == x2) // then it is target
		{ //in this case there cannot be a better path to target!!!
			RestorePath(pCurrent, y1, x1, mode, s);
			return false;
		}
		// paint current black
		blacks.push_back(*pCurrent);
		it_gray = find(grays.begin(), grays.end(), *pCurrent); // we have to define operator ==
		if (it_gray != grays.end()) // current was found
			grays.erase(it_gray); // and removed from grays
		// now check the neighbors of current
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		// try to go UP (row -1)
		if (row > 0) // we can go UP
			CheckNeighbor(pCurrent, row - 1, col, pq, grays, blacks, mode);

		if(row<MSZ-1) // DOWN
			CheckNeighbor(pCurrent, row + 1, col, pq, grays, blacks, mode);

		if (col < MSZ - 1) // RIGHT
			CheckNeighbor(pCurrent, row , col+ 1, pq, grays, blacks, mode);

		if (col > 0)//LEFT
			CheckNeighbor(pCurrent, row, col - 1, pq, grays, blacks, mode);

	}
	return false;
}

void DigTunnels() 
{
	int i, j;
	for (i = 0; i < NUM_ROOMS; i++)
		for (j = i + 1; j < NUM_ROOMS; j++)
		{
			DigPath(rooms[i]->getCenterRow(), rooms[i]->getCenterCol(), rooms[j]->getCenterRow(), rooms[j]->getCenterCol(), 0, nullptr); // A*
		}
}

void ShowMaze() 
{
	int i, j;
	for(i=0;i<MSZ;i++)
		for(j=0;j<MSZ;j++)
		{
			switch (maze[i][j]) 
			{
			case WALL: 
				glColor3d(0, 0, 0);// set color black
				break;
			case SPACE: 
//				glColor3d(1, 1, 1);// set color white
				// show security data
				glColor3d(1-security_map[i][j], 1 - security_map[i][j], 1 - security_map[i][j]);
				break;
			case GREEN_SOLD:
				glColor3d(0, 0.7, 0);// set color green
				break;
			case RED_SOLD:
				glColor3d(255, 0, 0);// set color red
				break;
			case AMMO:
				glColor3d(255, 0, 255);// set color blue
				break;
			case HEALTH:
				glColor3d(255, 255, 0);// set color yellow
				break;
			

			} // switch
			// now show the cell of maze
			glBegin(GL_POLYGON);
			glVertex2d(j,i); // left-bottom vertex
			glVertex2d(j,i+1); // left-top vertex
			glVertex2d(j+1,i+1); // right-top vertex
			glVertex2d(j+1,i); // right-bottom vertex
			glEnd();
		}
}

void CreateSecurityMap()
{
	int num_simulations = 1000;
	double damage = 0.001;
	int x, y;

	for(int i=0;i<num_simulations;i++)
	{
		x = rand() % MSZ;
		y = rand() % MSZ;
		Grenade* g = new Grenade(x, y);
		g->SimulateExplosion(maze, security_map,damage);
	}
}
void displaySoldiers() {
	for (int i = 0; i < 3; i++)
	{
		if (red_team[i]->GetBullet() && red_team[i]->GetBullet()->getIsFired()) {
			red_team[i]->GetBullet()->show();
		}
		if (red_team[i]->GetGrenade() && red_team[i]->GetGrenade()->getIsExploded()) {
			red_team[i]->GetGrenade()->show();
		}
		if (green_team[i]->GetBullet() && green_team[i]->GetBullet()->getIsFired()) {
			green_team[i]->GetBullet()->show();
		}
		if (green_team[i]->GetGrenade() && green_team[i]->GetGrenade()->getIsExploded()) {
			green_team[i]->GetGrenade()->show();
		}
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	ShowMaze();

	hide_empty_storages();
	RemoveDeadSoldiers();

	if (pb != nullptr)
	{
		pb->show();
	}

	if (pg != nullptr)
	{
		pg->show();
	}
	displaySoldiers();

	glutSwapBuffers(); // show all
}



// runs all the time in the background
void idle(int val)
{
	if (!isFinish && finishedInit)
	{
		for (int i = 0; i < 3; i++)
		{
			if (red_team[i]->GetHealth() > 0) {
				red_team[i]->MakeDecision(health_storages, ammo_storages, green_team, NUM_ROOMS / 2, NUM_SOLDIERS, maze, red_team, i);
				DigPath(red_team[i]->GetLocationY(), red_team[i]->GetLocationX(), red_team[i]->GetDesty(), red_team[i]->GetDestx(), 1, red_team[i]);
			}
			if (green_team[i]->GetHealth() > 0) {
				green_team[i]->MakeDecision(health_storages, ammo_storages, red_team, NUM_ROOMS / 2, NUM_SOLDIERS, maze, green_team, i);
				DigPath(green_team[i]->GetLocationY(), green_team[i]->GetLocationX(), green_team[i]->GetDesty(), green_team[i]->GetDestx(), 1, green_team[i]);
			}	
		}
		update_storages();
		if (!redAlive) {
			cout << "Green team won!\n";
			isFinish = true;
		}
		if (!greenAlive) {
			cout << "Red team won!\n";
			isFinish = true;
		}
		glutPostRedisplay(); // indirect call to display
		glutTimerFunc(FPS, idle, 0);
	}
}

void menu(int choice) 
{
	if (choice == 1) // start game
	{
		isFinish = !isFinish;
		glutTimerFunc(FPS, idle, 0);
	}
	else if (choice == 2) // Create Security Map
	{
		CreateSecurityMap();
	}
	else if (choice == 3) // Tip the bartender
	{
		red_team[2]->UpdateHealth(-START_HEALTH);
		green_team[2]->UpdateHealth(-START_HEALTH);
		cout << "you shot the squire.\n";
	}
	else if (choice == 4) // Tip the bartender
	{
		cout << "you have just tipped the bartender, thanks!\n";
	}

}

void mouse(int button, int state, int x, int y) 
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// place bullet in position (x,y)
		double xx, yy;
		xx = MSZ * (x / 600.0);
		yy = MSZ * ((600 - y) / 600.0);

	//	pb = new Bullet(xx, yy);
		//pg = new Grenade(xx, yy);

	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Dungeon");

	glutDisplayFunc(display);
//	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	// add menu
	glutCreateMenu(menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("Play/Pause", 1);
	glutAddMenuEntry("Create Security Map", 2);
	glutAddMenuEntry("Who Shot The Squire?", 3);
	glutAddMenuEntry("Tip The Bartender", 4);



	init();
	glutTimerFunc(FPS, idle, 0);
	glutMainLoop();
	return 0;
}