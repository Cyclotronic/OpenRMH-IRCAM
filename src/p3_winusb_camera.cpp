/*
 *  p3_winusb_camera.cpp
 *
 *  See p3_winusb_camera.h for background.
 *
 *  Protocol notes (from a USB capture of the vendor "Thermal Master" app talking to a real P3):
 *
 *   - The device is composite: interface 0 is the vendor control interface WinUSB is bound to
 *     (via P3.inf); interface 1 carries the video and only exposes its bulk endpoints (0x81 IN,
 *     0x02 OUT) in alternate setting 1 - alternate setting 0 has none.
 *
 *   - Vendor commands are sent as a control transfer OUT (bmRequestType 0x41, bRequest 0x20,
 *     wValue 0, wIndex 0) with an 18-byte payload:
 *       [0-1]   opcode (big endian pair, e.g. 0x01 0x01 = read info string, 0x01 0x2f = start
 *               stream, 0x01 0x36 = shutter calibration trigger)
 *       [2]     0x81 for every command seen except the shutter trigger, which uses 0x43 here
 *       [3]     0x00 (constant)
 *       [4]     parameter id for info-string reads; 0x00 (unused) for start-stream/shutter
 *       [5-11]  reserved / write payload (unused here - only read commands are needed)
 *       [12-15] expected response length, uint32 little-endian (0 for start-stream and the
 *               shutter trigger - both are fire-and-forget, no data phase)
 *       [16-17] CRC16/XMODEM (poly 0x1021, init 0x0000) of bytes [0-15], little-endian
 *     For a command with a non-zero expected length the device answers with a short interspersed
 *     status/poll pattern before handing back the actual payload on a control transfer IN of that
 *     length, followed by a trailing 1-byte poll. A zero-length command (start-stream, shutter
 *     trigger) gets only the first 1-byte poll and nothing else - sending the usual trailing poll
 *     after one of these anyway was tried and appeared to interfere with the command actually
 *     being carried out (start-stream/shutter would partially "work" - e.g. the shutter command
 *     would still freeze the image - without the camera doing the rest of what the command asks).
 *
 *   - Sending the start-stream command (0x01 0x2f) then switching interface 1 to alternate
 *     setting 1 makes the camera start pushing raw YUY2 frames on bulk endpoint 0x81: one 256 x
 *     386 (197632-byte) buffer per frame, preceded by two small 12-byte packets that this code
 *     discards (they fall within the region Pool 4/5 already skips - see below - so their content
 *     never reaches the screen either way). The 197632-byte block itself has no extra header.
 *
 *   - This frame is fed into the existing "Pool 4" pipeline (Thermal Master P2) in
 *     RMH_ThermalCameraSupport_Library.cpp, which is where the rest of the layout is actually
 *     handled: Pool 4's own FrameHeightPixelOffset constant is 194 (not 0), so display starts 194
 *     rows into the 386-row buffer, using the last 192 rows as the image and the first 194 as
 *     something else entirely (unexamined - it is simply skipped). Within that display-start row,
 *     the first 6 pixels read as a fixed near-zero value; RMH_ThermalCameraSupport_Library.cpp
 *     sets Pool 5's FrameWidthPixelOffset to 6 to skip them too, which pushes the read window 6
 *     pixels past the end of this 197632-byte buffer - handled below by duplicating the last 6
 *     real pixels into that trailing space.
 *
 *   - WinUsb_Initialize requires the device handle to have been opened with FILE_FLAG_OVERLAPPED,
 *     which in turn means every WinUsb_ControlTransfer/ReadPipe call must be given a real OVERLAPPED
 *     structure and have its completion waited for (see waitOverlapped()) - passing NULL for the
 *     OVERLAPPED parameter on such a handle makes the call return immediately with ERROR_IO_PENDING
 *     without ever surfacing an error, which looks like the command was silently dropped.
 *
 */

#include "p3_winusb_camera.h"

#include <windows.h>
#include <setupapi.h>
#include <winusb.h>
#include <cstring>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "winusb.lib")

