#ifndef IMAGECOMPRESSOR_H
#define IMAGECOMPRESSOR_H

#include <string>
#include "QuadTreeNode.h"

class QuadTreeCompressor {
private:
    unsigned char* imageData;
    int imgWidth, imgHeight, channels;
    QuadTreeNode* root;
    int minBlockSize;
    double varianceThreshold;
    int errorMethod; // variance, MAD, maximum pixel difference, entropy

public:
    QuadTreeCompressor(const std::string& inputPath, int minSize, double threshold, int method);
    ~QuadTreeCompressor();
    void saveCompressedImage(const std::string& outputPath);
};

#endif