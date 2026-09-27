
/*
 *  RMH_AnalysisMode_Routines.c
 *
 *  Author: Rune Mark Hansen
 *  Date: July 2023
 *
 */

// Included libraries
#include <Windows.h>
#include <wincodec.h>
#include <opencv2/opencv.hpp>
#include <msclr/marshal_cppstd.h>
#include "RMH_MathConversions_Library.h"
#include "RMH_AnalysisMode_Routines.h"

// Global namespaces
using namespace System;
using namespace System::Windows::Forms;
using namespace System::Diagnostics;
using namespace std;

// Global variables and objects
cv::Mat FrameFormatRGB;
cv::VideoWriter RECAnalysisModeFileWriterRAW;
cv::VideoWriter LiveViewDataFileWriter;
cv::VideoCapture RECAnalysisModeFileReader;

// -------------------------------- Common Video File Recording And Analysis Mode Handling Routines -------------------------------- //

void RMH_AnalysisMode_AddIDAndMetaDataToFrameArray(unsigned int FrameWidth, unsigned int FrameHeight, unsigned char *RAWFrameDataArray,
    unsigned short CameraPoolID, unsigned int MetaDataSizeID, unsigned int FrameWidthPixelOffsetID, unsigned int FrameHeightPixelOffsetID, 
    float CameraTempCorrectionSetting, float CameraAmbientTempSetting, float CameraReflectedTempSetting, float CameraHumiditySetting, float CameraEmissivitySetting, unsigned int CameraDistanceSetting) {

    // This routine adds an extra row of data to the given frame metadata area.
    // The data is used to identify the data in a saved video or snapshot file, as well as the file itself
    // This is likewise used to add metadata to the recorded file.

    // Read the temporary array data and sort the kernel array
    const char* IdentificationDataPointer;
    unsigned int lineSize = FrameWidth * 3;
    unsigned int lineIndex = FrameHeight - 1;
    unsigned int startIndex = lineIndex * lineSize;
    unsigned char SettingsValueArray[4];

    // Write the identification data string of the file to the frame metadata area
    for (unsigned int i = 0; i < _RAWRecordingFileMetaDataIndex_IDStringStop; i++) {

        // Write characters to the frame metadata area
        RAWFrameDataArray[startIndex + i] = _RAWFileIDData_FileIDString[i];

    }

    // Write the camera pool identification data of the file to the frame metadata area
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraPool] = CameraPoolID;

    // Write the metadata size identification data of the file to the frame metadata area
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_MetaDataSizeMSB] = (MetaDataSizeID & 0xFF00) >> 8;
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_MetaDataSizeLSB] = MetaDataSizeID & 0x00FF;

    // Write the frame width pixel offsets identification data of the file to the frame metadata area
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_FrameWidthPixelOffsetMSB] = (FrameWidthPixelOffsetID & 0xFF00) >> 8;
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_FrameWidthPixelOffsetLSB] = FrameWidthPixelOffsetID & 0x00FF;

    // Write the frame height pixel offsets identification data of the file to the frame metadata area
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_FrameHeightPixelOffsetMSB] = (FrameHeightPixelOffsetID & 0xFF00) >> 8;
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_FrameHeightPixelOffsetLSB] = FrameHeightPixelOffsetID & 0x00FF;

    // Convert the temperature correction value to 4x8-bit
    RMH_Conversion_SinglePrecisionFloatTo4xUnsignedChar(CameraTempCorrectionSetting, &SettingsValueArray[0]);
    // Write the temperature correction value of the thermal camera to the frame metadata area
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraTempCorrectionMSB] = SettingsValueArray[0];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraTempCorrectionLSB1] = SettingsValueArray[1];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraTempCorrectionLSB2] = SettingsValueArray[2];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraTempCorrectionLSB3] = SettingsValueArray[3];

    // Convert the ambient temperature value to 4x8-bit
    RMH_Conversion_SinglePrecisionFloatTo4xUnsignedChar(CameraAmbientTempSetting, &SettingsValueArray[0]);
    // Write the ambient temperature value of the thermal camera to the frame metadata area
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraAmbientTempMSB] = SettingsValueArray[0];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraAmbientTempLSB1] = SettingsValueArray[1];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraAmbientTempLSB2] = SettingsValueArray[2];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraAmbientTempLSB3] = SettingsValueArray[3];

    // Convert the reflected temperature value to 4x8-bit
    RMH_Conversion_SinglePrecisionFloatTo4xUnsignedChar(CameraReflectedTempSetting, &SettingsValueArray[0]);
    // Write the reflected temperature value of the thermal camera to the frame metadata area
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraReflectedTempMSB] = SettingsValueArray[0];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraReflectedTempLSB1] = SettingsValueArray[1];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraReflectedTempLSB2] = SettingsValueArray[2];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraReflectedTempLSB3] = SettingsValueArray[3];

    // Convert the humidity setting value to 4x8-bit
    RMH_Conversion_SinglePrecisionFloatTo4xUnsignedChar(CameraHumiditySetting, &SettingsValueArray[0]);
    // Write the humidity setting value of the thermal camera to the frame metadata area
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraHumidityMSB] = SettingsValueArray[0];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraHumidityLSB1] = SettingsValueArray[1];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraHumidityLSB2] = SettingsValueArray[2];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraHumidityLSB3] = SettingsValueArray[3];

    // Convert the emissivity setting value to 4x8-bit
    RMH_Conversion_SinglePrecisionFloatTo4xUnsignedChar(CameraEmissivitySetting, &SettingsValueArray[0]);
    // Write the emissivity setting value of the thermal camera to the frame metadata area
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraEmissivityMSB] = SettingsValueArray[0];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraEmissivityLSB1] = SettingsValueArray[1];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraEmissivityLSB2] = SettingsValueArray[2];
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraEmissivityLSB3] = SettingsValueArray[3];

    // Write the distance setting value of the thermal camera to the frame metadata area
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraDistanceMSB] = (CameraDistanceSetting & 0xFF00) >> 8;
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_CameraDistanceLSB] = CameraDistanceSetting & 0x00FF;

    // Write the end character of the file - indicates the end of the ID data
    RAWFrameDataArray[startIndex + _RAWRecordingFileMetaDataIndex_DataEndChar] = '!';

}

