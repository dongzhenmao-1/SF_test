#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>

#include <zmq.hpp>
#include <SFML/Graphics.hpp>

#include "../../my_algorithm/my_algorithm.hpp"
#include "snake.hpp"
#include "food.hpp"
#include "shared.hpp"

namespace Game::Snake_game {
    struct Server {
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
        static constexpr int snake_num = 3;

        std::array<Snake, snake_num> snake;
        std::array<Food, food_num> food;

        sf::RenderWindow window;

        zmq::context_t context;
        zmq::socket_t router;

        Server();

        int get_avl_id() const;
        std::vector<mtd::Point> get_rd_avl_pos();

        mtd::Ex_array_2D<sf::Color, w_r, h_r> world;

        static bool out_of_world(const mtd::Point&);
        sf::Color get_world_color(const mtd::Point&);

        typedef mtd::Ex_array_2D<sf::Color, view_r, view_r> View_window;
        void get_view_world(Ob_window &view, mtd::Point center);

        void draw_to_world();
        void t_run();
        void draw();

        std::map<std::string, int> token_to_id;

        void init_context();
        void init_view_window();

        void solve_join_case(const std::string &token);
        void solve_input_case(const std::string &token);
        void solve_quit_case(const std::string &token);

        void solve_view_case(const std::string &token);

        void run_server();

    };


