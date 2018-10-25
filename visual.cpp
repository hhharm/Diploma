#pragma once
#include "MyForm.h"
using namespace DoseEffectsWithGui;

/*chart functions */
void MyForm::addToGraph(coreType ct, System::Windows::Forms::DataVisualization::Charting::Chart^ chart) {
	if (u == NULL) return;
	System::Windows::Forms::DataVisualization::Charting::Series^ col;
	switch (ct)
	{
	case DoseEffectsWithGui::epiph:

		col = chart->Series["Func"];
		break;
	case DoseEffectsWithGui::cos:

		col = chart->Series["Func1"];
		break;
	case DoseEffectsWithGui::lap:
		col = chart->Series["Func2"];
		break;
	case DoseEffectsWithGui::gaus:
		col = chart->Series["Func3"];
		break;
	case DoseEffectsWithGui::quart:
		col = chart->Series["Func4"];
		break;
	case DoseEffectsWithGui::ravn:
		col = chart->Series["Func5"];
		break;
	case DoseEffectsWithGui::triangl:
		col = chart->Series["Func6"];
		break;
		//this is normal distribution
	case DoseEffectsWithGui::notCore:
		col = chart->Series["Func7"];
	default:
		break;
	}
	col->Points->Clear();
	for (int i = 0; i < numberOfPoints; i++) {
		col->Points->AddXY(f[i].x, f[i].y);
	}
}

void MyForm::clearAllGraphs(System::Windows::Forms::DataVisualization::Charting::Chart^ chart) {
	chart->Series["Func"]->Points->Clear();
	chart->Series["Func1"]->Points->Clear();
	chart->Series["Func2"]->Points->Clear();
	chart->Series["Func3"]->Points->Clear();
	chart->Series["Func4"]->Points->Clear();
	chart->Series["Func5"]->Points->Clear();
	chart->Series["Func6"]->Points->Clear();
	chart->Series["Func7"]->Points->Clear();
}

void MyForm::drawGraph(coreType ct, System::Windows::Forms::DataVisualization::Charting::Chart^ chart) {
	clearAllGraphs(chart);
	addToGraph(ct, chart);
}

/*Other*/
coreType MyForm::getCoreType() {
	String^ coreValue = Convert::ToString(comboBox1->SelectedItem);
	if (coreValue == "Епанечникова ядро") {
		return epiph;
	}
	else if (Convert::ToString(comboBox1->SelectedItem) == "Косинус-ядро") {
		return cos;
	}
	else if (coreValue == "Лапласа ядро") {
		return lap;
	}
	else if (coreValue == "Гауссово ядро ") {
		return gaus;
	}
	else if (coreValue == "Квартическое ядро") {
		return quart;
	}
	else if (coreValue == "Равномерное ядро") {
		return ravn;
	}
	else if (coreValue == "Треугольное ядро") {
		return triangl;
	}
	else {
		return epiph;
	}
}


/*read/parse/update parameters*/
void MyForm::refreshGrid() {
	while (dataGridViewU->Rows->Count > 1)
		for (int i = 0; i < dataGridViewU->Rows->Count - 1; i++) {
			dataGridViewU->Rows->Remove(dataGridViewU->Rows[i]);
		}
	if (u != NULL) {
		for (int i = 0; i < volume; i++) {
			dataGridViewU->Rows->Add();
			dataGridViewU->Rows[i]->Cells[0]->Value = Convert::ToString(u[i]);
			dataGridViewU->Rows[i]->Cells[1]->Value = Convert::ToString(w[i]);
		}

	}
}
void MyForm::parseInterval() {
	try {
		x_low = Convert::ToDouble(textBox1->Text);
		x_high = Convert::ToDouble(textBox2->Text);
	}
	catch (...) {
		try {
			String^ s = textBox1->Text->Replace('.', ',');
			x_low = Convert::ToDouble(s);
			s = textBox2->Text->Replace('.', ',');
			x_high = Convert::ToDouble(s);

		}
		catch (...) {
			MessageBox::Show("Интервал указан неверно");
			return;
		}
	}
}
void MyForm::countInterval(double &low, double &high) {
	//строим функцию по всем встречающимся значениям
	low = *(std::min_element(u, u + volume));
	high = *(std::max_element(u, u + volume));
}
void MyForm::updateRange() {
	double low, high;
	if (u != NULL) {
		countInterval(low, high);
	}
	else {
		double range = sqrt(Convert::ToDouble(numericUpDown7->Value)) * 3;
		low = Convert::ToDouble(numericUpDown6->Value) - range;
		high = Convert::ToDouble(numericUpDown6->Value) + range;
	}
	textBox1->Text = Convert::ToString(low);
	textBox2->Text = Convert::ToString(high);
	x_low = low;
	x_high = high;
}


