#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <zmq.hpp>

#include "game/snake_game/server.hpp"

int main() {
    Game::Snake_game::Server<47, 28, 9, 10> server;
    server.run_server();
    

    return 0;
}