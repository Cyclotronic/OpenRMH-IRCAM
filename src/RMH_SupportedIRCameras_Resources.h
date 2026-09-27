/*
 *  RMH_SupportedIRCameras_Resources.h
 *
 *  Author: Rune Mark Hansen
 *  Date: November 2022
 *
 */

#pragma once

 // Included libraries
#include <string>
#include <vector>

// RMH_SupportedIRCameras_Resources.h
#ifndef RMH_SupportedIRCameras_Resources_H 
#define RMH_SupportedIRCameras_Resources_H

// -------------------- Common Supported IR Camera Reference Macros --------------------- //

// Aspect Ratio Konstant for InfiRay & HTI Termiske kameraer
#define _FixedThermalCameraFrame_AspectRatio_Pool_1      1.33333333
#define _FixedThermalCameraFrame_AspectRatio_Pool_2      1.33333333
#define _FixedThermalCameraFrame_AspectRatio_Pool_3      1.33333333
#define _FixedThermalCameraFrame_AspectRatio_Pool_4      1.33333333

// Kamera Pools tids Konstanter Reference Macroer
#define _ThermalCameraShutter_CloseTimeMs                500
#define _ThermalCameraShutter_OpenTimeMs                 850
#define _ThermalCameraShutter_CALOpenTimeMs              1000
#define _ThermalCameraPool1_RangeSwitchReadyTimeMs       500
#define _ThermalCameraPool3_ReadyTimeMs                  4000
#define _ThermalCameraPool3_RangeSwitchReadyTimeMs       5000
#define _ThermalCameraPool4_RangeSwitchReadyTimeMs       5000

// IR camera temperature calculation parameter reference macros
#define _IRThermalCameraDefault_TemperatureCorrectionValue      0.0
#define _IRThermalCameraDefault_AmbientTemperatureValue         25.1    
#define _IRThermalCameraDefault_ReflectedTemperatureValue       25.1        
#define _IRThermalCameraDefault_SurroundingHumidityValue        0.45       
#define _IRThermalCameraDefault_ObjectEmissivityValue           0.98     
#define _IRThermalCameraDefault_ObjectDistanceValue             1.0  

// Supporterede Termiske kamera pool reference macroer
#define _SupportedThermalCameras_Pool_1                   1
#define _SupportedThermalCameras_Pool_2                   2
#define _SupportedThermalCameras_Pool_3                   3
#define _SupportedThermalCameras_Pool_4                   4

// Thermal camera temperature range reference macros 
#define _ThermalCamera_TemperatureRange_HighRange         1
#define _ThermalCamera_TemperatureRange_LowRange          0

// Supporterede Termiske kamera reference index macroer
#define _SnapShotAnalysisMode                             0  
#define _RecordingAnalysisMode                            1   
#define _SupportedThermalCamera_InfiRayT2L                2     
#define _SupportedThermalCamera_InfiRayT2LV2              3  
#define _SupportedThermalCamera_InfiRayT2Search           4 
#define _SupportedThermalCamera_InfiRayT2SearchV2         5  
#define _SupportedThermalCamera_InfiRayT2Sp               6   
#define _SupportedThermalCamera_InfiRayT2SpV2             7
#define _SupportedThermalCamera_InfiRayT2Pro              8  
#define _SupportedThermalCamera_InfiRayT2ProV2            9  
#define _SupportedThermalCamera_InfiRayT3Search           10          
#define _SupportedThermalCamera_InfiRayT3S                11    
#define _SupportedThermalCamera_InfiRayT3Pro              12       
#define _SupportedThermalCamera_InfiRayP2                 13   
#define _SupportedThermalCamera_InfiRayP2Pro              14     
#define _SupportedThermalCamera_InfiRayDVDL13             15    
#define _SupportedThermalCamera_InfiRayS0Series           16 
#define _SupportedThermalCamera_InfiRayTiny1C             17
#define _SupportedThermalCamera_ThermalMasterP2           18
#define _SupportedThermalCamera_HTIHT301                  19  
#define _SupportedThermalCamera_UNITUTi260M               20  
#define _SupportedThermalCamera_TOPDONTC001               21  
#define _SupportedThermalCamera_TOPDONTC002               22 
#define _SupportedThermalCamera_Victor328B                23 
#define _SupportedThermalCamera_LODESTARL2                24

