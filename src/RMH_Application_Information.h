/*
 *  RMH_Application_Information.h
 *
 *  Author: Rune Mark Hansen
 *  Date: November 2022
 *
 */

#pragma once

// RMH_Application_Information.h
#ifndef RMH_Application_Information_H 
#define RMH_Application_Information_H

// Discord server URL link string of the application
static std::string DiscordServerLinkAddress = "https://discord.gg/3zq3zXFA8B";

// Preset file name of the application ->
static std::string Application_PresetFileName = "IRCAMApplicationPreset.txt";

// Application information ->
static std::string Application_Name = "IRCAM Thermal Viewer";
static std::string Application_VersionNumber = "3.0.0";
static std::string Application_Revision = "";
static std::string Application_VersionMonth = "September";
static std::string Application_VersionYear = "2026";

// GUI information and version string (shown at the top of the GUI)
static std::string ApplicationInformationString = Application_Name + " - [Developed & Written By: Rune Mark Glendorf, " + Application_VersionMonth + " " + Application_VersionYear + " - Version " + Application_VersionNumber + Application_Revision + "]";

#endif /* RMH_Application_Information_H */

