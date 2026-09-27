/*
 *  RMH_Winforms_Library.c
 *
 *  Author: Rune Mark Hansen
 *  Date: November 2022
 *
 */

// Included libraries
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <wincodec.h>
#include <msclr/marshal_cppstd.h>
#include "RMH_Winforms_Library.h"
#include "RMH_MathConversions_Library.h"
#include "SplashScreen.h"
#include "MainGUI.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

// Global variables and objects
double MaxExecutionTime = 0.0;
double MinExecutionTime = 1000.0;
unsigned int RichTextBoxNumberOfLines = 0;

// Global namespaces
using namespace System;
using namespace System::Windows::Forms;
using namespace System::Diagnostics;
using namespace IRCAMThermalViewer;
using namespace std;
using namespace Microsoft::Win32;

// ------------------------- WinForms Title Bar Handling Routines -------------------------- //

void RMH_Winforms_EnableTitleBarDarkMode(System::IntPtr FormHandle) {

	// This routine enables the WinForms application title bar dark mode

	// Local variables
	BOOL UseDark = TRUE;
	const int DWMWA_USE_IMMERSIVE_DARK_MODE = 20;

	// Enable the WinForms application title bar dark mode
	DwmSetWindowAttribute(static_cast<HWND>(FormHandle.ToPointer()), DWMWA_USE_IMMERSIVE_DARK_MODE, &UseDark, sizeof(UseDark));

}

// ----------------------------- WinForms Benchmarking Routines ----------------------------- //

System::Diagnostics::Stopwatch^ RMH_Winforms_StartBenchMarkTimer() {

	// This routine starts an internal timer for code benchmarking

	/*
	 *  Example ->
	 *  
	 *   // Start benchmark timer
	 *   System::Diagnostics::Stopwatch^ BenchMarkTimer = RMH_Winforms_StartBenchMarkTimer();
	 * 
	 *   // ----- Benchmark code here ----- //
	 * 
	 *   // Stop the timer and display the benchmark time
	 *   RMH_Winforms_StopBenchmarkTimerAndDisplay(BenchMarkTimer);
	 * 
	 */

	// Local objects and variables
	System::Diagnostics::Stopwatch^ BenchmarkTimer;

	// Start the internal timer for code benchmarking
	BenchmarkTimer = Stopwatch::StartNew();

	// Return the timer object
	return BenchmarkTimer;

}

void RMH_Winforms_StopBenchmarkTimerAndDisplay(System::Diagnostics::Stopwatch^ BenchmarkTimer) {

	// This routine stops the internal benchmark timer, calculates and shows 
	// the code execution time [in seconds], in the output window, since the timer was started

	// Stop the benchmark timer
	BenchmarkTimer->Stop();

	// Calculate the execution benchmark times
	double TimerFrequency = double(BenchmarkTimer->Frequency);
	double ElapsedTicks = double(BenchmarkTimer->ElapsedTicks);
	double ExecutionTime = ElapsedTicks * (1.0 / TimerFrequency);

	// Check the maximum execution time
	if (ExecutionTime > MaxExecutionTime) {
		// Store the maximum execution time
		MaxExecutionTime = ExecutionTime;
	}

	// Check the minimum execution time
	if (ExecutionTime < MinExecutionTime) {
		// Store the minimum execution time
		MinExecutionTime = ExecutionTime;
	}

	// Display the benchmark time in the output window
	cout << "Elapsed Time [Sec]:" << ExecutionTime << endl;
	cout << "Max Execution Time [Sec]:" << MaxExecutionTime << endl;
	cout << "Min Execution Time [Sec]:" << MinExecutionTime << endl;

	// Reset Benchmark Timer 
	BenchmarkTimer->Reset();

}

// ---------------------------- WinForms GUI Handling Routines ---------------------------- //

void RMH_Winforms_StartMainApplicationGUI() {

	// This routine configures the application parameters and starts the WinForms GUI

	// Enable the visual style rendering of the application
	Application::EnableVisualStyles();
	// The application uses the global default text rendering 
	Application::SetCompatibleTextRenderingDefault(false);

	// Show the start splash screen
	Application::Run(gcnew SplashScreen());
	// Start the main GUI application
	Application::Run(gcnew MainGUI());

}

void RMH_Winforms_ChangeFormTitleBarText(System::Windows::Forms::Form^ Winform, std::string Text) {

	// This routine updates the text at the top of the WinForms GUI

	// Update the text at the top of the WinForms GUI
	Winform->Text = RMH_Conversion_StdStringToSystemString(Text);

}

// --------------------- WinForms External Process Handling Routines -------------------- //

void RMH_Winforms_OpenLinkURL(System::String^ LinkURL) {

	// This routine starts a process that opens a given link URL

	// Handling for unsupported Windows versions (e.g. Windows 10 S)
	try {

		// Navigate to the given URL address
		System::Diagnostics::Process::Start(LinkURL);

	}
	catch (System::Exception^ Ex) {}

}

bool RMH_Winforms_OpenPDFWithDefaultViewer(System::String^ PDFFileName) {

	// This routine opens a PDF file, located in the same path as the application .exe, in the system's default PDF viewer.
	// The routine returns false if the file does not exist or no application is associated with PDF files.

	// Construct the full path to the PDF file
	String^ PDFPath = Path::Combine(Application::StartupPath, PDFFileName);

	// Check whether the PDF file exists
	if (File::Exists(PDFPath) == false) { return false; }

	// Handle the error thrown when no application is associated with PDF files
	try {

		// Open the PDF file with the application associated with PDF files
		ProcessStartInfo^ StartInfo = gcnew ProcessStartInfo(PDFPath);
		StartInfo->UseShellExecute = true;
		Process::Start(StartInfo);

	}
	catch (System::Exception^) {

		// Return: the PDF file could not be opened
		return false;

	}

	// Return: the PDF file was opened
	return true;

}

bool RMH_Winforms_OpenWindowsMicrosoftStoreApp(System::String^ PackageFamilyName) {

	// This routine opens a selected external Microsoft Store application in a new process
	// Relevant information -> https://www.auslogics.com/en/articles/how-to-open-microsoft-store-apps-from-command-prompt/

	// Locally defined constants
	bool AppProcessErrorFlag = false;
	System::Diagnostics::Process^ AppProcess = gcnew System::Diagnostics::Process();

	// Handling of errors when opening the app process
	try {

		// Write the execution command to the shell terminal
		AppProcess->StartInfo->FileName = "CMD.exe";
		// Execute the "Open MS Store App" shell terminal string command
		AppProcess->StartInfo->Arguments = "/c explorer.exe shell:AppsFolder\\" + PackageFamilyName + "!App";
		// Do not open the shell terminal window
		AppProcess->StartInfo->WindowStyle = System::Diagnostics::ProcessWindowStyle::Hidden;

		// Execute the command
		AppProcess->Start();

		// Bring the application process window to the front of the screen
		BringWindowToTop(static_cast<HWND>(AppProcess->MainWindowHandle.ToPointer()));

	}
	catch (System::Exception^ Ex) { AppProcessErrorFlag = true; }

	// Return the process status
	return AppProcessErrorFlag;

}

