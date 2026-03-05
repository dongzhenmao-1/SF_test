#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <iostream>
#include <ctime>

#include <zmq.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "../../my_algorithm/my_algorithm.hpp"
#include "shared.hpp"

namespace Game::Snake_game {
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

    inline bool Client::init_context() {
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

    inline void Client::init_view_window() {
        window.create(sf::VideoMode({pixel_size * view_r * 2, pixel_size * view_r * 2}),
              "My window", sf::Style::Default);
        sf::View view;
        view.setCenter({0, 0});
        view.setSize({pixel_size * view_r * 2, -pixel_size * view_r * 2});
        window.setView(view);
        printf("Window created successfully!\n");
    }

    inline void Client::handle_input() {
        auto send_dir = [this](DIR dir) -> void {
            Msg_type head = Msg_type::Input;
            dealer.send(zmq::message_t(&head, sizeof(head)), zmq::send_flags::sndmore);
            dealer.send(zmq::message_t(&dir, sizeof(dir)), zmq::send_flags::none);
        };

        DIR final_dir = DIR::None;
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Right) final_dir = DIR::Right;
                else if (key->code == sf::Keyboard::Key::Left) final_dir = DIR::Left;
                else if (key->code == sf::Keyboard::Key::Down) final_dir = DIR::Down;
                else if (key->code == sf::Keyboard::Key::Up) final_dir = DIR::Up;
            }
        }

        if (final_dir != DIR::None) {
            send_dir(final_dir);
            printf("%lld: send dir(%d) to server\n", mytime(), static_cast<int>(final_dir));
        }
    }

    inline void Client::draw() {
        zmq::message_t msg;
        (void) dealer.recv(msg, zmq::recv_flags::none);
        const auto _it = static_cast<sf::Color*>(msg.data());
        std::vector<sf::Color> buffer(_it, _it + view_r * view_r * 4);
        Ob_window world;
        decode_ob_window(buffer, world);

        window.clear(sf::Color::Black);
        for (int x = -view_r; x < view_r; ++x) {
            for (int y = -view_r; y < view_r; ++y) {
                sf::RectangleShape rectangle({pixel_size, pixel_size});
                rectangle.setPosition({static_cast<float>(x) * pixel_size, static_cast<float>(y) * pixel_size});
                rectangle.setFillColor(world[mtd::Point(x, y)]);
                window.draw(rectangle);
            }
        }
        window.display();
    }

    inline void Client::run_client() {
        if (!init_context()) return;
        init_view_window();

        while (window.isOpen()) {
            zmq::message_t msg;
            while (dealer.recv(msg, zmq::recv_flags::none)) { // wait for flag
                Msg_type head = *static_cast<Msg_type*>(msg.data());
                if (head == Msg_type::Flag) break;
            }

            sf::sleep(sf::seconds(0.02));
            while (dealer.recv(msg, zmq::recv_flags::dontwait)) {
                Msg_type head = *static_cast<Msg_type*>(msg.data());
                if (head == Msg_type::Die) window.close();
                else if (head == Msg_type::View) draw();
            }

            printf("%lld: receive the view from server\n", mytime());
            sf::Clock c;
            sf::sleep((tick - c.getElapsedTime()) - sf::seconds(0.05));
            handle_input();

            printf("\n");
        }
    }

}























