//#define EXAMPLE_293
#ifdef EXAMPLE_293


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
	printf("发现了%d个设备\n", mapSize);
	HANDLE hThread = NULL;
	for (int i = 0; i < mapSize; ++i)
	{

		streamDock* stream = dm->streamDockmaps[i].value;
		stream->open(stream);
		stream->wakeScreen(stream);
		stream->setBrightness(stream, 100);
		hThread = test_streamDock_read(stream);

		stream->clearAllIcon(stream);
		for (int j = 1; j <= 15; ++j)
		{
			{
			/*
				// set key image
				IplImage* img = cvLoadImage("./img/12345.png", CV_LOAD_IMAGE_COLOR);		// 从文件加载图像
				if (!img) {
					fprintf(stderr, "Error loading image\n");
					return -1;
				}
				IplImage* temp = rotateEx(img);
				int buffer_size = 0;
				uchar* buffer = saveImageToMemory(temp, &buffer_size, 95);
				tranSportSetKeyImgData(stream->transport, buffer, buffer_size, j, 0, 0);
				//stream->setKeyImgData(stream, img->imageData, j);
				stream->refresh(stream);
			*/
			}
			{
			/*
				// set one background
				stream->setBackgroundImg(stream, "./img/bg.png");
			*/
			}
			{
			/*
				// set many background
				if (j % 2 == 0)
					stream->setBackgroundImg(stream, "./img/YiFei.png");
				else
					stream->setBackgroundImg(stream, "./img/bg.png");
			*/
			}
		}
	}
	// 等待子线程结束
	if (hThread)
		WaitForSingleObject(hThread, INFINITE);
	test_streamDock_listen(dm);
	DeviceManager_free(dm);
	return 0;
}

#endif // EXAMPLE_293