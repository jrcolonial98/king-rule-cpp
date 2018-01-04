//
//  position.cpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 5/22/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#include "position.hpp"

Position::Position() {
    for (int i = 0; i < 32; i++) {
        pieces[i] = INIT_PIECES[i];
    }
    for (int i = 0; i < 64; i++) {
        board[i] = INIT_BOARD[i];
    }
    
    moves_since_cp = 0;
    draw_offered = false;
    move_count = 0;
    move_index = 0;
    position_is_valid = true;
    pv_move = 0;
    material_imbalance = 0;
    
    w_OO = true; w_OOO = true;
    b_OO = true; b_OOO = true;
    en_passant_file = NOWHERE;
    
    find_moves();
    
    //TODO: create hash. pos_hash, pawn_hash. Define constants for these?
}

Position::Position(Position const &p) {
    for (int i = 0; i < 32; i++) {
        pieces[i] = p.pieces[i];
    }
    for (int i = 0; i < 64; i++) {
        board[i] = p.board[i];
    }
    
    //TODO: copy moves from move list over
    
    moves_since_cp = p.moves_since_cp;
    draw_offered = p.draw_offered;
    move_count = p.move_count;
    move_index = p.move_index;
    position_is_valid = p.position_is_valid;
    //pv_move = p.pv_move; //not this - this is specific to the position
    pv_move = 0;
    material_imbalance = p.material_imbalance;
    
    w_OO = p.w_OO; w_OOO = p.w_OOO;
    b_OO = p.b_OO; b_OOO = p.b_OOO;
    en_passant_file = p.en_passant_file;
    
    pos_hash = p.get_pos_hash();
    pawn_hash = p.get_pawn_hash();
}

bool Position::update(Move m) {
    Square from = Move_from(m), to = Move_to(m);
    Piece piece = piece_at(from);
    
    //TODO: threefold rep
    bool draw_claim_valid = false;
    if (moves_since_cp > 100) draw_claim_valid = true;
    if (draw_offered) draw_claim_valid = true;
    //TODO: if move includes claim
    if (Move_includes_offer(m)) draw_offered = true;
    else draw_offered = false;
    
    material_imbalance -= Piece_value(piece_at(to));
    
    //update board data and hash data
    if (Move_is_en_passant(m)) {
        int dest_displacement = (side_to_move() ? 8 : -8);
        Square dest = (Square)(to + dest_displacement); //diagonal from capture
        
        pos_hash ^= get_xor(piece, from);
        pos_hash ^= get_xor(piece_at(to), to);
        pos_hash ^= get_xor(piece, dest);
        
        pawn_hash ^= get_xor(piece, from);
        pawn_hash ^= get_xor(piece, dest);
        pawn_hash ^= get_xor(piece_at(to), to);
        
        pieces[board[from]] = dest;
        pieces[board[to]] = NOWHERE;
        board[to] = EMPTY;
        board[from] = EMPTY;
        board[dest] = piece;
    }
    else {
        pos_hash ^= get_xor(piece, from);
        pos_hash ^= get_xor(piece, to);
        pos_hash ^= get_xor(piece_at(to), to);
        
        if (Piece_is_pawn(piece)) {
            pawn_hash ^= get_xor(piece, from);
            pawn_hash ^= get_xor(piece, to);
        }
        if (Piece_is_pawn(piece_at(to))) {
            pawn_hash ^= get_xor(piece_at(to), to);
        }
        
        pieces[board[from]] = to;
        pieces[board[to]] = NOWHERE;
        board[to] = piece;
        board[from] = EMPTY;
    }
    
    //TODO: castling rights
    //consider how the data will be stored and handled. See position.h
    
    
    if (Piece_is_pawn(piece)) {
        
    }
    
    find_moves();
    
    move_count++;
    
    return position_is_valid;
    
    //find_moves() will invoke add_move() for all moves found
    //if add_move() takes an invalid move, position_is_valid
    // is set to false
}

/*
void Position::update() {
    find_moves();
}
 */

bool Position::is_valid(Move m) { //TODO
    return true;
}

bool Position::is_over() { //TODO
    return false;
}

Hash Position::get_pos_hash() const {
    return pos_hash;
}
Hash Position::get_pawn_hash() const {
    return pawn_hash;
}

inline Color Position::side_to_move() {
    return (move_count % 2 == 0 ? WHITE : BLACK);
}

inline Piece Position::piece_at(Square s) {
    return board[s];
}

