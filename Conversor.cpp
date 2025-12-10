#include "Conversor.h"

namespace Conversor
{
	double TempConvert::ConvertF2C(double dFahrenheit) {
		return ((dFahrenheit - 32.0) * (5.0 / 9.0));
	}
	double TempConvert::ConvertC2F(double dCelsius) {
		return (9.0 / 5.0 * dCelsius) + 32;
	}
}
