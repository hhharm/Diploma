#include "Functions.h"
double biasedEstimate(double x, double *u, int size_u, double *w, int size_w, double h, Core *core) {
	if (size_u != size_w) {
		throw std::exception("Wrong sizes of arrays");
	}
	double s1 = 0.0,
		s0 = 0.0;
	for (int j = 0; j < size_u; j++) {
		//s0 += w[j] * (core->calculate((u[j] - x) / h) /h);
		s0 += w[j] * core->calculate((u[j] - x) / h);
	}
	//s0 /= size_u;
	for (int j = 0; j < size_u; j++) {
		//s1 += (core->calculate((u[j] - x) / h)) / h; 
		s1 += core->calculate((u[j] - x) / h);
	}
	//s1 /= size_u;
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



//Оценки ближайших соседей (kNN оценки) можно получить из оценок Надарая–Ватсона,
//если  взять ширину окна просмотра данных h специальным образом, именно, 
//чтобы в интервал x-h, x+h попадало k выборочных значений случайной величины U. Тогда величина h является случайной величиной. 
void getH(double x_low, double x_high, double step, int k, double *u, double *w, int size, double* h) {
	sort(u, u+size); //correct?
	//удалить потом
	for (int i = 1; i < size; i++) {
		if (u[i] < u[i - 1]) {
			throw "Сортировка кривая, перепиши! function getH";
		}
	}
	int number_of_calc = (int)((x_high - x_low) / step); //число итераций вычеслений
	int index, index_left,index_right; //здесь храним индекс 1ого элемента, который больше текущей точки
	double h_left, h_right, //здесь храним высчитанные границы текущего интервала
		x = x_low; //это точка, которую рассматриваем сейчас
	//мы ищем окно для каждой точки. в окно должны умещаться k соседей
	for (int i = 0; i < number_of_calc; i++) {
		//если точка не найдена, то окно фиксированное (0.1). Просто рандомное число
		index = -1;
		while (u[index] <= x) {                                       
			index++;
		}
		index_left = index - k / 2; //k - четное?
		index_right = index + k / 2;

		if (!(index_left < 0 || index_right > size)) {
			h_left = x - u[index_left];
			h_right = u[index_right] - x;
			if (h_left > h_right) {
				h[i] = h_left;
			}
			else {
				h[i] = h_right;
			}
		}
		else {
			if (index_left < 0) {
				index_right = index + k;
				h[i] = u[index_right] - x;
			}
			else {
				index_left = index - k;
				h[i] = x - u[index_left];
			}
		}
		

		//не учитывается ситуация, когда x вне пределов значений u
		x += step;
	}

}

point* getKNN(double x_low, double x_high, double step, Core *core, int k, double *u, double *w, int size) {
	int number_of_calc = (int)((x_high - x_low) / step);
	double *h = new double [number_of_calc];
	getH(x_low, x_high, step, k, u, w, size, h);
	point* result = new point[number_of_calc];
	double x = x_low, y = 0.0;
	for (int i = 0; i < number_of_calc; i++) {
		y = biasedEstimate(x, u, size, w, size, h[i], core);
		result[i].x = x;
		result[i].y = y;
		x += step;
	}
	delete[] h;
	return result;
}