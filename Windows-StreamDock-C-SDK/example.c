#define EXAMPLE

#ifdef EXAMPLE
#include <stdio.h>
#include "DeviceManager.h"
#include "test.h"
#include "memorybytestrem.h"
#include "rotateimg.h"
#include <time.h>

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
		//hThread = test_streamDock_read(stream);
		stream->clearAllIcon(stream);
		stream->refresh(stream);

		for (int j = 1; j <= 15; ++j)
		{
			IplImage* img = cvLoadImage("./img/tiga112.png", CV_LOAD_IMAGE_COLOR);		// 从文件加载图像
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
					uchar* buffer = saveImageToMemory(temp, &bufferLen, 80);
					writeToFile(buffer, bufferLen, "xxx.jpg");		// save the memory byte stream
					 int ret = tranSportSetKeyImgDataDualDeviceEx(stream->transport, buffer, bufferLen, j);
					cvRelease(&temp);
					free(buffer);
			*/
			}


			{
				/*
					// usage 2: set key image by fread
					// only support by JEPG read from fread()
					FILE* file = fopen("./img/code112.png", "rb");	// rb mode necessarily
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
					//int ret = tranSportSetKeyImgDataDualDeviceEx(stream->transport, buffer, file_size, j);
					int ret = tranSportSetKeyImgDualDevice(stream->transport, "./img/2.jpg", j);
					if (ret == -1)
					{
						printf("ret: %d\n", ret);
						//printf("getLastErrorCode: %d\n", getLastErrorCode());
						printLastErrorInfo();
					}
					tranSportRefresh(stream->transport);
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
				/*
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
				*/
			}
			cvRelease(&img);
		}

		{
			/* ================================================================================================================
			IplImage* img1 = cvLoadImage("./img/tiga112.png", CV_LOAD_IMAGE_COLOR);		// 从文件加载图像
			int bufferLen1 = 0;
			IplImage* temp1 = rotateEx(img1);
			uchar* buffer1 = saveImageToMemory(temp1, &bufferLen1, 90);

			IplImage* img2 = cvLoadImage("./img/code112.png", CV_LOAD_IMAGE_COLOR);		// 从文件加载图像
			int bufferLen2 = 0;
			IplImage* temp2 = rotateEx(img2);
			uchar* buffer2 = saveImageToMemory(temp2, &bufferLen2, 90);
			// 测试用例, 计算显示时长
			clock_t start_time, end_time;
			double elapsed_time;
			start_time = clock();


			for (int j = 1; j < 45; ++j)
			{
				for (int i = 1; i <= 15; ++i)
				{
					if (j % 2 == 0)
					{
						int ret = tranSportSetKeyImgDataDualDeviceEx(stream->transport, buffer1, bufferLen1, i);		// tranSportSetKeyImgDataDualDeviceEx [OK]
						//stream->setKeyImgData(stream, img1->imageData, i);											// tranSportSetKeyImgDataDualDevice	  [OK]
					}
					else
					{
						int ret = tranSportSetKeyImgDataDualDeviceEx(stream->transport, buffer2, bufferLen2, i);		// tranSportSetKeyImgDataDualDeviceEx [OK]
						//stream->setKeyImgData(stream, img2->imageData, i);											// tranSportSetKeyImgDataDualDevice	  [OK]
					}
				}
				tranSportRefresh(stream->transport);
				tranSportKeyAllClear(stream->transport);
			}

			IplImage* img = cvLoadImage("./img/YiFei.png", CV_LOAD_IMAGE_COLOR);
			int bufferLen = 0;								// 获取大小
			IplImage* temp = rotateEx(img);					// 翻转
			uchar* buffer = saveImageToMemory(temp, &bufferLen, 90);	// 压缩为质量90的jpg缓冲区流数据
			int ret = tranSportSetBackgroundImgDataDualDevice(stream->transport, buffer, bufferLen);

			cvRelease(&temp1);
			cvRelease(&temp2);
			cvRelease(&img);
			cvRelease(&temp);
			end_time = clock();
			elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
			printf("Program execution time: %.6f seconds\n", elapsed_time);
			================================================================================================================*/
		}


		{
			resetDebugFlag();
			int ret = tranSportSetKeyImgDualDevice(stream->transport, "./img/key_sample_q30.jpg", 15);
			//int ret = tranSportSetBackgroundImgDualDevice(stream->transport, "./img/bg.jpg");
			if (-1 == ret)
			{
				printLastErrorInfo();
			}
			tranSportRefresh(stream->transport);
			/// 测试背景用例
			// 读取到opencv(解码decode)
			IplImage* img1 = cvLoadImage("./img/bg.png", CV_LOAD_IMAGE_COLOR);
			IplImage* temp1 = rotateEx(img1);
			IplImage* img2 = cvLoadImage("./img/YiFei.png", CV_LOAD_IMAGE_COLOR);
			IplImage* temp2 = rotateEx(img2);
			IplImage* img3 = cvLoadImage("./img/bg.jpg", CV_LOAD_IMAGE_COLOR);
			IplImage* temp3 = rotateEx(img3);
			IplImage* img4 = cvLoadImage("./img/desktop.png", CV_LOAD_IMAGE_COLOR);
			IplImage* temp4 = rotateEx(img4);
			for (int j = 1; j <= 10000; ++j)
			{
				int randnum = rand() % 4 + 1;	// 产生随机数1~3

				IplImage* bg;
				if (randnum == 1) bg = temp1;
				else if (randnum == 2) bg = temp2;
				else if (randnum == 3) bg = temp3;
				else if (randnum == 4) bg = temp4;
				else break;
				int bufferLen = 0;

				uchar* buffer = saveImageToMemory(bg, &bufferLen, 90);
				int ret = tranSportSetBackgroundImgDataDualDevice(stream->transport, buffer, bufferLen);
				printf("now picture: %d\n", randnum);
				free(buffer);
			}
			cvRelease(&img1);cvRelease(&temp1);
			cvRelease(&img2);cvRelease(&temp2);
			cvRelease(&img3);cvRelease(&temp3);
			cvRelease(&img4);cvRelease(&temp4);

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