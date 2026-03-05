#pragma once

#include "../../my_algorithm/my_algorithm.hpp"
#include <SFML/Graphics.hpp>

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
        Flag, // The start symbol for a message sent from the server to the client.
        Join,
        Accept,
        Reject,
        Input,
        View,
        Quit,
        Kick,
        Die,
        Hit,
        Eat,
    };

    constexpr int w_r = 96, h_r = 54, view_r = 30, pixel_size = 4;

    typedef mtd::Ex_array_2D<sf::Color, view_r, view_r> Ob_window; // Observation window

    inline void encode_ob_window(Ob_window &a, std::vector<sf::Color> &buffer) {
        buffer.clear();
        for (int i = -view_r; i < view_r; i++) {
            for (int e = -view_r; e < view_r; e++) {
                buffer.push_back(a[mtd::Point(i, e)]);
            }
        }
    }

    inline void decode_ob_window(std::vector<sf::Color> &buffer, Ob_window &a) {
        auto it = buffer.begin();
        for (int i = -view_r; i < view_r; i++) {
            for (int e = -view_r; e < view_r; e++) {
                a[mtd::Point(i, e)] = *(it++);
            }
        }
    }

    constexpr sf::Time tick(sf::seconds(0.2));

    inline long long mytime() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count() % 60000;
    }

}
