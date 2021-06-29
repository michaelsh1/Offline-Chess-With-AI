#include "Defs.hpp"
#include "Menu.hpp"
#include "Board.hpp"
#include "AI.hpp"

extern sf::Font VarelaRound, SourceSans;

class Games
{
public:
    game_outcomes gameManager(sf::RenderWindow &window, game_modes mode);

private:
    game_outcomes online(sf::RenderWindow &window);

    game_outcomes offline(sf::RenderWindow &window);

    game_outcomes AI(sf::RenderWindow &window);
};