#include "Games.hpp"
#include "mingw.thread.h"
#include <chrono>
#include <SFML/Network.hpp>

game_outcomes Games::gameManager(sf::RenderWindow &window, game_modes mode)
{
    switch (mode)
    {
    case game_modes::Offline:
        return offline(window);
    case game_modes::Online:
        return online(window);
    case game_modes::AI:
        return AI(window);
    }
}

game_outcomes Games::online(sf::RenderWindow &window)
{
    return game_outcomes::_ERROR;
    std::map<std::string, int> returns = {
        { "Host", 0 },
        { "Join", 1 },
        { "Back", 2 },
    };

    bool white_up;

    std::string button_return = "";

    if (!loadFont(VarelaRound, "res/media/VarelaRound.ttf") || !loadFont(SourceSans, "res/media/SourceSans.ttf"))
        exit(1);

    Menu menu({800, 800}, {246, 180}, "Online", 100, sf::Color::Black, VarelaRound);
    menu.addButtons(17, 6, SourceSans, { 300, 300 }, 
                    "res\\media\\onButton.png", 
                    "res/media/offButton.png", 
                    sf::Color::Black, sf::Color::Black,
                    3, "Host", "Join", "Back");
    menu.setBackground({ 600, 600 });

    sf::IpAddress ip = sf::IpAddress::getLocalAddress();
    sf::TcpSocket socket;
    sf::TcpListener listener;

    while (window.isOpen() || button_return.compare("") == 0)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseMoved:
                    menu.checkButtonsHover(window);
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left)
                        button_return = menu.checkButtonsClick(window);
            }
        }
        window.clear();
        VisualBoard::drawEmptyBoard(window);
        menu.drawTo(window);
        window.display();
        if (button_return.compare("") != 0)
        {
            switch (returns[button_return])
            {
                case 0: // host
                {
                    window.setTitle("Chess - Host - ip: " + ip.toString());

                    listener.listen(2000);
                    listener.accept(socket);

                    white_up = (rand() % 2) != 0;
                    auto wd = !white_up;

                    socket.send(&wd, 1);
                } 
                    break;
                case 1: // join
                    break;
                case 2: // back
                    break;
            }
            break;
        }
    }
}

