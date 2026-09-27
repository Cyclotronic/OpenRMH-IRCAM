
/*
 *  Main.cpp
 *
 *  Author: Rune Mark Hansen
 *  Start Date: April 2023
 *  Updated: 25-09-2026
 * 
 *  TODO:
 * 
 *    Ideas ->
 *   
 *    Large tasks ->
 * 	
 *		- Update the histogram to PBO OpenGL
 *		- Update the live view snapshot functionality (BITMAP)
 *		- Update the temperature alarm functionality
 *      - Update the periodic trigger functionality
 *		- Test the new 2D plot speed!
 *		- If the surface plot form is undocked with the 2D plot form undocked, then the surface plot line mode cannot be changed?
 *		- Update the application with a faster surface plot using vertex arrays etc.
 *      - Update the application to use new, modern and faster OpenGL rendering.
 * 
 *    Medium tasks ->
 *   
 *    Small tasks ->
 *		
 *		- Add alarm "reset event trigger" as a periodic trigger option
 *		- Save the live view rotation setting of the session 
 *		- Add TNV256i camera
 *		- Add HT203U camera (Tiny1-C sensor)
 *		- Add HIKMICRO Mini2Plus
 *      - Add TOOLTOP T7 camera (test whether it is a P2/Pro variant...)
 * 
 *    Found errors, or features to check ->
 * 
 *		- Recording Analysis mode - high range problem!!
 *		- The high range look-up table for T2S+_V2 does not match (check the calculations) (possibly due to & 0x3FFF in the line: TemperatureLookUpTabel[PixelValue & 0x3FFF])
 *		- Find out how to trigger a shutter calibration for InfiRay P2/P2Pro (pool 2 cameras)
 *		- Find out how to switch the temperature range for InfiRay P2/P2Pro (pool 2 cameras)
 * 
 *    Fixed or added -> 
 * 
 *	  MUST BE CHECKED FIRST!
 *		- Check the snapshot temperature data for P2 cameras
 *		- HIGH range for T2 V2 camera series 
 * 
 *      
 *		 
 */

// Disable the application console
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

// Included libraries
#include "MainGUI.h"
#include "SplashScreen.h"

// Global namespaces
using namespace System;
using namespace System::Windows::Forms;
using namespace System::Diagnostics;
using namespace IRCAMThermalViewer;
using namespace std;
[STAThreadAttribute]

void main() {

	// Enable the visual style rendering of the application
	System::Windows::Forms::Application::EnableVisualStyles();
	// The application uses the global default text rendering 
	System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false);

	// Show the start splash screen
	System::Windows::Forms::Application::Run(gcnew SplashScreen());
	// Start the main GUI application
	System::Windows::Forms::Application::Run(gcnew MainGUI());

}