RAWFileIDFormat RMH_AnalysisMode_ReadRAWMetaData(unsigned int FrameWidth, unsigned int FrameHeight, unsigned char* RAWFrameDataArray) {

    // This routine reads the identification/metadata of the RAW file.
    // The data is used to identify the data in the file and whether the file is a RAW file for analysis in snapshot or recording "Analysis Mode"

    // Read the temporary array data and sort the kernel array
    RAWFileIDFormat FileIDData;
    unsigned int LineSize = FrameWidth * 3;
    unsigned int LineIndex = FrameHeight - 1;
    unsigned int StartIndex = LineIndex * LineSize;
    unsigned char SettingsValueArray[4];

    // Loop from the start index of the added metadata row
    for (unsigned int i = StartIndex, j = 0; i < StartIndex + LineSize; i++, j++) {

        // Read the identification characters of the file
        if (j <= _RAWRecordingFileMetaDataIndex_IDStringStop) {  FileIDData.RAWIDCharData[j] = RAWFrameDataArray[i]; }
        // Read the camera pool metadata of the file
        if (j == _RAWRecordingFileMetaDataIndex_CameraPool) { FileIDData.CameraPoolID = RAWFrameDataArray[i]; }
        // Read the metadata size metadata of the file
        if (j == _RAWRecordingFileMetaDataIndex_MetaDataSizeMSB) { FileIDData.FileMetaDataSizeID = ((unsigned int)RAWFrameDataArray[i] << 8) | (unsigned int)RAWFrameDataArray[i + 1]; }
        // Read the frame width pixel offsets metadata
        if (j == _RAWRecordingFileMetaDataIndex_FrameWidthPixelOffsetMSB) { FileIDData.FileFrameWidthPixelOffsetID = ((unsigned int)RAWFrameDataArray[i] << 8) | (unsigned int)RAWFrameDataArray[i + 1]; }
        // Read the frame height pixel offsets metadata
        if (j == _RAWRecordingFileMetaDataIndex_FrameHeightPixelOffsetMSB) { FileIDData.FileFrameHeightPixelOffsetID = ((unsigned int)RAWFrameDataArray[i] << 8) | (unsigned int)RAWFrameDataArray[i + 1]; }

        // Read the temperature correction parameter of the thermal camera
        if (j == _RAWRecordingFileMetaDataIndex_CameraTempCorrectionMSB) { FileIDData.RecordingTempCorrectionSetting = RMH_Conversion_UnsignedCharToSinglePrecisionFloat2(RAWFrameDataArray[i + 0], RAWFrameDataArray[i + 1], RAWFrameDataArray[i + 2], RAWFrameDataArray[i + 3]); }
        // Read the ambient temperature parameter of the thermal camera
        if (j == _RAWRecordingFileMetaDataIndex_CameraAmbientTempMSB) { FileIDData.RecordingAmbientTempSetting = RMH_Conversion_UnsignedCharToSinglePrecisionFloat2(RAWFrameDataArray[i + 0], RAWFrameDataArray[i + 1], RAWFrameDataArray[i + 2], RAWFrameDataArray[i + 3]); }
        // Read the reflected temperature parameter of the thermal camera
        if (j == _RAWRecordingFileMetaDataIndex_CameraReflectedTempMSB) { FileIDData.RecordingReflectedTempSetting = RMH_Conversion_UnsignedCharToSinglePrecisionFloat2(RAWFrameDataArray[i + 0], RAWFrameDataArray[i + 1], RAWFrameDataArray[i + 2], RAWFrameDataArray[i + 3]); }
        // Read the humidity setting parameter of the thermal camera
        if (j == _RAWRecordingFileMetaDataIndex_CameraHumidityMSB) { FileIDData.RecordingHumiditySetting = RMH_Conversion_UnsignedCharToSinglePrecisionFloat2(RAWFrameDataArray[i + 0], RAWFrameDataArray[i + 1], RAWFrameDataArray[i + 2], RAWFrameDataArray[i + 3]); }
        // Read the emissivity setting parameter of the thermal camera
        if (j == _RAWRecordingFileMetaDataIndex_CameraEmissivityMSB) { FileIDData.RecordingEmissivitySetting = RMH_Conversion_UnsignedCharToSinglePrecisionFloat2(RAWFrameDataArray[i + 0], RAWFrameDataArray[i + 1], RAWFrameDataArray[i + 2], RAWFrameDataArray[i + 3]); }
        // Read the distance setting parameter of the thermal camera
        if (j == _RAWRecordingFileMetaDataIndex_CameraDistanceMSB) { FileIDData.RecordingDistanceSetting = ((unsigned int)RAWFrameDataArray[i] << 8) | (unsigned int)RAWFrameDataArray[i + 1]; }

        // Check whether the end character has been reached
        if (RAWFrameDataArray[i] == '!') {

            // Break the for loop
            break;

        }
        
    }
    
    // Return the identification data structure
    return FileIDData;

}

