#include "streamDock.h"
#include "streamDock293V3.h"
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
static int transform(int x);
static unsigned char* streamDock293V3_getFirmVersion(struct streamDock* self, int lenth);
static int streamDock293V3_setBrightness(struct streamDock* self, int percent);
static int streamDock293V3_setBackgroundImg(struct streamDock* self, const char* path);
static int streamDock293V3_setBackgroundImgData(struct streamDock* self, unsigned char* imagedata);
static unsigned char* streamDock293V3_read(struct streamDock* self);
static void streamDock293V3_readToVector(struct streamDock* self, unsigned char* vec);
static int streamDock293V3_setKeyImg(struct streamDock* self, const char* path, int key);
static int streamDock293V3_setKeyImgData(struct streamDock* self, unsigned char* imagedata, int key);
static int streamDock293V3_clearIcon(struct streamDock* self, int index);


streamDock* streamDock293V3_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo)
{
    // 构造基类
    self = streamDock_init(self, transport, devInfo);
    // 构造子类
    self->init = streamDock293V3_init;
    self->destory = streamDock293V3_destory;         // 内部使用基类的析构函数
    self->getFirmVersion = streamDock293V3_getFirmVersion;
    self->setBrightness = streamDock293V3_setBrightness;
    self->setBackgroundImg = streamDock293V3_setBackgroundImg;
    self->setBackgroundImgData = streamDock293V3_setBackgroundImgData;
    self->read = streamDock293V3_read;
    self->readToVector = streamDock293V3_readToVector;
    self->setKeyImg = streamDock293V3_setKeyImg;
    self->setKeyImgData = streamDock293V3_setKeyImgData;
    self->clearIcon = streamDock293V3_clearIcon;
    return self;
}

void streamDock293V3_destory(streamDock* self)
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

// 返回值需要释放
static IplImage* rotate180_293V3(IplImage* image, const char* temp_filename, int width, int height)
{
    //  复制图像，便于操作翻转
    IplImage* backup_image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    if (!image || !backup_image) {
        fprintf(stderr, "Failed to create IplImage.\n");
        if (backup_image) cvReleaseImage(&backup_image);
        return -1;
    }
    //  使用 cvFlip 实现 180° 旋转 (-1 表示 x 和 y 翻转)
    cvFlip(image, backup_image, -1);
    //  保存图像到磁盘
    if (!cvSaveImage(temp_filename, backup_image, 0)) {
        fprintf(stderr, "Failed to save image as %s\n", temp_filename);
        cvReleaseImage(&backup_image);
        return -1;
    }
    return backup_image;
}

static void rotate180_293V3_BG(IplImage* image, const char* temp_filename)
{
    // 旋转图像 180°
    cvFlip(image, image, -1);
    // 将图像编码为 JPEG 格式并存储到内存缓冲区
    if (!cvSaveImage(temp_filename, image, 0)) {
        fprintf(stderr, "Failed to save image as JPEG.\n");
        cvReleaseImage(&image);
        return NULL;
    }

    // 读取JPEG文件并将其加载到缓冲区中
    FILE* file = fopen(temp_filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open temporary file: %s\n", temp_filename);
        cvReleaseImage(&image);
        return NULL;
    }
    fclose(file);
}


unsigned char* streamDock293V3_getFirmVersion(streamDock* self, int lenth)
{
    if (!self) return NULL;
    return tranSportGetInputReport(self->transport, lenth);
}

int streamDock293V3_setBrightness(streamDock* self, int percent)
{
    if (!self) return -1;
    return tranSportSetBrightness(self->transport, percent);
}

