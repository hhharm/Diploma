#include "MyForm.h"
#include <time.h>
using namespace System;
using namespace System::Windows::Forms;


[STAThread]
int main()
{
	srand(unsigned(time(NULL)));
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	DoseEffectsWithGui::MyForm form;
	Application::Run(%form);
	return 0;
}
