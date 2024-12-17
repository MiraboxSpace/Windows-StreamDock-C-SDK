#include "streamDock.h"
#include "streamDockN3.h"
#include "tranSport.h"
#include "hidapi.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
//#define CV_IGNORE_DEBUG_BUILD_GUARD		// C���Ի���DEBUGģʽ����������������ܱ���ͨ��
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/core/core_c.h> 
#include <opencv2/highgui/highgui_c.h> 
#include <stdbool.h>


// ������Ҫ��д�ĺ���
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

// ����(streamDock293)ȫ�ֱ���, ������������Ҫ��д��, extern�������ֱ��init��������
//streamDock streamDock293 = {
//    .init = streamDockN3_init,
//    .destory = streamDockN3_destory
//};

streamDock* streamDockN3_init(streamDock* self, tranSport* transport, struct hid_device_info* devInfo)
{
    // �������
    self = streamDock_init(self, transport, devInfo);
    // ��������
    self->init = streamDockN3_init;
    self->destory = streamDockN3_destory;         // �ڲ�ʹ�û������������
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
    // ����û�п������Ķ���
    /*  ... */
    // ��������
    streamDock_destory(self);
}

static unsigned char* rotate90_N3(IplImage* image, const char* temp_filename, long* filesize)
{
    // ����һ����ͼ�����洢ת�ý������ԭʼͼ��ĳߴ粻ͬ�������Ҫ������
    IplImage* rotated_image = cvCreateImage(cvSize(image->height, image->width), image->depth, image->nChannels);

    // 1. ��תͼ�� 90�� ˳ʱ�� (ת�� + ˮƽ��ת)
    cvTranspose(image, rotated_image);   // 1. ת��
    cvFlip(rotated_image, rotated_image, 1); // 2. ˮƽ��ת

    // 2. ��ͼ�����Ϊ JPEG ��ʽ���洢����ʱ�ļ���
    if (!cvSaveImage(temp_filename, rotated_image, 0)) {
        fprintf(stderr, "Failed to save image as JPEG.\n");
        cvReleaseImage(&rotated_image);
        return NULL;
    }

    // 3. ��ȡ JPEG �ļ���������ص���������
    FILE* file = fopen(temp_filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open temporary file: %s\n", temp_filename);
        cvReleaseImage(&rotated_image);
        return NULL;
    }

    // 4. ��ȡ�ļ���С
    fseek(file, 0, SEEK_END);
    *filesize = ftell(file);
    rewind(file);

    // 5. Ϊ�����������ڴ�
    unsigned char* buffer = (unsigned char*)malloc(*filesize);
    if (!buffer) {
        fprintf(stderr, "streamDockN3 Memory allocation failed for buffer.\n");
        fclose(file);
        cvReleaseImage(&rotated_image);
        return NULL;
    }

    // 6. ��ȡ�ļ����ݵ�������
    if (fread(buffer, 1, *filesize, file) != *filesize) {
        fprintf(stderr, "streamDockN3 Failed to read image data from file.\n");
        free(buffer);
        fclose(file);
        cvReleaseImage(&rotated_image);
        return NULL;
    }

    // 7. �ر��ļ�, ��ɾ����ʱ�ļ�������Ҫ��
    fclose(file);
    //remove(temp_filename);

    // 8. �ͷ�ͼ���ڴ�
    cvReleaseImage(&rotated_image);
    return buffer;
}

static void rotate90_N3_BG(IplImage* image, const char* temp_filename)
{
    // 1. ������ת���ͼ�� (ע���߽���)
    IplImage* rotated_image = cvCreateImage(cvSize(image->height, image->width), image->depth, image->nChannels);
    if (!rotated_image) {
        fprintf(stderr, "Failed to create rotated image.\n");
        cvReleaseImage(&image);
        return;
    }

    // 2. ��ת 90�� ˳ʱ�� (ת�� + ˮƽ��ת)
    cvTranspose(image, rotated_image);     // ��ת��
    cvFlip(rotated_image, rotated_image, 1); // ��ˮƽ��ת

    // 3. ��ͼ�����Ϊ JPEG ��ʽ���洢����ʱ�ļ���
    if (!cvSaveImage(temp_filename, rotated_image, 0)) {
        fprintf(stderr, "Failed to save image as JPEG.\n");
        cvReleaseImage(&image);
        cvReleaseImage(&rotated_image);
        return;
    }

    // 4. ��ȡ JPEG �ļ���ȷ���ļ��Ƿ���ȷд�� (��ѡ)
    FILE* file = fopen(temp_filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open temporary file: %s\n", temp_filename);
        cvReleaseImage(&image);
        cvReleaseImage(&rotated_image);
        return;
    }

    // 5. �ر��ļ�
    fclose(file);

    // 6. �ͷ�ͼ����Դ
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
    // ��ȡͼ���ļ�
    IplImage* image = cvLoadImage(path, CV_LOAD_IMAGE_COLOR);
    if (image == NULL) {
        fprintf(stderr, "streamDockN3_setBackgroundImg malloc error.\n");
        return -1;
    }
    if (image->width != 320 || image->height != 240)
    {
        // ����ͼƬ��������
        cvReleaseImage(&image);
        fprintf(stderr, "the picture size is 800 * 480, yours isn't suitable\n");
        return -1;
    }
    // ��ת����
    const char* temp_filename = "N3_tmp_background.jpg";
    rotate90_N3_BG(image, temp_filename);
    int res = tranSportSetBackgroundImgDualDevice(self->transport, temp_filename);
    remove(temp_filename);
    // �ͷ���Դ
    cvReleaseImage(&image);
    return res;
}

