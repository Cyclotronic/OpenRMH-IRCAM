%% Example On How To Read The CSV File From IRCAM Thermal Viewer

% In the following example the Data in the data logging file is:

% Sample,Time(ms),ROI 1 Max Temp
% 1,500,34.72890
% 2,1000,34.70929
% 3,1500,34.70929
% 4,2000,34.68968
% 5,2500,34.67007
% 6,3000,34.68968
% 7,3500,34.67007
% ................

% Clear Command Window
clc
% Clear Plots
clf

% Path for the Data Logging file
FileLocation = 'DataLogSession_12520519032023.txt';

% Read the data from the file, separated by each header description
FileData = readtable(FileLocation);

% Plot "ROI 1 Max Temp" Data
plot(FileData.ROI1MaxTemp);
grid on
grid minor