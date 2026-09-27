#pragma once

/*
 *  RMH_OpenGL_ColorBar.h
 *
 *  Author: Rune Mark Hansen
 *  Date: January 2023
 *
 */

// Included libraries
#include <windows.h>
#include <GL/GLU.h>
#include <GL/GL.h>
#include <iostream>

 // Associated namespaces
using namespace System::Windows::Forms;
using namespace std;

// Class configuration macros
#define _ColorBarMaxNumberOfTicks       20 // Major Ticks + Minor Ticks
#define _NumberOfMovableTags            2

// Static global arrays and variables
static GLfloat MinorTickLabelXPos[_ColorBarMaxNumberOfTicks];
static GLfloat MinorTickLabelYPos[_ColorBarMaxNumberOfTicks];
static unsigned short FirstColorPaletteData[16384 * 4];
static unsigned short SecondColorPaletteData[16384 * 4];
static GLdouble MovableTagY0[_NumberOfMovableTags + 1];
static std::string MaximumTagStdString = "MAX";
static std::string MinimumTagStdString = "MIN";

// OpenGL class definition
namespace OpenGLColorBar {

	// ------------------------- Global Class Structure Objects -------------------------- //

	// ColorBar tag position data class structure
	class ColorBarTagPosition {
	public:

		// Tag position parameters
		unsigned short MaximumTagPos = 0;
		unsigned short MinimumTagPos = 16384;

	};

	// ColorBar manual range max/min temperature class structure
	class ColorBarManualRangeTemps {
	public:

		// Tag position parameters
		GLfloat ManualMaxRangeTemp = 0.0f;
		GLfloat ManualMinRangeTemp = 0.0f;

	};

	// ----------------- Private Custom WinForms Transparent Panel Class ----------------- //

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

	// --------------------------- Main OpenGL ColorBar Class -------------------------- //