static int streamDockN3_setBackgroundImgData(streamDock* self, unsigned char* imagedata)
{
    if (!self) return -1;
    int height = 240;
    int width = 320;

    // ����ͼ��
    IplImage* image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    if (!image) {
        fprintf(stderr, "streamDockN3_setBackgroundImgData malloc error.\n");
        return -1;
    }
    // ����ε�ͼ�����ݸ��Ƶ� IplImage ��
    memcpy(image->imageData, imagedata, width * height * 3);
    // ��ת����
    const char* temp_filename = "N3_tmp_background_data.jpg";
    rotate90_N3_BG(image, temp_filename);
    int res = tranSportSetBackgroundImgDualDevice(self->transport, temp_filename);
    remove(temp_filename);
    // �ͷ���Դ
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

    // ����ͼ�񣬱���͸��ͨ��
    IplImage* image = cvLoadImage(path, CV_LOAD_IMAGE_UNCHANGED);
    if (!image) {
        fprintf(stderr, "Unable to load image: %s\n", path);
        return -1;
    }
    if (image->height != 64 || image->width != 64)
    {
        // ����ͼƬ��������
        cvReleaseImage(&image);
        fprintf(stderr, "the picture size is 64 * 64, yours isn't suitable\n");
        return -1;
    }
    // ���ͼ���Ƿ���� 4 ��ͨ����BGRA��
    if (image->nChannels == 4) {
        // ����ͨ����B, G, R, A��
        IplImage* bChannel = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        IplImage* gChannel = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        IplImage* rChannel = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        IplImage* aChannel = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);

        cvSplit(image, bChannel, gChannel, rChannel, aChannel);

        // ����һ������ alpha ͨ����ͼ��
        IplImage* imageBGR = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
        cvMerge(bChannel, gChannel, rChannel, NULL, imageBGR);

        // ����ÿ�����أ�����͸������
        for (int y = 0; y < imageBGR->height; y++) {
            for (int x = 0; x < imageBGR->width; x++) {
                uchar* alphaPixel = (uchar*)(aChannel->imageData + y * aChannel->widthStep + x);
                uchar alphaValue = *alphaPixel;
                if (alphaValue < 255) { // ��͸������ȫ͸��
                    uchar* pixel = (uchar*)(imageBGR->imageData + y * imageBGR->widthStep + x * 3);
                    double blendFactor = alphaValue / 255.0;
                    pixel[0] = (uchar)(pixel[0] * blendFactor); // B
                    pixel[1] = (uchar)(pixel[1] * blendFactor); // G
                    pixel[2] = (uchar)(pixel[2] * blendFactor); // R
                }
            }
        }

        // �� imageBGR �滻ԭʼ image
        cvReleaseImage(&image);
        image = imageBGR;

        // �ͷŷ����ͨ��
        cvReleaseImage(&bChannel);
        cvReleaseImage(&gChannel);
        cvReleaseImage(&rChannel);
        cvReleaseImage(&aChannel);
    }
    // ��תͼƬ90�Ⱥ���
    long filesize = 0;
    const char* temp_filename = "N3_tmp_key.jpg";
    unsigned char* buffer = rotate90_N3(image, temp_filename, &filesize);
    int result = tranSportSetKeyImgDualDevice(self->transport, temp_filename, key);
    // �ͷ��ڴ�
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
    // ��תͼƬ90�Ⱥ���
    long filesize = 0;
    const char* temp_filename = "N3_tmp_key_data.jpg";
    unsigned char* buffer = rotate90_N3(image, temp_filename, &filesize);
    int result = tranSportSetKeyImgDualDevice(self->transport, temp_filename, key);
    // �ͷ��ڴ�
    free(buffer);
    cvReleaseImage(&image);
    remove(temp_filename);
    return result;
}