namespace {

    // Device interface GUID the vendor's WinUSB driver package (P3.inf) registers for the device.
    const GUID P3_DEVICE_INTERFACE_GUID = { 0x80F70370, 0x086B, 0xC345, { 0x5A, 0x54, 0x59, 0xDE, 0x45, 0x19, 0xAF, 0x2A } };

    const UCHAR P3_STREAM_ENDPOINT = 0x81;
    const unsigned long P3_FRAME_WIDTH = 256;
    const unsigned long P3_FRAME_HEIGHT = 386;
    const unsigned long P3_FRAME_SIZE_BYTES = P3_FRAME_WIDTH * P3_FRAME_HEIGHT * 2; // YUY2, 197632 bytes

}

P3WinUsbCamera::P3WinUsbCamera() :
    m_deviceHandle(NULL),
    m_ctrlInterface(NULL),
    m_streamInterface(NULL),
    m_ctrlEvent(NULL),
    m_streamEvent(NULL),
    m_opened(false),
    m_capturing(false) {
}

P3WinUsbCamera::~P3WinUsbCamera() {

    close();

}

unsigned short P3WinUsbCamera::crc16Xmodem(const unsigned char* data, size_t len) {

    unsigned short crc = 0x0000;

    for (size_t i = 0; i < len; i++) {

        crc ^= (unsigned short)(data[i] << 8);

        for (int bit = 0; bit < 8; bit++) {

            if (crc & 0x8000) { crc = (unsigned short)((crc << 1) ^ 0x1021); }
            else { crc = (unsigned short)(crc << 1); }

        }

    }

    return crc;

}

bool P3WinUsbCamera::waitOverlapped(int apiResult, void* overlapped, unsigned long* transferred, unsigned long timeoutMs) {

    if (apiResult) { return true; }

    if (GetLastError() != ERROR_IO_PENDING) { return false; }

    LPOVERLAPPED ov = (LPOVERLAPPED)overlapped;

    // Bounded wait: a request the camera never answers must not hang the caller (and, since these
    // calls run on the UI/capture thread, must not freeze the whole application) - cancel it instead.
    DWORD waitResult = WaitForSingleObject(ov->hEvent, timeoutMs);

    if (waitResult != WAIT_OBJECT_0) {

        CancelIoEx((HANDLE)m_deviceHandle, ov);
        GetOverlappedResult((HANDLE)m_deviceHandle, ov, transferred, TRUE);
        return false;

    }

    return GetOverlappedResult((HANDLE)m_deviceHandle, ov, transferred, FALSE) != FALSE;

}

