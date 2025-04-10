#include "ImageCompressor.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <cmath>
#include <cstdlib> 
using namespace std;

int frameCount = 0;

ImageCompressor::ImageCompressor(const string& inputPath, int minSize, double threshold, int method){
    minBlockSize = minSize;
    varianceThreshold = threshold;
    errorMethod = method;
    treeDepth = 0;
    nodeCount = 0;

    imageData = stbi_load(inputPath.c_str(), &imgWidth, &imgHeight, &channels, 3);
    if (!imageData) {
        cerr << "Error: Could not load image " << inputPath << endl;
        exit(1);
    }

    root = new QuadTreeNode(0, 0, imgWidth, imgHeight);
}

ImageCompressor::~ImageCompressor(){
    stbi_image_free(imageData);
    delete root;
}

RGB ImageCompressor::calculateAverageColor(int x, int y, int w, int h){
    long long sumR = 0, sumG = 0, sumB = 0;
    int count = 0;

    for (int i = x; i < x + h && i < imgHeight; i++) {
        for (int j = y; j < y + w && j < imgWidth; j++) {
            int idx = (i * imgWidth + j) * 3;
            sumR += imageData[idx];
            sumG += imageData[idx + 1];
            sumB += imageData[idx + 2];
            count++;
        }
    }

    if (count == 0) return RGB(0, 0, 0);
    return RGB(sumR / count, sumG / count, sumB / count);
}
double ImageCompressor::calculateError(int x, int y, int w, int h) {
    int count = 0;
    vector<RGB> pixels;

    for (int i = x; i < x + h && i < imgHeight; i++) {
        for (int j = y; j < y + w && j < imgWidth; j++) {
            int idx = (i * imgWidth + j) * 3;
            pixels.push_back(RGB(imageData[idx], imageData[idx + 1], imageData[idx + 2]));
            count++;
        }
    }

    if (count == 0) return 0;

    if (errorMethod == 1) { // variance
        double meanR = 0, meanG = 0, meanB = 0;
        for (const auto& p : pixels) {
            meanR += p.r;
            meanG += p.g;
            meanB += p.b;
        }
        meanR /= count;
        meanG /= count;
        meanB /= count;

        double varR = 0, varG = 0, varB = 0;
        for (const auto& p : pixels) {
            varR += pow(p.r - meanR, 2);
            varG += pow(p.g - meanG, 2);
            varB += pow(p.b - meanB, 2);
        }
        varR /= count;
        varG /= count;
        varB /= count;
        return (varR + varG + varB) / 3.0;
    }
    else if (errorMethod == 2) { // mean absolute deviation (MAD)
        double meanR = 0, meanG = 0, meanB = 0;
        for (const auto& p : pixels) {
            meanR += p.r;
            meanG += p.g;
            meanB += p.b;
        }
        meanR /= count;
        meanG /= count;
        meanB /= count;

        double madR = 0, madG = 0, madB = 0;
        for (const auto& p : pixels) {
            madR += abs(p.r - meanR);
            madG += abs(p.g - meanG);
            madB += abs(p.b - meanB);
        }
        madR /= count;
        madG /= count;
        madB /= count;
        return (madR + madG + madB) / 3.0;
    }
    else if (errorMethod == 3) { // max pixel difference
        double maxDiffR = 0, maxDiffG = 0, maxDiffB = 0;
        for (size_t i = 0; i < pixels.size(); i++) {
            for (size_t j = i + 1; j < pixels.size(); j++) {
                maxDiffR = max(maxDiffR, (double)abs(pixels[i].r - pixels[j].r));
                maxDiffG = max(maxDiffG, (double)abs(pixels[i].g - pixels[j].g));
                maxDiffB = max(maxDiffB, (double)abs(pixels[i].b - pixels[j].b));
            }
        }
        return (maxDiffR + maxDiffG + maxDiffB) / 3.0;
    }
    else if (errorMethod == 4) { // entropy
        vector<int> histR(256, 0), histG(256, 0), histB(256, 0);
        for (const auto& p : pixels) {
            histR[p.r]++;
            histG[p.g]++;
            histB[p.b]++;
        }

        double entropyR = 0, entropyG = 0, entropyB = 0;
        for (int i = 0; i < 256; i++) {
            if (histR[i] > 0) {
                double p = (double)histR[i] / count;
                entropyR -= p * log2(p);
            }
            if (histG[i] > 0) {
                double p = (double)histG[i] / count;
                entropyG -= p * log2(p);
            }
            if (histB[i] > 0) {
                double p = (double)histB[i] / count;
                entropyB -= p * log2(p);
            }
        }
        return (entropyR + entropyG + entropyB) / 3.0;
    }
    return 0;
}

