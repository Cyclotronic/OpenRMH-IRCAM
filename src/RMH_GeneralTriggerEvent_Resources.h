
/*
 *  RMH_GeneralTriggerEvent_Resources.h
 *
 *  Author: Rune Mark Hansen
 *  Date: April 2024
 *
 */

#pragma once

// RMH_GeneralTriggerEvent_Resources.h
#ifndef RMH_GeneralTriggerEvent_Resources_H 
#define RMH_GeneralTriggerEvent_Resources_H

// ------------ Available General And Periodic Trigger Event Resource Data ------------ //
 
// Maximum number of configurable periodic triggers
#define _MaxNumberOfConfigurablePeriodicTriggerEvents         5

// Periodic trigger event number macros
#define _PeriodicTriggerEvent_1                               0
#define _PeriodicTriggerEvent_2                               1
#define _PeriodicTriggerEvent_3                               2
#define _PeriodicTriggerEvent_4                               3
#define _PeriodicTriggerEvent_5                               4

// Periodic trigger event function index macros
#define _TriggerEventFunction_None							  0    
#define _TriggerEventFunction_StartDataLogging                1                
#define _TriggerEventFunction_StopDataLogging                 2      
#define _TriggerEventFunction_StartVideoRecording             3 
#define _TriggerEventFunction_StopVideoRecording              4 
#define _TriggerEventFunction_SaveSnapshot                    5 
#define _TriggerEventFunction_SaveFullFrameTempData           6

// Periodic trigger event function type name strings ->
static std::vector<std::string> PeriodicTriggerEventFuncStrings = { "None",
																    "Start Data Logging",
																    "Stop Data Logging",
																    "Start Video Recording",
																    "Stop Video Recording",
																    "Save Snapshot", 
																	"Save Frame Temp Data"};

// ------------------------------------------------------------------------------------------ //

#endif /* RMH_GeneralTriggerEvent_Resources_H */
