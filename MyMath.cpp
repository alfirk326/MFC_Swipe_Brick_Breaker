#include "pch.h"
#include "MyMath.h"


double Clamp(double min, double max, double value)
{
	if (value < min) {
		return min;
	}
	else if (value > max) {
		return max;
	}
	else {
		return value;
	}
}

double GetDistance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0));
}

double GetDistance2(double x1, double y1, double x2, double y2)
{
	return pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0);
}

int Normalize(double min, double max, double value, double toNum1, double toNum2)
{
	if (min == max) {
		return (int)toNum1; // toNum2로 바꿔도 된다.
	}

	return (int)(toNum1 + ((toNum2 - toNum1) * ((value - min) / (max - min))));
}