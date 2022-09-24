#include "Menu.hpp"
#include "Board.hpp"
#include "Games.hpp"


extern sf::Cursor cursor;
extern sf::Font VarelaRound, SourceSans;

int main()
{
    std::map<game_outcomes, const char *> outcomes = {
        { game_outcomes::BLACK,  "black" },
        { game_outcomes::DRAW,   "draw" },
        { game_outcomes::WHITE,  "white" },
        { game_outcomes::_ERROR, "error" },
    };

    // main window
    sf::RenderWindow window(sf::VideoMode(800, 800), 
                            "Simple Chess", 
                            sf::Style::Titlebar | sf::Style::Close);

    auto icon = sf::Image{};
    if (!icon.loadFromFile("res/media/icon.png"))
    {
        MessageBoxA(NULL, "Unable to load icon", "Icon loading error", MB_OK | MB_ICONERROR);
        exit(2);
    }

    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // display board
    Board board;
    VisualBoard v_board;
    std::fstream newfile("res\\media\\randomFENs.txt", std::ios::in);

    // get  arandom chess riddle
    if (newfile.is_open()) 
    {
        std::string tp;
        int counter = 0;

        srand(time(NULL));
        int line_num = (rand() % 222);

        while (std::getline(newfile, tp))
        {
            if (counter == line_num)
                break;
            counter++;
        }
        std::cout << tp << std::endl;
        board.setFEN(tp.c_str());
        v_board.loadTextures();
        v_board.loadSprites(board.Square);
        newfile.close();
        // if its whites turn he should be in the bottom
        if ((board.rights & 1) == 1)
            v_board.flipBoard(false);
    }

    std::stack<Board> position_stack;


    // load fonts
    if (!loadFont(VarelaRound, "res/media/VarelaRound.ttf") || !loadFont(SourceSans, "res/media/SourceSans.ttf"))
        exit(1);

    // does the user want to view the riddle
    bool view_riddle = false;

    // main menu
    Menu main_menu({ 800, 800 }, { 244, 180 }, "Chess!", 100, sf::Color::Black, VarelaRound);
    main_menu.setBackground({ 600, 600 });
    main_menu.addButtons(17, 6, SourceSans, { 300, 300 }, 
                            "res\\media\\onButton.png",
                            "res\\media\\offButton.png",
                            sf::Color::Black, sf::Color::Black,
                            3, "New Game", "Help", "Exit");
    

    // New Game menu
    Menu new_game_menu({ 800, 800 }, { 236, 180 }, "Games", 100, sf::Color::Black, VarelaRound);
    new_game_menu.setBackground({ 600, 600 });
    new_game_menu.addButtons(17, 6, SourceSans, { 300, 300 },
                                "res\\media\\onButton.png",
                                "res\\media\\offButton.png",
                                sf::Color::Black, sf::Color::Black,
                                3, "Offline Game", "Against AI", "Back");

    // help menu
    Menu help_menu({ 800, 800 }, { 291, 180 }, "Help", 100, sf::Color::Black, VarelaRound);
    help_menu.setBackground({ 600, 600 }, sf::Color(255, 255, 255, 200));
    help_menu.addButtons(17, 6, SourceSans, { 300, 553 },
                            "res\\media\\onButton.png",
                            "res\\media\\offButton.png",
                            sf::Color::Black, sf::Color::Black,
                            1, "Back");
    sf::RectangleShape help_photo;
    sf::Texture hp_t;
    if (!hp_t.loadFromFile("res\\media\\help.png"))
    { 
        std::cout << "unable to load texture" << std::endl; 
        exit(1);
    }
    help_photo.setTexture(&hp_t);
    help_photo.setSize(sf::Vector2f(hp_t.getSize()));
    help_photo.setPosition({ 227, 315 });

    // game manager
    Games games_manager;
    game_outcomes game_outcome = _ERROR;

    Menu *current_menu;
    current_menu = &main_menu;

    std::string button_return = "";

    // main game loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::V)
                    {
                        view_riddle = !view_riddle;
                        if (view_riddle)
                            MessageBoxA(NULL, (board.rights & 1) == 1 ? "White To Move" : "Black To Move",
                                        "Riddle: Mate in 2", MB_OK | MB_ICONINFORMATION);
                    }
                    if(event.key.code == sf::Keyboard::Escape)
                        view_riddle = false;
                    break;
                case sf::Event::MouseMoved:
                    current_menu->checkButtonsHover(window);
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left && !view_riddle)
                    {
                        button_return = current_menu->checkButtonsClick(window);

                        if (button_return.compare("") != 0)
                        {
                            // check if going back to main menu, no need to check twice
                            if (button_return.compare("Back") == 0)
                            {
                                current_menu = &main_menu;
                                break;
                            }

                            // only have to check the main menu buttons, doesnt make sense to check other menus
                            if (current_menu == &main_menu)
                            {
                                if (button_return.compare("New Game") == 0)
                                    current_menu = &new_game_menu;
                                else if (button_return.compare("Help") == 0)
                                    current_menu = &help_menu;
                                else if (button_return.compare("Exit") == 0)
                                    exit(1);
                            }
                            // only have to check the new game menu buttons, doesnt make sense to check other menus
                            else if (current_menu == &new_game_menu)
                            {
                                if (button_return.compare("Online Game") == 0)
                                {
                                    game_outcome = games_manager.gameManager(window, game_modes::Online);
                                }
                                else if (button_return.compare("Offline Game") == 0)
                                {
                                    game_outcome = games_manager.gameManager(window, game_modes::Offline);
                                }
                                else if (button_return.compare("Against AI") == 0)
                                {
                                    game_outcome = games_manager.gameManager(window, game_modes::AI);
                                }
                                switch (game_outcome)
                                {
                                    case game_outcomes::BLACK:
                                        MessageBoxA(NULL, "The winner is Black", "GAME OVER", MB_OK | MB_ICONINFORMATION);
                                        break;
                                    case game_outcomes::WHITE:
                                        MessageBoxA(NULL, "The winner is White", "GAME OVER", MB_OK | MB_ICONINFORMATION);
                                        break;
                                    case game_outcomes::DRAW:
                                        MessageBoxA(NULL, "Game ended in a Draw", "GAME OVER", MB_OK | MB_ICONINFORMATION);
                                        break;
                                    case game_outcomes::_ERROR:
                                        MessageBoxA(NULL, "ERROR", "Unexpected exit", MB_OK | MB_ICONERROR);
                                        break;
                                }
                            }
                            // There is no need for us to check the help menu \
                            because it has only back button
                        }
                    }
                    break;
            }
        }
        window.clear();
        v_board.displayBoard(window, board);
        // dont display the menu if view riddle
        if (!view_riddle)
        {
            current_menu->drawTo(window);
            if (current_menu == &help_menu)
                window.draw(help_photo);
        }
        window.display();
    }
}
//     sf::IpAddress ip = sf::IpAddress::getLocalAddress();
//     sf::TcpSocket socket;
//     char connection_type, mode;

