#include "pch.h"
#include "Brick.h"


const int Brick::width = 100;
const int Brick::height = 50;

Brick::Brick() {
	x1 = 0;
	y1 = 0;
	x2 = 0;
	y2 = 0;
	life = 0;
}

Brick::Brick(int x, int y, int _life)
{
	x1 = x;
	y1 = y;
	x2 = x + width;
	y2 = y + height;
	life = _life;
}

void Brick::SetLeftTop(int x, int y)
{
	x1 = x;
	y1 = y;
	x2 = x + width;
	y2 = y + height;
}

void Brick::MoveOneLine(void)
{
	SetLeftTop(x1, y1 + Brick::height);
}
