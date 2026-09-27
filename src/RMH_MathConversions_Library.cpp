/*
 *  RMH_MathConversions_Library.c
 *
 *  Author: Rune Mark Hansen
 *  Date: November 2022
 *
 */

 // Included libraries
#include <sstream>
#include <math.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <msclr\marshal_cppstd.h>
#include <string>
#include "RMH_MathConversions_Library.h"

// Namespaces for the associated library
using namespace System;
using namespace std;

// --------------------------------- Konverterings Routiner --------------------------------- //

void RMH_Conversion_IntToUnsignedCharArray(unsigned int InputInteger, unsigned short TargetStringLength, unsigned char *OutputCharArray) {

	// This routine converts an input integer to unsigned char (C implementation)

	// Read the temporary array data and sort the kernel array
	unsigned long CharacterDigitMultiplier = 0;

	// Loop up to and including the maximum string length
	for (unsigned int i = 0; i < TargetStringLength; i++) {

		// Udregn digit skallerings faktoren
		CharacterDigitMultiplier = pow(10, i);

		// Convert the integer to unsigned char and write it to the pointer array
		*(OutputCharArray + i) = (InputInteger / CharacterDigitMultiplier) % 10 + 48;

	}

}

System::String^ RMH_Conversion_StdStringToSystemString(std::string InputString) {

	// This routine converts a std::string to a System::String

	// Local objects
	System::String^ SystemString;

	// Konverter std::string til System::String
	SystemString = gcnew System::String(InputString.c_str());

	// Return the converted System::String
	return SystemString;

}

std::string RMH_Conversion_SystemStringToStdString(System::String^ InputString) {

	// This routine converts a System::String to a std::string 

	// Local objects
	std::string StdString;

	// Konverter System::String til std::string
	StdString = msclr::interop::marshal_as<std::string>(InputString);

	// Return the converted std::string
	return StdString;

}

System::String^ RMH_Conversion_IntToSystemString(unsigned int InputValue) {

	// This routine converts an input integer to a System::String

	// Return the converted System::String
	return System::Convert::ToString(InputValue);

}

std::string RMH_Conversion_IntToStdString(unsigned int InputValue) {

	// This routine converts an input integer to a std::string

	// Return the converted std::string
	return std::to_string(InputValue);

}

unsigned int RMH_Conversion_StdStringToInt(std::string InputString) {

	// This routine converts a std::string to an integer

	// Return the converted string as int
	return std::stoi(InputString);

}

unsigned int RMH_Conversion_SystemStringToInt(System::String^ InputString) {

	// This routine converts a System::String to an integer

	// Return the converted string as int
	return std::stoi(RMH_Conversion_SystemStringToStdString(InputString));

}

std::string RMH_Conversion_FloatToStdString(float Inputvalue, unsigned char Precision) {

	// This routine converts an input float to a std::string and returns the std::string

	// Local variables and objects
	std::ostringstream out;
	// Set the string decimal precision
	out.precision(Precision);
	// Write the string to the stringstream
	out << std::fixed << Inputvalue;

	// Retuner konverterede Float -> std::string
	return out.str();

}

float RMH_Conversion_StdStringToFloat(std::string inputString) {

	// This routine converts a std::string to float

	// Retuner konverterede std::string -> float
	return std::stof(inputString);

}

double RMH_Conversion_StdStringToDouble(std::string inputString) {

	// This routine converts a std::string to double

	// Retuner konverterede std::string -> double
	return std::stod(inputString);

}

bool RMH_Conversion_ReplaceCharOrStringInString(std::string& InputString, std::string& From, std::string& To) {

	// This routine replaces a given character or string ""

	// Find Start Opsitionen Af "From" string
	size_t start_pos = InputString.find(From);

	// If the string start position has an overflow
	if (start_pos == std::string::npos) return false;

	// Erstat String "From" i "InputString" med "To" String 
	InputString.replace(start_pos, From.length(), To);

	// Return the status
	return true;
}

float RMH_Conversion_uint32ToSinglePrecisionFloat(unsigned int InputValue) {

	// This routine converts a 32-bit unsigned integer to a single-precision float
	// The converted single-precision number is returned as a float

	// Return the 32-bit integer as a type-converted single-precision float
	return *reinterpret_cast<float*>(&InputValue);

}

void RMH_Conversion_SinglePrecisionFloatTo4xUnsignedChar(float Value, unsigned char* OutputValues) {

	// This routine converts a given float value to 4 x unsigned char values

	// Typecast ponter som unsigned int
	unsigned int* IntValue = reinterpret_cast<unsigned int*>(&Value);

	// Write the typecast values to the pointer array
	OutputValues[0] = (*IntValue >> 24) & 0xFF;
	OutputValues[1] = (*IntValue >> 16) & 0xFF;
	OutputValues[2] = (*IntValue >> 8) & 0xFF;
	OutputValues[3] = *IntValue & 0xFF;
	
}

