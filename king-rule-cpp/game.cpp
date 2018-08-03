//
//  game.cpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 5/22/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#include "game.hpp"

void Game::play() {
    bool white = true;
    
    while (true) {
        bool human;
        if (white) human = w_human;
        else human = b_human;
        
        Move move;
        do {
            move = (human ? human_move() : comp_move());
        } while (!p.move_is_valid(move));

        Move_to_string(move);
        p.update(move);
        
        if (p.is_over()) break;
        
        white = !white;
    }
    
    end();
}

Move Game::human_move() {
    int x, y;
    std::cout << "From file: ";
    std::cin >> x;
    std::cout << "From rank: ";
    std::cin >> y;
    Square from = Square_new(x, y);
    
    std::cout << "To file: ";
    std::cin >> x;
    std::cout << "To rank: ";
    std::cin >> y;
    Square to = Square_new(x, y);
    
    Move move = Move_new(from, to);
    
    return move;
}

Move Game::comp_move() {
    const int NUM_THREADS = 3; //temporary
    Search s = Search(&p, NUM_THREADS);
    s.start();
    wait_for_search();
    s.stop();
    return s.get_pv_move();
}

void Game::wait_for_search() {
    while (!should_stop_searching) {
        //TODO: update user with search data
       // if (!std::cin.eof()) {
            char input;
            std::cin >> input;
            if (input == 's') {
                std::cout <<"stopping...\n";
                should_stop_searching = true;
            }
        //}
    }
}

void Game::end() {
    std::cout << "Game over.\n";
}