bool RMH_AnalysisMode_IsRAWMetaDataValid(RAWFileIDFormat* MetaData, unsigned int FrameWidth, unsigned int FrameHeight) {

    // This routine checks that the metadata read from a RAW file is consistent with the frame it describes.
    // The values come from the file, so they are checked before they are used to index the frame buffers.
    // The routine returns true if the metadata can be used safely.

    // The frame itself must fit the frame buffers
    if (RMH_FrameBuffer_IsFrameSizeSupported(FrameWidth, FrameHeight) == false) { return false; }

    // The camera pool must be one of the supported pools (1 - 4)
    if (MetaData->CameraPoolID < 1 || MetaData->CameraPoolID > 4) { return false; }

    // The metadata rows must be fewer than the frame rows
    if (MetaData->FileMetaDataSizeID >= FrameHeight) { return false; }

    // The pixel offsets must lie inside the frame
    if (MetaData->FileFrameWidthPixelOffsetID >= FrameWidth) { return false; }
    if (MetaData->FileFrameHeightPixelOffsetID >= FrameHeight) { return false; }

    // Return: the metadata is valid
    return true;

}

// ---------------------------- Video File Recording, Configuration, Setting And Writing Routines ---------------------------- //

bool RMH_VideoFileRecording_SetupRecordingAnalysisModeVideoFile(System::String^ FileSavePath, System::String^ FileName, unsigned int FrameWidth, unsigned int FrameHeight, double FrameRate) {

    // This routine configures and prepares an .avi video file for recording RAW camera data.
    // This file is used in "Recording Analysis mode".
    // The saved AVI video file name is formatted as: "'FileName'_HHmmssddMMyyyy"
    // Input file path (FileSavePath) example: C:\Users\User\Desktop
    
    // Format the file name string (Recording_HHmmssddMMyyyy)
    System::String^ FileNameDate = System::DateTime::Now.ToString("HHmmssddMMyyyy");

    // Convert the given file name string to a cv::string - with the associated .avi file type string 
    cv::String FileNameString = RMH_VideoRecording_ConvertSystemStringToCVString(FileSavePath + "/" + FileName + FileNameDate + ".avi");

    // Configure and open the AVI video file for writing - FourCC: RGBA - RAW format
    RECAnalysisModeFileWriterRAW.open(FileNameString, cv::VideoWriter::fourcc('R', 'G', 'B', 'A'), FrameRate, cv::Size(FrameWidth, FrameHeight + 1));

    // Check whether the "VideoWriter" object has been initialized correctly and is ready for writing.
    if (RECAnalysisModeFileWriterRAW.isOpened() == true) {

        // Return: "VideoWriter" object initialization OK
        return true;

    }
    else {

        // Return: "VideoWriter" object initialization error!
        return false;

    }

}