bool P3WinUsbCamera::sendCommand(unsigned char opcodeHi, unsigned char opcodeLo, unsigned char paramId, unsigned long expectedResponseLen, unsigned char* responseOut, unsigned char byte2) {

    if (!m_opened) { return false; }

    unsigned char payload[18] = { 0 };
    payload[0] = opcodeHi;
    payload[1] = opcodeLo;
    payload[2] = byte2;
    payload[3] = 0x00;
    payload[4] = paramId;
    payload[12] = (unsigned char)(expectedResponseLen & 0xFF);
    payload[13] = (unsigned char)((expectedResponseLen >> 8) & 0xFF);
    payload[14] = (unsigned char)((expectedResponseLen >> 16) & 0xFF);
    payload[15] = (unsigned char)((expectedResponseLen >> 24) & 0xFF);

    unsigned short crc = crc16Xmodem(payload, 16);
    payload[16] = (unsigned char)(crc & 0xFF);
    payload[17] = (unsigned char)((crc >> 8) & 0xFF);

    WINUSB_SETUP_PACKET setup;
    ZeroMemory(&setup, sizeof(setup));
    setup.RequestType = 0x41;
    setup.Request = 0x20;
    setup.Value = 0;
    setup.Index = 0;
    setup.Length = sizeof(payload);

    OVERLAPPED ov;
    ULONG transferred = 0;

    ZeroMemory(&ov, sizeof(ov));
    ov.hEvent = (HANDLE)m_ctrlEvent;
    ResetEvent((HANDLE)m_ctrlEvent);

    BOOL ok = WinUsb_ControlTransfer((WINUSB_INTERFACE_HANDLE)m_ctrlInterface, setup, payload, sizeof(payload), &transferred, &ov);
    if (!waitOverlapped(ok, &ov, &transferred)) {

        m_lastError = "WinUsb_ControlTransfer (command out) failed (GetLastError=" + std::to_string(GetLastError()) + ")";
        return false;

    }

    // The device answers a write with an exact, fixed-length sequence of control-IN reads:
    // a 1-byte "ready" poll, then the actual expectedResponseLen-byte payload, then a trailing
    // 1-byte poll. Requesting anything other than these exact lengths goes unanswered.
    unsigned char scratch[64];
    unsigned char pollByte = 0;

    setup.RequestType = 0xC1;
    setup.Request = 0x22; // "ready" poll uses its own vendor request code, distinct from the write (0x20) and read (0x21)
    setup.Length = 1;
    ULONG pollReceived = 0;
    ZeroMemory(&ov, sizeof(ov));
    ov.hEvent = (HANDLE)m_ctrlEvent;
    ResetEvent((HANDLE)m_ctrlEvent);

    // The "ready" poll can take much longer than a normal command turnaround when the camera is
    // physically doing something first (e.g. a shutter calibration trigger: a real capture showed
    // ~714ms before this specific poll answered, versus sub-millisecond for an ordinary read) -
    // give it a generous timeout so a slow-but-genuine response isn't cancelled as if it were a
    // real failure (cancelling here appeared to cut the shutter action short: no audible click).
    ok = WinUsb_ControlTransfer((WINUSB_INTERFACE_HANDLE)m_ctrlInterface, setup, &pollByte, 1, &pollReceived, &ov);
    if (!waitOverlapped(ok, &ov, &pollReceived, 3000)) {

        m_lastError = "WinUsb_ControlTransfer (ready poll) failed (GetLastError=" + std::to_string(GetLastError()) + ")";
        return false;

    }

    if (expectedResponseLen > 0 && expectedResponseLen <= sizeof(scratch)) {

        setup.Request = 0x21; // read response data
        setup.Length = (USHORT)expectedResponseLen;
        ULONG received = 0;
        ZeroMemory(&ov, sizeof(ov));
        ov.hEvent = (HANDLE)m_ctrlEvent;
        ResetEvent((HANDLE)m_ctrlEvent);

        ok = WinUsb_ControlTransfer((WINUSB_INTERFACE_HANDLE)m_ctrlInterface, setup, scratch, (ULONG)expectedResponseLen, &received, &ov);
        if (!waitOverlapped(ok, &ov, &received) || received != expectedResponseLen) {

            m_lastError = "WinUsb_ControlTransfer (response) failed or short (GetLastError=" + std::to_string(GetLastError()) + ")";
            return false;

        }

        if (responseOut != NULL) { memcpy(responseOut, scratch, received); }

        // Trailing 1-byte poll - only sent for commands that had a data-carrying response above
        // (info reads). A real capture of a 0-response command (the shutter trigger) showed no
        // trailing poll at all - just this one "ready" poll, then nothing else on the control
        // pipe. Sending an extra, unexpected poll after a 0-response command appeared to interfere
        // with the device actually carrying out that command (the shutter never audibly fired).
        unsigned char trailByte = 0;
        ULONG trailReceived = 0;
        ZeroMemory(&ov, sizeof(ov));
        ov.hEvent = (HANDLE)m_ctrlEvent;
        ResetEvent((HANDLE)m_ctrlEvent);
        setup.Request = 0x22;
        setup.Length = 1;
        ok = WinUsb_ControlTransfer((WINUSB_INTERFACE_HANDLE)m_ctrlInterface, setup, &trailByte, 1, &trailReceived, &ov);
        waitOverlapped(ok, &ov, &trailReceived);

    }

    return true;

}

