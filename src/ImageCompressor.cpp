#include "ImageCompressor.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

QuadTreeCompressor::QuadTreeCompressor(const std::string& inputPath, int minSize, double threshold, int method) {
    minBlockSize = minSize;
    varianceThreshold = threshold;
    errorMethod = method;

    // Load image
    imageData = stbi_load(inputPath.c_str(), &imgWidth, &imgHeight, &channels, 3); // 3 channels for RGB
    if (!imageData) {
        std::cerr << "Error: Could not load image " << inputPath << std::endl;
        exit(1);
    }

    // Initialize root node
    root = new QuadTreeNode(0, 0, imgWidth, imgHeight);
}

QuadTreeCompressor::~QuadTreeCompressor() {
    stbi_image_free(imageData);
    delete root;
}

void QuadTreeCompressor::saveCompressedImage(const std::string& outputPath) {
    // placeholder: save the image (implemented otw)
    unsigned char* outputData = new unsigned char[imgWidth * imgHeight * 3];
    // copying the original image for now
    for (int i = 0; i < imgWidth * imgHeight * 3; i++) {
        outputData[i] = imageData[i];
    }
    stbi_write_png(outputPath.c_str(), imgWidth, imgHeight, 3, outputData, imgWidth * 3);
    delete[] outputData;
}