#pragma once

#include <vector>
#include <string>
#include <set>

#include <zmq.hpp>
#include <SFML/Graphics.hpp>

#include "../../my_algorithm/my_algorithm.hpp"
#include "snake.hpp"
#include "food.hpp"

namespace Game::Snake_game {

    template <int w_r = 32, int h_r = 18>
    struct Game {
        static constexpr sf::Color wall_color = sf::Color::White;
        static constexpr sf::Color none_color = sf::Color::Black;
        static constexpr sf::Color food_color = sf::Color::Red;
        static constexpr std::array snake_colors_range = {
            sf::Color::Cyan,
            sf::Color::Blue,
            sf::Color::Green,
            sf::Color::Yellow,
        };

        static constexpr int food_num = 5;
        static constexpr int snake_num = 1;

        std::array<Snake, snake_num> snake;
        std::array<Food, food_num> food;

        zmq::context_t context;
        zmq::socket_t receiver;
        zmq::socket_t publisher;

        std::vector<mtd::Point> get_rd_avl();

        mtd::Ex_array_2D<sf::Color, w_r, h_r> world;

        void draw();
        void t_run();
        void run_server();

        Game();

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
        for (const Food f : food) {
            avl.erase(f.pos);
        }

        for (const mtd::Point p : avl) rd_avl.push_back(p);
        mtd::shuffle(rd_avl.begin(), rd_avl.end());

        return rd_avl;
    }

    template <int w_r, int h_r>
    void Game<w_r, h_r>::draw() {
        for (int x = -w_r; x < w_r; ++x) world[mtd::Point(x, -h_r)] = wall_color;
        for (int x = -w_r; x < w_r; ++x) world[mtd::Point(x, h_r - 1)] = wall_color;
        for (int y = -h_r; y < h_r; ++y) world[mtd::Point(-w_r, y)] = wall_color;
        for (int y = -h_r; y < h_r; ++y) world[mtd::Point(w_r - 1, y)] = wall_color;

        for (int x = -w_r + 1; x < w_r - 1; ++x) {
            for (int y = -h_r + 1; y < h_r - 1; ++y) {
                world[mtd::Point(x, y)] = none_color;
            }
        }

        for (Food f : food) if (f.is_alive) {
            world[f.pos] = food_color;
        }

        for (Snake &s : snake) {
            if (!s.is_alive) continue;
            for (const mtd::Point p : s.v) {
                world[p] = snake_colors_range[0];
            }
        }
    }

    template <int w_r, int h_r>
    void Game<w_r, h_r>::t_run() {
        for (Snake &s : snake) s._change_dir();

        for (Snake &sa : snake) {
            mtd::Point head = sa.next_head_pos();
            for (Snake &sb : snake) {
                for (mtd::Point p : sb.v) {
                    if (head == p) sa.hit();
                }
            }
            if (head.x <= -w_r || head.x >= w_r - 1 || head.y <= -h_r || head.y >= h_r - 1) {
                sa.hit();
            }
            for (Food &f : food) if (f.is_alive && head == f.pos) {
                sa.eat(f);
            }
        }

        for (Snake &s : snake) s.t_run();

        std::vector<mtd::Point> rd_avl = get_rd_avl();
        for (Food &f : food) if (!f.is_alive){
            f = Food(rd_avl.back(), 1);
            rd_avl.pop_back();
        }

        draw();
    }



    template <int w_r, int h_r>
    Game<w_r, h_r>::Game() : context(1),
                             receiver(context, zmq::socket_type::pull),
                             publisher(context, zmq::socket_type::pub) {
        receiver.bind("tcp://127.0.0.1:5555");
        publisher.bind("tcp://127.0.0.1:5556");

        printf("Game is running on 5555 and 5556\n");

        for (int i = 0; i < snake.size(); ++i) {
            std::vector<mtd::Point> rd_avl = get_rd_avl();
            snake[i] = Snake(rd_avl.back(), i);
        }
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

