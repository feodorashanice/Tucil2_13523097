#ifndef QUADTREE_COMPRESSOR_H
#define QUADTREE_COMPRESSOR_H

#include <string>
#include <vector>
#include "QuadTreeNode.h"
using namespace std;

class QuadTreeCompressor {
private:
    unsigned char* imageData;
    int imgWidth, imgHeight, channels;
    QuadTreeNode* root;
    int minBlockSize;
    double varianceThreshold;
    int errorMethod; // variance, MAD, maximum pixel difference, entropy
    int treeDepth;
    int nodeCount;

    RGB calculateAverageColor(int x, int y, int w, int h);
    double calculateError(int x, int y, int w, int h);
    void buildQuadTree(QuadTreeNode* node);
    void reconstructImage(unsigned char* outputData, QuadTreeNode* node);

public:
    QuadTreeCompressor(const string& inputPath, int minSize, double threshold, int method);
    ~QuadTreeCompressor();
    void compress();
    void saveCompressedImage(const string& outputPath);
};

#endif