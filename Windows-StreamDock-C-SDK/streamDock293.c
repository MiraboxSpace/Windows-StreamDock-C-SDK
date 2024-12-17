#include "streamDock.h"
#include "streamDock293.h"
#include "tranSport.h"
#include "hidapi.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
//#define CV_IGNORE_DEBUG_BUILD_GUARD		// C语言环境DEBUG模式必须添加这个玩意才能编译通过
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/core/core_c.h> 
#include <opencv2/highgui/highgui_c.h> 
#include <stdbool.h>


// 声明需要重写的函数
//static streamDock* streamDock293_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo);
//static void streamDock293_destory(struct streamDock* self);
static int transform(int x);
static unsigned char* streamDock293_getFirmVersion(struct streamDock* self, int lenth);
static int streamDock293_setBrightness(struct streamDock* self, int percent);
static int streamDock293_setBackgroundImg(struct streamDock* self, const char* path);
static int streamDock293_setBackgroundImgData(struct streamDock* self, unsigned char* imagedata);
static unsigned char* streamDock293_read(struct streamDock* self);
static void streamDock293_readToVector(struct streamDock* self, unsigned char* vec);
static int streamDock293_setKeyImg(struct streamDock* self, const char* path, int key);
static int streamDock293_setKeyImgData(struct streamDock* self, unsigned char* imagedata, int key);
static int streamDock293_clearIcon(struct streamDock* self, int index);

// 子类(streamDock293)全局变量, 析构函数不需要重写了, extern到外面后直接init就能用了
//streamDock streamDock293 = {
//    .init = streamDock293_init,
//    .destory = streamDock293_destory
//};

streamDock* streamDock293_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo)
{
    // 构造基类
    self = streamDock_init(self, transport, devInfo);
    // 构造子类
    self->init = streamDock293_init;
    self->destory = streamDock293_destory;         // 内部使用基类的析构函数
    self->getFirmVersion = streamDock293_getFirmVersion;
    self->setBrightness = streamDock293_setBrightness;
    self->setBackgroundImg = streamDock293_setBackgroundImg;
    self->setBackgroundImgData = streamDock293_setBackgroundImgData;
    self->read = streamDock293_read;
    self->readToVector = streamDock293_readToVector;
    self->setKeyImg = streamDock293_setKeyImg;
    self->setKeyImgData = streamDock293_setKeyImgData;
    self->clearIcon = streamDock293_clearIcon;
    return self;
}

void streamDock293_destory(struct streamDock* self)
{
    if (!self) return;
    // 子类没有可析构的东西
    /*  ... */
    // 析构基类
    streamDock_destory(self);
}

// 对上述声明好的函数进行定义
static int transform(int x) 
{
	if (x >= 1 && x <= 5) {
		return x + 10;
	}
	else if (x >= 6 && x <= 10) {
		return x;
	}
	else if (x >= 11 && x <= 15) {
		return x - 10;
	}
    return 1;
}

static unsigned char *rotate180_293(IplImage *image, const char * temp_filename, long *filesize)
{
    // 旋转图像 180°, 原地翻转可以直接在这里释放内存
    cvFlip(image, image, -1);
    // 将图像编码为 JPEG 格式并存储到内存缓冲区
    if (!cvSaveImage(temp_filename, image, 0)) {
        fprintf(stderr, "Failed to save image as JPEG.\n");
        return NULL;
    }
    // 读取JPEG文件并将其加载到缓冲区中
    FILE* file = fopen(temp_filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open temporary file: %s\n", temp_filename);
        return NULL;
    }
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    *filesize = ftell(file);
    rewind(file);
    unsigned char* buffer = (unsigned char*)malloc(*filesize);
    if (!buffer) {
        fprintf(stderr, "streamDock293 Memory allocation failed for buffer.\n");
        fclose(file);
        return NULL;
    }
    // 读取文件数据到缓冲区
    if (fread(buffer, 1, *filesize, file) != *filesize) {
        fprintf(stderr, "streamDock293 Failed to read image data from file.\n");
        free(buffer);
        fclose(file);
        return NULL;
    }
    fclose(file);
    remove(temp_filename);
    return buffer;
}

