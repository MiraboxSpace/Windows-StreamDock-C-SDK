#include "streamDock.h"
#include <stdio.h>

// getFirmVersion
static unsigned char* streamDock_getFirmVersion(struct streamDock* self, int length);
// open
static int streamDock_open(struct streamDock* self);
// disconnected
static int streamDock_disconnected(struct streamDock* self);
// clearIcon
static int streamDock_clearIcon(struct streamDock* self, int index);
// clearAllIcon
static int streamDock_clearAllIcon(struct streamDock* self);
// wakeScreen
static int streamDock_wakeScreen(struct streamDock* self);
// refresh
static int streamDock_refresh(struct streamDock* self);
// getPath
static char* streamDock_getPath(struct streamDock* self);


streamDock* streamDock_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo)
{
	if (self) return self;
	self = (streamDock*)malloc(sizeof(struct streamDock));
	if (!self)
	{
		fprintf(stderr, "streamDock_init malloc error...\n");
		return NULL;
	}
	memset(self, 0, sizeof(struct streamDock));
	self->transport = transport;
	self->vendor_id = devInfo->vendor_id;
	self->product_id = devInfo->product_id;
	self->product_string = devInfo->product_string;
	self->path = devInfo->path;
	self->serial_number = devInfo->serial_number;
	self->manufacturer_string = devInfo->manufacturer_string;
	self->release_number = devInfo->release_number;

	// 初始化互斥锁
	InitializeCriticalSection(&self->mtx);

	// 设置默认实现的函数(不需要override的函数)
	//self->init = streamDock_init;
	//self->destory = streamDock_destory;
	self->open = streamDock_open;
	self->getFirmVersion = streamDock_getFirmVersion;
	self->disconnected = streamDock_disconnected;
	self->clearIcon = streamDock_clearIcon;
	self->clearAllIcon = streamDock_clearAllIcon;
	self->wakeScreen = streamDock_wakeScreen;
	self->refresh = streamDock_refresh;
	self->getPath = streamDock_getPath;

	//// 其他的默认设置成null防止误触
	//self->setBrightness = NULL;
	//self->setBackgroundImg = NULL;
	//self->setBackgroundImgData = NULL;
	//self->read = NULL;
	//self->readToVector = NULL;
	//self->setKeyImg = NULL;
	//self->setKeyImgData = NULL;

	// 返回该实例
	return self;
}

void streamDock_destory(streamDock* self)
{
	if (!self) return;
	// 销毁互斥锁
	DeleteCriticalSection(&self->mtx);
	free(self);
	self = NULL;
}

static unsigned char* streamDock_getFirmVersion(streamDock* self, int length)
{
	if (!self) return NULL;
	return tranSportGetInputReport(self->transport, length);
}

static int streamDock_open(streamDock* self)
{
	if (!self) return -1;
	if (tranSportOpen(self->transport, self->path) == -1)
	{
		return -1;
	}
	return 1;
}

static int streamDock_disconnected(streamDock* self)
{
	if (!self) return -1;
	return tranSportDisconnected(self->transport);
}

static int streamDock_clearIcon(streamDock* self, int index)
{
	if (!self) return -1;
	return tranSportKeyClear(self->transport, index);
}

static int streamDock_clearAllIcon(streamDock* self)
{
	if (!self) return -1;
	return tranSportKeyAllClear(self->transport);
}

static int streamDock_wakeScreen(streamDock* self)
{
	if (!self) return -1;
	return tranSportWakeScreen(self->transport);
}

static int streamDock_refresh(streamDock* self)
{
	if (!self) return -1;
	return tranSportRefresh(self->transport);
}

static char* streamDock_getPath(streamDock* self)
{
	if (!self) return NULL;
	return self->path;
}
