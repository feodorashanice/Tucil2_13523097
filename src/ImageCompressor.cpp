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

ImageCompressor::ImageCompressor(const string& inputPath, int minSize, double threshold, int method, double targetComp, bool genGif) {
    minBlockSize = minSize;
    varianceThreshold = threshold;
    errorMethod = method;
    treeDepth = 0;
    nodeCount = 0;
    targetCompression = targetComp;
    generateGif = genGif;

    if (generateGif) {
        #ifdef _WIN32
            system("mkdir src\\frames >nul 2>&1");
        #else
            system("mkdir -p src/frames >/dev/null 2>&1");
        #endif
    }

    imageData = stbi_load(inputPath.c_str(), &imgWidth, &imgHeight, &channels, 3);
    if (!imageData) {
        cerr << "Error: Could not load image " << inputPath << endl;
        exit(1);
    }

    originalData = new unsigned char[imgWidth * imgHeight * 3];
    memcpy(originalData, imageData, imgWidth * imgHeight * 3);

    workingData = new unsigned char[imgWidth * imgHeight * 3];
    memcpy(workingData, originalData, imgWidth * imgHeight * 3);

    root = new QuadTreeNode(0, 0, imgWidth, imgHeight);
}

ImageCompressor::~ImageCompressor() {
    stbi_image_free(imageData);
    delete[] originalData;
    delete[] workingData;
    delete root;
}

RGB ImageCompressor::calculateAverageColor(int x, int y, int w, int h) {
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

    if (errorMethod == 1) { // variance using Welford's method
        double meanR = 0, meanG = 0, meanB = 0;
        double m2R = 0, m2G = 0, m2B = 0;

        for (int i = 0; i < pixels.size(); i++) {
            double r = pixels[i].r;
            double g = pixels[i].g;
            double b = pixels[i].b;

            double deltaR = r - meanR;
            double deltaG = g - meanG;
            double deltaB = b - meanB;
            meanR += deltaR / (i + 1);
            meanG += deltaG / (i + 1);
            meanB += deltaB / (i + 1);

            m2R += deltaR * (r - meanR);
            m2G += deltaG * (g - meanG);
            m2B += deltaB * (b - meanB);
        }
        double varR = m2R / count;
        double varG = m2G / count;
        double varB = m2B / count;
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
        if (generateGif) saveFrame();
        node->isLeaf = false;
        int newW = node->width / 2;
        int newH = node->height / 2;

        node->child[0] = new QuadTreeNode(node->x, node->y, newW, newH);
        node->child[1] = new QuadTreeNode(node->x, node->y + newW, newW, newH);
        node->child[2] = new QuadTreeNode(node->x + newH, node->y, newW, newH);
        node->child[3] = new QuadTreeNode(node->x + newH, node->y + newW, newW, newH);

        for (int i = 0; i < 4; i++) {
            buildQuadTree(node->child[i]);
        }
    } else {
        node->averageColor = calculateAverageColor(node->x, node->y, node->width, node->height);
        for (int i = node->x; i < node->x + node->height && i < imgHeight; i++) {
            for (int j = node->y; j < node->y + node->width && j < imgWidth; j++) {
                int idx = (i * imgWidth + j) * 3;
                workingData[idx] = node->averageColor.r;
                workingData[idx + 1] = node->averageColor.g;
                workingData[idx + 2] = node->averageColor.b;
            }
        }
        if (generateGif) saveFrame();
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

void ImageCompressor::adjustThresholdForTarget() {
    if (targetCompression <= 0.0) {
        return;
    }

    double low = 0.0;
    double high = 100000.0;
    const double tolerance = 0.01;
    const int maxIterations = 50;

    if (targetCompression >= 0.99) {
        varianceThreshold = 0.0;
        delete root;
        root = new QuadTreeNode(0, 0, imgWidth, imgHeight);
        frameCount = 0;
        buildQuadTree(root);
        treeDepth = calculateDepth(root);
        nodeCount = calculateNodeCount(root);
        return;
    }

    for (int i = 0; i < maxIterations; i++) {
        delete root;
        root = new QuadTreeNode(0, 0, imgWidth, imgHeight);
        frameCount = 0;
        varianceThreshold = (low + high) / 2;
        buildQuadTree(root);
        treeDepth = calculateDepth(root);
        nodeCount = calculateNodeCount(root);

        double currentCompression = getCompressionPercentage() / 100.0;
        if (abs(currentCompression - targetCompression) <= tolerance) {
            break;
        }
        if (currentCompression > targetCompression) {
            high = varianceThreshold;
        } else {
            low = varianceThreshold;
        }
    }
}

void ImageCompressor::compress() {
    if (generateGif) {
        #ifdef _WIN32
            system("del /Q src\\frames\\frame_*.png >nul 2>&1");
        #else
            system("rm -f src/frames/frame_*.png >/dev/null 2>&1");
        #endif
    }
    frameCount = 0;

    buildQuadTree(root);
    treeDepth = calculateDepth(root);
    nodeCount = calculateNodeCount(root);
    adjustThresholdForTarget();
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
    char filename[100];
    sprintf(filename, "src/frames/frame_%03d.png", frameCount++);
    stbi_write_png(filename, imgWidth, imgHeight, 3, workingData, imgWidth * 3);
}

void ImageCompressor::generateGIF(const string& gifOutputPath) {
    if (!generateGif) {
        cout << "Skipping GIF generation..." << endl;
        return;
    }

    string command = "convert -delay 10 -loop 0 src/frames/frame_*.png " + gifOutputPath;
    int result = system(command.c_str());

    if (result != 0) {
        cerr << "Failed to create GIF." << endl;
    } else {
        cout << "GIF saved to: " << gifOutputPath << endl;
    }
}