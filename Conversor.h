#pragma once
#using <System.dll>

namespace Conversor {
    public ref class TempConvert 
	{
	public:
			double ConvertF2C(double dFahrenheit);
			double ConvertC2F(double dCalsius);
	};
}