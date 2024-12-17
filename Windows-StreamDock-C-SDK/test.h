#ifndef _LOG_H
#define _LOg_H
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/core/core_c.h> 
#include <opencv2/highgui/highgui_c.h> 

#include "streamDock.h"

void writeFile(unsigned char* array, int len)
{
	FILE* file = fopen("array.txt", "w");
	fwrite(array, 1, len, file);
	fclose(file);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI ReadThread(LPVOID lpParam) 
{
    streamDock* s = (streamDock*)lpParam;
    while (1)
    {
        unsigned char* buf = s->read(s);
        printf("%s %s %d %d\n", buf, buf + 5, (int)buf[9], (int)buf[10]);
		free(buf);		// must free here
    }
}

void test_streamDock_read(streamDock* stream)
{
	HANDLE hThread;
	DWORD threadID;
	hThread = CreateThread(NULL, 0, ReadThread, (LPVOID)stream, 0, &threadID);
	if (hThread == NULL) {
		printf("Failed to create thread. Error code: %lu\n", GetLastError());
		return;
	}
	// 等待子线程结束
	WaitForSingleObject(hThread, INFINITE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI ReadToVectorThread(LPVOID lpParam)
{
	streamDock* s = (streamDock*)lpParam;
	while (1)
	{
		//unsigned char* retVec = (unsigned char*)malloc(sizeof(unsigned char) * 13);
		unsigned char retVec[13] = { 0 };
		s->readToVector(s, retVec);
		printf("%s %s %d %d\n", retVec, retVec + 5, (int)retVec[9], (int)retVec[10]);
		//free(retVec);
	}
}

void test_streamDock_readToVector(streamDock* stream)
{
	HANDLE hThread;
	DWORD threadID;
	hThread = CreateThread(NULL, 0, ReadToVectorThread, (LPVOID)stream, 0, &threadID);
	if (hThread == NULL) {
		printf("Failed to create thread. Error code: %lu\n", GetLastError());
		return;
	}
	// 等待子线程结束
	WaitForSingleObject(hThread, INFINITE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI ListenThread(LPVOID lpParam)
{
	DeviceManager* dm = (DeviceManager*)lpParam;
	DeviceManager_listen(dm);
}

void test_streamDock_listen(DeviceManager* dm)
{
	HANDLE hThread;
	DWORD threadID;
	hThread = CreateThread(NULL, 0, ListenThread, (LPVOID)dm, 0, &threadID);
	if (hThread == NULL) {
		printf("Failed to create thread. Error code: %lu\n", GetLastError());
		return;
	}
	// 等待子线程结束
	WaitForSingleObject(hThread, INFINITE);
}


#endif