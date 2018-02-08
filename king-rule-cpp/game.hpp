//
//  game.hpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 5/22/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#ifndef game_hpp
#define game_hpp

#include "types.hpp"
#include "position.hpp"
#include "search.hpp"

class Game {
    Position p;
    bool w_human, b_human;
    Move human_move();
    Move comp_move();
    void move(bool white); //determines move and makes move, with parameter whose turn is it
    void end();
public:
    Game(bool wh, bool bh) { w_human = wh; b_human = bh; }
    void start();
};

#endif /* game_hpp */
