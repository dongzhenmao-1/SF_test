#pragma once

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

    enum class Message_type {
        Join,
        Accept,
        Reject,
        Input,
        View,
        Quit,
        Kick,
    };

    struct Message_head {

    };

}
