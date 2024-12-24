#define EXAMPLE

#ifdef EXAMPLE
#include <stdio.h>
#include "DeviceManager.h"
#include "test.h"
#include "memorybytestrem.h"
#include "rotateimg.h"

int main()
{
	DeviceManager* dm = DeviceManager_init();
	DeviceManager_enumerate(dm);
	int mapSize = dm->streamDockmaps_size;
	printf("find %d device\n", mapSize);
	HANDLE hThread = NULL;
	for (int i = 0; i < mapSize; ++i)
	{
		streamDock* stream = dm->streamDockmaps[i].value;
		stream->open(stream);
		stream->wakeScreen(stream);
		stream->setBrightness(stream, 100);
		hThread = test_streamDock_read(stream);
		stream->clearAllIcon(stream);
		stream->refresh(stream);

		for (int j = 1; j <= 15; ++j)
		{
			IplImage* img = cvLoadImage("./img/code112.png", CV_LOAD_IMAGE_COLOR);		// 从文件加载图像
			if (!img) {
				fprintf(stderr, "Error loading image\n");
				return -1;
			}
			{
			/*
				// usage 1: set key image
				// read from IplImage, support all picture format
				int bufferLen = 0;
				IplImage* temp = rotateEx(img);
				uchar* buffer = saveImageToMemory(temp, &bufferLen, 90);
				//////// writeToFile(buffer, bufferLen, "xxx.jpg");		// save the memory byte stream
				int ret = tranSportSetKeyImgDataDualDeviceEx(stream->transport, buffer, bufferLen, j);
				cvRelease(&temp);
			*/
			}
			

			{
			/*
				// usage 2: set key image by fread
				// only support by JEPG read from fread()
				FILE* file = fopen("./img/2.jpg", "rb");	// rb mode necessarily
				// 获取文件大小
				fseek(file, 0, SEEK_END);					// get file size throght file seek
				long file_size = ftell(file);	
				fseek(file, 0, SEEK_SET);		
				// malloc
				unsigned char* buffer = (unsigned char*)malloc(file_size);
				size_t bytes_read;
				if (buffer)
				{
					bytes_read = fread(buffer, 1, file_size, file);
				}
				int ret = tranSportSetKeyImgDataDualDeviceEx(stream->transport, buffer, file_size, j);
				//printf("ret: %d\n", ret);
				fclose(file);
				free(buffer);
			*/
			}
			
			{
			/*
				// usage 3: set many background through api
				// test set background
				if (j % 2 == 0)
					stream->setBackgroundImg(stream, "./img/bg.jpg");
				else
					stream->setBackgroundImg(stream, "./img/YiFei.png");
			*/
			}
			
			{

				// usage 4: set many background through tranSport
				IplImage* img;
				if (j % 2 == 0)
					img = cvLoadImage("./img/bg.png", CV_LOAD_IMAGE_COLOR);		
				else
					img = cvLoadImage("./img/YiFei.png", CV_LOAD_IMAGE_COLOR);		
				if (!img) {
					fprintf(stderr, "Error loading image\n");
					return -1;
				}
				int bufferLen = 0;
				IplImage* temp = rotateEx(img);
				uchar* buffer = saveImageToMemory(temp, &bufferLen, 90);
				//////// writeToFile(buffer, bufferLen, "xxx.jpg");		// save the memory byte stream
				int ret = tranSportSetBackgroundImgDataDualDevice(stream->transport, buffer, bufferLen);
				//int ret = tranSportSetBackgroundImg(stream->transport, buffer, bufferLen);

				cvRelease(&temp);
				cvRelease(&img);

			}
			cvRelease(&img);
		}
	}
	// 等待子线程结束
	if (hThread)
	{
		WaitForSingleObject(hThread, INFINITE);
	}
	test_streamDock_listen(dm);
	DeviceManager_free(dm);
	return 0;
}

#endif // EXAMPLE