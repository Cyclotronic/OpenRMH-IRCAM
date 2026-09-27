
/*
 *  RMH_ImageProcessing_Library.c
 *
 *  Author: Rune Mark Hansen
 *  Date: November 2022
 *
 */

// Included libraries
#include "GlobalObjectsAndVariables.h"
#include "RMH_ImageProcessing_Library.h"
#include "RMH_MathConversions_Library.h"
#include "RMH_ThermalCameraSupport_Library.h"
#include "RMH_SupportedIRCameras_Resources.h"

// Namespaces for the associated library
using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Diagnostics;
using namespace std;

// ------------------------------ Image Calculation Routines ------------------------------- //

double RMH_ImageCalculations_CalMeanOfImage16Bit(unsigned short* ImageData, unsigned int FrameWidth, unsigned int FrameHeight) {

	// This routine calculates the mean value of a given image data array.
	// The given input image array data must be 16-bit or lower.

	// Local variables
	double PixelValMean = 0.0;
	unsigned long long int PixelValSum = 0;

	// Sum the pixels over all Y positions
	for (unsigned int Y = 0; Y < FrameHeight; Y++) {

		// Sum the pixels over all X positions
		for (unsigned int X = 0; X < FrameWidth; X++) {

			// Udregn samlede pixel data Sum
			PixelValSum += ImageData[Y * FrameWidth + X]; 

		}
	}

	// Calculate the mean value of the pixel data from the pixel sum
	PixelValMean = (double)PixelValSum / ((double)FrameWidth * (double)FrameHeight);

	// Return the calculated pixel data mean value
	return PixelValMean;

}

// ---------------------- Billede Non-Uniformity Korrektions Routiner ----------------------- //

void RMH_ImageNonUniformityCorrection_ConvertBaselineImageTo16Bit(unsigned char* BaselineImageData, unsigned short* OutputBaseline16Bit, unsigned int FrameWidth, unsigned int FrameHeight) {

	// This routine converts a given baseline image data array to a 16-bit baseline array

	// Local variables
	register unsigned short Pixel16BitValue[3];

	// Loop through all baseline band pixels
	for (unsigned int i = 0, j = 0; i < (FrameWidth * FrameHeight); i += 3, j += 6) {

		// Read the baseline image band data and convert it to 16-bit pixel data 
		Pixel16BitValue[0] = ((unsigned short)(*(BaselineImageData + (j + 1))) << 8) | ((unsigned short)*(BaselineImageData + (j + 0)));
		Pixel16BitValue[1] = ((unsigned short)(*(BaselineImageData + (j + 3))) << 8) | ((unsigned short)*(BaselineImageData + (j + 2)));
		Pixel16BitValue[2] = ((unsigned short)(*(BaselineImageData + (j + 5))) << 8) | ((unsigned short)*(BaselineImageData + (j + 4)));

		// Write the converted pixel values to the pointer array
		*(OutputBaseline16Bit + (i + 0)) = Pixel16BitValue[0];
		*(OutputBaseline16Bit + (i + 1)) = Pixel16BitValue[1];
		*(OutputBaseline16Bit + (i + 2)) = Pixel16BitValue[2];

	}

}

double RMH_ImageNonUniformityCorrection_CalNonUniformityMap(unsigned short* BaselineImageData, unsigned int FrameWidth, unsigned int FrameHeight, unsigned int FrameMetaDataSize, double* OutputNonUniformityMap) {

	// This routine calculates the non-uniformity map of a given CMOS baseline image data array.
	// The routine returns the mean value of the CMOS baseline image, together with the non-uniformity mapping as a double array.

	// Local variables
	double BaselineImageMeanValue = 0.0;

	// Calculate the mean value of the CMOS baseline measurement - without metadata
	BaselineImageMeanValue = RMH_ImageCalculations_CalMeanOfImage16Bit(BaselineImageData, FrameWidth, FrameHeight - FrameMetaDataSize);

	// Subtract the mean value of the CMOS baseline measurement from the baseline pixel values - calculates the non-uniformity mapping
	for (unsigned int i = 0; i < (FrameWidth * (FrameHeight - FrameMetaDataSize)); i += 3) {

		// Calculate the non-uniformity mapping for the baseline image data
		OutputNonUniformityMap[i + 0] = (double)(*(BaselineImageData + (i + 0))) - BaselineImageMeanValue;
		OutputNonUniformityMap[i + 1] = (double)(*(BaselineImageData + (i + 1))) - BaselineImageMeanValue;
		OutputNonUniformityMap[i + 2] = (double)(*(BaselineImageData + (i + 2))) - BaselineImageMeanValue;

	}

	// Return the mean value of the CMOS baseline measurement
	return BaselineImageMeanValue;

}

void RMH_ImageNonUniformityCorrection_ZeroNonUniformityMapArrayData(double* OutputNonUniformityMap, unsigned int FrameWidth, unsigned int FrameHeight) {

	// This routine resets all data positions of the non-uniformity map array to '0'

	// Loop igennem all Non-Uniformity Map Arrayets data positioner
	for (unsigned int i = 0; i < FrameWidth * FrameHeight; i += 8) {

		// Reset the array index data to '0'
		*(OutputNonUniformityMap + (i + 0)) = 0.0;
		*(OutputNonUniformityMap + (i + 1)) = 0.0;
		*(OutputNonUniformityMap + (i + 2)) = 0.0;
		*(OutputNonUniformityMap + (i + 3)) = 0.0;
		*(OutputNonUniformityMap + (i + 4)) = 0.0;
		*(OutputNonUniformityMap + (i + 5)) = 0.0;
		*(OutputNonUniformityMap + (i + 6)) = 0.0;
		*(OutputNonUniformityMap + (i + 7)) = 0.0;

	}

}

// ----------------------------- Image Conversion Routines ----------------------------- //

void RMH_ImageConversion_ArrangeYUY2ToRGB24(unsigned char* YUY2in, unsigned char* RGBout, unsigned int FrameWidth, unsigned int FrameHeight) {

	// This routine rearranges an input image frame array in YUY2 format
	// into an image frame array in RGB24 format.

	// Omarrangere YUY2 Pixels til RGB24 Format
	for (int i = 0, j = 0; i < (FrameWidth * FrameHeight * 3); i += 6, j += 4) {

		// Write the rearranged data to the pointer
		*(RGBout + (i + 0)) = *(YUY2in + (j + 1)); // Red   - U0
		*(RGBout + (i + 1)) = *(YUY2in + (j + 1)); // Green - U0
		*(RGBout + (i + 2)) = *(YUY2in + (j + 0)); // Blue  - Y0
		*(RGBout + (i + 3)) = *(YUY2in + (j + 3)); // Red   - U1
		*(RGBout + (i + 4)) = *(YUY2in + (j + 3)); // Green - U2
		*(RGBout + (i + 5)) = *(YUY2in + (j + 2)); // Red   - Y1

	}

}

void RMH_ImageConversion_ConvertYUY2ToGrayscaleRGB24(unsigned char* YUY2in, unsigned char* GrayscaleOut, unsigned int FrameWidth, unsigned int FrameHeight) {

	// This routine converts 24-bit YUY2 to 24-bit grayscale
	// and returns the converted grayscale array through the argument pointer

	// Local variables
	unsigned char PixelValue = 0;

	// Loop through all YUY2 band pixels
	for (int i = 0, j = 0; i < (FrameWidth * FrameHeight * 3); i += 3, j += 2) {

		// Calculate the common RGB24 grayscale pixel value
		PixelValue = (unsigned char)RMH_Math_Round((0.2989 * (double)(*(YUY2in + (j + 0))))) +
				  	 (unsigned char)RMH_Math_Round((0.5870 * (double)(*(YUY2in + (j + 1))))) +
					 (unsigned char)RMH_Math_Round((0.1140 * (double)(*(YUY2in + (j + 1)))));

		// Write the grayscale pixel value to the pointer array
		*(GrayscaleOut + (i + 0)) = PixelValue;
		*(GrayscaleOut + (i + 1)) = PixelValue;
		*(GrayscaleOut + (i + 2)) = PixelValue;

	}

}