void RMH_Winforms_OpenExternalApplicationEXE(System::String^ ExternalEXENameString) {

	// This routine starts an external process to open an external .exe executable

	// Get the current folder and .exe path of the main application
	String^ ExePath = Process::GetCurrentProcess()->MainModule->FileName;
	String^ Directory = System::IO::Path::GetDirectoryName(ExePath);

	// Format the path of the application .exe file
	String^ targetExePath = System::IO::Path::Combine(Directory, ExternalEXENameString);

	// Error handling
	try {

		// Start the application process
		Process::Start(targetExePath);

	}
	catch (Exception^ ex) {

		// Display status message box
		MessageBox::Show("External Executable Not Found!");

	}

}

// ------------ WinForms Windows Screen Width/Height/Scaling/DPI Reading Routines ------------ //

WINMonitorSettings RMH_Winforms_ReadWindowsScreenSettings() {

	// This routine reads the screen width and height in pixels, as well as the scale and DPI setting values
	// The returned values are for the screen where the application is placed.

	// Local objects and variables
	DEVMODE DevMode;
	MONITORINFOEX MonitorInfoEx;
	WINMonitorSettings CurrentMonitorSettings;
	HWND activeWindow = GetActiveWindow();
	HMONITOR Monitor = MonitorFromWindow(activeWindow, MONITOR_DEFAULTTONEAREST);

	// Reset the number of active monitors
	CurrentMonitorSettings.NumberOfConnectedMonitors = 0;

	// Loop through the number of active screens
	for (unsigned int i = 0; i < System::Windows::Forms::Screen::AllScreens->Length; i++) {

		// Increment the number of active monitors in Windows
		CurrentMonitorSettings.NumberOfConnectedMonitors = CurrentMonitorSettings.NumberOfConnectedMonitors + 1;

	}

	// Read the visual width and height of the current monitor
	MonitorInfoEx.cbSize = sizeof(MonitorInfoEx);
	GetMonitorInfo(Monitor, &MonitorInfoEx);
	CurrentMonitorSettings.MonitorVirtualWidth = (unsigned short)(MonitorInfoEx.rcMonitor.right - MonitorInfoEx.rcMonitor.left);
	CurrentMonitorSettings.MonitorVirtualHeight = (unsigned short)(MonitorInfoEx.rcMonitor.bottom - MonitorInfoEx.rcMonitor.top);

	// Read the physical width and height of the current monitor
	DevMode.dmSize = sizeof(DevMode);
	DevMode.dmDriverExtra = 0;
	EnumDisplaySettings(MonitorInfoEx.szDevice, ENUM_CURRENT_SETTINGS, &DevMode);
	CurrentMonitorSettings.MonitorPhysicalWidth = (unsigned short)DevMode.dmPelsWidth;
	CurrentMonitorSettings.MonitorPhysicalHeight = (unsigned short)DevMode.dmPelsHeight;

	// Calculate the scaling factor of the monitor
	CurrentMonitorSettings.MonitorHorizontalScaleSetting = (unsigned short)(((float)CurrentMonitorSettings.MonitorPhysicalWidth / (float)CurrentMonitorSettings.MonitorVirtualWidth) * 100.0);
	CurrentMonitorSettings.MonitorVerticalScaleSetting = (unsigned short)(((float)CurrentMonitorSettings.MonitorPhysicalHeight / (float)CurrentMonitorSettings.MonitorVirtualHeight) * 100.0);

	// Read the corresponding monitor DPI setting from the scaling setting
	switch (CurrentMonitorSettings.MonitorHorizontalScaleSetting) {

		// Read the DPI setting of the monitor
		case 100: CurrentMonitorSettings.MonitorDPISetting = 96;  break; // DPI 96  -> 100 % scale
		case 125: CurrentMonitorSettings.MonitorDPISetting = 120; break; // DPI 120 -> 125 % scale
		case 150: CurrentMonitorSettings.MonitorDPISetting = 144; break; // DPI 144 -> 150 % scale
		case 175: CurrentMonitorSettings.MonitorDPISetting = 168; break; // DPI 168 -> 175 % scale

	}

	// Return the Windows screen settings parameter structure
	return CurrentMonitorSettings;

}

// -------------------------- WinForms ComboBox Handling Routines ------------------------- //

void RMH_Winforms_CombiBox_AddArrayOfItemStrings(System::Windows::Forms::ComboBox^ CombiBox, std::vector<std::string> StringArray) {

	// This routine adds an array of std::string to the items of the selected ComboBox

	// Local objects
	cli::array<System::Object^>^ ItemObjects = gcnew cli::array<System::Object^ >(StringArray.size());

    // Loop up to and including the size of the input array
	for (int i = 0; i < StringArray.size(); i++) {

		// Add the string names of the objects to the item object array
		ItemObjects[i] = RMH_Conversion_StdStringToSystemString(StringArray[i]);

	}

	// Add the object array to the ComboBox list
	CombiBox->Items->AddRange(ItemObjects);

}

void RMH_Winforms_CombiBox_SetSellectedItemPosition(System::Windows::Forms::ComboBox^ CombiBox, int ItemIndex) {

	// This routine sets the ComboBox to the selected item position
	// An index outside the item list (for example from an old or edited saved session) leaves the selection unchanged

	// Check that the item index is inside the item list
	if (ItemIndex < 0 || ItemIndex >= CombiBox->Items->Count) { return; }

	// Set the ComboBox position to the item index
	CombiBox->SelectedIndex = ItemIndex;

}

// ----------------------- WinForms NumericUpDown Handling Routines ----------------------- //

bool RMH_Winforms_NumericUpDown_ChangeNumber(System::Windows::Forms::NumericUpDown^ NumericUpDown, float InputNumber, float ScaleFactor, float Offset, float DefaultValue) {

	// This routine sets the given "InputNumber" to the numericUpDown control
	// The input arguments "ScaleFactor" and "Offset" are given for optional conversion of "Number" 
	// "DefaultValue" is given as the default value on overflow. (Must be within the maximum/minimum value of the NumericUpDown component)
	// The routine returns "True" if the value was within range and ->
	// "False" if it was out of range and the default value has been used instead.

	// Local variables
	bool ReturnStatus = false;
	float CalculatedValue = InputNumber * ScaleFactor + Offset;
	float CalculatedDefault = DefaultValue * ScaleFactor + Offset;

	// Check whether the calculated value is within the range of a "System::Decimal"
	// and that the value is within the maximum and minimum value of the NumericUpDown.
	if (CalculatedValue >= (float)Decimal::MinValue && CalculatedValue <= (float)Decimal::MaxValue && 
		CalculatedValue >= (float)NumericUpDown->Minimum && CalculatedValue <= (float)NumericUpDown->Maximum) {

		// Convert and display the given number in the NumericUpDown component
		NumericUpDown->Value = System::Convert::ToDecimal(CalculatedValue);

		// Update the status
		ReturnStatus = true;

	}
	else {

		// The value is out of range - write the default value to the NumericUpDown
		NumericUpDown->Value = System::Convert::ToDecimal(CalculatedDefault);

	}

	// Return the status
	return ReturnStatus;

}