int streamDock293V3_setBackgroundImg(streamDock* self, const char* path)
{
    if (!self) return -1;
    // 读取图像文件
    IplImage* image = cvLoadImage(path, CV_LOAD_IMAGE_COLOR);
    if (image == NULL) {
        fprintf(stderr, "streamDock293V3_setBackgroundImg malloc error.\n");
        return -1;
    }
    if (image->width != 800 || image->height != 480)
    {
        // 传入图片长宽不合适
        cvReleaseImage(&image);
        fprintf(stderr, "the picture size is 800 * 480, yours isn't suitable\n");
        return -1;
    }
    // 旋转后发送
    const char* temp_filename = "293V3_tmp_background.jpg";
    rotate180_293V3_BG(image, temp_filename);
    int res = tranSportSetBackgroundImgDualDevice(self->transport, temp_filename);
    remove(temp_filename);
    // 释放资源
    cvReleaseImage(&image);
    return res;
}

int streamDock293V3_setBackgroundImgData(streamDock* self, unsigned char* imagedata)
{
    if (!self) return -1;
    int height = 480;
    int width = 800;

    // 创建图像
    IplImage* image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    if (!image) {
        fprintf(stderr, "streamDock293V3_setBackgroundImgData malloc error.\n");
        return -1;
    }
    // 将入参的图像数据复制到 IplImage 中
    memcpy(image->imageData, imagedata, width * height * 3);
    // 旋转后发送
    const char* temp_filename = "293V3_tmp_background_data.jpg";
    rotate180_293V3_BG(image, temp_filename);
    int res = tranSportSetBackgroundImgDualDevice(self->transport, temp_filename);
    remove(temp_filename);
    // 释放资源
    cvReleaseImage(&image);
    return res;
}

// you should free the return pointer
unsigned char* streamDock293V3_read(streamDock* self)
{
    if (!self) return NULL;
    //unsigned char* command = new unsigned char(13);
    unsigned char* command = (unsigned char*)malloc(sizeof(unsigned char) * 13);
    if (!command)
    {
        fprintf(stderr, "streamDock293V3_readToVector malloc error...\n");
    }
    //if (this->transport->read(command, 13) != -1)
    if (tranSportRead(self->transport, command, 13) != -1)
        return command;
    else
        return NULL;
}

void streamDock293V3_readToVector(streamDock* self, unsigned char* vec)
{
    if (!self) return;
    if (!vec) return;
    // clear vec
    memset(vec, 0, sizeof(unsigned char) * 13);
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

int streamDock293V3_setKeyImg(streamDock* self, const char* path, int key)
{
    if (!self) return -1;
    key = transform(key);

    // 加载图像，保留透明通道
    IplImage* image = cvLoadImage(path, CV_LOAD_IMAGE_UNCHANGED);
    if (!image) {
        fprintf(stderr, "Unable to load image: %s\n", path);
        return -1;
    }
    if (image->width != 112 || image->height != 112)
    {
        // 传入图片长宽不合适
        cvReleaseImage(&image);
        fprintf(stderr, "the picture size is 112 * 112, yours isn't suitable\n");
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
    return streamDock293V3_setKeyImgData(self, image->imageData, key);
}

int streamDock293V3_setKeyImgData(streamDock* self, unsigned char* imagedata, int key)
{
    if (!self) return -1;
    key = transform(key);
    int height = 112;
    int width = 112;
    //  创建 IplImage 并将数据与 IplImage 关联
    IplImage* image = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);
    image->imageData = (char*)imagedata;
    const char *temp_filename = "293V3_tmp_key.jpg";

    // 旋转180度后发送
    IplImage* backup_image = rotate180_293V3(image, temp_filename, width, height);
    //  调用 transport->setKeyImgDataDualDevice 
    int result = tranSportSetKeyImgDataDualDevice(self->transport, temp_filename, key);
    // 删除临时图像文件
    if (remove(temp_filename) != 0) {
        fprintf(stderr, "Failed to delete file %s\n", temp_filename);
    }
    // 释放图像和内存
    cvReleaseImage(&image);
    cvReleaseImage(&backup_image);     
    return result;
}

int streamDock293V3_clearIcon(streamDock* self, int index)
{
    if (!self) return -1;
    int key = transform(index);
    //return this->transport->keyClear(key);
    return tranSportKeyClear(self->transport, key);
}
