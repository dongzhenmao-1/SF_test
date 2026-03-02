#pragma once

#include <vector>
#include <string>
#include <set>

#include <zmq.hpp>
#include <SFML/Graphics.hpp>

#include "../../my_algorithm/my_algorithm.hpp"
#include "snake.hpp"

namespace Game {

    template <int w_r = 32, int h_r = 18>
    struct Game {
        static constexpr int food_num = 5;

        std::vector<Snake> snake;
        std::vector<mtd::Point> food;

        zmq::context_t context;
        zmq::socket_t receiver;
        zmq::socket_t publisher;

        std::vector<mtd::Point> get_rd_avl();

        mtd::Ex_array_2D<int, w_r, h_r> world; // -1 代表空

        void draw();

        void t_run();

        Snake &join_a_snake() {

            snake.emplace_back(0, 0, static_cast<int>(snake.size()));
            return snake.back();
        }

        Game();

        void run_server();

    };

    template <int w_r, int h_r>
    std::vector<mtd::Point> Game<w_r, h_r>::get_rd_avl() {
        std::set<mtd::Point> avl;
        std::vector<mtd::Point> rd_avl;

        for (int x = -w_r + 1; x < w_r - 1; ++x) {
            for (int y = -h_r + 1; y < h_r - 1; ++y) {
                avl.insert(mtd::Point(x, y));
            }
        }

        for (Snake &s : snake) {
            for (mtd::Point p : s.v) avl.erase(p);
        }
        for (const mtd::Point p : food) {
            avl.erase(p);
        }

        for (const mtd::Point p : avl) rd_avl.push_back(p);
        mtd::shuffle(rd_avl.begin(), rd_avl.end());

        return rd_avl;
    }

    template <int w_r, int h_r>
    void Game<w_r, h_r>::draw() {
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

    template <int w_r, int h_r>
    void Game<w_r, h_r>::t_run() {
        for (int id = 0; id < snake.size(); ++id) {
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

        for (int id = 0; id < snake.size(); ++id) {
            snake[id].t_run();
        }

        auto rd_avl = get_rd_avl();
        while (food.size() < food_num && !rd_avl.empty()) {
            food.push_back(rd_avl.back()), rd_avl.pop_back();
        }

        draw();
    }



    template <int w_r, int h_r>
    Game<w_r, h_r>::Game() : context(1), receiver(context, zmq::socket_type::pull), publisher(context, zmq::socket_type::pub) {
        receiver.bind("tcp://127.0.0.1:5555");
        publisher.bind("tcp://127.0.0.1:5556");

        printf("Game is running on 5555 and 5556\n");

        join_a_snake();
    }

    template <int w_r, int h_r>
    void Game<w_r, h_r>::run_server() {
        constexpr sf::Time tick(sf::seconds(0.2));
        while (true) {
            sf::Clock c;
            zmq::message_t msg;
            while (receiver.recv(msg, zmq::recv_flags::dontwait)) {

            }

            t_run();

            sf::sleep(tick - c.getElapsedTime());
        }
    }


}

