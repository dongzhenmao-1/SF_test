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
    // template <int w_r = 48, int h_r = 27, int view_r = 9, int pixiv_size = 10>
    template <int w_r, int h_r, int view_r, int pixiv_size>
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
        mtd::Ex_array_2D<sf::Color, view_r, view_r> get_view_world(const mtd::Point&);

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

    template <int w_r, int h_r, int view_r, int pixiv_size>
    int Server<w_r, h_r, view_r, pixiv_size>::get_avl_id() const {
        for (int i = 0; i < snake_num; ++i) {
            if (!snake[i].is_alive) return i;
        }
        return -1;
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    std::vector<mtd::Point> Server<w_r, h_r, view_r, pixiv_size>::get_rd_avl_pos() {
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

    template <int w_r, int h_r, int view_r, int pixiv_size>
    bool Server<w_r, h_r, view_r, pixiv_size>::out_of_world(const mtd::Point &p) {
        return (p.x <= -w_r || p.x >= w_r - 1 || p.y <= -h_r || p.y >= h_r - 1);
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    sf::Color Server<w_r, h_r, view_r, pixiv_size>::get_world_color(const mtd::Point &p) {
        if (out_of_world(p)) return none_color;
        return world[p];
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    mtd::Ex_array_2D<sf::Color, view_r, view_r> Server<w_r, h_r, view_r, pixiv_size>::get_view_world(const mtd::Point &center) {
        mtd::Ex_array_2D<sf::Color, view_r, view_r> view;
        for (int i = -view_r; i < view_r; ++i) {
            for (int e = -view_r; e < view_r; ++e) {
                view[center + mtd::Point(i, e)] = get_world_color(center + mtd::Point(i, e));
            }
        }
        return view;
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    void Server<w_r, h_r, view_r, pixiv_size>::draw_to_world() {
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

    template <int w_r, int h_r, int view_r, int pixiv_size>
    void Server<w_r, h_r, view_r, pixiv_size>::t_run() {
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

    template <int w_r, int h_r, int view_r, int pixiv_size>
    Server<w_r, h_r, view_r, pixiv_size>::Server() = default;

    template <int w_r, int h_r, int view_r, int pixiv_size>
    void Server<w_r, h_r, view_r, pixiv_size>::draw() {
        window.clear(sf::Color::Black);
        for (int x = -w_r; x < w_r; ++x) {
            for (int y = -h_r; y < h_r; ++y) {
                sf::RectangleShape rectangle({pixiv_size, pixiv_size});
                rectangle.setPosition({static_cast<float>(x) * 10, static_cast<float>(y) * 10});
                rectangle.setFillColor(world[mtd::Point(x, y)]);
                window.draw(rectangle);
            }
        }
        window.display();
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    void Server<w_r, h_r, view_r, pixiv_size>::init_context() {
        context = zmq::context_t(1);
        router = zmq::socket_t(context, zmq::socket_type::router);
        router.bind("tcp://127.0.0.1:5555");
        printf("The game is running on port 5555.\n");
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    void Server<w_r, h_r, view_r, pixiv_size>::init_view_window() {
        window.create(sf::VideoMode({pixiv_size * w_r * 2, pixiv_size * h_r * 2}),
              "My window", sf::Style::Default);
        sf::View view;
        view.setCenter({0, 0});
        view.setSize({pixiv_size * w_r * 2, -pixiv_size * h_r * 2});
        window.setView(view);
        printf("Window created successfully!\n");
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    void Server<w_r, h_r, view_r, pixiv_size>::solve_join_case(const std::string &token) {
        router.send(zmq::message_t(token.data(), token.size()), zmq::send_flags::sndmore);
        if (const int id = get_avl_id(); id != -1) {
            token_to_id[token] = id;
            snake[id] = Snake(get_rd_avl_pos().back(), token);
            Msg_type head = Msg_type::Accept;
            router.send(zmq::message_t(&head, sizeof(head)), zmq::send_flags::none);
        } else {
            Msg_type head = Msg_type::Reject;
            router.send(zmq::message_t(&head, sizeof(head)), zmq::send_flags::none);
        }
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    void Server<w_r, h_r, view_r, pixiv_size>::solve_input_case(const std::string &token) {
        zmq::message_t _msg;
        (void) router.recv(_msg, zmq::recv_flags::none);
        const int id = token_to_id[token];
        snake[id].change_dir(*static_cast<DIR*>(_msg.data()));
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    void Server<w_r, h_r, view_r, pixiv_size>::solve_quit_case(const std::string &token) {
        const int id = token_to_id[token];
        snake[id].is_alive = false;
        token_to_id.erase(token);
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    void Server<w_r, h_r, view_r, pixiv_size>::solve_view_case(const std::string &token) {
        const int id = token_to_id[token];
        router.send(zmq::message_t(token.data(), token.size()), zmq::send_flags::sndmore);
        mtd::Ex_array_2D<sf::Color, view_r, view_r> view = get_view_world(snake[id].v.front());
        router.send(zmq::message_t(&view, sizeof(view)), zmq::send_flags::none);
    }

    template <int w_r, int h_r, int view_r, int pixiv_size>
    void Server<w_r, h_r, view_r, pixiv_size>::run_server() {
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
            draw();

            for (const auto &[first, second] : token_to_id) {
                solve_view_case(first);
            }

            sf::sleep(tick - c.getElapsedTime());
        }
    }


}

