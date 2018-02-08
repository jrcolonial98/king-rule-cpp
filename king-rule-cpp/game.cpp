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
    
    //----------
    /*
    Move m = Move_new(1, 2);
    Move_set_piece(&m, WKNIGHT);
    Move_set_score(&m, SC_KILLER);
    Move_set_checked(&m, false);
    Move_set_is_promotion(&m, true);
    Move_set_is_en_passant(&m, true);
    std::cout << "Move from " << (int)Move_from(m) << " to " << (int)Move_to(m) << "\n";
    std::cout << "Piece " << (int)Move_piece(m) << " Score " << (int)Move_score(m) << "\n";
    std::cout << "Ch " << Move_checked(m) << " pro " << Move_is_promotion(m) << "\n";
    std::cout << "ep " << Move_is_en_passant(m) << "\n";
     */
    //----------
    
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
        if ( human && !p.move_is_valid(move) ) { //comp moves are always valid
            std::cout << "Move is not valid.\n";
            continue;
        }
        Move_to_string(move);
        p.update(move);
        break;
    }
    
    if (p.is_over()) end();
    else this->move(!white);
}

void Game::end() {
    std::cout << "Game over.\n";
}
