#ifndef _STREAMDOCK293_
#define _STREAMDOCK293_
#include "streamDock.h"
#include "tranSport.h"
#include "hidapi.h"

// ֻ�ṩ������������ⲿ, ʵ���ڲ��������ĺ���������, ��stream->setKeyImg(stream, ...);
streamDock* streamDock293_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo);
void streamDock293_destory(struct streamDock* self);

#endif