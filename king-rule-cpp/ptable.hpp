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

const int PT_POS_SIZE = 1048576;
const int PT_PAWN_SIZE = 1048576;

extern int INIT_POS_HASH;
extern int INIT_PAWN_HASH;

struct PosData {
    Eval eval;
    char eval_depth;
    Hash hash;
    Move best_or_refutation_move;
    char node_type;
};
//size: 10 bytes?

//EVAL_EXACT: eval represents the results of a recursive search, PV node
//EVAL_LOWER: eval represents a beta cutoff - player can do at least as good as beta
//EVAL_UPPER: eval represents a fail-low - all moves searched evaluate to less than alpha
const char EVAL_EXACT = 0, EVAL_LOWER = 1, EVAL_UPPER = 2;

struct PawnData {
    unsigned int backwards_squares[2];
    char files_occupied[2];
    Eval eval; //always relative to white
    Hash hash;
};
//size: 7 bytes?


//PUBLIC
void pt_hash_values_init();

void pt_pos_init();
void pt_pos_insert(Hash hash, PosData data);
PosData* pt_pos_get(Hash hash); //returns NULL if hash doesn't match

void pt_pawn_init();
void pt_pawn_insert(Hash hash, PawnData data);
PawnData* pt_pawn_get(Hash hash);
void pt_pawn_reset();

Hash pt_get_xor(Piece p, Square s);


//PRIVATE
int pt_pos_hash_to_address(Hash hash);
int pt_pawn_hash_to_address(Hash hash);

extern PosData* pos_table;
extern PawnData* pawn_table;

extern Hash hash_values[781];
    //one for each piece on each square, 12x64 (0-767)
    //one for each side to move is white (768)
    //four for castling rights (769-772)
    //eight for en passant file (773-780)

#endif /* ptable_hpp */
