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
void MyForm::refreshData() {
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