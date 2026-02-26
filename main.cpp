#include <stdio.h>
#include <optional>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "snake.hpp"

sf::RenderWindow window(sf::VideoMode({640, 360}), "My window", sf::Style::Default);

void init() {
    sf::View view;
    view.setCenter({0, 0});
    view.setSize({640, -360});
    window.setView(view);
}

SNAKE::Game<32, 18> game;
SNAKE::Snake *snake;

void pre_draw() {
    window.clear(sf::Color::Black);
    for (int x = -32; x < 32; ++x) {
        for (int y = -18; y < 18; ++y) {
            int state = game.world[my_std::Point(x, y)];
            sf::RectangleShape rectangle({10, 10});
            rectangle.setPosition({x * 10, y * 10});
            if (state == -1) { // 空地
                rectangle.setFillColor(sf::Color::Black); 
            } else {
                if ((state >> 1) == 0) {
                    rectangle.setFillColor(sf::Color::White);
                    if (state & 1) {
                        rectangle.setSize({8, 8});
                        rectangle.setOutlineColor(sf::Color::Red);
                        rectangle.setOutlineThickness(1.f);
                        rectangle.setPosition({x * 10 + 1, y * 10 + 1});
                    }
                }
            }
            
            window.draw(rectangle);
        }
    }
}

int main() {
    init();

    snake = &game.join_a_snake();
    game.t_run();
    
    while (window.isOpen()) {
        for (sf::Clock c; c.getElapsedTime() < sf::seconds(0.15); ) {
            const std::optional event = window.pollEvent();
            if (event == std::nullopt) continue; 
            if (event->is<sf::Event::Closed>()) window.close();
            else if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
                     if (key->scancode == sf::Keyboard::Scancode::Right) snake->change_dir(0);
                else if (key->scancode == sf::Keyboard::Scancode::Left) snake->change_dir(1);
                else if (key->scancode == sf::Keyboard::Scancode::Down) snake->change_dir(2);
                else if (key->scancode == sf::Keyboard::Scancode::Up) snake->change_dir(3);
            }
        }
        
        game.t_run();

        pre_draw();

        window.display();
    }
    

    return 0;
}