// ------------------------ WinForms RichTextBox Handling Routines ------------------------ //

void RMH_Winforms_RichTextBox_WriteLine(System::Windows::Forms::RichTextBox^ RichTextBox, std::string Text, unsigned int MessageType) {

	// This routine writes a given string to a text box (RichTextBox) 

	/*
	 *  Associated macros ->
	 *
	 *  // Status message type macros
	 *  #define _StatusMessageType_Normal      1
	 *  #define _StatusMessageType_Success     2
	 *  #define _StatusMessageType_Warning     3
	 *  #define _StatusMessageType_Error       4
	 * 
	 */


	// Change the text color depending on the message type
	switch (MessageType) {

		// The message is a normal status message
		case _StatusMessageType_Normal:

			// Set the message color
			RichTextBox->SelectionColor = System::Drawing::Color::White;

		break;

		// The message is a success status message
		case _StatusMessageType_Success:

			// Set the message color
			RichTextBox->SelectionColor = System::Drawing::Color::Lime;

		break;

		// The message is a warning status message
		case _StatusMessageType_Warning:

			// Set the message color
			RichTextBox->SelectionColor = System::Drawing::Color::Yellow;

		break;

		// The message is an error status message
		case _StatusMessageType_Error:

			// Set the message color
			RichTextBox->SelectionColor = System::Drawing::Color::Red;

		break;

	}

	// Check whether the text box is full of text
	if (String::IsNullOrEmpty(RichTextBox->Text)) {
		
		// Reset the text box line count variable
		RichTextBoxNumberOfLines = 0; 

	}

	// Increment the shown lines count variable
	RichTextBoxNumberOfLines = RichTextBoxNumberOfLines + 1;

	// If the number of shown lines has reached a maximum
	if (RichTextBoxNumberOfLines >= 35) {

		// Reset the text box line count variable
		RichTextBoxNumberOfLines = 0;

		// Clear the text box
		RichTextBox->Clear();

	}

	// Write the given string to the text box
	RichTextBox->AppendText(RMH_Conversion_StdStringToSystemString(Text));
	// Line feed - new line
	RichTextBox->AppendText("\n");

	// Scroll down to the bottom of the text box
	RichTextBox->ScrollToCaret();

	// Update the text box 
	RichTextBox->Update();

}

// ----------------------- WinForms DataGridView Handling Routines ------------------------ //

void RMH_Winforms_DataGridView_Display2ColumnDataGridView(System::Windows::Forms::DataGridView^ DataGridView, std::vector<std::string> ColumnsHeaderText, System::String^ RowHeaderText, float ColumnHeaderTextSize, float RowHeaderTextSize, float CellTextSize, System::Drawing::Color ColumnRowHeaderTextColor, System::Drawing::Color ColumnRowHeaderBackColor, unsigned int ColumnHeaderHeight, unsigned int RowHeaderWidth, std::vector<std::string> Column1Strings, float *Column2Data) {

	// This routine adds a column to a given DataGridView 

	// Set the text color of the column header
	DataGridView->ColumnHeadersDefaultCellStyle->ForeColor = ColumnRowHeaderTextColor;
	// Set the background color of the column header
	DataGridView->ColumnHeadersDefaultCellStyle->BackColor = ColumnRowHeaderBackColor;
	// Disable visual styles for the column
	DataGridView->EnableHeadersVisualStyles = false;

	// Set the column header border style - no border
	DataGridView->ColumnHeadersBorderStyle = System::Windows::Forms::DataGridViewHeaderBorderStyle::None;

	// Set the column header font size
	DataGridView->ColumnHeadersDefaultCellStyle->Font = gcnew System::Drawing::Font("Arial", ColumnHeaderTextSize, FontStyle::Bold);

	// Set the column header height
	DataGridView->ColumnHeadersHeight = ColumnHeaderHeight;

	// Disable resizing of the header
	DataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::DisableResizing;

	// Add the number of columns
	for (unsigned char i = 0; i < ColumnsHeaderText.size(); i++) {

		// Add the column to the data grid view
		DataGridView->Columns->Add(i.ToString(), RMH_Conversion_StdStringToSystemString(ColumnsHeaderText[i]));

		// Set the text color of the column
		DataGridView->Columns[i.ToString()]->DefaultCellStyle->ForeColor = ColumnRowHeaderTextColor;
		// Set the background color of the column
		DataGridView->Columns[i.ToString()]->DefaultCellStyle->BackColor = ColumnRowHeaderBackColor;

		// The column must fill the whole data grid view
		DataGridView->Columns[i.ToString()]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;

		// Disable the column sorting feature
		DataGridView->Columns[i.ToString()]->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;

	}

	// ---------------------------------- Add Row Data To The Created Column ---------------------------------- // 
	
	// Set the text color of the row header
	DataGridView->RowHeadersDefaultCellStyle->ForeColor = ColumnRowHeaderTextColor;
	// Set the background color of the row header
	DataGridView->RowHeadersDefaultCellStyle->BackColor = ColumnRowHeaderBackColor;
	// Disable visual styles for the column
	DataGridView->EnableHeadersVisualStyles = false;

	// Set the border style of the row header - no border
	DataGridView->RowHeadersBorderStyle = System::Windows::Forms::DataGridViewHeaderBorderStyle::None;
	// Set the border style of the row cells - no border
	DataGridView->CellBorderStyle = System::Windows::Forms::DataGridViewCellBorderStyle::None;

	// Set the font size of the row header
	DataGridView->RowHeadersDefaultCellStyle->Font = gcnew System::Drawing::Font("Arial", RowHeaderTextSize, FontStyle::Bold);
	// Set the font size of the row cells
	DataGridView->DefaultCellStyle->Font = gcnew System::Drawing::Font("Arial", CellTextSize, FontStyle::Bold);

	// Set the width of the row header
	DataGridView->RowHeadersWidth = RowHeaderWidth;

	// Loop through the whole given string array
	for (unsigned int i = 0; i < Column1Strings.size(); i++) {

		// Add the row to the data grid view
		DataGridView->Rows->Add(RMH_Conversion_StdStringToSystemString(Column1Strings[i]), RMH_Conversion_FloatToSystemString(Column2Data[i]));

		// Add the row header text
		DataGridView->Rows[i]->HeaderCell->Value = String::Format(RowHeaderText + " {0}", i + 1);

		// Set the tag of the row
		DataGridView->Rows[i]->Tag = i;

	}

	// ----------------------------------------------------------------------------------------------------------- // 

}

// --------------------------- WinForms Image Display Routines --------------------------- //

void RMH_Winforms_PictureBox_UpdateImageBitmap(System::Windows::Forms::PictureBox^ PictureBox, System::Drawing::Bitmap^ Bitmap, System::Drawing::Imaging::ColorPalette^ ColorPalette) {

	// This routine shows a given bitmap object in a selected "PictureBox" control handler

	// Set the bitmap color palette
	Bitmap->Palette = ColorPalette;

	// Delete the unmanaged memory of the PictureBox
	//delete PictureBox->Image;
	// Update the PictureBox image frame data 
	PictureBox->Image = Bitmap;

}