void RMH_ImageConversion_ConvertYUY2ToGrayscaleRGB8(unsigned char* YUY2in, unsigned char* GrayscaleOut, unsigned int FrameWidth, unsigned int FrameHeight) {

	// This routine converts 24-bit YUY2 to 8-bit grayscale
	// and returns the converted grayscale array through the argument pointer

	// Local variables
	unsigned char PixelValue = 0;

	// Loop through all YUY2 band pixels
	for (int i = 0, j = 0; i < (FrameWidth * FrameHeight); i += 1, j += 2) {

		// Calculate the common RGB24 grayscale pixel value
		PixelValue = (unsigned char)RMH_Math_Round((0.2989 * (double)(*(YUY2in + (j + 0))))) +
				 	 (unsigned char)RMH_Math_Round((0.5870 * (double)(*(YUY2in + (j + 1))))) +
					 (unsigned char)RMH_Math_Round((0.1140 * (double)(*(YUY2in + (j + 1)))));

		// Write the grayscale pixel value to the pointer array
		*(GrayscaleOut + (i + 0)) = PixelValue;

	}

}

void RMH_ImageConversion_ConvertRGB24ToGrayscaleRGB24(unsigned char *RGBin, unsigned char *GrayscaleOut, unsigned int FrameWidth, unsigned int FrameHeight) {

	// This routine converts 24-bit RGB to 24-bit grayscale
	// and returns the converted grayscale array through the argument pointer

	// Local variables
	unsigned char PixelValue = 0;

	// Loop through all RGB band pixels
	for (int i = 0, j = 0; i < (FrameWidth * FrameHeight * 3); i += 3, j += 3) {

		// Calculate the common RGB24 grayscale pixel value
		PixelValue = (unsigned char)RMH_Math_Round((0.2989 * (double)(*(RGBin + (j + 2))))) +
					 (unsigned char)RMH_Math_Round((0.5870 * (double)(*(RGBin + (j + 1))))) +
					 (unsigned char)RMH_Math_Round((0.1140 * (double)(*(RGBin + (j + 0)))));

		// Write the grayscale pixel value to the pointer array
		*(GrayscaleOut + (i + 0)) = PixelValue;
		*(GrayscaleOut + (i + 1)) = PixelValue;
		*(GrayscaleOut + (i + 2)) = PixelValue;

	}

}

// -------------- Color Palette Billede Processerings & Konverterings Routiner -------------- //

void RMH_ImageProcessing_ApplyColorPaletteToGrayscaleImageData(unsigned short* Data, unsigned int FrameWidth, unsigned int FrameHeight, unsigned short ColorPalette[3][16384], bool InvertColorPalette, unsigned short* MappedData) {

	// This routine applies a given color palette to the input image data.
	// This is done by using the color palette array as a look-up table for the image data
	// The routine returns the color-mapped RGB24 image data

	// Loop through the grayscale image data
	for (unsigned int i = 0, j = 0; i < (FrameWidth * FrameHeight * 3); i += 9, j += 3) {

		// Should the color palette be inverted
		if (InvertColorPalette == true) {

			// Format the image data to the color palette format - inverted color palette - 3 sub-bands at a time (fastest -> "loop unrolling")
			*(MappedData + (i + 0)) = ColorPalette[0][_ImageProcessing_ImageResolution_14Bit - (unsigned short)*(Data + j)];
			*(MappedData + (i + 1)) = ColorPalette[1][_ImageProcessing_ImageResolution_14Bit - (unsigned short)*(Data + j)];
			*(MappedData + (i + 2)) = ColorPalette[2][_ImageProcessing_ImageResolution_14Bit - (unsigned short)*(Data + j)];
			*(MappedData + (i + 3)) = ColorPalette[0][_ImageProcessing_ImageResolution_14Bit - (unsigned short)*(Data + (j + 1))];
			*(MappedData + (i + 4)) = ColorPalette[1][_ImageProcessing_ImageResolution_14Bit - (unsigned short)*(Data + (j + 1))];
			*(MappedData + (i + 5)) = ColorPalette[2][_ImageProcessing_ImageResolution_14Bit - (unsigned short)*(Data + (j + 1))];
			*(MappedData + (i + 6)) = ColorPalette[0][_ImageProcessing_ImageResolution_14Bit - (unsigned short)*(Data + (j + 2))];
			*(MappedData + (i + 7)) = ColorPalette[1][_ImageProcessing_ImageResolution_14Bit - (unsigned short)*(Data + (j + 2))];
			*(MappedData + (i + 8)) = ColorPalette[2][_ImageProcessing_ImageResolution_14Bit - (unsigned short)*(Data + (j + 2))];

		}
		else {

			// Format the image data to the color palette format - normal color palette - 3 sub-bands at a time (fastest -> "loop unrolling")
			*(MappedData + (i + 0)) = ColorPalette[0][(unsigned short)*(Data + j)];
			*(MappedData + (i + 1)) = ColorPalette[1][(unsigned short)*(Data + j)];
			*(MappedData + (i + 2)) = ColorPalette[2][(unsigned short)*(Data + j)];
			*(MappedData + (i + 3)) = ColorPalette[0][(unsigned short)*(Data + (j + 1))];
			*(MappedData + (i + 4)) = ColorPalette[1][(unsigned short)*(Data + (j + 1))];
			*(MappedData + (i + 5)) = ColorPalette[2][(unsigned short)*(Data + (j + 1))];
			*(MappedData + (i + 6)) = ColorPalette[0][(unsigned short)*(Data + (j + 2))];
			*(MappedData + (i + 7)) = ColorPalette[1][(unsigned short)*(Data + (j + 2))];
			*(MappedData + (i + 8)) = ColorPalette[2][(unsigned short)*(Data + (j + 2))];

		}

	}

}

