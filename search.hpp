//
//  search.hpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 7/5/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#ifndef search_hpp
#define search_hpp

#include "position.hpp"
#include "types.hpp"
#include "evaluate.hpp"

//PUBLIC
//finds a move at a given depth of search
Move search(Position p, Eval alpha, Eval beta, int depth);


//PRIVATE
//thse are called by search

//alphabeta() is the standard recursive search
Eval alphabeta(Position p, PosData *pd, Eval alpha, Eval beta, Move hash_move, int depth);

//alphabeta_leaf() is used at leaf nodes
Eval alphabeta_leaf(Position p, Eval alpha, Eval beta);

//see() looks at captures only and is used for the horizon effect
//returns absolute Eval (not relative to side to move)
Eval see(SeePosition sp, Square s);

#endif /* search_hpp */
