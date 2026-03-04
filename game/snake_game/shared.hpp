#pragma once

#include "../../my_algorithm/my_algorithm.hpp"

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

    enum class Msg_type {
        Join,
        Accept,
        Reject,
        Input,
        View,
        Quit,
        Kick,
    };

    typedef mtd::Ex_array_2D<sf::Color, 9, 9> Ob_window; // Observation window

    constexpr int w_r = 47, h_r = 27, view_r = 9, pixel_size = 10;

    inline void encode_ob_window(Ob_window &a, std::vector<sf::Color> &buffer) {
        auto it = buffer.begin();
        for (int i = 0; i < 9; i++) {
            for (int e = 0; e < 9; e++) {
                *(it++) = a[mtd::Point(i, e)];
            }
        }
    }

    inline void decode_ob_window(std::vector<sf::Color> &buffer, Ob_window &a) {
        auto it = buffer.begin();
        for (int i = 0; i < 9; i++) {
            for (int e = 0; e < 9; e++) {
                a[mtd::Point(i, e)] = *(it++);
            }
        }
    }

}
