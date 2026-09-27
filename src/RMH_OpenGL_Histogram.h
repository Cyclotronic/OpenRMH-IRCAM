#pragma once

/*
 *  RMH_OpenGL_Histogram.h
 *
 *  Author: Rune Mark Hansen
 *  Date: January 2023
 *  Updated: 20-07-2024
 *
 */

// Included libraries
#include <windows.h>
#include <GL/GLU.h>
#include <GL/GL.h>
#include <iostream>
#include "RMH_ThermalCameraSupport_Library.h"
#include "RMH_Winforms_Library.h"

 // Associated namespaces
using namespace System::Windows::Forms;
using namespace std;

// Static global class variables
static unsigned int HistDistributionData[16384];

// OpenGL class definition
namespace OpenGLHistogram {

	public ref class RMHOpenGLHistogram : public System::Windows::Forms::NativeWindow {

	private:

		// Histogram configuration parameters
		private: unsigned int NumberOfHistogramBins = 256;
		private: unsigned int HistgramColorPaletteResolution = 16384;
		private: unsigned int HistgramColorPaletteResolutionRange = 65535;

		// Private global class objects and variables
		private: HDC m_hDC;
		private: HGLRC m_hglrc;
		private: GLuint BaseFont;
		private: GLint iPixelFormat;
		private: GLuint* HistogramTexture;
		private: unsigned int TextureWidth;
		private: unsigned int TextureHeight;
		private: GLdouble CurrentTexturePanelWidth;
		private: GLdouble CurrentTexturePanelHeight;
		private: GLdouble TextureToPanelScaleWidthFactor;
		private: GLdouble TextureToPanelScaleHeightFactor;
		private: CreateParams^ ControlParams = gcnew CreateParams;

		// Miscellaneous histogram variables
		private: unsigned int TexturePanelTopButMargin = 22;
		private: unsigned int RefLineRightPanelOffset = 12;
		private: unsigned int HistogramX0 = 0;
		private: unsigned int HistogramY0 = 0;
		private: unsigned int HistogramY1 = 0;
		private: unsigned int HistogramMaxBinLength = 0;
		private: unsigned int HistogramHeight = 0;
		private: unsigned short (*HistogramPalettePtr)[16384];
		private: unsigned int HistHighestBinValue = 0;
		private: GLfloat HistBinDataFitScaleFactor = 1.0;
		private: bool HistogramColorPaletteInvertFlag = false;
		private: GLfloat ColorPaletteBinIndexOffsetValue = 0;

	public:

		// ------------------------ Histogram Constructor Routines ------------------------- //

		RMHOpenGLHistogram(System::Windows::Forms::Panel^ TexturePanel, unsigned char HeightScaleFactor) {

			// Set the position of the control class
			ControlParams->X = 0;
			ControlParams->Y = 0;
			ControlParams->Width = (GLdouble)TexturePanel->Width;
			ControlParams->Height = (GLdouble)TexturePanel->Height * HeightScaleFactor;

			// Configure the texture parent handler
			ControlParams->Parent = TexturePanel->Handle;
			// Create a "child" of the selected "parent" and make it OpenGL compliant
			ControlParams->Style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			// Generate the texture window handle
			this->CreateHandle(ControlParams);

			// Pointer to the texture handle
			m_hDC = GetDC((HWND)this->Handle.ToPointer());

			// Is this handle active
			if (m_hDC) {

				// Configure the texture pixel format
				RMH_OpenGL_SetTexturePixelFormat(m_hDC);
				// Configure the size of the texture
				RMH_OpenGL_ResizeOpenGLWinformsScene(ControlParams->Width, ControlParams->Height);
				// Initialize OpenGL for WinForms C++
				RMH_OpenGL_Init();

			}

			// Set up the histogram texture area for graphics rendering
			RMH_OpenGL_InitHistogramTexture(ControlParams->Width, ControlParams->Height);

		}

		// ----------------- Texture Initialization And Handling Routines ----------------- //

		private: GLvoid RMH_OpenGL_MakeRenderContextCurrent() {

			// This routine makes the associated render context the current render context

			// Make the associated render context the current render context
			wglMakeCurrent(m_hDC, m_hglrc);

		}

		private: GLvoid RMH_OpenGL_MakeRenderContextNULL() {

			// This routine resets the associated render context

			// Reset the render context
			wglMakeCurrent(NULL, NULL);

		}

		private: GLvoid RMH_OpenGL_InitHistogramTexture(unsigned int HistogramTextureWidth, unsigned int HistogramTextureHeight) {

			// This routine is used to set up an OpenGL texture for graphics rendering

			// Set the texture parameters
			HistogramTexture = new GLuint[1];
			TextureWidth = HistogramTextureWidth;
			TextureHeight = HistogramTextureHeight;

			// Use the texture that renders the histogram data
			glGenTextures(1, HistogramTexture);

			// Enable OpenGL 2D texture
			glEnable(GL_TEXTURE_2D);

			// Bind the texture as a 2D texture
			glBindTexture(GL_TEXTURE_2D, HistogramTexture[0]);

			// Configure the texture parameters
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, HistogramTextureWidth, HistogramTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			// Disable the texture
			glDisable(GL_TEXTURE_2D);

		}

		private: GLvoid RMH_OpenGL_UpdateTextureFieldOfView(unsigned int TextureWidth, unsigned int TextureHeight) {

			// This routine sets the viewing angle (field of view) of the texture for display in the control handler component

			// Local variables
			GLdouble PlaneXLook = 0.0;
			GLdouble PlaneYLook = 0.0;
			GLdouble PlaneFieldOfView = 60.0;
			GLdouble HalfFieldOfView = 0.5235986291;   // 3.141592654 * PlaneFieldOfView * (1 / 360); 
			GLdouble TanHalfFieldOfView = 1.732051393; // tanf(HalfFieldOfView);
			GLdouble PlaneDistance = 0.0;
			GLdouble PlaneAspectRatio = 0.0;

			// Read the height and width of the data frame
			PlaneXLook = (GLdouble)TextureWidth * 0.5;
			PlaneYLook = (GLdouble)TextureHeight * 0.5;

			// Calculate the texture aspect ratio
			PlaneAspectRatio = ((GLdouble)TextureWidth / (GLdouble)TextureHeight);

			// Calculate the distance between the frame data plane and the texture plane
			PlaneDistance = (GLdouble)TextureHeight * TanHalfFieldOfView;

			// Update the viewing angle (field of view) of the texture
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(PlaneFieldOfView, PlaneAspectRatio, 0.01f, 2250.0);
			gluLookAt(PlaneXLook, PlaneYLook, PlaneDistance * 0.5, PlaneXLook, PlaneYLook, 0, 0, 1, 0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

		}

		// ------------------ Label Rendering And Handling Routines ------------------- //

		private: GLvoid RMH_OpenGL_glPrint(const char* CharArray) {

			// This routine renders a set of characters on an OpenGL texture 

			// Add the font list properties
			glPushAttrib(GL_LIST_BIT);
			// Use the FONT base list
			glListBase(BaseFont - 32);
			// Execute and render the characters on the texture
			glCallLists(strlen(CharArray), GL_UNSIGNED_BYTE, CharArray);
			// Restore the list properties
			glPopAttrib();

		}

		private: GLvoid RMH_OpenGL_RenderStringOnTexture(GLfloat StringX, GLfloat StringY, std::string DisplayString, GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine renders a given string on an OpenGL texture

			// Configure the text color
			glColor3ub(ColorR, ColorG, ColorB);
			// Set the position of the text on the texture
			glRasterPos2f(StringX, StringY);

			// Render the given string on the texture
			RMH_OpenGL_glPrint(DisplayString.c_str());

		}

		// -------------------- Histogram Texture Rendering Routines --------------------- //

		private: GLvoid RMH_OpenGL_ClearTextureBuffer() {

			// This routine clears the associated texture buffers

			// Clear the texture color and bit buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		}

		private: GLvoid RMH_OpenGL_StartHistogramRender() {

			// This routine is the start state of the OpenGL histogram rendering

			// Rotate the texture to match the correct image orientation
			glTranslatef(0.0f, (GLdouble)TextureHeight, 0.0f);
			glRotatef(180.0f, 1.0f, 0.0f, 0.0f);

		}

		private: GLvoid RMH_OpenGL_RenderLine(GLfloat LineX0, GLfloat LineY0, GLfloat LineX1, GLfloat LineY1, GLfloat LineWidth, GLfloat ColorR, GLfloat ColorG, GLfloat ColorB) {

			// This routine renders a simple line with the given input parameters

			// Enable OpenGL 1D texture
			glEnable(GL_TEXTURE_1D);

			// Set the color of the arrow
			glColor3ub(ColorR, ColorG, ColorB);
			// Set the line thickness of the arrow
			glLineWidth(LineWidth);

			// Render lines on the texture
			glBegin(GL_LINES);

			// Render the primary line of the arrow
			glVertex2f(LineX0, LineY0);
			glVertex2f(LineX1, LineY1);

			// End of configuration
			glEnd();
			// Disable the 1D texture
			glDisable(GL_TEXTURE_1D);

		}

		private: GLvoid RMH_OpenGL_RenderHistogramBin(GLfloat BinX0, GLfloat BinY0, GLfloat BinHeight, GLfloat BinLength, GLfloat ColorR, GLfloat ColorG, GLfloat ColorB) {
			   
			// This routine renders a histogram bin rectangle

			// Enable OpenGL 1D texture
			glEnable(GL_TEXTURE_1D);

			// Set the polygon color
			glColor3f(ColorR / (GLfloat)HistgramColorPaletteResolutionRange, ColorG / (GLfloat)HistgramColorPaletteResolutionRange, ColorB / (GLfloat)HistgramColorPaletteResolutionRange);

			// Set the polygon line thickness
			glLineWidth(1);

			// Render the polygon on the texture
			glBegin(GL_POLYGON);

			// Render the polygon rectangle lines
			glVertex2f(BinX0, BinY0);
			glVertex2f(BinX0, BinY0 + BinHeight);
			glVertex2f(BinX0, BinY0);
			glVertex2f(BinX0 - BinLength, BinY0);
			glVertex2f(BinX0, BinY0 + BinHeight);
			glVertex2f(BinX0 - BinLength, BinY0 + BinHeight);
			glVertex2f(BinX0 - BinLength, BinY0);
			glVertex2f(BinX0 - BinLength, BinY0 + BinHeight);

			// End of configuration
			glEnd();

			// Line rendering mode
			/*glBegin(GL_LINES);
			glVertex2f(BinX0, BinY0);
			glVertex2f(BinX0, BinY0 + BinHeight);
			glVertex2f(BinX0, BinY0);
			glVertex2f(BinX0 - BinLength, BinY0);
			glVertex2f(BinX0 - BinLength, BinY0);
			glVertex2f(BinX0 - BinLength, BinY0 + BinHeight);
			glVertex2f(BinX0 - BinLength, BinY0 + BinHeight);
			glVertex2f(BinX0, BinY0 + BinHeight);
			glEnd();*/

			// Disable the 1D texture
			glDisable(GL_TEXTURE_1D);
  
		}
		
		private: GLvoid RMH_OpenGL_RenderHistogramBins(unsigned int *BinData) {

			// This routine renders the different bins of the histogram from the given input parameters

			// Read the temporary array data and sort the kernel array
			GLfloat HistogramBinHeight = 0.0;
			register GLfloat HistogramNextBinPos1 = 0.0;
			register GLfloat HistogramNextBinPos2 = 0.0;
			register GLfloat HistogramNextBinPos3 = 0.0;
			register GLfloat HistogramNextBinPos4 = 0.0;
			register GLfloat HistogramBinLengthData1 = 0.0;
			register GLfloat HistogramBinLengthData2 = 0.0;
			register GLfloat HistogramBinLengthData3 = 0.0;
			register GLfloat HistogramBinLengthData4 = 0.0;
			register unsigned int IntvertedIndex1 = 0;
			register unsigned int IntvertedIndex2 = 0;
			register unsigned int IntvertedIndex3 = 0;
			register unsigned int IntvertedIndex4 = 0;
			register unsigned int ColorPaletteBinIndexValue1 = 0;
			register unsigned int ColorPaletteBinIndexValue2 = 0;
			register unsigned int ColorPaletteBinIndexValue3 = 0;
			register unsigned int ColorPaletteBinIndexValue4 = 0;

			// Calculate the histogram bin height for each bin
			HistogramBinHeight = (GLfloat)HistogramHeight / (GLfloat)NumberOfHistogramBins;

			// Calculate the color palette index offset value of the histogram
			ColorPaletteBinIndexOffsetValue = (GLfloat)HistgramColorPaletteResolution / (GLfloat)NumberOfHistogramBins;

			// Loop up to and including the number of histogram bins
			for (unsigned int i = 0; i < NumberOfHistogramBins; i += 4) {

				// Read the histogram bin length data
				HistogramBinLengthData1 = *(BinData + ((NumberOfHistogramBins - 1) - (i + 0)));
				HistogramBinLengthData2 = *(BinData + ((NumberOfHistogramBins - 1) - (i + 1)));
				HistogramBinLengthData3 = *(BinData + ((NumberOfHistogramBins - 1) - (i + 2)));
				HistogramBinLengthData4 = *(BinData + ((NumberOfHistogramBins - 1) - (i + 3)));
			
				// Scale the histogram bin data. Fit the data to the histogram Y axis
				HistogramBinLengthData1 = HistogramBinLengthData1 * HistBinDataFitScaleFactor;
				HistogramBinLengthData2 = HistogramBinLengthData2 * HistBinDataFitScaleFactor;
				HistogramBinLengthData3 = HistogramBinLengthData3 * HistBinDataFitScaleFactor;
				HistogramBinLengthData4 = HistogramBinLengthData4 * HistBinDataFitScaleFactor;

				// Calculate the next histogram bin position
				HistogramNextBinPos1 = (GLfloat)HistogramY0 + (HistogramBinHeight * (GLfloat)(i + 0));
				HistogramNextBinPos2 = (GLfloat)HistogramY0 + (HistogramBinHeight * (GLfloat)(i + 1));
				HistogramNextBinPos3 = (GLfloat)HistogramY0 + (HistogramBinHeight * (GLfloat)(i + 2));
				HistogramNextBinPos4 = (GLfloat)HistogramY0 + (HistogramBinHeight * (GLfloat)(i + 3));

				// Calculate the actual color palette index value for the histogram bin
				ColorPaletteBinIndexValue1 = (unsigned int)((ColorPaletteBinIndexOffsetValue * ((GLfloat)(i + 0) + 1.0)) - 1.0);
				ColorPaletteBinIndexValue2 = (unsigned int)((ColorPaletteBinIndexOffsetValue * ((GLfloat)(i + 1) + 1.0)) - 1.0);
				ColorPaletteBinIndexValue3 = (unsigned int)((ColorPaletteBinIndexOffsetValue * ((GLfloat)(i + 2) + 1.0)) - 1.0);
				ColorPaletteBinIndexValue4 = (unsigned int)((ColorPaletteBinIndexOffsetValue * ((GLfloat)(i + 3) + 1.0)) - 1.0);

				// Set the minimum bin size of the histogram
				if (HistogramBinLengthData1 <= 1) { HistogramBinLengthData1 = 1; }
				if (HistogramBinLengthData2 <= 1) { HistogramBinLengthData2 = 1; }
				if (HistogramBinLengthData3 <= 1) { HistogramBinLengthData3 = 1; }
				if (HistogramBinLengthData4 <= 1) { HistogramBinLengthData4 = 1; }

				// Should the color palette of the histogram be inverted
				if (HistogramColorPaletteInvertFlag == true) {

					// Render the histogram bin rectangles vertically
					RMH_OpenGL_RenderHistogramBin(HistogramX0, HistogramNextBinPos1, HistogramBinHeight, HistogramBinLengthData1, HistogramPalettePtr[0][ColorPaletteBinIndexValue1], HistogramPalettePtr[1][ColorPaletteBinIndexValue1], HistogramPalettePtr[2][ColorPaletteBinIndexValue1]);
					RMH_OpenGL_RenderHistogramBin(HistogramX0, HistogramNextBinPos2, HistogramBinHeight, HistogramBinLengthData2, HistogramPalettePtr[0][ColorPaletteBinIndexValue2], HistogramPalettePtr[1][ColorPaletteBinIndexValue2], HistogramPalettePtr[2][ColorPaletteBinIndexValue2]);
					RMH_OpenGL_RenderHistogramBin(HistogramX0, HistogramNextBinPos3, HistogramBinHeight, HistogramBinLengthData3, HistogramPalettePtr[0][ColorPaletteBinIndexValue3], HistogramPalettePtr[1][ColorPaletteBinIndexValue3], HistogramPalettePtr[2][ColorPaletteBinIndexValue3]);
					RMH_OpenGL_RenderHistogramBin(HistogramX0, HistogramNextBinPos4, HistogramBinHeight, HistogramBinLengthData4, HistogramPalettePtr[0][ColorPaletteBinIndexValue4], HistogramPalettePtr[1][ColorPaletteBinIndexValue4], HistogramPalettePtr[2][ColorPaletteBinIndexValue4]);

				}
				else {

					// Calculate the index of the inverted color palette
					IntvertedIndex1 = (HistgramColorPaletteResolution - 1) - ColorPaletteBinIndexValue1;
					IntvertedIndex2 = (HistgramColorPaletteResolution - 1) - ColorPaletteBinIndexValue2;
					IntvertedIndex3 = (HistgramColorPaletteResolution - 1) - ColorPaletteBinIndexValue3;
					IntvertedIndex4 = (HistgramColorPaletteResolution - 1) - ColorPaletteBinIndexValue4;

					// Render the histogram bin rectangles vertically
					RMH_OpenGL_RenderHistogramBin(HistogramX0, HistogramNextBinPos1, HistogramBinHeight, HistogramBinLengthData1, HistogramPalettePtr[0][IntvertedIndex1], HistogramPalettePtr[1][IntvertedIndex1], HistogramPalettePtr[2][IntvertedIndex1]);
					RMH_OpenGL_RenderHistogramBin(HistogramX0, HistogramNextBinPos2, HistogramBinHeight, HistogramBinLengthData2, HistogramPalettePtr[0][IntvertedIndex2], HistogramPalettePtr[1][IntvertedIndex2], HistogramPalettePtr[2][IntvertedIndex2]);
					RMH_OpenGL_RenderHistogramBin(HistogramX0, HistogramNextBinPos3, HistogramBinHeight, HistogramBinLengthData3, HistogramPalettePtr[0][IntvertedIndex3], HistogramPalettePtr[1][IntvertedIndex3], HistogramPalettePtr[2][IntvertedIndex3]);
					RMH_OpenGL_RenderHistogramBin(HistogramX0, HistogramNextBinPos4, HistogramBinHeight, HistogramBinLengthData4, HistogramPalettePtr[0][IntvertedIndex4], HistogramPalettePtr[1][IntvertedIndex4], HistogramPalettePtr[2][IntvertedIndex4]);

				}
				
			}

		}

		// ---------------- Combined Histogram Graphics Rendering Routine ---------------- //

		public: GLvoid RMH_OpenGL_LoadHistogramColorPalette(unsigned short (*HistogramPalette)[16384]) {

			// This routine loads a given histogram color palette

			// Update the histogram color palette pointer
			HistogramPalettePtr = HistogramPalette;

		}

		public: GLvoid RMH_OpenGL_InvertHistogramColorPalette(bool FlagState) {

			// This routine updates the inversion flag of the histogram color palette

			// Update the inversion flag of the histogram color palette
			HistogramColorPaletteInvertFlag = FlagState;

		}

		public: void RMH_OpenGL_FormatHistogramBinDataRaw(unsigned short* RawThermalData, unsigned int DataLength, unsigned short MaxBinVal, unsigned short MinBinVal) {

			// This routine distributes the given input data into the associated histogram data bins

			// Read the temporary array data and sort the kernel array
			GLfloat BinWidth, BinIndex;

			// Reset the highest histogram bin variable
			HistHighestBinValue = 0;

			// Reset the histogram distribution values before the next iteration
			for (unsigned int i = 0; i < NumberOfHistogramBins; i++) {

				// Reset the histogram distribution values
				HistDistributionData[i] = 0;

			}

			// Calculate the histogram bin data width
			BinWidth = ((GLfloat)MaxBinVal - (GLfloat)MinBinVal) / ((GLfloat)NumberOfHistogramBins - 1.0);

			// Distribute the given data into the associated histogram bins 
			for (unsigned int i = 0; i < DataLength; i++) {

				// Calculate the histogram bin index value of the data
				BinIndex = (RawThermalData[i] - MinBinVal) / BinWidth;

				// Limit the histogram bin index value
				// to match the range of the number of histogram bins
				if (BinIndex > NumberOfHistogramBins - 1 || BinIndex < 0) {}
				else {

					// Increment the histogram bin index distribution value
					HistDistributionData[(unsigned int)BinIndex] = HistDistributionData[(unsigned int)BinIndex] + 1;

					// Read the value of the largest histogram bin 
					if (HistDistributionData[(unsigned int)BinIndex] > HistHighestBinValue) {

						// Update the highest histogram bin value
						HistHighestBinValue = HistDistributionData[(unsigned int)BinIndex];

					}

				}

			}

			// Calculate the bin data scaling factor, to fit the data to the histogram Y axis
			HistBinDataFitScaleFactor = ((GLfloat)HistogramMaxBinLength / (GLfloat)HistHighestBinValue);

		}

		public: void RMH_OpenGL_FormatHistogramBinDataTemp(ThermalCameraDevice::IRCameraDeviceFormat* IRCamera, unsigned short* RawThermalData, unsigned int DataLength, GLfloat MaxBinVal, GLfloat MinBinVal, GLfloat TempUnitScaleFactor , GLfloat TempUnitOffsetFactor, unsigned short SupportedCameraPool) {

			// This routine distributes the given input data into the associated histogram data bins

			// Read the temporary array data and sort the kernel array
			GLfloat BinWidth, BinIndex;
			GLfloat PixlTemperatureValue = 0.0f;

			// Reset the highest histogram bin variable
			HistHighestBinValue = 0;

			// Reset the histogram distribution values before the next iteration
			for (unsigned int i = 0; i < NumberOfHistogramBins; i++) {

				// Reset the histogram distribution values
				HistDistributionData[i] = 0;

			}

			// Calculate the histogram bin data width
			BinWidth = (MaxBinVal - MinBinVal) / ((GLfloat)NumberOfHistogramBins - 1.0);

			// Distribute the given data into the associated histogram bins 
			for (unsigned int i = 0; i < DataLength; i++) {

				// Read the pixel data temperature value
				PixlTemperatureValue = RMH_IRThermalCamera_ReadPixelTemperature(IRCamera, RawThermalData[i], SupportedCameraPool);

				// Compensate for the temperature unit
				PixlTemperatureValue = (PixlTemperatureValue * TempUnitScaleFactor) + TempUnitOffsetFactor;

				// Calculate the histogram bin index value of the data
				BinIndex = (PixlTemperatureValue - MinBinVal) / BinWidth;

				// Limit the histogram bin index value
				// to match the range of the number of histogram bins
				if (BinIndex > NumberOfHistogramBins - 1 || BinIndex < 0) {}
				else {

					// Increment the histogram bin index distribution value
					HistDistributionData[(unsigned int)BinIndex] = HistDistributionData[(unsigned int)BinIndex] + 1;

					// Read the value of the largest histogram bin 
					if (HistDistributionData[(unsigned int)BinIndex] > HistHighestBinValue) {

						// Update the highest histogram bin value
						HistHighestBinValue = HistDistributionData[(unsigned int)BinIndex];

					}

				}

			}

			// Calculate the bin data scaling factor, to fit the data to the histogram Y axis
			HistBinDataFitScaleFactor = ((GLfloat)HistogramMaxBinLength / (GLfloat)HistHighestBinValue);

		}

		public: void RMH_OpenGL_FormatHistogramBinDataLine(ThermalCameraDevice::IRCameraDeviceFormat* IRCamera, unsigned short* RawThermalData, GLfloat *LineXCordinates, GLfloat *LineYCordinates, unsigned int LineLength, GLfloat MaxBinVal, GLfloat MinBinVal, GLfloat TempUnitScaleFactor, GLfloat TempUnitOffsetFactor) {

			// This routine distributes the given input data into the associated histogram data bins

			// Read the temporary array data and sort the kernel array
			GLfloat BinWidth, BinIndex;
			GLfloat PixlTemperatureValue = 0.0f;

			// Reset the highest histogram bin variable
			HistHighestBinValue = 0;

			// Reset the histogram distribution values before the next iteration
			for (unsigned int i = 0; i < NumberOfHistogramBins; i++) {

				// Reset the histogram distribution values
				HistDistributionData[i] = 0;

			}

			// Calculate the histogram bin data width
			BinWidth = (MaxBinVal - MinBinVal) / ((GLfloat)NumberOfHistogramBins - 1.0);

			// Distribute the given data into the associated histogram bins 
			for (unsigned int i = 0; i < LineLength; i++) {

				// Read the pixel data temperature value
				PixlTemperatureValue = RMH_IRThermalCamera_ReadFramePixelTemperature(IRCamera, RawThermalData, LineXCordinates[i], LineYCordinates[i], IRCamera->ThermalCameraSupportPool);

				// Compensate for the temperature unit
				PixlTemperatureValue = (PixlTemperatureValue * TempUnitScaleFactor) + TempUnitOffsetFactor;

				// Calculate the histogram bin index value of the data
				BinIndex = (PixlTemperatureValue - MinBinVal) / BinWidth;

				// Limit the histogram bin index value
				// to match the range of the number of histogram bins
				if (BinIndex > NumberOfHistogramBins - 1 || BinIndex < 0) {}
				else {

					// Increment the histogram bin index distribution value
					HistDistributionData[(unsigned int)BinIndex] = HistDistributionData[(unsigned int)BinIndex] + 1;

					// Read the value of the largest histogram bin 
					if (HistDistributionData[(unsigned int)BinIndex] > HistHighestBinValue) {

						// Update the highest histogram bin value
						HistHighestBinValue = HistDistributionData[(unsigned int)BinIndex];

					}

				}

			}

			// Calculate the bin data scaling factor, to fit the data to the histogram Y axis
			HistBinDataFitScaleFactor = ((GLfloat)HistogramMaxBinLength / (GLfloat)HistHighestBinValue);

		}

		public: void RMH_OpenGL_SetHistogramNumberOfBins(System::Object^ sender) {

			// This routine sets the number of bins rendered by the histogram

			// Cast the sender object as a WinForms ToolStrip object
			System::Windows::Forms::ToolStripMenuItem^ BinsTagValue = (System::Windows::Forms::ToolStripMenuItem^)sender;
			// Read the sub context menu identification tag
			unsigned int NumberOfBins = Convert::ToInt32(BinsTagValue->Tag);

			// Set the number of bins rendered
			NumberOfHistogramBins = NumberOfBins;

		}

		public: GLvoid RMH_OpenGL_RenderHistogram(unsigned int TexturePanelWidth, unsigned int TexturePanelHeight) {

			// This routine renders all graphical objects that the histogram consists of

			// Read the current pixel height and width of the texture panel
			CurrentTexturePanelWidth = (GLdouble)TexturePanelWidth;
			CurrentTexturePanelHeight = (GLdouble)TexturePanelHeight;

			// Calculate the start positions of the histogram and the associated texture parameters
			HistogramX0 = CurrentTexturePanelWidth - RefLineRightPanelOffset;
			HistogramY0 = TexturePanelTopButMargin;
			HistogramY1 = CurrentTexturePanelHeight - TexturePanelTopButMargin;
			HistogramMaxBinLength = HistogramX0;
			HistogramHeight = HistogramY1 - HistogramY0;

			// Make the associated render context the current render context
			RMH_OpenGL_MakeRenderContextCurrent();
			// Clear the texture color and bit buffers
			RMH_OpenGL_ClearTextureBuffer();
			// Update the texture field of view
			RMH_OpenGL_UpdateTextureFieldOfView(TextureWidth, TextureHeight);

			// Bind the OpenGL texture and start OpenGL rendering
			RMH_OpenGL_StartHistogramRender();

			// ----------------------------------- Render Histogram ----------------------------------- //
			
			// Render the histogram reference line (X axis)
			RMH_OpenGL_RenderLine(HistogramX0, HistogramY0, HistogramX0, HistogramY1, 1, 0, 0, 0);

			// Renderer Histogram Bins
			RMH_OpenGL_RenderHistogramBins(&HistDistributionData[0]);

			// ---------------------------------------------------------------------------------------- //

			// Mark the end of an OpenGL rendering sequence
			RMH_OpenGL_RenderingFinishedMark();

		}

		// -------------------- OpenGL Rendering Endpoint Routines -------------------- //

		private: GLvoid RMH_OpenGL_SwapOpenGLBuffers(GLvoid) {

			// This routine swaps the front/back buffers

			// Swap the buffers
			SwapBuffers(m_hDC);

		}

		private: GLvoid RMH_OpenGL_RenderingFinishedMark(GLvoid) {

			// This routine marks the end of an OpenGL rendering sequence
			// and must always be called last, when all object renderings have been executed

			// Swap the texture buffers
			RMH_OpenGL_SwapOpenGLBuffers();

			// Reset the render context
			//this->RMH_OpenGL_MakeRenderContextNULL();

		}

		// --------------------------------------------------------------------------------- //

	private:

		// ------------- Additional OpenGL Handling And Setup Routines ------------- //

		~RMHOpenGLHistogram(GLvoid) {

			// Delete the OpenGL context
			DeleteOpenGL();

			// Destroy the OpenGL handler object
			this->DestroyHandle();

			// Garbage Collect managed data
			System::GC::Collect();

		}

		private: GLvoid DeleteOpenGL(GLvoid) {

			// This routine deletes the whole OpenGL context

			// Read the temporary array data and sort the kernel array
			HGLRC hglrc;
			HDC  hdc;

			// Read the thread context
			hglrc = wglGetCurrentContext();
			// Read the associated device context 
			hdc = wglGetCurrentDC();
			// Make the render context the current context
			wglMakeCurrent(NULL, NULL);
			// Release the device context
			ReleaseDC(NULL, hdc);
			// Delete the render context
			wglDeleteContext(hglrc);

			// Reset the context variables
			m_hglrc = nullptr;
			m_hDC = nullptr;

		}

		private: bool RMH_OpenGL_SetTexturePixelFormat(HDC hdc) {

			// This routine configures the pixel format of the texture

			// The format tells Windows how the texture data should be interpreted
			PIXELFORMATDESCRIPTOR pfd = {

				sizeof(PIXELFORMATDESCRIPTOR),				// Size of this pixel format descriptor
				1,											// Version number of the format 
				PFD_DRAW_TO_WINDOW |						// The format must support Windows
				PFD_SUPPORT_OPENGL |						// The format must support OpenGL
				PFD_DOUBLEBUFFER,							// The format must support "double buffering"
				PFD_TYPE_RGBA,								// Request an RGBA format
				16,										    // Select the "color depth" (16-bit)
				0, 0, 0, 0, 0, 0,							// Color bits are to be ignored
				0,											// No "alpha buffer"
				0,											// Shift bit is to be ignored
				0,											// No "accumulation buffer"
				0, 0, 0, 0,									// Accumulator bits are to be ignored
				16,											// 16-bit Z-buffer (buffer depth)  
				0,											// No "stencil buffer"
				0,											// No "auxiliary buffer"
				PFD_MAIN_PLANE,								// Set as the main "drawing" layer
				0,											// Reserved
				0, 0, 0										// Layer "masks" are to be ignored

			};

			// Select the pixel format for the control handler
			if ((iPixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0) {
				// Write error message - if an error is registered
				MessageBox::Show("ChoosePixelFormat Failed");
				// Return error
				return false;
			}

			// Set the pixel format for the control handler 
			if (SetPixelFormat(hdc, iPixelFormat, &pfd) == FALSE) {
				// Write error message - if an error is registered
				MessageBox::Show("SetPixelFormat Failed");
				// Return error
				return false;
			}

			if ((m_hglrc = wglCreateContext(hdc)) == NULL) {
				// Write error message - if an error is registered
				MessageBox::Show("wglCreateContext Failed");
				// Return error
				return false;
			}

			if ((wglMakeCurrent(hdc, m_hglrc)) == NULL) {
				// Write error message - if an error is registered
				MessageBox::Show("wglMakeCurrent Failed");
				// Return error
				return false;
			}

			// Return status OK
			return true;
		}

		private: GLvoid RMH_OpenGL_ResizeOpenGLWinformsScene(unsigned int TotalTextureWidth, unsigned int TotalTextureHeight) {

			// Format the size and initialize the OpenGL window in WinForms

			// Prevent division by '0'
			if (TotalTextureHeight == 0) {
				// The pixel height is always at least '1'
				TotalTextureHeight = 1;
			}

			// Reset the current "viewport"
			glViewport(0, 0, TotalTextureWidth, TotalTextureHeight);
			// Select the projection matrix
			glMatrixMode(GL_PROJECTION);
			// Reset the projection matrix
			glLoadIdentity();
			// Calculate the aspect ratio of the window
			gluPerspective(60.0f, (GLfloat)TotalTextureWidth / (GLfloat)TotalTextureHeight, 0.01f, 10000.0f);
			// Select the "model view" matrix
			glMatrixMode(GL_MODELVIEW);
			// Reset the "model view" matrix
			glLoadIdentity();

		}

		private: GLvoid RMH_OpenGL_BuildFont(GLvoid) {

			// This routine generates the FONT for display in the OpenGL texture

			// Read the temporary array data and sort the kernel array
			HFONT TextureFont;

			// Update the font list
			BaseFont = glGenLists(96);

			// Generate the structured font object
			TextureFont = CreateFont(
				-12,                            // nHeight
				0,								// nWidth
				0,								// nEscapement
				0,				     			// nOrientation
				FW_BOLD,						// nWeight
				FALSE,							// bItalic
				FALSE,							// bUnderline
				FALSE,							// cStrikeOut
				ANSI_CHARSET,					// nCharSet
				OUT_TT_PRECIS,					// nOutPrecision
				CLIP_DEFAULT_PRECIS,			// nClipPrecision
				ANTIALIASED_QUALITY,			// nQuality
				FF_ROMAN | DEFAULT_PITCH,		// nPitchAndFamily
				L"Arial");				        // lpszFacename

			// Set the FONT to the OpenGL object structure
			SelectObject(m_hDC, TextureFont);
			// Generate the bitmap display FONT list
			wglUseFontBitmaps(m_hDC, 32, 96, BaseFont);

		}

		private: bool RMH_OpenGL_Init(GLvoid) {

			// This routine initializes OpenGL in WinForms C++/CLR

			// Enable "flat shader" mode
			glShadeModel(GL_SMOOTH);
			// Default background color
			glClearColor(0.13725f, 0.13725f, 0.13725f, 1.0f);
			// Set up the "depth buffer"
			glClearDepth(1.0f);
			// Disable OpenGL "depth testing"
			glDisable(GL_DEPTH_TEST);
			// For perspective - use "very nice" calculations
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

			// Generate the FONT object
			RMH_OpenGL_BuildFont();

			// Return the "OpenGL setup" finished flag
			return true;

		}

		// --------------------------------------------------------------------------------- //

	};

	// ------------------------------------------------------------------------------------- //

}