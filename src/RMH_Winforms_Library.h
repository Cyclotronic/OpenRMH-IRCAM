/*
 *  RMH_Winforms_Library.h
 *
 *  Author: Rune Mark Hansen
 *  Date: November 2022
 *
 */

#pragma once

 // RMH_Winforms_Library.h
#ifndef RMH_Winforms_Library_H 
#define RMH_Winforms_Library_H

// Included libraries
#include <string>
#include <vector>
#include <array>

// Status message type macros
#define _StatusMessageType_Normal      1
#define _StatusMessageType_Success     2
#define _StatusMessageType_Warning     3
#define _StatusMessageType_Error       4

// Microsoft Store application "PackageFamilyName" strings
#define _MicrosoftStore_ScreenRecorderForWindows11          "45907smallapp.ScreenRecorderforWindows11_z9hw59krvrfng"
#define _MicrosoftStore_SnippingTool                        "Microsoft.ScreenSketch_8wekyb3d8bbwe"

// Form docking & undocking settings macros
#define _FormDockingState_DockForm         1
#define _FormDockingState_UndockForm       2

// ---------------------------- Associated Classes And Structures ---------------------------- //

// Associated namespace for the class
namespace RMHWinformsLib {

    // Specific class for file reading 
    class FileReadFormat {
    public:

        // Class variables and objects
        bool FileReadSuccess = false;
        bool FileZeroLengthFlag = false;
        std::vector<std::string> FileStrings{120};
        unsigned long FileLineLength = 0;

    };

}

// Specific structure for reading screen setting parameters
struct WINMonitorSettings {

    // Structure variables and objects (max number of monitors = 10)
    unsigned short NumberOfConnectedMonitors = 0;
    unsigned short MonitorVirtualWidth = 0;
    unsigned short MonitorPhysicalWidth = 0;
    unsigned short MonitorVirtualHeight = 0;
    unsigned short MonitorPhysicalHeight = 0;
    unsigned short MonitorDPISetting = 0;
    unsigned short MonitorHorizontalScaleSetting = 0;
    unsigned short MonitorVerticalScaleSetting = 0;

};

// ------------------------- WinForms Title Bar Handling Routines -------------------------- //

void RMH_Winforms_EnableTitleBarDarkMode(System::IntPtr FormHandle);

// ----------------------------- WinForms Benchmarking Routines ----------------------------- //

System::Diagnostics::Stopwatch^ RMH_Winforms_StartBenchMarkTimer();
void RMH_Winforms_StopBenchmarkTimerAndDisplay(System::Diagnostics::Stopwatch^ BenchmarkTimer);

// ---------------------------- WinForms GUI Handling Routines ---------------------------- //

void RMH_Winforms_StartMainApplicationGUI();
void RMH_Winforms_ChangeFormTitleBarText(System::Windows::Forms::Form^ Winform, std::string Text);

// --------------------- WinForms External Process Handling Routines -------------------- //

void RMH_Winforms_OpenLinkURL(System::String^ LinkURL);
bool RMH_Winforms_OpenWindowsMicrosoftStoreApp(System::String^ PackageFamilyName);
void RMH_Winforms_OpenExternalApplicationEXE(System::String^ ExternalEXENameString);

// ------------ WinForms Windows Screen Width/Height/Scaling/DPI Reading Routines ------------ //

WINMonitorSettings RMH_Winforms_ReadWindowsScreenSettings();

// -------------------------- WinForms ComboBox Handling Routines ------------------------- //

void RMH_Winforms_CombiBox_AddArrayOfItemStrings(System::Windows::Forms::ComboBox^ CombiBox, std::vector<std::string> StringArray);
void RMH_Winforms_CombiBox_SetSellectedItemPosition(System::Windows::Forms::ComboBox^ CombiBox, int ItemIndex);

// ----------------------- WinForms NumericUpDown Handling Routines ----------------------- //

bool RMH_Winforms_NumericUpDown_ChangeNumber(System::Windows::Forms::NumericUpDown^ NumericUpDown, float InputNumber, float ScaleFactor, float Offset, float DefaultValue);

// ------------------------ WinForms RichTextBox Handling Routines ------------------------ //

void RMH_Winforms_RichTextBox_WriteLine(System::Windows::Forms::RichTextBox^ RichTextBox, std::string Text, unsigned int MessageType);

// ----------------------- WinForms DataGridView Handling Routines ------------------------ //

void RMH_Winforms_DataGridView_Display2ColumnDataGridView(System::Windows::Forms::DataGridView^ DataGridView, std::vector<std::string> ColumnsHeaderText, System::String^ RowHeaderText, float ColumnHeaderTextSize, float RowHeaderTextSize, float CellTextSize, System::Drawing::Color ColumnRowHeaderTextColor, System::Drawing::Color ColumnRowHeaderBackColor, unsigned int ColumnHeaderHeight, unsigned int RowHeaderWidth, std::vector<std::string> Column1Strings, float* Column2Data);

// --------------------------- WinForms Image Display Routines --------------------------- //

void RMH_Winforms_PictureBox_UpdateImageBitmap(System::Windows::Forms::PictureBox^ PictureBox, System::Drawing::Bitmap^ Bitmap, System::Drawing::Imaging::ColorPalette^ ColorPalette);

// --------------------- WinForms Menu And Sub-Menu Handling Routines --------------------- //

