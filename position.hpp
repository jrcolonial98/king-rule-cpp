//
//  position.h
//  KingRule Cpp
//
//  Created by Brendan Gregory on 5/22/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#ifndef position_h
#define position_h

#include "types.hpp"
#include "ptable.hpp"

const Square INIT_PIECES[32] = {
    E1, D1, H1, A1, G1, B1, F1, C1, A2, B2, C2, D2, E2, F2, G2, H2,
    E8, D8, H8, A8, G8, B8, F8, C8, A7, B7, C7, D7, E7, F7, G7, H7
};
const Piece INIT_BOARD[64] = {
    WROOK, WKNIGHT, WBISHOP, WQUEEN, WKING, WBISHOP, WKNIGHT, WROOK,
    WPAWN, WPAWN,   WPAWN,   WPAWN,  WPAWN, WPAWN,   WPAWN,   WPAWN,
    EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY, EMPTY,   EMPTY,   EMPTY,
    EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY, EMPTY,   EMPTY,   EMPTY,
    EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY, EMPTY,   EMPTY,   EMPTY,
    EMPTY, EMPTY,   EMPTY,   EMPTY,  EMPTY, EMPTY,   EMPTY,   EMPTY,
    BPAWN, BPAWN,   BPAWN,   BPAWN,  BPAWN, BPAWN,   BPAWN,   BPAWN,
    BROOK, BKNIGHT, BBISHOP, BQUEEN, BKING, BBISHOP, BKNIGHT, BROOK
};
const Square
    DISP_NE  = (Square)9,   DISP_E   = (Square)1,
    DISP_SE  = (Square)-7,  DISP_S   = (Square)-8,
    DISP_SW  = (Square)-9,  DISP_W   = (Square)-1,
    DISP_NW  = (Square)7,   DISP_N   = (Square)8,
    DISP_NNE = (Square)17,  DISP_SNE = (Square)10,
    DISP_NSE = (Square)-6,  DISP_SSE = (Square)-15,
    DISP_NSW = (Square)-10, DISP_SSW = (Square)-17,
    DISP_NNW = (Square)15,  DISP_SNW = (Square)6;
const Square QUEEN_KING_DISP[8] = {DISP_NE, DISP_E, DISP_SE, DISP_S, DISP_SW, DISP_W, DISP_NW, DISP_N};
const Square BISHOP_DISP[4] = {DISP_NE, DISP_SE, DISP_SW, DISP_NW};
const Square KNIGHT_DISP[8] = {DISP_NNE, DISP_SNE, DISP_NSE, DISP_SSE, DISP_NSW, DISP_SSW, DISP_NNW, DISP_SNW};
const Square ROOK_DISP[4] = {DISP_N, DISP_E, DISP_S, DISP_W};

class Position {
    
public:
    Position();
    Position(Position const &p);
    
    int moves_since_cp;
    bool draw_offered;
    int move_count;
    
    Move moves[256]; //need a better object for this (?)
    int move_index;
    
    bool position_is_valid;
    inline void set_pv_move(Move move); //make this for pointers?
    inline Move get_pv_move();
    double material_imbalance;
    
    bool update(Move m); //returns whether subsequent pos is valid
    //void unmake_move(); //used for search, to save time cloning positions
    void update(); //TODO: change design of this. Just calls find_moves()
    bool is_valid(Move m); //can this move be made in this position?
    bool is_over(); //position is checkmate or stalemate
    Hash get_pos_hash() const;
    Hash get_pawn_hash() const;
    
    Color side_to_move();
    Piece piece_at(Square s); //TODO: turn this into piece_id_at
    Square location_of(Piece p);
    //
    inline bool is_enemy(Piece p); //relative to side to move
    inline bool is_enemy(Square s);
    inline bool is_empty(Piece p);
    inline bool is_empty(Square s);
    inline bool is_empty_or_enemy(Piece p);
    inline bool is_empty_or_enemy(Square s);
    
    Piece board[64]; //piece on each square
    Square pieces[32]; //location of each piece
    
private:
    bool w_OO, w_OOO, b_OO, b_OOO; //TODO: consider putting these in a char
    //TODO: consider a bit in the above char for "has queen moved" for opening penalty
    
    char en_passant_file;
    Hash pos_hash;
    Hash pawn_hash;
    
    void find_moves();
    void add_move(Move m);
    Hash get_xor(Piece p, Square s);
    
    /* 
    //MOVE UNMAKING DATA
    Piece last_piece_taken;
    Move last_move_made;
     
    */
    
    Move pv_move;
    
};

class SeePosition {
    
    // no need for Position object, just a board and pieces.
    // as well as position_is_valid and is_valid(Move m)
    // and material_imbalance and move_count
    
public:
    SeePosition(Position &p, Square s);
    
    void update(Move m);
    void find_moves(); //TODO: this was in the pseudo, look at this
    Move get_smallest_attacker_move(); //return EMPTY if nothing
    bool is_valid(Move m);
    Piece piece_at(Square s);
private:
    Piece board[64];
    Square pieces[32];
    bool position_is_valid;
    double material_imbalance;
    int move_count;
    Square s;
    char smallest_attacker_value;
    Move smallest_attacker_move;
    void add_move(Move m);
};

#endif /* position_h */
