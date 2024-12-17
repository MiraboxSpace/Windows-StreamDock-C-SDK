#ifndef _DEVICEMANAGER_H_
#define _DEVICEMANAGER_H_

#include <windows.h>
#include "tranSport.h"
#include "streamDock.h"
#include "productIDs.h"
#include "streamDock293.h"
#include "streamDockN3.h"
#include "streamDockN4.h"
#include "streamDock293V3.h"
#include "hidapi.h"
#include <Dbt.h>
#include <tchar.h>

#define MAX_DEVICES 100  // 最多支持的设备数量


// 设备管理器的 Key-Value 键值对结构体
typedef struct {
    char* key;          // 键（设备路径）
    streamDock* value;  // 值（指向设备的指针）
} KeyValuePair;

// 设备管理器的主结构体
typedef struct {
    struct hid_device_info* deviceInfo;
    HINSTANCE hInstance;
    HWND hwnd;
    KeyValuePair* streamDockmaps;  // 存储设备的键值对
    int streamDockmaps_size;       // 记录存储的设备数量
    int* deviceType;               // 记录设备类型的数量
    tranSport* transport;
    GUID GUID_DEVINTERFACE_USB_DEVICE;
} DeviceManager;

// 设备管理器的函数声明
DeviceManager* DeviceManager_init();  // 初始化设备管理器
void DeviceManager_free(DeviceManager* dm); // 释放设备管理器的内存
void DeviceManager_add_streamDock(DeviceManager* dm, const char* path, streamDock* dock); // 添加设备
void DeviceManager_remove_streamDock(DeviceManager* dm, int index);    // 删除设备
void DeviceManager_enumerate(DeviceManager* dm); // 枚举设备
void DeviceManager_listen(DeviceManager* dm);   // 监听usb插拔

#endif // _DEVICEMANAGER_H_