// --------------------- WinForms Menu And Sub-Menu Handling Routines --------------------- //

void RMH_Winforms_HideSubMenuPanel(System::Windows::Forms::Panel^ SubMenuPanel) {

	// This routine closes the selected sub-menu panel

	// Check whether the sub-menu is visible
	if (SubMenuPanel->Visible == true) {
		// Make the sub-menu invisible
		SubMenuPanel->Visible = false;
	}

}

void RMH_Winforms_ToggleSubMenuPanel(System::Windows::Forms::Panel^ SubMenuPanel, System::Windows::Forms::Button^ MenuButton) {

	// This routine toggles the selected sub-menu panel and updates the "expanded" character (+/-) of the menu button

	// Check whether the sub-menu is invisible
	if (SubMenuPanel->Visible == false) {

		// Close the selected sub-menu panel
		RMH_Winforms_HideSubMenuPanel(SubMenuPanel);

		// Make the sub-menu visible
		SubMenuPanel->Visible = true;

		// Update the "expanded" character of the menu button
		MenuButton->Text = MenuButton->Text->Replace('+', '-');

		// Refresh Sub-Menu Panel
		SubMenuPanel->Refresh();

	}
	else {

		// Make the sub-menu invisible
		SubMenuPanel->Visible = false;

		// Update the "expanded" character of the menu button
		MenuButton->Text = MenuButton->Text->Replace('-', '+');

	}

}

// ---------------- WinForms Form Docking & Undocking Handling Routines ---------------- //

void RMH_Winforms_CloseForm(System::Windows::Forms::Form^ FormObject, bool *FormOpenedFlag, bool *FormDockedFlag, bool *FormUndockedFlag) {

	// This routine closes a given form

	// Close the form object
	FormObject->Close();

	// Update the status flag of the form
	*FormOpenedFlag = false;
	*FormDockedFlag = false;
	*FormUndockedFlag = false;

}

void RMH_Winforms_OpenFormInSeperateWindow(System::Windows::Forms::Form^ FormObject, bool *FormOpenedFlag, bool *FormDockedFlag, bool *FormUndockedFlag) {

	// This routine opens a given form in a separate window

	// Configure the border style of the form 
	FormObject->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Sizable;
	// Configure the start position of the form when opening/undocking
	FormObject->StartPosition = FormStartPosition::CenterScreen;

	// Configure the form as a "top level" form
	FormObject->TopLevel = true;

	// Show the form
	FormObject->Show();

	// Update the status flag of the form
	*FormOpenedFlag = true;
	*FormDockedFlag = false;
	*FormUndockedFlag = true;

}

void RMH_Winforms_OpenAndDockFormInParentPanel(System::Windows::Forms::Form^ FormObject, System::Windows::Forms::Panel^ ParentPanel, bool *FormOpenedFlag, bool *FormDockedFlag, bool *FormUndockedFlag) {

	// This routine opens and "docks" a given form in a given "parent" panel

	/*
	*  Associated form "FormClosing" override function ->
	* 
	*	private: System::Void ThermalCameraGUI_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
	*
	*		// On closing, the form is hidden instead
	*		this->Hide();
	*		// Disable "Dispose" of the form
	*		e->Cancel = true;
	*
	*		// Update the form "is open" flag
	*       FormOpenedFlag = false;
	*		FormDockedFlag = false;
	*		isFormUndocked = false;
	*
	*	}
	*  
	*/

	// Make sure the form object is not maximized before it is docked
	if (FormObject->WindowState == FormWindowState::Maximized) {
		FormObject->WindowState = FormWindowState::Normal;
	}

	// Set the form as a top-level form
	FormObject->TopLevel = false;
	FormObject->Parent = ParentPanel;

	// Set the "parent" of the form to the given "parent" panel
	FormObject->Parent = ParentPanel;

	// Configure the border style of the form 
	FormObject->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
	// The form must fill the whole "parent" panel
	FormObject->Size = ParentPanel->ClientSize;
	FormObject->Dock = DockStyle::Fill;

	// Show the form in the "parent" panel
	FormObject->Show();
	ParentPanel->PerformLayout();
	FormObject->PerformLayout();

	// Update the status flag of the form
	*FormOpenedFlag = true;
	*FormDockedFlag = true;
	*FormUndockedFlag = false;

}

void RMH_Winforms_UndockFormFromParentPanel(System::Windows::Forms::Form^ FormObject, System::Windows::Forms::Panel^ ParentPanel, bool *FormOpenedFlag, bool *FormDockedFlag, bool *FormUndockedFlag, System::Windows::Forms::FormBorderStyle FormBorderStyle) {

	// This routine opens a given form. If the form is "docked" in a "parent" panel, the form is "undocked" from the panel and opened in a separate window.

	// Reset the docking style of the form
	FormObject->Dock = DockStyle::None;
	// Reset the parent of the form 
	FormObject->Parent = nullptr;

	// Remove the form as a "control" from the given "parent" panel
	ParentPanel->Controls->Clear();

	// Configure the border style of the form
	FormObject->FormBorderStyle = FormBorderStyle;
	// Configure the start position of the form when opening/undocking
	FormObject->StartPosition = FormStartPosition::CenterScreen;

	// Configure the form as a "top most" form
	FormObject->TopMost = true;
	// Configure the form as a "top level" form
	FormObject->TopLevel = true;

	// Show the form
	FormObject->Show();

	// Update the status flag of the form
	*FormOpenedFlag = true;
	*FormDockedFlag = false;
	*FormUndockedFlag = true;

}

// ------------ WinForms Display Child Form In Parent Panel Handling Routines ------------- //

bool RMH_Winforms_ToggleChildFormInParentPanel(System::Windows::Forms::Form^ ChildForm, cli::interior_ptr<System::Windows::Forms::Form^> CurrentActiveForm, System::Windows::Forms::Panel^ ParentPanel) {

	// This routine opens the selected child form in a given parent form panel
	// The returned status indicates whether the given form is open or closed

	// If an active view form is active in the parent panel
	if ((*CurrentActiveForm) != nullptr) {

		// Close the currently active form
		(*CurrentActiveForm)->Close();
		// Reset the currently active form to NULL
		(*CurrentActiveForm) = nullptr;

		// Return the form status
		return false;

	}
	else {

		// Update the active form to the child form
		(*CurrentActiveForm) = ChildForm;

		// The new child form is not a top-level form
		ChildForm->TopLevel = false;
		// Set the child form without a "border"
		ChildForm->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
		// The shown child form must fill the whole parent panel
		ChildForm->Dock = System::Windows::Forms::DockStyle::Fill;
		// Set the size of the child form to the parent panel size - avoids flicker
		ChildForm->Size.Width = ParentPanel->Size.Width;
		ChildForm->Size.Height = ParentPanel->Size.Height;

		// Add the selected child form to the parent panel, as a control component
		ParentPanel->Controls->Add(ChildForm);
		// Set the tag of the parent panel to the child form
		ParentPanel->Tag = ChildForm;

		// Bring the child form to the front of the parent panel
		ChildForm->BringToFront();
		// Display and show the child form in the parent panel
		ChildForm->Show();

		// Return the form status
		return true;

	}

}