bool P3WinUsbCamera::open() {

    if (m_opened) { return true; }

    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&P3_DEVICE_INTERFACE_GUID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {

        m_lastError = "SetupDiGetClassDevs failed";
        return false;

    }

    SP_DEVICE_INTERFACE_DATA interfaceData;
    ZeroMemory(&interfaceData, sizeof(interfaceData));
    interfaceData.cbSize = sizeof(interfaceData);

    bool found = false;
    std::wstring devicePath;

    if (SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &P3_DEVICE_INTERFACE_GUID, 0, &interfaceData)) {

        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetailW(deviceInfoSet, &interfaceData, NULL, 0, &requiredSize, NULL);

        if (requiredSize > 0) {

            PSP_DEVICE_INTERFACE_DETAIL_DATA_W detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)malloc(requiredSize);
            detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

            if (SetupDiGetDeviceInterfaceDetailW(deviceInfoSet, &interfaceData, detailData, requiredSize, NULL, NULL)) {

                devicePath = detailData->DevicePath;
                found = true;

            }

            free(detailData);

        }

    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    if (!found) {

        m_lastError = "No Thermal Master P3 (WinUSB) device found";
        return false;

    }

    // FILE_FLAG_OVERLAPPED is required for WinUsb_Initialize to accept the handle; every WinUsb
    // I/O call against it is then completed synchronously here via waitOverlapped().
    HANDLE fileHandle = CreateFileW(devicePath.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

    if (fileHandle == INVALID_HANDLE_VALUE) {

        m_lastError = "CreateFile on the P3 device path failed (GetLastError=" + std::to_string(GetLastError()) + ")";
        return false;

    }

    WINUSB_INTERFACE_HANDLE ctrlHandle = NULL;
    if (!WinUsb_Initialize(fileHandle, &ctrlHandle)) {

        m_lastError = "WinUsb_Initialize failed (GetLastError=" + std::to_string(GetLastError()) + ")";
        CloseHandle(fileHandle);
        return false;

    }

    WINUSB_INTERFACE_HANDLE streamHandle = NULL;
    if (!WinUsb_GetAssociatedInterface(ctrlHandle, 0, &streamHandle)) {

        m_lastError = "WinUsb_GetAssociatedInterface failed (GetLastError=" + std::to_string(GetLastError()) + ")";
        WinUsb_Free(ctrlHandle);
        CloseHandle(fileHandle);
        return false;

    }

    // A short, bounded timeout so a stalled bulk read cannot hang the capture thread forever.
    ULONG timeoutMs = 500;
    WinUsb_SetPipePolicy(streamHandle, P3_STREAM_ENDPOINT, PIPE_TRANSFER_TIMEOUT, sizeof(timeoutMs), &timeoutMs);

    HANDLE ctrlEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    HANDLE streamEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    m_deviceHandle = fileHandle;
    m_ctrlInterface = ctrlHandle;
    m_streamInterface = streamHandle;
    m_ctrlEvent = ctrlEvent;
    m_streamEvent = streamEvent;
    m_opened = true;
    m_capturing = false;

    return true;

}

bool P3WinUsbCamera::isOpened() {

    return m_opened;

}

bool P3WinUsbCamera::close() {

    if (!m_opened) { return true; }

    stopCapture();

    if (m_streamInterface != NULL) { WinUsb_Free((WINUSB_INTERFACE_HANDLE)m_streamInterface); m_streamInterface = NULL; }
    if (m_ctrlInterface != NULL) { WinUsb_Free((WINUSB_INTERFACE_HANDLE)m_ctrlInterface); m_ctrlInterface = NULL; }
    if (m_deviceHandle != NULL) { CloseHandle((HANDLE)m_deviceHandle); m_deviceHandle = NULL; }
    if (m_ctrlEvent != NULL) { CloseHandle((HANDLE)m_ctrlEvent); m_ctrlEvent = NULL; }
    if (m_streamEvent != NULL) { CloseHandle((HANDLE)m_streamEvent); m_streamEvent = NULL; }

    m_opened = false;

    return true;

}

