#pragma once

#include <random>
#include <algorithm>
#include <string>
#include <vector>

namespace mtd {

struct Point {
    int x, y;
    Point() = default;
    Point(int _x, int _y) : x(_x), y(_y) { }
};

Point operator +(Point a, Point b) {
    return Point(a.x + b.x, a.y + b.y);
}

bool operator <(const Point &a, const Point &b) {
    return a.x == b.x ? a.y < b.y : a.x < b.x;
}

bool operator ==(const Point &a, const Point &b) {
    return a.x == b.x && a.y == b.y;
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

static std::mt19937 gen(713);

int rand_int(int l, int r) { // [l, r)
    return std::uniform_int_distribution<int>(l, r - 1)(gen);
}

double rand_double(double l = 0, double r = 1) {
    return std::uniform_real_distribution<double>(l, r)(gen);
}

int rand_bool(double k = 0.5) {
    return std::bernoulli_distribution(k)(gen);
}

template <typename tnt>
void shuffle(tnt a, tnt b) {
    std::shuffle(a, b, gen);
}

std::vector<std::string> split_string(std::string s) { // 时间复杂度为 O(n), erase 的底层实现为 popback
    std::vector<std::string> v;
    s = ' ' + s;
    for (int p = int(s.size()) - 1; p >= 0; --p) {
        if (s[p] == ' ') {
            if (p != int(s.size()) - 1) v.push_back(s.substr(p + 1, int(s.size()) - p - 1));
            s.erase(s.begin() + p, s.end());
        }
    }
    std::reverse(v.begin(), v.end());
    return v;
}

int string_to_int(std::string s) {
    int v = 0, f = (s[0] == '-');
    if (s[0] == '-') s.erase(s.begin());
    for (char c : s) v = v * 10 + c - '0';
    return v;
}

}

