#ifndef _STREAMDOCKN4_
#define _STREAMDOCKN4_
#include "streamDock.h"
#include "tranSport.h"
#include "hidapi.h"

// ֻ�ṩ������������ⲿ, ʵ���ڲ��������ĺ���������, ��stream->setKeyImg(stream, ...);
streamDock* streamDockN4_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo);
void streamDockN4_destory(struct streamDock* self);

#endif