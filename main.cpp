#include <iostream>
#include "Form.h"
#include "Conversor.h"
#using <mscorlib.dll>

using namespace System;
using namespace Conversor;
using namespace System::Threading;
using namespace System::Reflection;

[STAThread]
int main() {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	

	String^ savedLanguage = AppSettings::LoadLanguage();

	CultureInfo^ culture = gcnew CultureInfo(savedLanguage);
	Thread::CurrentThread->CurrentCulture = culture;
	Thread::CurrentThread->CurrentUICulture = culture;


	ConvForm^ CF = gcnew ConvForm;
	Application::Run(CF);

	return 0;
}