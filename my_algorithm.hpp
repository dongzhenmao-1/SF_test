#pragma once

namespace my_std {

struct Point {
    int x, y;
    Point() = default;
    Point(int _x, int _y) : x(_x), y(_y) { }
};

Point operator +(Point a, Point b) {
    return Point(a.x + b.x, a.y + b.y);
}

template <typename tnt, int _siz> // 提供 [-siz, siz) 的访问
struct Ex_array { 
    tnt g[_siz * 2];
    tnt &operator [](int pos) {
        return g[pos + _siz];
    }
};

template <typename tnt, int w_siz, int h_siz> // 同上
struct Ex_array_2D {
    tnt g[w_siz * 2][h_siz * 2];
    tnt &operator [](Point pos) {
        return g[pos.x + w_siz][pos.y + h_siz];
    }
};

}