bool RMH_VideoFileRecording_SetupLiveViewCaptureVideoFile(System::String^ FileSavePath, System::String^ FileName, unsigned int FrameWidth, unsigned int FrameHeight, double FrameRate) {

    // This routine configures and prepares an .avi video file for recording the live view stream data.
    // The saved AVI video file name is formatted as: "'FileName'_HHmmssddMMyyyy"
    // Input file path (FileSavePath) example: C:\Users\User\Desktop

    // Format the file name string (Recording_HHmmssddMMyyyy)
    System::String^ FileNameDate = System::DateTime::Now.ToString("HHmmssddMMyyyy");

    // Convert the given file name string to a cv::string - with the associated .avi file type string 
    cv::String FileNameString = RMH_VideoRecording_ConvertSystemStringToCVString(FileSavePath + "/" + FileName + FileNameDate + ".avi");

    // Configure and open the AVI video file for writing - FourCC: YUYV 
    LiveViewDataFileWriter.open(FileNameString, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), FrameRate, cv::Size(FrameWidth, FrameHeight));

    // Check whether the "VideoWriter" object has been initialized correctly and is ready for writing.
    if (LiveViewDataFileWriter.isOpened() == true) {

        // Return: "VideoWriter" object initialization OK
        return true;

    }
    else {

        // Return: "VideoWriter" object initialization error!
        return false;

    }

}

void RMH_VideoFileRecording_WriteDataToFile(unsigned short FileIndex, unsigned int FrameWidth, unsigned int FrameHeight, unsigned char *CapturedFrameData) {

    // This routine writes the given frame data array to the selected video file

    // Which VideoWriter object has been selected
    switch (FileIndex) {

        // VideoWriter object number 1
        case _VideoFileWriteObject_RecordingAnalysisModeFile:

            // Add an extra pixel row to the analysis mode file - for ID data
            FrameHeight = FrameHeight + 1;

        break;

    }

    // Convert the image data to cv::Mat format
    cv::Mat MatFrame(FrameHeight, FrameWidth, CV_8UC3, CapturedFrameData);

    // Convert BGR format to RGB format
    cv::cvtColor(MatFrame, FrameFormatRGB, cv::COLOR_BGR2RGB);

    // Which VideoWriter object has been selected
    switch (FileIndex) {

        // VideoWriter object number 1 - Recording Analysis mode file
        case _VideoFileWriteObject_RecordingAnalysisModeFile:

            // Write the frame data to the AVI video file
            RECAnalysisModeFileWriterRAW.write(FrameFormatRGB);
            
        break;

        // VideoWriter object number 2 - live view stream file
        case _VideoFileWriteObject_LiveViewStreamFile:

            // Write the frame data to the AVI video file
            LiveViewDataFileWriter.write(FrameFormatRGB);

        break;

    }

}

void RMH_VideoFileRecording_WriteDataToFile16Bit(unsigned short FileIndex, unsigned int FrameWidth, unsigned int FrameHeight, unsigned short* CapturedFrameData) {

    // This routine writes the given frame data array to the selected video file

    // Local objects
    cv::Mat MatFrame8U;

    // Which VideoWriter object has been selected
    switch (FileIndex) {

        // VideoWriter object number 1
        case _VideoFileWriteObject_RecordingAnalysisModeFile:

            // Add an extra pixel row to the analysis mode file - for ID data
            FrameHeight = FrameHeight + 1;

        break;

    }

    // Convert the image data to cv::Mat format
    cv::Mat MatFrame(FrameHeight, FrameWidth, CV_16UC3, CapturedFrameData);

    // Convert a 16-bit video frame to 8-bit (1 / 256 = 0.00390625)
    MatFrame.convertTo(MatFrame8U, CV_8UC3, 0.00390625);

    // Convert BGR format to RGB format
    cv::cvtColor(MatFrame8U, FrameFormatRGB, cv::COLOR_BGR2RGB);

    // Which VideoWriter object has been selected
    switch (FileIndex) {

        // VideoWriter object number 1 - Recording Analysis mode file
        case _VideoFileWriteObject_RecordingAnalysisModeFile:

            // Write the frame data to the AVI video file
            RECAnalysisModeFileWriterRAW.write(FrameFormatRGB);

        break;

        // VideoWriter object number 2 - live view stream file
        case _VideoFileWriteObject_LiveViewStreamFile:

            // Write the frame data to the AVI video file
            LiveViewDataFileWriter.write(FrameFormatRGB);

        break;

    }

}