void RMH_Winforms_HideSubMenuPanel(System::Windows::Forms::Panel^ SubMenuPanel);
void RMH_Winforms_ToggleSubMenuPanel(System::Windows::Forms::Panel^ SubMenuPanel, System::Windows::Forms::Button^ MenuButton);

// ---------------- WinForms Form Docking & Undocking Handling Routines ---------------- //

void RMH_Winforms_CloseForm(System::Windows::Forms::Form^ FormObject, bool* FormOpenedFlag, bool* FormDockedFlag, bool* FormUndockedFlag);
void RMH_Winforms_OpenFormInSeperateWindow(System::Windows::Forms::Form^ FormObject, bool* FormOpenedFlag, bool* FormDockedFlag, bool* FormUndockedFlag);
void RMH_Winforms_OpenAndDockFormInParentPanel(System::Windows::Forms::Form^ FormObject, System::Windows::Forms::Panel^ ParentPanel, bool* FormOpenedFlag, bool* FormDockedFlag, bool* FormUndockedFlag);
void RMH_Winforms_UndockFormFromParentPanel(System::Windows::Forms::Form^ FormObject, System::Windows::Forms::Panel^ ParentPanel, bool* FormOpenedFlag, bool* FormDockedFlag, bool* FormUndockedFlag, System::Windows::Forms::FormBorderStyle FormBorderStyle);

// ------------ WinForms Display Child Form In Parent Panel Handling Routines ------------- //

bool RMH_Winforms_ToggleChildFormInParentPanel(System::Windows::Forms::Form^ ChildForm, cli::interior_ptr<System::Windows::Forms::Form^> CurrentActiveForm, System::Windows::Forms::Panel^ ParentPanel);
bool RMH_Winforms_AddChildAsControlToParentPanel(System::Windows::Forms::Form^ ChildForm, System::Windows::Forms::Panel^ ParentPanel);
bool RMH_Winforms_BringChildFormTOFront(System::Windows::Forms::Form^ ChildForm);
bool RMH_Winforms_SendChildFormToBack(System::Windows::Forms::Form^ ChildForm);

// ------------------------ WinForms Color Dialog Select Color Routines ------------------------ //

System::Drawing::Color^ RMH_Winforms_ShowAndReadColorDialog(bool* DialogAbortedFlag);

// ------------------------ WinForms CSV Write/Read Routines ------------------------ //

void RMH_Winforms_WriteHeaderStringsToCSVFile(std::string FilePath, std::string FileName, std::vector<std::string> HeaderStrings, unsigned int NmbOfHeaderStrings, System::String^ DataDelimiter);
void RMH_Winforms_WriteDataArrayToCSVFile(std::string FilePath, std::string FileName, std::string RowIDString, std::string RowHeaderString, double* CSVData, unsigned int NmbOfValues, System::String^ DataDelimiter);
void RMH_Winforms_WriteDataArrayMatrixToCSVFile(std::string FilePath, std::string FileName, double* CSVData, unsigned int ArrayMatrixWidth, unsigned int ArrayMatrixHeight, System::String^ DataDelimiter);
void RMH_Winforms_GenerateAndWriteCSVFile(std::string FilePath, std::string FileName, std::vector<std::string> AppendString); 
RMHWinformsLib::FileReadFormat RMH_Winforms_ReadLinesFromCSVFile(std::string FilePath, std::string FileName);

// -------------------- WinForms File Open/Save Dialog Handling Routines -------------------- //

System::String^ RMH_Winforms_GetSaveFileDialogDirectory();
System::String^ RMH_Winforms_GetOpenFileDialogDirectory();

// --------------------------- WinForms Chart Handling Routines --------------------------- //

void RMH_Winforms_Charts_ChangeXAxesLimits(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartArea1Index, double ChartXAxesMinimum, double ChartXAxesMaximum);
void RMH_Winforms_Charts_ChangeXAxesTickInterval(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartArea1Index, double AxesInterval);
void RMH_Winforms_Charts_ChangeYAxesLimits(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartArea1Index, double ChartYAxesMinimum, double ChartYAxesMaximum);
void RMH_Winforms_Charts_ChangeYAxesTickInterval(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartArea1Index, double AxesInterval);
void RMH_Winforms_Charts_AddDataArrayToChartSeries(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartSeriesIndex, double* SeriesXDataArray, double* SeriesYDataArray, unsigned int SeriesDataArrayLength);
void RMH_Winforms_Charts_AddDataPointToChartSeries(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartSeriesIndex, double PointXData, double PointYData);
void RMH_Winforms_Charts_ClearChartDataPoints(System::Windows::Forms::DataVisualization::Charting::Chart^ Chart, unsigned int ChartSeriesIndex);

// ------------------------ WinForms Panel Image Snapshot Routines ------------------------- //

bool RMH_Winforms_SavePanelSnapShotPNG(System::Windows::Forms::Panel^ SrcPanel, System::String^ SnapShotPath);
bool RMH_Winforms_SaveRawImageDataAsSnapShotPNG(System::String^ SnapShotPath, unsigned int ImageDataWidth, unsigned int ImageDataHeight, unsigned char* ImageData);

// ------------------------ WinForms Web Browser Handling Routines ------------------------ //

bool RMH_Winforms_IsAdobeReaderInstalled();
void RMH_Winforms_OpenPDFInWebbrowser(System::Windows::Forms::WebBrowser^ WebBrowserControl, System::String^ PDFFileName);

// ------------------------------------------------------------------------------------------ //

#endif /* RMH_Winforms_Library_H */
