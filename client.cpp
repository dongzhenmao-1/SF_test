#include "game/snake_game/client.hpp"

int main() {
    Game::Snake_game::Client<10, 9> client;
    client.run_client();

    return 0;
}