float RMH_Conversion_UnsignedCharToSinglePrecisionFloat(unsigned char* InputValues) {

	// This routine converts 4 x unsigned char values to a single-precision float

	// Reset the baseline integer value
	unsigned int IntValue = 0;

	// Formater samlede 32Bit integer
	IntValue |= (static_cast<unsigned int>(InputValues[0]) << 24);
	IntValue |= (static_cast<unsigned int>(InputValues[1]) << 16);
	IntValue |= (static_cast<unsigned int>(InputValues[2]) << 8);
	IntValue |= static_cast<unsigned int>(InputValues[3]);

	// Typecast the integer value to float
	float floatValue = *reinterpret_cast<float*>(&IntValue);

	// Return the float value
	return floatValue;
}

float RMH_Conversion_UnsignedCharToSinglePrecisionFloat2(unsigned char Input1, unsigned char Input2, unsigned char Input3, unsigned char Input4) {

	// This routine converts 4 x unsigned char values to a single-precision float

	// Reset the baseline integer value
	unsigned int IntValue = 0;

	// Formater samlede 32Bit integer
	IntValue |= (static_cast<unsigned int>(Input1) << 24);
	IntValue |= (static_cast<unsigned int>(Input2) << 16);
	IntValue |= (static_cast<unsigned int>(Input3) << 8);
	IntValue |= static_cast<unsigned int>(Input4);

	// Typecast the integer value to float
	float floatValue = *reinterpret_cast<float*>(&IntValue);

	// Return the float value
	return floatValue;
}

float RMH_Conversion_uint16x2ToSinglePrecisionFloat(unsigned short HighValue, unsigned short LowValue) {

	// This routine converts two 16-bit unsigned integers to a single-precision float
	// The converted single-precision number is returned as a float

	// Kombiner de to 16bit integer til 32Bit
	unsigned int Combined32Bit = ((unsigned int)HighValue << 16) | LowValue;

	// Return the 2x16-bit integer as a type-converted single-precision float
	return *reinterpret_cast<float*>(&Combined32Bit);

}

float RMH_Conversion_SystemDecimalToFloat(System::Decimal Decimal) {

	// This routine converts a System::Decimal to float

	// Return the converted System::Decimal -> float
	return (float)System::Decimal::ToDouble(Decimal);

}

double RMH_Conversion_SystemDecimalToDouble(System::Decimal Decimal) {

	// This routine converts a System::Decimal to double

	// Return the converted System::Decimal -> double
	return (float)System::Decimal::ToDouble(Decimal);

}

System::Decimal RMH_Conversion_FloatToSystemDecimal(float InputValue) {

	// This routine converts a given float to System::Decimal

	// Retuner konverterede Float -> System:Decimal
	return System::Convert::ToDecimal(InputValue);

}

System::Decimal RMH_Conversion_DoubleToSystemDecimal(double InputValue) {

	// This routine converts a given double to System::Decimal

	// Retuner konverterede double -> System:Decimal
	return System::Convert::ToDecimal(InputValue);

}

bool RMH_Conversion_StdStringToBoolean(std::string InputString) {

	// This routine converts a std::string to boolean

	// Check the string
	if (InputString == "True") {
		// Return boolean true
		return true;
	}
	else {
		// Return boolean false
		return false;
	}

}

System::String^ RMH_Conversion_FloatToSystemString(float Value) {

	// This routine converts a given float to a System::String

	// Return the System::String
	return Value.ToString();

}

const char* RMH_Conversion_SystemStringToCharPtr(System::String^ str) {

	// This routine converts a System::String to a const char pointer

	// Konverter string til Std::String
	std::string stdStr = msclr::interop::marshal_as<std::string>(str);
	// Konverter Std::String til const char pointer
	const char* charPtr = stdStr.c_str();

	// Return the const char pointer
	return charPtr;

}

System::String^ RMH_Conversion_UnsignedCharArrayToSystemString(unsigned char* InputArray, unsigned int ArrayLength) {

	// This routine converts an unsigned char array to a System::String

	// Konverter unsigned char array til std::string
	std::string stdString(reinterpret_cast<char*>(InputArray), ArrayLength);

	// Konverter Std::String til System::String
	System::String^ ConvertedSystemString = msclr::interop::marshal_as<System::String^>(stdString);

	// Return the converted System::String
	return ConvertedSystemString;

}

cv::String RMH_VideoRecording_ConvertSystemStringToCVString(System::String^ sysString) {

	// This routine converts a System::String to a cv::string
	// and returns the converted cv::String

	// Convert the System::String to a std::string
	std::string StdString = msclr::interop::marshal_as<std::string>(sysString);

	// Convert the std::string to a cv::String
	cv::String CvString(StdString.c_str());

	// Return the converted cv::String
	return CvString;

}

// -------------------- Matematiske Udregnings & Konverterings Routiner --------------------- //

unsigned int RMH_Math_Round(double InputValue) {

	// This routine returns the nearest integer of the input value 

	// Check the input value
	if (InputValue < 0.0) {
		// Round down to the nearest integer
		return (unsigned int)(InputValue - 0.5);
	}
	else {
		// Round up to the nearest integer
		return (unsigned int)(InputValue + 0.5);
	}

}

double RMH_Math_absDouble(double InputValue) {

	// This routine calculates and returns the absolute value of a given input

	// If the given input value is lower than 0
	if (InputValue < 0.0) return -InputValue;
	else return InputValue;

}

// ------------------------------------------------------------------------------------------ //