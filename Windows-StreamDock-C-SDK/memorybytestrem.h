#ifndef _MEMORYBYTESTREAM_H
#define _MEMORYBYTESTREAM_H
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///		简介: 1 可以保存图片的字节流数据到内存里面, 保存的数据与使用fread得到的数据是一样的
///			  2 同样可以使用使用fwrite保存saveImageToMemory得到的数据, 得到的就是一张图片文件
///			  3 可以使用封装好的writeToFile函数来保存saveImageToMemory得到的字节流数据
///			  4 使用loadImageFromMemory就可以把内存里面的图片字节流数据转换为IplImage* 图像原始数据类型
///			  5 暂时只支持.jpg格式
///			  6 jpg质量参数最好不要超过95，因为大小最好不要超过10K
/// 
///		注意事项: 1 saveImageToMemory_B中的返回值, 你必须使用free来释放, 他是堆区内存
///				  2 loadImageFromMemory的IplImage* 需要你使用cvReleaseImage来对该指针进行释放
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///		Description:
///			  1 It can save image byte stream data into memory, and the saved data is the same as the data obtained using fread.
///			  2 Similarly, you can use fwrite to save the data obtained from saveImageToMemory, and the result will be an image file.
///			  3 You can use the encapsulated writeToFile function to save the byte stream data obtained from saveImageToMemory.
///			  4 You can use loadImageFromMemory to convert the image byte stream data in memory into an IplImage* image.
///			  5 Currently, only ".jpg" formats are supported.
///			  6 The quality parameter should not exceed 95, as the file size should preferably not exceed 10K Bytes.
///     Notes:
///				  1 The return value of saveImageToMemory_B must to be freed, as it is heap memory.
///				  2 The IplImage* returned by loadImageFromMemory needs to be released using cvReleaseImage.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		USAGE OF THIS
//		IplImage* img = cvLoadImage("./img/tiga.png", CV_LOAD_IMAGE_COLOR);  // Load color image, we dont need the alpha
//		Save image to memory as byte stream
//		uchar* buffer = NULL;
//		int buffer_size = 0;
//		uchar* buffer = saveImageToMemory(img1, &bufferLen, 90);
//		
//		IplImage* new_img = loadImageFromMemory(buffer, buffer_size);
//		free(buffer);
//		cvShowImage("Loaded Image", new_img);
//		cvWaitKey(0);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// you can get the return value: buffer_size and unsigned char *memoryByte stream which is from jpg file 
uchar* saveImageToMemory(IplImage* img, int* buffer_size, int quality);

// Write byte stream to file
void writeToFile(const uchar* buffer, int buffer_size, const char* filename);

// Load image from byte stream (decode from memory)
IplImage* loadImageFromMemory(const uchar* buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif