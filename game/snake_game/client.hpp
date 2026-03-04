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
    template <int pixiv_size = 10, int view_r = 9>
    struct Client {
        zmq::context_t context;
        zmq::socket_t dealer;

        sf::RenderWindow window;

        bool init_context();
        void init_view_window();

        void handle_input();

        void draw();

        void run_client();
    };

    template <int pixiv_size, int view_r>
    bool Client<pixiv_size, view_r>::init_context() {
        context = zmq::context_t(1);
        dealer = zmq::socket_t(context, zmq::socket_type::dealer);
        dealer.connect("tcp://127.0.0.1:5555");
        printf("Connecting...\n");

        Msg_type head = Msg_type::Join;
        dealer.send(zmq::message_t(&head, sizeof(head)), zmq::send_flags::none);

        zmq::message_t msg;
        (void) dealer.recv(msg, zmq::recv_flags::none);
        head = *static_cast<Msg_type*>(msg.data());
        if (head == Msg_type::Accept) {
            printf("Accept!\n");
            return true;
        } else {
            printf("Sorry, the server is full");
            return false;
        }
    }

    template <int pixiv_size, int view_r>
    void Client<pixiv_size, view_r>::init_view_window() {
        window.create(sf::VideoMode({pixiv_size * view_r * 2, pixiv_size * view_r * 2}),
              "My window", sf::Style::Default);
        sf::View view;
        view.setCenter({0, 0});
        view.setSize({pixiv_size * view_r * 2, -pixiv_size * view_r * 2});
        window.setView(view);
        printf("Window created successfully!\n");
    }

    template <int pixiv_size, int view_r>
    void Client<pixiv_size, view_r>::handle_input() {
        auto send_dir = [this](DIR dir) -> void {
            Msg_type head = Msg_type::Input;
            dealer.send(zmq::message_t(&head, sizeof(head)), zmq::send_flags::sndmore);
            dealer.send(zmq::message_t(&dir, sizeof(dir)), zmq::send_flags::none);
        };

        DIR final_dir;
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Right) final_dir = DIR::Right;
                else if (key->code == sf::Keyboard::Key::Left) final_dir = DIR::Left;
                else if (key->code == sf::Keyboard::Key::Down) final_dir = DIR::Down;
                else if (key->code == sf::Keyboard::Key::Up) final_dir = DIR::Up;
            }
        }
        send_dir(final_dir);
    }

    template <int pixiv_size, int view_r>
    void Client<pixiv_size, view_r>::draw() {
        zmq::message_t msg;
        (void) dealer.recv(msg, zmq::recv_flags::none);
        const auto world = *static_cast<mtd::Ex_array_2D<sf::Color, view_r, view_r>*>(msg.data());

        window.clear(sf::Color::Black);
        for (int x = -view_r; x < view_r; ++x) {
            for (int y = -view_r; y < view_r; ++y) {
                sf::RectangleShape rectangle({pixiv_size, pixiv_size});
                rectangle.setPosition({static_cast<float>(x) * 10, static_cast<float>(y) * 10});
                rectangle.setFillColor(world[mtd::Point(x, y)]);
                window.draw(rectangle);
            }
        }
        window.display();
    }

    template <int pixiv, int view_r>
    void Client<pixiv, view_r>::run_client() {
        init_context();
        init_view_window();
        while (window.isOpen()) {
            handle_input();
            draw();


        }

    }


}























