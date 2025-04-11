<h1 align="center"> Tugas Kecil 2 IF2211 Strategi Algoritma </h1>
<h1 align="center">  Kompresi Citra PGM menggunakan Algoritma Quadtree </h1>

## General Information
Program ini mengimplementasikan algoritma *Divide and Conquer* untuk melakukan kompresi gambar menggunakan struktur data **Quadtree**. Quadtree adalah struktur hierarkis yang membagi gambar menjadi blok-blok yang lebih kecil berdasarkan tingkat keseragaman warna.

Proses dimulai dengan memecah gambar menjadi empat bagian dan memeriksa apakah bagian tersebut cukup seragam. Jika tidak, maka bagian tersebut akan terus dibagi hingga ukuran blok mencapai batas minimum atau tingkat keseragaman sudah terpenuhi.

Setiap blok akhir (leaf node) akan diisi dengan nilai rata-rata dari intensitas warna pada blok tersebut. Hasil akhir adalah gambar terkompresi yang direkonstruksi dari struktur Quadtree. Program ini juga menghitung waktu eksekusi, ukuran gambar awal dan hasil, serta persentase kompresi yang dicapai.

## Technology Used
- C++
- g++ compiler
- ImageMagick

## Author
|   NIM    |              Nama               |
| :------: | :------------------------------------: |
| 13523097 |    Shanice Feodora Tjahjono     |

## Project Structure
```bash
.
│   README.md
│
├───bin
│   └─── quadtree_compression
│
├───doc
│
├───src
│   ├─── ImageCompressor.cpp
│   ├─── ImageCompressor.h
│   ├─── main.cpp
│   ├─── QuadTreeNode.cpp
│   ├─── QuadTreeNode.h
│   ├─── RGB.h
│   ├─── stb_image_write.h
│   └─── stb_image.h
│
├───test
│   ├─── test1_64x64.png
│   ├─── test2_128x128.png
│   └─── test3_256x256.png

```
## Install ImageMagick for GIF generation
1. Untuk Linux (Ubuntu/WSL), ketik `sudo apt install imagemagick` pada terminal.
2. Untuk macOS (Homebrew), ketik `brew install imagemagick` pada terminal.
3. Untuk Windows, install via Chocolatey dengan mengetik `choco install imagemagick` pada terminal, atau install manual dari: https://imagemagick.org

## How to Compile
Jalankan perintah `g++ src/*.cpp -o bin/quadtree_compression` pada terminal dari root directory.

## How to Run
1. Ketik `./bin/quadtree_compression` pada terminal.
2. Masukkan input berupa path file (contoh: test/test1_64x64.png), metode perhitungan error, threshold, ukuran blok minimum, target persentase kompresi, output file .png, serta output file .gif jika memilih untuk generate gif.
3. Jika semua masukan valid, program akan memulai kompresi gambar dan generate GIF apabila disetujui pengguna.