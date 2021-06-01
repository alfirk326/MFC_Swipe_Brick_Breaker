#pragma once

class Brick
{
public:
	int x1, y1, x2, y2;
	const static int width, height;
	int life;
public:
	Brick();
	Brick(int x, int y, int _life);
	void SetLeftTop(int, int);
	void MoveOneLine(void);
};