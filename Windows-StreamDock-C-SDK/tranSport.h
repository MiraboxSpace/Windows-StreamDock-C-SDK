#ifndef _TRANSPOT_
#define _TRANSPOT_
#undef UNICODE
#undef _UNICODE
#include "hidapi.h"
#include <sys/stat.h>

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
int tranSportSetBackgroundImg(tranSport* self, unsigned char* buffer, const int size);
int tranSportSetBackgroundImgDualDevice(tranSport* self, const char* path);
int tranSportSetKeyImg(tranSport* self, const char *path, int key);
int tranSportSetKeyImgDualDevice(tranSport* self, const char* path, int key); 
int tranSportSetKeyImgDataDualDevice(tranSport* self, char* path, int key);
int tranSportSetKeyImgData(tranSport* self, unsigned char* buffer, int bufferLen, int key, int width, int height);
int tranSportKeyClear(tranSport* self, int i);
int tranSportKeyAllClear(tranSport* self);
int tranSportWakeScreen(tranSport* self);
int tranSportRefresh(tranSport* self);
int tranSportDisconnected(tranSport* self);
void tranSportClose(tranSport* self);


#endif