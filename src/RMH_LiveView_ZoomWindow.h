#pragma once

/*
 *  RMH_LiveView_ZoomWindow.h
 *
 *  Author: Rune Mark Glendorf
 *  Date: September 2024
 *
 */

// Included libraries
#include <windows.h>
#include <GL/GLU.h>
#include <GL/GL.h>
#include <glfw3.h>
#include <iostream>
#include "RMH_Winforms_Library.h"
#include "RMH_MathConversions_Library.h"
#include "GlobalObjectsAndVariables.h"

// Associated namespaces
using namespace System;
using namespace System::Windows::Forms;
using namespace std;

// Global statisk PBO Buffere ID varaibel
static GLuint PBOIDs[2];

// OpenGL Klasse definition
namespace LiveViewZoomWindow {

	// ---------------------------------- Global Class Structure Objects --------------------------------- //

	// ----------------------------------------------------------------------------------------------------- //

	public ref class RMHLiveViewZoomWindow : public System::Windows::Forms::NativeWindow {

	private:

		// Private global class objects and variables
		private: HDC m_hDC;
		private: HGLRC m_hglrc;
		private: GLuint BaseFont;
		private: GLint iPixelFormat;
		private: GLuint* LiveViewZoomTexture;
		private: GLdouble CurrentTexturePanelWidth;
		private: GLdouble CurrentTexturePanelHeight;
		private: GLfloat TextureToPanelWidthOffset = 0.0;
		private: GLfloat TextureToPanelHeightOffset = 0.0;
		private: CreateParams^ ControlParams = gcnew CreateParams;

		// Live view zoom window rendering position variables
		private: GLdouble TextureResScaleFactor = 32;
		private: GLdouble LiveViewZoomWindowPosX0 = 0.0;
		private: GLdouble LiveViewZoomWindowPosY0 = 0.0;
		private: GLdouble LiveViewZoomWindowPosX1 = 0.0;
		private: GLdouble LiveViewZoomWindowPosY1 = 0.0;
		private: GLdouble TotalTextureScalableWidth;
		private: GLdouble TotalTextureScalableHeight;
		private: GLdouble LiveViewZoomWindowRotationDegrees = 0.0;
		private: GLdouble AspectRatioWidthOffSet;
		private: GLdouble AspectRatioHeightOffSet;
		private: GLdouble CurrentPanelWidthFixedAspect;
		private: GLdouble CurrentPanelHeightFixedAspect;
		private: unsigned int LiveViewZoomTextureWidth;
		private: unsigned int LiveViewZoomTextureHeight;
		private: unsigned int UltraResolutionTextureScaleFactor = 2;
		private: bool UntraResolutionModeEnabledFlag = false;

			   

		private: GLdouble ImageDataPixelWidth;
		private: GLdouble ImageDataPixelHeight;
		private: GLfloat MovableLineQuadrant1LabelXOffset = -10.0;
		private: GLfloat MovableLineQuadrant1LabelYOffset = 3.0;
		private: GLfloat MovableLineQuadrant2LabelXOffset = 2.0;
		private: GLfloat MovableLineQuadrant2LabelYOffset = 3.0;
		private: GLfloat MovableLineQuadrant3LabelXOffset = 2.0;
		private: GLfloat MovableLineQuadrant3LabelYOffset = -2.0;
		private: GLfloat MovableLineQuadrant4LabelXOffset = -10.0;
		private: GLfloat MovableLineQuadrant4LabelYOffset = -2.0;
		private: bool LocalAspectRatioFlag = false;
		private: GLdouble ImageDataPixelAspectRatio;
		private: bool EnableLabelBackgroundFlag = true;
		private: GLubyte CommonLabelBackgroundAlpha = 180;
		private: GLubyte LabelBackgroundColorR = 35;
		private: GLubyte LabelBackgroundColorG = 35;
		private: GLubyte LabelBackgroundColorB = 35;
		private: GLfloat LabelBackgroundXOffset = 0.2;
		private: GLfloat LabelBackgroundYOffset = 1.1;
		private: GLfloat LabelBackgroundWidth = 8.5;
		private: GLfloat LabelBackgroundHeight = 1.5;
		private: GLfloat CrosshairSize = 2.0;
		private: unsigned short CrosshairLineWidth = 2;
		private: GLubyte CommonLabelColorR = 255;
		private: GLubyte CommonLabelColorG = 255;
		private: GLubyte CommonLabelColorB = 255;
	



	public:

		// ------------------------- 2D Plot Konstruktur Routiner -------------------------- //

		RMHLiveViewZoomWindow(System::Windows::Forms::Panel^ TexturePanel) {

			// This routine is the initialization routine of the class
			
			// Calculate the total scalable texture height and width
			TotalTextureScalableWidth = 1.0 / ((GLdouble)TexturePanel->Width * TextureResScaleFactor);
			TotalTextureScalableHeight = 1.0 / ((GLdouble)TexturePanel->Height * TextureResScaleFactor);

			// Set the position of the control class
			ControlParams->X = 0;
			ControlParams->Y = 0;
			ControlParams->Width = (GLdouble)TexturePanel->Width * TextureResScaleFactor;
			ControlParams->Height = (GLdouble)TexturePanel->Height * TextureResScaleFactor;
	
			// Konfigurer Textur parent handler
			ControlParams->Parent = TexturePanel->Handle;
			// Create a "child" of the selected "parent" and make it OpenGL compliant
			ControlParams->Style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			// Generate the texture window handle
			this->CreateHandle(ControlParams);

			// Pointer to the texture handle
			m_hDC = GetDC((HWND)this->Handle.ToPointer());

			// Is this handle active
			if (m_hDC) {

				// Konfigurer Textur Pixel format
				RMH_OpenGL_SetTexturePixelFormat(m_hDC);
				// Configure the size of the texture
				RMH_OpenGL_ResizeOpenGLWinformsScene(ControlParams->Width, ControlParams->Height);
				// Initialisere OpenGL
				RMH_OpenGL_Init();

			}

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

		private: GLvoid RMH_OpenGL_ClearTextureBuffer() {

			// This routine clears the associated texture buffers

			// Ryd Textur farve og bit buffere
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		}

		private: GLvoid RMH_OpenGL_UpdateTextureFieldOfView(unsigned int FrameWidth, unsigned int FrameHeight) {

			// Routinen indstiller texturens syns vinkel for display i control handler komponentet

			// Local variables
			GLdouble PlaneXLook = 0.0;
			GLdouble PlaneYLook = 0.0;
			GLdouble PlaneFieldOfView = 60.0;
			GLdouble HalfFieldOfView = 0.5235986291;   // 3.141592654 * PlaneFieldOfView * (1 / 360); 
			GLdouble TanHalfFieldOfView = 1.732051393; // tanf(HalfFieldOfView);
			GLdouble PlaneDistance = 0.0;
			GLdouble PlaneAspectRatio = 0.0;

			// Read the height and width of the data frame
			PlaneXLook = (GLdouble)LiveViewZoomTextureWidth * 0.5;
			PlaneYLook = (GLdouble)LiveViewZoomTextureHeight * 0.5;

			// Udregn textur Aspect ratio
			PlaneAspectRatio = ((GLdouble)LiveViewZoomTextureWidth / (GLdouble)LiveViewZoomTextureHeight);

			// Udregn affstanden imellem Frame data planet og textur planet
			PlaneDistance = (GLdouble)LiveViewZoomTextureHeight * TanHalfFieldOfView;

			// Update the viewing angle (field of view) of the texture
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(PlaneFieldOfView, PlaneAspectRatio, 0.1, 500.0);
			gluLookAt(PlaneXLook, PlaneYLook, PlaneDistance * 0.5, PlaneXLook, PlaneYLook, 0, 0, 1, 0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

		}

		public: GLvoid RMH_OpenGL_InitLiveViewZoomWindow(unsigned int FrameWidth, unsigned int FrameHeight) {

			// This routine is used to set up an OpenGL texture for graphics rendering

			// Set the texture parameters and reference variables
			LiveViewZoomTexture = new GLuint[1];
			LiveViewZoomTextureWidth = FrameWidth;
			LiveViewZoomTextureHeight = FrameHeight;

			// Make the associated render context the current render context
			RMH_OpenGL_MakeRenderContextCurrent();

			//Generer Textur ID
			glGenTextures(1, LiveViewZoomTexture);

			// Bind Texturen til genereret textus ID
			glBindTexture(GL_TEXTURE_2D, LiveViewZoomTexture[0]);

			// Allocate memory for texture generation (FrameWidth * UltraResolutionTextureScaleFactor, FrameHeight * UltraResolutionTextureScaleFactor - ultra resolution)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, FrameWidth * UltraResolutionTextureScaleFactor, FrameHeight * UltraResolutionTextureScaleFactor, 0, GL_RGB, GL_UNSIGNED_SHORT, nullptr);

			// Konfigurer textur wrapping og filter indstillinger
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Unbind the texture, prevent changes to the texture
			glBindTexture(GL_TEXTURE_2D, 0);

			// ------------------------------------ Generer Og Bind PBO (Pixel Buffer Object) ------------------------------------ //

			// Generer PBOernes (Pixel Buffer Object) IDer
			__glewGenBuffers(2, PBOIDs);

			// Loop igennem begge genereret PBO Buffere (Double Buffer Objekt)
			for (int i = 0; i < 2; ++i) {

				// Bind the buffer to the associated PBO ID
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBOIDs[i]);

				// Generer nyt data lager for begge PBO buffere (GL_RGB Format W * H * RGB * Size)
				glBufferData(GL_PIXEL_UNPACK_BUFFER, (FrameWidth * UltraResolutionTextureScaleFactor) * (FrameHeight * UltraResolutionTextureScaleFactor) * 3 * sizeof(unsigned short), nullptr, GL_STREAM_DRAW);

			}

			// Afbind Pixel Buffer Objecter
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

			// ------------------------------------------------------------------------------------------------------------------- //

		}

		// ---------------- CrossHair Rendering And Handling Routines ----------------- //
	
		private: GLvoid RMH_LiveView_glPrint(const char* CharArray) {

			// This routine renders a set of characters on an OpenGL texture 

			// Add the font list properties
			glPushAttrib(GL_LIST_BIT);
			// Benyt FONT Base List
			glListBase(BaseFont - 32);
			// Execute and render the characters on the texture
			glCallLists(strlen(CharArray), GL_UNSIGNED_BYTE, CharArray);
			// Restore the list properties
			glPopAttrib();

		}

		private: GLvoid RMH_LiveView_RenderStringOnTexture(GLfloat StringX, GLfloat StringY, std::string DisplayString, GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine renders a given string on an OpenGL texture

			// Konfigurer Textens Farve
			glColor3ub(ColorR, ColorG, ColorB);

			// Set the position of the text on the texture
			glRasterPos2f(StringX, StringY);

			// Render the given string on the texture
			RMH_LiveView_glPrint(DisplayString.c_str());

		}
		
		public: GLvoid RMH_LiveView_EnableLabelBackground(bool EnableFlag) {

			// This routine enables the rendering of a background rectangle for all rendered text labels

			// Opdater Label Baggrunds aktiverings flag
			EnableLabelBackgroundFlag = EnableFlag;

		}

		public: GLvoid RMH_LiveView_ChangeRenderedLabelsColor(GLubyte LabelColorR, GLubyte LabelColorG, GLubyte LabelColorB) {

			// This routine updates the color of all rendered text labels

			// Update the color of all rendered text labels
			CommonLabelColorR = LabelColorR;
			CommonLabelColorG = LabelColorG;
			CommonLabelColorB = LabelColorB;

		}

		public: GLvoid RMH_LiveView_ChangeLabelBackgroundColor(GLubyte BackgroundColorR, GLubyte BackgroundColorG, GLubyte BackgroundColorB) {

			// Routinen opdaterer label baggrundens farve

			// Set the color of the label background
			LabelBackgroundColorR = BackgroundColorR;
			LabelBackgroundColorG = BackgroundColorG;
			LabelBackgroundColorB = BackgroundColorB;

		}

		public: GLvoid RMH_LiveView_RenderCrossHairWithLabel(GLfloat X, GLfloat Y, bool EnableLabel, System::String^ LabelString, GLubyte CrosshairColorR, GLubyte CrosshairColorG, GLubyte CrosshairColorB) {

			// This routine renders a crosshair on the texture, with or without an associated label

			// Read the temporary array data and sort the kernel array
			GLfloat QuadrantXOffset = 0.0;
			GLfloat QuadrantYOffset = 0.0;

			// Should a label be added to the crosshair
			if (EnableLabel == true) {

				// Check whether the position is in quadrant 1
				if (X >= ((GLfloat)ImageDataPixelWidth * 0.5) && Y <= ((GLfloat)ImageDataPixelHeight * 0.5)) {

					// Kontroller Live View Roterings Indstillingen
					if (LiveViewZoomWindowRotationDegrees == 0) {

						// Update the quadrant offset values - 0 degrees rotation
						QuadrantXOffset = MovableLineQuadrant1LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant1LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 90) {

						// Update the quadrant offset values - 90 degrees rotation
						QuadrantXOffset = MovableLineQuadrant2LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant2LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 180) {

						// Update the quadrant offset values - 180 degrees rotation
						QuadrantXOffset = MovableLineQuadrant3LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant3LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 270) {

						// Update the quadrant offset values - 270 degrees rotation
						QuadrantXOffset = MovableLineQuadrant4LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant4LabelYOffset;

					}

				}

				// Check whether the position is in quadrant 2
				if (X <= ((GLfloat)ImageDataPixelWidth * 0.5) && Y <= ((GLfloat)ImageDataPixelHeight * 0.5)) {

					// Kontroller Live View Roterings Indstillingen
					if (LiveViewZoomWindowRotationDegrees == 0) {

						// Update the quadrant offset values - 0 degrees rotation
						QuadrantXOffset = MovableLineQuadrant2LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant2LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 90) {

						// Update the quadrant offset values - 90 degrees rotation
						QuadrantXOffset = MovableLineQuadrant3LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant3LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 180) {

						// Update the quadrant offset values - 180 degrees rotation
						QuadrantXOffset = MovableLineQuadrant4LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant4LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 270) {

						// Update the quadrant offset values - 270 degrees rotation
						QuadrantXOffset = MovableLineQuadrant1LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant1LabelYOffset;

					}

				}

				// Check whether the position is in quadrant 3
				if (X <= ((GLfloat)ImageDataPixelWidth * 0.5) && Y >= ((GLfloat)ImageDataPixelHeight * 0.5)) {

					// Kontroller Live View Roterings Indstillingen
					if (LiveViewZoomWindowRotationDegrees == 0) {

						// Update the quadrant offset values - 0 degrees rotation
						QuadrantXOffset = MovableLineQuadrant3LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant3LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 90) {

						// Update the quadrant offset values - 90 degrees rotation
						QuadrantXOffset = MovableLineQuadrant4LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant4LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 180) {

						// Update the quadrant offset values - 180 degrees rotation
						QuadrantXOffset = MovableLineQuadrant1LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant1LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 270) {

						// Update the quadrant offset values - 270 degrees rotation
						QuadrantXOffset = MovableLineQuadrant2LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant2LabelYOffset;

					}

				}

				// Check whether the position is in quadrant 4
				if (X >= ((GLfloat)ImageDataPixelWidth * 0.5) && Y >= ((GLfloat)ImageDataPixelHeight * 0.5)) {

					// Kontroller Live View Roterings Indstillingen
					if (LiveViewZoomWindowRotationDegrees == 0) {

						// Update the quadrant offset values - 0 degrees rotation
						QuadrantXOffset = MovableLineQuadrant4LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant4LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 90) {

						// Update the quadrant offset values - 90 degrees rotation
						QuadrantXOffset = MovableLineQuadrant1LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant1LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 180) {

						// Update the quadrant offset values - 180 degrees rotation
						QuadrantXOffset = MovableLineQuadrant2LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant2LabelYOffset;

					}
					if (LiveViewZoomWindowRotationDegrees == 270) {

						// Update the quadrant offset values - 270 degrees rotation
						QuadrantXOffset = MovableLineQuadrant3LabelXOffset;
						QuadrantYOffset = MovableLineQuadrant3LabelYOffset;

					}

				}

			}

			// Check the selected aspect ratio setting
			if (LocalAspectRatioFlag == true) {

				// Kontroller Live View Roterings Indstillingen
				if (LiveViewZoomWindowRotationDegrees == 0) {

					// If horizontal aspect ratio compensation is needed
					if (LiveViewZoomWindowPosX0 <= 0.0) {

						// Calculate the crosshair Y coordinate when scaling the texture window - fixed aspect ratio mode
						Y = Y * CurrentPanelHeightFixedAspect * TotalTextureScalableHeight;
						Y = Y + AspectRatioHeightOffSet;

						// Calculate the crosshair X coordinate when scaling the texture window
						X = X * CurrentTexturePanelWidth * TotalTextureScalableWidth;

					}
					else {

						// Calculate the crosshair Y coordinate when scaling the texture window
						Y = Y * CurrentTexturePanelHeight * TotalTextureScalableHeight;

						// Calculate the crosshair X coordinate when scaling the texture window - fixed aspect ratio mode
						X = X * CurrentPanelWidthFixedAspect * TotalTextureScalableWidth;
						X = X + AspectRatioWidthOffSet;

					}

				}
				if (LiveViewZoomWindowRotationDegrees == 90) {

					// Skaller X/Y koordinater
					X = X / ImageDataPixelAspectRatio;
					Y = Y * ImageDataPixelAspectRatio;

					// If horizontal aspect ratio compensation is needed
					if (LiveViewZoomWindowPosX0 <= 0.0) {

						// Calculate the crosshair Y coordinate when scaling the texture window - fixed aspect ratio mode
						X = X * CurrentPanelHeightFixedAspect * TotalTextureScalableHeight;

						// Calculate the crosshair X coordinate when scaling the texture window
						Y = Y * CurrentTexturePanelWidth * TotalTextureScalableWidth;

					}
					else {

						// Calculate the crosshair Y coordinate when scaling the texture window
						X = X * CurrentTexturePanelHeight * TotalTextureScalableHeight;

						// Calculate the crosshair X coordinate when scaling the texture window - fixed aspect ratio mode
						Y = Y * CurrentPanelWidthFixedAspect * TotalTextureScalableWidth;
						Y = Y + AspectRatioWidthOffSet;

					}

					// Juster X Koordinat
					X = LiveViewZoomWindowPosY1 - X;

				}
				if (LiveViewZoomWindowRotationDegrees == 180) {

					// Skaller X/Y koordinater
					Y = ImageDataPixelHeight - Y;
					X = ImageDataPixelWidth - X;

					// If horizontal aspect ratio compensation is needed
					if (LiveViewZoomWindowPosX0 <= 0.0) {

						// Calculate the crosshair Y coordinate when scaling the texture window - fixed aspect ratio mode
						Y = Y * CurrentPanelHeightFixedAspect * TotalTextureScalableHeight;
						Y = Y + AspectRatioHeightOffSet;

						// Calculate the crosshair X coordinate when scaling the texture window
						X = X * CurrentTexturePanelWidth * TotalTextureScalableWidth;

					}
					else {

						// Calculate the crosshair Y coordinate when scaling the texture window
						Y = Y * CurrentTexturePanelHeight * TotalTextureScalableHeight;

						// Calculate the crosshair X coordinate when scaling the texture window - fixed aspect ratio mode
						X = X * CurrentPanelWidthFixedAspect * TotalTextureScalableWidth;
						X = X + AspectRatioWidthOffSet;

					}

				}
				if (LiveViewZoomWindowRotationDegrees == 270) {

					// Skaller X/Y koordinater
					X = (ImageDataPixelWidth - X) / ImageDataPixelAspectRatio;
					Y = (ImageDataPixelHeight - Y) * ImageDataPixelAspectRatio;

					// If horizontal aspect ratio compensation is needed
					if (LiveViewZoomWindowPosX0 <= 0.0) {

						// Calculate the crosshair Y coordinate when scaling the texture window - fixed aspect ratio mode
						X = X * CurrentPanelHeightFixedAspect * TotalTextureScalableHeight;

						// Calculate the crosshair X coordinate when scaling the texture window
						Y = Y * CurrentTexturePanelWidth * TotalTextureScalableWidth;

					}
					else {

						// Calculate the crosshair Y coordinate when scaling the texture window
						X = X * CurrentTexturePanelHeight * TotalTextureScalableHeight;

						// Calculate the crosshair X coordinate when scaling the texture window - fixed aspect ratio mode
						Y = Y * CurrentPanelWidthFixedAspect * TotalTextureScalableWidth;
						Y = Y + AspectRatioWidthOffSet;

					}

					// Juster X Koordinat
					X = LiveViewZoomWindowPosY1 - X;

				}

			}
			else {

				// Kontroller Live View Roterings Indstillingen
				if (LiveViewZoomWindowRotationDegrees == 0) {

					// Calculate the crosshair Y coordinate when scaling the texture window
					Y = Y * CurrentTexturePanelHeight * TotalTextureScalableHeight;

					// Calculate the crosshair X coordinate when scaling the texture window - auto aspect ratio mode
					X = X * CurrentTexturePanelWidth * TotalTextureScalableWidth;

				}
				if (LiveViewZoomWindowRotationDegrees == 90) {

					// Skaller X/Y koordinater
					X = X / ImageDataPixelAspectRatio;
					Y = Y * ImageDataPixelAspectRatio;

					// Calculate the crosshair Y coordinate when scaling the texture window
					Y = Y * CurrentTexturePanelWidth * TotalTextureScalableWidth;

					// Calculate the crosshair X coordinate when scaling the texture window - auto aspect ratio mode
					X = X * CurrentTexturePanelHeight * TotalTextureScalableHeight;

					// Juster X Koordinat
					X = LiveViewZoomWindowPosY1 - X;

				}
				if (LiveViewZoomWindowRotationDegrees == 180) {

					// Skaller X/Y koordinater
					Y = ImageDataPixelHeight - Y;
					X = ImageDataPixelWidth - X;

					// Calculate the crosshair Y coordinate when scaling the texture window
					Y = Y * CurrentTexturePanelHeight * TotalTextureScalableHeight;

					// Calculate the crosshair X coordinate when scaling the texture window - auto aspect ratio mode
					X = X * CurrentTexturePanelWidth * TotalTextureScalableWidth;

				}
				if (LiveViewZoomWindowRotationDegrees == 270) {

					// Skaller X/Y koordinater
					X = (ImageDataPixelWidth - X) / ImageDataPixelAspectRatio;
					Y = (ImageDataPixelHeight - Y) * ImageDataPixelAspectRatio;

					// Calculate the crosshair Y coordinate when scaling the texture window
					Y = Y * CurrentTexturePanelWidth * TotalTextureScalableWidth;

					// Calculate the crosshair X coordinate when scaling the texture window - auto aspect ratio mode
					X = X * CurrentTexturePanelHeight * TotalTextureScalableHeight;

					// Juster X Koordinat
					X = LiveViewZoomWindowPosY1 - X;

				}

			}

			// Should a background be rendered for the label
			if (EnableLabelBackgroundFlag == true) {

				// Enable OpenGL 1D texture
				glEnable(GL_TEXTURE_1D);
				glEnable(GL_BLEND);

				// Set the color of the label background
				glColor4ub(LabelBackgroundColorR, LabelBackgroundColorG, LabelBackgroundColorB, CommonLabelBackgroundAlpha);

				// Render the rectangle on the texture
				glBegin(GL_QUADS);

				// Kontroller Live View Roterings Indstillingen
				if (LiveViewZoomWindowRotationDegrees == 0) {

					// Render label rektanglens positioner
					glVertex2f((X + QuadrantXOffset) - LabelBackgroundXOffset, (Y + QuadrantYOffset) - LabelBackgroundYOffset);
					glVertex2f((X + QuadrantXOffset) - LabelBackgroundXOffset + LabelBackgroundWidth, (Y + QuadrantYOffset) - LabelBackgroundYOffset);
					glVertex2f((X + QuadrantXOffset) - LabelBackgroundXOffset + LabelBackgroundWidth, (Y + QuadrantYOffset) - LabelBackgroundYOffset + LabelBackgroundHeight);
					glVertex2f((X + QuadrantXOffset) - LabelBackgroundXOffset, (Y + QuadrantYOffset) - LabelBackgroundYOffset + LabelBackgroundHeight);

				}
				if (LiveViewZoomWindowRotationDegrees == 90) {

					// Render label rektanglens positioner
					glVertex2f((Y + QuadrantXOffset) - LabelBackgroundXOffset, (X + QuadrantYOffset) - LabelBackgroundYOffset);
					glVertex2f((Y + QuadrantXOffset) - LabelBackgroundXOffset + LabelBackgroundWidth, (X + QuadrantYOffset) - LabelBackgroundYOffset);
					glVertex2f((Y + QuadrantXOffset) - LabelBackgroundXOffset + LabelBackgroundWidth, (X + QuadrantYOffset) - LabelBackgroundYOffset + LabelBackgroundHeight);
					glVertex2f((Y + QuadrantXOffset) - LabelBackgroundXOffset, (X + QuadrantYOffset) - LabelBackgroundYOffset + LabelBackgroundHeight);

				}
				if (LiveViewZoomWindowRotationDegrees == 180) {

					// Render label rektanglens positioner
					glVertex2f((X + QuadrantXOffset) - LabelBackgroundXOffset, (Y + QuadrantYOffset) - LabelBackgroundYOffset);
					glVertex2f((X + QuadrantXOffset) - LabelBackgroundXOffset + LabelBackgroundWidth, (Y + QuadrantYOffset) - LabelBackgroundYOffset);
					glVertex2f((X + QuadrantXOffset) - LabelBackgroundXOffset + LabelBackgroundWidth, (Y + QuadrantYOffset) - LabelBackgroundYOffset + LabelBackgroundHeight);
					glVertex2f((X + QuadrantXOffset) - LabelBackgroundXOffset, (Y + QuadrantYOffset) - LabelBackgroundYOffset + LabelBackgroundHeight);

				}
				if (LiveViewZoomWindowRotationDegrees == 270) {

					// Render label rektanglens positioner
					glVertex2f((Y + QuadrantXOffset) - LabelBackgroundXOffset, (X + QuadrantYOffset) - LabelBackgroundYOffset);
					glVertex2f((Y + QuadrantXOffset) - LabelBackgroundXOffset + LabelBackgroundWidth, (X + QuadrantYOffset) - LabelBackgroundYOffset);
					glVertex2f((Y + QuadrantXOffset) - LabelBackgroundXOffset + LabelBackgroundWidth, (X + QuadrantYOffset) - LabelBackgroundYOffset + LabelBackgroundHeight);
					glVertex2f((Y + QuadrantXOffset) - LabelBackgroundXOffset, (X + QuadrantYOffset) - LabelBackgroundYOffset + LabelBackgroundHeight);

				}

				// Konfiguration Slut
				glEnd();
				// Disable the 1D texture
				glDisable(GL_TEXTURE_1D);
				glDisable(GL_BLEND);

			}

			// Should a label be added to the crosshair
			if (EnableLabel == true) {

				// Kontroller Live View Roterings Indstillingen
				if (LiveViewZoomWindowRotationDegrees == 0) {

					// Add the label to the crosshair
					RMH_LiveView_RenderStringOnTexture(X + QuadrantXOffset, Y + QuadrantYOffset, RMH_Conversion_SystemStringToStdString(LabelString), CommonLabelColorR, CommonLabelColorG, CommonLabelColorB);

				}
				if (LiveViewZoomWindowRotationDegrees == 90) {

					// Add the label to the crosshair
					RMH_LiveView_RenderStringOnTexture(Y + QuadrantXOffset, X + QuadrantYOffset, RMH_Conversion_SystemStringToStdString(LabelString), CommonLabelColorR, CommonLabelColorG, CommonLabelColorB);

				}
				if (LiveViewZoomWindowRotationDegrees == 180) {

					// Add the label to the crosshair
					RMH_LiveView_RenderStringOnTexture(X + QuadrantXOffset, Y + QuadrantYOffset, RMH_Conversion_SystemStringToStdString(LabelString), CommonLabelColorR, CommonLabelColorG, CommonLabelColorB);

				}
				if (LiveViewZoomWindowRotationDegrees == 270) {

					// Add the label to the crosshair
					RMH_LiveView_RenderStringOnTexture(Y + QuadrantXOffset, X + QuadrantYOffset, RMH_Conversion_SystemStringToStdString(LabelString), CommonLabelColorR, CommonLabelColorG, CommonLabelColorB);

				}
			}

			// Enable OpenGL 1D texture
			glEnable(GL_TEXTURE_1D);

			// Set the crosshair color
			glColor3ub(CrosshairColorR, CrosshairColorG, CrosshairColorB);

			// Set the crosshair line thickness
			glLineWidth(CrosshairLineWidth);

			// Render the line on the texture
			glBegin(GL_LINES);

			// Kontroller Live View Roterings Indstillingen
			if (LiveViewZoomWindowRotationDegrees == 0) {

				// Vertikale Linje
				glVertex2f(X, Y - (CrosshairSize * 0.5));
				glVertex2f(X, Y + (CrosshairSize * 0.5));

				// Horisontal Linje
				glVertex2f(X - (CrosshairSize * 0.5), Y);
				glVertex2f(X + (CrosshairSize * 0.5), Y);

			}
			if (LiveViewZoomWindowRotationDegrees == 90) {

				// Vertikale Linje
				glVertex2f(Y - (CrosshairSize * 0.5), X);
				glVertex2f(Y + (CrosshairSize * 0.5), X);

				// Horisontal Linje
				glVertex2f(Y, X - (CrosshairSize * 0.5));
				glVertex2f(Y, X + (CrosshairSize * 0.5));

			}
			if (LiveViewZoomWindowRotationDegrees == 180) {

				// Vertikale Linje
				glVertex2f(X, Y - (CrosshairSize * 0.5));
				glVertex2f(X, Y + (CrosshairSize * 0.5));

				// Horisontal Linje
				glVertex2f(X - (CrosshairSize * 0.5), Y);
				glVertex2f(X + (CrosshairSize * 0.5), Y);

			}
			if (LiveViewZoomWindowRotationDegrees == 270) {

				// Vertikale Linje
				glVertex2f(Y - (CrosshairSize * 0.5), X);
				glVertex2f(Y + (CrosshairSize * 0.5), X);

				// Horisontal Linje
				glVertex2f(Y, X - (CrosshairSize * 0.5));
				glVertex2f(Y, X + (CrosshairSize * 0.5));

			}

			// Konfiguration Slut
			glEnd();
			// Disable the 1D texture
			glDisable(GL_TEXTURE_1D);

		}

		// -------------- Live View Zoom Window Graphics Rendering Routine -------------- //

		private: GLvoid RMH_LiveView_WriteImageDataToZoomWindow(unsigned short* FrameData, unsigned int FrameWidth, unsigned int FrameHeight) {

			// This routine writes image data to the generated texture
			// PBO (Pixel Buffer Object) double buffer inplementering

			// Bind the buffer to the associated PBO ID
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBOIDs[0]);

			// Map the buffer data storage to a specific address space
			void* AddressSpacePointer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

			// Check whether the address pointer is valid
			if (AddressSpacePointer) {

				// Kopier "FrameData" til pointer addresse rum
				memcpy(AddressSpacePointer, FrameData, FrameWidth * FrameHeight * 3 * sizeof(unsigned short));

				// Unmap the buffer data storage from the specific address space
				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

			}

			// Bind texturen til texturets ID
			glBindTexture(GL_TEXTURE_2D, LiveViewZoomTexture[0]);
			// Update the texture with data from the PBO object
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FrameWidth, FrameHeight, GL_RGB, GL_UNSIGNED_SHORT, nullptr);

			// Afbind Pixel Buffer Objecter
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

			// Swap between PBOs (pixel buffer objects) for the next iteration
			std::swap(PBOIDs[0], PBOIDs[1]);

		}

