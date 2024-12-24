#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "rotateimg.h"

// Function for rotating the image 180 degrees
void rotate(IplImage* img1, IplImage* img2)
{
    int rows = img1->height;
    int cols = img2->width;

    // Traverse all pixels and flip them 180 degrees
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // Set the pixel at the new position
            cvSet2D(img2, rows - 1 - y, cols - 1 - x, cvGet2D(img1, y, x));
        }
    }
}

// Return the image rotated by 180 degrees
IplImage* rotateEx(IplImage* img)
{
    // Get the number of rows and columns of the original image
    int rows = img->height;
    int cols = img->width;

    // Create a new image to store the rotated result
    IplImage* rotated_img = cvCreateImage(cvSize(cols, rows), IPL_DEPTH_8U, 3);

    // Perform the rotation
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // Place the pixel (x, y) from img to the position (cols - 1 - x, rows - 1 - y) in rotated_img
            cvSet2D(rotated_img, rows - 1 - y, cols - 1 - x, cvGet2D(img, y, x));
        }
    }

    // Return the rotated image
    return rotated_img;
}

// Function for rotating the image 90 degrees clockwise
void rotate90_clockWise(IplImage* img1, IplImage* img2)
{
    int rows = img1->height;
    int cols = img1->width;

    // Target image size is cols x rows
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // Rotate 90 degrees clockwise, new coordinates are (x, rows - 1 - y)
            cvSet2D(img2, x, rows - 1 - y, cvGet2D(img1, y, x));
        }
    }
}

// Return the image rotated 90 degrees clockwise
IplImage* rotate90Ex_clockWise(IplImage* img)
{
    int rows = img->height;
    int cols = img->width;

    // Create a new image to store the rotated result, size is swapped to cols x rows
    IplImage* rotated_img = cvCreateImage(cvSize(rows, cols), IPL_DEPTH_8U, 3);

    // Perform the 90-degree clockwise rotation
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // Rotate 90 degrees clockwise, new coordinates are (x, rows - 1 - y)
            cvSet2D(rotated_img, x, rows - 1 - y, cvGet2D(img, y, x));
        }
    }

    // Return the rotated image
    return rotated_img;
}

// Function for rotating the image 90 degrees counterclockwise
void rotate90_counterClockwise(IplImage* img1, IplImage* img2)
{
    int rows = img1->height;
    int cols = img1->width;

    // Target image size is cols x rows
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // Rotate 90 degrees counterclockwise, new coordinates are (cols - 1 - x, y)
            cvSet2D(img2, cols - 1 - x, y, cvGet2D(img1, y, x));
        }
    }
}

// Return the image rotated 90 degrees counterclockwise
IplImage* rotate90_counterClockwiseEx(IplImage* img)
{
    int rows = img->height;
    int cols = img->width;

    // Create a new image to store the rotated result, size is swapped to cols x rows
    IplImage* rotated_img = cvCreateImage(cvSize(cols, rows), IPL_DEPTH_8U, 3);

    // Perform the 90-degree counterclockwise rotation
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // Rotate 90 degrees counterclockwise, new coordinates are (cols - 1 - x, y)
            cvSet2D(rotated_img, cols - 1 - x, y, cvGet2D(img, y, x));
        }
    }

    // Return the rotated image
    return rotated_img;
}

// Function for horizontal flipping (left-right mirror)
void flipHorizontal(IplImage* img1, IplImage* img2)
{
    int rows = img1->height;
    int cols = img1->width;

    // Traverse each row and flip the pixels in the row
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols / 2; x++) {
            // Swap horizontally positioned pixels
            CvScalar pixel1 = cvGet2D(img1, y, x);
            CvScalar pixel2 = cvGet2D(img1, y, cols - 1 - x);
            cvSet2D(img2, y, x, pixel2);
            cvSet2D(img2, y, cols - 1 - x, pixel1);
        }
    }
}

// Return the horizontally flipped image
IplImage* flipHorizontalEx(IplImage* img)
{
    int rows = img->height;
    int cols = img->width;

    // Create a new image to store the flipped result
    IplImage* flipped_img = cvCreateImage(cvSize(cols, rows), IPL_DEPTH_8U, 3);

    // Traverse each row and flip the pixels in the row
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols / 2; x++) {
            // Swap horizontally positioned pixels
            CvScalar pixel1 = cvGet2D(img, y, x);
            CvScalar pixel2 = cvGet2D(img, y, cols - 1 - x);
            cvSet2D(flipped_img, y, x, pixel2);
            cvSet2D(flipped_img, y, cols - 1 - x, pixel1);
        }
    }

    // Return the flipped image
    return flipped_img;
}

// Function for vertical flipping (top-bottom mirror)
void flipVertical(IplImage* img1, IplImage* img2)
{
    int rows = img1->height;
    int cols = img1->width;

    // Traverse each column and flip the pixels in the column
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows / 2; y++) {
            // Swap vertically positioned pixels
            CvScalar pixel1 = cvGet2D(img1, y, x);
            CvScalar pixel2 = cvGet2D(img1, rows - 1 - y, x);
            cvSet2D(img2, y, x, pixel2);
            cvSet2D(img2, rows - 1 - y, x, pixel1);
        }
    }
}

// Return the vertically flipped image
IplImage* flipVerticalEx(IplImage* img)
{
    int rows = img->height;
    int cols = img->width;

    // Create a new image to store the flipped result
    IplImage* flipped_img = cvCreateImage(cvSize(cols, rows), IPL_DEPTH_8U, 3);

    // Traverse each column and flip the pixels in the column
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows / 2; y++) {
            // Swap vertically positioned pixels
            CvScalar pixel1 = cvGet2D(img, y, x);
            CvScalar pixel2 = cvGet2D(img, rows - 1 - y, x);
            cvSet2D(flipped_img, y, x, pixel2);
            cvSet2D(flipped_img, rows - 1 - y, x, pixel1);
        }
    }

    // Return the flipped image
    return flipped_img;
}
