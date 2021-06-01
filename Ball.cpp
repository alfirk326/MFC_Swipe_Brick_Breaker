#include "pch.h"
#include "Ball.h"


const double Ball::radius = 10.0;


Ball::Ball() {
	isDead = true;

	x = 0.0;
	y = 0.0;

	x1 = 0.0;
	y1 = 0.0;
	x2 = 0.0;
	y2 = 0.0;

	velocity[0] = 0.0;
	velocity[1] = 0.0;
}

Ball::Ball(double _x, double _y)
{
	isDead = false;

	x = _x;
	y = _y;

	x1 = x - radius;
	y1 = y - radius;
	x2 = x + radius;
	y2 = y + radius;

	velocity[0] = 0.0;
	velocity[1] = 0.0;
}

void Ball::SetCenterPoint(double _x, double _y)
{
	x = _x;
	y = _y;
	x1 = _x - radius;
	y1 = _y - radius;
	x2 = _x + radius;
	y2 = _y + radius;
}

void Ball::Move()
{
	SetCenterPoint(x + velocity[0], y + velocity[1]);
}