inline Square Position::location_of(Piece p) {
    return pieces[p];
}

//FIX THESE FOR 0-32 VALUES NOT 0-12
//PieceID not Piece
inline bool Position::is_enemy(Piece p) {
    if (side_to_move() == WHITE)
        return (p > 5 && p < 12);
    else
        return (p < 6);
}

inline bool Position::is_enemy(Square s) {
    return is_enemy(piece_at(s));
}

inline bool Position::is_empty(Piece p) {
    return p == EMPTY;
}

inline bool Position::is_empty(Square s) {
    return is_empty(piece_at(s));
}

inline bool Position::is_empty_or_enemy(Piece p) {
    return is_empty(p) || is_enemy(p);
}

inline bool Position::is_empty_or_enemy(Square s) {
    return is_empty_or_enemy(piece_at(s));
}

void Position::find_moves() {
    //most possible moves in one position is 218
    
    move_index = 0;
    
    Square square, s;
    Piece piece;
    for (int j = 0; j < 32; j++) {
        square = pieces[j];
        piece = (Piece)j;
        
        if (is_enemy(piece)) continue;
        if (square == NOWHERE) continue;
        
        if (Piece_is_queen(piece)) {
            int x = Square_x(square);
            int count_right = H - x;
            int count_left = x - A;
            for (int i = 0; i < 8; i++) {
                int count;
                if (i < 3) //moving righward
                    count = count_right;
                else if (i > 3 && i < 7) //moving leftward
                    count = count_left;
                else count = 8;
                
                s = square;
                for (int k = 0; k < count; k++) {
                    s = (Square)(s + QUEEN_KING_DISP[i]);
                    if (!Square_is_valid(s)) break;
                    
                    if (is_empty(s))
                        add_move(Move_new(square, s));
                    else if (is_enemy(s)) {
                        add_move(Move_new(square, s));
                        break;
                    }
                    else break;
                }
            }
        }
        else if (Piece_is_rook(piece)) {
            int x = Square_x(square);
            int count_right = H - x;
            int count_left = x - A;
            for (int i = 0; i < 4; i++) {
                int count;
                if (i == 1)
                    count = count_right;
                else if (i == 3)
                    count = count_left;
                else count = 8;
                
                s = square;
                for (int k = 0; k < count; k++) {
                    s = (Square)(s + ROOK_DISP[i]);
                    if (!Square_is_valid(s)) break;
                    
                    if (is_empty(s))
                        add_move(Move_new(square, s));
                    else if (is_enemy(s)) {
                        add_move(Move_new(square, s));
                        break;
                    }
                    else break;
                }
            }
        }
        else if (Piece_is_bishop(piece)) {
            int x = Square_x(square);
            int count_right = H - x;
            int count_left = x - A;
            for (int i = 0; i < 4; i++) {
                int count;
                if (i < 2)
                    count = count_right;
                else
                    count = count_left;
                
                s = square;
                for (int k = 0; k < count; k++) {
                    s = (Square)(s + BISHOP_DISP[i]);
                    if (!Square_is_valid(s)) break;
                    
                    if (is_empty(s))
                        add_move(Move_new(square, s));
                    else if (is_enemy(s)) {
                        add_move(Move_new(square, s));
                        break;
                    }
                    else break;
                }
            }
        }
        else if (Piece_is_king(piece)) {
            int x = Square_x(square);

            for (int i = 0; i < 8; i++) {
                if (i < 3 && x == H) continue;
                else if (i > 3 && i < 7 && x == A) continue;
                
                s = (Square)(square + QUEEN_KING_DISP[i]);
                if (!Square_is_valid(s)) continue;
                    
                if (is_empty_or_enemy(s))
                    add_move(Move_new(square, s));
            }
        }
        else if (Piece_is_knight(piece)) {
            int x = Square_x(square);
            
            for (int i = 0; i < 8; i++) {
                if (x == H && i < 4) continue;
                else if (x == G && (i == 1 || i == 2)) continue;
                else if (x == B && (i == 5 || i == 6)) continue;
                else if (x == A && i > 3) continue;
                
                s = (Square)(square + KNIGHT_DISP[i]);
                if (!Square_is_valid(s)) continue;
                
                if (is_empty_or_enemy(s))
                    add_move(Move_new(square, s));
            }
        }
        else if (Piece_is_pawn(piece)) {
            int x = Square_x(square);
            int y = Square_y(square);
            int disp1, dispL, dispR, start;
            if (side_to_move()) {
                disp1 = DISP_N; dispL = DISP_NW; dispR = DISP_NE;
                start = TWO;
            }
            else {
                disp1 = DISP_S; dispL = DISP_SE; dispR = DISP_SW;
                start = SEVEN;
            }
            
            bool promotion = false;
            if (Square_y(square) == (side_to_move() ? SEVEN : TWO))
                promotion = true;
            
            //move forward 1
            s = (Square)(square + disp1);
            if (is_empty(s)) add_move(Move_new(square, s));
            
            //move forward 2
            if (y == start && is_empty((Square)(s + disp1)))
                add_move(Move_new(square, (Square)(s + disp1)));
            
            //capture left
            s = (Square)(square + dispL);
            if (is_enemy(s)) add_move(Move_new(square, s));
            
            //capture right
            s = (Square)(square + dispR);
            if (is_enemy(s)) add_move(Move_new(square, s));
            
            //en passant
            if (y == (side_to_move() ? FIVE : FOUR)) {
                if (en_passant_file - x == 1)
                    add_move(Move_new(square, (Square)(s + 1)));
                else if (en_passant_file - x == -1)
                    add_move(Move_new(square, (Square)(s - 1)));
            }
        }
    }
}

