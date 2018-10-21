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
	double h_left, h_right, h_tmp, //здесь храним высчитанные границы текущего интервала
		x = x_low, //это точка, которую рассматриваем сейчас
		k_counter = 0; 
	//мы ищем окно для каждой точки. в окно должны умещаться k соседей

	index = 0;
	
	for (int i = 0; i < number_of_calc; i++) {
		while (u[index] < x && index < size) {                                       
			index++; //ищем точку u[i], большую текущей
		}
		//смысл такой: идём влево и вправо, получаем начальную ширину окна слева-справа
		k_counter = 2;
		if (index == 0) {
			h[i] = abs(u[k-1] - x); //если слева нет ничего, то h - это от k-ого соседа до текущей точки
			continue;
		}
		index_left = index - 1;
		index_right = index + 1;

		//пока число найденных соседей не стало нужным нам мы идём в сторону наименьшего окна
		while (k_counter < k && index_left > 0 && index_right < size) {
			h_left = abs(u[index] - u[index_left]);
			h_right = abs(u[index] - u[index_right]);
			
			if (h_left < h_right) {
				index_left--;
				h_tmp = h_right;
			}
			else {
				index_right++;
				h_tmp = h_left;
			}
			k_counter++;
		}
		if (k_counter != k) {
			throw "В функция по поиску h в KNN случилось исключение :( Надо дописать код для случая, когда точки попадают на границу участка" ; 
		}
		x += step;
	}

}

//kNN оценка отличается от Надарая-Ватсона тем, что ширана окна (h) - изменяется. Она пересчитывается в каждой точке
//в интервал от (x-h, x+h) должно попадать k соседей (собственно поэтому это kNN: k nearest neighbours)
point* getKNN(double x_low, double x_high, double step, Core *core, int k, double *u, double *w, int size) {
	int number_of_calc = (int)((x_high - x_low) / step);
	double *h = new double [number_of_calc], 
		*u_copy = new double [size]; //при поиске окон мы сортируем массив на месте, поэтому надо создавать копию

	memcpy(u_copy, u, size * sizeof(double));
	getH(x_low, x_high, step, k, u_copy, w, size, h); 
	point* result = new point[number_of_calc];
	double x = x_low, y = 0.0;
	for (int i = 0; i < number_of_calc; i++) {
		y = biasedEstimate(x, u, size, w, size, h[i], core);
		result[i].x = x;
		result[i].y = y;
		x += step;
	}
	delete u_copy;
	delete[] h;
	return result;
}

void countConfidenceInterval(double &low, double &high, double mean, int n, double dispersion) {
	//Входные аргументы : n - размер выборки, дисперсия и мат. ожидания понятно что,
	//  критическое значение (какой процент должен попасть в интервал)
	// дл 95% = 1.96
	// для 98% = 3.58

	//вычисляем доверительный интервал (где большинство значений будет лежать)
	//изменяет значения low и high соответственно

	double range = 3.58 * (dispersion) / sqrt(n);
	low = mean - range;
	high = mean + range;
}