		public: GLvoid RMH_LiveView_RenderZoomWindowTexture(GLdouble TexturePanelWidth, GLdouble TexturePanelHeight, GLdouble FrameWidth, GLdouble FrameHeight, bool FixedAspectRatio, GLfloat ZoomROIX0, GLfloat ZoomROIY0, GLfloat ZoomROIWidth, GLfloat ZoomROIHeight) {

			// This routine renders the configured texture in a given area of the total allocated texture

			// Local variables
			GLfloat TextureZoomX0 = 0.0;
			GLfloat TextureZoomY0 = 0.0;
			GLfloat TextureZoomX1 = 0.0;
			GLfloat TextureZoomY1 = 0.0;

			// Reset the live view stream image coordinates
			LiveViewZoomWindowPosX0 = 0.0;
			LiveViewZoomWindowPosY0 = 0.0;
			LiveViewZoomWindowPosX1 = 0.0;
			LiveViewZoomWindowPosY1 = 0.0;

			// Nulstil Aspect-Ratio Offset parameter
			AspectRatioWidthOffSet = 0.0;

			// Read the current pixel height and width of the texture and data panel
			ImageDataPixelWidth = FrameWidth;
			ImageDataPixelHeight = FrameHeight;
			CurrentTexturePanelWidth = TexturePanelWidth;
			CurrentTexturePanelHeight = TexturePanelHeight;
			ImageDataPixelAspectRatio = ImageDataPixelWidth / ImageDataPixelHeight;

			// Update the local class aspect ratio status flag
			LocalAspectRatioFlag = FixedAspectRatio;

			// Calculate the Y1 position of the image to fill the texture window
			LiveViewZoomWindowPosY1 = (FrameHeight * CurrentTexturePanelHeight * TotalTextureScalableHeight);

			// Check the selected aspect ratio setting
			if (FixedAspectRatio == true) {

				// Kontroller Live View Roterings Indstillingen
				if (LiveViewZoomWindowRotationDegrees == 90 || LiveViewZoomWindowRotationDegrees == 270) {

					// Calculate the aspect-ratio-compensated pixel width and height of the texture panel - for 90 and 270 degree rotation
					CurrentPanelWidthFixedAspect = (FrameHeight * CurrentTexturePanelHeight) / FrameWidth;
					CurrentPanelHeightFixedAspect = (FrameWidth * CurrentTexturePanelWidth) / FrameHeight;

				}
				else {

					// Calculate the aspect-ratio-compensated pixel width and height of the texture panel 
					CurrentPanelWidthFixedAspect = (FrameWidth * CurrentTexturePanelHeight) / FrameHeight;
					CurrentPanelHeightFixedAspect = (FrameHeight * CurrentTexturePanelWidth) / FrameWidth;

				}

				// Scale the aspect ratio width/height offset to the actual frame data aspect ratio offset - divide by 2 for the combined right and left margin image offset
				AspectRatioWidthOffSet = (FrameWidth * (CurrentTexturePanelWidth - CurrentPanelWidthFixedAspect) * TotalTextureScalableWidth) * 0.5;
				AspectRatioHeightOffSet = (FrameHeight * (CurrentTexturePanelHeight - CurrentPanelHeightFixedAspect) * TotalTextureScalableHeight) * 0.5;

				// Calculate the X1 position of the image to fill the texture window in fixed aspect ratio mode - add the X1 aspect ratio margin on the right side of the texture window
				LiveViewZoomWindowPosX1 = (FrameWidth * CurrentPanelWidthFixedAspect * TotalTextureScalableWidth) + AspectRatioWidthOffSet;

				// Add the X0 aspect ratio margin on the left side of the texture window
				LiveViewZoomWindowPosX0 = AspectRatioWidthOffSet;

				// Kompenser for fast aspect ratio i horizontal retning
				if (LiveViewZoomWindowPosX0 <= 0.0) {

					// Reset the X0 position
					LiveViewZoomWindowPosX0 = 0.0;
					// Remove the aspect ratio margin on the right side of the texture window
					LiveViewZoomWindowPosX1 = LiveViewZoomWindowPosX1 + AspectRatioWidthOffSet;

					// Add the Y1 aspect ratio margin at the bottom of the texture window
					LiveViewZoomWindowPosY1 = LiveViewZoomWindowPosY1 - AspectRatioHeightOffSet;
					// Add the Y0 aspect ratio margin at the top of the texture window
					LiveViewZoomWindowPosY0 = AspectRatioHeightOffSet;

				}

			}
			else {

				// Calculate the X1 position of the image to fill the texture window
				LiveViewZoomWindowPosX1 = (FrameWidth * CurrentTexturePanelWidth * TotalTextureScalableWidth);

			}

			// Udregn Textur Zoom parametere - Normaliseret
			TextureZoomX0 = ZoomROIX0 / FrameWidth;
			TextureZoomY0 = ZoomROIY0 / FrameHeight;
			TextureZoomX1 = (ZoomROIX0 + ZoomROIWidth) / FrameWidth;
			TextureZoomY1 = (ZoomROIY0 + ZoomROIHeight) / FrameHeight;

			// If ultra resolution mode is not enabled
			if (UntraResolutionModeEnabledFlag == false) {

				// Scale the zoom parameters to ultra resolution
				TextureZoomX0 = TextureZoomX0 * 0.5;
				TextureZoomY0 = TextureZoomY0 * 0.5;
				TextureZoomX1 = TextureZoomX1 * 0.5;
				TextureZoomY1 = TextureZoomY1 * 0.5;

			}	

			// Ryd Textur farve og bit buffere
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Enable OpenGL 2D texture
			glEnable(GL_TEXTURE_2D);
			// Bind the texture as a 2D texture
			glBindTexture(GL_TEXTURE_2D, LiveViewZoomTexture[0]);

			// Rotate the texture to match the correct image orientation
			glTranslatef(0.0f, FrameHeight, 0.0f);
			glRotatef(180.0f, 1.0f, 0.0f, 0.0f);

			// Begynd Renderering
			glBegin(GL_QUADS);

			// Kontroller Live View Roterings Indstillingen
			if (LiveViewZoomWindowRotationDegrees == 0) {

				// Opdater renderede textur kordinater - 0 Grader Rotering
				glTexCoord2f(TextureZoomX0, TextureZoomY0);
				glVertex2f(LiveViewZoomWindowPosX0, LiveViewZoomWindowPosY0);
				glTexCoord2f(TextureZoomX1, TextureZoomY0);
				glVertex2f(LiveViewZoomWindowPosX1, LiveViewZoomWindowPosY0);
				glTexCoord2f(TextureZoomX1, TextureZoomY1);
				glVertex2f(LiveViewZoomWindowPosX1, LiveViewZoomWindowPosY1);
				glTexCoord2f(TextureZoomX0, TextureZoomY1);
				glVertex2f(LiveViewZoomWindowPosX0, LiveViewZoomWindowPosY1);

			}
			else if (LiveViewZoomWindowRotationDegrees == 90) {

				// Opdater renderede textur kordinater - 90 Grader Rotering
				glTexCoord2f(TextureZoomX0, TextureZoomY0);
				glVertex2f(LiveViewZoomWindowPosX0, LiveViewZoomWindowPosY1);
				glTexCoord2f(TextureZoomX1, TextureZoomY0);
				glVertex2f(LiveViewZoomWindowPosX0, LiveViewZoomWindowPosY0);
				glTexCoord2f(TextureZoomX1, TextureZoomY1);
				glVertex2f(LiveViewZoomWindowPosX1, LiveViewZoomWindowPosY0);
				glTexCoord2f(TextureZoomX0, TextureZoomY1);
				glVertex2f(LiveViewZoomWindowPosX1, LiveViewZoomWindowPosY1);

			}
			else if (LiveViewZoomWindowRotationDegrees == 180) {

				// Opdater renderede textur kordinater - 180 Grader Rotering
				glTexCoord2f(TextureZoomX0, TextureZoomY0);
				glVertex2f(LiveViewZoomWindowPosX1, LiveViewZoomWindowPosY1);
				glTexCoord2f(TextureZoomX1, TextureZoomY0);
				glVertex2f(LiveViewZoomWindowPosX0, LiveViewZoomWindowPosY1);
				glTexCoord2f(TextureZoomX1, TextureZoomY1);
				glVertex2f(LiveViewZoomWindowPosX0, LiveViewZoomWindowPosY0);
				glTexCoord2f(TextureZoomX0, TextureZoomY1);
				glVertex2f(LiveViewZoomWindowPosX1, LiveViewZoomWindowPosY0);

			}
			else {

				// Opdater renderede textur kordinater - 270 Grader Rotering
				glTexCoord2f(TextureZoomX0, TextureZoomY0);
				glVertex2f(LiveViewZoomWindowPosX1, LiveViewZoomWindowPosY0);
				glTexCoord2f(TextureZoomX1, TextureZoomY0);
				glVertex2f(LiveViewZoomWindowPosX1, LiveViewZoomWindowPosY1);
				glTexCoord2f(TextureZoomX1, TextureZoomY1);
				glVertex2f(LiveViewZoomWindowPosX0, LiveViewZoomWindowPosY1);
				glTexCoord2f(TextureZoomX0, TextureZoomY1);
				glVertex2f(LiveViewZoomWindowPosX0, LiveViewZoomWindowPosY0);

			}

			// Konfiguration Slut
			glEnd();
			// Disable 2D texture
			glDisable(GL_TEXTURE_2D);

		}

		public: GLvoid RMH_LiveView_RenderZoomWindow(unsigned int LiveViewZoomPanelWidth, unsigned int LiveViewZoomPanelHeight, unsigned short* FrameData, unsigned int FrameDataWidth, unsigned int FrameDataHeight, bool FixedAspectRatio, GLfloat ZoomROIX0, GLfloat ZoomROIY0, GLfloat ZoomROIWidth, GLfloat ZoomROIHeight, GLdouble LiveViewRotation) {

			// Routinen Rendererer Live View Zoom Winduet

			// Opdater Live View Zoom Roteringen
			LiveViewZoomWindowRotationDegrees = LiveViewRotation;

			// Reset the local "ultra resolution mode" enabled flag
			UntraResolutionModeEnabledFlag = false;

			// ----------------------------- Render Live View Zoom Vindue ----------------------------- //
			
			// Make the associated render context the current render context
			RMH_OpenGL_MakeRenderContextCurrent();
			// Write the image data to the texture
			RMH_LiveView_WriteImageDataToZoomWindow(FrameData, FrameDataWidth, FrameDataHeight);
			// Opdater Textur Field Of View
			RMH_OpenGL_UpdateTextureFieldOfView(FrameDataWidth, FrameDataHeight);
			// Render the texture image data
			RMH_LiveView_RenderZoomWindowTexture(LiveViewZoomPanelWidth, LiveViewZoomPanelHeight, FrameDataWidth, FrameDataHeight, FixedAspectRatio, ZoomROIX0, ZoomROIY0, ZoomROIWidth, ZoomROIHeight);

			// ---------------------------------------------------------------------------------------- //

		}

