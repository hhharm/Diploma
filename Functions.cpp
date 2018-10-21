#include "Functions.h"
double biasedEstimate(double x, double *u, int size_u, double *w, int size_w, double h, Core *core) {
	if (size_u != size_w) {
		throw std::exception("Wrong sizes of arrays");
	}
	double s1 = 0.0,
		s0 = 0.0;
	for (int j = 0; j < size_u; j++) {
		s0 += w[j] * core->calculate((u[j] - x) / h);
	}
	for (int j = 0; j < size_u; j++) {
		s1 += core->calculate((u[j] - x) / h);
	}
	if (s1 == 0.0)
		return 0.0;
	else
		return s0 / s1;
}

//generate arrays
void createUArray(double *u, int size_u, double M, double D) {
	// create random number generator, seed is set to the time since the epoch begin
	default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());

	// create normal distribution of double with set mean and stddev
	normal_distribution <double> distribution(M, D);
	for (int i = 0; i < size_u; i++)
		u[i] = distribution(generator); 
}


void createWArray(double *u, double *w, int size, double M, double D) {
	double temp;
	// create random number generator, seed is set to the time since the epoch begin
	default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());

	// create normal distribution of double with set mean and stddev
	normal_distribution <double> distribution(M, D);
	for (int i = 0; i < size; i++) {
		temp = distribution(generator);
		if (u[i] > temp)
			w[i] = 1.0;
		else
			w[i] = 0.0;
	}
}

point* getFbiased(double x_low, double x_high, double step, Core *core, double h, double *u, double *w, int size) {
	int number_of_calc = (int)((x_high - x_low) / step);
	point* result = new point[number_of_calc];
	double x = x_low, y = 0.0;
	for (int i = 0; i < number_of_calc; i++) {
		y = biasedEstimate(x, u, size, w, size, h, core);
		result[i].x = x;
		result[i].y = y;
		x += step;
	}
	return result;
}

double getH_point(double x, double *u, int k, int size) {
	//��������� ������ ���� ��������� ��� knn ������
	int ui = 0, i1, i2;
	double distance = abs(u[size-1] - u[0]); //����� �������� ��������

	for (int i = 0; i < size; i++) {
		if (abs(x - u[i]) < distance) { //���� ����� u[i], ������� ����� ����� � �������
			distance = abs(x - u[i]); 
			ui = i;
		}
	}
	i1 = ui;
	i2 = ui;
	//� 1, ������ ��� ������ ����� - u[ui]
	for (int i = 1; i < k; i++) {
		if (i1 == 0) {
			i2++;
			continue;
		}
		if (i2 == (size - 1)) {
			i1--;
			continue;
		}
		if (abs(u[ui] - u[i1 - 1]) <= abs(u[i2 + 1] - u[ui])) {
			i1--;
		}
		else {
			i2++;
		}
	}
	return abs(u[i2] - u[i1]) / 2;
}

//������ ��������� ������� (kNN ������) ����� �������� �� ������ ���������������,
//����  ����� ������ ���� ��������� ������ h ����������� �������, ������, 
//����� � �������� x-h, x+h �������� k ���������� �������� ��������� �������� U. ����� �������� h �������� ��������� ���������. 
void getH(double x, int iter_count, double step, int k, double *u, int size, double* h) {
	for (int i = 0; i < iter_count; i++, x+= step) {
		h[i] = getH_point(x,u,k,size);
	}

}

//kNN ������ ���������� �� �������-������� ���, ��� ������ ���� (h) - ����������. ��� ��������������� � ������ �����
//� �������� �� (x-h, x+h) ������ �������� k ������� (���������� ������� ��� kNN: k nearest neighbours)
point* getKNN(double x_low, double x_high, double step, Core *core, int k, double *u, double *w, int size) {
	int number_of_calc = (int)((x_high - x_low) / step);
	double *h = new double [number_of_calc], 
		*u_copy = new double [size]; //��� ������ ���� �� ��������� ������ �� �����, ������� ���� ��������� �����
	memcpy(u_copy, u, sizeof(double)*size);
	sort(u_copy, u_copy + size);
	getH(x_low, number_of_calc, step, k, u_copy, size, h);
	point* result = new point[number_of_calc];
	double x = x_low, y = 0.0;
	for (int i = 0; i < number_of_calc; i++) {
		y = biasedEstimate(x, u, size, w, size, h[i], core);
		result[i].x = x;
		result[i].y = y;
		x += step;
	}
	delete []u_copy;
	delete[] h;
	return result;
}

void countConfidenceInterval(double &low, double &high, double mean, int n, double dispersion) {
	//������� ��������� : n - ������ �������, ��������� � ���. �������� ������� ���,
	//  ����������� �������� (����� ������� ������ ������� � ��������)
	// �� 95% = 1.96
	// ��� 98% = 3.58

	//��������� ������������� �������� (��� ����������� �������� ����� ������)
	//�������� �������� low � high ��������������

	double range = 3.58 * (dispersion) / sqrt(n);
	low = mean - range;
	high = mean + range;
}