static unsigned char* streamDock293_getFirmVersion(struct streamDock* self, int lenth)
{
    if (!self) return NULL;
	return tranSportGetInputReport(self->transport, lenth);
}

static int streamDock293_setBrightness(struct streamDock* self, int percent)
{
    if (!self) return -1;
	return tranSportSetBrightness(self->transport, percent);
}

static int streamDock293_setBackgroundImg(struct streamDock* self, const char* path)
{
    if (!self) return -1;
    // 读取图像文件
    IplImage* image = cvLoadImage(path, CV_LOAD_IMAGE_COLOR);
    if (image == NULL) {
        fprintf(stderr, "streamDock293_setBackgroundImg malloc error.\n");
        return -1;
    }
    if (image->width != 800 || image->height != 480)
    {
        // 传入图片长宽不合适
        cvReleaseImage(&image);
        fprintf(stderr, "the picture size is 800 * 480, yours isn't suitable\n");
        return -1;
    }
    int size = image->height * image->width * 3; // 图像总像素数量 * 每个像素的3个通道 (BGR)
    unsigned char* buffer = (unsigned char*)malloc(size); 
    if (buffer == NULL) {
        cvReleaseImage(&image);
        return -1;
    }
    // 遍历图像的每个像素
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // 计算每个像素在一维数组中的位置
            int offset = (y * image->widthStep) + (x * 3); // BGR图像，每个像素3字节
            int buffer_offset = size - y * image->width * 3 - x * 3 - 3;

            buffer[buffer_offset] = (unsigned char)image->imageData[offset + 0]; // B
            buffer[buffer_offset + 1] = (unsigned char)image->imageData[offset + 1]; // G
            buffer[buffer_offset + 2] = (unsigned char)image->imageData[offset + 2]; // R
        }
    }
    int result = tranSportSetBackgroundImg(self->transport, buffer, size);
    // 释放分配的资源
    free(buffer);
    cvReleaseImage(&image);
    return result;
}
static int streamDock293_setBackgroundImgData(struct streamDock* self, unsigned char* imagedata)
{
    if (!self) return -1;
    int height = 480;
    int width = 800;
    IplImage* image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    if (!image) {
        fprintf(stderr, "streamDock293_setBackgroundImgData malloc error.\n");
        return -1;
    }
    //if (strlen(image->imageData) != width * height * 3)
    //{
    //    // 传入图片长宽不合适
    //    cvReleaseImage(&image);
    //    fprintf(stderr, "the picture size is 800 * 480, yours isn't suitable\n");
    //    return -1;
    //}
    // 将入参的图像数据复制到 IplImage 中
    memcpy(image->imageData, imagedata, width * height * 3);
    int size = image->height * image->width * 3;
    unsigned char* buffer = (unsigned char*)malloc(size);
    if (!buffer) {
        fprintf(stderr, "streamDock293_setBackgroundImgData malloc error for buffer.\n");
        cvReleaseImage(&image);
        return -1;
    }
    // 遍历图像的每个像素
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // 获取像素点的BGR值
            unsigned char* pixel = (unsigned char*)(image->imageData + y * image->widthStep + x * 3);

            // 提取BGR分量
            buffer[size - y * image->width * 3 - x * 3 - 3] = pixel[0];
            buffer[size - y * image->width * 3 - x * 3 - 2] = pixel[1];
            buffer[size - y * image->width * 3 - x * 3 - 1] = pixel[2];
        }
    }
    // 调用给实例的 setBackgroundImg 函数
    int result = tranSportSetBackgroundImg(self->transport, buffer, size);
    // 释放内存
    free(buffer);
    cvReleaseImage(&image);
    return result;
}

// you should the return pointer
static unsigned char* streamDock293_read(struct streamDock* self)
{
    if (!self) return NULL;
    //unsigned char* command = new unsigned char(13);
    unsigned char* command = (unsigned char*)malloc(sizeof(unsigned char) * 13);
    if (!command)
    {
        fprintf(stderr, "streamDock293_readToVector malloc error...\n");
    }
    //if (this->transport->read(command, 13) != -1)
    if (tranSportRead(self->transport, command, 13) != -1)
        return command;
    else
        return NULL;
}