		public: GLvoid RMH_LiveView_RenderZoomWindowUltraResolution(unsigned int LiveViewZoomPanelWidth, unsigned int LiveViewZoomPanelHeight, unsigned short* FrameData, unsigned int NativeFrameWidth, unsigned int NativeFrameHeight, unsigned int UltraFrameWidth, unsigned int UltraFrameHeight, bool FixedAspectRatio, GLfloat ZoomROIX0, GLfloat ZoomROIY0, GLfloat ZoomROIWidth, GLfloat ZoomROIHeight, GLdouble LiveViewRotation) {

			// Routinen Rendererer Live View Zoom Winduet

			// Opdater Live View Zoom Roteringen
			LiveViewZoomWindowRotationDegrees = LiveViewRotation;

			// Update the local "ultra resolution mode" enabled flag
			UntraResolutionModeEnabledFlag = true;

			// ----------------------------- Render Live View Zoom Vindue ----------------------------- //

			// Make the associated render context the current render context
			RMH_OpenGL_MakeRenderContextCurrent();
			// Write the image data to the texture
			RMH_LiveView_WriteImageDataToZoomWindow(FrameData, UltraFrameWidth, UltraFrameHeight);
			// Configure the resolution and field of view (FOV) of the texture
			RMH_OpenGL_UpdateTextureFieldOfView(NativeFrameWidth, NativeFrameHeight);
			// Render the texture image data
			RMH_LiveView_RenderZoomWindowTexture(LiveViewZoomPanelWidth, LiveViewZoomPanelHeight, NativeFrameWidth, NativeFrameHeight, FixedAspectRatio, ZoomROIX0, ZoomROIY0, ZoomROIWidth, ZoomROIHeight);

			// ---------------------------------------------------------------------------------------- //

		}

