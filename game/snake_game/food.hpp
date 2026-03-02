#pragma once

#include <vector>
#include <string>
#include <set>

#include <zmq.hpp>

#include "../../my_algorithm/my_algorithm.hpp"

namespace Game::Snake_game {
    struct Food {
        mtd::Point pos;
        int I;
        int is_alive;
        Food() : is_alive(false) {}
        Food(mtd::Point _pos, int _I) : pos(_pos), I(_I), is_alive(true) {}
    };



}