bool P3WinUsbCamera::checkDisconnection() {

    if (!m_opened) { return true; }

    UCHAR altSetting = 0;
    if (!WinUsb_GetCurrentAlternateSetting((WINUSB_INTERFACE_HANDLE)m_streamInterface, &altSetting)) {

        return true;

    }

    return false;

}

bool P3WinUsbCamera::startCapture() {

    if (!m_opened) { return false; }
    if (m_capturing) { return true; }

    // Switch the streaming interface into its active alternate setting FIRST. A capture of the
    // vendor app against a freshly plugged-in (cold) camera showed control commands going
    // completely unanswered for ~9 seconds until, in this order: (1) SET_INTERFACE to alternate
    // setting 1, (2) a device-level vendor command (bmRequestType 0x40, bRequest 0xEE, wIndex 1,
    // no data) - only after both of those did the very next command finally get a response. Doing
    // our commands before this was a chicken-and-egg deadlock: nothing ever unlocked the device.
    if (!WinUsb_SetCurrentAlternateSetting((WINUSB_INTERFACE_HANDLE)m_streamInterface, 1)) {

        m_lastError = "WinUsb_SetCurrentAlternateSetting(1) failed (GetLastError=" + std::to_string(GetLastError()) + ")";
        return false;

    }

    // The device-level "unlock" command observed right after SET_INTERFACE. Best-effort: its
    // response (if any) is not checked, matching what the capture showed (no data stage at all).
    {
        WINUSB_SETUP_PACKET unlockSetup;
        ZeroMemory(&unlockSetup, sizeof(unlockSetup));
        unlockSetup.RequestType = 0x40;
        unlockSetup.Request = 0xEE;
        unlockSetup.Value = 0;
        unlockSetup.Index = 1;
        unlockSetup.Length = 0;

        OVERLAPPED unlockOv;
        ZeroMemory(&unlockOv, sizeof(unlockOv));
        unlockOv.hEvent = (HANDLE)m_ctrlEvent;
        ResetEvent((HANDLE)m_ctrlEvent);

        ULONG unlockTransferred = 0;
        BOOL unlockOk = WinUsb_ControlTransfer((WINUSB_INTERFACE_HANDLE)m_ctrlInterface, unlockSetup, NULL, 0, &unlockTransferred, &unlockOv);
        waitOverlapped(unlockOk, &unlockOv, &unlockTransferred);
    }

    // Send the vendor "start stream" command (opcode 0x01 0x2f, param 0x00, 1-byte ACK). A cold
    // camera can still take a few seconds after the unlock above before it actually answers, so
    // retry with patience rather than failing on the first attempt.
    bool started = false;
    for (int attempt = 0; attempt < 20 && !started; attempt++) {

        started = sendCommand(0x01, 0x2f, 0x00, 1, NULL);
        if (!started) { Sleep(500); }

    }

    if (!started) {

        m_lastError = "Start-stream command failed: " + m_lastError;
        return false;

    }

    m_capturing = true;

    return true;

}

bool P3WinUsbCamera::triggerShutterCalibration() {

    // Opcode 0x01 0x36, byte2 0x43 (not the usual 0x81 - this command's payload layout differs
    // from the info-read/start-stream family), param 0x00, no response expected. Confirmed by a
    // USB capture of the vendor app's manual shutter calibration button, triggered several times:
    // this exact 18-byte payload (same CRC each time, since the payload is constant) appeared once
    // per trigger with no other command in between. An earlier version of this call wrongly used
    // the default byte2 (0x81) with 0x43 as the param instead, producing a different (wrong) 18-byte
    // payload that the camera accepted enough to freeze the image but never audibly fired the shutter.
    return sendCommand(0x01, 0x36, 0x00, 0, NULL, 0x43);

}