		// -------------------- OpenGL Renderering Slut Punkts Routiner -------------------- //

		private: GLvoid RMH_OpenGL_SwapOpenGLBuffers(GLvoid) {

			// This routine swaps the front/back buffers

			// Swap the buffers
			SwapBuffers(m_hDC);

		}

		public: GLvoid RMH_OpenGL_RenderingFinishedMark(GLvoid) {

			// This routine marks the end of an OpenGL rendering sequence
			// and must always be called last, when all object renderings have been executed

			// Swap Textur buffere
			RMH_OpenGL_SwapOpenGLBuffers();

		}

		// --------------------------------------------------------------------------------- //

	private:

		// ------------- Additional OpenGL Handling And Setup Routines ------------- //

		~RMHLiveViewZoomWindow(GLvoid) {

			// Delete the OpenGL context
			DeleteOpenGL();

			// Destruer OpenGL Handler objekt
			this->DestroyHandle();

			// Garbage Collect managed data
			System::GC::Collect();

		}

		private: GLvoid DeleteOpenGL(GLvoid) {

			// Routinen sletter alt OpenGL Context

			// Read the temporary array data and sort the kernel array
			HGLRC hglrc;
			HDC  hdc;

			// Read the thread context
			hglrc = wglGetCurrentContext();
			// Read the associated device context 
			hdc = wglGetCurrentDC();
			// Make the render context the current context
			wglMakeCurrent(NULL, NULL);
			// Frigiv Device context
			ReleaseDC(NULL, hdc);
			// Delete the render context
			wglDeleteContext(hglrc);

			// Reset the context variables
			m_hglrc = nullptr;
			m_hDC = nullptr;

		}

