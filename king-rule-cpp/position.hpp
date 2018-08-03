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

extern int INIT_POS_HASH; //defined in position.cpp
extern int INIT_PAWN_HASH; //defined in position.cpp
extern Hash hash_values[781]; //defined in position.cpp
    //one for each piece on each square, 12x64 (0-767)
    //one for each side to move is white (768)
    //four for castling rights (769-772)
    //eight for en passant file (773-780)
Hash get_xor(Piece p, Square s);
void hash_values_init();

const Square INIT_PIECES[32] = {
    E1, D1, H1, A1, G1, B1, F1, C1, A2, B2, C2, D2, E2, F2, G2, H2,
    E8, D8, H8, A8, G8, B8, F8, C8, A7, B7, C7, D7, E7, F7, G7, H7
};
const PieceID INIT_BOARD[64] = {
    WQR, WQN, WQB, WQ, WK, WKB, WKN, WKR,
    WAP, WBP, WCP, WDP, WEP, WFP, WGP, WHP,
    EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID,
    EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID,
    EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID,
    EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID, EMPTYID,
    BAP, BBP, BCP, BDP, BEP, BFP, BGP, BHP,
    BQR, BQN, BQB, BQ, BK, BKB, BKN, BKR
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

//       NNW       NNE
//  SNW  NW    N    NE  SNE
//        W   [ ]   E
//  NSW  SW    S    SE  NSE
//       SSW       SSE
const Square QUEEN_KING_DISP[8] = {DISP_NE, DISP_E, DISP_SE, DISP_S, DISP_SW, DISP_W, DISP_NW, DISP_N};
const Square BISHOP_DISP[4] = {DISP_NE, DISP_SE, DISP_SW, DISP_NW};
const Square KNIGHT_DISP[8] = {DISP_NNE, DISP_SNE, DISP_NSE, DISP_SSE, DISP_SSW, DISP_NSW, DISP_SNW, DISP_NNW};
const Square ROOK_DISP[4] = {DISP_N, DISP_E, DISP_S, DISP_W};

class MoveList {
public:
    MoveList(); //how to create properly?
    void add(Move move);
    void set_best(Move move);
    Move* next(); //get next move, in order from best to worst
    void reset(); //reset iterator
    void clear();
    int size();
private:
    Move moves[SC_COUNT][256];
    Move best;
    //most possible moves in one position is 218
    //256 is "more than enough" space, "just in case"
    //one list for each type of move for faster adding/searching
    //statically allocated, so memory isn't a problem
    int count;
    int new_move_index[SC_COUNT];
    int iterator_index[SC_COUNT];
    int interator_count; //number of moves iterated so far
};

class Position {
    
public:
    Position();
    Position(Position const &p);
    
    bool is_over(); //position is checkmate or stalemate
    int moves_since_cp;
    bool draw_offered;
    int move_count; //not number of moves possible, but number of moves made
    bool is_valid;
    void set_pv_move(Move move);
    Move get_pv_move();
    short material_imbalance;
    
    bool update(Move m); //returns whether subsequent pos is valid
    bool update_next(Move m); //updates Position* next. assumes next is identical to this
    //TODO: every time update(m) is called, remember that it returns bool
    //void unmake_move(); //used for search, to save time cloning positions
    void unmake_move();
    
    void update(); //TODO: change design of this. Just calls find_moves()
    
    bool move_is_valid(Move m); //can this move be made in this position?
    Move* get_next_move();
    void reset_move_iterator();
    
    Hash get_pos_hash() const;
    Hash get_pawn_hash() const;
    
    Color side_to_move();
    PieceID piece_id_at(Square s);
    Piece piece_at(Square s);
    Square location_of(PieceID p);
    //
    inline bool is_enemy(Piece p); //relative to side to move
    inline bool is_enemy(Square s);
    inline bool is_empty(Piece p);
    inline bool is_empty(Square s);
    inline bool is_empty_or_enemy(Piece p);
    inline bool is_empty_or_enemy(Square s);
    
    Position* get_next();
    Position* get_previous();
    void set_previous(Position* p);
    
private:
    PieceID board[64]; //piece on each square
    Square pieces[32]; //location of each piece
    
    bool w_OO, w_OOO, b_OO, b_OOO; //TODO: consider putting these in a char
    //TODO: consider a bit in the above char for "has queen moved" for opening penalty
    
    char en_passant_file;
    Hash pos_hash;
    Hash pawn_hash;
    
    MoveList moves;
    
    void find_moves(); 
    void add_move(Move m);
    
    //move unmaking data
    PieceID last_piece_id_taken;
    Move last_move_made;
    
    Move pv_move; //points to an element of moves
    
    Position* next;
    Position* previous;
    
};

class SeePosition {
    
public:
    SeePosition(Position &p, Square s);
    //no longer need to run find_moves() on Position
    //ensure there is no king on s
    void update(Move m);
    Move get_smallest_attacker_move(); //return MOVE_NONE if nothing
    //bool move_is_valid(Move m);
    PieceID piece_id_at(Square s);
    Piece piece_at(Square s); //rename get_piece
    //get_location
    bool is_enemy(Square s); 
    bool is_empty(Square s);
    bool is_enemy(Piece p);
    Color side_to_move();
private:
    Square s;
    int x, y;
    
    PieceID board[64];
    int move_index;
    Square immediately_attacking_squares[16];
    //Square pieces[32];
    
    //bool is_valid;
    //double material_imbalance;
    int move_count;
    
    char smallest_attacker_value;
    Move smallest_attacker_move;
    
    void add_move(Square from);
};

#endif /* position_h */