void RMH_ImageProcessing_ApplyOverlayedPaletteToGrayScaleImageData(unsigned short* Data, unsigned short* MappedData, unsigned int FrameWidth, unsigned int FrameHeight, unsigned short BackGroundColorPalette[3][16384], unsigned short OverlayedColorPalette[3][16384], bool InvertBackGroundColorPalette, bool InvertOverlayedColorPalette, unsigned int X0Pos, unsigned int Y0Pos, unsigned int OCPWidth, unsigned int OCPHeight) {

	// This routine applies a given background color palette to the given input image data,
	// and adds an extra overlaid color palette on top of it.
	// This makes it possible to show the image data with several different color palettes at the same time.

	// Local variables
	unsigned int DataFrameRowCount = 0;
	unsigned int OverlayedPaletteStartIndex = 0;
	unsigned int OverlayedPaletteStopIndex = 0;

	// Calculate the start and stop index values for the overlaid palette
	OverlayedPaletteStartIndex = (Y0Pos * FrameWidth) + X0Pos;
	OverlayedPaletteStopIndex = (OverlayedPaletteStartIndex + OCPWidth) - 1;

	// Loop through the grayscale image data
	for (unsigned int i = 0, j = 0; i < (FrameWidth * FrameHeight * 3); i += 3, j += 1) {

		// Check whether the current index is within the index range of the overlaid color palette
		if ((j >= OverlayedPaletteStartIndex && j <= OverlayedPaletteStopIndex) && DataFrameRowCount < OCPHeight) {

			// Should the overlaid color palette be inverted
			if (InvertOverlayedColorPalette == true) {

				// Format the image data with the overlaid color palette data - inverted
				*(MappedData + (i + 0)) = OverlayedColorPalette[0][_ImageProcessing_ImageResolution_14Bit - *(Data + (j + 0))];
				*(MappedData + (i + 1)) = OverlayedColorPalette[1][_ImageProcessing_ImageResolution_14Bit - *(Data + (j + 0))];
				*(MappedData + (i + 2)) = OverlayedColorPalette[2][_ImageProcessing_ImageResolution_14Bit - *(Data + (j + 0))];

			}
			else {

				// Format the image data with the overlaid color palette data 
				*(MappedData + (i + 0)) = OverlayedColorPalette[0][*(Data + (j + 0))];
				*(MappedData + (i + 1)) = OverlayedColorPalette[1][*(Data + (j + 0))];
				*(MappedData + (i + 2)) = OverlayedColorPalette[2][*(Data + (j + 0))];

			}

			// Has the stop index value of the overlaid palette been reached - per row
			if (j >= OverlayedPaletteStopIndex) {

				// Increment the frame data row counter variable
				DataFrameRowCount = DataFrameRowCount + 1;

				// Increment to the next overlaid palette data row
				OverlayedPaletteStartIndex = OverlayedPaletteStartIndex + FrameWidth;
				OverlayedPaletteStopIndex = OverlayedPaletteStopIndex + FrameWidth;

			}

		}
		else {

			// Should the background palette be inverted
			if (InvertBackGroundColorPalette == true) {

				// Format the image data with the background color palette data - inverted
				*(MappedData + (i + 0)) = BackGroundColorPalette[0][_ImageProcessing_ImageResolution_14Bit - *(Data + (j + 0))];
				*(MappedData + (i + 1)) = BackGroundColorPalette[1][_ImageProcessing_ImageResolution_14Bit - *(Data + (j + 0))];
				*(MappedData + (i + 2)) = BackGroundColorPalette[2][_ImageProcessing_ImageResolution_14Bit - *(Data + (j + 0))];

			}
			else {

				// Format the image data with the background color palette data 
				*(MappedData + (i + 0)) = BackGroundColorPalette[0][*(Data + (j + 0))];
				*(MappedData + (i + 1)) = BackGroundColorPalette[1][*(Data + (j + 0))];
				*(MappedData + (i + 2)) = BackGroundColorPalette[2][*(Data + (j + 0))];

			}

		}

	}

}