// Reference macros for whether a thermal camera supports a higher temperature range
#define _SupportedThermalCamera_InfiRayT2L_SupportsHighRange                false      
#define _SupportedThermalCamera_InfiRayT2LV2_SupportsHighRange              false  
#define _SupportedThermalCamera_InfiRayT2Search_SupportsHighRange           false 
#define _SupportedThermalCamera_InfiRayT2SearchV2_SupportsHighRange         false 
#define _SupportedThermalCamera_InfiRayT2Sp_SupportsHighRange               true 
#define _SupportedThermalCamera_InfiRayT2SpV2_SupportsHighRange             true  // To be added when I figure out the calculations 
#define _SupportedThermalCamera_InfiRayT2Pro_SupportsHighRange              false 
#define _SupportedThermalCamera_InfiRayT2ProV2_SupportsHighRange            false 
#define _SupportedThermalCamera_InfiRayT3Search_SupportsHighRange           false          
#define _SupportedThermalCamera_InfiRayT3S_SupportsHighRange                true    
#define _SupportedThermalCamera_InfiRayT3Pro_SupportsHighRange              true       
#define _SupportedThermalCamera_InfiRayP2_SupportsHighRange                 true     
#define _SupportedThermalCamera_ThermalMasterP2_SupportsHighRange           true 
#define _SupportedThermalCamera_InfiRayP2Pro_SupportsHighRange              true     
#define _SupportedThermalCamera_InfiRayDVDL13_SupportsHighRange             true  
#define _SupportedThermalCamera_InfiRayS0Series_SupportsHighRange           true  
#define _SupportedThermalCamera_InfiRayTiny1C_SupportsHighRange             true  
#define _SupportedThermalCamera_HTIHT301_SupportsHighRange                  true  
#define _SupportedThermalCamera_UNITUTi260M_SupportsHighRange               true  
#define _SupportedThermalCamera_TOPDONTC001_SupportsHighRange               true   
#define _SupportedThermalCamera_TOPDONTC002_SupportsHighRange               true 
#define _SupportedThermalCamera_Victor328B_SupportsHighRange                true 
#define _SupportedThermalCamera_LODESTARL2_SupportsHighRange                true     

// Supporterer Termiske kamera Frame Rate Reference Macroer
#define _SupportedThermalCamera_InfiRayT2L_FrameRate                25.0      
#define _SupportedThermalCamera_InfiRayT2LV2_FrameRate              25.0  
#define _SupportedThermalCamera_InfiRayT2Search_FrameRate           25.0   
#define _SupportedThermalCamera_InfiRayT2SearchV2_FrameRate         25.0  
#define _SupportedThermalCamera_InfiRayT2Sp_FrameRate               25.0 
#define _SupportedThermalCamera_InfiRayT2SpV2_FrameRate             25.0 
#define _SupportedThermalCamera_InfiRayT2Pro_FrameRate              25.0  
#define _SupportedThermalCamera_InfiRayT2ProV2_FrameRate            25.0 
#define _SupportedThermalCamera_InfiRayT3Search_FrameRate           25.0           
#define _SupportedThermalCamera_InfiRayT3S_FrameRate                25.0     
#define _SupportedThermalCamera_InfiRayT3Pro_FrameRate              25.0        
#define _SupportedThermalCamera_InfiRayP2_FrameRate                 25.0      
#define _SupportedThermalCamera_ThermalMasterP2_FrameRate           25.0 
#define _SupportedThermalCamera_InfiRayP2Pro_FrameRate              25.0      
#define _SupportedThermalCamera_InfiRayDVDL13_FrameRate             25.0   
#define _SupportedThermalCamera_InfiRayS0Series_FrameRate           25.0   
#define _SupportedThermalCamera_InfiRayTiny1C_FrameRate             25.0   
#define _SupportedThermalCamera_HTIHT301_FrameRate                  25.0   
#define _SupportedThermalCamera_UNITUTi260M_FrameRate               25.0   
#define _SupportedThermalCamera_TOPDONTC001_FrameRate               25.0     
#define _SupportedThermalCamera_TOPDONTC002_FrameRate               25.0  
#define _SupportedThermalCamera_Victor328B_FrameRate                25.0  
#define _SupportedThermalCamera_LODESTARL2_FrameRate                25.0  

// -------------------- Supporterede IR Kamera Pool 1 Reference Macroer --------------------- //

// Specifikke Supporterede Termiske kamera pool 1 konfiguration macroer
#define _SupporteredeThermalCameraPool1_FrameWidthPixelOffset       0  
#define _SupporteredeThermalCameraPool1_FrameHeightPixelOffset      0 

// IR camera temperature range macros - for supported camera pool 1
#define _IRCameraPool1_TemperatureRange_HighRangeREG           0x8021
#define _IRCameraPool1_TemperatureRange_LowRangeREG            0x8020

