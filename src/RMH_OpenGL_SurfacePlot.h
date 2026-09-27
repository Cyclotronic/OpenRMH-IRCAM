#pragma once

/*
 *  RMH_OpenGL_SurfacePlot.h
 *
 *  Author: Rune Mark Hansen
 *  Date: February 2023
 *
 */

// Included libraries
#include "RMH_MathConversions_Library.h"
#include <windows.h>
#include <GL/GLU.h>
#include <GL/GL.h>
#include <iostream>
#include "RMH_Winforms_Library.h"

// Surface plot default start position macros 
#define _SurfacePlotStartDefault_XPos     0
#define _SurfacePlotStartDefault_YPos     0
#define _SurfacePlotStartDefault_ZPos     0
#define _SurfacePlotStartAngle_XAngle     0
#define _SurfacePlotStartAngle_YAngle     0

// Maximum Z height configuration macros of the 3D surface plot
#define _SurfacePlotDefaultMaximumZHeight    0.5   // 50% of the panel height

// Associated namespaces
using namespace System;
using namespace System::Windows::Forms;
using namespace std;

// OpenGL class definition
namespace OpenGLSurfacePlot {

	// ------------------------- Private Custom WinForms Transparent Panel Class ------------------------- //

	// Associated local class namespace object
	namespace NativeForm = System::Windows::Forms;

	// Transparent overlay panel class for the image rendering panel
	private ref class TextureOverlayPanel : System::Windows::Forms::Panel {

		// Local class objects
		protected: System::Drawing::Graphics^ graphics;

		protected: virtual property NativeForm::CreateParams^ CreateParams {

			// Override the configuration parameters of the panel
			NativeForm::CreateParams^ get() override {

				// Read the control parameters of the panel
				NativeForm::CreateParams^ PanalParams = __super::CreateParams;
				// The extended style of the panel must be transparent
				PanalParams->ExStyle |= WS_EX_TRANSPARENT;

				// Return the config parameters of the panel
				return PanalParams;

			}

		}

		public: TextureOverlayPanel() {

			// Transparent overlay panel class constructor

		}

		virtual void OnPaintBackground(PaintEventArgs^ e) override {

			// No background should be drawn

		}

		protected: virtual void OnPaint(PaintEventArgs^ e) override {

			// No additional graphics should be generated for the panel 

		}

	};

	// ----------------------------------------------------------------------------------------------------- //

	public ref class RMHOpenGLSurfacePlot : public System::Windows::Forms::NativeWindow {

	private:

		// Private global class objects and variables
		private: HDC m_hDC;
		private: HGLRC m_hglrc;
		private: GLuint BaseFont;
		private: GLint iPixelFormat;
		private: GLuint* SurfacePlotTexture;
		private: GLdouble ImageDataPixelWidth;
		private: GLdouble ImageDataPixelHeight;
		private: unsigned int OpenGLWindowWidth;
		private: unsigned int OpenGLWindowHeight;
		private: bool OverlayPanelIsClick = false;
		private: GLdouble CurrentTexturePanelWidth;
		private: GLdouble CurrentTexturePanelHeight;
		private: GLfloat TextureToPanelWidthOffset = 0.0;
		private: GLfloat TextureToPanelHeightOffset = 0.0;
		private: CreateParams^ ControlParams = gcnew CreateParams;
		private: TextureOverlayPanel^ OverlayPanel = gcnew TextureOverlayPanel();

		// General surface plot objects and variables
		private: GLdouble SurfacePlotAspectRatio = 0.0;
		private: GLdouble AspectRatioCompensatedWidth = 0.0;
		private: GLdouble TranstaledYSurfacePlotValue = 0.0;
		private: GLfloat SurfaceZDataToZHeightScaleFactor = 0.0;
		private: bool RightMouseButtonClicked = false;
		private: bool MouseWheelButtonClicked = false;
		private: bool LeftMouseButtonClicked = false;
		private: bool XAxesMouseMoveZRotationStateFlag = true;
		private: GLdouble SurfacePlotXTranstaledPosClickOffset;
		private: GLdouble SurfacePlotYTranstaledPosClickOffset;
		private: GLdouble SurfacePlotYXTranstaledPosClickOffset;
		private: GLfloat SurfacePlotXAngleClickOffset = 0.0;
		private: GLfloat SurfacePlotYAngleClickOffset = 0.0;
		private: GLfloat CurrentSurfacePlotXTranstaledPos = 0.0;
		private: GLfloat CurrentSurfacePlotYTranstaledPos = 0.0;
		private: GLfloat CurrentSurfacePlotZTranstaledPos = 0.0;
		private: GLfloat CurrentSurfacePlotYAngle = 0.0;
		private: GLfloat CurrentSurfacePlotXAngle = 0.0;
		private: GLfloat CurrentSurfacePlotYXAngle = 0.0;
		private: GLfloat SurfacePlotMaxZHeight = _SurfacePlotDefaultMaximumZHeight;
		private: GLfloat SurfacePlotXTranstaledPos = _SurfacePlotStartDefault_XPos;
		private: GLfloat SurfacePlotYTranstaledPos = _SurfacePlotStartDefault_YPos;
		private: GLfloat SurfacePlotZTranstaledPos = _SurfacePlotStartDefault_ZPos;
		private: GLfloat SurfacePlotXAngle = _SurfacePlotStartAngle_XAngle;
		private: GLfloat SurfacePlotYAngle = _SurfacePlotStartAngle_YAngle;
		private: GLfloat SurfacePlotYXAngle = _SurfacePlotStartAngle_XAngle;
		private: GLfloat PointPolygonModePointSize = 1.0;
		private: GLfloat LinePolygonModeLineSize = 1.0;

	public:

		// ----------------------- Surface Plot Constructor Routines ----------------------- //

		RMHOpenGLSurfacePlot(System::Windows::Forms::Panel^ TexturePanel, unsigned char WidthScaleFactor, unsigned char HeightScaleFactor) {

			// Set the position of the control class
			ControlParams->X = 0;
			ControlParams->Y = 0;
			ControlParams->Width = (GLdouble)TexturePanel->Width * WidthScaleFactor;
			ControlParams->Height = (GLdouble)TexturePanel->Height * HeightScaleFactor;

			// Read the pixel width and height of the OpenGL window
			OpenGLWindowWidth = ControlParams->Width;
			OpenGLWindowHeight = ControlParams->Height;

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
				// Initialize OpenGL
				RMH_OpenGL_Init();

			}

			// Add an overlaid transparent panel to the texture panel
			RMH_OpenGL_AddOverlayPanelToMainTexturePanel(TexturePanel);

		}

