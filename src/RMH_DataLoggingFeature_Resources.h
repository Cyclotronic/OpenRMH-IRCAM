
/*
 *  RMH_DataLoggingFeature_Resources.h
 *
 *  Author: Rune Mark Hansen
 *  Date: January 2024
 *
 */

#pragma once

// RMH_DataLoggingFeature_Resources.h
#ifndef RMH_DataLoggingFeature_Resources_H 
#define RMH_DataLoggingFeature_Resources_H

// Data logging CSV delimiter index macros
#define _DataLoggingDelimiterIndex_Comma           0       
#define _DataLoggingDelimiterIndex_Semicolon       1          
#define _DataLoggingDelimiterIndex_Colon           2            
#define _DataLoggingDelimiterIndex_Space           3       
#define _DataLoggingDelimiterIndex_Tab             4 

// Available data logging CSV delimiters ->
static std::vector<std::string> DataLoggingCSVDataDelimiters = { "Comma", "Semicolon", "Colon", "Space", "Tab" };

#endif /* RMH_DataLoggingFeature_Resources_H */
