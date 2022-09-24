#pragma once
#include "Button.hpp"

extern sf::Cursor cursor;

class Menu
{
private:
    //-----------------------------text-----------------------------
    sf::Text header;

    //-----------------------------buttons--------------------------
    Button **button_array;
    int num_of_buttons;

    //-----------------------------backgrounds----------------------
    sf::Vector2f window_dimensions;
    bool isBackground;
    sf::RectangleShape background;

public:
    Menu(sf::Vector2f _window_dimensions,
         sf::Vector2f _header_position,
         std::string _header,
         int _header_size,
         sf::Color _header_color,
         sf::Font &_header_font);

    ~Menu();

    void setBackground(sf::Vector2f size, sf::Color color = sf::Color(255, 255, 255, 128));

    /*
        add chosen number of buttons
        in: the button features
            count: the number of buttons
            ...: insert the strings of the buttons
        out: (nothing)
    */
    void addButtons(int text_size,
                    int button_gap,
                    sf::Font &font,
                    sf::Vector2f first_button_position,
                    const char *on_button,
                    const char *off_button,
                    sf::Color text_color_on,
                    sf::Color text_color_off,
                    int count, ...);

    /*
        checks the state of all the buttons in this menu
        in: the Render Window
        out: the string inside the button that was pressed
    */
    void checkButtonsHover(sf::RenderWindow &window);

    std::string checkButtonsClick(sf::RenderWindow &window);

    /*
        draw the menu to
        in: the Render Window
        out: (nothing)
    */
    void drawTo(sf::RenderWindow &window);
};