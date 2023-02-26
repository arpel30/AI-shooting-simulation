#include "Cell.h"
#include <math.h>

Cell::Cell()
{

}

Cell::Cell(const Cell& other) {
	row = other.row;
	col = other.col;
	target_row = other.target_row;
	target_col = other.target_col;
	parent = other.parent;
	g = other.g;
	h = other.h;
	f = h + g;
}

Cell::Cell(int r, int c,int tr, int tc,double g,Cell * p)
{
	row = r;
	col = c;
	target_row = tr;
	target_col = tc;
	parent = p;
	this->g = g;
	h = sqrt(pow(r - tr, 2) + pow(c - tc, 2));
	f = h + g;
}

Cell::~Cell()
{
}

void Cell::updateTargetRowCol(int newTr, int newTc) { 
	target_row = newTr; 
	target_col = newTc; 
	h = sqrt(pow(row - target_row, 2) + pow(col - target_col, 2));
	f = h + g;
}
