#include <stdio.h>
#include "DeviceManager.h"
#include "test.h"

/*
	293
	api right:
		1 stream->open(stream);								ok
		2 stream->wakeScreen(stream);						ok
		3 stream->setBrightness(stream, 100);				ok
		4 stream->refresh(stream);							ok
		5 stream->clearAllIcon(stream);						ok
		6 stream->clearIcon(stream, index);					ok
		7 stream->setKeyImg(stream, "12345.png", index);	ok
		8 stream->setKeyImgData(stream, imgData, ubdex);	ok
		9 stream->disconnected(stream);						ok
		10 stream->read(stream);							ok
		11 stream->readToVector(stream, retVec);			ok

	293V3
	api right:
		1 stream->open(stream);								ok
		2 stream->wakeScreen(stream);						ok
		3 stream->setBrightness(stream, 100);				ok
		4 stream->refresh(stream);							ok
		5 stream->clearAllIcon(stream);						ok
		6 stream->clearIcon(stream, index);					ok
		7 stream->setKeyImg(stream, "12345.png", index);	ok
		8 stream->setKeyImgData(stream, imgData, ubdex);	ok
		9 stream->disconnected(stream);						ok
		10 stream->read(stream);							ok
		11 stream->readToVector(stream, retVec);			ok

	N3
	api right:
		1 stream->open(stream);								ok
		2 stream->wakeScreen(stream);						ok
		3 stream->setBrightness(stream, 100);				ok
		4 stream->refresh(stream);							ok
		5 stream->clearAllIcon(stream);						ok
		6 stream->clearIcon(stream, index);					ok
		7 stream->setKeyImg(stream, "12345.png", index);	ok
		8 stream->setKeyImgData(stream, imgData, ubdex);	ok
		9 stream->disconnected(stream);						ok
		10 stream->read(stream);							ok
		11 stream->readToVector(stream, retVec);			ok

	N4
	api right:
		1 stream->open(stream);								ok
		2 stream->wakeScreen(stream);						ok
		3 stream->setBrightness(stream, 100);				ok
		4 stream->refresh(stream);							ok
		5 stream->clearAllIcon(stream);						ok
		6 stream->clearIcon(stream, index);					ok
		7 stream->setKeyImg(stream, "12345.png", index);	ok
		8 stream->setKeyImgData(stream, imgData, ubdex);	ok
		9 stream->disconnected(stream);						ok
		10 stream->read(stream);							ok
		11 stream->readToVector(stream, retVec);			ok
*/

int main()
{
	DeviceManager* dm =	DeviceManager_init();
	DeviceManager_enumerate(dm);
	
	int mapSize = dm->streamDockmaps_size;
	printf("发现了%d个设备\n", mapSize);
	for (int i = 0; i < mapSize; ++i)
	{
		streamDock* stream = dm->streamDockmaps[i].value;
		//////////////////////////////////////////////////////////////////////////////
		// api 1 test open
		//////////////////////////////////////////////////////////////////////////////
		stream->open(stream);
		//////////////////////////////////////////////////////////////////////////////
		// api 2 test wakeScreen
		//////////////////////////////////////////////////////////////////////////////
		stream->wakeScreen(stream);
		//////////////////////////////////////////////////////////////////////////////
		// api 3 test setBrightness
		//////////////////////////////////////////////////////////////////////////////
		////设置设备屏幕亮度为100%
		stream->setBrightness(stream, 100);
		//////////////////////////////////////////////////////////////////////////////
		// api 4 test refresh
		//////////////////////////////////////////////////////////////////////////////
		//stream->refresh(stream);
		//////////////////////////////////////////////////////////////////////////////
		// api 5 test clearAllIcon
		//////////////////////////////////////////////////////////////////////////////
		//stream->clearAllIcon(stream);
		//stream->refresh(stream);
		//////////////////////////////////////////////////////////////////////////////
		// api 6 test clearIcon
		//////////////////////////////////////////////////////////////////////////////
		//stream->clearIcon(stream, 5);
		//stream->refresh(stream);
		//////////////////////////////////////////////////////////////////////////////
		// api 7 test setKeyImg
		//////////////////////////////////////////////////////////////////////////////
		//for (int j = 1; j <= 15; ++j)
		//{
		//	stream->setKeyImg(stream, "./img/code112.png", j);
		//	stream->refresh(stream);
		//}
		//Sleep(2000);
		//////////////////////////////////////////////////////////////////////////////
		// api 8 test setKeyImgData
		//////////////////////////////////////////////////////////////////////////////
		//stream->clearAllIcon(stream);
		//for (int j = 1; j <= 15; ++j)
		//{
		//	IplImage* img = cvLoadImage("./img/tiga112.png", CV_LOAD_IMAGE_COLOR);		// 从文件加载图像
		//	if (!img) {
		//		fprintf(stderr, "Error loading image\n");
		//		return -1;
		//	}
		//	unsigned char* imagedata = (unsigned char*)img->imageData;			// 获取图像数据指针
		//	stream->setKeyImgData(stream, imagedata, j);
		//	stream->refresh(stream);
		//}
		//Sleep(2000);
		//////////////////////////////////////////////////////////////////////////////
		// api 9 test disconnect
		//////////////////////////////////////////////////////////////////////////////
		//stream->disconnected(stream);
		//////////////////////////////////////////////////////////////////////////////
		// api 10 test read
		//////////////////////////////////////////////////////////////////////////////
		//test_streamDock_read(stream);
		//////////////////////////////////////////////////////////////////////////////
		// api 11 test readToVecTor
		//////////////////////////////////////////////////////////////////////////////
		//test_streamDock_readToVector(stream);
		//////////////////////////////////////////////////////////////////////////////
		// api 12 test setBackgroundImg
		//////////////////////////////////////////////////////////////////////////////		
		//stream->clearAllIcon(stream);
		//stream->setBackgroundImg(stream, "./img/bg.png");
		//stream->refresh(stream);
		//Sleep(4000);
		//stream->setBackgroundImg(stream, "./img/YiFei.png");
		//stream->refresh(stream);
		//Sleep(2000);
		//////////////////////////////////////////////////////////////////////////////
		// api 13 test setBackgroundImgData
		//////////////////////////////////////////////////////////////////////////////
		IplImage* img = cvLoadImage("./img/bg.png", CV_LOAD_IMAGE_COLOR);		// 从文件加载图像
		if (!img) {
			fprintf(stderr, "Error loading image\n");
			return -1;
		}
		
		unsigned char* imagedata = (unsigned char*)img->imageData;			// 获取图像数据指针
		stream->setBackgroundImgData(stream, imagedata);
		stream->refresh(stream);
		//stream->disconnected(stream);
		Sleep(4000);
		stream->clearAllIcon(stream);
		stream->refresh(stream);
		for (int j = 1; j <= 15; ++j)
		{
			IplImage* img = cvLoadImage("./img/tiga112.png", CV_LOAD_IMAGE_COLOR);		// 从文件加载图像
			if (!img) {
				fprintf(stderr, "Error loading image\n");
				return -1;
			}
			unsigned char* imagedata = (unsigned char*)img->imageData;			// 获取图像数据指针
			stream->setKeyImgData(stream, imagedata, j);
			stream->refresh(stream);
		}
		Sleep(2000);

		//////////////////////////////////////////////////////////////////////////////
		///// block the app end...
		//////////////////////////////////////////////////////////////////////////////
		//test_streamDock_read(stream);
	}
	test_streamDock_listen(dm);
	DeviceManager_free(dm);
	return 0;
}