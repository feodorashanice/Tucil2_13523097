#include <iostream>
#include <string>
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

    QuadTreeCompressor compressor(inputPath, minBlockSize, threshold, errorMethod);
    compressor.compress();
    compressor.saveCompressedImage(outputPath);

    cout << "Image compressed successfully.\n";
    return 0;
}