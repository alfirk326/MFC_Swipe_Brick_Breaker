#pragma once

class Ball
{
public:
	double x, y;
	double x1, y1, x2, y2;
	double velocity[2];
	const static double radius;
	bool isDead;

public:
	Ball();
	Ball(double _x, double _y);
	void SetCenterPoint(double _x, double _y);
	void Move(void);
};