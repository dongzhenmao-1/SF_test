#pragma once

#include <vector>
#include <string>
#include <set>

#include <zmq.hpp>

#include "../../my_algorithm/my_algorithm.hpp"
#include "food.hpp"

namespace Game::Snake_game {

    enum class DIR {
        None = -1,
        Right = 0,
        Left = 1,
        Down = 2,
        Up = 3,
    };

    inline mtd::Point point_dir[4] = {
        mtd::Point(1, 0),
        mtd::Point(-1, 0),
        mtd::Point(0, -1),
        mtd::Point(0, 1),
    };

    struct Snake {
        std::vector<mtd::Point> v;
        DIR dir, next_dir;
        int I; // Length to be added
        int D; // Length to be reduced
        int is_alive;

        Snake(const mtd::Point p) : v({p}), dir(static_cast<DIR>(0)),
            next_dir(static_cast<DIR>(-1)), I(2), D(0), is_alive(true) {
        }
        Snake() : is_alive(false) {}

        mtd::Point next_head_pos() { return v[0] + point_dir[static_cast<int>(dir)]; }

        bool t_run();
        void change_dir(const DIR _dir) { next_dir = _dir; } // External Interface
        void _change_dir() {
            if (next_dir == DIR::None) return;
            dir = next_dir, next_dir = DIR::None;
        }

        void eat(Food &food) { // eat;
            I += food.I, food.is_alive = false;
        }
        void hit() { ++D; } // hit

    };

    inline bool Snake::t_run() {
        mtd::Point pre = v[0];
        v[0] = next_head_pos();
        for (int i = 1; i < v.size(); ++i) {
            std::swap(v[i], pre);
        }
        if (I > 0) --I, v.push_back(pre);
        if (D > 0) --D, v.erase(v.begin());
        return !v.empty();
    }


}