game_outcomes Games::offline(sf::RenderWindow &window)
{
    game_outcomes outcome = _ERROR;

    if (!loadFont(VarelaRound, "res/media/VarelaRound.ttf") || !loadFont(SourceSans, "res/media/SourceSans.ttf"))
        exit(1);

    Menu promotion_menu({ 800, 800 }, { 120, 180 }, "Promote to:", 100, sf::Color::Black, VarelaRound);
    promotion_menu.setBackground({ 600, 600 });
    promotion_menu.addButtons(170, 6, SourceSans, { 300, 300 },
                              "res\\media\\onButton.png",
                              "res\\media\\offButton.png",
                              sf::Color::Black, sf::Color::Black,
                              4, "Queen", "Rook", "Bishop", "Knight");


    bool white_up = false;
    Board board;

    // setup a board
    board.setFEN(START_FEN);
    board.printBoard();


    // visual setup
    VisualBoard v_board;
    v_board.loadTextures();
    v_board.loadSprites(board.Square);

    std::string button_return = "Home";

    v_board.flipBoard(white_up);

    sf::Mouse mouse;

    std::list<move::MoveCell> move_lst;

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
                if (event.key.code == sf::Keyboard::F) {
                    white_up = !white_up;
                    v_board.flipBoard(white_up);
                }
                else if (event.key.code == sf::Keyboard::Escape)
                    return outcome;
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left)
                {

                    auto mouse_press = mouse.getPosition(window);

                    char move[5];

                    v_board.convertMouseToMove(mouse_press, mouse_press, move);

                    auto pos = move::returnMoveVecflip(move);

                    // check turn
                    if ((board.Square[move::MoveCell::returnMoveVec(move).sq1] & PieceType::color_mask) != (((board.rights & 1) + 1) * 8))
                        break;

                    if (v_board.pieces[pos.sq1] != nullptr)
                    {
                        auto move_list_highlight = board.calculateMoveList(move::MoveCell::returnMoveVec(move).sq1);

                        changeCursor(sf::Cursor::Hand, window);

                        while (event.type != sf::Event::MouseButtonReleased)
                        {
                            window.pollEvent(event);
                            // drag & drop
                            window.clear();
                            v_board.pieces[pos.sq1]->setPosition(sf::Vector2f(mouse.getPosition(window)));
                            v_board.displayBoard(window, sf::Color(209, 139, 71), sf::Color(255, 206, 158), move_list_highlight);
                            window.draw(*v_board.pieces[pos.sq1]);
                            window.display();
                        }

                        auto mouse_release = mouse.getPosition(window);

                        if ((0 > mouse_release.x || mouse_release.x > 800) ||
                            (0 > mouse_release.y || mouse_release.y > 800))
                        {
                            v_board.pieces[pos.sq1]->setPosition({ (float)(int(mouse_press.x / 100) * 100 + 50),
                                                                    (float)(int(mouse_press.y / 100) * 100 + 50) });
                            break;
                        }

                        v_board.convertMouseToMove(mouse_press, mouse_release, move);

                        auto mve = move::MoveCell::returnMoveVec(move);
                        int promote_to = -1;

                        // if promotion
                        if (((board.Square[mve.sq1] & PieceType::piece_mask) == PieceType::pawn) && 
                            (((mve.sq2 / 8) == loc::RANK_1) || ((mve.sq2 / 8) == loc::RANK_8)))
                        {
                            char piece_char;

                            auto it = move_list_highlight->begin();
                            while (it != move_list_highlight->end())
                            {
                                if (move::cmpMove(move, it->move))
                                {
                                    while (promote_to == -1)
                                    {
                                        while (window.pollEvent(event))
                                        {
                                            switch (event.type)
                                            {
                                                case sf::Event::Closed:
                                                    window.close();
                                                    return game_outcomes::_ERROR;
                                                case sf::Event::MouseMoved:
                                                    promotion_menu.checkButtonsHover(window);
                                                    break;
                                                case sf::Event::MouseButtonPressed:
                                                    if (event.mouseButton.button == sf::Mouse::Left)
                                                    {
                                                        piece_char = promotion_menu.checkButtonsClick(window).c_str()[0];
                                                        if (piece_char != '\0')
                                                        {
                                                            switch (piece_char) {                                                                
                                                                case 'Q':
                                                                    promote_to = PieceType::queen;
                                                                    break;
                                                                case 'K':
                                                                    promote_to = PieceType::knight;
                                                                    break;
                                                                case 'R':
                                                                    promote_to = PieceType::rook;
                                                                    break;
                                                                case 'B':
                                                                    promote_to = PieceType::bishop;
                                                                    break;
                                                                default:
                                                                    promote_to = PieceType::queen;
                                                                    break;
                                                            }
                                                        }
                                                        break;
                                                    }
                                                    break;
                                            }
                                        }
                                        window.clear();
                                        v_board.displayBoard(window);
                                        promotion_menu.drawTo(window);
                                        window.display();
                                    }
                                    break;
                                }
                                it++;
                            }
                        }

                        move::MoveType move_type;

                        // move the piece
                        if (board.movePiece(move, &move_lst, move_type, promote_to) == true)
                        {
                            v_board.movePiece(board, move, move_type, promote_to);
                            changeCursor(sf::Cursor::Arrow, window);

                            board.isGameOver(outcome);
                        }
                        else
                        {
                            v_board.pieces[pos.sq1]->setPosition({ (float)(int(mouse_press.x / 100) * 100 + 50),
                                                                   (float)(int(mouse_press.y / 100) * 100 + 50) });
                            changeCursor(sf::Cursor::Arrow, window);

                            break;
                        }
                        std::cout << move << std::endl;
                    }
                }
                break;
            }
        }
        window.clear();
        v_board.displayBoard(window);
        window.display();
        if (outcome != _ERROR)
            return outcome;
    }
}

