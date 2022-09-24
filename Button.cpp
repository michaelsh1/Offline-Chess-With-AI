#include "Button.hpp"

Button::Button(sf::Vector2f _button_position,
       sf::Font &_font,
       const char *_text,
       unsigned int _text_size,
       std::string _onButton,
       std::string _offButton,
       sf::Color _text_color,
       sf::Color _text_color_hover)
{
    text_color_default = _text_color;
    text_color_hover = _text_color_hover;
    if (!on_button_texture.loadFromFile(_offButton) || !off_button_texture.loadFromFile(_onButton))
    {
        std::cerr << "Couldn't load texture" << std::endl;
        exit(1);
    }

    on_button_texture.setSmooth(true);
    off_button_texture.setSmooth(true);

    // button setup
    button.setSize(sf::Vector2f(off_button_texture.getSize()));
    button.setTexture(&off_button_texture);
    button.setPosition(_button_position);

    // text setup
    text.setFont(_font);
    text.setString(_text);
    text.setFillColor(_text_color);

    // set text position:
    float text_height = text.getLocalBounds().height;
    float text_width = text.getLocalBounds().width;

    float new_h = _button_position.y + (button.getLocalBounds().height - text_height) / 5;
    float new_w = _button_position.x + (button.getLocalBounds().width - text_width) / 2;

    text.setPosition(sf::Vector2f(new_w, new_h));
}

void Button::drawTo(sf::RenderWindow &window)
{
    window.draw(button);
    window.draw(text);
}

bool Button::isMouseOver(sf::RenderWindow &window)
{
    sf::Vector2i mouse_position = sf::Mouse::getPosition(window);

    sf::Vector2f button_top_left = button.getPosition();
    sf::Vector2f button_botton_right = {button_top_left.x + button.getLocalBounds().width,
                                        button_top_left.y + button.getLocalBounds().height};

    if (mouse_position.x < button_botton_right.x && mouse_position.y < button_botton_right.y &&
        mouse_position.x > button_top_left.x && mouse_position.y > button_top_left.y)
        return true;
    return false;
}

void Button::changeToNormalTexture()
{
    button.setTexture(&off_button_texture);
    text.setFillColor(text_color_default);
}

void Button::changeToHoverTexture()
{
    button.setTexture(&on_button_texture);
    text.setFillColor(text_color_hover);
}