/*button clicks*/
void MyForm::generate() {
	if (u != NULL) {
		delete[] u;
		delete[] w;
		delete[] f;
	}
	volume = Convert::ToInt32(numericUpDown4->Value);
	if (Convert::ToString(comboBox2->SelectedItem) != "From file") {
		u = new double[volume];
		w = new double[volume];
		double M = Convert::ToDouble(numericUpDown6->Value); //mean
		double D = Convert::ToDouble(numericUpDown7->Value); //dispersion
		createUArray(u, volume, M, sqrt(D));
		createWArray(u, w, volume, M, sqrt(D));
		updateRange();
	}
	else {
		//donow
	}
	refreshGrid();
}



point* getNormalDistribution(double x_low, double x_high, double step, double M, double stddev) {

	// create normal distribution of double with set mean and stddev
	normal_distribution <double> distribution(M, stddev);
	int number_of_calc = (int)((x_high - x_low) / step);

	point* result = new point[number_of_calc];
	double x = x_low, y = 0.0;
	for (int i = 0; i < number_of_calc; i++, x+=step) {
		result[i].x = x;
		// Returns the probability of [-inf,x] of a gaussian distribution
		result[i].y = 0.5 * (1 + erf((x - M) / (stddev * sqrt(2.))));
	}
	return result;
}
//если у нас выборка получена случайным образом, то мы знаем заранее, как распределён Х и можем
// добавить на чарты графики CDF (cumulative distribution function), т.е. функцию распределения для нормального
// распределения с заданными мат. ожиданием и стандартным отклонением
void MyForm::addCDFtoChart(System::Windows::Forms::DataVisualization::Charting::Chart^ chart) {
	if (Convert::ToString(comboBox2->SelectedItem) == "From file") return;

	double M = Convert::ToDouble(numericUpDown6->Value); //mean
	double D = Convert::ToDouble(numericUpDown7->Value); //dispersion
	f = getNormalDistribution(x_low, x_high, step, M, sqrt(D));
	addToGraph(notCore, chart);
}

Core* MyForm::createCore() {
	cT = getCoreType();
	Core *core;
	switch (cT)
	{
	case DoseEffectsWithGui::epiph:
		core = new EpanchCore;
		break;
	case DoseEffectsWithGui::cos:
		core = new CosCore;
		break;
	case DoseEffectsWithGui::lap:
		core = new LaplCore;
		break;
	case DoseEffectsWithGui::ravn:
		core = new RavnCore;
		break;
	case DoseEffectsWithGui::gaus:
		core = new GausCore;
		break;
	case DoseEffectsWithGui::triangl:
		core = new TreangCore;
		break;
	case DoseEffectsWithGui::quart:
		core = new SquareCore;
		break;
	default:
		core = new EpanchCore;
		break;
	}
	return core;
}
void MyForm::doNWestimate(){
	step = Convert::ToDouble(numericUpDown3->Value);
	c = Convert::ToDouble(numericUpDown5->Value);
	h = c * pow(volume, -0.2);
	Core* core = createCore();
	numberOfPoints = (int)((x_high - x_low) / step);
	f = getFbiased(x_low, x_high, step, core, h, u, w, volume);
	delete core;
}
void MyForm::doKnnEstimate() {
	step = Convert::ToDouble(numericUpDown3->Value);
	numberOfPoints = (int)((x_high - x_low) / step);
	cT = getCoreType();
	Core *core = createCore();
	int k = Convert::ToInt32(numericUpDown1->Value);
	f = getKNN(x_low, x_high, step, core, k, u, w, volume);
	delete core;
}