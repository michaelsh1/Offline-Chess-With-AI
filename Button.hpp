#pragma once
#include "Defs.hpp"

class Button
{
public:
    sf::Texture off_button_texture, on_button_texture;
    sf::Color text_color_default, text_color_hover;
    sf::RectangleShape button;
    sf::Text text;

    Button(sf::Vector2f _button_position,
           sf::Font &_font,
           const char *_text,
           unsigned int _text_size,
           std::string _onButton,
           std::string _offButton,
           sf::Color _text_color = sf::Color::Black,
           sf::Color _text_color_hover = sf::Color::Black);

    void drawTo(sf::RenderWindow &window);

    bool isMouseOver(sf::RenderWindow &window);

    void changeToNormalTexture();

    void changeToHoverTexture();
};