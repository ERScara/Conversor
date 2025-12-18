#include "Conversor.h"
#using <System.Windows.Forms.dll>

using namespace System::Windows::Forms;

namespace Conversor
{
	double TempConvert::ConvertF2C(double dFahrenheit) {
		if (dFahrenheit < -459.67) {
			MessageBox::Show("Values below -459.67ºF are not meaningful since it's the absolute zero temperature!", "Below Absolute Zero", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			return 0.0;
		}
		else {
			return ((dFahrenheit - 32.0) * (5.0 / 9.0));
		}
	}
	double TempConvert::ConvertC2F(double dCelsius) {
		if (dCelsius < -273.15) {
			MessageBox::Show("Values below -273.15ºC are not meaningful since it's the absolute zero temperature!", "Below Absolute Zero", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			return 0.0;
		} else {
		return (9.0 / 5.0 * dCelsius) + 32;
		}
	}
}
