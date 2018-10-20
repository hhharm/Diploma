#include "Cores.h"

double EpanchCore::calculate(double value) {
	if (abs(value) <= 1)
		return (double) (0.75 * (1 - value * value));
	else
		return 0.0;
}

double SquareCore::calculate(double value) {
	if (abs(value) <= 1) {
		double res = 15 * (1 - value * value) * (1 - value * value) / 16;
		return res;
	}
	else
		return 0.0;
}

double RavnCore::calculate(double value) {
	if (abs(value) <= 1) {
		return 0.5;
	}
	else
		return 0.0;
}

double TreangCore::calculate(double value) {
	if (abs(value) <= 1) {
		return 1 - abs(value);
	}
	else
		return 0.0;
}

double CosCore::calculate(double value) {
	if (abs(value) <= 1) {
		return (M_PI / 4)* cos(M_PI * value / 2);
	}
	else
		return 0.0;
}

double LaplCore::calculate(double value) {
	double res = 0.5 * exp(-abs(value));
	return res;
}

double GausCore::calculate(double value) {
	if (abs(value) <= 1) {
		double res = (1 / sqrt(2* M_PI))*exp(-(value*value)/2);
		return res;
	}
	else
		return 0.0;
}