bool RMH_VideoFileRecording_CloseVideoFileWriting(unsigned short FileIndex) {

    // This routine closes and saves the AVI video file
    // Returns "true" if the "VideoWriter" object has been closed correctly

    // Which VideoWriter object has been selected
    switch (FileIndex) {

        // VideoWriter object number 1 - Recording Analysis mode file
        case _VideoFileWriteObject_RecordingAnalysisModeFile:

            // Check whether the "VideoWriter" object is open.
            if (RECAnalysisModeFileWriterRAW.isOpened() == true) {

                // Close the AVI video file for writing
                RECAnalysisModeFileWriterRAW.release();

                // Return the status
                return true;

            }
            else {

                // Return the status
                return false;

            }

        break;

        // VideoWriter object number 2 - live view stream file
        case _VideoFileWriteObject_LiveViewStreamFile:

            // Check whether the "VideoWriter" object is open.
            if (LiveViewDataFileWriter.isOpened() == true) {

                // Close the AVI video file for writing
                LiveViewDataFileWriter.release();

                // Return the status
                return true;

            }
            else {

                // Return the status
                return false;

            }

        break;

    }

}

// ----------------------------- Video File Reading, Configuration, Setting And Writing Routines ----------------------------- //

bool RMH_VideoFileReading_IsRECAnalysisModeFileOpen() {

    // This routine checks whether the AVI file used in "Recording Analysis" mode is open

    // Check whether the AVI file is open
    if (RECAnalysisModeFileReader.isOpened() == true) {

        // Return the status
        return true;

    }
    else {

        // Return the status
        return false;

    }

}

RAWVideoFileInfo RMH_VideoFileReading_SetupRecordingAnalysisModeVideoFileReader(System::String^ AVIFilePath) {

    // This routine configures and sets up a VideoReader object for reading an AVI video file in "Recording Analysis" mode
    // The routine returns "true" if the file was opened correctly - otherwise "false" on error

    // Local variables
    System::String^ FileTypeExtension;
    RAWVideoFileInfo RECAnalysisModeFileInfo;

    // If the given file path is "None" - no file selected
    if (AVIFilePath == "None") {

        // Reset the parameters of the file information structure
        RECAnalysisModeFileInfo.IsFileOpenFlag = false;
        RECAnalysisModeFileInfo.IsAVIFileFlag = false;
        RECAnalysisModeFileInfo.FrameWidth = 0;
        RECAnalysisModeFileInfo.FrameHeight = 0;
        RECAnalysisModeFileInfo.FrameRate = 0.0;
        RECAnalysisModeFileInfo.NumberOfFrames = 0;
        RECAnalysisModeFileInfo.DurationTime = 0.0;
        
    }
    else {

        // Check whether the file is already open
        if (RMH_VideoFileReading_IsRECAnalysisModeFileOpen() == true) {

            // Close the video file
            RECAnalysisModeFileReader.release();

        }

        // Read which file type has been selected from the given input file path
        FileTypeExtension = AVIFilePath->Substring(AVIFilePath->LastIndexOf(".") + 1);

        // Check whether the given path is to an ".avi" file
        if (FileTypeExtension != "avi") {

            // Reset the "the file is an .avi file" flag
            RECAnalysisModeFileInfo.IsAVIFileFlag = false;

        }
        else {

            // Convert the given input file path to a cv::string
            cv::String FilePath = RMH_VideoRecording_ConvertSystemStringToCVString(AVIFilePath);

            // Open the selected AVI file 
            RECAnalysisModeFileReader.open(FilePath);

            // Check whether the file has been opened
            if (RMH_VideoFileReading_IsRECAnalysisModeFileOpen() == true) {

                // Update the "the file is an .avi file" flag
                RECAnalysisModeFileInfo.IsAVIFileFlag = true;

                // Update the "the file is open" flag
                RECAnalysisModeFileInfo.IsFileOpenFlag = true;

                // Read the pixel width and height of the file
                RECAnalysisModeFileInfo.FrameWidth = RECAnalysisModeFileReader.get(cv::CAP_PROP_FRAME_WIDTH);
                RECAnalysisModeFileInfo.FrameHeight = RECAnalysisModeFileReader.get(cv::CAP_PROP_FRAME_HEIGHT);

                // Read the frame rate of the file
                RECAnalysisModeFileInfo.FrameRate = RECAnalysisModeFileReader.get(cv::CAP_PROP_FPS);

                // Read the number of data frames the file contains
                RECAnalysisModeFileInfo.NumberOfFrames = RECAnalysisModeFileReader.get(cv::CAP_PROP_FRAME_COUNT);

                // Calculate the length of the file in seconds
                RECAnalysisModeFileInfo.DurationTime = (double)RECAnalysisModeFileInfo.NumberOfFrames / (double)RECAnalysisModeFileInfo.FrameRate;

            }
            else {

                // Reset the parameters of the file
                RECAnalysisModeFileInfo.IsFileOpenFlag = false;
                RECAnalysisModeFileInfo.IsAVIFileFlag = false;
                RECAnalysisModeFileInfo.FrameWidth = 0;
                RECAnalysisModeFileInfo.FrameHeight = 0;
                RECAnalysisModeFileInfo.FrameRate = 0.0;
                RECAnalysisModeFileInfo.NumberOfFrames = 0;
                RECAnalysisModeFileInfo.DurationTime = 0.0;

            }

        }

    }

    // Return the file information
    return RECAnalysisModeFileInfo;

}