		private: bool RMH_OpenGL_SetTexturePixelFormat(HDC hdc) {

			// Routinen konfigurerer Texturens Pixel format

			// The format tells Windows how the texture data should be interpreted
			PIXELFORMATDESCRIPTOR pfd = {

				sizeof(PIXELFORMATDESCRIPTOR),				// Size of this pixel format descriptor
				1,											// Formatets Versions Nummer 
				PFD_DRAW_TO_WINDOW |						// The format must support Windows
				PFD_SUPPORT_OPENGL |						// The format must support OpenGL
				PFD_DOUBLEBUFFER,							// Formatet skal supporterer "Double Buffering"
				PFD_TYPE_RGBA,								// Request an RGBA format
				16,										    // Select the "color depth" (16-bit)
				0, 0, 0, 0, 0, 0,							// Color bits are to be ignored
				0,											// No "alpha buffer"
				0,											// Shift bit is to be ignored
				0,											// No "accumulation buffer"
				0, 0, 0, 0,									// Accumulator bits are to be ignored
				16,											// 16Bit Z-Buffer (Buffer dybde)  
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
			// Udregn vinduets aspect ratio
			gluPerspective(60.0f, (GLfloat)TotalTextureWidth / (GLfloat)TotalTextureHeight, 0.1, 500.0);
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

			// Generer Strutureret Font Objekt
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
			// Generer Bitmap Display FONT Liste
			wglUseFontBitmaps(m_hDC, 32, 96, BaseFont);

		}

		private: bool RMH_OpenGL_Init(GLvoid) {

			// Routinen Initialisere OpenGL I Winforms C++/CLR

			// Configure and initialize GLEW
			glewInit();

			// Enable "flat shader" mode
			glShadeModel(GL_FLAT);
			// Default Baggrund farve
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			// Set up the "depth buffer"
			glClearDepth(1.0f);
			// Disable OpenGL "depth testing"
			glDisable(GL_DEPTH_TEST);
			// For perspektiv - Fortag "Very Nice" udregniner
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

			// Generer FONT Objekt
			RMH_OpenGL_BuildFont();

			// Return the "OpenGL setup" finished flag
			return true;

		}

		// --------------------------------------------------------------------------------- //

	};

	// ------------------------------------------------------------------------------------- //

}
