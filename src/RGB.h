#ifndef RGB_H
#define RGB_H

struct RGB {
    unsigned char r, g, b;
    RGB(unsigned char r_ = 0, unsigned char g_ = 0, unsigned char b_ = 0) : r(r_), g(g_), b(b_) {}
};

#endif