bool P3WinUsbCamera::setTemperatureRange(bool highRange) {

    // Opcode 0x01 0x2f (the same opcode used for start-stream) with byte2 0x41 (not the usual
    // 0x81, nor the shutter trigger's 0x43) and param 0x00/0x01, no response expected. Confirmed by
    // three isolated USB captures of the vendor app's range menu, each with exactly one 18-byte
    // command on the wire: switching to the 100-600C range sent param 0x00; switching to the
    // -20-150C range sent param 0x01; switching to "Auto" sent the exact same command as -20-150C,
    // meaning Auto is a vendor-app-side display feature over the low-range sensor data, not a
    // separate camera hardware mode - there is nothing to send this camera for it.
    return sendCommand(0x01, 0x2f, highRange ? 0x00 : 0x01, 0, NULL, 0x41);

}

bool P3WinUsbCamera::stopCapture() {

    if (!m_opened || !m_capturing) { return true; }

    WinUsb_SetCurrentAlternateSetting((WINUSB_INTERFACE_HANDLE)m_streamInterface, 0);

    m_capturing = false;

    return true;

}

bool P3WinUsbCamera::getFrame(unsigned char* frame, int* numOfBytes, bool onlyGetNewFrame, int maxBytes) {

    if (!m_opened || !m_capturing || frame == NULL) { return false; }
    if ((unsigned long)maxBytes < P3_FRAME_SIZE_BYTES + 12) {

        m_lastError = "Frame buffer too small";
        return false;

    }

    // Each frame cycle on the wire is [12 bytes][12 bytes][197632-byte block]. Both small packets
    // are discarded here: RMH_ThermalCameraSupport_Library.cpp's Pool 4 constants (which Pool 5
    // reuses) already skip the first 194 rows of the 197632-byte block via FrameHeightPixelOffset
    // before the image is ever displayed, so nothing in that skipped region - including whichever
    // of these two packets might belong there - reaches the screen. Earlier attempts to "fix" the
    // first bytes of this block were consequently inert (they modified data that is never read).
    for (int attempt = 0; attempt < 12; attempt++) {

        OVERLAPPED ov;
        ZeroMemory(&ov, sizeof(ov));
        ov.hEvent = (HANDLE)m_streamEvent;
        ResetEvent((HANDLE)m_streamEvent);

        ULONG bytesRead = 0;
        BOOL ok = WinUsb_ReadPipe((WINUSB_INTERFACE_HANDLE)m_streamInterface, P3_STREAM_ENDPOINT, frame, P3_FRAME_SIZE_BYTES, &bytesRead, &ov);

        if (!waitOverlapped(ok, &ov, &bytesRead)) {

            m_lastError = "WinUsb_ReadPipe failed (GetLastError=" + std::to_string(GetLastError()) + ")";
            return false;

        }

        if (bytesRead == 12) { continue; }

        if (bytesRead == P3_FRAME_SIZE_BYTES) {

            // The first 6 pixels of the row where display starts (byte offset 99328 = word 49664 =
            // row 194, the point RMH_ThermalCameraSupport_Library.cpp's Pool 4/5 FrameHeightPixelOffset
            // skips to) read as a fixed near-zero value - not fixed up here; Pool 5's own
            // FrameWidthPixelOffset (set to 6) skips past them entirely instead. That shifts the
            // conversion's read window 6 pixels past the true end of this buffer too, into bytes
            // this function never wrote - duplicate the last 6 real pixels into that trailing
            // region so the conversion reads valid (if slightly repeated) data instead of whatever
            // was left over in the caller's buffer from an earlier frame or its zero-initialization.
            memcpy(frame + P3_FRAME_SIZE_BYTES, frame + P3_FRAME_SIZE_BYTES - 12, 12);

            if (numOfBytes != NULL) { *numOfBytes = (int)bytesRead; }
            return true;

        }

    }

    m_lastError = "Timed out waiting for a full video frame";
    return false;

}

int P3WinUsbCamera::getWidth() {

    return (int)P3_FRAME_WIDTH;

}

int P3WinUsbCamera::getHeight() {

    return (int)P3_FRAME_HEIGHT;

}

double P3WinUsbCamera::getFPS() {

    return 25.0;

}

std::string P3WinUsbCamera::getLastError() {

    return m_lastError;

}
