#ifndef _TRANSPOT_
#define _TRANSPOT_
#undef UNICODE
#undef _UNICODE
#include <sys/stat.h>
#include "hidapi.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
    @@ The following macros are the maximum blocking duration when setting the background. When you use _INFINITE_TM, the main thread cannot unblock if the correct return signal
    is not received while setting the background. In other words, the main thread will keep waiting for the 'successful background setting signal.' Also, if you use this macro
    as the blocking duration, you need to be aware that in the current program, you cannot spawn a separate thread to listen for tranSportRead events, as this would conflict with
    the internal tranSportRead during background setting, potentially causing a deadlock

    @@ It is recommended to use different blocking durations for different models. The minimum blocking duration should not exceed 3000ms.
*/

#define _INFINITE_TM (0xffffffff)
#define _SD293_TM  (5000)
#define _SD293V3_TM  (3000)
#define _SDN3_TM  (5000)
#define _SDN4_TM  (3000)
static unsigned long BLOCKTIME = _SD293_TM;

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
int tranSportSetBackgroundImg(tranSport* self, unsigned char* buffer, const int size);
int tranSportSetBackgroundImgDualDevice(tranSport* self, const char* path);
/*
    @ usage example: For specific usage, refer to example.c(usage 4)
    @ note: You must first view the usage examples before writing it yourself, because the usage examples include various precautions,
            such as the format and size of the image being read, etc. The same applies to tranSportSetKeyImgDataDualDeviceEx below.
            Reading these usage examples won't take you much time, as their function names represent their purpose, and they come with English comments.
    @ breif: set background with image data(file data, read from fread, not opencv data)
*/
int tranSportSetBackgroundImgDataDualDevice(tranSport* self, unsigned char* memoryByteBuffer, int bufferLen);


int tranSportSetKeyImg(tranSport* self, const char* path, int key);
int tranSportSetKeyImgDualDevice(tranSport* self, const char* path, int key);
int tranSportSetKeyImgDataDualDevice(tranSport* self, const char* path, int key);
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
int tranSportSetKeyImgDataDualDeviceEx(tranSport* self, unsigned char* memoryByteBuffer, int bufferLen, int key);
int tranSportSetKeyImgData(tranSport* self, unsigned char* buffer, int bufferLen, int key, int width, int height);
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