void RMH_ImageProcessing_DisplayColorPaletteInPictureBox(unsigned char ColorPalette[3][256], System::Windows::Forms::PictureBox^ PictureBox) {

	// This routine reformats a given color palette array to an RGB array and converts it to a bitmap
	// and displays the bitmap in a given PictureBox without the usual anti-aliasing problems

	// Locally defined constants
	#define _ColorBar_Width      4      // Must be a multiple of 2!
	#define _ColorBar_Height     256    // Must be a multiple of 2!
	#define _ColorBar_RGBBands   3

	// Local variables and objects
	System::Drawing::Bitmap^ TempBitmap = gcnew Bitmap(PictureBox->Width, PictureBox->Height, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
	System::Drawing::Graphics^ BitmapGraphics = Graphics::FromImage(TempBitmap);

	// Local array for reformatting the given color palette
	unsigned char ColorPaletteRGB[_ColorBar_Width * _ColorBar_Height * _ColorBar_RGBBands] = { 0 };
	// Udregn Givet Color Palette array til RGB Color Palette array offset  
	unsigned char ColorPaletteArrayOffset = (_ColorBar_Width * _ColorBar_Height * _ColorBar_RGBBands) / _ColorBar_Height;

	// Convert and format the color palette to an RGB array
	for (int i = 0; i < (_ColorBar_Width * _ColorBar_Height * _ColorBar_RGBBands); i += 3) {

		// Write the RGB values from the given color palette array to the formatted RGB array
		ColorPaletteRGB[i + 0] = ColorPalette[2][i / ColorPaletteArrayOffset];  // Blue
		ColorPaletteRGB[i + 1] = ColorPalette[1][i / ColorPaletteArrayOffset];  // Green
		ColorPaletteRGB[i + 2] = ColorPalette[0][i / ColorPaletteArrayOffset];  // Red

	}

	// Konverter Formaterede RGB array til Bitmap
	System::Drawing::Bitmap^ ColorBarBitmap = gcnew System::Drawing::Bitmap(
		_ColorBar_Width,
		_ColorBar_Height,
		_ColorBar_RGBBands * _ColorBar_Width,
		System::Drawing::Imaging::PixelFormat::Format24bppRgb,
		IntPtr(ColorPaletteRGB));

	// Rotate the bitmap by 180 degrees - CAN BE OPTIMIZED!!
	ColorBarBitmap->RotateFlip(System::Drawing::RotateFlipType::Rotate180FlipNone);

	// Ryd Grafik objektet til default "BackColor"
	BitmapGraphics->Clear(PictureBox->BackColor);

	// Konfigurer Bitmap Interpolations metode
	BitmapGraphics->InterpolationMode = System::Drawing::Drawing2D::InterpolationMode::Bilinear;

	// Fit the color palette bitmap into the area of the PictureBox
	BitmapGraphics->DrawImage(
		ColorBarBitmap,                                          // Source bitmap image
		System::Drawing::Rectangle(0, 0, PictureBox->Width, PictureBox->Height),  // Distanations Rectangle
		0,                                                       // Source X kordinat
		0,                                                       // Source Y kordinat
		_ColorBar_Width - 1,                                     // Width of the source rectangle
		_ColorBar_Height,                                        // Height of the source rectangle
		GraphicsUnit::Pixel);                                    // Grafisk Unit Format 

    // Delete the temporary graphics object
	delete BitmapGraphics;

	// Display Formaterede Color Palette Bitmap i PictureBox
	PictureBox->Image = TempBitmap;

}

void RMH_ImageProcessing_FormatColorPaletteRangeInsideBackgroundPalette(unsigned short MainPalette[3][16384], bool MainPaletteInvertFlag, bool AdaptFullPaletteWithinRange, unsigned short BackPalette[3][16384], bool BackPaletteInvertFlag, unsigned short MainPaletteMaxRange, unsigned short MainPaletteMinRange, unsigned short (*OutputPalette)[16384]) {

	// This routine formats a given color palette on top of a background color palette, with a given range parameter

	// Local variables
	unsigned int RangedIndex = 0;
	unsigned int RangeDifference = MainPaletteMaxRange - MainPaletteMinRange;
	double RangeScale = (double)_ImageProcessing_ImageResolution_14Bit / RangeDifference;

	// Loop up to and including the length of the color palette
	for (unsigned int i = 0; i < (_ImageProcessing_ImageResolution_14Bit + 1); i++) {

		// If the index is within the primary palette max/min range
		if (i >= MainPaletteMinRange && i <= MainPaletteMaxRange) {

			// Should the whole primary color palette be adjusted to fit the set range
			if (AdaptFullPaletteWithinRange == true) {

				// Udregn Range Index for full color palette range justering
				RangedIndex = (unsigned int)(RangeScale * (i - MainPaletteMinRange));

			}
			else {

				// The range index is equal to 'i'
				RangedIndex = i;

			}

			// Write the primary palette RGB values to the output array pointer
			*(*(OutputPalette + 0) + i) = MainPalette[0][RangedIndex];
			*(*(OutputPalette + 1) + i) = MainPalette[1][RangedIndex];
			*(*(OutputPalette + 2) + i) = MainPalette[2][RangedIndex];

		}
		else {

			// Kompenser for inverterede main color palette
			if (MainPaletteInvertFlag == true) {

				// Should the background palette be inverted
				if (BackPaletteInvertFlag == true) {

					// Write the background palette RGB values to the output array pointer
					*(*(OutputPalette + 0) + i) = BackPalette[0][i];
					*(*(OutputPalette + 1) + i) = BackPalette[1][i];
					*(*(OutputPalette + 2) + i) = BackPalette[2][i];


				}
				else {

					// Write the background palette RGB values to the output array pointer
					*(*(OutputPalette + 0) + i) = BackPalette[0][_ImageProcessing_ImageResolution_14Bit - i];
					*(*(OutputPalette + 1) + i) = BackPalette[1][_ImageProcessing_ImageResolution_14Bit - i];
					*(*(OutputPalette + 2) + i) = BackPalette[2][_ImageProcessing_ImageResolution_14Bit - i];

				}

			}
			else {

				// Should the background palette be inverted
				if (BackPaletteInvertFlag == true) {

					// Write the background palette RGB values to the output array pointer
					*(*(OutputPalette + 0) + i) = BackPalette[0][_ImageProcessing_ImageResolution_14Bit - i];
					*(*(OutputPalette + 1) + i) = BackPalette[1][_ImageProcessing_ImageResolution_14Bit - i];
					*(*(OutputPalette + 2) + i) = BackPalette[2][_ImageProcessing_ImageResolution_14Bit - i];


				}
				else {

					// Write the background palette RGB values to the output array pointer
					*(*(OutputPalette + 0) + i) = BackPalette[0][i];
					*(*(OutputPalette + 1) + i) = BackPalette[1][i];
					*(*(OutputPalette + 2) + i) = BackPalette[2][i];

				}

			}

		}

	}

}

// ----------------------------- Image Processing Routines ----------------------------- //

void RMH_ImageProcessing_LinearAutomaticGainControlRaw(unsigned short *ThermalData, unsigned short *GainGrayscale, unsigned int FrameWidth, unsigned int FrameHeight, double MaxOutPixelVal, double MinOutPixelVal, double MaxInPixelVal, double MinInPixelVal) {

	// This routine implements linear automatic gain control (AGC) for an input image data array
	// The AGC image data is then passed on to the pointer array.
	// The algorithm is linearized as: y = a * x + b

	// Local variables - stored in CPU registers
	register double PixelValue1 = 0.0;
	register double PixelValue2 = 0.0;
	register double PixelValue3 = 0.0;
	register double PixelValue4 = 0.0;

	// Udregn linear skallerings faktoren (a Parameter)
	double LinearScaleFactor = ((double)MinOutPixelVal - (double)MaxOutPixelVal) / ((double)MinInPixelVal - (double)MaxInPixelVal);
	// Udregn linear Offset skalleringen (b Parameter)
	double OffsetScale = -LinearScaleFactor * (double)MaxInPixelVal + (double)MaxOutPixelVal;

	// Loop up to and including the frame resolution W * H
	for (unsigned int i = 0; i < (FrameWidth * FrameHeight); i += 4) {

		// Read the pixel values
		PixelValue1 = (double)*(ThermalData + i);
		PixelValue2 = (double)*(ThermalData + (i + 1));
		PixelValue3 = (double)*(ThermalData + (i + 2));
		PixelValue4 = (double)*(ThermalData + (i + 3));

		// Scale the pixel values by the given scaling and offset factors
		*(GainGrayscale + i) = (unsigned short)(LinearScaleFactor * PixelValue1 + OffsetScale);
		*(GainGrayscale + (i + 1)) = (unsigned short)(LinearScaleFactor * PixelValue2 + OffsetScale);
		*(GainGrayscale + (i + 2)) = (unsigned short)(LinearScaleFactor * PixelValue3 + OffsetScale);
		*(GainGrayscale + (i + 3)) = (unsigned short)(LinearScaleFactor * PixelValue4 + OffsetScale);

	}

}

unsigned char RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(unsigned char* ImageData, unsigned int ImageDataWidth, unsigned int ImageDataHeight, float XPos, float YPos) {

	// This routine handles the pixel value for a filter kernel with a coordinate outside the image data matrix

	// Local variables
	unsigned int ArrayIndex = 0;

	// For pixel values outside the image data matrix area
	if (XPos < 0.0) { XPos = 0.0; }
	if (XPos > ImageDataWidth - 1) { XPos = ImageDataWidth - 1; }
	if (YPos < 0.0) { YPos = 0.0; }
	if (YPos > ImageDataHeight - 1) { YPos = ImageDataHeight - 1; }

	// Konverter matrice index til array index
	ArrayIndex = (YPos * ImageDataWidth) + XPos;

	// Return the pixel data value
	return *(ImageData + ArrayIndex);

}

// --- Gaussian Image Filtering --->

void RMH_ImageProcessing_2DGaussian3x3KernelBlur(unsigned char* ImageData, unsigned int ImageDataWidth, unsigned int ImageDataHeight, unsigned char *BluredImage) {

	// This routine implements a 2D Gaussian kernel blur filter

	// Local variables
	unsigned int ArrayIndex = 0;
	float GaussianKernelSum = 0.0;
	float GaussianImageKernelSum = 0.0;
	unsigned int KernalMaskSize = 3 * 3;

	// Loop through all rows of the image matrix
	for (unsigned int Y = 0; Y < ImageDataHeight; Y++) {

		// Loop through all columns of the image matrix
		for (unsigned int X = 0; X < ImageDataWidth; X++) {

			// Nulstil Gaussian kernel sum variabelet
			GaussianImageKernelSum = 0;
			// Nulstil Gaussian Kernal sum variablet
			GaussianKernelSum = 0;

			// Loop over each position in the kernel mask
			for (unsigned int i = 0; i < KernalMaskSize; i++) {

				// Udregn Gaussian Kernelens Sum
				GaussianKernelSum = GaussianKernelSum + Gaussian3x3KernelMask[i];

				// Calculate the filtered 2nd-order Laplacian image data coefficient
				GaussianImageKernelSum = GaussianImageKernelSum + (Gaussian3x3KernelMask[i] * (float)RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)X + Kernel3x3MatrixXCoordinates[i], (float)Y + Kernel3x3MatrixYCoordinates[i]));

			}
			
			// Divide the calculated Gaussian blur pixel value by the kernel sum (kernel normalization)
			GaussianImageKernelSum = GaussianImageKernelSum / GaussianKernelSum;

			// Konverter matrice index til array index
			ArrayIndex = (Y * ImageDataWidth) + X;

			// Clamp the processed image pixel data to the 8-bit range
			if (GaussianImageKernelSum > 255.0) { GaussianImageKernelSum = 255.0; }
			if (GaussianImageKernelSum < 0.0) { GaussianImageKernelSum = 0.0; }

			// Write the Gaussian blur image data to the pointer array
			*(BluredImage + ArrayIndex) = (unsigned char)GaussianImageKernelSum;

		}

	}

}

// --- Unsharp Mask Billede Sharpenning Processering --->

bool RMH_ImageProcessing_GenerateUnsharpKernelMask(unsigned char KernelMaskSize, float Sigma, float *KernelMaskPointer) {

	// This routine generates a normalized unsharp kernel mask for the unsharp image filtering routine
	// Givet sigma indput er Unsharp Kernel maskens Standard diviation
	// The routine returns a status flag indicating that the kernel mask was generated correctly

	// Local variables
	float PI = 3.141592654;
	float EQDivisionPart = 0.0;
	float* KernelXCoordPointer;
	float* KernelYCoordPointer;
	float EQExponentialPart = 0.0;
	float KernalScaleFactor = 0.0;
	bool KernelMaskOKFlag = false;
	float GaussianKernalValue = 0.0;
	float MinimumKernalValue = 0xFFFF;
	bool KernelMaskGeneratedOkFlag = false;

	// Check the selected kernel mask size and set the relevant parameters
	switch (KernelMaskSize) {

		// Set the pointers to the selected filter kernel and the filter kernel X/Y coordinate matrices
		case _ImageKernelMaskFilter_Size3x3: KernelMaskOKFlag = true; KernelXCoordPointer = Kernel3x3MatrixXCoordinates; KernelYCoordPointer = Kernel3x3MatrixYCoordinates; break;
		case _ImageKernelMaskFilter_Size5x5: KernelMaskOKFlag = true; KernelXCoordPointer = Kernel5x5MatrixXCoordinates; KernelYCoordPointer = Kernel5x5MatrixYCoordinates; break;

	}

	// Check whether correct kernel mask parameters have been given
	if (KernelMaskOKFlag == true) {

		// Loop up to and including the size of the Gaussian kernel mask to be generated
		for (unsigned int i = 0; i < KernelMaskSize; i++) {

			// Udregn del stykker af samlede Gaussian Kernel formular
			EQDivisionPart = 1 / (2 * PI * pow(Sigma, 2));
			EQExponentialPart = exp(-(pow(KernelXCoordPointer[i], 2) + pow(KernelYCoordPointer[i], 2)) / (2 * pow(Sigma, 2)));

			// Calculate the total current Gaussian kernel value
			GaussianKernalValue = EQDivisionPart * EQExponentialPart;

			// Find the lowest Gaussian kernel value 
			if (GaussianKernalValue < MinimumKernalValue) {

				// Update the lowest Gaussian kernel value
				MinimumKernalValue = GaussianKernalValue;

			}

			// Udregn Gaussian kernelens Normalicerings Faktor
			KernalScaleFactor = 1.0 / MinimumKernalValue;

			// Calculate the normalized Gaussian kernel value
			*(KernelMaskPointer + i) = KernalScaleFactor * GaussianKernalValue;

		}

		// Opdater kernel maske status flag
		KernelMaskGeneratedOkFlag = true;

	}
	else {

		// Opdater kernel maske status flag
		KernelMaskGeneratedOkFlag = false;

	}

	// Retuner kernel maske genererings stauts
	return KernelMaskGeneratedOkFlag;

}

