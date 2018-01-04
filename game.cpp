//
//  game.cpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 5/22/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#include "game.hpp"

void Game::start() {
    pt_hash_values_init(); //random numbers used for position hash
    
    move(true);
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
    const int SEARCH_DEPTH = 5;
    return search(p, EVAL_MIN, EVAL_MAX, SEARCH_DEPTH);
}

void Game::move(bool white) {
    Move move;
    
    bool human;
    if (white) human = w_human;
    else human = b_human;
    
    while (true) {
        move = (human ? human_move() : comp_move());
        if ( human && !p.is_valid(move) ) { //comp moves are always valid
            std::cout << "Move is not valid.\n";
            continue;
        }
        std::cout << "Move from " << Move_from(move) << " to " << Move_to(move) << "\n";
        p.update(move);
        break;
    }
    
    if (p.is_over()) end();
    else this->move(!white);
}

void Game::end() {
    std::cout << "Game over.\n";
}
