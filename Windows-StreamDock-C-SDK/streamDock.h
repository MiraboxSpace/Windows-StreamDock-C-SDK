#ifndef _STREAMDOCK_
#define _STREAMDOCK_

#include "tranSport.h"
#include "hidapi.h"
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

typedef struct streamDock {
    // public
    tranSport* transport;
    struct hid_device_info* devInfo;
    unsigned short vendor_id;
    unsigned short product_id;
    int id;
    // private
    char* path;
    wchar_t* serial_number;
    unsigned short release_number;
    wchar_t* manufacturer_string;
    wchar_t* product_string;
    CRITICAL_SECTION  mtx; // 互斥锁

    // 基类方法
    struct streamDock* (*init)(struct streamDock* self, tranSport* transport, struct hid_device_info* devInfo);
    void (*destory)(struct streamDock* self);
    unsigned char* (*getFirmVersion)(struct streamDock* self, int length);
    int (*open)(struct streamDock* self);
    int (*disconnected)(struct streamDock* self);
    int (*setBrightness)(struct streamDock* self, int percent);
    int (*setBackgroundImg)(struct streamDock* self, const char* path);
    int (*setBackgroundImgData)(struct streamDock* self, unsigned char* imagedata);
    unsigned char* (*read)(struct streamDock* self);
    void (*readToVector)(struct streamDock* self, unsigned char* vec);
    int (*setKeyImg)(struct streamDock* self, const char* path, int key);
    int (*setKeyImgData)(struct streamDock* self, unsigned char* imagedata, int key);
    int (*clearIcon)(struct streamDock* self, int index);
    int (*clearAllIcon)(struct streamDock* self);
    int (*wakeScreen)(struct streamDock* self);
    int (*refresh)(struct streamDock* self);
    char* (*getPath)(struct streamDock* self);
}streamDock;

//// 默认不需要重写的函数
// 默认基类构造
streamDock* streamDock_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo);
// 默认基类析构
void streamDock_destory(streamDock* self);

#endif