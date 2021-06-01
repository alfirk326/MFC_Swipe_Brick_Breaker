#pragma once

#include "Ball.h"

class DropBall : public Ball
{
public:
	bool isValid;

public:
	DropBall();
	DropBall(double _x, double _y);
	void Drop(void);
	void MoveOneLine(void);
};