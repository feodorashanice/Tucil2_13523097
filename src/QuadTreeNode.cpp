#include "QuadTreeNode.h"

QuadTreeNode::QuadTreeNode(int x_, int y_, int w_, int h_) : x(x_), y(y_), width(w_), height(h_), isLeaf(true) {
    for (int i = 0; i < 4; ++i) {
        child[i] = nullptr;
    }
}

QuadTreeNode::~QuadTreeNode() {
    for (int i = 0; i < 4; ++i) {
        delete child[i];
    }
}