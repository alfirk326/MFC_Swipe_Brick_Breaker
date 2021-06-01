#include "pch.h"
#include "DropBall.h"
#include "Brick.h"

DropBall::DropBall()
{
	isValid = false;
}

DropBall::DropBall(double _x, double _y): Ball(_x, _y)
{
	isValid = true;
}

void DropBall::Drop()
{
	isValid = false;
	velocity[1] = 5.0;
}

void DropBall::MoveOneLine()
{
	SetCenterPoint(x, y + Brick::height);
}