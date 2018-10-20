#pragma once
#include "defines.h"
struct Core {
	virtual double calculate(double value) = 0;
};

//ядро Епанечникова
struct EpanchCore : public Core {
	double calculate(double value);
};

//	квартическое ядро,
struct SquareCore : public Core {
	double calculate(double value);
};

//Равномерное ядро
struct RavnCore : public Core {
	double calculate(double value);
};

//  Треугольное ядро
struct TreangCore : public Core {
	double calculate(double value);
};

//	Косинус - ядро
struct CosCore : public Core {
	double calculate(double value);
};

//	Лапласа ядро
struct LaplCore : public Core {
	double calculate(double value);
};

//	Гауссово ядро
struct GausCore : public Core {
	double calculate(double value);
};



