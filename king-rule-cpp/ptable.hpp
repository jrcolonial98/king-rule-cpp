//
//  ptable.hpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 7/8/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#ifndef ptable_h
#define ptable_h

#include "types.hpp"

//TODO: list of positions within a game. Used for threefold.

//IDEA: make PosData and PawnData members of Position?

namespace PTable {
    
const int PT_POS_SIZE = 1048576;
const int PT_PAWN_SIZE = 1048576;



struct PosData {
    //std::mutex mtx;
    Eval eval;
    char eval_depth;
    Hash hash;
    Move best_or_refutation_move;
    char node_type;
};
struct PawnData {
    //std::mutex mtx;
    unsigned int backwards_squares[2];
    char files_occupied[2];
    Eval eval; //always relative to white
    Hash hash;
};

//EVAL_EXACT: eval represents the results of a recursive search, PV node
//EVAL_LOWER: eval represents a beta cutoff - player can do at least as good as beta
//EVAL_UPPER: eval represents a fail-low - all moves searched evaluate to less than alpha
const char EVAL_EXACT = 0, EVAL_LOWER = 1, EVAL_UPPER = 2;



//PUBLIC
void init();

void pt_pos_insert(Hash hash, PosData *data);
bool pt_pos_get(PosData *pd, Hash hash); //returns false if hash doesn't match

void pt_pawn_insert(Hash hash, PawnData *data);
bool pt_pawn_get(PawnData *pd, Hash hash);

void pt_pawn_reset();
void pt_pos_reset();


//PRIVATE
void pt_pos_init();
void pt_pawn_init();

int pt_pos_hash_to_address(Hash hash);
int pt_pawn_hash_to_address(Hash hash);

    extern std::atomic<PosData>* pos_table; //defined in ptable.cpp
    extern std::atomic<PawnData>* pawn_table; //defined in ptable.cpp
//extern std::mutex pos_mtx[];
//extern std::mutex pawn_mtx[];

} //namespace PTable

#endif /* ptable_hpp */