void ImageCompressor::buildQuadTree(QuadTreeNode* node) {
    double error = calculateError(node->x, node->y, node->width, node->height);

    bool shouldSplit = error > varianceThreshold &&
                       node->width > minBlockSize &&
                       node->height > minBlockSize &&
                       node->width / 2 >= minBlockSize &&
                       node->height / 2 >= minBlockSize;

    if (shouldSplit) {
        saveFrame();
        node->isLeaf = false;
        int newW = node->width / 2;
        int newH = node->height / 2;

        node->child[0] = new QuadTreeNode(node->x, node->y, newW, newH); // NW
        node->child[1] = new QuadTreeNode(node->x, node->y + newW, newW, newH); // NE
        node->child[2] = new QuadTreeNode(node->x + newH, node->y, newW, newH); // SW
        node->child[3] = new QuadTreeNode(node->x + newH, node->y + newW, newW, newH); // SE

        for (int i = 0; i < 4; i++) {
            buildQuadTree(node->child[i]);
        }
    } else {
        node->averageColor = calculateAverageColor(node->x, node->y, node->width, node->height);
    }
}

void ImageCompressor::reconstructImage(unsigned char* outputData, QuadTreeNode* node) {
    if (node->isLeaf) {
        for (int i = node->x; i < node->x + node->height && i < imgHeight; i++) {
            for (int j = node->y; j < node->y + node->width && j < imgWidth; j++) {
                int idx = (i * imgWidth + j) * 3;
                outputData[idx] = node->averageColor.r;
                outputData[idx + 1] = node->averageColor.g;
                outputData[idx + 2] = node->averageColor.b;
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            if (node->child[i]) {
                reconstructImage(outputData, node->child[i]);
            }
        }
    }
}

void ImageCompressor::compress() {
    buildQuadTree(root);
    treeDepth = calculateDepth(root);
    nodeCount = calculateNodeCount(root);
}

void ImageCompressor::saveCompressedImage(const string& outputPath) {
    unsigned char* outputData = new unsigned char[imgWidth * imgHeight * 3];
    reconstructImage(outputData, root);
    stbi_write_png(outputPath.c_str(), imgWidth, imgHeight, 3, outputData, imgWidth * 3);
    delete[] outputData;
}

int ImageCompressor::calculateDepth(QuadTreeNode* node) {
    if (node->isLeaf) return 1;
    int maxChildDepth = 0;
    for (int i = 0; i < 4; i++) {
        if (node->child[i]) {
            int childDepth = calculateDepth(node->child[i]);
            maxChildDepth = std::max(maxChildDepth, childDepth);
        }
    }
    return 1 + maxChildDepth;
}

int ImageCompressor::calculateNodeCount(QuadTreeNode* node) {
    int count = 1;
    if (!node->isLeaf) {
        for (int i = 0; i < 4; i++) {
            if (node->child[i]) {
                count += calculateNodeCount(node->child[i]);
            }
        }
    }
    return count;
}

int ImageCompressor::getOriginalSize() {
    return imgWidth * imgHeight * 3; // approximate
}

int ImageCompressor::getCompressedSize() {
    return nodeCount * (sizeof(int) * 4 + sizeof(RGB)); // approximate
}

double ImageCompressor::getCompressionPercentage() {
    return (1.0 - (double)getCompressedSize() / getOriginalSize()) * 100.0;
}

int ImageCompressor::getTreeDepth() {
    return treeDepth;
}

int ImageCompressor::getNodeCount() {
    return nodeCount;
}

void ImageCompressor::saveFrame() {
    std::vector<unsigned char> copy(originalData, originalData + imgWidth * imgHeight * 3);

    char filename[100];
    sprintf(filename, "src/frames/frame_%03d.png", frameCount++);
    stbi_write_png(filename, imgWidth, imgHeight, 3, copy.data(), imgWidth * 3);
}


void ImageCompressor::generateGIF(const string& gifOutputPath){
    string command = "convert -delay 10 -loop 0 src/frames/frame_*.png " + gifOutputPath;
    int result = system(command.c_str());

    if (result != 0) {
        cerr << "Failed to create GIF." << endl;
    } else {
        cout << "GIF saved to: " << gifOutputPath << endl;
    }
}