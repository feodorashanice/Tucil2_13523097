#include <iostream>
#include <string>
#include <chrono>
#include "ImageCompressor.h"
using namespace std;

int main() {
    string inputPath, outputPath, gifPath;
    int errorMethod, minBlockSize;
    double threshold, targetCompression;
    char continueChoice;

    cout << "ᴡᴇʟᴄᴏᴍᴇ ᴛᴏ" << endl;
    cout << "█▀▀█ █░░█ █▀▀█ █▀▀▄ ▀▀█▀▀ █▀▀█ █▀▀ █▀▀ \t █▀▀ █▀▀█ █▀▄▀█ █▀▀█ █▀▀█ █▀▀ █▀▀ █▀▀ ░▀░ █▀▀█ █▀▀▄\n";
    cout << "█░░█ █░░█ █▄▄█ █░░█ ░░█░░ █▄▄▀ █▀▀ █▀▀ \t █░░ █░░█ █░▀░█ █░░█ █▄▄▀ █▀▀ ▀▀█ ▀▀█ ▀█▀ █░░█ █░░█\n";
    cout << "▀▀▀█ ░▀▀▀ ▀░░▀ ▀▀▀░ ░░▀░░ ▀░▀▀ ▀▀▀ ▀▀▀ \t ▀▀▀ ▀▀▀▀ ▀░░░▀ █▀▀▀ ▀░▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀▀ ▀░░▀\n";
    cout << endl;

    do {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        cout << "ᴡᴇʟᴄᴏᴍᴇ ᴛᴏ" << endl;
        cout << "█▀▀█ █░░█ █▀▀█ █▀▀▄ ▀▀█▀▀ █▀▀█ █▀▀ █▀▀ \t █▀▀ █▀▀█ █▀▄▀█ █▀▀█ █▀▀█ █▀▀ █▀▀ █▀▀ ░▀░ █▀▀█ █▀▀▄\n";
        cout << "█░░█ █░░█ █▄▄█ █░░█ ░░█░░ █▄▄▀ █▀▀ █▀▀ \t █░░ █░░█ █░▀░█ █░░█ █▄▄▀ █▀▀ ▀▀█ ▀▀█ ▀█▀ █░░█ █░░█\n";
        cout << "▀▀▀█ ░▀▀▀ ▀░░▀ ▀▀▀░ ░░▀░░ ▀░▀▀ ▀▀▀ ▀▀▀ \t ▀▀▀ ▀▀▀▀ ▀░░░▀ █▀▀▀ ▀░▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀▀ ▀░░▀\n";
        cout << endl;

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
        cout << "Compressing image..." << endl;

        auto start = chrono::high_resolution_clock::now();

        ImageCompressor compressor(inputPath, minBlockSize, threshold, errorMethod, targetCompression);
        compressor.compress();
        compressor.saveCompressedImage(outputPath);
        compressor.generateGIF(gifPath);

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

        cout << endl;
        cout << "Execution time: " << duration.count() << " ms" << endl;
        cout << "Previous image size: " << compressor.getOriginalSize() << " bytes" << endl;
        cout << "Current image size after compression: " << compressor.getCompressedSize() << " bytes" << endl;
        cout << "Compression percentage: " << compressor.getCompressionPercentage() << "%" << endl;
        cout << "Depth of tree: " << compressor.getTreeDepth() << endl;
        cout << "Number of nodes: " << compressor.getNodeCount() << endl;

        cout << endl;
        cout << "Do you want to compress another image? (y/n): ";
        cin >> continueChoice;
        cin.ignore();

    } while (continueChoice == 'y' || continueChoice == 'Y');

    cout << endl;
    cout << "Byeeee" << endl;

    return 0;
}