// Internal IR camera configuration parameter addresses - for supported camera pool 1
#define _IRCameraPool1_ConfigParameterAddress_TempCorrREG      0x0000     
#define _IRCameraPool1_ConfigParameterAddress_ReflTempREG      0x0004      
#define _IRCameraPool1_ConfigParameterAddress_AmbTempREG       0x0008        
#define _IRCameraPool1_ConfigParameterAddress_HumidityREG      0x000C       
#define _IRCameraPool1_ConfigParameterAddress_EmissivityREG    0x0010         
#define _IRCameraPool1_ConfigParameterAddress_DistanceREG      0x0014  

// IR Camera Kalibrerings Register Kommando - For Supporterede Kamera Pool 1
#define _IRCameraPool1_NUCCalibrationCommand                   0x8000

// -------------------- Supporterede IR Kamera Pool 2 Reference Macroer --------------------- //

// Spesifikke Supporterede Termiske kamera pool 2 konfiguration macroer
#define _SupporteredeThermalCameraPool2_SensorWidthWithThermalData       256  
#define _SupporteredeThermalCameraPool2_SensorHeightWithThermalData      384  
#define _SupporteredeThermalCameraPool2_FrameWidthPixelOffset            0  
#define _SupporteredeThermalCameraPool2_FrameHeightPixelOffset           192  

// -------------------- Supporterede IR Kamera Pool 3 Reference Macroer --------------------- //

// Specifikke Supporterede Termiske kamera pool 3 konfiguration macroer
#define _SupporteredeThermalCameraPool3_FrameWidthPixelOffset       0  
#define _SupporteredeThermalCameraPool3_FrameHeightPixelOffset      0 

// IR camera temperature range macros - for supported camera pool 3
#define _IRCameraPool3_TemperatureRange_HighRangeREG           0x8021
#define _IRCameraPool3_TemperatureRange_LowRangeREG            0x8020

// Internal IR camera configuration parameter addresses - for supported camera pool 3
#define _IRCameraPool3_ConfigParameterAddress_TempCorrREG      0x0000     
#define _IRCameraPool3_ConfigParameterAddress_ReflTempREG      0x0004      
#define _IRCameraPool3_ConfigParameterAddress_AmbTempREG       0x0008        
#define _IRCameraPool3_ConfigParameterAddress_HumidityREG      0x000C       
#define _IRCameraPool3_ConfigParameterAddress_EmissivityREG    0x0010         
#define _IRCameraPool3_ConfigParameterAddress_DistanceREG      0x0014  

// IR Camera Kalibrerings Register Kommando - For Supporterede Kamera Pool 3
#define _IRCameraPool3_NUCCalibrationCommand                   0x8000

// -------------------- Supporterede IR Kamera Pool 4 Reference Macroer --------------------- //

// Spesifikke Supporterede Termiske kamera pool 4 konfiguration macroer
#define _SupporteredeThermalCameraPool4_SensorWidthWithThermalData       256  
#define _SupporteredeThermalCameraPool4_SensorHeightWithThermalData      386 
#define _SupporteredeThermalCameraPool4_FrameWidthPixelOffset            0  
#define _SupporteredeThermalCameraPool4_FrameHeightPixelOffset           194  

// ------------------- Supported IR Camera Device Names & Manufacturers -------------------- //

// Supported thermal camera model names ->
static std::vector<std::string> SupportedCamerasModelNames = { "Snapshot Analysis Mode",
                                                               "Recording Analysis Mode", 
                                                               "InfiRay T2L",
                                                               "InfiRay T2L V2",
                                                               "InfiRay T2-Search",
                                                               "InfiRay T2-Search V2",
                                                               "InfiRay T2S+", 
                                                               "InfiRay T2S+ V2",
                                                               "InfiRay T2Pro And T2SPro", 
                                                               "InfiRay T2Pro And T2SPro V2",
                                                               "InfiRay T3-Search", 
                                                               "InfiRay T3S", 
                                                               "InfiRay T3Pro", 
                                                               "InfiRay P2",
                                                               "InfiRay Or Thermal Master P2Pro",
                                                               "InfiRay DV-DL13", 
                                                               "InfiRay S0 Series", 
                                                               "InfiRay Tiny1-C",
                                                               "Thermal Master P2",
                                                               "HTI HT-301",
                                                               "UNI-T UTi260M",
                                                               "TOPDON TC001 or TS001", 
                                                               "TOPDON TC002 or TC003",
                                                               "Victor 328B", 
                                                               "LODESTAR L2"};

// Supported camera device names - InfiRay T2L - supported pool 1
static std::vector<std::string> InfiRayT2LDeviceNames = { "T2L-A4L", "T2L-A6L", "T2L-A8L", "T2L", "T2L-A4L_R", "T2L-A4L_A", "T2L-A4L_C" };