void RMH_ImageProcessing_2DUnsharpMaskKernelImageSharpening(unsigned short* ImageData, unsigned int ImageResolution, unsigned int ImageDataWidth, unsigned int ImageDataHeight, unsigned char KernelMaskSize, float *KernelMaskPointer, float SharpeningStrength, bool OutputUnsharpMaskFlag, unsigned short* SharpenedImage) {

	// This routine implements a 2-dimensional Gaussian blur unsharp kernel mask image processing technique
	// which is used to sharpen an image, with configurable strength.
	// 19-05-2023 -> The routine has been heavily optimized with "loop unrolling" by a factor of 8 = 450% faster
	// 23-07-2023 -> Opdaterede.

	/*
		
		Associated macros ->

		// Image Resolution Format Reference Macros
		#define _ImageProcessing_ImageResolution_8Bit           255
		#define _ImageProcessing_ImageResolution_14Bit          16383
		#define _ImageProcessing_ImageResolution_16Bit          65535
	
	
	*/

	// Local variables - stored in CPU registers
	register float* KernelXCoordPointer;
	register float* KernelYCoordPointer;
	register float GaussianKernelSum = 0.0;
	register float KernelToImageXPos1 = 0;
	register float KernelToImageXPos2 = 0;
	register float KernelToImageXPos3 = 0;
	register float KernelToImageXPos4 = 0;
	register float KernelToImageXPos5 = 0;
	register float KernelToImageXPos6 = 0;
	register float KernelToImageXPos7 = 0;
	register float KernelToImageXPos8 = 0;
	register float KernelToImageYPos = 0;
	register unsigned int ArrayIndex1 = 0;
	register unsigned int ArrayIndex2 = 0;
	register unsigned int ArrayIndex3 = 0;
	register unsigned int ArrayIndex4 = 0;
	register unsigned int ArrayIndex5 = 0;
	register unsigned int ArrayIndex6 = 0;
	register unsigned int ArrayIndex7 = 0;
	register unsigned int ArrayIndex8 = 0;
	register float BlurMaskPixelValue1 = 0;
	register float BlurMaskPixelValue2 = 0;
	register float BlurMaskPixelValue3 = 0;
	register float BlurMaskPixelValue4 = 0;
	register float BlurMaskPixelValue5 = 0;
	register float BlurMaskPixelValue6 = 0;
	register float BlurMaskPixelValue7 = 0;
	register float BlurMaskPixelValue8 = 0;
	register float SharpenedPixelValue1 = 0;
	register float SharpenedPixelValue2 = 0;
	register float SharpenedPixelValue3 = 0;
	register float SharpenedPixelValue4 = 0;
	register float SharpenedPixelValue5 = 0;
	register float SharpenedPixelValue6 = 0;
	register float SharpenedPixelValue7 = 0;
	register float SharpenedPixelValue8 = 0;
	register float GaussianImageKernelSum1 = 0.0;
	register float GaussianImageKernelSum2 = 0.0;
	register float GaussianImageKernelSum3 = 0.0;
	register float GaussianImageKernelSum4 = 0.0;
	register float GaussianImageKernelSum5 = 0.0;
	register float GaussianImageKernelSum6 = 0.0;
	register float GaussianImageKernelSum7 = 0.0;
	register float GaussianImageKernelSum8 = 0.0;
	register unsigned int KernelToImageIndex1 = 0;
	register unsigned int KernelToImageIndex2 = 0;
	register unsigned int KernelToImageIndex3 = 0;
	register unsigned int KernelToImageIndex4 = 0;
	register unsigned int KernelToImageIndex5 = 0;
	register unsigned int KernelToImageIndex6 = 0;
	register unsigned int KernelToImageIndex7 = 0;
	register unsigned int KernelToImageIndex8 = 0;

	// Check the selected kernel mask size and set the relevant parameters
	switch (KernelMaskSize) {

		// Set the pointers to the selected filter kernel and the filter kernel X/Y coordinate matrices
		case _ImageKernelMaskFilter_Size3x3: KernelXCoordPointer = Kernel3x3MatrixXCoordinates; KernelYCoordPointer = Kernel3x3MatrixYCoordinates; break;
		case _ImageKernelMaskFilter_Size5x5: KernelXCoordPointer = Kernel5x5MatrixXCoordinates; KernelYCoordPointer = Kernel5x5MatrixYCoordinates; break;

	}

	// Loop over each position in the kernel mask
	for (unsigned int i = 0; i < KernelMaskSize; i++) {

		// Udregn Gaussian Kernel maskens Sum
		GaussianKernelSum = GaussianKernelSum + *(KernelMaskPointer + i);  

	}

	// Division til Multiplikations konverter gaussian sum (CPU Cycle Optimering)
	GaussianKernelSum = 1.0 / GaussianKernelSum;

	// Loop through all rows of the image matrix
	for (unsigned int Y = 0; Y < ImageDataHeight; Y++) {

		// Loop through all columns of the image matrix
		for (unsigned int X = 0; X < ImageDataWidth; X += 8) {

			// Nulstil Gaussian kernel sum variabelet
			GaussianImageKernelSum1 = 0.0;
			GaussianImageKernelSum2 = 0.0;
			GaussianImageKernelSum3 = 0.0;
			GaussianImageKernelSum4 = 0.0;
			GaussianImageKernelSum5 = 0.0;
			GaussianImageKernelSum6 = 0.0;
			GaussianImageKernelSum7 = 0.0;
			GaussianImageKernelSum8 = 0.0;

			// Loop for each position in the kernel mask
			for (unsigned int i = 0; i < KernelMaskSize; i++) {

				// Calculate the kernel-to-image X and Y coordinates
				KernelToImageXPos1 = X + *(KernelXCoordPointer + i);
				KernelToImageXPos2 = (X + 1) + *(KernelXCoordPointer + i);
				KernelToImageXPos3 = (X + 2) + *(KernelXCoordPointer + i);
				KernelToImageXPos4 = (X + 3) + *(KernelXCoordPointer + i);
				KernelToImageXPos5 = (X + 4) + *(KernelXCoordPointer + i);
				KernelToImageXPos6 = (X + 5) + *(KernelXCoordPointer + i);
				KernelToImageXPos7 = (X + 6) + *(KernelXCoordPointer + i);
				KernelToImageXPos8 = (X + 7) + *(KernelXCoordPointer + i);
				KernelToImageYPos = Y + *(KernelYCoordPointer + i);

				// Compensate for pixel values outside the image data matrix area
				if (KernelToImageXPos1 < 0) { KernelToImageXPos1 = 0; } if (KernelToImageXPos1 > ImageDataWidth - 1) { KernelToImageXPos1 = ImageDataWidth - 1; }
				if (KernelToImageXPos2 < 0) { KernelToImageXPos2 = 0; } if (KernelToImageXPos2 > ImageDataWidth - 1) { KernelToImageXPos2 = ImageDataWidth - 1; }
				if (KernelToImageXPos3 < 0) { KernelToImageXPos3 = 0; } if (KernelToImageXPos3 > ImageDataWidth - 1) { KernelToImageXPos3 = ImageDataWidth - 1; }
				if (KernelToImageXPos4 < 0) { KernelToImageXPos4 = 0; } if (KernelToImageXPos4 > ImageDataWidth - 1) { KernelToImageXPos4 = ImageDataWidth - 1; }
				if (KernelToImageXPos5 < 0) { KernelToImageXPos5 = 0; } if (KernelToImageXPos5 > ImageDataWidth - 1) { KernelToImageXPos5 = ImageDataWidth - 1; }
				if (KernelToImageXPos6 < 0) { KernelToImageXPos6 = 0; } if (KernelToImageXPos6 > ImageDataWidth - 1) { KernelToImageXPos6 = ImageDataWidth - 1; }
				if (KernelToImageXPos7 < 0) { KernelToImageXPos7 = 0; } if (KernelToImageXPos7 > ImageDataWidth - 1) { KernelToImageXPos7 = ImageDataWidth - 1; }
				if (KernelToImageXPos8 < 0) { KernelToImageXPos8 = 0; } if (KernelToImageXPos8 > ImageDataWidth - 1) { KernelToImageXPos8 = ImageDataWidth - 1; }
				if (KernelToImageYPos < 0)  { KernelToImageYPos = 0;  } if (KernelToImageYPos > ImageDataHeight - 1) { KernelToImageYPos = ImageDataHeight - 1; }

				// Calculate the kernel-to-image index
				KernelToImageIndex1 = KernelToImageYPos * ImageDataWidth + KernelToImageXPos1;
				KernelToImageIndex2 = KernelToImageYPos * ImageDataWidth + KernelToImageXPos2;
				KernelToImageIndex3 = KernelToImageYPos * ImageDataWidth + KernelToImageXPos3;
				KernelToImageIndex4 = KernelToImageYPos * ImageDataWidth + KernelToImageXPos4;
				KernelToImageIndex5 = KernelToImageYPos * ImageDataWidth + KernelToImageXPos5;
				KernelToImageIndex6 = KernelToImageYPos * ImageDataWidth + KernelToImageXPos6;
				KernelToImageIndex7 = KernelToImageYPos * ImageDataWidth + KernelToImageXPos7;
				KernelToImageIndex8 = KernelToImageYPos * ImageDataWidth + KernelToImageXPos8;

				// Calculate the Gaussian unsharp kernel-to-image data sum
				GaussianImageKernelSum1 = GaussianImageKernelSum1 + *(KernelMaskPointer + i) * *(ImageData + KernelToImageIndex1);
				GaussianImageKernelSum2 = GaussianImageKernelSum2 + *(KernelMaskPointer + i) * *(ImageData + KernelToImageIndex2);
				GaussianImageKernelSum3 = GaussianImageKernelSum3 + *(KernelMaskPointer + i) * *(ImageData + KernelToImageIndex3);
				GaussianImageKernelSum4 = GaussianImageKernelSum4 + *(KernelMaskPointer + i) * *(ImageData + KernelToImageIndex4);
				GaussianImageKernelSum5 = GaussianImageKernelSum5 + *(KernelMaskPointer + i) * *(ImageData + KernelToImageIndex5);
				GaussianImageKernelSum6 = GaussianImageKernelSum6 + *(KernelMaskPointer + i) * *(ImageData + KernelToImageIndex6);
				GaussianImageKernelSum7 = GaussianImageKernelSum7 + *(KernelMaskPointer + i) * *(ImageData + KernelToImageIndex7);
				GaussianImageKernelSum8 = GaussianImageKernelSum8 + *(KernelMaskPointer + i) * *(ImageData + KernelToImageIndex8);

			}

			// Konverter matrice index til array index
			ArrayIndex1 = (Y * ImageDataWidth) + X;
			ArrayIndex2 = ArrayIndex1 + 1;
			ArrayIndex3 = ArrayIndex1 + 2;
			ArrayIndex4 = ArrayIndex1 + 3;
			ArrayIndex5 = ArrayIndex1 + 4;
			ArrayIndex6 = ArrayIndex1 + 5;
			ArrayIndex7 = ArrayIndex1 + 6;
			ArrayIndex8 = ArrayIndex1 + 7;

			// Divide (compensated division) the calculated Gaussian blur pixel value by the kernel sum (kernel normalization)
			GaussianImageKernelSum1 = GaussianImageKernelSum1 * GaussianKernelSum;
			GaussianImageKernelSum2 = GaussianImageKernelSum2 * GaussianKernelSum;
			GaussianImageKernelSum3 = GaussianImageKernelSum3 * GaussianKernelSum;
			GaussianImageKernelSum4 = GaussianImageKernelSum4 * GaussianKernelSum;
			GaussianImageKernelSum5 = GaussianImageKernelSum5 * GaussianKernelSum;
			GaussianImageKernelSum6 = GaussianImageKernelSum6 * GaussianKernelSum;
			GaussianImageKernelSum7 = GaussianImageKernelSum7 * GaussianKernelSum;
			GaussianImageKernelSum8 = GaussianImageKernelSum8 * GaussianKernelSum;

			// Calculate the unsharp mask pixel value
			BlurMaskPixelValue1 = (float)(*(ImageData + ArrayIndex1) - GaussianImageKernelSum1) * SharpeningStrength;
			BlurMaskPixelValue2 = (float)(*(ImageData + ArrayIndex2) - GaussianImageKernelSum2) * SharpeningStrength;
			BlurMaskPixelValue3 = (float)(*(ImageData + ArrayIndex3) - GaussianImageKernelSum3) * SharpeningStrength;
			BlurMaskPixelValue4 = (float)(*(ImageData + ArrayIndex4) - GaussianImageKernelSum4) * SharpeningStrength;
			BlurMaskPixelValue5 = (float)(*(ImageData + ArrayIndex5) - GaussianImageKernelSum5) * SharpeningStrength;
			BlurMaskPixelValue6 = (float)(*(ImageData + ArrayIndex6) - GaussianImageKernelSum6) * SharpeningStrength;
			BlurMaskPixelValue7 = (float)(*(ImageData + ArrayIndex7) - GaussianImageKernelSum7) * SharpeningStrength;
			BlurMaskPixelValue8 = (float)(*(ImageData + ArrayIndex8) - GaussianImageKernelSum8) * SharpeningStrength;

			// Should the unsharp mask pixel values be written to the output array
			if (OutputUnsharpMaskFlag == true) {

				// The unsharp mask pixel values are written to the output array
				SharpenedPixelValue1 = BlurMaskPixelValue1;
				SharpenedPixelValue2 = BlurMaskPixelValue2;
				SharpenedPixelValue3 = BlurMaskPixelValue3;
				SharpenedPixelValue4 = BlurMaskPixelValue4;
				SharpenedPixelValue5 = BlurMaskPixelValue5;
				SharpenedPixelValue6 = BlurMaskPixelValue6;
				SharpenedPixelValue7 = BlurMaskPixelValue7;
				SharpenedPixelValue8 = BlurMaskPixelValue8;

			}
			else {

				// Write the sharpened image processed pixel to the output array
				SharpenedPixelValue1 = *(ImageData + ArrayIndex1) + BlurMaskPixelValue1;
				SharpenedPixelValue2 = *(ImageData + ArrayIndex2) + BlurMaskPixelValue2;
				SharpenedPixelValue3 = *(ImageData + ArrayIndex3) + BlurMaskPixelValue3;
				SharpenedPixelValue4 = *(ImageData + ArrayIndex4) + BlurMaskPixelValue4;
				SharpenedPixelValue5 = *(ImageData + ArrayIndex5) + BlurMaskPixelValue5;
				SharpenedPixelValue6 = *(ImageData + ArrayIndex6) + BlurMaskPixelValue6;
				SharpenedPixelValue7 = *(ImageData + ArrayIndex7) + BlurMaskPixelValue7;
				SharpenedPixelValue8 = *(ImageData + ArrayIndex8) + BlurMaskPixelValue8;

			}

			// Clamp the processed image pixel data to the full range of the image resolution
			if (SharpenedPixelValue1 > ImageResolution) { SharpenedPixelValue1 = ImageResolution; } if (SharpenedPixelValue1 < 0.0) { SharpenedPixelValue1 = 0.0; }
			if (SharpenedPixelValue2 > ImageResolution) { SharpenedPixelValue2 = ImageResolution; } if (SharpenedPixelValue2 < 0.0) { SharpenedPixelValue2 = 0.0; }
			if (SharpenedPixelValue3 > ImageResolution) { SharpenedPixelValue3 = ImageResolution; } if (SharpenedPixelValue3 < 0.0) { SharpenedPixelValue3 = 0.0; }
			if (SharpenedPixelValue4 > ImageResolution) { SharpenedPixelValue4 = ImageResolution; } if (SharpenedPixelValue4 < 0.0) { SharpenedPixelValue4 = 0.0; }
			if (SharpenedPixelValue5 > ImageResolution) { SharpenedPixelValue5 = ImageResolution; } if (SharpenedPixelValue5 < 0.0) { SharpenedPixelValue5 = 0.0; }
			if (SharpenedPixelValue6 > ImageResolution) { SharpenedPixelValue6 = ImageResolution; } if (SharpenedPixelValue6 < 0.0) { SharpenedPixelValue6 = 0.0; }
			if (SharpenedPixelValue7 > ImageResolution) { SharpenedPixelValue7 = ImageResolution; } if (SharpenedPixelValue7 < 0.0) { SharpenedPixelValue7 = 0.0; }
			if (SharpenedPixelValue8 > ImageResolution) { SharpenedPixelValue8 = ImageResolution; } if (SharpenedPixelValue8 < 0.0) { SharpenedPixelValue8 = 0.0; }

			// Write the Gaussian blur sharpened image data to the pointer array
			*(SharpenedImage + ArrayIndex1) = (unsigned short)SharpenedPixelValue1;
			*(SharpenedImage + ArrayIndex2) = (unsigned short)SharpenedPixelValue2;
			*(SharpenedImage + ArrayIndex3) = (unsigned short)SharpenedPixelValue3;
			*(SharpenedImage + ArrayIndex4) = (unsigned short)SharpenedPixelValue4;
			*(SharpenedImage + ArrayIndex5) = (unsigned short)SharpenedPixelValue5;
			*(SharpenedImage + ArrayIndex6) = (unsigned short)SharpenedPixelValue6;
			*(SharpenedImage + ArrayIndex7) = (unsigned short)SharpenedPixelValue7;
			*(SharpenedImage + ArrayIndex8) = (unsigned short)SharpenedPixelValue8;

		}

	}

}