static void streamDock293_readToVector(struct streamDock* self, unsigned char* vec)
{
    if (!self) return;
    if (!vec) return;
    // clear vec
    memset(vec, 0, sizeof(unsigned char) * 13);
    //unsigned char* command = new unsigned char(13);
    unsigned char command[13] = { 0 };
    //if (this->transport->read(command, 13) != -1)
    if (tranSportRead(self->transport, command, 13) != -1)
    {
        for (int i = 0; i < 13; i++)
        {
            //vec.push_back((int)command[i]);
            vec[i] = (int)command[i];
        }
    }
}

static int streamDock293_setKeyImg(struct streamDock* self, const char* path, int key)
{
    if (!self) return -1;
    key = transform(key);

    // 加载图像，保留透明通道
    IplImage* image = cvLoadImage(path, CV_LOAD_IMAGE_UNCHANGED);
    if (!image) {
        fprintf(stderr, "Unable to load image: %s\n", path);
        return -1;
    }
    if (image->height != 100 || image->width != 100)
    {
        // 传入图片长宽不合适
        cvReleaseImage(&image);
        fprintf(stderr, "the picture size is 100 * 100, yours isn't suitable\n");
        return -1;
    }
    // 检查图像是否包含 4 个通道（BGRA）
    if (image->nChannels == 4) {
        // 分离通道（B, G, R, A）
        IplImage* bChannel = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        IplImage* gChannel = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        IplImage* rChannel = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        IplImage* aChannel = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);

        cvSplit(image, bChannel, gChannel, rChannel, aChannel);

        // 创建一个不带 alpha 通道的图像
        IplImage* imageBGR = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
        cvMerge(bChannel, gChannel, rChannel, NULL, imageBGR);

        // 遍历每个像素，处理透明部分
        for (int y = 0; y < imageBGR->height; y++) {
            for (int x = 0; x < imageBGR->width; x++) {
                uchar* alphaPixel = (uchar*)(aChannel->imageData + y * aChannel->widthStep + x);
                uchar alphaValue = *alphaPixel;
                if (alphaValue < 255) { // 半透明或完全透明
                    uchar* pixel = (uchar*)(imageBGR->imageData + y * imageBGR->widthStep + x * 3);
                    double blendFactor = alphaValue / 255.0;
                    pixel[0] = (uchar)(pixel[0] * blendFactor); // B
                    pixel[1] = (uchar)(pixel[1] * blendFactor); // G
                    pixel[2] = (uchar)(pixel[2] * blendFactor); // R
                }
            }
        }

        // 将 imageBGR 替换原始 image
        cvReleaseImage(&image);
        image = imageBGR;

        // 释放分离的通道
        cvReleaseImage(&bChannel);
        cvReleaseImage(&gChannel);
        cvReleaseImage(&rChannel);
        cvReleaseImage(&aChannel);
    }
    // 旋转图片180度后发送
    long filesize = 0;
    const char* temp_filename = "293_tmp_key.jpg";
    unsigned char* buffer = rotate180_293(image, temp_filename, &filesize);
    int height = 100;
    int width = 100;
    int result = tranSportSetKeyImgData(self->transport, buffer, filesize, key, width, height);
    // 释放内存
    free(buffer);
    cvReleaseImage(&image);
    return result;
}

static int streamDock293_setKeyImgData(struct streamDock* self, unsigned char* imagedata, int key)
{
    if (!self) return -1;
    key = transform(key);

    int height = 100;
    int width = 100;

    IplImage* image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    if (!image) {
        fprintf(stderr, "Failed to create IplImage.\n");
        return -1;
    }
    memcpy(image->imageData, imagedata, width * height * 3);
    // 旋转图片180度后发送
    long filesize = 0;
    const char* temp_filename = "293_tmp_key_data.jpg";
    unsigned char* buffer = rotate180_293(image, temp_filename, &filesize);
    int result = tranSportSetKeyImgData(self->transport, buffer, filesize, key, width, height);
    // 释放内存
    free(buffer);
    cvReleaseImage(&image);
    return result;
}

static int streamDock293_clearIcon(struct streamDock* self, int index)
{
    if (!self) return -1;
    int key = transform(index);
    //return this->transport->keyClear(key);
    return tranSportKeyClear(self->transport, key);
}