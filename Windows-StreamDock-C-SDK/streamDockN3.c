#include "streamDock.h"
#include "streamDockN3.h"
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
//static streamDock* streamDockN3_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo);
//static void streamDockN3_destory(struct streamDock* self);
static unsigned char* streamDockN3_getFirmVersion(struct streamDock* self, int lenth);
static int streamDockN3_setBrightness(struct streamDock* self, int percent);
static int streamDockN3_setBackgroundImg(struct streamDock* self, const char* path);
static int streamDockN3_setBackgroundImgData(struct streamDock* self, unsigned char* imagedata);
static unsigned char* streamDockN3_read(struct streamDock* self);
static void streamDockN3_readToVector(struct streamDock* self, unsigned char* vec);
static int streamDockN3_setKeyImg(struct streamDock* self, const char* path, int key);
static int streamDockN3_setKeyImgData(struct streamDock* self, unsigned char* imagedata, int key);

// 子类(streamDock293)全局变量, 析构函数不需要重写了, extern到外面后直接init就能用了
//streamDock streamDock293 = {
//    .init = streamDockN3_init,
//    .destory = streamDockN3_destory
//};

streamDock* streamDockN3_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo)
{
    // 构造基类
    self = streamDock_init(self, transport, devInfo);
    // 构造子类
    self->init = streamDockN3_init;
    self->destory = streamDockN3_destory;         // 内部使用基类的析构函数
    self->getFirmVersion = streamDockN3_getFirmVersion;
    self->setBrightness = streamDockN3_setBrightness;
    self->setBackgroundImg = streamDockN3_setBackgroundImg;
    self->setBackgroundImgData = streamDockN3_setBackgroundImgData;
    self->read = streamDockN3_read;
    self->readToVector = streamDockN3_readToVector;
    self->setKeyImg = streamDockN3_setKeyImg;
    self->setKeyImgData = streamDockN3_setKeyImgData;
    return self;
}

void streamDockN3_destory(struct streamDock* self)
{
    if (!self) return;
    // 子类没有可析构的东西
    /*  ... */
    // 析构基类
    streamDock_destory(self);
}

static unsigned char* rotate90_N3(IplImage* image, const char* temp_filename, long* filesize)
{
    // 创建一个新图像来存储转置结果（与原始图像的尺寸不同，宽高需要交换）
    IplImage* rotated_image = cvCreateImage(cvSize(image->height, image->width), image->depth, image->nChannels);

    // 1. 旋转图像 90° 顺时针 (转置 + 水平翻转)
    cvTranspose(image, rotated_image);   // 1. 转置
    cvFlip(rotated_image, rotated_image, 1); // 2. 水平翻转

    // 2. 将图像编码为 JPEG 格式并存储到临时文件中
    if (!cvSaveImage(temp_filename, rotated_image, 0)) {
        fprintf(stderr, "Failed to save image as JPEG.\n");
        cvReleaseImage(&rotated_image);
        return NULL;
    }

    // 3. 读取 JPEG 文件并将其加载到缓冲区中
    FILE* file = fopen(temp_filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open temporary file: %s\n", temp_filename);
        cvReleaseImage(&rotated_image);
        return NULL;
    }

    // 4. 获取文件大小
    fseek(file, 0, SEEK_END);
    *filesize = ftell(file);
    rewind(file);

    // 5. 为缓冲区分配内存
    unsigned char* buffer = (unsigned char*)malloc(*filesize);
    if (!buffer) {
        fprintf(stderr, "streamDockN3 Memory allocation failed for buffer.\n");
        fclose(file);
        cvReleaseImage(&rotated_image);
        return NULL;
    }

    // 6. 读取文件数据到缓冲区
    if (fread(buffer, 1, *filesize, file) != *filesize) {
        fprintf(stderr, "streamDockN3 Failed to read image data from file.\n");
        free(buffer);
        fclose(file);
        cvReleaseImage(&rotated_image);
        return NULL;
    }

    // 7. 关闭文件, 不删除临时文件，外面要用
    fclose(file);
    //remove(temp_filename);

    // 8. 释放图像内存
    cvReleaseImage(&rotated_image);
    return buffer;
}

static void rotate90_N3_BG(IplImage* image, const char* temp_filename)
{
    // 1. 创建旋转后的图像 (注意宽高交换)
    IplImage* rotated_image = cvCreateImage(cvSize(image->height, image->width), image->depth, image->nChannels);
    if (!rotated_image) {
        fprintf(stderr, "Failed to create rotated image.\n");
        cvReleaseImage(&image);
        return;
    }

    // 2. 旋转 90° 顺时针 (转置 + 水平翻转)
    cvTranspose(image, rotated_image);     // 先转置
    cvFlip(rotated_image, rotated_image, 1); // 再水平翻转

    // 3. 将图像编码为 JPEG 格式并存储到临时文件中
    if (!cvSaveImage(temp_filename, rotated_image, 0)) {
        fprintf(stderr, "Failed to save image as JPEG.\n");
        cvReleaseImage(&image);
        cvReleaseImage(&rotated_image);
        return;
    }

    // 4. 读取 JPEG 文件以确认文件是否正确写入 (可选)
    FILE* file = fopen(temp_filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open temporary file: %s\n", temp_filename);
        cvReleaseImage(&image);
        cvReleaseImage(&rotated_image);
        return;
    }

    // 5. 关闭文件
    fclose(file);

    // 6. 释放图像资源
    cvReleaseImage(&rotated_image);
}

