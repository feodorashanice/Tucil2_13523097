#include <iostream>
#include <string>
#include <chrono>
#include "ImageCompressor.h"
using namespace std;

int main() {
    string inputPath, outputPath, gifPath;
    int errorMethod, minBlockSize;
    double threshold, targetCompression;

    cout << "Enter input image path: ";
    getline(cin, inputPath);
    cout << "Enter error measurement method (1: Variance, 2: MAD, 3: Max Pixel Difference, 4: Entropy): ";
    cin >> errorMethod;
    cout << "Enter threshold: ";
    cin >> threshold;
    cout << "Enter minimum block size: ";
    cin >> minBlockSize;
    cout << "Enter target compression percentage (0 to disable): ";
    cin >> targetCompression;
    cin.ignore();
    cout << "Enter output image path: ";
    getline(cin, outputPath);
    cout << "Enter output GIF path: ";
    getline(cin, gifPath);

    auto start = chrono::high_resolution_clock::now();

    ImageCompressor compressor(inputPath, minBlockSize, threshold, errorMethod);
    compressor.compress();
    compressor.saveCompressedImage(outputPath);

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Execution time: " << duration.count() << " ms\n";
    cout << "Original image size: " << compressor.getOriginalSize() << " bytes\n";
    cout << "Compressed image size: " << compressor.getCompressedSize() << " bytes\n";
    cout << "Compression percentage: " << compressor.getCompressionPercentage() << endl;
    cout << "Quadtree depth: " << compressor.getTreeDepth() << endl;
    cout << "Number of nodes: " << compressor.getNodeCount() << endl;

    return 0;
}