bool RMH_Winforms_AddChildAsControlToParentPanel(System::Windows::Forms::Form^ ChildForm, System::Windows::Forms::Panel^ ParentPanel) {

	// This routine adds the selected form as a "control" in the given parent form panel

	// The child form is not a top-level form
	ChildForm->TopLevel = false;
	// Set the child form without a "border"
	ChildForm->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
	// The shown child form must fill the whole parent panel
	ChildForm->Dock = System::Windows::Forms::DockStyle::Fill;
	// Set the size of the child form to the panel size - avoids flicker
	ChildForm->Size.Width = ParentPanel->Size.Width;
	ChildForm->Size.Height = ParentPanel->Size.Height;

	// Add the selected child form to the panel, as a control component
	ParentPanel->Controls->Add(ChildForm);

	// Set the tag of the parent panel to the child form
	ParentPanel->Tag = ChildForm;

	// Bring the child form to the back of the parent panel
	ChildForm->SendToBack();

	// Display and show the added child form
	ChildForm->Show();

	// Return the status
	return true;

}

bool RMH_Winforms_BringChildFormTOFront(System::Windows::Forms::Form^ ChildForm) {

	// This routine sends a child form (shown in a parent form panel) 
	// to the front position in a parent panel.

	// Send the child form to the front of the parent panel
	ChildForm->BringToFront();

	// Return the status
	return true;

}

bool RMH_Winforms_SendChildFormToBack(System::Windows::Forms::Form^ ChildForm) {

	// This routine sends a child form (shown in a parent form panel) 
	// to the back position in the parent panel.

	// Send the child form to the back position in the parent panel
	ChildForm->SendToBack();

	// Return the status
	return true;

}

// ------------------------ WinForms Color Dialog Select Color Routines ------------------------ //

System::Drawing::Color^ RMH_Winforms_ShowAndReadColorDialog(bool *DialogAbortedFlag) {

	// This routine opens a color dialog and returns the selected color
	// If the dialog has been closed without a color being selected, "DialogAbortedFlag" is set to true.

	// Read the temporary array data and sort the kernel array
	System::Drawing::Color^ SelectedColor;
	ColorDialog^ MyDialog = gcnew ColorDialog;

	// Allow selection of a custom color.
	MyDialog->AllowFullOpen = true;
	// Allow help functionality
	MyDialog->ShowHelp = true;

	// Update the dialog abort flag
	*DialogAbortedFlag = false;

	// Update the text box color if the user clicks OK 
	if (MyDialog->ShowDialog() == ::System::Windows::Forms::DialogResult::OK) {

		// Read the selected color from the dialog
		SelectedColor = MyDialog->Color;

		// Update the dialog abort flag
		*DialogAbortedFlag = false;

	}
	else {

		// Return white on dialog abort
		SelectedColor = System::Drawing::Color::FromArgb(255, 255, 255, 255);

		// Update the dialog abort flag
		*DialogAbortedFlag = true;

	}

	// Return the selected color from the dialog
	return SelectedColor;

}

// ------------------------ WinForms CSV Write/Read Routines ------------------------ //

void RMH_Winforms_WriteHeaderStringsToCSVFile(std::string FilePath, std::string FileName, std::vector<std::string> HeaderStrings, unsigned int NmbOfHeaderStrings, System::String^ DataDelimiter) {

	// This routine writes header description strings to the CSV file path

	// Local objects and variables
	std::ofstream File;
	std::string CSVFilePath = FilePath + "/" + FileName;
	std::string CombinedHeaderString;

	// Open the csv file - append to the file (creates the file if there is no file at the path)
	File.open(CSVFilePath, std::ios_base::app);

	// Loop up to and including the number of header strings
	for (unsigned int i = 0; i < NmbOfHeaderStrings; i++) {

		// Do not write a comma after the last string
		if (i < NmbOfHeaderStrings - 1) {

			// Format the total write string - with comma
			CombinedHeaderString = CombinedHeaderString + HeaderStrings[i] + RMH_Conversion_SystemStringToStdString(DataDelimiter);

		}
		else {

			// Format the total write string - without comma
			CombinedHeaderString = CombinedHeaderString + HeaderStrings[i];

		}

	}

	// Append the string to the CSV file 
	File << CombinedHeaderString << std::endl;

	// Close the opened CSV file
	File.close();

}

void RMH_Winforms_WriteDataArrayToCSVFile(std::string FilePath, std::string FileName, std::string RowIDString, std::string RowHeaderString, double *CSVData, unsigned int NmbOfValues, System::String^ DataDelimiter) {

	// This routine writes an array of data to a CSV file

	// Local objects and variables
	std::ofstream File;
	std::string CSVFilePath = FilePath + "/" + FileName;
	std::string DataStrings[10];
	std::string CombinedCSVWriteString;

	// Open the csv file - append to the file (creates the file if there is no file at the path)
	File.open(CSVFilePath, std::ios_base::app);

	// Check whether the file is already open
	if (File.is_open() == true) {

		// Close the file before it can be opened
		File.close();

	}

	// Open the csv file - append to the file (creates the file if there is no file at the path)
	File.open(CSVFilePath, std::ios_base::app);

	// Loop up to and including the number of array data points
	for (unsigned int i = 0; i < NmbOfValues; i++) {

		// Format the data to a string for CSV writing
		DataStrings[i] = RMH_Conversion_SystemStringToStdString(CSVData[i].ToString("F5"));

		// Do not write a comma after the last string
		if (i < NmbOfValues - 1) {

			// Format the total write string - with comma
			CombinedCSVWriteString = CombinedCSVWriteString + DataStrings[i] + RMH_Conversion_SystemStringToStdString(DataDelimiter);

		}
		else {

			// Format the total write string - without comma 
			CombinedCSVWriteString = CombinedCSVWriteString + DataStrings[i];

		}

	}

	// Append the string to the CSV file 
	File << RowIDString + RMH_Conversion_SystemStringToStdString(DataDelimiter) + RowHeaderString + RMH_Conversion_SystemStringToStdString(DataDelimiter) + CombinedCSVWriteString << std::endl;

	// Close the opened CSV file
	File.close();

}