static unsigned char* streamDockN3_getFirmVersion(streamDock* self, int lenth)
{
    if (!self) return NULL;
    return tranSportGetInputReport(self->transport, lenth);
}

static int streamDockN3_setBrightness(streamDock* self, int percent)
{
    if (!self) return -1;
    return tranSportSetBrightness(self->transport, percent);
}

static int streamDockN3_setBackgroundImg(streamDock* self, const char* path)
{
    if (!self) return -1;
    // 读取图像文件
    IplImage* image = cvLoadImage(path, CV_LOAD_IMAGE_COLOR);
    if (image == NULL) {
        fprintf(stderr, "streamDockN3_setBackgroundImg malloc error.\n");
        return -1;
    }
    if (image->width != 320 || image->height != 240)
    {
        // 传入图片长宽不合适
        cvReleaseImage(&image);
        fprintf(stderr, "the picture size is 800 * 480, yours isn't suitable\n");
        return -1;
    }
    // 旋转后发送
    const char* temp_filename = "N3_tmp_background.jpg";
    rotate90_N3_BG(image, temp_filename);
    int res = tranSportSetBackgroundImgDualDevice(self->transport, temp_filename);
    remove(temp_filename);
    // 释放资源
    cvReleaseImage(&image);
    return res;
}

static int streamDockN3_setBackgroundImgData(streamDock* self, unsigned char* imagedata)
{
    if (!self) return -1;
    int height = 240;
    int width = 320;

    // 创建图像
    IplImage* image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    if (!image) {
        fprintf(stderr, "streamDockN3_setBackgroundImgData malloc error.\n");
        return -1;
    }
    // 将入参的图像数据复制到 IplImage 中
    memcpy(image->imageData, imagedata, width * height * 3);
    // 旋转后发送
    const char* temp_filename = "N3_tmp_background_data.jpg";
    rotate90_N3_BG(image, temp_filename);
    int res = tranSportSetBackgroundImgDualDevice(self->transport, temp_filename);
    remove(temp_filename);
    // 释放资源
    cvReleaseImage(&image);
    return res;
}

static unsigned char* streamDockN3_read(streamDock* self)
{
    if (!self) return NULL;
    //unsigned char* command = new unsigned char(13);
    unsigned char* command = (unsigned char*)malloc(sizeof(unsigned char) * 13);
    if (!command)
    {
        fprintf(stderr, "N3_readToVector malloc error...\n");
    }
    //if (this->transport->read(command, 13) != -1)
    if (tranSportRead(self->transport, command, 13) != -1)
        return command;
    else
        return NULL;
}

static void streamDockN3_readToVector(streamDock* self, unsigned char* vec)
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

static int streamDockN3_setKeyImg(streamDock* self, const char* path, int key)
{
    if (!self) return -1;

    // 加载图像，保留透明通道
    IplImage* image = cvLoadImage(path, CV_LOAD_IMAGE_UNCHANGED);
    if (!image) {
        fprintf(stderr, "Unable to load image: %s\n", path);
        return -1;
    }
    if (image->height != 64 || image->width != 64)
    {
        // 传入图片长宽不合适
        cvReleaseImage(&image);
        fprintf(stderr, "the picture size is 64 * 64, yours isn't suitable\n");
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
    // 旋转图片90度后发送
    long filesize = 0;
    const char* temp_filename = "N3_tmp_key.jpg";
    unsigned char* buffer = rotate90_N3(image, temp_filename, &filesize);
    int result = tranSportSetKeyImgDualDevice(self->transport, temp_filename, key);
    // 释放内存
    free(buffer);
    cvReleaseImage(&image);
    remove(temp_filename);
    return result;
}

static int streamDockN3_setKeyImgData(streamDock* self, unsigned char* imagedata, int key)
{
    if (!self) return -1;

    int height = 64;
    int width = 64;

    IplImage* image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    if (!image) {
        fprintf(stderr, "Failed to create IplImage.\n");
        return -1;
    }
    memcpy(image->imageData, imagedata, width * height * 3);
    // 旋转图片90度后发送
    long filesize = 0;
    const char* temp_filename = "N3_tmp_key_data.jpg";
    unsigned char* buffer = rotate90_N3(image, temp_filename, &filesize);
    int result = tranSportSetKeyImgDualDevice(self->transport, temp_filename, key);
    // 释放内存
    free(buffer);
    cvReleaseImage(&image);
    remove(temp_filename);
    return result;
}

