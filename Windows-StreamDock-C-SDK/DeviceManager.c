#include "DeviceManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <stdbool.h>

const GUID init_val = { 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
DeviceManager* other;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 获取 WebSocketNotifier 指针
    //WebSocketNotifier* wsNotifier = (WebSocketNotifier*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    Sleep(100);//延时100毫秒使设备能够被加载
    int productsVendor[] = { USB_VID, USB_VID293s, USB_VID293V3, USB_VID293V3, USB_VID293V3, USB_VIDN3, USB_VIDN3, USB_VIDN4, USB_VIDN4EN };
    int productsProduct[] = { USB_PID_STREAMDOCK_936, USB_PID_STREAMDOCK_936s, USB_PID_STREAMDOCK_293V3, USB_PID_STREAMDOCK_293V3EN, USB_PID_STREAMDOCK_293V25, USB_PID_STREAMDOCK_N3, USB_PID_STREAMDOCK_N3EN, USB_PID_STREAMDOCK_N4, USB_PID_STREAMDOCK_N4EN };
    int productsType[] = { 1, 2, 3, 3, 3, 4, 4, 5, 5 };
    int tupleSize = 9;

    switch (message)
    {
    case WM_DEVICECHANGE:
    {
        
        if (wParam == DBT_DEVICEARRIVAL)
        {
            for (int i = 0; i < tupleSize; i++) {
                int vendorId = productsVendor[i];
                int productId = productsProduct[i];
                int deviceType = productsType[i];
                struct hid_device_info* deviceInfo = tranSportEnumerate(other->transport, vendorId, productId);
                bool flag = false;

                while (deviceInfo) {
                    //遍历所有的已有的设备类，看看当前查找出的设备是不是已经存在的，存在的就跳过，不存在的就创建
                    for (int i = 0; i < other->streamDockmaps_size; ++i)
                    {
                        if (strcmp(deviceInfo->path, other->streamDockmaps->key) == 0)
                            flag = true;
                    }
                    if (!flag)
                    {
                        streamDock* node = NULL;

                        if (deviceType == 1) {
                            node = streamDock293_init(NULL, other->transport, deviceInfo);
                        }
                        else if (deviceType == 3 && deviceInfo->usage == 1) {
                            node = streamDock293V3_init(NULL, other->transport, deviceInfo);
                        }
                        else if (deviceType == 4 && deviceInfo->usage == 1) {
                            node = streamDockN3_init(NULL, other->transport, deviceInfo);
                        }
                        else if (deviceType == 5 && deviceInfo->usage == 1) {
                            node = streamDockN4_init(NULL, other->transport, deviceInfo);
                        }
                        if (node) {
                            DeviceManager_add_streamDock(other, deviceInfo->path, node);
                            other->deviceType[deviceType]++;

                        }
                        printf("Create Success\n");
                        break;
                    }
                    deviceInfo = deviceInfo->next;
                }
            }
        }
        else if (wParam == DBT_DEVICEREMOVECOMPLETE)
        {
            int deviceNum = 0;
            for (int i = 0; i < tupleSize; i++) 
            {
                int vendorId = productsVendor[i];
                int productId = productsProduct[i];
                int deviceType = productsType[i];
                struct hid_device_info* deviceInfo = tranSportEnumerate(other->transport, vendorId, productId);
                while (deviceInfo)
                {
                    if (deviceInfo->usage == 1) 
                    {
                        deviceNum++;
                    }
                    deviceInfo = deviceInfo->next;
                }
            }
            if (other->streamDockmaps_size == deviceNum) 
            {
                return 0;
            }
            for (int i = 0; i < tupleSize; i++) 
            {
                int vendorId = productsVendor[i];
                int productId = productsProduct[i];
                int deviceType = productsType[i];
                struct hid_device_info* info = tranSportEnumerate(other->transport, vendorId, productId);
                struct hid_device_info* infoHead;
                int flag = 0;
                for (int i = 0; i < other->streamDockmaps_size; ++i)
                {
                    if (other->streamDockmaps[i].value->product_id != productId || other->streamDockmaps[i].value->vendor_id != vendorId)
                        continue;
                    infoHead = info;
                    if (other->deviceType[deviceType] <= 0)
                        break;
                    flag = 0;
                    while (infoHead)
                    {
                        if (strcmp(other->streamDockmaps[i].key, infoHead->path) == 0)
                        {
                            flag = 1;
                        }
                        infoHead = infoHead->next;
                    }
                    infoHead = info;    // 记录头节点
                    struct hid_device_info* next = info->next;
                    if (flag == 0)
                    {
                        DeviceManager_remove_streamDock(other, i);
                        printf("Deletion Success\n");
                    }
                }
            }
        }
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

// 初始化设备管理器
DeviceManager* DeviceManager_init() {
    DeviceManager* dm = (DeviceManager*)malloc(sizeof(DeviceManager));
    dm->transport = tranSportInit();
    dm->streamDockmaps = (KeyValuePair*)malloc(MAX_DEVICES * sizeof(KeyValuePair));
    dm->streamDockmaps_size = 0;
    dm->deviceType = (int*)calloc(6, sizeof(int)); 
    dm->GUID_DEVINTERFACE_USB_DEVICE = init_val;
    other = dm;
    return dm;
}

// 释放设备管理器的内存
void DeviceManager_free(DeviceManager* dm) {
    if (dm) {
        if (dm->streamDockmaps) {
            for (int i = 0; i < dm->streamDockmaps_size; i++) {
                free(dm->streamDockmaps[i].key);
                dm->streamDockmaps[i].value->destory(dm->streamDockmaps[i].value);
            }
            free(dm->streamDockmaps);
        }
        if (dm->transport) {
            tranSportDestory(dm->transport);
        }
        free(dm->deviceType);
        if (dm->deviceInfo)
        {
            // 释放 struct hid_device_info* deviceInfo;
            hid_free_enumeration(dm->deviceInfo);
        }
        free(dm);
    }
}

// 将设备路径和设备指针存储到streamDockmaps
void DeviceManager_add_streamDock(DeviceManager* dm, const char* path, streamDock* dock) {
    if (dm->streamDockmaps_size < MAX_DEVICES) {
        dm->streamDockmaps[dm->streamDockmaps_size].key = _strdup(path);
        dm->streamDockmaps[dm->streamDockmaps_size].value = dock;
        dm->streamDockmaps_size++;
    }
}

void DeviceManager_remove_streamDock(DeviceManager* dm, int index) {
    if (dm->streamDockmaps_size < MAX_DEVICES) {
        free(dm->streamDockmaps[index].value);
        dm->streamDockmaps[dm->streamDockmaps_size].key = NULL;
        dm->streamDockmaps[dm->streamDockmaps_size].value = NULL;
        dm->streamDockmaps_size--;
    }
}

// 枚举设备
void DeviceManager_enumerate(DeviceManager* dm) {
    int productsVendor[] = { USB_VID, USB_VID293s, USB_VID293V3, USB_VID293V3, USB_VID293V3, USB_VIDN3, USB_VIDN3, USB_VIDN4, USB_VIDN4EN };
    int productsProduct[] = { USB_PID_STREAMDOCK_936, USB_PID_STREAMDOCK_936s, USB_PID_STREAMDOCK_293V3, USB_PID_STREAMDOCK_293V3EN, USB_PID_STREAMDOCK_293V25, USB_PID_STREAMDOCK_N3, USB_PID_STREAMDOCK_N3EN, USB_PID_STREAMDOCK_N4, USB_PID_STREAMDOCK_N4EN };
    int productsType[] = { 1, 2, 3, 3, 3, 4, 4, 5, 5 };
    int tupleSize = 9;

    for (int i = 0; i < tupleSize; i++) {
        int vendorId = productsVendor[i];
        int productId = productsProduct[i];
        int deviceType = productsType[i];

        struct hid_device_info* deviceInfo = tranSportEnumerate(dm->transport, vendorId, productId);
        dm->deviceInfo = deviceInfo;

        while (deviceInfo) {
            if (deviceInfo->serial_number == NULL) {
                deviceInfo = deviceInfo->next;
                continue;
            }

            streamDock* node = NULL;

            if (deviceType == 1) {
                node = streamDock293_init(NULL, dm->transport, deviceInfo);
            }
            else if (deviceType == 3 && deviceInfo->usage == 1) {
                node = streamDock293V3_init(NULL, dm->transport, deviceInfo);
            }
            else if (deviceType == 4 && deviceInfo->usage == 1) {
                node = streamDockN3_init(NULL, dm->transport, deviceInfo);
            }
            else if (deviceType == 5 && deviceInfo->usage == 1) {
                node = streamDockN4_init(NULL, dm->transport, deviceInfo);
            }

            if (node) {
                DeviceManager_add_streamDock(dm, deviceInfo->path, node);
                dm->deviceType[deviceType]++;
            }

            deviceInfo = deviceInfo->next;
        }
    }
}

void DeviceManager_listen(DeviceManager* dm)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SampleClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        L"SampleClass",
        L"Sample Window",
        0,
        0, 0, 0, 0,
        HWND_MESSAGE, // No actual window is created
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        fprintf(stderr, "Failed to create window.");
        return 1;
    }

    // 将 WebSocketNotifier 的指针存储到窗口数据中
    // SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&wsNotifier);

    // 注册设备通知
    DEV_BROADCAST_DEVICEINTERFACE dbdi;
    dbdi.dbcc_size = sizeof(dbdi);
    dbdi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    dbdi.dbcc_classguid = dm->GUID_DEVINTERFACE_USB_DEVICE;
    HDEVNOTIFY hDevNotify = RegisterDeviceNotification(hwnd, &dbdi, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (!hDevNotify) {
        fprintf(stderr, "Failed to register for device notifications.");
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
