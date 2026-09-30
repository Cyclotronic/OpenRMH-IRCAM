/*
 *  p3_winusb_camera.h
 *
 *  Raw WinUSB access to the Thermal Master P3 (VID_3474 PID_45A2).
 *  The camera has no standard USB Video Class interface - the vendor's own driver package
 *  (P3.inf) binds WinUSB to the device and its own app talks to it with a vendor-specific
 *  protocol over the default control pipe plus a bulk endpoint. This class reimplements just
 *  enough of that protocol - reverse engineered from a USB capture of the vendor app - to
 *  pull raw YUY2 frames, matching the exact frame layout already handled by "Pool 4"
 *  (Thermal Master P2) in RMH_ThermalCameraSupport_Library.cpp: 256 x 386, YUY2, no header.
 *
 */

#pragma once
#ifndef P3_WINUSB_CAMERA_H
#define P3_WINUSB_CAMERA_H

#include <string>

class P3WinUsbCamera {

public:

    P3WinUsbCamera();
    ~P3WinUsbCamera();

    // Finds and opens the P3 over WinUSB. Returns false if no P3 is connected.
    bool open();
    bool isOpened();
    bool close();
    bool checkDisconnection();

    // Starts/stops the vendor video stream (switches the streaming interface's alternate
    // setting and, on start, sends the vendor "start stream" command).
    bool startCapture();
    bool stopCapture();

    // Triggers the camera's shutter (NUC) calibration (opcode 0x01 0x36, param 0x43, fire-and-forget).
    bool triggerShutterCalibration();

    // Switches the camera's own hardware temperature range (opcode 0x01 0x2f, byte2 0x41,
    // param 0x00 = high range/100-600C, param 0x01 = low range/-20-150C), fire-and-forget.
    bool setTemperatureRange(bool highRange);

    // Reads one raw YUY2 frame (256 x 386 x 2 bytes) into "frame". Mirrors the signature of
    // DirectShowCamera::UVCCamera::getFrame() so call sites can branch on pool with minimal change.
    bool getFrame(unsigned char* frame, int* numOfBytes, bool onlyGetNewFrame, int maxBytes);

    int getWidth();
    int getHeight();
    double getFPS();

    std::string getLastError();

private:

    void* m_deviceHandle;      // HANDLE - opened with FILE_FLAG_OVERLAPPED, as WinUsb_Initialize requires
    void* m_ctrlInterface;     // WINUSB_INTERFACE_HANDLE - interface 0 (vendor control, WinUSB-bound)
    void* m_streamInterface;   // WINUSB_INTERFACE_HANDLE - interface 1 (bulk video, associated interface)
    void* m_ctrlEvent;         // HANDLE - reused OVERLAPPED completion event for control transfers
    void* m_streamEvent;       // HANDLE - reused OVERLAPPED completion event for bulk reads
    bool m_opened;
    bool m_capturing;
    std::string m_lastError;

    // byte2 defaults to 0x81, the constant seen in every info-read/start-stream command; the
    // shutter calibration command uses 0x43 there instead (and 0x00 for the param byte).
    bool sendCommand(unsigned char opcodeHi, unsigned char opcodeLo, unsigned char paramId, unsigned long expectedResponseLen, unsigned char* responseOut, unsigned char byte2 = 0x81);
    static unsigned short crc16Xmodem(const unsigned char* data, size_t len);

    // The device handle is opened for overlapped I/O (required by WinUsb_Initialize), so every
    // WinUsb_ControlTransfer/ReadPipe call must be given a real OVERLAPPED and its completion waited
    // for here - a call that finished synchronously already (apiResult == TRUE) is a no-op passthrough.
    bool waitOverlapped(int apiResult, void* overlapped, unsigned long* transferred, unsigned long timeoutMs = 300);
};

#endif
