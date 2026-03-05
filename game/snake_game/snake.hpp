#pragma once

#include <utility>
#include <vector>
#include <string>
#include <algorithm>

#include "../../my_algorithm/my_algorithm.hpp"
#include "food.hpp"
#include "shared.hpp"

namespace Game::Snake_game {
    struct Snake {
        std::vector<mtd::Point> v;
        DIR dir, next_dir;
        int I; // Length to be added
        int D; // Length to be reduced
        int is_alive, is_online;
        std::string token;

        Snake(const mtd::Point p, std::string _token) : v({p}), dir(static_cast<DIR>(0)),
            next_dir(static_cast<DIR>(-1)), I(2), D(0), is_alive(true), is_online(true), token(std::move(_token)) {
        }
        Snake() : is_alive(false), is_online(false) {}

        mtd::Point next_head_pos() { return v[0] + point_dir[static_cast<int>(dir)]; }

        void t_run();
        void change_dir(const DIR _dir) { // External Interface
            if (_dir == DIR::None) return;
            next_dir = _dir;
        }
        void _change_dir() {
            if (next_dir == DIR::None) return;
            if ((static_cast<int>(next_dir) ^ static_cast<int>(dir)) == 1) return;
            dir = next_dir, next_dir = DIR::None;
        }

        void eat(Food &food) { // eat;
            I += food.I, food.is_alive = false;
        }
        void hit() { ++D; } // hit

    };

    inline void Snake::t_run() {
        mtd::Point pre = v[0];
        v[0] = next_head_pos();
        for (int i = 1; i < v.size(); ++i) {
            std::swap(v[i], pre);
        }
        if (I > 0) --I, v.push_back(pre);
        if (D > 0) --D, v.erase(v.begin());
        if (v.empty()) is_alive = false;
    }


}

