#pragma once

#include <vector>
#include <string>
#include <set>

#include <zmq.hpp>

#include "my_algorithm/my_algorithm.hpp"

namespace SNAKE {

enum class DIR {
    None = -1,
    Right = 0,
    Left = 1,           
    Down = 2,           
    Up = 3,   
};

DIR string_to_DIR(std::string s) {
    if (s == "Right") return DIR::Right;
    else if (s == "Left") return DIR::Left;
    else if (s == "Down") return DIR::Down;
    else if (s == "Up") return DIR::Up;
    else return DIR::None;
}

mtd::Point point_dir[4] = {
    mtd::Point(1, 0),
    mtd::Point(-1, 0),
    mtd::Point(0, -1),
    mtd::Point(0, 1),
};

struct Snake {
    std::vector<mtd::Point> v;
    DIR dir, __dir;
    int id;
    int I; // 待添加长度
    int D; // 待减小长度


    Snake(int _x, int _y) : v({mtd::Point(_x, _y)}), I(2), D(0), dir(DIR(0)) { }

    mtd::Point head_next_pos() { return v[0] + point_dir[int(dir)]; }

    bool t_run();
    void change_dir(DIR _dir) { __dir = _dir; }
    void ex_change_dir() {
        if (int(__dir) == -1) return;
        // if ((int(__dir) ^ 1) == int(dir)) return;
        dir = __dir, __dir = DIR::None;
    }

    void eat() { ++I; } // 我吃饭
    void hit() { ++D; } // 撞到墙了

};

bool Snake::t_run() {
    mtd::Point pre = v[0];
    v[0] = head_next_pos();
    for (int i = 1; i < int(v.size()); ++i) {
        std::swap(v[i], pre);
    }
    if (I > 0) --I, v.push_back(pre);
    if (D > 0) --D, v.erase(v.begin());
    return !v.empty();
}

// template <int w_r = 32, int h_r = 18>
static const int w_r = 32, h_r = 18;
struct Game {
    static const int food_num = 5;

    std::vector<Snake> snake;
    std::vector<mtd::Point> food;
    std::set<mtd::Point> avl;
    std::vector<mtd::Point> rd_avl;
    zmq::context_t context;
    zmq::socket_t receiver;
    zmq::socket_t publisher;
    
    void get_rd_avl() {
        rd_avl.clear();
        for (const mtd::Point p : avl) rd_avl.push_back(p);
        mtd::shuffle(rd_avl.begin(), rd_avl.end());
    }

    mtd::Ex_array_2D<int, w_r, h_r> world; // -1 代表空

    void draw();

    void t_run();

    Snake &join_a_snake() {
        snake.push_back(Snake(0, 0));
        return snake.back();
    }

    Game();

    void run_server();

};

void Game::draw() {
    for (int x = -w_r; x < w_r; ++x) world[mtd::Point(x, -h_r)] = -3;
    for (int x = -w_r; x < w_r; ++x) world[mtd::Point(x, h_r - 1)] = -3;
    for (int y = -h_r; y < h_r; ++y) world[mtd::Point(-w_r, y)] = -3;
    for (int y = -h_r; y < h_r; ++y) world[mtd::Point(w_r - 1, y)] = -3;

    for (int x = -w_r + 1; x < w_r - 1; ++x) {
        for (int y = -h_r + 1; y < h_r - 1; ++y) {
            world[mtd::Point(x, y)] = -1;
        }
    }

    for (mtd::Point p : food) {
        world[p] = -2;
    }

    for (int id = 0; id < snake.size(); ++id) {
        for (mtd::Point p : snake[id].v) {
            world[p] = id * 2;
        }
        world[snake[id].v[0]] = id * 2 + 1;
    } 
}

void Game::t_run() {
    for (int id = 0; id < int(snake.size()); ++id) {
        snake[id].ex_change_dir();
    }        

    for (int ida = 0; ida < snake.size(); ++ida) { // 处理碰撞逻辑
        mtd::Point head = snake[ida].head_next_pos();
        for (int idb = 0; idb < snake.size(); ++idb) {  
            for (mtd::Point p : snake[idb].v) { // 撞到对方身体了
                if (head == p) snake[ida].hit();
            }
        }
        if (head.x <= -w_r || head.x >= w_r - 1 || head.y <= -h_r || head.y >= h_r - 1) {
            snake[ida].hit();
        }
        for (int idb = 0; idb < food.size(); ++idb) {
            if (head == food[idb] && snake[ida].D == 0) { //
                snake[ida].eat(), food.erase(food.begin() + idb), --idb;
            }
        }
    }

    for (int id = 0; id < int(snake.size()); ++id) {
        snake[id].t_run();
    }

    avl.clear();
    for (int x = -w_r + 1; x < w_r - 1; ++x) {
        for (int y = -h_r + 1; y < h_r - 1; ++y) {
            avl.insert(mtd::Point(x, y));
        }
    }

    for (int id = 0; id < snake.size(); ++id) {
        for (mtd::Point p : snake[id].v) avl.erase(p);
    }
    for (const mtd::Point p : food) {
        avl.erase(p);
    }

    get_rd_avl();
    for ( ; food.size() < food_num; ) {
        food.push_back(rd_avl.back()), rd_avl.pop_back();
    }

    draw();
}

Game::Game() : context(1), receiver(context, zmq::socket_type::pull), publisher(context, zmq::socket_type::pub) {
    receiver.bind("tcp://127.0.0.1:5555");
    publisher.bind("tcp://127.0.0.1:5556");

    printf("Game is running on 5555 and 5556\n");

    join_a_snake();
}

void Game::run_server() {
    const sf::Time tick(sf::seconds(0.2));
    while (true) {
        sf::Clock c;
        zmq::message_t msg;
        while (receiver.recv(msg, zmq::recv_flags::dontwait)) {
            std::vector<std::string> cmd = mtd::split_string(msg.to_string());
            if (cmd[0] == "snake") {
                int id = mtd::string_to_int(cmd[1]);
                if (cmd[2] == "change") {
                    if (cmd[3] == "dir") {
                        snake[id].change_dir(string_to_DIR(cmd[4]));
                    }
                }
            }
        }

        t_run();

        sf::sleep(tick - c.getElapsedTime());
    }
}


}