// --- Laplacian Image Sharpening --->

void RMH_ImageProcessing_LaplacianImageSharpening(unsigned char* ImageData, unsigned int ImageDataWidth, unsigned int ImageDataHeight, unsigned char KernelMask, float SharpeningStreangth, bool ShowFilteredMaks, unsigned char* SharpenedImage) {

	// This routine implements Laplacian kernel filtering of the given input image data
	// which is used to sharpen an image

	// Local variables
	float* KernelMaskPointer;
	float* KernelXCoordPointer;
	float* KernelYCoordPointer;
	float IILaplacianVal = 0.0;
	unsigned int ArrayIndex = 0;
	float SharpenedPixelValue = 0.0;
	unsigned int KernalMaskSize = 0;
	unsigned int KernelCenterIndex = 0;

	// Limit the selectable kernel mask input value
	if (KernelMask < _LaplacianImageSharpening_MinNmbOfKernelMasks) { KernelMask = _LaplacianImageSharpening_MinNmbOfKernelMasks; }
	if (KernelMask > _LaplacianImageSharpening_MaxNmbOfKernelMasks) { KernelMask = _LaplacianImageSharpening_MaxNmbOfKernelMasks; }

	// Selection of the filter mask
	switch (KernelMask) {

		// Set the pointers to the selected filter kernel and the filter kernel X/Y coordinate matrices, and set a fixed kernel size
		case _LaplacianImageKernel_3x3KernalMask1: KernalMaskSize = 3 * 3; KernelMaskPointer = Laplacian3x3KernelMask1; KernelXCoordPointer = Kernel3x3MatrixXCoordinates; KernelYCoordPointer = Kernel3x3MatrixYCoordinates;  break;
		case _LaplacianImageKernel_3x3KernalMask2: KernalMaskSize = 3 * 3; KernelMaskPointer = Laplacian3x3KernelMask2; KernelXCoordPointer = Kernel3x3MatrixXCoordinates; KernelYCoordPointer = Kernel3x3MatrixYCoordinates;  break;
		case _LaplacianImageKernel_5x5KernalMask1: KernalMaskSize = 5 * 5; KernelMaskPointer = Laplacian5x5KernelMask2; KernelXCoordPointer = Kernel5x5MatrixXCoordinates; KernelYCoordPointer = Kernel5x5MatrixYCoordinates;  break;

	}

	// Udregn Kernel mertricens center koefficients array index
	KernelCenterIndex = (KernalMaskSize - 1) / 2;

	// Loop through all rows of the image matrix
	for (unsigned int Y = 0; Y < ImageDataHeight; Y++) {

		// Loop through all columns of the image matrix
		for (unsigned int X = 0; X < ImageDataWidth; X++) {

			// Reset the Laplacian sharpening sum value
			IILaplacianVal = 0;

			// Loop over each position in the kernel mask
			for (unsigned int i = 0; i < KernalMaskSize; i++) {

				// Calculate the filtered 2nd-order Laplacian image data coefficient
				IILaplacianVal = IILaplacianVal + ((KernelMaskPointer[i] * SharpeningStreangth) * (float)RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)X + KernelXCoordPointer[i], (float)Y + KernelYCoordPointer[i]));

			}

			// Konverter matrice index til array index
			ArrayIndex = (Y * ImageDataWidth) + X;

			// Check the polarity of the filter mask center coefficient (avoids clipping)
			if (KernelMaskPointer[KernelCenterIndex] < 0.0) {

				// Always subtract the lowest value from the highest value
				if (IILaplacianVal > (float)ImageData[ArrayIndex]) {

					// Should only the filtered Laplacian image be shown
					if (ShowFilteredMaks == true) {

						// Calculate the sharpened image pixel value
						SharpenedPixelValue = IILaplacianVal;

					}
					else {

						// Calculate the sharpened image pixel value
						SharpenedPixelValue = IILaplacianVal - (float)ImageData[ArrayIndex];

					}

					// Clamp the sharpened image pixel data to the 8-bit range
					if (SharpenedPixelValue > 255.0) { SharpenedPixelValue = 255.0; }
					if (SharpenedPixelValue < 0.0) { SharpenedPixelValue = 0.0; }

					// Write the sharpened image data to the pointer array
					*(SharpenedImage + ArrayIndex) = (unsigned char)SharpenedPixelValue;

				}
				else {

					// Should only the filtered Laplacian image be shown
					if (ShowFilteredMaks == true) {

						// Calculate the sharpened image pixel value
						SharpenedPixelValue = IILaplacianVal;

					}
					else {

						// Calculate the sharpened image pixel value
						SharpenedPixelValue = (float)ImageData[ArrayIndex] - IILaplacianVal;

					}

					// Clamp the sharpened image pixel data to the 8-bit range
					if (SharpenedPixelValue > 255.0) { SharpenedPixelValue = 255.0; }
					if (SharpenedPixelValue < 0.0) { SharpenedPixelValue = 0.0; }

					// Write the sharpened image data to the pointer array
					*(SharpenedImage + ArrayIndex) = (unsigned char)SharpenedPixelValue;

				}

			}
			else {

				// Should only the filtered Laplacian image be shown
				if (ShowFilteredMaks == true) {

					// Calculate the sharpened image pixel value
					SharpenedPixelValue = IILaplacianVal;

				}
				else {

					// Calculate the sharpened image pixel value
					SharpenedPixelValue = IILaplacianVal + (float)ImageData[ArrayIndex];

				}

				// Clamp the sharpened image pixel data to the 8-bit range
				if (SharpenedPixelValue > 255.0) { SharpenedPixelValue = 255.0; }
				if (SharpenedPixelValue < 0.0) { SharpenedPixelValue = 0.0; }

				// Write the sharpened image data to the pointer array
				*(SharpenedImage + ArrayIndex) = (unsigned char)SharpenedPixelValue;

			}

		}

	}

}