		// ----------- Texture Overlaid Transparent Panel Setup Routine ------------ //

		private: GLvoid RMH_OpenGL_AddOverlayPanelToMainTexturePanel(System::Windows::Forms::Panel^ TexturePanel) {

			// This routine adds an overlaid transparent panel to the texture panel
			// This overlaid panel is used to manipulate objects on the image texture

			// The overlaid panel must not have any margin or padding
			OverlayPanel->Margin = System::Windows::Forms::Padding(0, 0, 0, 0);
			OverlayPanel->Padding = System::Windows::Forms::Padding(0, 0, 0, 0);

			// The overlaid transparent panel must fill the whole texture panel
			OverlayPanel->Dock = System::Windows::Forms::DockStyle::Fill;

			// Add the overlaid transparent panel as a "child" of the texture panel
			TexturePanel->Controls->Add(OverlayPanel);

			// Enable mouse handler events for the transparent panel
			OverlayPanel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &RMHOpenGLSurfacePlot::TexturePanel_MouseDown);
			OverlayPanel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &RMHOpenGLSurfacePlot::TexturePanel_MouseUp);
			OverlayPanel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &RMHOpenGLSurfacePlot::TexturePanel_MouseMove);
			OverlayPanel->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &RMHOpenGLSurfacePlot::TexturePanel_MouseWheel);

		}

		// ---------------- Texture Panel To Texture Conversion Routines ----------------- //

		private: GLdouble RMH_OpenGL_TranslateOverlayPanelMouseXPosToTextureXPos(System::Windows::Forms::MouseEventArgs^ OverlayPanelMouseEvent) {

			// This routine translates the overlaid panel mouse positions to the actual texture panel mouse positions

			// Local variables
			GLdouble MouseTextureXPos = 0.0;
			GLdouble PanelsWidthDifference = 0.0;

			// Calculate the pixel difference between the overlaid panel and the texture panel 
			PanelsWidthDifference = CurrentTexturePanelWidth - OverlayPanel->Width;

			// Convert the overlaid panel mouse position to the actual texture panel mouse position
			MouseTextureXPos = (OverlayPanelMouseEvent->X + PanelsWidthDifference) * (ImageDataPixelWidth / CurrentTexturePanelWidth);

			// Handling at the minimum texture mouse position 
			if (MouseTextureXPos <= 0) {
				// Set the mouse position to the minimum value
				MouseTextureXPos = 0;
			}

			// Handling at the maximum texture mouse position 
			if (MouseTextureXPos >= ImageDataPixelWidth) {
				// Set the mouse position to the maximum value
				MouseTextureXPos = ImageDataPixelWidth;
			}

			// Round the mouse position up to the nearest integer
			MouseTextureXPos = RMH_Math_Round(MouseTextureXPos);

			// Return the current texture panel mouse position
			return MouseTextureXPos;

		}

		private: GLdouble RMH_OpenGL_TranslateOverlayPanelMouseYPosToTextureYPos(System::Windows::Forms::MouseEventArgs^ OverlayPanelMouseEvent) {

			// This routine translates the overlaid panel mouse positions to the actual texture panel mouse positions

			// Local variables
			GLdouble MouseTextureYPos = 0.0;
			GLdouble PanelsHeightDifference = 0.0;

			// Calculate the pixel difference between the overlaid panel and the texture panel 
			PanelsHeightDifference = CurrentTexturePanelHeight - OverlayPanel->Height;

			// Convert the overlaid panel mouse position to the actual texture panel mouse position
			MouseTextureYPos = (OverlayPanelMouseEvent->Y + PanelsHeightDifference) * (ImageDataPixelHeight / CurrentTexturePanelHeight);

			// Handling at the minimum texture mouse position 
			if (MouseTextureYPos <= 0) {
				// Set the mouse position to the minimum value
				MouseTextureYPos = 0;
			}

			// Handling at the maximum texture mouse position 
			if (MouseTextureYPos >= ImageDataPixelHeight) {
				// Set the mouse position to the maximum value
				MouseTextureYPos = ImageDataPixelHeight;
			}

			// Round the mouse position up to the nearest integer
			MouseTextureYPos = RMH_Math_Round(MouseTextureYPos);

			// Return the current texture panel mouse position
			return MouseTextureYPos;

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
			gluPerspective(PlaneFieldOfView, PlaneAspectRatio, 5.0f, 10000.0);
			gluLookAt(PlaneXLook, PlaneYLook, PlaneDistance * 0.5, PlaneXLook, PlaneYLook, 0, 0, 1, 0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

		}

		// ------------------- Surface Plot Texture Rendering Routines ------------------- //

		private: GLvoid RMH_OpenGL_ClearTextureBuffer() {

			// This routine clears the associated texture buffers

			// Clear the texture color and bit buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		}

		private: GLvoid RMH_OpenGL_RenderSurfacePlotPolygons(unsigned int TexturePanelWidth, unsigned int TexturePanelHeight, unsigned int NmbOfHorizontalPolyGons, unsigned int NmbOfVerticalPolyGons, unsigned short* SurfacePolygonPixelData, unsigned short* SurfaceZData, GLfloat MaxZDataValue, GLfloat MinZDataValue) {

			// This routine renders the pixel polygons of the surface plot

			// Read the temporary array data and sort the kernel array
			register GLfloat PolygonZ0Pos1 = 0.0;
			register GLfloat PolygonZ0Pos2 = 0.0;
			register GLfloat PolygonZ0Pos3 = 0.0;
			register GLfloat PolygonZ0Pos4 = 0.0;
			register GLfloat PolygonZ0Pos5 = 0.0;
			register GLfloat PolygonZ0Pos6 = 0.0;
			register GLfloat PolygonZ0Pos7 = 0.0;
			register GLfloat PolygonZ0Pos8 = 0.0;
			register GLfloat PolygonZ1Pos1 = 0.0;
			register GLfloat PolygonZ1Pos2 = 0.0;
			register GLfloat PolygonZ1Pos3 = 0.0;
			register GLfloat PolygonZ1Pos4 = 0.0;
			register GLfloat PolygonZ1Pos5 = 0.0;
			register GLfloat PolygonZ1Pos6 = 0.0;
			register GLfloat PolygonZ1Pos7 = 0.0;
			register GLfloat PolygonZ1Pos8 = 0.0;
			register GLfloat PolygonZ2Pos1 = 0.0;
			register GLfloat PolygonZ2Pos2 = 0.0;
			register GLfloat PolygonZ2Pos3 = 0.0;
			register GLfloat PolygonZ2Pos4 = 0.0;
			register GLfloat PolygonZ2Pos5 = 0.0;
			register GLfloat PolygonZ2Pos6 = 0.0;
			register GLfloat PolygonZ2Pos7 = 0.0;
			register GLfloat PolygonZ2Pos8 = 0.0;
			register GLfloat PolygonZ3Pos1 = 0.0;
			register GLfloat PolygonZ3Pos2 = 0.0;
			register GLfloat PolygonZ3Pos3 = 0.0;
			register GLfloat PolygonZ3Pos4 = 0.0;
			register GLfloat PolygonZ3Pos5 = 0.0;
			register GLfloat PolygonZ3Pos6 = 0.0;
			register GLfloat PolygonZ3Pos7 = 0.0;
			register GLfloat PolygonZ3Pos8 = 0.0;
			GLdouble ConversionFactor = 0.00001525902190; // 1 / 2^16

			// Calculate the scaling factor of the Z axis
			GLfloat ZAxesScaleFactor = 1.0 / ((MaxZDataValue - MinZDataValue) * (1.0 / (CurrentTexturePanelHeight * SurfacePlotMaxZHeight)));

			// Calculate the height and width of the surface plot polygons in pixels
			register GLfloat SurfacePolygonWidth = (GLfloat)TexturePanelWidth / (GLfloat)NmbOfHorizontalPolyGons;
			register GLfloat SurfacePolygonHeight = (GLfloat)TexturePanelHeight / (GLfloat)NmbOfVerticalPolyGons;
			
			// Reset the X0 and Y0 coordinates of the start polygon before rendering
			register GLfloat SurfacePolygonX0 = -((GLfloat)TexturePanelWidth * 0.5);
			register GLfloat SurfacePolygonY0 = -((GLfloat)TexturePanelHeight * 0.5);

			// Reset the number of polygons rendered
			register unsigned int RenderedHorizontalPolygons = 0;
			register unsigned int RenderedVerticalPolygons = 0;

			// Render polygons on the texture
			glBegin(GL_QUADS);

			// Render the given number of polygons
			for (unsigned int i = 0, j = 0; i < (NmbOfHorizontalPolyGons * NmbOfVerticalPolyGons); i += 8, j += 24) {

				// Do not render the first Y polygon
				if (RenderedHorizontalPolygons != 0) {

					// Set the polygon color to the associated pixel color
					glColor3f(*(SurfacePolygonPixelData + j + 0) * ConversionFactor, *(SurfacePolygonPixelData + j + 1) * ConversionFactor, *(SurfacePolygonPixelData + j + 2) * ConversionFactor);

					// Render the lines of the surface plot rectangles
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0, -PolygonZ0Pos1);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0, -PolygonZ1Pos1);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ2Pos1);
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ3Pos1);

					// Update the X0 coordinate of the next polygon
					SurfacePolygonX0 = SurfacePolygonX0 + SurfacePolygonWidth;

					// Set the polygon color to the associated pixel color
					glColor3f(*(SurfacePolygonPixelData + j + 3) * ConversionFactor, *(SurfacePolygonPixelData + j + 4) * ConversionFactor, *(SurfacePolygonPixelData + j + 5) * ConversionFactor);

					// Render the lines of the surface plot rectangles
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0, -PolygonZ0Pos2);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0, -PolygonZ1Pos2);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ2Pos2);
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ3Pos2);

					// Update the X0 coordinate of the next polygon
					SurfacePolygonX0 = SurfacePolygonX0 + SurfacePolygonWidth;

					// Set the polygon color to the associated pixel color
					glColor3f(*(SurfacePolygonPixelData + j + 6) * ConversionFactor, *(SurfacePolygonPixelData + j + 7) * ConversionFactor, *(SurfacePolygonPixelData + j + 8) * ConversionFactor);

					// Render the lines of the surface plot rectangles
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0, -PolygonZ0Pos3);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0, -PolygonZ1Pos3);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ2Pos3);
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ3Pos3);

					// Update the X0 coordinate of the next polygon
					SurfacePolygonX0 = SurfacePolygonX0 + SurfacePolygonWidth;

					// Set the polygon color to the associated pixel color
					glColor3f(*(SurfacePolygonPixelData + j + 9) * ConversionFactor, *(SurfacePolygonPixelData + j + 10) * ConversionFactor, *(SurfacePolygonPixelData + j + 11) * ConversionFactor);

					// Render the lines of the surface plot rectangles
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0, -PolygonZ0Pos4);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0, -PolygonZ1Pos4);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ2Pos4);
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ3Pos4);

					// Update the X0 coordinate of the next polygon
					SurfacePolygonX0 = SurfacePolygonX0 + SurfacePolygonWidth;

					// Set the polygon color to the associated pixel color
					glColor3f(*(SurfacePolygonPixelData + j + 12) * ConversionFactor, *(SurfacePolygonPixelData + j + 13) * ConversionFactor, *(SurfacePolygonPixelData + j + 14) * ConversionFactor);

					// Render the lines of the surface plot rectangles
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0, -PolygonZ0Pos5);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0, -PolygonZ1Pos5);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ2Pos5);
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ3Pos5);

					// Update the X0 coordinate of the next polygon
					SurfacePolygonX0 = SurfacePolygonX0 + SurfacePolygonWidth;

					// Set the polygon color to the associated pixel color
					glColor3f(*(SurfacePolygonPixelData + j + 15) * ConversionFactor, *(SurfacePolygonPixelData + j + 16) * ConversionFactor, *(SurfacePolygonPixelData + j + 17) * ConversionFactor);

					// Render the lines of the surface plot rectangles
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0, -PolygonZ0Pos6);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0, -PolygonZ1Pos6);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ2Pos6);
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ3Pos6);

					// Update the X0 coordinate of the next polygon
					SurfacePolygonX0 = SurfacePolygonX0 + SurfacePolygonWidth;

					// Set the polygon color to the associated pixel color
					glColor3f(*(SurfacePolygonPixelData + j + 18) * ConversionFactor, *(SurfacePolygonPixelData + j + 19) * ConversionFactor, *(SurfacePolygonPixelData + j + 20) * ConversionFactor);

					// Render the lines of the surface plot rectangles
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0, -PolygonZ0Pos7);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0, -PolygonZ1Pos7);
					glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ2Pos7);
					glVertex3f(SurfacePolygonX0, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ3Pos7);

					// Update the X0 coordinate of the next polygon
					SurfacePolygonX0 = SurfacePolygonX0 + SurfacePolygonWidth;

					// Do not render the last horizontal polygon - data overflow due to the rendering structure
					if (RenderedHorizontalPolygons < NmbOfHorizontalPolyGons - 8) {

						// Set the polygon color to the associated pixel color
						glColor3f(*(SurfacePolygonPixelData + j + 21) * ConversionFactor, *(SurfacePolygonPixelData + j + 22) * ConversionFactor, *(SurfacePolygonPixelData + j + 23) * ConversionFactor);

						// Render the lines of the surface plot rectangles
						glVertex3f(SurfacePolygonX0, SurfacePolygonY0, -PolygonZ0Pos8);
						glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0, -PolygonZ1Pos8);
						glVertex3f(SurfacePolygonX0 + SurfacePolygonWidth, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ2Pos8);
						glVertex3f(SurfacePolygonX0, SurfacePolygonY0 + SurfacePolygonHeight, -PolygonZ3Pos8);

						// Update the X0 coordinate of the next polygon
						SurfacePolygonX0 = SurfacePolygonX0 + SurfacePolygonWidth;

					}

				}

				// Increment the number of horizontal polygons rendered
				RenderedHorizontalPolygons = RenderedHorizontalPolygons + 8;

				// Has the number of horizontal polygons rendered been reached 
				if (RenderedHorizontalPolygons >= NmbOfHorizontalPolyGons) {

					// Increment the number of vertical polygon rows rendered
					RenderedVerticalPolygons = RenderedVerticalPolygons + 1;
					// Reset the number of horizontal polygons rendered
					RenderedHorizontalPolygons = 0;

					// Reset the X0 coordinate of the start polygon
					SurfacePolygonX0 = -((GLfloat)TexturePanelWidth * 0.5);
					// Update the Y0 coordinate of the next polygon row
					SurfacePolygonY0 = SurfacePolygonY0 + SurfacePolygonHeight;

				}

				// Update the X0/Y0 Z coordinates of the polygon (last row compensated) - loop unroll part 1
				PolygonZ0Pos1 = ((*(SurfaceZData + i + 0 + 8)) - MinZDataValue) * ZAxesScaleFactor;
				PolygonZ1Pos1 = ((*(SurfaceZData + i + 1 + 8)) - MinZDataValue) * ZAxesScaleFactor;
				PolygonZ1Pos2 = ((*(SurfaceZData + i + 2 + 8)) - MinZDataValue) * ZAxesScaleFactor;
				PolygonZ1Pos3 = ((*(SurfaceZData + i + 3 + 8)) - MinZDataValue) * ZAxesScaleFactor;
				PolygonZ1Pos4 = ((*(SurfaceZData + i + 4 + 8)) - MinZDataValue) * ZAxesScaleFactor;
				PolygonZ0Pos2 = PolygonZ1Pos1;
				PolygonZ0Pos3 = PolygonZ1Pos2;
				PolygonZ0Pos4 = PolygonZ1Pos3;

				// Update the X0/Y0 Z coordinates of the polygon (last row compensated) - loop unroll part 2
				PolygonZ0Pos5 = ((*(SurfaceZData + i + 4 + 8)) - MinZDataValue) * ZAxesScaleFactor;
				PolygonZ1Pos5 = ((*(SurfaceZData + i + 5 + 8)) - MinZDataValue) * ZAxesScaleFactor;
				PolygonZ1Pos6 = ((*(SurfaceZData + i + 6 + 8)) - MinZDataValue) * ZAxesScaleFactor;
				PolygonZ1Pos7 = ((*(SurfaceZData + i + 7 + 8)) - MinZDataValue) * ZAxesScaleFactor;
				PolygonZ1Pos8 = ((*(SurfaceZData + i + 8 + 8)) - MinZDataValue) * ZAxesScaleFactor;
				PolygonZ0Pos6 = PolygonZ1Pos5;
				PolygonZ0Pos7 = PolygonZ1Pos6;
				PolygonZ0Pos8 = PolygonZ1Pos7;

				// If the last vertical polygon row has been reached
				if (RenderedVerticalPolygons >= NmbOfVerticalPolyGons - 1) {
	
					// Update the X0/Y0 Z coordinates of the polygon (last row compensated) - for the first polygon - loop unroll part 1
					PolygonZ2Pos1 = PolygonZ1Pos1;
					PolygonZ2Pos2 = PolygonZ1Pos2;
					PolygonZ2Pos3 = PolygonZ1Pos3;
					PolygonZ2Pos4 = PolygonZ1Pos4;
					PolygonZ3Pos1 = PolygonZ0Pos1;
					PolygonZ3Pos2 = PolygonZ0Pos2;
					PolygonZ3Pos3 = PolygonZ0Pos3;
					PolygonZ3Pos4 = PolygonZ0Pos4;

					// Update the X0/Y0 Z coordinates of the polygon (last row compensated) - for the first polygon - loop unroll part 2
					PolygonZ2Pos5 = PolygonZ1Pos5;
					PolygonZ2Pos6 = PolygonZ1Pos6;
					PolygonZ2Pos7 = PolygonZ1Pos7;
					PolygonZ2Pos8 = PolygonZ1Pos8;
					PolygonZ3Pos5 = PolygonZ0Pos5;
					PolygonZ3Pos6 = PolygonZ0Pos6;
					PolygonZ3Pos7 = PolygonZ0Pos7;
					PolygonZ3Pos8 = PolygonZ0Pos8;
					
				}		
				else {
					
					// Update the X0/Y0 Z coordinates of the polygon - for the first polygon - loop unroll part 1
					PolygonZ2Pos1 = ((*(SurfaceZData + i + 1 + 8 + NmbOfHorizontalPolyGons)) - MinZDataValue) * ZAxesScaleFactor;
					PolygonZ2Pos2 = ((*(SurfaceZData + i + 2 + 8 + NmbOfHorizontalPolyGons)) - MinZDataValue) * ZAxesScaleFactor;
					PolygonZ2Pos3 = ((*(SurfaceZData + i + 3 + 8 + NmbOfHorizontalPolyGons)) - MinZDataValue) * ZAxesScaleFactor;
					PolygonZ2Pos4 = ((*(SurfaceZData + i + 4 + 8 + NmbOfHorizontalPolyGons)) - MinZDataValue) * ZAxesScaleFactor;
					PolygonZ3Pos1 = ((*(SurfaceZData + i + 0 + 8 + NmbOfHorizontalPolyGons)) - MinZDataValue) * ZAxesScaleFactor;
					PolygonZ3Pos2 = PolygonZ2Pos1;
					PolygonZ3Pos3 = PolygonZ2Pos2;
					PolygonZ3Pos4 = PolygonZ2Pos3;

					// Update the X0/Y0 Z coordinates of the polygon - for the first polygon - loop unroll part 2			
					PolygonZ2Pos5 = ((*(SurfaceZData + i + 5 + 8 + NmbOfHorizontalPolyGons)) - MinZDataValue) * ZAxesScaleFactor;
					PolygonZ2Pos6 = ((*(SurfaceZData + i + 6 + 8 + NmbOfHorizontalPolyGons)) - MinZDataValue) * ZAxesScaleFactor;
					PolygonZ2Pos7 = ((*(SurfaceZData + i + 7 + 8 + NmbOfHorizontalPolyGons)) - MinZDataValue) * ZAxesScaleFactor;
					PolygonZ2Pos8 = ((*(SurfaceZData + i + 8 + 8 + NmbOfHorizontalPolyGons)) - MinZDataValue) * ZAxesScaleFactor;
					PolygonZ3Pos5 = ((*(SurfaceZData + i + 4 + 8 + NmbOfHorizontalPolyGons)) - MinZDataValue) * ZAxesScaleFactor;
					PolygonZ3Pos6 = PolygonZ2Pos5;
					PolygonZ3Pos7 = PolygonZ2Pos6;
					PolygonZ3Pos8 = PolygonZ2Pos7;

				}

			}

			// End of configuration
			glEnd();

		}

		// -------------- Combined Surface Plot Graphics Rendering Routine --------------- //

		public: GLvoid RMH_OpenGL_ResetSurfacePlotView() {

			// This routine resets the viewing angle of the 3D surface plot to the start position

			// Reset the viewing angle of the surface plot back to the start position
			SurfacePlotXTranstaledPos = _SurfacePlotStartDefault_XPos;
			SurfacePlotYTranstaledPos = _SurfacePlotStartDefault_YPos;
			SurfacePlotZTranstaledPos = _SurfacePlotStartDefault_ZPos;
			SurfacePlotXAngle = _SurfacePlotStartAngle_XAngle;
			SurfacePlotYAngle = _SurfacePlotStartAngle_YAngle;
			SurfacePlotYXAngle = _SurfacePlotStartAngle_YAngle;

		}

		public: GLvoid RMH_OpenGL_UpdateSurfacePlotMaxZHeight(System::Object^ sender) {

			// This routine updates the maximum Z height of the surface plot in pixels

			// Cast the sender object as a WinForms ToolStrip object
			System::Windows::Forms::ToolStripMenuItem^ TagSurfaceZHeight = (System::Windows::Forms::ToolStripMenuItem^)sender;

			// Read the sub context menu identification tag (surface plot scale factor value)
			GLfloat SurfaceZHeight = Convert::ToDouble(TagSurfaceZHeight->Tag);

			// Update the maximum Z pixel height of the surface plot in percent
			SurfacePlotMaxZHeight = (GLfloat)SurfaceZHeight;

		}
		
		public: GLvoid RMH_OpenGL_SetSurfacePlotPolygonMode(System::Object^ sender) {

			// This routine switches between the available polygon rendering modes

			// Cast the sender object as a WinForms ToolStrip object
			System::Windows::Forms::ToolStripMenuItem^ TagSurfacePolygonMode = (System::Windows::Forms::ToolStripMenuItem^)sender;

			// Read the sub context menu identification tag 
			unsigned char SurfacePolygonMode = Convert::ToDouble(TagSurfacePolygonMode->Tag);

			// Selection of the surface plot polygon mode
			switch (SurfacePolygonMode) {

				// Set the polygon mode and size parameter of the surface plot
				case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); glPointSize(PointPolygonModePointSize);    break;
				case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  glLineWidth(LinePolygonModeLineSize);    break;
				case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); glPointSize(0); glLineWidth(0); break;

			}

		}

		public: GLvoid RMH_OpenGL_SetSurfacePlotPolygonModePointSize(System::Object^ sender) {

			// This routine sets the point size in the "GL_POINT" polygon rendering mode

			// Cast the sender object as a WinForms ToolStrip object
			System::Windows::Forms::ToolStripMenuItem^ TagPointPolygonMode = (System::Windows::Forms::ToolStripMenuItem^)sender;

			// Read the sub context menu identification tag 
			unsigned char PointPolygonModeSizeValue = Convert::ToDouble(TagPointPolygonMode->Tag);

			// Update the associated private class variables
			PointPolygonModePointSize = PointPolygonModeSizeValue;

			// Set the point size for the polygon mode
			glPointSize(PointPolygonModePointSize);

		}

		public: GLvoid RMH_OpenGL_SetSurfacePlotPolygonModeLineSize(System::Object^ sender) {

			// This routine sets the line size in the "GL_LINE" polygon rendering mode

			// Cast the sender object as a WinForms ToolStrip object
			System::Windows::Forms::ToolStripMenuItem^ TagLinePolygonMode = (System::Windows::Forms::ToolStripMenuItem^)sender;

			// Read the sub context menu identification tag 
			unsigned char LinePolygonModeSizeValue = Convert::ToDouble(TagLinePolygonMode->Tag);

			// Update the associated private class variables
			LinePolygonModeLineSize = LinePolygonModeSizeValue;

			// Set the line size for the polygon mode
			glLineWidth(LinePolygonModeLineSize);

		}

		public: GLvoid RMH_OpenGL_RenderSurfacePlot(unsigned int SurfacePlotPanelWidth, unsigned int SurfacePlotPanelHeight, unsigned short *SurfacePolygonPixelData, unsigned int SurfaceDataWidth, unsigned int SurfaceDataHeight, unsigned short* SurfaceZData, GLfloat MaxZDataValue, GLfloat MinZDataValue) {

			// This routine renders all graphical objects that the surface plot consists of

			// Read the current pixel height and width of the texture panel
			CurrentTexturePanelWidth = (GLdouble)SurfacePlotPanelWidth;
			CurrentTexturePanelHeight = (GLdouble)SurfacePlotPanelHeight;

			// Read the given surface plot image data pixel width and height
			ImageDataPixelWidth = SurfaceDataWidth;
			ImageDataPixelHeight = SurfaceDataHeight;

			// Calculate the pixel offset between the texture area and the associated GUI panel
			TextureToPanelWidthOffset = OpenGLWindowWidth - CurrentTexturePanelWidth;
			TextureToPanelHeightOffset = OpenGLWindowHeight - CurrentTexturePanelHeight;

			// Calculate the aspect ratio of the surface plot
			SurfacePlotAspectRatio = (GLdouble)SurfaceDataWidth / (GLdouble)SurfaceDataHeight;
			// Calculate the aspect-ratio-compensated rendering width of the surface plot
			AspectRatioCompensatedWidth = SurfacePlotAspectRatio * CurrentTexturePanelHeight;
			// Calculate the translated Y position of the surface plot - relative to the aspect ratio etc.
			TranstaledYSurfacePlotValue = (GLfloat)OpenGLWindowHeight - CurrentSurfacePlotYTranstaledPos - TextureToPanelHeightOffset - (SurfacePlotPanelHeight * 0.5);
			
			// Make the associated render context the current render context
			RMH_OpenGL_MakeRenderContextCurrent();
			// Clear the texture color and bit buffers
			RMH_OpenGL_ClearTextureBuffer();
			// Update the texture field of view
			RMH_OpenGL_UpdateTextureFieldOfView(CurrentTexturePanelWidth, CurrentTexturePanelHeight);

			// Update the texture viewport to the center of the surface plot
			glViewport(0, TextureToPanelHeightOffset, CurrentTexturePanelWidth, CurrentTexturePanelHeight);

			// Read the current position parameters of the surface plot
			CurrentSurfacePlotXTranstaledPos = SurfacePlotXTranstaledPos;
			CurrentSurfacePlotYTranstaledPos =  SurfacePlotYTranstaledPos;
			CurrentSurfacePlotZTranstaledPos = SurfacePlotZTranstaledPos;
			CurrentSurfacePlotYAngle = SurfacePlotYAngle;
			CurrentSurfacePlotXAngle = SurfacePlotXAngle;
			CurrentSurfacePlotYXAngle = SurfacePlotYXAngle;
	
			// Update the viewing angle of the surface plot 
			glTranslatef(((GLfloat)SurfacePlotPanelWidth * 0.5) + CurrentSurfacePlotXTranstaledPos, TranstaledYSurfacePlotValue, CurrentSurfacePlotZTranstaledPos);
			glRotatef(180.0f + CurrentSurfacePlotYAngle, 1.0f, 0.0f, 0.0f);

			// Rotate about the Z axis of the surface plot for mouse movement on the X axis - left click
			glRotatef(-CurrentSurfacePlotXAngle, 0.0f, 0.0f, 1.0f);
			// Rotate about the Y axis of the surface plot for mouse movement on the X axis - right click
			glRotatef(-CurrentSurfacePlotYXAngle, 0.0f, 1.0f, 0.0f);

			// --------------------------------- Render Surface Plot ---------------------------------- //

			// Render the 3D surface plot polygons
			RMH_OpenGL_RenderSurfacePlotPolygons(AspectRatioCompensatedWidth, CurrentTexturePanelHeight, SurfaceDataWidth, SurfaceDataHeight, SurfacePolygonPixelData, SurfaceZData, MaxZDataValue, MinZDataValue);

			// ---------------------------------------------------------------------------------------- //

			// Mark the end of an OpenGL rendering sequence
			RMH_OpenGL_RenderingFinishedMark();

		}
		
		// ------------ Texture Panel Interaction Cursor Event Callback Routines ----------- //

		private: GLvoid TexturePanel_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {

			// Read the mouse cursor position
			GLdouble MouseXPosition = RMH_OpenGL_TranslateOverlayPanelMouseXPosToTextureXPos(e);
			GLdouble MouseYPosition = RMH_OpenGL_TranslateOverlayPanelMouseYPosToTextureYPos(e);

			// Update the overlay panel click flag
			OverlayPanelIsClick = true;

			// Reset the mouse button flag
			RightMouseButtonClicked = false;
			MouseWheelButtonClicked = false;
			LeftMouseButtonClicked = false;

			// Which mouse button has been pressed
			switch (e->Button) {

				// Right mouse button
				case System::Windows::Forms::MouseButtons::Right:

					// Update the mouse button flag
					RightMouseButtonClicked = true;

					// Read the position offset of the mouse cursor
					SurfacePlotYXTranstaledPosClickOffset = MouseXPosition - CurrentSurfacePlotYXAngle;

				break;

				// Mouse wheel button
				case System::Windows::Forms::MouseButtons::Middle:

					// Update the mouse button flag
					MouseWheelButtonClicked = true;

					// Read the position offset of the mouse cursor
					SurfacePlotXTranstaledPosClickOffset = MouseXPosition - CurrentSurfacePlotXTranstaledPos;
					SurfacePlotYTranstaledPosClickOffset = MouseYPosition - CurrentSurfacePlotYTranstaledPos;

				break;

				// Left mouse button
				case System::Windows::Forms::MouseButtons::Left:

					// Update the mouse button flag
					LeftMouseButtonClicked = true;

					// Read the position offset of the mouse cursor
					SurfacePlotXAngleClickOffset = MouseXPosition - CurrentSurfacePlotXAngle;
					SurfacePlotYAngleClickOffset = MouseYPosition - CurrentSurfacePlotYAngle;

				break;

			}

		}

		private: GLvoid TexturePanel_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {

			// Update the overlay panel click flag
			OverlayPanelIsClick = false;

		}

		private: GLvoid TexturePanel_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {

			// Read the mouse cursor position
			GLdouble MouseXPosition = RMH_OpenGL_TranslateOverlayPanelMouseXPosToTextureXPos(e);
			GLdouble MouseYPosition = RMH_OpenGL_TranslateOverlayPanelMouseYPosToTextureYPos(e);

			// If the panel has not yet been clicked
			if (OverlayPanelIsClick == false) {

				// Do not continue
				return;

			}

			// Is the right mouse button pressed
			if (RightMouseButtonClicked == true) {

				// Update the Y-transformed positions of the surface plot
				SurfacePlotYXAngle = MouseXPosition - SurfacePlotYXTranstaledPosClickOffset;

			}
			// Is the mouse wheel button pressed
			if (MouseWheelButtonClicked == true) {

				// Update the X and Y transformed positions of the surface plot
				SurfacePlotXTranstaledPos = MouseXPosition - SurfacePlotXTranstaledPosClickOffset;
				SurfacePlotYTranstaledPos = MouseYPosition - SurfacePlotYTranstaledPosClickOffset;

			}
			// Is the left mouse button pressed
			if (LeftMouseButtonClicked == true) {

				// Update the X and Y angle positions of the surface plot
				SurfacePlotXAngle = MouseXPosition - SurfacePlotXAngleClickOffset;
				SurfacePlotYAngle = MouseYPosition - SurfacePlotYAngleClickOffset;

			}
			
		}

		private: GLvoid TexturePanel_MouseWheel(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {

			// Check the rotation polarity of the mouse wheel
			if (e->Delta < 0.0) {

				// For negative polarity - reduce the Z position
				SurfacePlotZTranstaledPos = SurfacePlotZTranstaledPos - 25.0;

			}
			else {

				// For positive polarity - increment the Z position
				SurfacePlotZTranstaledPos = SurfacePlotZTranstaledPos + 25.0;

			}

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

		}

		// --------------------------------------------------------------------------------- //

		private:

		// ------------- Additional OpenGL Handling And Setup Routines ------------- //

		~RMHOpenGLSurfacePlot(GLvoid) {

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
			// Enable OpenGL "depth testing"
			glEnable(GL_DEPTH_TEST);
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