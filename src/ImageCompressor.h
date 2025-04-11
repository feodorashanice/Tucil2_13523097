// ImageCompressor.h
#ifndef IMAGE_COMPRESSOR_H
#define IMAGE_COMPRESSOR_H

#include <string>
#include <vector>
#include "QuadTreeNode.h"
#include "RGB.h"

class ImageCompressor {
private:
    unsigned char* imageData;
    unsigned char* originalData;
    unsigned char* workingData;
    int imgWidth, imgHeight, channels;
    int minBlockSize;
    double varianceThreshold;
    int errorMethod;
    int treeDepth;
    int nodeCount;
    double targetCompression;
    QuadTreeNode* root;
    bool generateGif;

    RGB calculateAverageColor(int x, int y, int w, int h);
    double calculateError(int x, int y, int w, int h);
    void buildQuadTree(QuadTreeNode* node);
    void reconstructImage(unsigned char* outputData, QuadTreeNode* node);
    void adjustThresholdForTarget();
    int calculateDepth(QuadTreeNode* node);
    int calculateNodeCount(QuadTreeNode* node);
    void saveFrame();

public:
    ImageCompressor(const std::string& inputPath, int minSize, double threshold, int method, double targetComp, bool genGif = true);
    ~ImageCompressor();
    void compress();
    void saveCompressedImage(const std::string& outputPath);
    void generateGIF(const std::string& gifOutputPath);
    int getOriginalSize();
    int getCompressedSize();
    double getCompressionPercentage();
    int getTreeDepth();
    int getNodeCount();
};

#endif