unsigned int RMH_VideoFileReading_ReadVideoFileFrame(unsigned long TargetFrameNumber, unsigned long FileTotalNumOfFrames, unsigned char *ReadFrameData, size_t ReadFrameDataCapacity) {

    // This routine reads a video frame from the open "Recording Analysis" mode RAW video file
    
    /*
     *   Returns the following status/error codes:
     * 
     *   // Video file read error code reference macros
     *   #define _ReadAVIFile_StatusCode_FrameReadOK              1
     *   #define _ReadAVIFile_StatusCode_FileIsNotOpen            2
     *   #define _ReadAVIFile_StatusCode_FrameNumberOutOfRange    3
     *   #define _ReadAVIFile_StatusCode_FrameReadError           4
     *   #define _ReadAVIFile_StatusCode_FrameTooLarge            5
     * 
     */

    // Read the temporary array data and sort the kernel array
    cv::Mat ReadFrame;
    cv::Mat ReadFrameRGB;

    // Check whether the video file is open
    if (RMH_VideoFileReading_IsRECAnalysisModeFileOpen() == true) {

        // Is the given frame number within 0 - FileTotalNumOfFrames
        if (TargetFrameNumber >= 0 && TargetFrameNumber < FileTotalNumOfFrames) {

            // Set the position of the selected frame number for reading
            RECAnalysisModeFileReader.set(cv::CAP_PROP_POS_FRAMES, TargetFrameNumber);

            // Read the data of the frame number - check whether the data was read correctly
            if (RECAnalysisModeFileReader.read(ReadFrame)) {

                // Convert the frame read from BGR to RGB
                cv::cvtColor(ReadFrame, ReadFrameRGB, cv::COLOR_BGR2RGB);

                // Refuse a frame that is larger than the destination buffer (the frame size comes from the file)
                if (ReadFrameRGB.total() * ReadFrameRGB.elemSize() > ReadFrameDataCapacity) {

                    // Return status - the frame is too large for the buffer
                    return _ReadAVIFile_StatusCode_FrameTooLarge;

                }

                // Write the frame data to the pointer 
                memcpy(ReadFrameData, ReadFrameRGB.data, ReadFrameRGB.total() * ReadFrameRGB.elemSize());

            }
            else {

                // Return status - error reading the frame
                return _ReadAVIFile_StatusCode_FrameReadError;

            }

        }
        else {

            // Return status - the given "FrameNumber" is out of range
            return _ReadAVIFile_StatusCode_FrameNumberOutOfRange;

        }

        // Return status - the frame has been read
        return _ReadAVIFile_StatusCode_FrameReadOK;

    }
    else {

        // Return status - file not open
        return _ReadAVIFile_StatusCode_FileIsNotOpen;

    }

}

