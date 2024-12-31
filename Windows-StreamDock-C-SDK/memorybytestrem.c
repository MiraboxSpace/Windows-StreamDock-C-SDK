#include "memorybytestrem.h"

// Save image to memory as byte stream (encoded)
// override return a unsigned char *buffer
uchar* saveImageToMemory(IplImage* img, int* buffer_size, int quality) {
    quality = (quality > 95) ? 95 : quality;
    int params[] = { CV_IMWRITE_JPEG_QUALITY, quality };  // Quality set

    // Encode the image to a byte stream (CvMat)
    CvMat* encoded_image = cvEncodeImage(".jpg", img, params);

    // If encoding is successful, copy the byte data
    if (encoded_image != NULL) {
        *buffer_size = encoded_image->rows * encoded_image->cols;  // Get the size of the encoded image

        // Allocate independent memory for the encoded image
        uchar* data_ptr = (uchar*)malloc(*buffer_size);
        if (data_ptr != NULL) {
            memcpy(data_ptr, encoded_image->data.ptr, *buffer_size);  // Copy the byte data
        }
        else {
            printf("Memory allocation failed.\n");
            *buffer_size = 0;
        }

        // Release the CvMat object since it's no longer needed
        cvReleaseMat(&encoded_image);

        return data_ptr;  // Return the byte data
    }
    else {
        printf("Failed to encode image.\n");
        *buffer_size = 0;  // Set buffer size to 0 if encoding fails
        return NULL;
    }
}

// Write byte stream to file
void writeToFile(const uchar* buffer, int buffer_size, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file) {
        fwrite(buffer, sizeof(uchar), buffer_size, file);  // Write byte stream to file
        fclose(file);
        printf("Image written to file: %s\n", filename);
    }
    else {
        printf("Failed to write image to file.\n");
    }
}

// Load image from byte stream (decode from memory)
// Function to load image from memory buffer
IplImage* loadImageFromMemory(const uchar* buffer, size_t buffer_size) {
    // Create a CvMat object to hold the image data from memory
    CvMat* mat = cvCreateMatHeader(1, buffer_size, CV_8U);
    cvSetData(mat, (void*)buffer, buffer_size);

    // Decode the image from the memory buffer
    IplImage* img = cvDecodeImage(mat, CV_LOAD_IMAGE_COLOR);

    // Release the CvMat object
    cvReleaseMat(&mat);

    return img;
}