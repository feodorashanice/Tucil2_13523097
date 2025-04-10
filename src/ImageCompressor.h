#ifndef IMAGECOMPRESSOR_H
#define IMAGECOMPRESSOR_H

#include <string>
#include <vector>
#include <cstdlib>
#include "QuadTreeNode.h"
using namespace std;

class ImageCompressor{
private:
    unsigned char* imageData;
    int imgWidth, imgHeight, channels;
    QuadTreeNode* root;
    int minBlockSize;
    double varianceThreshold;
    int errorMethod;
    int treeDepth;
    int nodeCount;

    RGB calculateAverageColor(int x, int y, int w, int h);
    unsigned char* originalData;
    unsigned char* workingData;

    double calculateError(int x, int y, int w, int h);
    void buildQuadTree(QuadTreeNode* node);
    void reconstructImage(unsigned char* outputData, QuadTreeNode* node);
    int calculateDepth(QuadTreeNode* node);
    int calculateNodeCount(QuadTreeNode* node);

public:
    ImageCompressor(const string& inputPath, int minSize, double threshold, int method);
    ~ImageCompressor();
    void compress();
    void saveCompressedImage(const string& outputPath);
    int getOriginalSize();
    int getCompressedSize();
    double getCompressionPercentage();
    int getTreeDepth();
    int getNodeCount();
    void saveFrame();
    void generateGIF(const string& gifOutputPath);
};

#endif