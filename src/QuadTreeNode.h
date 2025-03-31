#ifndef QUADTREENODE_H
#define QUADTREENODE_H

#include "RGB.h"

class QuadTreeNode {
public:
    int x, y;
    int width, height;
    RGB averageColor;
    QuadTreeNode* child[4];
    bool isLeaf;

    QuadTreeNode(int x_, int y_, int w_, int h_);
    ~QuadTreeNode();
};

#endif