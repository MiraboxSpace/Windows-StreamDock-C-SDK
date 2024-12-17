#ifndef _STREAMDOCK293V3_
#define _STREAMDOCK293V3_
#include "streamDock.h"
#include "tranSport.h"
#include "hidapi.h"

// 只提供构造和析构给外部, 实例内部有其他的函数供访问, 如stream->setKeyImg(stream, ...);
streamDock* streamDock293V3_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo);
void streamDock293V3_destory(struct streamDock* self);

#endif