void RMH_Winforms_WriteDataArrayMatrixToCSVFile(std::string FilePath, std::string FileName, double* CSVData, unsigned int ArrayMatrixWidth, unsigned int ArrayMatrixHeight, System::String^ DataDelimiter) {

	// This routine writes an array of data to a CSV file

	// Local objects and variables
	std::ofstream File;
	std::string CSVFilePath = FilePath + "/" + FileName;
	std::string DataStrings[10];
	std::string CombinedCSVWriteString;
	unsigned int MatrixArrayIndex = 0;
	System::String^ DateHeaderString = System::DateTime::Now.ToString("HH:mm:ss.fff dd-MM-yyyy");

	// Open the csv file - append to the file (creates the file if there is no file at the path)
	File.open(CSVFilePath, std::ios_base::app);

	// Check whether the file is already open
	if (File.is_open() == true) {

		// Close the file before it can be opened
		File.close();

	}

	// Open the csv file - append to the file (creates the file if there is no file at the path)
	File.open(CSVFilePath, std::ios_base::app);

	// Write the file date header string 
	File << "Time And Data For Captured Data: " + RMH_Conversion_SystemStringToStdString(DateHeaderString);

	// New line
	File << std::endl;
	File << std::endl;

	// Loop through the rows of the array matrix
	for (unsigned int Y = 0; Y < ArrayMatrixHeight; Y++) {

		// Loop through the columns of the array matrix
		for (unsigned int X = 0; X < ArrayMatrixWidth; X++) {

			// Convert the matrix index to an array index
			MatrixArrayIndex = (Y * ArrayMatrixWidth) + X;

			// Do not write a comma after the last string
			if (X < ArrayMatrixWidth - 1) {

				// Format the total write string - with comma
				File << RMH_Conversion_SystemStringToStdString(CSVData[MatrixArrayIndex].ToString("F5")) + RMH_Conversion_SystemStringToStdString(DataDelimiter);

			}
			else {

				// Format the total write string - without comma 
				File << RMH_Conversion_SystemStringToStdString(CSVData[MatrixArrayIndex].ToString("F5"));

			}

		}

		// New line
		File << std::endl;

	}

	// Close the opened CSV file
	File.close();

}

void RMH_Winforms_GenerateAndWriteCSVFile(std::string FilePath, std::string FileName, std::vector<std::string> AppendString) {

	// This routine writes a string array to a CSV file with the given file path

	// Local objects and variables
	std::ofstream File;
	std::string RemovefilePath = FilePath + "/" + FileName;

	// Delete the existing CSV file at the path
	std::remove(RemovefilePath.c_str());

	// Check whether the file is already open
	if (File.is_open() == true) {

		// Close the file before it can be opened
		File.close();

	}

	// Open the csv file - append to the file (creates the file if there is no file at the path)
	File.open(RemovefilePath, std::ios_base::app);

	// Write the string vector data to the CSV file
	for (unsigned int i = 0; i < AppendString.size(); i++) {

		// Append the string to the CSV file 
		File << AppendString[i] << std::endl;

	}

	// Close the opened CSV file
	File.close();

}

RMHWinformsLib::FileReadFormat RMH_Winforms_ReadLinesFromCSVFile(std::string FilePath, std::string FileName) {

	// This routine reads and returns a std::string array from the given input file path
	// which contains all lines read from the selected file

	// Local variables and objects
	std::ifstream File;
	unsigned long i = 0;
	std::string ReadStringLine;
	unsigned long FileNumbOfLines = 0;
	std::string FullPath = FilePath + "/" + FileName;
	RMHWinformsLib::FileReadFormat ReadFile;

	// Open the selected input file path
	File.open(FullPath);

	// Check whether the file was opened
	// If the path location of the file is wrong - error
	if (!File) {
		
		// Reset the "file was read correctly" flag
		// Due to a file read error
		ReadFile.FileReadSuccess = false;
		// Update the "zero length" status flag
		ReadFile.FileZeroLengthFlag = true;
		// Reset the number of lines read from the file
		ReadFile.FileLineLength = 0;

	}
	else {

		// Reset the number of lines read 
		FileNumbOfLines = 0;

		// Loop through all lines of the file
		while (File.good()) {

			// Read the string lines of the file
			std::getline(File, ReadStringLine, '\n');

			// Increment the number of lines read 
			FileNumbOfLines++;

		}

		// Reset the file pointers
		File.clear();
		File.seekg(0);

		// Check that the file is not empty
		if (FileNumbOfLines == 0) {

			// Update the "zero length" status flag
			ReadFile.FileZeroLengthFlag = true;

		}
		else {

			// Load the file strings into the local vector string
			for (i = 0; i < FileNumbOfLines - 1; i++) {

				// Read the string lines of the file
				std::getline(File, ReadStringLine, '\n');

				// Write the string read to the format vector array 
				ReadFile.FileStrings[i] = ReadStringLine;

			}

			// Reset the "zero length" status flag
			ReadFile.FileZeroLengthFlag = false;
			// Update the "file was read correctly" flag
			ReadFile.FileReadSuccess = true;
			// Store the number of lines read from the file
			ReadFile.FileLineLength = FileNumbOfLines - 1;

		}

	}

	// Close the opened CSV file
	File.close();

	// Return the file data and status flag
	return ReadFile;

}

// -------------------- WinForms File Open/Save Dialog Handling Routines -------------------- //

System::String^ RMH_Winforms_GetSaveFileDialogDirectory() {

	// This routine opens a file explorer, which is used to set a path for where a file should be saved.
	// The routine returns the path location string

	// Local objects and variables
	System::String^ FilePathNameString;
	System::String^ FilePathString = "None";
	System::Windows::Forms::SaveFileDialog^ SaveFilePathDialog = gcnew System::Windows::Forms::SaveFileDialog;

	// Configure the file type filters
	SaveFilePathDialog->Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
	// Look for all available file types
	SaveFilePathDialog->FilterIndex = 2;
	// Restore the previously selected directory
	SaveFilePathDialog->RestoreDirectory = true;
	// Set the default dummy file name
	SaveFilePathDialog->FileName = "DefaultSavePath";

	// Open the "Save File Dialog" and wait for a valid selected path
	if (SaveFilePathDialog->ShowDialog() == ::DialogResult::OK) {

		// Read the selected file name path 
		FilePathNameString = SaveFilePathDialog->FileName;

		// Handling if a file location was not selected
		try {

			// Read the selected file directory path string without the file name
			FilePathString = System::IO::Path::GetDirectoryName(FilePathNameString);

		}
		catch (System::Exception^ Ex) {

			// The dialog explorer was closed and no path was selected
			FilePathString = "None";

		}

	}

	// Perform garbage collection
	GC::Collect();

	// Return the selected file path string
	return FilePathString;

}

System::String^ RMH_Winforms_GetOpenFileDialogDirectory() {

	// This routine opens a file explorer, which is used to read a path from where a file should be opened.
	// The routine returns the path location string

	// Local objects and variables
	System::String^ FilePathNameString;
	System::String^ FilePathString = "None";
	System::Windows::Forms::OpenFileDialog^ OpenFilePathDialog = gcnew System::Windows::Forms::OpenFileDialog;

	// Configure the file type filters
	OpenFilePathDialog->Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
	// Look for all available file types
	OpenFilePathDialog->FilterIndex = 2;
	// Restore the previously selected directory
	OpenFilePathDialog->RestoreDirectory = true;
	// Set the default dummy file name
	OpenFilePathDialog->FileName = "DefaultOpenPath";

	// Open the dialog
	DialogResult DResult = OpenFilePathDialog->ShowDialog();

	// Wait for a valid selected path
	if (DResult == DialogResult::OK) {

		// Read the selected file name path 
		FilePathNameString = OpenFilePathDialog->FileName;

	}
	else if (DResult == DialogResult::Cancel) {

		// The dialog explorer was closed and no path was selected
		FilePathNameString = "None";

	}

	// Perform garbage collection
	GC::Collect();

	// Return the selected file path string
	return FilePathNameString;

}