	public ref class RMHOpenGLColorBar : public System::Windows::Forms::NativeWindow {

	private:

		// ------------------------ Local Form Reference Structure ------------------------- //

		// Local reference structure
		ref struct PrivateLocals {

			// ColorBar major and minor tick label array
			static cli::array<System::String^>^ ColorBarTickLabels = gcnew cli::array<System::String^>(_ColorBarMaxNumberOfTicks);

		};

		// --------------------------------------------------------------------------------- //

	private:

		// ColorBar configuration parameters
		private: GLfloat ColorBarPixelWidth = 1;
		private: GLfloat ColorBarPixelHeight = 256;
		private: unsigned int ColorBarPaletteResolution = 16384;
		private: unsigned int ColorBarPaletteIntegerRange = 65535;

		// Private global class objects and variables
		private: HDC m_hDC;
		private: HGLRC m_hglrc;
		private: GLuint BaseFont;
		private: GLint iPixelFormat;
		private: GLuint* ColorBarTexture;
		private: GLfloat ColorBarTexture_t;
		private: GLfloat ColorBarTexture_u;
		private: unsigned int TextureWidth;
		private: unsigned int TextureHeight;
		private: GLdouble InitialTextureWidth;
		private: GLdouble InitialTextureHeight;
		private: GLdouble TextureResScaleFactor;
		private: bool OverlayPanelIsClick = false;
		private: GLdouble CurrentTexturePanelWidth;
		private: GLdouble CurrentTexturePanelHeight;
		private: GLdouble TotalTextureScalableWidth;
		private: GLdouble TotalTextureScalableHeight;
		private: GLdouble TextureToPanelScaleWidthFactor;
		private: GLdouble TextureToPanelScaleHeightFactor;
		private: CreateParams^ ControlParams = gcnew CreateParams;
		private: TextureOverlayPanel^ OverlayPanel = gcnew TextureOverlayPanel();

		// ColorBar panel, texture, tick line and position configuration variables
		private: GLfloat ColorBarPanelTextureWidth = 24;
		private: GLfloat ColorBarPanelTexturePadding = 8;
		private: GLfloat ColorBarX0TexturePos = 9;
		private: GLfloat ColorBarTextureWidth = 4;
		private: GLfloat TickLineColorBarOffset = 1;
		private: GLfloat ColorBarMajorTickLength = 0.5;
		private: GLfloat ColorBarMinorTickLength = 0.3;
		private: GLfloat TickLabelXOffset = 0.5;
		private: GLfloat TickLabelYOffset = 0.8;

		// Position-adjustable tag configuration variables
		private: unsigned char MaxTagID = 1;
		private: unsigned char MinTagID = 2;
		private: GLfloat MaxMinTagX0Pos = 9;
		private: GLfloat MaxMinTagX0Offset = 1;
		private: GLfloat MaxMinTagArrowLength = 2;
		private: GLfloat MaxMinTagHeight = 8;
		private: GLfloat MaxMinTagLength = 6;
		private: GLfloat TagLabelXOffset = 4;
		private: GLfloat TagLabelYOffset = 0.7;

		// Miscellaneous ColorBar variables
		private: GLfloat ColorBarAreaX0;
		private: GLfloat ColorBarAreaY0;
		private: GLfloat ColorBarAreaWidth;
		private: GLfloat ColorBarAreaHeight;
		private: unsigned char NmbOfColorBarTicks = 0;
		private: GLfloat MinorTickPixelRes = 0.0f;
		private: GLfloat ColorBarMaxTickXPos = 0.0f;
		private: GLfloat ColorBarMaxTickYPos = 0.0f;
		private: GLfloat ColorBarMinTickXPos = 0.0f;
		private: GLfloat ColorBarMinTickYPos = 0.0f;
		private: GLfloat MajorTickLinePixelSize = 0.0f;
		private: unsigned short SelectedTagIndex = 0;
		private: bool TagMoveFlag = false;
		private: GLdouble ClickTagYPositionOffset;
		private: bool ColorBarTagMoveEnableFlag = true;
		private: System::String^ MaximumTagSystemString;
		private: System::String^ MinimumTagSystemString;
		private: bool BarManualRangeFlag = false;
		private: bool BarManualHighRangeFlag = false;
		private: bool BarManualLowRangeFlag = false;
		private: GLfloat ColorBarMaxArrowYPos = 0.0f;
		private: GLfloat ColorBarMinArrowYPos = 0.0f;
		private: GLfloat ColorBarCntArrowYPos = 0.0f;
		private: GLfloat ColorBarMaxTempRange = 0.0f;
		private: GLfloat ColorBarMinTempRange = 0.0f;
		private: GLfloat MouseWheelScrollPolarity = 0.0f;
		private: GLfloat MouseWheelStepSize = 1;
		private: GLfloat ColorBarMaxRangeOffsetValue = 0.0f;
		private: GLfloat ColorBarMinRangeOffsetValue = 0.0f;
		private: bool ShowTopOutsideIndicatorFlag = false;
		private: bool ShowButOutsideIndicatorFlag = false;
		private: bool InvertFirstPaletteFlag = false;
		private: bool InvertSecondPaletteFlag = false;

		// Global ColorBar color variables (set to default color values)
		private: GLubyte TickLineColorR = 255;
		private: GLubyte TickLineColorG = 255;
		private: GLubyte TickLineColorB = 255;
		private: GLubyte MaxTagColorR = 90;
		private: GLubyte MaxTagColorG = 90;
		private: GLubyte MaxTagColorB = 90;
		private: GLubyte MinTagColorR = 90;
		private: GLubyte MinTagColorG = 90;
		private: GLubyte MinTagColorB = 90;
		private: GLubyte MaxTrackArrowColorR = 255;
		private: GLubyte MaxTrackArrowColorG = 255;
		private: GLubyte MaxTrackArrowColorB = 255;
		private: GLubyte MinTrackArrowColorR = 255;
		private: GLubyte MinTrackArrowColorG = 255;
		private: GLubyte MinTrackArrowColorB = 255;
		private: GLubyte CntTrackArrowColorR = 255;
		private: GLubyte CntTrackArrowColorG = 255;
		private: GLubyte CntTrackArrowColorB = 255;
		private: GLubyte MaxTrackArrowLabelColorR = 255;
		private: GLubyte MaxTrackArrowLabelColorG = 255;
		private: GLubyte MaxTrackArrowLabelColorB = 255;
		private: GLubyte MinTrackArrowLabelColorR = 255;
		private: GLubyte MinTrackArrowLabelColorG = 255;
		private: GLubyte MinTrackArrowLabelColorB = 255;
		private: GLubyte CntTrackArrowLabelColorR = 255;
		private: GLubyte CntTrackArrowLabelColorG = 255;
		private: GLubyte CntTrackArrowLabelColorB = 255;
		private: GLubyte ColorBarMaxLabelColorR = 255;
		private: GLubyte ColorBarMaxLabelColorG = 255;
		private: GLubyte ColorBarMaxLabelColorB = 255;
		private: GLubyte ColorBarMinLabelColorR = 255;
		private: GLubyte ColorBarMinLabelColorG = 255;
		private: GLubyte ColorBarMinLabelColorB = 255;

	public:

		// ------------------------- ColorBar Constructor Routines ------------------------- //

		RMHOpenGLColorBar(System::Windows::Forms::Panel^ TexturePanel, unsigned char ResolutionScaleFactor) {

			// This routine sets up an OpenGL-supported graphics area for rendering
			// A WinForms panel is given as the physical texture area.

			// Initialize the colorbar tick labels array with start strings
			RMH_OpenGL_InitCliArray(PrivateLocals::ColorBarTickLabels, "N/A");

			// Set start values for the max/min tag Y positions
			MovableTagY0[MaxTagID] = ColorBarPanelTexturePadding;
			MovableTagY0[MinTagID] = ColorBarPixelHeight + ColorBarPanelTexturePadding;

			// Set start values for the Y positions of the colorbar max/min/center arrows
			ColorBarMaxArrowYPos = ColorBarPanelTexturePadding;
			ColorBarMinArrowYPos = ColorBarPixelHeight + ColorBarPanelTexturePadding;
			ColorBarCntArrowYPos = ((ColorBarPixelHeight + ColorBarPanelTexturePadding) - ColorBarPanelTexturePadding) / 2.0f;

			// Set the initial texture parameters
			InitialTextureWidth = (GLdouble)TexturePanel->Width;
			InitialTextureHeight = (GLdouble)TexturePanel->Height;
			TextureResScaleFactor = (GLdouble)ResolutionScaleFactor;

			// Calculate the total scalable texture height and width
			TotalTextureScalableWidth = InitialTextureWidth * TextureResScaleFactor;
			TotalTextureScalableHeight = InitialTextureHeight * TextureResScaleFactor;

			// Set the position of the control class
			ControlParams->X = 0;
			ControlParams->Y = 0;
			ControlParams->Width = (GLdouble)TexturePanel->Width * TextureResScaleFactor;
			ControlParams->Height = (GLdouble)TexturePanel->Height * TextureResScaleFactor;

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

			// Add an overlaid transparent panel to the texture panel
			RMH_OpenGL_AddOverlayPanelToMainTexturePanel(TexturePanel);

			// Set up the ColorBar texture area for graphics rendering
			RMH_OpenGL_InitColorBarTexture(ColorBarPanelTextureWidth, ColorBarPanelTexturePadding + ColorBarPixelHeight + ColorBarPanelTexturePadding);

		}

		private: GLvoid RMH_OpenGL_InitCliArray(cli::array<System::String^>^ InputArray, System::String^ InitString) {

			// This routine initializes the given array with start values

			// Loop up to and including the maximum length of the array
			for (unsigned int i = 0; i < _ColorBarMaxNumberOfTicks; i++) {

				// Write the string to the array index
				InputArray[i] = InitString;

			}

		}

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
			OverlayPanel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &RMHOpenGLColorBar::TexturePanel_MouseDown);
			OverlayPanel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &RMHOpenGLColorBar::TexturePanel_MouseUp);
			OverlayPanel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &RMHOpenGLColorBar::TexturePanel_MouseMove);
			OverlayPanel->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &RMHOpenGLColorBar::TexturePanel_MouseWheel);

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
			MouseTextureXPos = (OverlayPanelMouseEvent->X + PanelsWidthDifference) * (TextureWidth / CurrentTexturePanelWidth);

			// Handling at the minimum texture mouse position 
			if (MouseTextureXPos <= 0) {
				// Set the mouse position to the minimum value
				MouseTextureXPos = 0;
			}

			// Handling at the maximum texture mouse position 
			if (MouseTextureXPos >= TextureWidth) {
				// Set the mouse position to the maximum value
				MouseTextureXPos = TextureWidth;
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
			MouseTextureYPos = (OverlayPanelMouseEvent->Y + PanelsHeightDifference) * (TextureHeight / CurrentTexturePanelHeight);

			// Handling at the minimum texture mouse position 
			if (MouseTextureYPos <= 0) {
				// Set the mouse position to the minimum value
				MouseTextureYPos = 0;
			}

			// Handling at the maximum texture mouse position 
			if (MouseTextureYPos >= TextureHeight) {
				// Set the mouse position to the maximum value
				MouseTextureYPos = TextureHeight;
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

		private: GLvoid RMH_OpenGL_InitColorBarTexture(unsigned int ColorBarTextureWidth, unsigned int ColorBarTextureHeight) {

			// This routine is used to set up an OpenGL texture for graphics rendering

			// Set the texture parameters - width and height must be a multiple of 2
			ColorBarTexture = new GLuint[1];
			TextureWidth = ColorBarTextureWidth;
			TextureHeight = ColorBarTextureHeight;
			ColorBarTexture_t = (GLfloat)ColorBarPixelWidth / (GLfloat)TextureWidth;
			ColorBarTexture_u = (GLfloat)ColorBarPaletteResolution / ((ColorBarPanelTexturePadding + (GLfloat)ColorBarPaletteResolution) + ColorBarPanelTexturePadding);

			// Make the associated render context the current render context
			wglMakeCurrent(m_hDC, m_hglrc);

			// Use the texture that renders the colorbar data
			glGenTextures(1, ColorBarTexture);

			// Enable OpenGL 2D texture
			glEnable(GL_TEXTURE_2D);

			// Enable texture blending
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Bind the texture as a 2D texture
			glBindTexture(GL_TEXTURE_2D, ColorBarTexture[0]);

			// Configure the texture parameters
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, ColorBarPixelWidth, ColorBarPaletteResolution, 0, GL_RGBA, GL_UNSIGNED_SHORT, NULL);
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
			gluPerspective(PlaneFieldOfView, PlaneAspectRatio, 0.01f, 2000.0);
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

		private: GLvoid RMH_OpenGL_RenderStringOnTextureCompensated(GLfloat StringX, GLfloat StringY, std::string DisplayString, GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine renders a given string on an OpenGL texture

			// Calculate the string position when scaling the texture window
			StringX = StringX * TextureToPanelScaleWidthFactor;
			StringY = StringY * TextureToPanelScaleHeightFactor;

			// Configure the text color
			glColor3ub(ColorR, ColorG, ColorB);
			// Set the position of the text on the texture
			glRasterPos2f(StringX, StringY);

			// Render the given string on the texture
			RMH_OpenGL_glPrint(DisplayString.c_str());

		}

		// --------------------- ColorBar Texture Rendering Routines --------------------- //

		private: GLvoid RMH_OpenGL_ClearTextureBuffer() {

			// This routine clears the associated texture buffers

			// Clear the texture color and bit buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		}

		private: GLvoid RMH_OpenGL_StartColorBarRender() {

			// This routine is the start state of the OpenGL colorbar rendering

			// Rotate the texture to match the correct image orientation
			glTranslatef(0.0f, (GLdouble)TextureHeight, 0.0f);
			glRotatef(180.0f, 1.0f, 0.0f, 0.0f);

		}
		
		private: GLvoid RMH_OpenGL_RenderColorBarPalette(GLfloat ColorBarX0, GLfloat ColorBarY0, GLfloat ColorBarWidth, GLfloat ColorBarHeight, unsigned short* PaletteData, bool InvertColorPalette) {

			// This routine renders a given colorbar palette

			// Store X/Y/W/H in global variables
			ColorBarAreaX0 = (GLfloat)ColorBarX0 * TextureToPanelScaleWidthFactor;
			ColorBarAreaY0 = (GLfloat)ColorBarY0 * TextureToPanelScaleHeightFactor;
			ColorBarAreaWidth = (GLfloat)ColorBarWidth * TextureToPanelScaleWidthFactor;
			ColorBarAreaHeight = (GLfloat)ColorBarHeight * TextureToPanelScaleHeightFactor;

			// Update the texture data with the image data
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBindTexture(GL_TEXTURE_2D, ColorBarTexture[0]);

			// Write the color palette data to the texture
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ColorBarPixelWidth, ColorBarPaletteResolution, GL_RGBA, GL_UNSIGNED_SHORT, PaletteData);
			
			// Begin rendering
			glBegin(GL_QUADS);

			// Should the display of the color palette be inverted
			if (InvertColorPalette == true) {

				// Update the render texture coordinates - inverted
				glTexCoord2f(ColorBarTexture_t, ColorBarTexture_u);
				glVertex2f(ColorBarAreaX0 + ColorBarAreaWidth, ColorBarAreaY0 + ColorBarAreaHeight);
				glTexCoord2f(0.0, ColorBarTexture_u);
				glVertex2f(ColorBarAreaX0, ColorBarAreaY0 + ColorBarAreaHeight);
				glTexCoord2f(0.0, 0.0);
				glVertex2f(ColorBarAreaX0, ColorBarAreaY0);
				glTexCoord2f(ColorBarTexture_t, 0.0);
				glVertex2f(ColorBarAreaX0 + ColorBarAreaWidth, ColorBarAreaY0);

			}
			else {

				// Update the render texture coordinates - not inverted
				glTexCoord2f(0.0, 0.0);
				glVertex2f(ColorBarAreaX0 + ColorBarAreaWidth, ColorBarAreaY0 + ColorBarAreaHeight);
				glTexCoord2f(ColorBarTexture_t, 0.0);
				glVertex2f(ColorBarAreaX0, ColorBarAreaY0 + ColorBarAreaHeight);
				glTexCoord2f(ColorBarTexture_t, ColorBarTexture_u);
				glVertex2f(ColorBarAreaX0, ColorBarAreaY0);
				glTexCoord2f(0.0, ColorBarTexture_u);
				glVertex2f(ColorBarAreaX0 + ColorBarAreaWidth, ColorBarAreaY0);

			}

			// End of configuration
			glEnd();

			// Disable 2D texture
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);	

		}

		private: GLvoid RMH_OpenGL_RenderColorBarTickLinesAndLabels(GLfloat TickLineX0, GLfloat TickLineY0, GLfloat TickLineHeight, unsigned int NmbOfMinorTicks, GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine renders a tick line on the right side of the colorbar, as well as the associated given number of minor ticks

			// Read the temporary array data and sort the kernel array
			unsigned int i = 0;

			// If the given number of minor ticks is higher than the allowed number of colorbar ticks - 2
			if (NmbOfMinorTicks > _ColorBarMaxNumberOfTicks - 2) {

				// Limit to the maximum allowed number of minor colorbar ticks
				NmbOfMinorTicks = _ColorBarMaxNumberOfTicks - 2;

			}

			// Format the ColorBar tick line max/min tick line positions - scale with the texture window
			ColorBarMaxTickXPos = TickLineX0 * TextureToPanelScaleWidthFactor;
			ColorBarMaxTickYPos = TickLineY0 * TextureToPanelScaleHeightFactor;
			ColorBarMinTickXPos = TickLineX0 * TextureToPanelScaleWidthFactor;
			ColorBarMinTickYPos = (TickLineY0 + TickLineHeight) * TextureToPanelScaleHeightFactor;

			// Store the number of colorbar ticks globally in the class
			NmbOfColorBarTicks = NmbOfMinorTicks + 2;

			// Increment the given input number of ticks 
			NmbOfMinorTicks = NmbOfMinorTicks + 1;
			// Calculate the length of the major tick line
			MajorTickLinePixelSize = ColorBarMinTickYPos - ColorBarMaxTickYPos;
			// Calculate the Y position resolution of the minor tick lines
			MinorTickPixelRes = MajorTickLinePixelSize / NmbOfMinorTicks;

			// Enable OpenGL 1D texture
			glEnable(GL_TEXTURE_1D);

			// Set the color of the line
			glColor3ub(ColorR, ColorG, ColorB);
			// Set the thickness of the line
			glLineWidth(1);

			// Render the line on the texture
			glBegin(GL_LINES);

			// Render the primary ColorBar tick line
			glVertex2f(ColorBarMaxTickXPos, ColorBarMaxTickYPos);
			glVertex2f(ColorBarMaxTickXPos, ColorBarMinTickYPos);

			// Render the top max ColorBar tick line
			glVertex2f(ColorBarMaxTickXPos, ColorBarMaxTickYPos);
			glVertex2f(ColorBarMaxTickXPos + ColorBarMajorTickLength, ColorBarMaxTickYPos);

			// Store the major maximum label positions in arrays
			MinorTickLabelXPos[0] = ColorBarMaxTickXPos + ColorBarMajorTickLength + TickLabelXOffset;
			MinorTickLabelYPos[0] = ColorBarMaxTickYPos + TickLabelYOffset;

			// Render the minor colorbar tick lines
			for (i = 0; i < NmbOfMinorTicks - 1; i++) {

				// Calculate the Y position of the minor tick line
				ColorBarMaxTickYPos = ColorBarMaxTickYPos + MinorTickPixelRes;

				// Render the minor ColorBar tick line
				glVertex2f(ColorBarMaxTickXPos, ColorBarMaxTickYPos);
				glVertex2f(ColorBarMaxTickXPos + ColorBarMinorTickLength, ColorBarMaxTickYPos);

				// Store the minor tick positions in arrays
				MinorTickLabelXPos[i + 1] = ColorBarMaxTickXPos + ColorBarMinorTickLength + TickLabelXOffset;
				MinorTickLabelYPos[i + 1] = ColorBarMaxTickYPos + TickLabelYOffset;

			}

			// Store the major minimum label positions in arrays
			MinorTickLabelXPos[i + 1] = ColorBarMinTickXPos + ColorBarMajorTickLength + TickLabelXOffset;
			MinorTickLabelYPos[i + 1] = ColorBarMinTickYPos + TickLabelYOffset;

			// Render the bottom min ColorBar tick line
			glVertex2f(ColorBarMinTickXPos, ColorBarMinTickYPos);
			glVertex2f(ColorBarMinTickXPos + ColorBarMajorTickLength, ColorBarMinTickYPos);

			// End of configuration
			glEnd();
			// Disable the 1D texture
			glDisable(GL_TEXTURE_1D);

			// Render the temperature labels of the colorbar
			RMH_OpenGL_RenderColorBarLabels(NmbOfMinorTicks, ColorR, ColorG, ColorB);

		}

		private: GLvoid RMH_OpenGL_RenderColorBarLabels(unsigned int NmbOfMinorTicks, GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine renders the temperature labels of the colorbar

			// Read the temporary array data and sort the kernel array
			unsigned int i = 0;

			// Render the maximum temperature label of the colorbar
			RMH_OpenGL_RenderStringOnTexture(MinorTickLabelXPos[0], MinorTickLabelYPos[0] + 1, 
				RMH_Conversion_SystemStringToStdString(PrivateLocals::ColorBarTickLabels[0]), ColorR, ColorG, ColorB);

			// Loop through the number of minor colorbar ticks
			for (i = 0; i < NmbOfMinorTicks - 1; i++) {

				// Render the minor tick labels of the colorbar
				RMH_OpenGL_RenderStringOnTexture(MinorTickLabelXPos[i + 1], MinorTickLabelYPos[i + 1], 
					RMH_Conversion_SystemStringToStdString(PrivateLocals::ColorBarTickLabels[i + 1]), ColorR, ColorG, ColorB);

			}

			// Render the minimum temperature label of the colorbar
			RMH_OpenGL_RenderStringOnTexture(MinorTickLabelXPos[i + 1], MinorTickLabelYPos[i + 1] - 1, 
				RMH_Conversion_SystemStringToStdString(PrivateLocals::ColorBarTickLabels[i + 1]), ColorR, ColorG, ColorB);

		}

		private: GLvoid RMH_OpenGL_RenderColorBarLimitTag(GLfloat TagX0, GLfloat TagY0, GLfloat TagLength, GLfloat TagHeight, GLfloat ArrowLength, GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine renders a ColorBar tag at the given position

			// Read the temporary array data and sort the kernel array
			GLfloat TagPos1 = 0.0f;
			GLfloat TagPos2 = 0.0f;
			GLfloat TagPos3 = 0.0f;
			GLfloat TagPos4 = 0.0f;

			// Calculate the tag size when scaling the texture window
			TagX0 = TagX0 * TextureToPanelScaleWidthFactor;
			TagY0 = TagY0 * TextureToPanelScaleHeightFactor;
			ArrowLength = ArrowLength * TextureToPanelScaleWidthFactor;
			TagLength = TagLength * TextureToPanelScaleWidthFactor;
			TagHeight = TagHeight * TextureToPanelScaleHeightFactor;

			// Calculate the positions of the tag polygon lines
			TagPos1 = (TagX0 - ArrowLength);
			TagPos2 = (TagX0 - ArrowLength - TagLength);
			TagPos3 = (TagY0 - (TagHeight * 0.5));
			TagPos4 = (TagY0 + (TagHeight * 0.5));

			// Enable OpenGL 1D texture
			glEnable(GL_TEXTURE_1D);

			// Set the polygon color
			glColor3ub(ColorR, ColorG, ColorB);
			// Set the polygon line thickness
			glLineWidth(1);

			// Render the polygon on the texture
			glBegin(GL_POLYGON);

			// Render the tag polygon lines
			glVertex2f(TagX0, TagY0);
			glVertex2f(TagPos1, TagPos3);
			glVertex2f(TagX0, TagY0);
			glVertex2f(TagPos1, TagPos4);
			glVertex2f(TagPos1, TagPos3);
			glVertex2f(TagPos2, TagPos3);
			glVertex2f(TagPos1, TagPos4);
			glVertex2f(TagPos2, TagPos4);
			glVertex2f(TagPos2, TagPos3);
			glVertex2f(TagPos2, TagPos4);

			// End of configuration
			glEnd();
			// Disable the 1D texture
			glDisable(GL_TEXTURE_1D);

		}

		private: GLvoid RMH_OpenGL_RenderArrowWithLabel(GLfloat ArrowX0, GLfloat ArrowY0, GLfloat ArrowLength, GLfloat ArrowHeadFactor, std::string ArrowLabel, GLubyte LineColorR, GLubyte LineColorG, GLubyte LineColorB, GLubyte LabelColorR, GLubyte LabelColorG, GLubyte LabelColorB) {

			// This routine renders a right-oriented horizontal arrow, with an associated label

			// Calculate the arrow parameters when scaling the texture window
			ArrowX0 = ArrowX0 * TextureToPanelScaleWidthFactor;
			ArrowY0 = ArrowY0 * TextureToPanelScaleHeightFactor;
			ArrowLength = ArrowLength * TextureToPanelScaleWidthFactor;

			// Enable OpenGL 1D texture
			glEnable(GL_TEXTURE_1D);

			// Set the color of the arrow
			glColor3ub(LineColorR, LineColorG, LineColorB);
			// Set the line thickness of the arrow
			glLineWidth(1);

			// Render lines on the texture
			glBegin(GL_LINES);

			// Render the primary line of the arrow
			glVertex2f(ArrowX0, ArrowY0);
			glVertex2f(ArrowX0 - ArrowLength, ArrowY0);
			
			// Render the direction lines of the arrow
			glVertex2f(ArrowX0, ArrowY0);
			glVertex2f(ArrowX0 - (ArrowLength / ArrowHeadFactor), ArrowY0 - 1);
			glVertex2f(ArrowX0, ArrowY0);
			glVertex2f(ArrowX0 - (ArrowLength / ArrowHeadFactor), ArrowY0 + 1);

			// End of configuration
			glEnd();
			// Disable the 1D texture
			glDisable(GL_TEXTURE_1D);

			// Render the associated label of the arrow
			RMH_OpenGL_RenderStringOnTexture(ArrowX0 - ArrowLength, ArrowY0 - 0.5, ArrowLabel, LabelColorR, LabelColorG, LabelColorB);

		}

		private: GLvoid RMH_OpenGL_RenderMaxArrowOutsideIndicator(GLfloat X0Pos, GLfloat Y0Pos, GLfloat Height, GLfloat Width, GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine renders an indicator that shows whether the maximum tracking arrow is outside the colorbar area

			// Calculate the indicator parameters when scaling the texture window
			X0Pos = X0Pos * TextureToPanelScaleWidthFactor;
			Y0Pos = Y0Pos * TextureToPanelScaleHeightFactor;
			Width = Width * TextureToPanelScaleWidthFactor;
			Height = Height * TextureToPanelScaleHeightFactor;

			// Enable OpenGL 1D texture
			glEnable(GL_TEXTURE_1D);

			// Set the polygon color
			glColor3ub(ColorR, ColorG, ColorB);
			// Set the polygon line thickness
			glLineWidth(1);

			// Render the polygon on the texture
			glBegin(GL_POLYGON);

			// Render the indicator polygon lines
			glVertex2f(X0Pos, Y0Pos);
			glVertex2f(X0Pos + (Width * 0.5), Y0Pos + Height);
			glVertex2f(X0Pos, Y0Pos);
			glVertex2f(X0Pos - (Width * 0.5), Y0Pos + Height);
			glVertex2f(X0Pos + (Width * 0.5), Y0Pos + Height);
			glVertex2f(X0Pos - (Width * 0.5), Y0Pos + Height);

			// End of configuration
			glEnd();
			// Disable the 1D texture
			glDisable(GL_TEXTURE_1D);

		}

		private: GLvoid RMH_OpenGL_RenderMinArrowOutsideIndicator(GLfloat X0Pos, GLfloat Y0Pos, GLfloat Height, GLfloat Width, GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine renders an indicator that shows whether the maximum tracking arrow is outside the colorbar area

			// Calculate the indicator parameters when scaling the texture window
			X0Pos = X0Pos * TextureToPanelScaleWidthFactor;
			Y0Pos = Y0Pos * TextureToPanelScaleHeightFactor;
			Width = Width * TextureToPanelScaleWidthFactor;
			Height = Height * TextureToPanelScaleHeightFactor;

			// Enable OpenGL 1D texture
			glEnable(GL_TEXTURE_1D);

			// Set the polygon color
			glColor3ub(ColorR, ColorG, ColorB);
			// Set the polygon line thickness
			glLineWidth(1);

			// Render the polygon on the texture
			glBegin(GL_POLYGON);

			// Render the indicator polygon lines
			glVertex2f(X0Pos, Y0Pos);
			glVertex2f(X0Pos + (Width * 0.5), Y0Pos - Height);
			glVertex2f(X0Pos, Y0Pos);
			glVertex2f(X0Pos - (Width * 0.5), Y0Pos - Height);
			glVertex2f(X0Pos + (Width * 0.5), Y0Pos - Height);
			glVertex2f(X0Pos - (Width * 0.5), Y0Pos - Height);

			// End of configuration
			glEnd();
			// Disable the 1D texture
			glDisable(GL_TEXTURE_1D);

		}

		// ------------- Position-Adjustable Colorbar Tag Handling Routines ------------- //

		private: bool RMH_OpenGL_IsCursorInsideTag(GLdouble MouseXPosition, GLdouble MouseYPosition, GLfloat TagX0, GLfloat TagY0, GLfloat TagLength, GLfloat TagHeight, GLfloat ArrowLength) {

			// This routine checks whether the mouse cursor is within the tag area

			// Read the temporary array data and sort the kernel array
			bool IsInsideStatus = false;

			// Check whether the mouse cursor is inside the tag area - X coordinate
			if (MouseXPosition >= TagX0 - ArrowLength - TagLength && MouseXPosition <= TagX0) {

				// Check whether the mouse cursor is inside the tag area - Y coordinate
				if (MouseYPosition >= TagY0 - (TagHeight / 2.0) && 
					MouseYPosition <= TagY0 + (TagHeight / 2.0)) {

					// Update the cursor position status
					IsInsideStatus = true;

				}

			}

			// Return the cursor position status
			return IsInsideStatus;

		}

		private: GLvoid RMH_OpenGL_HandleTagMouseDownEvents(GLdouble MouseXPosition, GLdouble MouseYPosition) {

			// This routine handles events and states when a position-adjustable tag is clicked

			// Is tag movement not enabled 
			if (ColorBarTagMoveEnableFlag == false) {

				// Do not continue
				return;

			}

			// Reset the selected tag index value
			SelectedTagIndex = 0;

			// Loop through all active tags
			for (unsigned short i = 0; i < _NumberOfMovableTags + 1; i++) {

				// Check whether the mouse cursor is inside the active tag area 
				if (RMH_OpenGL_IsCursorInsideTag(MouseXPosition, MouseYPosition, MaxMinTagX0Pos - MaxMinTagX0Offset,
					MovableTagY0[i], MaxMinTagLength, MaxMinTagHeight, MaxMinTagArrowLength)) {

					// Update the tag move flag
					TagMoveFlag = true;

					// Store the selected tag index
					SelectedTagIndex = i;

					// Break the for loop
					break;

				}

			}

			// Read the current tag coordinates/positions on a new click
			ClickTagYPositionOffset = MouseYPosition - MovableTagY0[SelectedTagIndex];

		}

		private: GLvoid RMH_OpenGL_HandleTagMouseMoveEvents(GLdouble MouseXPosition, GLdouble MouseYPosition) {

			// This routine handles events and states when a clicked tag is to move

			// Add the tag click offset to the mouse position
			MouseYPosition = MouseYPosition - ClickTagYPositionOffset;

			// If the panel has not yet been clicked, or if tag movement is not enabled 
			if (OverlayPanelIsClick == false || ColorBarTagMoveEnableFlag == false) {

				// Do not continue
				return;

			}

			// Should the ColorBar tag move
			if (TagMoveFlag == true) {

				// Update the X and Y coordinates of the tag
				MovableTagY0[SelectedTagIndex] = MouseYPosition;

			}

			// Limit the position of the tag to the colorbar area and handle the common positioning
			if (MovableTagY0[SelectedTagIndex] <= ColorBarPanelTexturePadding) { MovableTagY0[SelectedTagIndex] = ColorBarPanelTexturePadding; }
			if (MovableTagY0[SelectedTagIndex] >= ColorBarPixelHeight + ColorBarPanelTexturePadding) { MovableTagY0[SelectedTagIndex] = ColorBarPixelHeight + ColorBarPanelTexturePadding; }
			if ((MovableTagY0[MaxTagID] + (MaxMinTagHeight / 2) >= MovableTagY0[MinTagID] - (MaxMinTagHeight / 2)) && SelectedTagIndex == MinTagID) {
				MovableTagY0[MaxTagID] = MovableTagY0[MinTagID] - MaxMinTagHeight;
				if (MovableTagY0[MaxTagID] <= ColorBarPanelTexturePadding) {
					MovableTagY0[MaxTagID] = ColorBarPanelTexturePadding;
					MovableTagY0[MinTagID] = ColorBarPanelTexturePadding + MaxMinTagHeight;
				}
			}
			if ((MovableTagY0[MinTagID] - (MaxMinTagHeight / 2) <= MovableTagY0[MaxTagID] + (MaxMinTagHeight / 2)) && SelectedTagIndex == MaxTagID) {
				MovableTagY0[MinTagID] = MovableTagY0[MaxTagID] + MaxMinTagHeight;
				if (MovableTagY0[MinTagID] >= ColorBarPixelHeight + ColorBarPanelTexturePadding) {
					MovableTagY0[MinTagID] = ColorBarPixelHeight + ColorBarPanelTexturePadding;
					MovableTagY0[MaxTagID] = (ColorBarPixelHeight + ColorBarPanelTexturePadding) - MaxMinTagHeight;
				}
			}

		}

		private: GLvoid RMH_OpenGL_RenderMovableColorBarTag(GLfloat TagX0, GLfloat TagXOffset, GLfloat TagLength, GLfloat TagHeight, GLfloat ArrowLength, unsigned char TagID, GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine renders a position-adjustable colorbar tag

			// Render Colorbar Tag
			RMH_OpenGL_RenderColorBarLimitTag(TagX0 - TagXOffset, MovableTagY0[TagID], TagLength, TagHeight, ArrowLength, ColorR, ColorG, ColorB);

		}

		// ----------- Texture Panel Interaction Cursor Event Callback Routines ------------ //

		private: GLvoid TexturePanel_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {

			// Read the mouse cursor position
			GLdouble MouseXPosition = RMH_OpenGL_TranslateOverlayPanelMouseXPosToTextureXPos(e);
			GLdouble MouseYPosition = RMH_OpenGL_TranslateOverlayPanelMouseYPosToTextureYPos(e);
			
			// Update the overlay panel click flag
			OverlayPanelIsClick = true;

			// Handle events when a position-adjustable tag is clicked
			RMH_OpenGL_HandleTagMouseDownEvents(MouseXPosition, MouseYPosition);

		}

		private: GLvoid TexturePanel_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {

			// Update the overlay panel click flag
			OverlayPanelIsClick = false;

			// Reset the tag move flag
			TagMoveFlag = false;
		
		}

		private: GLvoid TexturePanel_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {

			// Read the mouse cursor position
			GLdouble MouseXPosition = RMH_OpenGL_TranslateOverlayPanelMouseXPosToTextureXPos(e);
			GLdouble MouseYPosition = RMH_OpenGL_TranslateOverlayPanelMouseYPosToTextureYPos(e);

			// Handle events when a clicked tag is to move
			RMH_OpenGL_HandleTagMouseMoveEvents(MouseXPosition, MouseYPosition);

		}

		private: GLvoid TexturePanel_MouseWheel(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {

			// Read the temporary array data and sort the kernel array
			bool MouseWheelPolNegativeFlag = false;
			GLfloat ColorBarPanelHeight = ColorBarPixelHeight + (ColorBarPanelTexturePadding * 2.0f);
			GLfloat ColorBarPanelMiddle = ColorBarPanelHeight / 2.0f;

			// Check the polarity of the scroll direction
			if (e->Delta > 0) {

				// Positive polarity
				MouseWheelScrollPolarity = MouseWheelStepSize;
				// Update the polarity flag of the mouse wheel
				MouseWheelPolNegativeFlag = false;

			}
			else {

				// Negative polarity
				MouseWheelScrollPolarity = -MouseWheelStepSize;
				// Update the polarity flag of the mouse wheel
				MouseWheelPolNegativeFlag = true;

			}

			// Check whether the mouse is in the top or bottom part of the colorbar panel
			if (RMH_OpenGL_TranslateOverlayPanelMouseYPosToTextureYPos(e) > ColorBarPanelMiddle) {

				// Is the mouse wheel polarity positive
				if (MouseWheelPolNegativeFlag == false) {

					// Is the min temperature range setting of the colorbar lower than the max temperature setting
					if (ColorBarMinTempRange < ColorBarMaxTempRange) {

						// Update the minimum temperature offset value of the colorbar from the polarity read
						ColorBarMinRangeOffsetValue = ColorBarMinRangeOffsetValue + MouseWheelScrollPolarity;

					}
					else {

						// Set the min range value to the max range value
						ColorBarMinTempRange = ColorBarMaxTempRange;

					}
	
				}
				else {

					// Update the minimum temperature offset value of the colorbar from the polarity read
					ColorBarMinRangeOffsetValue = ColorBarMinRangeOffsetValue + MouseWheelScrollPolarity;

				}

			}
			else {

				// Is the mouse wheel polarity negative
				if (MouseWheelPolNegativeFlag == true) {

					// Is the max temperature range setting of the colorbar higher than the min temperature setting
					if (ColorBarMaxTempRange > ColorBarMinTempRange) {

						// Update the maximum temperature offset value of the colorbar from the polarity read
						ColorBarMaxRangeOffsetValue = ColorBarMaxRangeOffsetValue + MouseWheelScrollPolarity;

					}
					else {

						// Set the max range value to the min range value
						ColorBarMaxTempRange = ColorBarMinTempRange;

					}

				}
				else {

					// Update the maximum temperature offset value of the colorbar from the polarity read
					ColorBarMaxRangeOffsetValue = ColorBarMaxRangeOffsetValue + MouseWheelScrollPolarity;

				}

			}

		}

		// ---------------------- ColorBar Data Handling Routines ----------------------- //
		
		public: GLvoid RMH_OpenGL_LoadFirstColorPalettesData(unsigned short ColorPalette[3][16384]) {

			// This routine loads the given color palette data into the global color palette array

			// Loop up to and including the size of the given color palettes
			for (unsigned int i = 0, j = 0; j < ColorBarPaletteResolution; i += 32, j += 8) {
	
				// Write the first color palette data to the global palette array
				FirstColorPaletteData[i + 0] = ColorPalette[0][j];
				FirstColorPaletteData[i + 1] = ColorPalette[1][j];
				FirstColorPaletteData[i + 2] = ColorPalette[2][j];
				FirstColorPaletteData[i + 3] = ColorBarPaletteIntegerRange; // Alpha channel
				FirstColorPaletteData[i + 4] = ColorPalette[0][j + 1];
				FirstColorPaletteData[i + 5] = ColorPalette[1][j + 1];
				FirstColorPaletteData[i + 6] = ColorPalette[2][j + 1];
				FirstColorPaletteData[i + 7] = ColorBarPaletteIntegerRange; // Alpha channel
				FirstColorPaletteData[i + 8] = ColorPalette[0][j + 2];
				FirstColorPaletteData[i + 9] = ColorPalette[1][j + 2];
				FirstColorPaletteData[i + 10] = ColorPalette[2][j + 2];
				FirstColorPaletteData[i + 11] = ColorBarPaletteIntegerRange; // Alpha channel
				FirstColorPaletteData[i + 12] = ColorPalette[0][j + 3];
				FirstColorPaletteData[i + 13] = ColorPalette[1][j + 3];
				FirstColorPaletteData[i + 14] = ColorPalette[2][j + 3];
				FirstColorPaletteData[i + 15] = ColorBarPaletteIntegerRange; // Alpha channel
				FirstColorPaletteData[i + 16] = ColorPalette[0][j + 4];
				FirstColorPaletteData[i + 17] = ColorPalette[1][j + 4];
				FirstColorPaletteData[i + 18] = ColorPalette[2][j + 4];
				FirstColorPaletteData[i + 19] = ColorBarPaletteIntegerRange; // Alpha channel
				FirstColorPaletteData[i + 20] = ColorPalette[0][j + 5];
				FirstColorPaletteData[i + 21] = ColorPalette[1][j + 5];
				FirstColorPaletteData[i + 22] = ColorPalette[2][j + 5];
				FirstColorPaletteData[i + 23] = ColorBarPaletteIntegerRange; // Alpha channel
				FirstColorPaletteData[i + 24] = ColorPalette[0][j + 6];
				FirstColorPaletteData[i + 25] = ColorPalette[1][j + 6];
				FirstColorPaletteData[i + 26] = ColorPalette[2][j + 6];
				FirstColorPaletteData[i + 27] = ColorBarPaletteIntegerRange; // Alpha channel
				FirstColorPaletteData[i + 28] = ColorPalette[0][j + 7];
				FirstColorPaletteData[i + 29] = ColorPalette[1][j + 7];
				FirstColorPaletteData[i + 30] = ColorPalette[2][j + 7];
				FirstColorPaletteData[i + 31] = ColorBarPaletteIntegerRange; // Alpha channel
				
			}

		}
		
		public: GLvoid RMH_OpenGL_LoadSecondColorPalettesData(unsigned short ColorPalette[3][16384]) {

			// This routine loads the given color palette data into the global color palette array

			// Loop up to and including the size of the given color palettes
			for (unsigned int i = 0, j = 0; j < ColorBarPaletteResolution; i += 32, j += 8) {

				// Write the first color palette data to the global palette array
				SecondColorPaletteData[i + 0] = ColorPalette[0][j];
				SecondColorPaletteData[i + 1] = ColorPalette[1][j];
				SecondColorPaletteData[i + 2] = ColorPalette[2][j];
				SecondColorPaletteData[i + 3] = ColorBarPaletteIntegerRange; // Alpha channel
				SecondColorPaletteData[i + 4] = ColorPalette[0][j + 1];
				SecondColorPaletteData[i + 5] = ColorPalette[1][j + 1];
				SecondColorPaletteData[i + 6] = ColorPalette[2][j + 1];
				SecondColorPaletteData[i + 7] = ColorBarPaletteIntegerRange; // Alpha channel
				SecondColorPaletteData[i + 8] = ColorPalette[0][j + 2];
				SecondColorPaletteData[i + 9] = ColorPalette[1][j + 2];
				SecondColorPaletteData[i + 10] = ColorPalette[2][j + 2];
				SecondColorPaletteData[i + 11] = ColorBarPaletteIntegerRange; // Alpha channel
				SecondColorPaletteData[i + 12] = ColorPalette[0][j + 3];
				SecondColorPaletteData[i + 13] = ColorPalette[1][j + 3];
				SecondColorPaletteData[i + 14] = ColorPalette[2][j + 3];
				SecondColorPaletteData[i + 15] = ColorBarPaletteIntegerRange; // Alpha channel
				SecondColorPaletteData[i + 16] = ColorPalette[0][j + 4];
				SecondColorPaletteData[i + 17] = ColorPalette[1][j + 4];
				SecondColorPaletteData[i + 18] = ColorPalette[2][j + 4];
				SecondColorPaletteData[i + 19] = ColorBarPaletteIntegerRange; // Alpha channel
				SecondColorPaletteData[i + 20] = ColorPalette[0][j + 5];
				SecondColorPaletteData[i + 21] = ColorPalette[1][j + 5];
				SecondColorPaletteData[i + 22] = ColorPalette[2][j + 5];
				SecondColorPaletteData[i + 23] = ColorBarPaletteIntegerRange; // Alpha channel
				SecondColorPaletteData[i + 24] = ColorPalette[0][j + 6];
				SecondColorPaletteData[i + 25] = ColorPalette[1][j + 6];
				SecondColorPaletteData[i + 26] = ColorPalette[2][j + 6];
				SecondColorPaletteData[i + 27] = ColorBarPaletteIntegerRange; // Alpha channel
				SecondColorPaletteData[i + 28] = ColorPalette[0][j + 7];
				SecondColorPaletteData[i + 29] = ColorPalette[1][j + 7];
				SecondColorPaletteData[i + 30] = ColorPalette[2][j + 7];
				SecondColorPaletteData[i + 31] = ColorBarPaletteIntegerRange; // Alpha channel

			}

		}
		
		public: ColorBarTagPosition RMH_OpenGL_ReadColorBarTagsPositions() {

			// This routine reads and returns the positions of the colorbar max/min tags on the colorbar

			// Read the temporary array data and sort the kernel array
			ColorBarTagPosition TagPositions;

			// Read the maximum and minimum tag positions
			TagPositions.MaximumTagPos = (MovableTagY0[MaxTagID] - ColorBarPanelTexturePadding) * ((GLfloat)ColorBarPaletteResolution / ColorBarPixelHeight);
			TagPositions.MinimumTagPos = (MovableTagY0[MinTagID] - ColorBarPanelTexturePadding) * ((GLfloat)ColorBarPaletteResolution / ColorBarPixelHeight);

			// Return the positions
			return TagPositions;

		}

		public: GLvoid RMH_OpenGL_SetColorBarMaxMinRangeTemperature(GLfloat MaxTemperature, GLfloat MinTemperature) {

			// This routine sets the maximum and minimum temperature range of the colorbar

			// Is the manual colorbar temperature range enabled
			if (BarManualRangeFlag == true && BarManualHighRangeFlag == false && BarManualLowRangeFlag == false) {

				// Read and store the maximum and minimum temperatures globally in the class
				ColorBarMaxTempRange = MaxTemperature + ColorBarMaxRangeOffsetValue;
				ColorBarMinTempRange = MinTemperature + ColorBarMinRangeOffsetValue;

			}
			else if (BarManualRangeFlag == false && BarManualHighRangeFlag == true && BarManualLowRangeFlag == false) {

				// Read and store the maximum and minimum temperatures globally in the class
				ColorBarMaxTempRange = MaxTemperature + ColorBarMaxRangeOffsetValue;
				ColorBarMinTempRange = MinTemperature;

			}
			else if (BarManualRangeFlag == false && BarManualHighRangeFlag == false && BarManualLowRangeFlag == true) {

				// Read and store the maximum and minimum temperatures globally in the class
				ColorBarMaxTempRange = MaxTemperature;
				ColorBarMinTempRange = MinTemperature + ColorBarMinRangeOffsetValue;

			}
			else {

				// Read and store the maximum and minimum temperatures globally in the class
				ColorBarMaxTempRange = MaxTemperature;
				ColorBarMinTempRange = MinTemperature;

			}

			// Is the max temperature range setting of the colorbar higher than the min temperature setting
			if (ColorBarMaxTempRange <= ColorBarMinTempRange) {

				// Set the max range value to the min range value
				ColorBarMaxTempRange = ColorBarMinTempRange;

			}

			// Is the min temperature range setting of the colorbar lower than the max temperature setting
			if (ColorBarMinTempRange >= ColorBarMaxTempRange) {

				// Set the min range value to the max range value
				ColorBarMinTempRange = ColorBarMaxTempRange;

			}

		}

		public: GLvoid RMH_OpenGL_ResetColorbarMaxMinRangeOffsetValues() {

			// This routine resets the maximum and minimum temperature range offset values for manual range mode

			// Reset the maximum and minimum temperature range offset values
			ColorBarMaxRangeOffsetValue = 0.0;
			ColorBarMinRangeOffsetValue = 0.0;

		}
		
		public: GLvoid RMH_OpenGL_FormatColorBarTickAndTagLabelStrings(System::String^ TempUnitString) {

			// This routine formats the major and minor tick label strings of the colorbar

			// Read the temporary array data and sort the kernel array
			unsigned char i = 0;
			GLfloat TempDifference;
			GLfloat TempTickStepDiff;
			GLfloat MaxTagTemperature;
			GLfloat MinTagTemperature;
			ColorBarTagPosition MaxMinTagsPositions;
			GLfloat TickTemperature = ColorBarMaxTempRange;

			// Read the tag positions of the colorbar
			MaxMinTagsPositions = RMH_OpenGL_ReadColorBarTagsPositions();

			// Calculate the temperature difference from max to min
			TempDifference = ColorBarMaxTempRange - ColorBarMinTempRange;
			// Calculate the temperature tick step difference 
			TempTickStepDiff = TempDifference / ((GLfloat)(NmbOfColorBarTicks - 1));

			// Calculate the max/min tag temperatures of the colorbar
			MaxTagTemperature = ColorBarMaxTempRange - ((TempDifference / ColorBarPaletteResolution) * MaxMinTagsPositions.MaximumTagPos);
			MinTagTemperature = ColorBarMaxTempRange - ((TempDifference / ColorBarPaletteResolution) * MaxMinTagsPositions.MinimumTagPos);

			// Format the maximum temperature tick label of the colorbar 
			PrivateLocals::ColorBarTickLabels[0] = ColorBarMaxTempRange.ToString("F2") + " " + TempUnitString;

			// Loop for the selected number of colorbar minor ticks
			for (i = 0; i < NmbOfColorBarTicks - 2; i++) {

				// Decrement the minor tick temperature value
				TickTemperature = TickTemperature - TempTickStepDiff;

				// Format the minor temperature tick labels of the colorbar 
				PrivateLocals::ColorBarTickLabels[i + 1] = TickTemperature.ToString("F1") + " " + TempUnitString;

			}

			// Format the minimum temperature tick label of the colorbar 
			PrivateLocals::ColorBarTickLabels[i + 1] = ColorBarMinTempRange.ToString("F2") + " " + TempUnitString;

			// Format the max/min tag temperatures to System::Strings
			MaximumTagSystemString = MaxTagTemperature.ToString("F1") + TempUnitString;
			MinimumTagSystemString = MinTagTemperature.ToString("F1") + TempUnitString;

			// If the maximum tag temperature is equal to the maximum temperature of the colorbar
			if ((unsigned int)(MaxTagTemperature * 1000.0) == (unsigned int)(ColorBarMaxTempRange * 1000.0)) {

				// Set the tag string to a fixed string
				MaximumTagStdString = "MAX";

			}
			else {

				// Display the tag setting temperature in the tag
				MaximumTagStdString = RMH_Conversion_SystemStringToStdString(MaximumTagSystemString);

			}

			// If the minimum tag temperature is equal to the minimum temperature of the colorbar
			if ((unsigned int)(MinTagTemperature * 1000.0) == (unsigned int)(ColorBarMinTempRange * 1000.0)) {

				// Set the tag string to a fixed string
				MinimumTagStdString = "MIN";

			}
			else {

				// Display the tag setting temperature in the tag
				MinimumTagStdString = RMH_Conversion_SystemStringToStdString(MinimumTagSystemString);

			}

		}

		public: GLvoid RMH_OpenGL_UpdateColorBarMaxMinCenterTempArrowsPos(GLfloat FrameMaxTemp, GLfloat FrameMinTemp, GLfloat FrameCenterTemp) {

			// This routine updates the positions of the maximum, minimum and center temperature arrows of the colorbar 

			// Read the temporary array data and sort the kernel array
			GLfloat LinearScaleFactor;
			GLfloat OffsetScale;

			// Calculate the linear scaling factor (a parameter)
			LinearScaleFactor = ((ColorBarPixelHeight + ColorBarPanelTexturePadding) - ColorBarPanelTexturePadding) / (ColorBarMinTempRange - ColorBarMaxTempRange);
			// Calculate the linear offset scaling (b parameter)
			OffsetScale = -ColorBarMaxTempRange * LinearScaleFactor + ColorBarPanelTexturePadding;

			// Calculate and update the positions of the maximum, minimum and center temperature arrows of the colorbar
			ColorBarMaxArrowYPos = LinearScaleFactor * FrameMaxTemp + OffsetScale;
			ColorBarMinArrowYPos = LinearScaleFactor * FrameMinTemp + OffsetScale;
			ColorBarCntArrowYPos = LinearScaleFactor * FrameCenterTemp + OffsetScale;

		}
		
		public: ColorBarManualRangeTemps RMH_OpenGL_ReadColorBarRangeMaxMinValues() {

			// This routine reads and returns the maximum and minimum colorbar range values

			// Read the temporary array data and sort the kernel array
			ColorBarManualRangeTemps TempRangeValues;

			// Read the maximum and minimum colorbar range values
			TempRangeValues.ManualMaxRangeTemp = ColorBarMaxTempRange;
			TempRangeValues.ManualMinRangeTemp = ColorBarMinTempRange;

			// Return the maximum and minimum colorbar range values
			return TempRangeValues;

		}

		public: GLvoid RMH_OpenGL_SetMouseWheelTempOffsetStepSize(GLfloat StepSize) {

			// This routine sets the temperature offset step size for the mouse scroll function of the colorbar

			// Set the temperature offset step size for the mouse scroll function of the colorbar
			MouseWheelStepSize = StepSize;

		}

		public: GLvoid RMH_OpenGL_InvertColorBarPalettes(bool InvertFirstPalette, bool InvertSecondPalette) {

			// This routine configures the inversion of the two ColorBar color palettes

			// Update the local class variables - inversion variables
			InvertFirstPaletteFlag = InvertFirstPalette;
			InvertSecondPaletteFlag = InvertSecondPalette;

		}

		// --------------------- ColorBar Color Setting Routines ---------------------- //

		public: GLvoid RMH_OpenGL_SetColorBarTickLineColor(GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine sets the color of the tick line of the colorbar

			// Set the tick line color of the colorbar
			TickLineColorR = ColorR;
			TickLineColorG = ColorG;
			TickLineColorB = ColorB;

		}

		public: GLvoid RMH_OpenGL_SetColorBarMaxTagColor(GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine sets the color of the maximum temperature tag of the colorbar

			// Set the color of the maximum tag of the colorbar
			MaxTagColorR = ColorR;
			MaxTagColorG = ColorG;
			MaxTagColorB = ColorB;

		}

		public: GLvoid RMH_OpenGL_SetColorBarMinTagColor(GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine sets the color of the minimum temperature tag of the colorbar

			// Set the color of the maximum tag of the colorbar
			MinTagColorR = ColorR;
			MinTagColorG = ColorG;
			MinTagColorB = ColorB;

		}

		public: GLvoid RMH_OpenGL_SetColorBarMaxArrowColor(GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine sets the color of the maximum temperature tracking arrow of the colorbar

			// Set the color of the maximum temperature tracking arrow of the colorbar
			MaxTrackArrowColorR = ColorR;
			MaxTrackArrowColorG = ColorG;
			MaxTrackArrowColorB = ColorB;

		}

		public: GLvoid RMH_OpenGL_SetColorBarMinArrowColor(GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine sets the color of the minimum temperature tracking arrow of the colorbar

			// Set the color of the minimum temperature tracking arrow of the colorbar
			MinTrackArrowColorR = ColorR;
			MinTrackArrowColorG = ColorG;
			MinTrackArrowColorB = ColorB;

		}

		public: GLvoid RMH_OpenGL_SetColorBarCenterArrowColor(GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine sets the color of the center temperature tracking arrow of the colorbar

			// Set the color of the center temperature tracking arrow of the colorbar
			CntTrackArrowColorR = ColorR;
			CntTrackArrowColorG = ColorG;
			CntTrackArrowColorB = ColorB;

		}

		public: GLvoid RMH_OpenGL_SetColorBarMaxArrowLabelColor(GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine sets the color of the label of the maximum temperature tracking arrow of the colorbar

			// Set the label color of the maximum temperature tracking arrow of the colorbar
			MaxTrackArrowLabelColorR = ColorR;
			MaxTrackArrowLabelColorG = ColorG;
			MaxTrackArrowLabelColorB = ColorB;

		}

		public: GLvoid RMH_OpenGL_SetColorBarMinArrowLabelColor(GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine sets the color of the label of the minimum temperature tracking arrow of the colorbar

			// Set the label color of the minimum temperature tracking arrow of the colorbar
			MinTrackArrowLabelColorR = ColorR;
			MinTrackArrowLabelColorG = ColorG;
			MinTrackArrowLabelColorB = ColorB;

		}

		public: GLvoid RMH_OpenGL_SetColorBarCenterLabelArrowColor(GLubyte ColorR, GLubyte ColorG, GLubyte ColorB) {

			// This routine sets the color of the label of the center temperature tracking arrow of the colorbar

			// Set the label color of the center temperature tracking arrow of the colorbar
			CntTrackArrowLabelColorR = ColorR;
			CntTrackArrowLabelColorG = ColorG;
			CntTrackArrowLabelColorB = ColorB;

		}

		// ---------------- Combined ColorBar Graphics Rendering Routine ----------------- //

		public: GLvoid RMH_OpenGL_RenderColorBar(unsigned int TexturePanelWidth, unsigned int TexturePanelHeight, bool DualPaletteEnableFlag, unsigned char NmbOfMinorTicks, bool EnableCenterArrowFlag, bool ManualRangeFlag, bool ManualRangeHighFlag, bool ManualRangeLowFlag) {

			// This routine renders all graphical objects of the colorbar
			// Various graphical offset parameters and values can be further configured here

			// Read the current pixel height and width of the texture panel 
			CurrentTexturePanelHeight = (GLdouble)TexturePanelHeight;
			CurrentTexturePanelWidth = (GLdouble)TexturePanelWidth;

			// Calculate the texture-to-panel scaling factor
			TextureToPanelScaleWidthFactor = CurrentTexturePanelWidth / TotalTextureScalableWidth;
			TextureToPanelScaleHeightFactor = CurrentTexturePanelHeight / TotalTextureScalableHeight;

			// Make the associated render context the current render context
			RMH_OpenGL_MakeRenderContextCurrent();
			// Clear the texture color and bit buffers
			RMH_OpenGL_ClearTextureBuffer();
			// Update the texture field of view
			RMH_OpenGL_UpdateTextureFieldOfView(TextureWidth, TextureHeight);

			// Bind the OpenGL texture and start OpenGL rendering
			RMH_OpenGL_StartColorBarRender();

			// Render the primary ColorBar
			RMH_OpenGL_RenderColorBarPalette(ColorBarX0TexturePos, ColorBarPanelTexturePadding, ColorBarTextureWidth, ColorBarPixelHeight, &FirstColorPaletteData[0], InvertFirstPaletteFlag);

			// Should the dual color palette be shown
			if (DualPaletteEnableFlag == true) {

				// Render the secondary ColorBar
				RMH_OpenGL_RenderColorBarPalette(ColorBarX0TexturePos, ColorBarPanelTexturePadding, ColorBarTextureWidth / 2.0f, ColorBarPixelHeight, &SecondColorPaletteData[0], InvertSecondPaletteFlag);

			}

			// Render the tick line of the colorbar
			RMH_OpenGL_RenderColorBarTickLinesAndLabels(ColorBarX0TexturePos + ColorBarTextureWidth + TickLineColorBarOffset, ColorBarPanelTexturePadding,
				ColorBarPixelHeight, NmbOfMinorTicks, TickLineColorR, TickLineColorG, TickLineColorB);

			// Reset the colorbar "temperature tracking arrow is outside the colorbar area" indicator flag 
			ShowTopOutsideIndicatorFlag = false;
			ShowButOutsideIndicatorFlag = false;

			// Set the range flag of the colorbar
			BarManualRangeFlag = ManualRangeFlag;
			BarManualHighRangeFlag = ManualRangeHighFlag;
			BarManualLowRangeFlag = ManualRangeLowFlag;

			// Is the manual colorbar temperature range enabled
			if (BarManualRangeFlag == true && BarManualHighRangeFlag == false && BarManualLowRangeFlag == false) {

				// Render the maximum indicator only if it is within the Y area of the colorbar
				if (ColorBarMaxArrowYPos < ColorBarPanelTexturePadding) {

					// The maximum temperature tracking arrow is above the maximum range temperature
					ShowTopOutsideIndicatorFlag = true;

				}
				else {

					// Render the maximum indicator only if it is within the Y area of the colorbar
					if (ColorBarMaxArrowYPos < ColorBarPixelHeight + ColorBarPanelTexturePadding) {

						// Render the maximum temperature indicator arrow of the colorbar
						RMH_OpenGL_RenderArrowWithLabel(MaxMinTagX0Pos - 1, ColorBarMaxArrowYPos, 5, 5, "MAX",
							MaxTrackArrowColorR, MaxTrackArrowColorG, MaxTrackArrowColorB,
							MaxTrackArrowLabelColorR, MaxTrackArrowLabelColorG, MaxTrackArrowLabelColorB);

					}
					else {

						// The maximum temperature tracking arrow is below the minimum range temperature
						ShowButOutsideIndicatorFlag = true;

					}

				}

				// Render the minimum indicator only if it is within the Y area of the colorbar
				if (ColorBarMinArrowYPos > ColorBarPixelHeight + ColorBarPanelTexturePadding) {

					// The minimum temperature tracking arrow is below the minimum range temperature
					ShowButOutsideIndicatorFlag = true;

				}
				else {

					// Render the minimum indicator only if it is within the Y area of the colorbar
					if (ColorBarMinArrowYPos > ColorBarPanelTexturePadding) {

						// Render the minimum temperature indicator arrow of the colorbar
						RMH_OpenGL_RenderArrowWithLabel(MaxMinTagX0Pos - 1, ColorBarMinArrowYPos, 5, 5, "MIN",
							MinTrackArrowColorR, MinTrackArrowColorG, MinTrackArrowColorB,
							MinTrackArrowLabelColorR, MinTrackArrowLabelColorG, MinTrackArrowLabelColorB);

					}
					else {

						// The minimum temperature tracking arrow is above the maximum range temperature
						ShowTopOutsideIndicatorFlag = true;

					}

				}

			}
			else if (BarManualRangeFlag == false && BarManualHighRangeFlag == true && BarManualLowRangeFlag == false) {

				// Render the maximum indicator only if it is within the Y area of the colorbar
				if (ColorBarMaxArrowYPos < ColorBarPanelTexturePadding) {

					// The maximum temperature tracking arrow is above the maximum range temperature
					ShowTopOutsideIndicatorFlag = true;

				}
				else {

					// Render the maximum indicator only if it is within the Y area of the colorbar
					if (ColorBarMaxArrowYPos < ColorBarPixelHeight + ColorBarPanelTexturePadding) {

						// Render the maximum temperature indicator arrow of the colorbar
						RMH_OpenGL_RenderArrowWithLabel(MaxMinTagX0Pos - 1, ColorBarMaxArrowYPos, 5, 5, "MAX",
							MaxTrackArrowColorR, MaxTrackArrowColorG, MaxTrackArrowColorB,
							MaxTrackArrowLabelColorR, MaxTrackArrowLabelColorG, MaxTrackArrowLabelColorB);

					}
					else {

						// The maximum temperature tracking arrow is below the minimum range temperature
						ShowButOutsideIndicatorFlag = true;

					}

				}

			}
			else if (BarManualRangeFlag == false && BarManualHighRangeFlag == false && BarManualLowRangeFlag == true) {

				// Render the minimum indicator only if it is within the Y area of the colorbar
				if (ColorBarMinArrowYPos > ColorBarPixelHeight + ColorBarPanelTexturePadding) {

					// The minimum temperature tracking arrow is below the minimum range temperature
					ShowButOutsideIndicatorFlag = true;

				}
				else {

					// Render the minimum indicator only if it is within the Y area of the colorbar
					if (ColorBarMinArrowYPos > ColorBarPanelTexturePadding) {

						// Render the minimum temperature indicator arrow of the colorbar
						RMH_OpenGL_RenderArrowWithLabel(MaxMinTagX0Pos - 1, ColorBarMinArrowYPos, 5, 5, "MIN",
							MinTrackArrowColorR, MinTrackArrowColorG, MinTrackArrowColorB,
							MinTrackArrowLabelColorR, MinTrackArrowLabelColorG, MinTrackArrowLabelColorB);

					}
					else {

						// The minimum temperature tracking arrow is above the maximum range temperature
						ShowTopOutsideIndicatorFlag = true;

					}

				}

			}

			// Is the center colorbar temperature indicator arrow enabled
			if (EnableCenterArrowFlag == true) {

				// Render the center indicator only if it is within the Y area of the colorbar
				if (ColorBarCntArrowYPos > ColorBarPanelTexturePadding) {

					// Render the center indicator only if it is within the Y area of the colorbar
					if (ColorBarCntArrowYPos < ColorBarPixelHeight + ColorBarPanelTexturePadding) {

						// Render the center temperature indicator arrow of the colorbar
						RMH_OpenGL_RenderArrowWithLabel(MaxMinTagX0Pos - 1, ColorBarCntArrowYPos, 7, 7, "Center",
							CntTrackArrowColorR, CntTrackArrowColorG, CntTrackArrowColorB,
							CntTrackArrowLabelColorR, CntTrackArrowLabelColorG, CntTrackArrowLabelColorB);

					}
					else {

						// The center temperature tracking arrow is below the minimum range temperature
						ShowButOutsideIndicatorFlag = true;

					}

				}
				else {

					// The center temperature tracking arrow is above the maximum range temperature
					ShowTopOutsideIndicatorFlag = true;

				}

			}

			// Are one or more tracking arrows above the maximum range temperature
			if (ShowTopOutsideIndicatorFlag == true) {

				// Render an indicator showing that the max/min/center tracking arrows are outside the colorbar area
				RMH_OpenGL_RenderMaxArrowOutsideIndicator(4, 3, 4, 4, MaxTrackArrowColorR, MaxTrackArrowColorG, MaxTrackArrowColorB);

			}

			// Are one or more tracking arrows below the minimum range temperature
			if (ShowButOutsideIndicatorFlag == true) {

				// Render an indicator showing that the max/min/center tracking arrows are outside the colorbar area
				RMH_OpenGL_RenderMinArrowOutsideIndicator(4, ColorBarPixelHeight + ColorBarPanelTexturePadding + 3, 4, 4, MinTrackArrowColorR, MinTrackArrowColorG, MinTrackArrowColorB);
					
			}

			// Render the colorbar maximum and minimum adjustable tags and the associated label strings
			RMH_OpenGL_RenderMovableColorBarTag(MaxMinTagX0Pos, MaxMinTagX0Offset, MaxMinTagLength, MaxMinTagHeight, MaxMinTagArrowLength, MaxTagID, MaxTagColorR, MaxTagColorG, MaxTagColorB);
			RMH_OpenGL_RenderMovableColorBarTag(MaxMinTagX0Pos, MaxMinTagX0Offset, MaxMinTagLength, MaxMinTagHeight, MaxMinTagArrowLength, MinTagID, MinTagColorR, MinTagColorG, MinTagColorB);
			RMH_OpenGL_RenderStringOnTexture(ColorBarAreaX0 - TagLabelXOffset, (MovableTagY0[MaxTagID] * TextureToPanelScaleHeightFactor) + TagLabelYOffset, MaximumTagStdString, TickLineColorR, TickLineColorG, TickLineColorB);
			RMH_OpenGL_RenderStringOnTexture(ColorBarAreaX0 - TagLabelXOffset, (MovableTagY0[MinTagID] * TextureToPanelScaleHeightFactor) + TagLabelYOffset, MinimumTagStdString, TickLineColorR, TickLineColorG, TickLineColorB);

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

		~RMHOpenGLColorBar(GLvoid) {

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
