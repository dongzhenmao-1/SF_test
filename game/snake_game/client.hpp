#pragma once

#include <vector>
#include <string>
#include <set>

#include <zmq.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "../../my_algorithm/my_algorithm.hpp"
#include "snake.hpp"
#include "food.hpp"
#include "shared.hpp"

namespace Game::Snake_game {
    template <int pixiv = 10, int view_r = 9>
    struct Client {
        void run_client();
    };

    void Client::run_client() {


    }


}