// --------------------------- WinForms Chart Handling Routines --------------------------- //

void RMH_Winforms_Charts_ChangeXAxesLimits(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartArea1Index, double ChartXAxesMinimum, double ChartXAxesMaximum) {

	// This routine sets the X-axis limits of the WinForms chart

	// Set the maximum and minimum X-axis limits of the chart
	Chart->ChartAreas[ChartArea1Index]->Axes[0]->Maximum = ChartXAxesMaximum;
	Chart->ChartAreas[ChartArea1Index]->Axes[0]->Minimum = ChartXAxesMinimum;

}

void RMH_Winforms_Charts_ChangeXAxesTickInterval(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartArea1Index, double AxesInterval) {

	// This routine sets the X-axis tick interval of the WinForms chart

	// Set the interval of the X axis of the chart
	Chart->ChartAreas[ChartArea1Index]->Axes[0]->Interval = AxesInterval;

}

void RMH_Winforms_Charts_ChangeYAxesLimits(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartArea1Index, double ChartYAxesMinimum, double ChartYAxesMaximum) {

	// This routine sets the Y-axis limits of the WinForms chart

	// Set the maximum and minimum Y-axis limits of the chart
	Chart->ChartAreas[ChartArea1Index]->Axes[1]->Maximum = ChartYAxesMaximum;
	Chart->ChartAreas[ChartArea1Index]->Axes[1]->Minimum = ChartYAxesMinimum;

}

void RMH_Winforms_Charts_ChangeYAxesTickInterval(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartArea1Index, double AxesInterval) {

	// This routine sets the Y-axis tick interval of the WinForms chart

	// Set the interval of the X axis of the chart
	Chart->ChartAreas[ChartArea1Index]->Axes[1]->Interval = AxesInterval;

}

void RMH_Winforms_Charts_AddDataArrayToChartSeries(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartSeriesIndex, double* SeriesXDataArray, double* SeriesYDataArray, unsigned int SeriesDataArrayLength) {

	// This routine writes an array of data to the selected chart data series

	// Clear the data points of the chart
	Chart->Series[ChartSeriesIndex]->Points->Clear();

	// Loop up to and including the length of the given data array
	for (unsigned int i = 0; i < SeriesDataArrayLength; i++) {

		// Add the given data array to the chart series data
		Chart->Series[ChartSeriesIndex]->Points->AddXY(SeriesXDataArray[i], SeriesYDataArray[i]);

	}

}

void RMH_Winforms_Charts_AddDataPointToChartSeries(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartSeriesIndex, double PointXData, double PointYData) {

	// This routine writes a given data point to the selected chart data series

	// Add the given data point to the chart series data
	Chart->Series[ChartSeriesIndex]->Points->AddXY(PointXData, PointYData);

}

void RMH_Winforms_Charts_ClearChartDataPoints(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartSeriesIndex) {

	// This routine resets and clears the data points of the selected chart index

	// Clear the data points of the chart
	Chart->Series[ChartSeriesIndex]->Points->Clear();

}

// -------------------------- WinForms Image And Snapshot Routines -------------------------- //

bool RMH_Winforms_SavePanelSnapShotPNG(System::Windows::Forms::Panel^ SrcPanel, System::String^ SnapShotPath) {

	// This routine saves a PNG snapshot from a given input graphics panel.
	// input file path example: C:\Users\User\Desktop
	// The routine returns "true" if the snapshot was saved correctly - otherwise "false"
	// The saved file name is formatted as: Snapshot_HHmmssddMMyyyy

	// Read the temporary array data and sort the kernel array
	bool ReturnStatus = false;
	float PanelUpperLeftSourceX = 0;
	float PanelUpperLeftSourceY = 0;
	float WinScaleSettingNormalized = 0;
	WINMonitorSettings WindowsScreenSettings;
	System::Drawing::Size CompensatedScreenSize = SrcPanel->Size;

	// Format the data identification string of the file (Snapshot_HHmmssddMMyyyy)
	System::String^ FileName = System::DateTime::Now.ToString("HHmmssfffddMMyyyy");

	// Read the Windows screen parameters
	WindowsScreenSettings = RMH_Winforms_ReadWindowsScreenSettings();

	// Normalize the Windows scaling setting 
	WinScaleSettingNormalized = (float)WindowsScreenSettings.MonitorHorizontalScaleSetting / 100.0;

	// Calculate the top-left X & Y pixel coordinates of the panel - compensate for the Windows scaling setting
	PanelUpperLeftSourceX = (float)SrcPanel->PointToScreen(System::Drawing::Point(0, 0)).X * (WinScaleSettingNormalized - 1.0);
	PanelUpperLeftSourceY = (float)SrcPanel->PointToScreen(System::Drawing::Point(0, 0)).Y * (WinScaleSettingNormalized - 1.0);

	// Calculate the snapshot-compensated height and width from the screen scaling factor read
	CompensatedScreenSize.Width = (float)SrcPanel->Size.Width * WinScaleSettingNormalized;
	CompensatedScreenSize.Height = (float)SrcPanel->Size.Height * WinScaleSettingNormalized;

	// Generate the reference bitmap for the snapshot graphics data
	System::Drawing::Bitmap^ SnapShotBitMap = gcnew System::Drawing::Bitmap(CompensatedScreenSize.Width, CompensatedScreenSize.Height);

	// Create a graphics reference object for storing the reference bitmap data
	System::Drawing::Graphics^ PanelGraphics = System::Drawing::Graphics::FromImage(SnapShotBitMap);

	// Copy the graphics data of the panel to the graphics reference object (within the panel bounds)
	PanelGraphics->CopyFromScreen(SrcPanel->PointToScreen(System::Drawing::Point(RMH_Math_Round(PanelUpperLeftSourceX), RMH_Math_Round(PanelUpperLeftSourceY))), System::Drawing::Point(0, 0), CompensatedScreenSize);

	// Handling of path string errors
	try {

		// Save the snapshot image at the selected file location
		SnapShotBitMap->Save(SnapShotPath + "/SnapShot_" + FileName + ".png", System::Drawing::Imaging::ImageFormat::Png);

		// Update the returned status
		ReturnStatus = true;

	}
	catch (System::Exception^ Ex) {

		// Update the returned status
		ReturnStatus = false;

	}

	// Perform garbage collection
	GC::Collect();

	// Return the status
	return ReturnStatus;

}