// --- Image Median Filtering --->

void RMH_ImageProcessing_ImageMedianFiltering(unsigned char* ImageData, unsigned int ImageDataWidth, unsigned int ImageDataHeight, unsigned char *MedianFilteredImage) {

	// This routine implements a 3x3 median mask image filter

	// Local variables
	unsigned int Temp = 0;
	unsigned int ArrayIndex = 0;
	unsigned char MedianKernelArray[3 * 3];
	unsigned int MedianMaskSize = 3 * 3;

	// Loop through all rows of the image matrix
	for (unsigned int FrameRow = 0; FrameRow < ImageDataHeight; FrameRow++) {

		// Loop through all columns of the image matrix
		for (unsigned int FrameColumn = 0; FrameColumn < ImageDataWidth; FrameColumn++) {

			// Read the pixel values of the median filter kernel matrix
			MedianKernelArray[0] = RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)FrameColumn - 1, (float)FrameRow - 1);
			MedianKernelArray[1] = RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)FrameColumn, (float)FrameRow - 1);
			MedianKernelArray[2] = RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)FrameColumn + 1, (float)FrameRow - 1);
			MedianKernelArray[3] = RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)FrameColumn - 1, (float)FrameRow);
			MedianKernelArray[4] = RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)FrameColumn, (float)FrameRow);
			MedianKernelArray[5] = RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)FrameColumn + 1, (float)FrameRow);
			MedianKernelArray[6] = RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)FrameColumn - 1, (float)FrameRow + 1);
			MedianKernelArray[7] = RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)FrameColumn, (float)FrameRow + 1);
			MedianKernelArray[8] = RMH_ImageProcessing_GetKernelPixelOverlayPixelValue(&ImageData[0], ImageDataWidth, ImageDataHeight, (float)FrameColumn + 1, (float)FrameRow + 1);

			// Konverter matrice index til array index
			ArrayIndex = (FrameRow * ImageDataWidth) + FrameColumn;

			// Sort the rows of the median kernel matrix
			for (unsigned int i = 0; i < MedianMaskSize - 1; i++) {

				// Sorter Median kernel matricens kolonner
				for (unsigned int j = 0; j < MedianMaskSize - i; j++) {

					// Sort the lowest kernel values from the start to the end index
					if (MedianKernelArray[j] <= MedianKernelArray[j + 1]) {

						// Read the temporary array data and sort the kernel array
						Temp = MedianKernelArray[j];
						MedianKernelArray[j] = MedianKernelArray[j + 1];
						MedianKernelArray[j + 1] = Temp;

					}
					else {

						// Continue the outer iteration 
						continue;

					}

				}
			}

			// Write the median value of the image to the filtered image array pointer
			*(MedianFilteredImage + ArrayIndex) = MedianKernelArray[4];

		}

	}

}

