#ifndef _TRANSPOT_
#define _TRANSPOT_
#undef UNICODE
#undef _UNICODE
#include <sys/stat.h>
#include "hidapi.h"

#ifdef __cplusplus
extern "C" {
#endif
/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */
    void setDebugFlag();
    void resetDebugFlag();
/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

/*
    @@ The following macros are the maximum blocking duration when setting the background. When you use _INFINITE_TM, the main thread cannot unblock if the correct return signal
    is not received while setting the background. In other words, the main thread will keep waiting for the 'successful background setting signal.' Also, if you use this macro
    as the blocking duration, you need to be aware that in the current program, you cannot spawn a separate thread to listen for tranSportRead events, as this would conflict with
    the internal tranSportRead during background setting, potentially causing a deadlock

    @@ It is recommended to use different blocking durations for different models. The minimum blocking duration should not exceed 3000ms.
*/
#define _INFINITE_TM    (0xffffffff)
#define _SD293_TM       (5000)
#define _SD293V3_TM     (3000)
#define _SDN3_TM        (5000)
#define _SDN4_TM        (3000)
static unsigned long BLOCKTIME = _INFINITE_TM;
static unsigned long dualBackGroundDelay = 5;   // set background then sleep for 5ms.

// error code
enum ERROR_CODE
{
    UnKnown = 0x00000000,   // Unknown
    INSTANCE_NULL = 0x00000001,   // Transport instance does not exist
    HID_DEVICE_NULL = 0x00000002,   // USB device does not exist
    HID_READ_ERROR = 0x00000004,   // USB device read error, you can call hid_error() to check the most recent error
    HID_WRITE_ERROR = 0x00000008,   // USB device write error, you can call hid_error() to check the most recent error
    FILE_OPEN_ERROR = 0x00000010,   // File open failure
    KEY_RANGE_EXCEEDS = 0x00000020,   // Key cannot be set to out of 1 - 15
    KEY_FILE_SIZE_EXCEEDS = 0x00000040,   // Key file size exceeds the limit
    BGI_FILE_SIZE_EXCEEDS = 0x00000080,   // Background file size exceeds the limit
    FILE_FORMAT_NOT_JPEG = 0x00000100,   // File format is not JPEG
    JPEG_SIZE_MISMATCH_64X64 = 0x00000200,   // JPEG image size is not 64x64
    JPEG_SIZE_MISMATCH_100X100 = 0x00000400,   // JPEG image size is not 100x100
    JPEG_SIZE_MISMATCH_112X112 = 0x00000800,   // JPEG image size is not 112x112
    JPEG_SIZE_MISMATCH_176X112 = 0x00001000,   // JPEG image size is not 176x112
    JPEG_SIZE_MISMATCH_320X240 = 0x00002000,   // JPEG image size is not 320x240
    JPEG_SIZE_MISMATCH_800X480 = 0x00004000,   // JPEG image size is not 800x480
};
// You can just get the error code plus by ERROR_CODE
unsigned int getLastErrorCode();
// You can visually see the error message
void printLastErrorInfo();

typedef struct tranSport
{
    hid_device* handle;
}tranSport;

tranSport* tranSportInit();
void tranSportDestory(tranSport* self);
int tranSportOpen(tranSport* self, char* path);
unsigned char* tranSportGetInputReport(tranSport* self, int length);
int tranSportRead(tranSport* self, unsigned char* data, unsigned long length);
int tranSportWrite(tranSport* self, unsigned char* data, unsigned long length);
void tranSportFreeEnumerate(tranSport* self, struct hid_device_info* devs);
struct hid_device_info* tranSportEnumerate(tranSport* self, int vid, int pid);
int tranSportSetBrightness(tranSport* self, int percent);

/*
    @ new addition: @@ three set background function will block until it get the "OK" signal that's from transport read
                    @@ You should not call the refresh() function after setting the background, as the background setting
                    process already takes care of this for you. On older platforms, calling refresh() after setting the
                    background may cause a black screen, while this issue does not occur on newer platforms.

    @ note: tranSportSetBackgroundImg's buffer is opencv's cv::Mat.imagedata, can't not read from fread();
            you can not use tranSportSetBackgroundImg() directly, see streamdock293.c
*/
int tranSportSetBackgroundImg(tranSport* self, unsigned char* buffer, int bufferLen);
int tranSportSetBackgroundImgDualDevice(tranSport* self, const char* path);
/*
    @ usage example: For specific usage, refer to example.c(usage 4)
    @ note: You must first view the usage examples before writing it yourself, because the usage examples include various precautions,
            such as the format and size of the image being read, etc. The same applies to tranSportSetKeyImgDataDualDevice below.
            Reading these usage examples won't take you much time, as their function names represent their purpose, and they come with English comments.
    @ breif: set background with image data(file data, read from fread, not opencv data)
*/
int tranSportSetBackgroundImgDataDualDevice(tranSport* self, unsigned char* memoryByteBuffer, int bufferLen);

int tranSportSetKeyImg(tranSport* self, const char* path, int key);
int tranSportSetKeyImgData(tranSport* self, unsigned char* memoryByteBuffer, int bufferLen, int key);
int tranSportSetKeyImgDualDevice(tranSport* self, const char* path, int key);
//int tranSportSetKeyImgDataDualDevice(tranSport* self, const char* path, int key);
/*
    @ usage example: For specific usage, refer to example.c(usage 1 OR usage 2)
    @ breif: set key with image data
    @ params:
            @@ self: transport instance
            @@ memoryByteBuffer: Of course, the simplest method is to directly read the parameter from the file using fread, however, only ".jpg" formats are supported.
                    however, this memory cam come from an IplImage image instance, and this memory is consistent with the data you read into memory using fread() from a file.
                    If you want to conveniently call this interface, you can use the function saveImageToMemory() from "memorybytestream.h" to obtain this parameter.
            @@ bufferLen: you can get like parma memoryByteBuffer from function saveImageToMemory(args...) in "memorybytestream.h"
            @@ key: which key you control in mirabox
*/
int tranSportSetKeyImgDataDualDevice(tranSport* self, unsigned char* memoryByteBuffer, int bufferLen, int key);

int tranSportKeyClear(tranSport* self, int i);
int tranSportKeyAllClear(tranSport* self);
int tranSportWakeScreen(tranSport* self);
int tranSportRefresh(tranSport* self);
int tranSportDisconnected(tranSport* self);
void tranSportClose(tranSport* self);


#ifdef __cplusplus
}
#endif


#endif