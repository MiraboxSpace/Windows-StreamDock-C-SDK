#ifndef _STREAMDOCK293V3_
#define _STREAMDOCK293V3_
#include "streamDock.h"
#include "tranSport.h"
#include "hidapi.h"

// ֻ�ṩ������������ⲿ, ʵ���ڲ��������ĺ���������, ��stream->setKeyImg(stream, ...);
streamDock* streamDock293V3_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo);
void streamDock293V3_destory(struct streamDock* self);

#endif