#ifndef IMAGE_OPERATIONS_H
#define IMAGE_OPERATIONS_H
#include <opencv/cv.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///		���: ���Է�תͼƬ(��ת180��, ˳ʱ����ʱ����ת90��, ˮƽ��ֱ����)
///		ע������: ����ֵΪ��void�ĺ���, ����ֵΪIplImage*�����ݱ����ֶ�ʹ��cvReleaseImage
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///		Description:The function can flip an image(rotate 180 degrees, rotate 90 degrees clockwise or counterclockwise, 
///					or perform horizontal / vertical mirroring).
///		Notes:The function returns a non - void value.When the return value is of type IplImage*, 
///					the image must be manually released using cvReleaseImage.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// Rotate the image by 180 degrees (in-place)
void rotate(IplImage* img1, IplImage* img2);

// Return a new image rotated by 180 degrees
IplImage* rotateEx(IplImage* img);

// Rotate the image 90 degrees clockwise (in-place)
void rotate90_clockWise(IplImage* img1, IplImage* img2);

// Return a new image rotated 90 degrees clockwise
IplImage* rotate90Ex_clockWise(IplImage* img);

// Rotate the image 90 degrees counterclockwise (in-place)
void rotate90_counterClockwise(IplImage* img1, IplImage* img2);

// Return a new image rotated 90 degrees counterclockwise
IplImage* rotate90_counterClockwiseEx(IplImage* img);

// Flip the image horizontally (left-right mirror) (in-place)
void flipHorizontal(IplImage* img1, IplImage* img2);

// Return a new horizontally flipped image
IplImage* flipHorizontalEx(IplImage* img);

// Flip the image vertically (top-bottom mirror) (in-place)
void flipVertical(IplImage* img1, IplImage* img2);

// Return a new vertically flipped image
IplImage* flipVerticalEx(IplImage* img);

#ifdef __cplusplus
}
#endif

#endif // IMAGE_OPERATIONS_H
