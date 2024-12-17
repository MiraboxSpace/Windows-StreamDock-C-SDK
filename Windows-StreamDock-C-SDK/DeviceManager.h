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

#define MAX_DEVICES 100  // ���֧�ֵ��豸����


// �豸�������� Key-Value ��ֵ�Խṹ��
typedef struct {
    char* key;          // �����豸·����
    streamDock* value;  // ֵ��ָ���豸��ָ�룩
} KeyValuePair;

// �豸�����������ṹ��
typedef struct {
    struct hid_device_info* deviceInfo;
    HINSTANCE hInstance;
    HWND hwnd;
    KeyValuePair* streamDockmaps;  // �洢�豸�ļ�ֵ��
    int streamDockmaps_size;       // ��¼�洢���豸����
    int* deviceType;               // ��¼�豸���͵�����
    tranSport* transport;
    GUID GUID_DEVINTERFACE_USB_DEVICE;
} DeviceManager;

// �豸�������ĺ�������
DeviceManager* DeviceManager_init();  // ��ʼ���豸������
void DeviceManager_free(DeviceManager* dm); // �ͷ��豸���������ڴ�
void DeviceManager_add_streamDock(DeviceManager* dm, const char* path, streamDock* dock); // ����豸
void DeviceManager_remove_streamDock(DeviceManager* dm, int index);    // ɾ���豸
void DeviceManager_enumerate(DeviceManager* dm); // ö���豸
void DeviceManager_listen(DeviceManager* dm);   // ����usb���

#endif // _DEVICEMANAGER_H_