// Supported camera device names - InfiRay T2L V2 - supported pool 3
static std::vector<std::string> InfiRayT2LV2DeviceNames = { "T2L-A4L", "T2L-A6L", "T2L-A8L", "T2L_V2", "T2L", "T2L_A2", "T2L-A4L_A2", "T2L-A4L_V2" };

// Supported camera device names - InfiRay T2-Search - supported pool 1
static std::vector<std::string> InfiRayT2SearchDeviceNames = { "T2-Search", "T2", "T2S"};

// Supported camera device names - InfiRay T2-Search V2 - supported pool 3
static std::vector<std::string> InfiRayT2SearchV2DeviceNames = { "T2_V2", "T2-Search", "T2", "T2S", "T2_A2", "T2S_A2" };

// Supported camera device names - InfiRay T2S+ - supported pool 1  
static std::vector<std::string> InfiRayT2SpDeviceNames = { "T2S+", "T2Sp", "T2S+_A", "T2S+_C", "T2S+_R", "T2SPro" };

// Supported camera device names - InfiRay T2S+ V2 - supported pool 3  
static std::vector<std::string> InfiRayT2SpV2DeviceNames = { "T2S+_V2", "T2S+_A", "T2S+_A2", "T2S+", "T2Sp", "T2SPro" };

// Supported camera device names - InfiRay T2Pro - supported pool 1
static std::vector<std::string> InfiRayT2ProDeviceNames = { "T2Pro", "T2+", "T2p", "T2P", "T2SPro" };

// Supported camera device names - InfiRay T2Pro V2 - supported pool 3
static std::vector<std::string> InfiRayT2ProV2DeviceNames = { "T2Pro_V2", "T2Pro_A1", "T2Pro_A2", "T2Pro", "T2pro", "T2P", "T2SPro"};

// Supported camera device names - InfiRay T3-Search - supported pool 1
static std::vector<std::string> InfiRayT3SearchDeviceNames = { "T3-Search", "T3" };

// Supported camera device names - InfiRay T3S - supported pool 1
static std::vector<std::string> InfiRayT3SDeviceNames = { "Xtherm-T3S", "T3S-A68", "T3S-A13", "T3S", "T3S-A13_A", "T3S-A13_C", "T3S-A13_R" };

// Supported camera device names - InfiRay T3Pro - supported pool 1
static std::vector<std::string> InfiRayT3ProDeviceNames = { "T3Pro-A13", "T3Pro-A68", "T3Pro", "T3Pro-A13_C", "T3Pro-A13_A", "T3Pro-A13_R" };

// Supported camera device names - InfiRay P2 - supported pool 2
static std::vector<std::string> InfiRayP2DeviceNames = { "USB Camera", "Camera" };

// Supported camera device names - Thermal Master P2 - supported pool 4
static std::vector<std::string> ThermalMasterP2DeviceNames = { "Camera" };

// Supported camera device names - InfiRay P2Pro - supported pool 2 
static std::vector<std::string> InfiRayP2ProDeviceNames = { "USB Camera", "Camera" };

// Supported camera device names - InfiRay DV-DL13 - supported pool 1
static std::vector<std::string> InfiRayDVDL13DeviceNames = { "VirtualBox Webcam - DV-DL13", "DV-DL13" };

// Supported camera device names - InfiRay S0 Series - supported pool 1
static std::vector<std::string> InfiRayS0SeriesDeviceNames = { "S0-90W", "S0-40", "S0-68", "S0-90" };

// Supported camera device names - InfiRay Tiny1-C - supported pool 2
static std::vector<std::string> InfiRayTiny1CDeviceNames = { "USB Camera", "Tiny1C" };

// Supported camera device names - HTI HT-301 - supported pool 1
static std::vector<std::string> HTIHT301DeviceNames = { "T3", "T3-317-13", "T3-317-68", "HT-301"};

// Supported camera device names - UNI-T UTi260M - supported pool 2
static std::vector<std::string> UNITUTi260MDeviceNames = { "USB Camera" };

// Supported camera device names - TOPDON TC001 - supported pool 2
static std::vector<std::string> TOPDONTC001DeviceNames = { "USB Camera", "TC001" };

// Supported camera device names - TOPDON TC002 - supported pool 2
static std::vector<std::string> TOPDONTC002DeviceNames = { "USB Camera", "TC002", "TC003"};

// Supported camera device names - Victor 328B - supported pool 2
static std::vector<std::string> Victor328BDeviceNames = { "USB Camera" };

// Supported camera device names - LODESTAR L2 - supported pool 2
static std::vector<std::string> LODESTARL2DeviceNames = { "USB Camera" };

// ------------------------------------------------------------------------------------------ //

#endif /* RMH_SupportedIRCameras_Resources_H */