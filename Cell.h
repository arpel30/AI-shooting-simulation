#pragma once
#include "Definitions.h"

class Cell
{
private:
	int row, col;
	Cell* parent;
	int target_row, target_col;
	double h, g, f;

public:
	Cell();
	Cell(const Cell& other);
	Cell(int r, int c, int tr, int tc, double g, Cell * p);
	~Cell();
	int getRow() { return row; }
	int getCol() { return col; }
	int getTargetRow() { return target_row; }
	int getTargetCol() { return target_col; }
	double getF() { return f; }
	double getG() { return g; }
	bool operator == (const Cell &other) { return other.col == col && other.row == row; }
	void updateTargetRowCol(int newTr, int newTc);
	void setG(double newG) { g = newG; }
	void setF(double newF) { f = newF; }
	Cell* getParent() { return parent; }

};