//     bool white_up = false;
//     std::size_t recived;

//     std::cout << "Enter (s) for hosting the game and (c) for joining a game: " << std::endl;
//     std::cin >> connection_type;

//     if (connection_type == 's')
//     {
//         std::cout << "game port: " << ip << " waiting for a player to join..." << std::endl;
//         sf::TcpListener listener;
//         listener.listen(2000);
//         listener.accept(socket);

//         white_up = (rand() % 2) != 0;

//         socket.send((white_up == false) ? "1" : "0", 2);
//     }
//     else if (connection_type == 'c')
//     {
//         std::cout << "enter game port: " << std::endl;
//         std::string game_port;
//         std::cin >> game_port;
//         sf::IpAddress ip_game_port = sf::IpAddress(game_port.c_str());
//         socket.connect(ip_game_port, 2000);
        
//         char recieve_side[2];
//         socket.receive(recieve_side, sizeof(recieve_side), recived);

//         if (std::string(recieve_side) == std::string("1"))
//             white_up = true;
//     }

//     std::cout << "matchmaking... you are " << ((white_up == true) ? "black" : "white") << std::endl;
//     system("pause");

//     sf::RenderWindow window(sf::VideoMode(SCREEN_HEIGHT, SCREEN_WIDTH), 
//                            "Chess", 
//                            sf::Style::Titlebar | sf::Style::Close);

//     sf::Font VarelaRound, SourceSans;
//     if (!VarelaRound.loadFromFile("res\\media\\VarelaRound.ttf") || !SourceSans.loadFromFile("res\\media\\SourceSansPro.ttf"))
//     {
//         std::cout << "Couldn't load font" << std::endl;
//         exit(1);
//     }

//     Menu menu(sf::Vector2f(window.getSize()), { 244, 179 }, "Chess!", 100, sf::Color::Black, VarelaRound);
//     menu.setBackground({600, 600});
//     menu.addButtons(17, 6, SourceSans, {300, 300},
//                     "res\\media\\onButton.png",
//                     "res\\media\\offButton.png",
//                     sf::Color::Black, sf::Color::Black, 2, "New Game", "Help");


