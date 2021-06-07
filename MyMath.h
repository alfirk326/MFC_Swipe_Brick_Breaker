#pragma once
#define PI 3.1415926535

double Clamp(double min, double max, double value);
double GetDistance(double x1, double y1, double x2, double y2);
double GetDistance2(double x1, double y1, double x2, double y2);
int Normalize(double min1, double max1, double value, double min2, double max2);