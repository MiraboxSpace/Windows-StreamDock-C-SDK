#ifndef _STREAMDOCKN3_
#define _STREAMDOCKN3_
#include "streamDock.h"
#include "tranSport.h"
#include "hidapi.h"

// ֻ�ṩ������������ⲿ, ʵ���ڲ��������ĺ���������, ��stream->setKeyImg(stream, ...);
streamDock* streamDockN3_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo);
void streamDockN3_destory(struct streamDock* self);

#endif