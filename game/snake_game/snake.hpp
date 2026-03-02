#pragma once

#include <vector>
#include <string>
#include <set>

#include <zmq.hpp>

#include "../../my_algorithm/my_algorithm.hpp"

namespace Game {

    enum class DIR {
        None = -1,
        Right = 0,
        Left = 1,
        Down = 2,
        Up = 3,
    };

    inline DIR string_to_DIR(const std::string& s) {
        if (s == "Right") return DIR::Right;
        else if (s == "Left") return DIR::Left;
        else if (s == "Down") return DIR::Down;
        else if (s == "Up") return DIR::Up;
        else return DIR::None;
    }

    inline mtd::Point point_dir[4] = {
        mtd::Point(1, 0),
        mtd::Point(-1, 0),
        mtd::Point(0, -1),
        mtd::Point(0, 1),
    };

    struct Snake {
        std::vector<mtd::Point> v;
        DIR dir, next_dir;
        int id;
        int I; // 待添加长度
        int D; // 待减小长度


        Snake(const int _x, const int _y, const int _id) : v({mtd::Point(_x, _y)}), dir(static_cast<DIR>(0)),
            next_dir(static_cast<DIR>(-1)), id(_id), I(2), D(0) {

        }

        mtd::Point head_next_pos() { return v[0] + point_dir[static_cast<int>(dir)]; }

        bool t_run();
        void change_dir(const DIR _dir) { next_dir = _dir; }
        void ex_change_dir() {
            if (static_cast<int>(next_dir) == -1) return;
            // if ((int(__dir) ^ 1) == int(dir)) return;
            dir = next_dir, next_dir = DIR::None;
        }

        void eat() { ++I; } // 我吃饭
        void hit() { ++D; } // 撞到墙了

    };

    inline bool Snake::t_run() {
        mtd::Point pre = v[0];
        v[0] = head_next_pos();
        for (int i = 1; i < v.size(); ++i) {
            std::swap(v[i], pre);
        }
        if (I > 0) --I, v.push_back(pre);
        if (D > 0) --D, v.erase(v.begin());
        return !v.empty();
    }


}