bool RMH_Winforms_SaveRawImageDataAsSnapShotPNG(System::String^ SnapShotPath, unsigned int ImageDataWidth, unsigned int ImageDataHeight, unsigned char *ImageData) {

	// This routine saves the raw input image data as a PNG snapshot.
	// input file path example: C:\Users\User\Desktop
	// The routine returns "true" if the snapshot was saved correctly - otherwise "false"
	// The saved file name is formatted as: Snapshot_HHmmssddMMyyyy

	// Read the temporary array data and sort the kernel array
	bool ReturnStatus = false;
	unsigned char* BGRImageData = new unsigned char[ImageDataWidth * ImageDataHeight * 3];

	// Format the data identification string of the file (SnapshotRAW_HHmmssddMMyyyy)
	System::String^ FileName = System::DateTime::Now.ToString("HHmmssfffddMMyyyy");

	// Convert the array data from RGB to BGR format
	for (unsigned int Y = 0; Y < ImageDataHeight; Y++) {

		// Loop through all rows of the array matrix
		for (unsigned int X = 0; X < ImageDataWidth; X++) {

			// Read the source and destination indices
			unsigned int SrcIndex = (Y * ImageDataWidth + X) * 3;
			unsigned int DstIndex = (Y * ImageDataWidth + X) * 3;

			// Rearrange the RGB data to BGR format
			BGRImageData[DstIndex + 2] = ImageData[SrcIndex + 0];
			BGRImageData[DstIndex + 1] = ImageData[SrcIndex + 1];
			BGRImageData[DstIndex + 0] = ImageData[SrcIndex + 2];

		}
	}

	// Create a bitmap from the given image data for the snapshot graphics data
	System::Drawing::Bitmap^ SnapShotBitMap = gcnew System::Drawing::Bitmap(
		ImageDataWidth, ImageDataHeight, 3 * ImageDataWidth, 
		System::Drawing::Imaging::PixelFormat::Format24bppRgb,
		System::IntPtr(&BGRImageData[0]));

	// Handling of path string errors
	try {

		// Save the snapshot image at the selected file location
		SnapShotBitMap->Save(SnapShotPath + "/SnapShotRAW_" + FileName + ".png", System::Drawing::Imaging::ImageFormat::Png);

		// Update the returned status
		ReturnStatus = true;

	}
	catch (System::Exception^ Ex) {

		// Update the returned status
		ReturnStatus = false;

	}

	// Perform garbage collection
	GC::Collect();

	// Free the allocated memory of the buffer array  
	delete[] BGRImageData;

	// Return the status
	return ReturnStatus;

}

// ------------------------ WinForms Web Browser Handling Routines ------------------------ //

bool RMH_Winforms_IsAdobeReaderInstalled() {

	// This routine checks whether "Adobe Reader" is installed on the user's computer

	// String array of possible registry key paths
	cli::array<String^>^ PossibleRegistryKeys = {
		"SOFTWARE\\Adobe\\Acrobat Reader",
		"SOFTWARE\\WOW6432Node\\Adobe\\Acrobat Reader",
		"SOFTWARE\\Adobe\\Adobe Acrobat",
		"SOFTWARE\\WOW6432Node\\Adobe\\Adobe Acrobat"
	};

	// Check each key in the registry
	for each (String ^ KeyPath in PossibleRegistryKeys) {

		// Handle exception
		try {

			// Open the registry key
			RegistryKey^ Key = Registry::LocalMachine->OpenSubKey(KeyPath);

			// If the registry key is not a null pointer
			if (Key != nullptr) {

				// Read the registry key name
				cli::array<String^>^ SubKeyNames = Key->GetSubKeyNames();

				// If the length of the subkey name string is above 0
				if (SubKeyNames->Length > 0) {

					// Try to find the Adobe Reader executable (.exe)
					for each (String ^ Version in SubKeyNames) {

						// Read the version of the registry key
						RegistryKey^ VersionKey = Key->OpenSubKey(Version + "\\InstallPath");

						// If the registry key version is not a null pointer
						if (VersionKey != nullptr) {

							// Read the executable path
							String^ InstallPath = static_cast<String^>(VersionKey->GetValue(""));

							// Check that the executable path is actually a string
							if (!String::IsNullOrEmpty(InstallPath)) {

								// Construct the full path to the executable
								String^ ReaderExePath = Path::Combine(InstallPath, "Acrobat.exe");

								// Check whether the Adobe Reader executable exists
								if (File::Exists(ReaderExePath)) {

									// Close the registry version key
									VersionKey->Close();
									// Close the registry key
									Key->Close();

									// The Adobe Reader executable was found (Adobe Reader is installed on the computer)
									return true; 

								}

							}

							// Close the registry version key
							VersionKey->Close();

						}

					}

				}

				// Close the registry key
				Key->Close();

			}

		}
		catch (Exception^ ex) { }

	}

	// The Adobe Reader executable was NOT found (Adobe Reader is NOT installed on the computer)
	return false; 

}

void RMH_Winforms_OpenPDFInWebbrowser(System::Windows::Forms::WebBrowser^ WebBrowserControl, System::String^ PDFFileName) {

	// This routine opens a PDF file in the given web browser component.
	// The PDF file must be located in the same path as the application .exe

	// Check whether Adobe Reader is installed on the computer
	if (RMH_Winforms_IsAdobeReaderInstalled() == true) {

		// Get the path to the application .exe file
		String^ exePath = Application::StartupPath;

		// Construct the full path to the PDF file
		String^ pdfPath = Path::Combine(exePath, PDFFileName);

		// Check whether the PDF file exists before it is loaded
		if (File::Exists(pdfPath)) {

			// Convert the file path to URI format
			String^ pdfUri = "file:///" + pdfPath->Replace("\\", "/");

			// Load the PDF file into the WebBrowser component
			WebBrowserControl->Navigate(pdfUri);

		}
		else {

			// Construct custom HTML for an error message
			String^ errorHtml = R"(
                <html>
                <head>
                    <style>
                        body { background-color: #232323; color: #ffffff; font-family: Arial, sans-serif; text-align: center; padding: 50px; }
                        h1 { color: #ff0000; }
                        p { font-size: 16px; }
                    </style>
                </head>
                <body>
                    <h1> PDF Manual File Error: The Software Manual PDF File Was Not Found!</h1>
                    <p>The PDF File Was Not Found On Path: <strong>)" + pdfPath + R"(</strong></p>
                    <p>Please Check If The File Is In The Displayed Path Or Contact Software Admin.</p>
                </body>
                </html>
            )";

			// Display the custom error HTML message in the WebBrowser component
			WebBrowserControl->DocumentText = errorHtml;

		}

	}
	else {

		// Construct custom HTML for an error message
		String^ errorHtml = R"(
                <html>
                <head>
                    <style>
                        body { background-color: #232323; color: #ffffff; font-family: Arial, sans-serif; text-align: center; padding: 50px; }
                        h1 { color: #ff0000; }
                        p { font-size: 16px; }
                    </style>
                </head>
                <body>
                    <h1> Adobe Reader Is Not Installed!</h1>
                    <p>- Please Install Adobe Reader To Read The Software User Manual -</p>
					<p>Link: https://get.adobe.com/dk/reader/</p>
                </body>
                </html>
            )";

		// Display the custom error HTML message in the WebBrowser component
		WebBrowserControl->DocumentText = errorHtml;

	}

}

// ------------------------------------------------------------------------------------------ //


