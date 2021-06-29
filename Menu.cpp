#include "Menu.hpp"

Menu::Menu(sf::Vector2f _window_dimensions,
           sf::Vector2f _header_position,
           std::string _header,
           int _header_size,
           sf::Color _header_color,
           sf::Font &_header_font)
{
    window_dimensions = _window_dimensions;
    num_of_buttons = 0;

    header.setString(_header);
    header.setFont(_header_font);
    header.setCharacterSize(_header_size);
    header.setPosition(_header_position);
    header.setFillColor(_header_color);
}

Menu::~Menu()
{
    for (int i = 0; i < num_of_buttons; i++)
        delete button_array[i];
    delete[] button_array;
}

void Menu::setBackground(sf::Vector2f size, sf::Color color)
{
    isBackground = true;
    background.setFillColor(color);
    background.setSize(size);

    // set position
    sf::Vector2f pos = {(window_dimensions.x - size.x) / 2,
                        (window_dimensions.y - size.y) / 2};
    background.setPosition(pos);
}

void Menu::addButtons(int text_size,
                int button_gap,
                sf::Font &font,
                sf::Vector2f first_button_position,
                const char *on_button,
                const char *off_button,
                sf::Color text_color_on,
                sf::Color text_color_off,
                int count, ...)
{
    button_array = new Button *[count];
    num_of_buttons = count;

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++)
    {
        button_array[i] = new Button(first_button_position,
                                     font,
                                     va_arg(args, const char *),
                                     text_size,
                                     on_button,
                                     off_button,
                                     text_color_on,
                                     text_color_off);
        first_button_position.y += button_array[i]->button.getSize().y + button_gap;
    }
    va_end(args);
}

void Menu::checkButtonsHover(sf::RenderWindow &window)
{
    for (int i = 0; i < num_of_buttons; i++)
    {
        if (button_array[i]->isMouseOver(window))
            button_array[i]->changeToHoverTexture();
        else
            button_array[i]->changeToNormalTexture();
    }
}

std::string Menu::checkButtonsClick(sf::RenderWindow &window)
{
    for (int i = 0; i < num_of_buttons; i++)
        if (button_array[i]->isMouseOver(window))
            return button_array[i]->text.getString().toAnsiString();
    return "";
}

void Menu::drawTo(sf::RenderWindow &window)
{
    if (isBackground)
        window.draw(background);
    window.draw(header);

    if (num_of_buttons != 0)
        for (int i = 0; i < num_of_buttons; i++)
        {
            button_array[i]->drawTo(window);
        }
}