bool RMH_VideoFileReading_CloseRecordingAnalysisModeFile() {

    // This routine closes reading of the AVI video file
    // Returns "true" if the "VideoCapture" object has been closed correctly

    // Check whether the "VideoCapture" object is open.
    if (RMH_VideoFileReading_IsRECAnalysisModeFileOpen() == true) {

        // Close the AVI video file for reading
        RECAnalysisModeFileReader.release();

        // Return the status
        return true;

    }
    else {

        // Return the status
        return false;

    }

}

// --------------------------- Snapshot File Reading, Configuration, Setting And Writing Routines ---------------------------- //

RAWSnapShotFileInfo RMH_AnalysisMode_ReadAndLoadPNGImage(System::String^ ImageFilePath, unsigned char* ImageData, size_t ImageDataCapacity) {

    // This routine opens and reads a PNG image file at the selected path, where the pixel data of the image can be read as: 
    // unsigned char RED = ImageData[3 * (Y * Width + X)];
    // unsigned char GREEN = ImageData[3 * (Y * Width + X) + 1];
    // unsigned char BLUE = ImageData[3 * (Y * Width + X) + 2];

    // Local variables
    unsigned int ImgWidth = 0;
    unsigned int ImgHeight = 0;
    size_t BufferSize = 0;
    WICPixelFormatGUID PixelFormat;
    System::String^ FileTypeExtension;
    RAWSnapShotFileInfo SnapShotAnalysisModeFileInfo;

    // Read which file type has been selected from the given input file path
    FileTypeExtension = ImageFilePath->Substring(ImageFilePath->LastIndexOf(".") + 1);

    // Check whether the given path is to a ".png" file
    if (FileTypeExtension != "png") {

        // Reset the "the file is a .png file" flag
        SnapShotAnalysisModeFileInfo.IsPNGFileFlag = false;
        // Reset the "the file is not ready" flag
        SnapShotAnalysisModeFileInfo.IsFileReady = false;
        // Reset the file error flag
        SnapShotAnalysisModeFileInfo.FileErrorFlag = true;

        // Return the info structure
        return SnapShotAnalysisModeFileInfo;

    }
    else if (FileTypeExtension == "png") {

        // Update the "the file is a .png file" flag
        SnapShotAnalysisModeFileInfo.IsPNGFileFlag = true;

    }

    // Convert System::String to a standard C++ string
    std::wstring wfilename = msclr::interop::marshal_as<std::wstring>(ImageFilePath);

    // Initialize the COM library
    CoInitialize(nullptr);

    // Create and configure the "WIC factory"
    IWICImagingFactory* pFactory = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));

    // Check the handler error
    if (FAILED(hr)) {

        // Uninitialize the COM library
        CoUninitialize();

        // Reset the "the file is not ready" flag
        SnapShotAnalysisModeFileInfo.IsFileReady = false;
        // Reset the file error flag
        SnapShotAnalysisModeFileInfo.FileErrorFlag = true;

        // Return the info structure
        return SnapShotAnalysisModeFileInfo;

    }

    // Create and configure the decoder for the PNG image
    IWICBitmapDecoder* pDecoder = nullptr;
    hr = pFactory->CreateDecoderFromFilename(wfilename.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

    // Check the handler error
    if (FAILED(hr)) {

        // Release the "WIC factory"
        pFactory->Release();
        // Uninitialize the COM library
        CoUninitialize();

        // Reset the "the file is not ready" flag
        SnapShotAnalysisModeFileInfo.IsFileReady = false;
        // Reset the file error flag
        SnapShotAnalysisModeFileInfo.FileErrorFlag = true;

        // Return the info structure
        return SnapShotAnalysisModeFileInfo;

    }

    // Read the first image frame from the file decoder (PNG images have only one frame)
    IWICBitmapFrameDecode* pFrame = nullptr;
    hr = pDecoder->GetFrame(0, &pFrame);

    // Check the handler error
    if (FAILED(hr)) {

        // Release the image decoder
        pDecoder->Release();
        // Release the "WIC factory"
        pFactory->Release();
        // Uninitialize the COM library
        CoUninitialize();

        // Reset the "the file is not ready" flag
        SnapShotAnalysisModeFileInfo.IsFileReady = false;
        // Reset the file error flag
        SnapShotAnalysisModeFileInfo.FileErrorFlag = true;

        // Return the info structure
        return SnapShotAnalysisModeFileInfo;

    }

    // Read the size of the PNG image
    hr = pFrame->GetSize(&ImgWidth, &ImgHeight);

    // Check the handler error
    if (FAILED(hr)) {

        // Release the image size structure
        pFrame->Release();
        // Release the image decoder
        pDecoder->Release();
        // Release the "WIC factory"
        pFactory->Release();
        // Uninitialize the COM library
        CoUninitialize();

        // Reset the "the file is not ready" flag
        SnapShotAnalysisModeFileInfo.IsFileReady = false;
        // Reset the file error flag
        SnapShotAnalysisModeFileInfo.FileErrorFlag = true;

        // Return the info structure
        return SnapShotAnalysisModeFileInfo;

    }

    // Read the pixel format of the PNG image
    hr = pFrame->GetPixelFormat(&PixelFormat);

    // Calculate the buffer size for the pixel data of the PNG image (3 bands RGB) - in 64-bit to avoid overflow
    BufferSize = (size_t)ImgWidth * (size_t)ImgHeight * 3;

    // The image must be a 24-bit RGB snapshot that fits the frame buffers (the size and format come from the file)
    if (FAILED(hr) || PixelFormat != GUID_WICPixelFormat24bppBGR || RMH_FrameBuffer_IsFrameSizeSupported(ImgWidth, ImgHeight) == false || BufferSize > ImageDataCapacity) {

        // Release the image size structure
        pFrame->Release();
        // Release the image decoder
        pDecoder->Release();
        // Release the "WIC factory"
        pFactory->Release();
        // Uninitialize the COM library
        CoUninitialize();

        // Reset the "the file is not ready" flag
        SnapShotAnalysisModeFileInfo.IsFileReady = false;
        // Reset the file error flag
        SnapShotAnalysisModeFileInfo.FileErrorFlag = true;

        // Return the info structure
        return SnapShotAnalysisModeFileInfo;

    }

    // Read and return, to pointers, the height and width of the PNG image read
    SnapShotAnalysisModeFileInfo.FrameWidth = static_cast<unsigned int>(ImgWidth);
    SnapShotAnalysisModeFileInfo.FrameHeight = static_cast<unsigned int>(ImgHeight);

    // Allocate memory for the pixel data of the image
    unsigned char* Buffer = new unsigned char[BufferSize];

    // Read the PNG image pixel data RGB
    WICRect Rect = { 0, 0, static_cast<unsigned int>(ImgWidth), static_cast<unsigned int>(ImgHeight) };
    hr = pFrame->CopyPixels(&Rect, ImgWidth * 3, static_cast<UINT>(BufferSize), Buffer);

    // Check the handler error
    if (FAILED(hr)) {

        // Delete the allocated memory of the pixel buffer 
        delete[] Buffer;
        // Release the image size structure
        pFrame->Release();
        // Release the image decoder
        pDecoder->Release();
        // Release the "WIC factory"
        pFactory->Release();
        // Uninitialize the COM library
        CoUninitialize();

        // Reset the "the file is not ready" flag
        SnapShotAnalysisModeFileInfo.IsFileReady = false;
        // Reset the file error flag
        SnapShotAnalysisModeFileInfo.FileErrorFlag = true;

        // Return the info structure
        return SnapShotAnalysisModeFileInfo;

    }

    // Convert the image BGR data read to RGB
    for (unsigned int Y = 0; Y < ImgHeight; Y++) {

        // Loop through all rows of the array matrix
        for (unsigned int X = 0; X < ImgWidth; X++) {

            // Calculate the start index of the current pixel
            unsigned int pixelIndex = 3 * (Y * ImgWidth + X);

            // Swap the R and B components
            unsigned char temp = Buffer[pixelIndex];
            Buffer[pixelIndex] = Buffer[pixelIndex + 2]; // R -> B
            Buffer[pixelIndex + 2] = temp; // B -> R
        }
    }

    // Release the image handler resources
    pFrame->Release();
    pDecoder->Release();
    pFactory->Release();
    // Uninitialize the COM library
    CoUninitialize();

    // Copy the pixel data to the "ImageData" array
    memcpy(ImageData, Buffer, BufferSize);

    // Delete the allocated memory of the pixel buffer 
    delete[] Buffer;

    // Update the "the file is ready" flag
    SnapShotAnalysisModeFileInfo.IsFileReady = true;
    // Update the file error flag
    SnapShotAnalysisModeFileInfo.FileErrorFlag = false;

    // Return the info structure
    return SnapShotAnalysisModeFileInfo;

}

// ----------------------------------------------------------------------------------------------------------------------------------- //