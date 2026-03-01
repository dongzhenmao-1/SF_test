#include <stdio.h>
#include <optional>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <zmq.hpp>

#include "snake.hpp"

sf::RenderWindow window(sf::VideoMode({640, 360}), "My window", sf::Style::Default);

void init() {
    sf::View view;
    view.setCenter({0, 0});
    view.setSize({640, -360});
    window.setView(view);
}

Game::Game game;
Game::Snake *snake;

void pre_draw() {
    window.clear(sf::Color::Black);
    for (int x = -32; x < 32; ++x) {
        for (int y = -18; y < 18; ++y) {
            int state = game.world[mtd::Point(x, y)];
            sf::RectangleShape rectangle({10, 10});
            rectangle.setPosition({static_cast<floot>(x) * 10, static_cast<floot>(y) * 10});
            if (state == -1) { // 空地
                rectangle.setFillColor(sf::Color::Black); 
            } else if (state >= 0) {
                if ((state >> 1) == 0) {
                    rectangle.setFillColor(sf::Color::White);
                }
            } else if (state == -2) {
                rectangle.setFillColor(sf::Color::Blue);
            } else {
                rectangle.setFillColor(sf::Color::Yellow);
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
        for (sf::Clock c; c.getElapsedTime() < sf::seconds(0.2); ) {
            const std::optional event = window.pollEvent();
            if (event == std::nullopt) continue; 
            if (event->is<sf::Event::Closed>()) window.close();
        }
        
        game.t_run();

        pre_draw();
        // if (snake->v.size()) printf("(%d, %d)\n", snake->v.at(0).x, snake->v.at(0).y);

        window.display();
    }
    

    return 0;
}