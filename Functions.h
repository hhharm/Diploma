#pragma once
#include "Cores.h"

#include <cstdio>
#include <random>
#include <ctime>
#include <algorithm>
#include <limits>
#include <chrono>	
double biasedEstimate(double x, double *u, int size_u, double *w, int size_w, double h, Core *core);
void createWArray(double *u, double *w, int size, double M = 0.0, double D = 1.0);
void createUArray(double *u, int size_u, double M = 0.0, double D = 1.0);

struct point {
	double x, y;
	point(double _x = 0.0, double _y = 0.0): x(_x), y(_y){}
};

/* Calculate NW
   Input: 
   limits to calculate and step
   way to calculate core
   window
   arrays with U, W and its size
   Output:
   array of points (x and y)
*/
point* getFbiased(double x_low, double x_high, double step, Core *core, double h, double *u, double *w, int size);

point* getKNN(double x_low, double x_high, double step, Core *core, int k, double *u, double *w, int size);