// --- Image 2D Interpolation --->

void RMH_ImageProcessing_2DBilinearInterpolation(unsigned short* ImageData, unsigned int ImageDataWidth, unsigned int ImageDataHeight, unsigned int InterpolatedImageWidth, unsigned int InterpolatedImageHeight, unsigned short* InterpolatedImage) {

	// Routinen implementerer 2D Bilinear Interpolation 
	// which is used to scale an image's resolution up or down
	// The routine is specially optimized to be extremely fast when handling positive integer image data.

	// Local variables
	double WidthRatio = 0.0;
	double HeightRatio = 0.0;
	double YHeightHeight = 0.0;
	double YHeightWeight = 0.0;
	double XWidthWeight = 0.0;
	double XHeightRatioStepSize = 0.0;
	double YWidthRatioStepSize = 0.0;
	unsigned int YHeightLength = 0.0;
	unsigned int XWidthLength = 0.0;
	unsigned int XWidthHeight = 0.0;
	unsigned int YHeightLengthIndex = 0;
	unsigned int YHeightHeightIndex = 0;
	unsigned int ImagePixelValue = 0.0;

	// Check the given image data resolution parameters - calculate the height/width ratio of the interpolated image
	if (InterpolatedImageWidth > 1) { WidthRatio = ((double)ImageDataWidth - 1.0) / ((double)InterpolatedImageWidth - 1.0); } else { WidthRatio = 0; }
	if (InterpolatedImageHeight > 1) { HeightRatio = ((double)ImageDataHeight - 1.0) / ((double)InterpolatedImageHeight - 1.0); } else { HeightRatio = 0; }

	// Loop through all columns of the image matrix
	for (unsigned int X = 0; X < InterpolatedImageHeight; X++) {

		// Calculate the X fraction parameter of the height ratio
		XHeightRatioStepSize = HeightRatio * X;

		// Calculate the Y pixel length, height and weight from the associated height/length ratio
		YHeightLength = XHeightRatioStepSize;
		YHeightHeight = (unsigned int)(XHeightRatioStepSize + 0.999);  // RMH Implementering af Hurtig -> ceil(XHeightRatioStepSize);
		YHeightWeight = XHeightRatioStepSize - YHeightLength;

		// Calculate the Y height and weight indices of the image data array
		YHeightLengthIndex = YHeightLength * ImageDataWidth;
		YHeightHeightIndex = YHeightHeight * ImageDataWidth;

		// Loop through all rows of the image array
		for (unsigned int Y = 0; Y < InterpolatedImageWidth; Y++) {

			// Calculate the Y fraction parameter of the length ratio
			YWidthRatioStepSize = WidthRatio * Y;

			// Calculate the X pixel length, height and weight from the associated height/length ratio
			XWidthLength = YWidthRatioStepSize;
			XWidthHeight = (unsigned int)(YWidthRatioStepSize + 0.999); // RMH Implementering af Hurtig -> ceil(YWidthRatioStepSize);
			XWidthWeight = YWidthRatioStepSize - XWidthLength;

			// Calculate the interpolated pixel value
			ImagePixelValue = ImageData[YHeightLengthIndex + XWidthLength] * (1.0 - XWidthWeight) * (1.0 - YHeightWeight) +
							  ImageData[YHeightLengthIndex + XWidthHeight] * XWidthWeight * (1.0 - YHeightWeight) +
							  ImageData[YHeightHeightIndex + XWidthLength] * YHeightWeight * (1.0 - XWidthWeight) +
							  ImageData[YHeightHeightIndex + XWidthHeight] * XWidthWeight * YHeightWeight;

			// Write the interpolated pixel value to the given pointer array
			*(InterpolatedImage + (X * InterpolatedImageWidth + Y)) = ImagePixelValue;

		}
	}

}

// ------------------------------------------------------------------------------------------ //