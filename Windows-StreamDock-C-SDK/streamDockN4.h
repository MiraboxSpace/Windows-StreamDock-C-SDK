#ifndef _STREAMDOCKN4_
#define _STREAMDOCKN4_
#include "streamDock.h"
#include "tranSport.h"
#include "hidapi.h"

// 只提供构造和析构给外部, 实例内部有其他的函数供访问, 如stream->setKeyImg(stream, ...);
streamDock* streamDockN4_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo);
void streamDockN4_destory(struct streamDock* self);

#endif