game_outcomes Games::AI(sf::RenderWindow &window)
{
    game_outcomes outcome = _ERROR;

    if (!loadFont(VarelaRound, "res/media/VarelaRound.ttf") || !loadFont(SourceSans, "res/media/SourceSans.ttf"))
        exit(1);

    Menu promotion_menu({800, 800}, {120, 180}, "Promote to:", 100, sf::Color::Black, VarelaRound);
    promotion_menu.setBackground({600, 600});
    promotion_menu.addButtons(170, 6, SourceSans, {300, 300},
                              "res\\media\\onButton.png",
                              "res\\media\\offButton.png",
                              sf::Color::Black, sf::Color::Black,
                              4, "Queen", "Rook", "Bishop", "Knight");
    
    ArtInt computer_move;
    int computer_player = (std::rand() % 2);

    bool white_up = (computer_player == 0 ? false : true);
    Board board;

    // setup a board
    board.setFEN(START_FEN);
    board.printBoard();

    // visual setup
    VisualBoard v_board;
    v_board.loadTextures();
    v_board.loadSprites(board.Square);

    std::string button_return = "Home";

    v_board.flipBoard(white_up);

    sf::Mouse mouse;

    std::list<move::MoveCell> move_lst;

    // open game log
    std::ofstream gamelog;
    gamelog.open("game_log.txt");

    while (window.isOpen())
    {
        sf::Event event;
        {    
            while (window.pollEvent(event))
            {
                switch (event.type)
                {
                case sf::Event::Closed:
                    window.close();
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::F)
                    {
                        white_up = !white_up;
                        v_board.flipBoard(white_up);
                    }
                    else if (event.key.code == sf::Keyboard::Escape)
                        return outcome;
                    break;

                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left && (board.rights & 1) != computer_player)
                    {
                        auto mouse_press = mouse.getPosition(window);

                        char move[5];

                        v_board.convertMouseToMove(mouse_press, mouse_press, move);

                        auto pos = move::returnMoveVecflip(move);

                        // check turn
                        if ((board.Square[move::MoveCell::returnMoveVec(move).sq1] & PieceType::color_mask) != (((board.rights & 1) + 1) * 8))
                            break;

                        if (v_board.pieces[pos.sq1] != nullptr)
                        {
                            auto move_list_highlight = board.calculateMoveList(move::MoveCell::returnMoveVec(move).sq1);

                            changeCursor(sf::Cursor::Hand, window);

                            while (event.type != sf::Event::MouseButtonReleased)
                            {
                                window.pollEvent(event);
                                // drag & drop
                                window.clear();
                                v_board.pieces[pos.sq1]->setPosition(sf::Vector2f(mouse.getPosition(window)));
                                v_board.displayBoard(window, sf::Color(209, 139, 71), sf::Color(255, 206, 158), move_list_highlight);
                                window.draw(*v_board.pieces[pos.sq1]);
                                window.display();
                            }

                            auto mouse_release = mouse.getPosition(window);

                            if ((0 > mouse_release.x || mouse_release.x > 800) ||
                                (0 > mouse_release.y || mouse_release.y > 800))
                            {
                                v_board.pieces[pos.sq1]->setPosition({(float)(int(mouse_press.x / 100) * 100 + 50),
                                                                    (float)(int(mouse_press.y / 100) * 100 + 50)});
                                break;
                            }

                            v_board.convertMouseToMove(mouse_press, mouse_release, move);

                            auto mve = move::MoveCell::returnMoveVec(move);
                            int promote_to = -1;

                            // if promotion
                            if (((board.Square[mve.sq1] & PieceType::piece_mask) == PieceType::pawn) &&
                                (((mve.sq2 / 8) == loc::RANK_1) || ((mve.sq2 / 8) == loc::RANK_8)))
                            {
                                char piece_char;

                                auto it = move_list_highlight->begin();
                                while (it != move_list_highlight->end())
                                {
                                    if (move::cmpMove(move, it->move))
                                    {
                                        while (promote_to == -1)
                                        {
                                            while (window.pollEvent(event))
                                            {
                                                switch (event.type)
                                                {
                                                case sf::Event::Closed:
                                                    window.close();
                                                    return game_outcomes::_ERROR;
                                                case sf::Event::MouseMoved:
                                                    promotion_menu.checkButtonsHover(window);
                                                    break;
                                                case sf::Event::MouseButtonPressed:
                                                    if (event.mouseButton.button == sf::Mouse::Left)
                                                    {
                                                        piece_char = promotion_menu.checkButtonsClick(window).c_str()[0];
                                                        if (piece_char != '\0')
                                                        {
                                                            switch (piece_char)
                                                            {
                                                            case 'Q':
                                                                promote_to = PieceType::queen;
                                                                break;
                                                            case 'K':
                                                                promote_to = PieceType::knight;
                                                                break;
                                                            case 'R':
                                                                promote_to = PieceType::rook;
                                                                break;
                                                            case 'B':
                                                                promote_to = PieceType::bishop;
                                                                break;
                                                            default:
                                                                promote_to = PieceType::queen;
                                                                break;
                                                            }
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                            }
                                            window.clear();
                                            v_board.displayBoard(window);
                                            promotion_menu.drawTo(window);
                                            window.display();
                                        }
                                        break;
                                    }
                                    it++;
                                }
                            }

                            move::MoveType move_type;

                            // move the piece
                            if (board.movePiece(move, &move_lst, move_type, promote_to) == true)
                            {
                                v_board.movePiece(board, move, move_type, promote_to);
                                changeCursor(sf::Cursor::Arrow, window);

                                board.isGameOver(outcome);
                            }
                            else
                            {
                                v_board.pieces[pos.sq1]->setPosition({(float)(int(mouse_press.x / 100) * 100 + 50),
                                                                    (float)(int(mouse_press.y / 100) * 100 + 50)});
                                changeCursor(sf::Cursor::Arrow, window);

                                break;
                            }
                            std::cout << "player: " << move << std::endl;
                            gamelog << "player: " << move << "\n";
                        }
                    }
                    break;
                }
            }
        }
        
        window.clear();
        v_board.displayBoard(window);
        window.display();
        if (outcome != _ERROR)
            return outcome;

        if ((board.rights & 1) == computer_player)
        {
            using namespace std::chrono;

            int promote_to;
            auto start = high_resolution_clock::now();
            auto comp_move = computer_move.generateComputerMove(board, promote_to, 4);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            std::cout << "time: " << (float)(duration.count() / 1000.0) << "s" << std::endl;

            if (board.movePiece(comp_move.move, &move_lst, comp_move.type, promote_to) == true)
            {
                v_board.movePiece(board, comp_move.move, comp_move.type, promote_to);

                std::cout <<  "computer: " << comp_move.move << std::endl;

                board.isGameOver(outcome);

                gamelog << "computer: " << comp_move.move << "\n";

                // board
            }
        }

    }

    gamelog.close();
}