//     // board setup
//     Board board;
//     board.setFEN(START_FEN);
//     board.printBoard();

//     int number_of_attackers;
//     std::list<move::MoveCell> moves_list;
//     behavior::PawnBehavior b;
//     auto a = b.isPinned(board, loc::d4, &moves_list);
//     auto it = moves_list.begin();
//     while (it != moves_list.end())
//     {
//         std::cout << it->move << ", ";
//         it++;
//     }
//     std::cout << std::endl;

//     // visual setup
//     VisualBoard v_board;
//     v_board.loadTextures();
//     v_board.loadSprites(board.Square);

//     std::string button_return = "Home";

//     v_board.flipBoard(white_up);
//     //initMenus menus(window);

//     sf::Mouse mouse;

//     std::list<move::MoveCell> move_lst;


//     while (window.isOpen())
//     {
//         sf::Event event;
//         while (window.pollEvent(event))
//         {
//             switch (event.type)
//             {
//             case sf::Event::Closed:
//                 window.close();
//             case sf::Event::KeyPressed:
//                 if (event.key.code == sf::Keyboard::Space) {
//                     white_up = !white_up;
//                     v_board.flipBoard(white_up);
//                 }
//                 break;

//             case sf::Event::MouseButtonPressed:
//                 if (event.mouseButton.button == sf::Mouse::Left)
//                 {
//                     auto mouse_press = mouse.getPosition(window);

//                     char move[5];

//                     v_board.convertMouseToMove(mouse_press, mouse_press, move);

//                     auto pos = move::returnMoveVecflip(move);

//                     // check turn
//                     if ((board.Square[move::MoveCell::returnMoveVec(move).sq1] & PieceType::color_mask) != (((board.rights & 1) + 1) * 8))
//                         break;

//                     auto move_list_highlight = board.calculateMoveList(move::MoveCell::returnMoveVec(move).sq1);

                    
//                     if (v_board.pieces[pos.sq1] != nullptr)
//                     {
//                         changeCursor(sf::Cursor::Hand, window);

//                         while (event.type != sf::Event::MouseButtonReleased)
//                         {
//                             window.pollEvent(event);
//                             // drag & drop
//                             window.clear();
//                             v_board.pieces[pos.sq1]->setPosition(sf::Vector2f(mouse.getPosition(window)));
//                             v_board.displayBoard(window, sf::Color(209, 139, 71), sf::Color(255, 206, 158), move_list_highlight);
//                             window.draw(*v_board.pieces[pos.sq1]);
//                             window.display();
//                         }

//                         auto mouse_release = mouse.getPosition(window);

//                         if ((0 > mouse_release.x || mouse_release.x > 800) ||
//                             (0 > mouse_release.y || mouse_release.y > 800))
//                         {
//                             v_board.pieces[pos.sq1]->setPosition({ (float)(int(mouse_press.x / 100) * 100 + 50),
//                                                                     (float)(int(mouse_press.y / 100) * 100 + 50) });
//                             break;
//                         }

//                         v_board.convertMouseToMove(mouse_press, mouse_release, move);


//                         if (board.movePiece(move, &move_lst) == true)
//                         {
//                             v_board.pieces[pos.sq1]->setPosition({(float)(int(mouse_release.x / 100) * 100 + 50),
//                                                                   (float)(int(mouse_release.y / 100) * 100 + 50)});

//                             pos = move::returnMoveVecflip(move);

//                             if (pos.sq1 != pos.sq2)
//                             {
//                                 // if target square is not empty
//                                 if (v_board.pieces[pos.sq2] != nullptr)
//                                     delete v_board.pieces[pos.sq2];

//                                 v_board.pieces[pos.sq2] = v_board.pieces[pos.sq1];
//                                 v_board.pieces[pos.sq1] = nullptr;
//                             }
//                             changeCursor(sf::Cursor::Arrow, window);
//                         }
//                         else
//                         {
//                             v_board.pieces[pos.sq1]->setPosition({(float)(int(mouse_press.x / 100) * 100 + 50),
//                                                                   (float)(int(mouse_press.y / 100) * 100 + 50)});
//                             changeCursor(sf::Cursor::Arrow, window);

//                             break;
//                         }
//                         std::cout << move << std::endl;
//                     }
//                 }
//                 break;
//             }
//         }
//         window.clear();
//         // if(button_return != "")
//         //     std::cout << button_return << std::endl;
//         //menus.pageController(button_return, window);
//         v_board.displayBoard(window);
//         window.display();
//     }

//     return 0;
// }