void Position::add_move(Move m) {
    //NOTE: the move MUST be assigned some score
    
    Score score = SC_NON_KILLER; //default score
    
    Square from = Move_from(m);
    Square to = Move_to(m);
    if (!is_empty(to)) {
        Piece p = piece_at(to);
        if (Piece_is_king(p)) position_is_valid = false;
        
        if (Piece_value(p) > Piece_value(piece_at(from)))
            score = SC_POS_CAPTURE;
        else if (Piece_value(p) == Piece_value(piece_at(from)))
            score = SC_EQUAL_CAPTURE;
        else
            score = SC_NEG_CAPTURE;
    }
    if (Move_is_promotion(m)) {
        //TODO
    }
    if (Square_y(from) == Square_y(to) && Piece_is_pawn(board[from])) {
        Move_set_is_en_passant(&m, true);
    }
    
    Move_set_score(&m, score);
    moves[move_index] = m;
    move_index++;
}

void Position::set_pv_move(Move move) {
    pv_move = move;
}

Move Position::get_pv_move() {
    return pv_move;
}

Hash Position::get_xor(Piece p, Square s) {
    if (p == EMPTY) return 0;
    int index = p * 64 + s;
    return hash_values[index];
}

SeePosition::SeePosition(Position &p, Square s) {
    for (int i = 0; i < 64; i++) {
        if (i < 32) pieces[i] = p.location_of((Piece)i);
        board[i] = p.piece_at((Square)i);
    }
    position_is_valid = p.position_is_valid;
    material_imbalance = p.material_imbalance;
    move_count = p.move_count;
    
    
    this->s = s;
}

void SeePosition::update(Move m) {
    Square from = Move_from(m), to = Move_to(m);
    Piece piece = piece_at(from);
    
    material_imbalance -= Piece_value(piece_at(to));
    pieces[board[from]] = to;
    pieces[board[to]] = NOWHERE;
    board[to] = piece;
    board[from] = EMPTY;
    
    //find_moves() done manually, not needed here
    
    move_count++;
}

void SeePosition::find_moves() {
    smallest_attacker_value = 0;
    for (int i = 0; i < 32; i++) {
        if (pieces[i] != NOWHERE) {
            add_move(Move_new(pieces[i], s));
        }
    }
}

Move SeePosition::get_smallest_attacker_move() {
    return smallest_attacker_move;
}

bool SeePosition::is_valid(Move m) {
    return true; //if this move exists/can be made.
    //no need to check for pin/legality
    //SeePos does after-the-fact invalid pos checking
}

Piece SeePosition::piece_at(Square s) {
    return board[s];
}

//TODO: move this to some object ListOfMoves
// as well as other functions that access the list
void SeePosition::add_move(Move m) {
    Square from = Move_from(m);
    Square to = Move_to(m);
    if (to != s) return;
    if (!is_valid(Move_new(from, to))) return;
    if (Piece_is_king(piece_at(to))) position_is_valid = false;
    
    if (abs(Piece_value(piece_at(from))) <= abs(smallest_attacker_value)) {
        smallest_attacker_move = Move_new(from, to);
        smallest_attacker_value = Piece_value(piece_at(from));
    }
}
