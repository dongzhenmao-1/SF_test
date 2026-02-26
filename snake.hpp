#pragma once

#include <vector>

#include "my_algorithm.hpp"

namespace SNAKE {

enum class DIR {
    Right = 0,
    Left = 1,           
    Down = 2,           
    Up = 3,   
};

my_std::Point point_dir[4] = {
    my_std::Point(1, 0),
    my_std::Point(-1, 0),
    my_std::Point(0, -1),
    my_std::Point(0, 1),
};

struct Snake {
    std::vector<my_std::Point> v;
    DIR dir;
    int id;
    int I; // 待添加长度

    Snake(int _x, int _y) : v({my_std::Point(_x, _y)}), I(2), dir(DIR(0)) { }

    my_std::Point head_next_pos() {
        
    }

    void t_run();
    
    void change_dir(int _dir) {
        dir = DIR(_dir);
    }

};

void Snake::t_run() {
    my_std::Point pre = v[0];
    v[0] = v[0] + point_dir[int(dir)];
    for (int i = 1; i < int(v.size()); ++i) {
        std::swap(v[i], pre);
    }
    if (I) --I, v.push_back(pre);
}

template <int w_r = 32, int h_r = 18>
struct Game {
    std::vector<Snake> snake;
    my_std::Ex_array_2D<int, w_r, h_r> world; // -1 代表空

    void draw() {
        for (int x = -w_r; x < w_r; ++x) {
            for (int y = -h_r; y < h_r; ++y) {
                world[my_std::Point(x, y)] = -1;
            }
        }        
        for (int id = 0; id < int(snake.size()); ++id) {
            for (my_std::Point p : snake[id].v) {
                world[p] = id * 2;
            }
            world[snake[id].v[0]] = id * 2 + 1;
        }
    }

    void t_run() {
        for (int id = 0; id < int(snake.size()); ++id) {
            snake[id].t_run();
        }
        draw();
    }

    Snake &join_a_snake() {
        snake.push_back(Snake(0, 0));
        return snake.back();
    }

};



}