    inline int Server::get_avl_id() const {
        for (int i = 0; i < snake_num; ++i) {
            if (!snake[i].is_alive) return i;
        }
        return -1;
    }

    
    inline std::vector<mtd::Point> Server::get_rd_avl_pos() {
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
        for (const Food f : food) avl.erase(f.pos);

        for (const mtd::Point p : avl) rd_avl.push_back(p);
        mtd::shuffle(rd_avl.begin(), rd_avl.end());

        return rd_avl;
    }

    
    inline bool Server::out_of_world(const mtd::Point &p) {
        return (p.x <= -w_r || p.x >= w_r - 1 || p.y <= -h_r || p.y >= h_r - 1);
    }

    
    inline sf::Color Server::get_world_color(const mtd::Point &p) {
        if (out_of_world(p)) return none_color;
        return world[p];
    }

    
    inline void Server::get_view_world(Ob_window &view, const mtd::Point center) {
        for (int i = -view_r; i < view_r; ++i) {
            for (int e = -view_r; e < view_r; ++e) {
                view[center + mtd::Point(i, e)] = get_world_color(center + mtd::Point(i, e));
            }
        }
    }

    
    inline void Server::draw_to_world() {
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

    
    inline void Server::t_run() {
        for (Snake &s : snake) if (s.is_alive) s._change_dir(); // It's necessary to split the function

        for (Snake &sa : snake) if (sa.is_alive) {
            mtd::Point head = sa.next_head_pos();
            for (const Snake &sb : snake) if (sb.is_alive) {
                Snake _sb = sb; _sb.t_run(); // To prevent this snake's head from hitting the heads of other snakes.
                for (mtd::Point p : _sb.v) if (head == p) sa.hit();
            }
            if (out_of_world(head)) sa.hit();

            for (Food &f : food) if (f.is_alive && head == f.pos) {
                sa.eat(f);
            }
        }

        for (Snake &s : snake) if (s.is_alive) s.t_run();

        std::vector<mtd::Point> rd_avl = get_rd_avl_pos();
        for (Food &f : food) if (!f.is_alive){
            f = Food(rd_avl.back(), 1);
            rd_avl.pop_back();
        }

        draw_to_world();
    }

    
    inline Server::Server() = default;

    
    inline void Server::draw() {
        window.clear(sf::Color::Black);
        for (int x = -w_r; x < w_r; ++x) {
            for (int y = -h_r; y < h_r; ++y) {
                sf::RectangleShape rectangle({pixel_size, pixel_size});
                rectangle.setPosition({static_cast<float>(x) * 10, static_cast<float>(y) * 10});
                rectangle.setFillColor(world[mtd::Point(x, y)]);
                window.draw(rectangle);
            }
        }
        window.display();
    }


    inline void Server::init_context() {
        context = zmq::context_t(1);
        router = zmq::socket_t(context, zmq::socket_type::router);
        router.bind("tcp://127.0.0.1:5555");
        printf("The game is running on port 5555.\n");
    }

    
    inline void Server::init_view_window() {
        window.create(sf::VideoMode({pixel_size * w_r * 2, pixel_size * h_r * 2}),
              "My window", sf::Style::Default);
        sf::View view;
        view.setCenter({0, 0});
        view.setSize({pixel_size * w_r * 2, -pixel_size * h_r * 2});
        window.setView(view);
        printf("Window created successfully!\n");
    }

    
    inline void Server::solve_join_case(const std::string &token) {
        router.send(zmq::message_t(token.data(), token.size()), zmq::send_flags::sndmore);
        if (const int id = get_avl_id(); id != -1) {
            token_to_id[token] = id;
            snake[id] = Snake(get_rd_avl_pos().back(), token);
            Msg_type head = Msg_type::Accept;
            router.send(zmq::message_t(&head, sizeof(head)), zmq::send_flags::none);
            printf("The player %d joins the game\n", id);
        } else {
            Msg_type head = Msg_type::Reject;
            router.send(zmq::message_t(&head, sizeof(head)), zmq::send_flags::none);
            printf("A player wants to join the game, but the server is full\n");
        }
    }

    inline void Server::solve_input_case(const std::string &token) {
        zmq::message_t _msg;
        (void) router.recv(_msg, zmq::recv_flags::none);
        const int id = token_to_id[token];
        snake[id].change_dir(*static_cast<DIR*>(_msg.data()));
    }

    inline void Server::solve_quit_case(const std::string &token) {
        const int id = token_to_id[token];
        snake[id].is_alive = false;
        token_to_id.erase(token);
        printf("The player %d quits the game\n", id);
    }

    
    inline void Server::solve_view_case(const std::string &token) {
        const int id = token_to_id[token];
        router.send(zmq::message_t(token.data(), token.size()), zmq::send_flags::sndmore);
        mtd::Ex_array_2D<sf::Color, view_r, view_r> view;
        get_view_world(view, snake[id].v.front());
        printf("get_view_world\n");
        std::vector<sf::Color> buffer;
        encode_ob_window(view, buffer);
        printf("encode_ob_window\n");
        router.send(zmq::message_t(buffer.data(), buffer.size() * sizeof(sf::Color)), zmq::send_flags::none);
    }

    inline void Server::run_server() {
        init_context();
        init_view_window();

        // int T = 0;
        constexpr sf::Time tick(sf::seconds(0.2));
        while (window.isOpen()) {
            // printf("%d\n", T++);
            while (const std::optional event = window.pollEvent()) {
                if (event == std::nullopt) continue;
                if (event->is<sf::Event::Closed>()) window.close();
            }

            sf::Clock c;
            zmq::message_t _msg;
            while (router.recv(_msg, zmq::recv_flags::dontwait)) {
                std::string token = _msg.to_string();
                (void) router.recv(_msg, zmq::recv_flags::none);
                const Msg_type head = *static_cast<Msg_type*>(_msg.data());
                if (head == Msg_type::Join) solve_join_case(token);
                else if (head == Msg_type::Input) solve_input_case(token);
                else if (head == Msg_type::Quit) solve_quit_case(token);
            }

            t_run();
            printf("t_run\n");
            draw();
            printf("draw\n");

            for (const auto &[first, second] : token_to_id) {
                printf("to %d\n", second);
                solve_view_case(first);
                printf("ed %d\n")
            }
            printf("view\n");

            sf::sleep(tick - c.getElapsedTime());
        }
    }


}

