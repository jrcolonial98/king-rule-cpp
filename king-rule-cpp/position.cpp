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
    is_valid = true;
    pv_move = 0;
    material_imbalance = 0;
    
    w_OO = true; w_OOO = true;
    b_OO = true; b_OOO = true;
    en_passant_file = NOWHERE;
    
    pos_hash = INIT_POS_HASH;
    pawn_hash = INIT_PAWN_HASH;
    
    find_moves();
}

Position::Position(Position const &p) {
    for (int i = 0; i < 32; i++) {
        pieces[i] = p.pieces[i];
    }
    for (int i = 0; i < 64; i++) {
        board[i] = p.board[i];
    }
    
    for (int i = 0; i < p.move_index; i++) {
        moves[i] = p.moves[i];
    }
    
    moves_since_cp = p.moves_since_cp;
    draw_offered = p.draw_offered;
    move_count = p.move_count;
    move_index = p.move_index;
    is_valid = p.is_valid;
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
    //assumed: m is a possible move
    //not assumed: m is a legal move (might take king, might be a pinned piece, might be in check)
    
    Square from = Move_from(m), to = Move_to(m);
    PieceID piece_id = board[from];
    Piece piece = PieceID_to_Piece(piece_id);
    PieceID captured_piece_id = board[to];
    Piece captured_piece = PieceID_to_Piece(captured_piece_id);
    
    //TODO: threefold rep
    
    bool draw_claim_valid = false;
    if (moves_since_cp > 100) draw_claim_valid = true; //fifty move rule
    if (draw_offered) draw_claim_valid = true; //accept draw offer
    if (Move_includes_claim(m) && draw_claim_valid) {
        //end game with draw
    }
    
    if (Move_includes_offer(m)) draw_offered = true;
    else draw_offered = false;
    
    material_imbalance -= Piece_value(captured_piece);
    
    //update board data and hash data
    if (Move_is_en_passant(m)) {
        int dest_displacement = (side_to_move() ? 8 : -8);
        Square dest = (Square)(to + dest_displacement); //diagonal from capture
        
        pos_hash ^= pt_get_xor(piece, from);
        pos_hash ^= pt_get_xor(captured_piece, to);
        pos_hash ^= pt_get_xor(piece, dest);
        
        pawn_hash ^= pt_get_xor(piece, from);
        pawn_hash ^= pt_get_xor(piece, dest);
        pawn_hash ^= pt_get_xor(captured_piece, to);
        
        pieces[board[from]] = dest;
        pieces[board[to]] = NOWHERE;
        board[to] = EMPTYID;
        board[from] = EMPTYID;
        board[dest] = piece_id;
    }
    else {
        pos_hash ^= pt_get_xor(piece, from);
        pos_hash ^= pt_get_xor(piece, to);
        pos_hash ^= pt_get_xor(captured_piece, to);
        
        if (Piece_is_pawn(piece)) {
            pawn_hash ^= pt_get_xor(piece, from);
            pawn_hash ^= pt_get_xor(piece, to);
        }
        if (Piece_is_pawn(piece_at(to))) {
            pawn_hash ^= pt_get_xor(captured_piece, to);
        }
        
        pieces[board[from]] = to;
        pieces[board[to]] = NOWHERE;
        board[to] = piece_id;
        board[from] = EMPTYID;
    }
    
    //set new en passant file
    if (Piece_is_pawn(piece) && abs(Square_y(from) - Square_y(to)) == 2) { //pawn moves two squares
        en_passant_file = Square_x(from);
    }
    else en_passant_file = NOWHERE;
    
    //TODO: castling rights
    //consider how the data will be stored and handled. See position.h
    
    if (Piece_is_pawn(piece) || captured_piece != EMPTY) {
        moves_since_cp = 0;
    }
    
    move_count++;
    
    find_moves();
    
    
    return is_valid; //will be false if find_moves finds a king capture
}

bool Position::move_is_valid(Move m) { //TODO
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

inline PieceID Position::piece_id_at(Square s) {
    return board[s];
}

inline Piece Position::piece_at(Square s) {
    return PieceID_to_Piece(piece_id_at(s));
}

Square Position::location_of(PieceID p) {
    return pieces[p];
}

inline bool Position::is_enemy(Piece p) {
    if (side_to_move() == WHITE)
        return Piece_is_black(p);
    else
        return Piece_is_white(p);
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
    
    Square from, to;
    PieceID piece_id;
    Piece piece;
    for (int j = 0; j < 32; j++) {
        
        from = pieces[j];
        piece_id = (PieceID)j;
        piece = PieceID_to_Piece(piece_id);
        
        if (is_enemy(piece)) continue;
        if (from == NOWHERE) continue;
        
        if (Piece_is_queen(piece)) {
            int x = Square_x(from);
            int count_right = H - x;
            int count_left = x - A;
            
            for (int i = 0; i < 8; i++) {
                int count;
                if (i < 3) //NE, E, SE
                    count = count_right;
                else if (i > 3 && i < 7) //SW, W, NW
                    count = count_left;
                else count = 7;
                
                to = from;
                for (int k = 0; k < count; k++) {
                    to = (Square)(to + QUEEN_KING_DISP[i]);
                    if (!Square_is_valid(to)) break;
                    
                    if (is_empty(to))
                        add_move(Move_new(from, to));
                    else if (is_enemy(to)) {
                        add_move(Move_new(from, to));
                        break;
                    }
                    else break;
                }
            }
        }
        else if (Piece_is_rook(piece)) {
            int x = Square_x(from);
            int count_right = H - x;
            int count_left = x - A;
            
            for (int i = 0; i < 4; i++) {
                int count;
                if (i == 1)
                    count = count_right;
                else if (i == 3)
                    count = count_left;
                else count = 7;
                
                to = from;
                for (int k = 0; k < count; k++) {
                    to = (Square)(to + ROOK_DISP[i]);
                    if (!Square_is_valid(to)) break;
                    
                    if (is_empty(to))
                        add_move(Move_new(from, to));
                    else if (is_enemy(to)) {
                        add_move(Move_new(from, to));
                        break;
                    }
                    else break;
                }
            }
        }
        else if (Piece_is_bishop(piece)) {
            int x = Square_x(from);
            int count_right = H - x;
            int count_left = x - A;
            
            for (int i = 0; i < 4; i++) {
                int count;
                if (i < 2)
                    count = count_right;
                else
                    count = count_left;
                
                to = from;
                for (int k = 0; k < count; k++) {
                    to = (Square)(to + BISHOP_DISP[i]);
                    if (!Square_is_valid(to)) break;
                    
                    if (is_empty(to))
                        add_move(Move_new(from, to));
                    else if (is_enemy(to)) {
                        add_move(Move_new(from, to));
                        break;
                    }
                    else break;
                }
            }
        }
        else if (Piece_is_king(piece)) {
            int x = Square_x(from);

            for (int i = 0; i < 8; i++) {
                if (i < 3 && x == H) continue;
                else if (i > 3 && i < 7 && x == A) continue;
                
                to = (Square)(from + QUEEN_KING_DISP[i]);
                if (!Square_is_valid(to)) continue;
                    
                if (is_empty_or_enemy(to))
                    add_move(Move_new(from, to));
            }
        }
        else if (Piece_is_knight(piece)) {
            int x = Square_x(from);
            
            for (int i = 0; i < 8; i++) {
                if (x == H && i < 4) continue;
                else if (x == G && (i == 1 || i == 2)) continue;
                else if (x == B && (i == 5 || i == 6)) continue;
                else if (x == A && i > 3) continue;
                
                to = (Square)(from + KNIGHT_DISP[i]);
                if (!Square_is_valid(to)) continue;
                
                if (is_empty_or_enemy(to))
                    add_move(Move_new(from, to));
            }
        }
        else if (Piece_is_pawn(piece)) {
            int x = Square_x(from);
            int y = Square_y(from);
            
            Square disp_1, disp_left, disp_right, start;
            if (side_to_move()) {
                disp_1 = DISP_N; disp_left = DISP_NW; disp_right = DISP_NE;
                start = TWO;
            }
            else {
                disp_1 = DISP_S; disp_left = DISP_SW; disp_right = DISP_SE;
                start = SEVEN;
            }
            
            bool promotion = false; //TODO: add this to the move
            if (y == (side_to_move() ? SEVEN : TWO))
                promotion = true;
            
            //move forward 1
            to = from + disp_1;
            if (is_empty(to)) add_move(Move_new(from, to));
            
            //move forward 2
            if (y == start && is_empty(from + disp_1))
                add_move(Move_new(from, from + disp_1 + disp_1));
            
            //capture left
            if (x != A) {
                to = from + disp_left;
                if (is_enemy(to)) add_move(Move_new(from, to));
            }
            
            //capture right
            if (x != H) {
                to = from + disp_right;
                if (is_enemy(to)) add_move(Move_new(from, to));
            }
            
            //en passant
            if (y == (side_to_move() ? FIVE : FOUR)) {
                if (en_passant_file - x == 1)
                    add_move(Move_new(from, from + 1));
                else if (en_passant_file - x == -1)
                    add_move(Move_new(from, from - 1));
            }
        }
    }
}

void Position::add_move(Move m) {
    //NOTE: the move MUST be assigned some score
    
    Square from = Move_from(m);
    Square to = Move_to(m);
    Piece piece = piece_at(from);
    
    Score score = SC_NON_KILLER; //default score
    
    if (!is_empty(to)) {
        Piece captured_piece = piece_at(to);
        if (Piece_is_king(captured_piece)) is_valid = false;
        
        if (abs(Piece_value(captured_piece)) > abs(Piece_value(piece)))
            score = SC_POS_CAPTURE;
        else if (Piece_value(captured_piece) == -Piece_value(piece))
            score = SC_EQUAL_CAPTURE;
        else
            score = SC_NEG_CAPTURE;
    }
    if (Piece_is_pawn(piece)) {
        if (Square_y(to) == ONE || Square_y(to) == EIGHT)
            Move_set_is_promotion(&m, true);
        else if (Square_y(to) == Square_y(from))
            Move_set_is_en_passant(&m, true);
    }
    
    Move_set_piece(&m, piece);
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

SeePosition::SeePosition(Position &p, Square s) {
    for (int i = 0; i < 64; i++) {
        //if (i < 32) pieces[i] = p.location_of((PieceID)i);
        board[i] = p.piece_id_at((Square)i);
    }
    move_count = p.move_count;
    
    this->s = s;
    x = Square_x(s);
    y = Square_y(s);
    
    //find valid captures
    int count_right = H - x;
    int count_left = x - A;
    Square from;
    
    move_index = 0;
    for (int i = 0; i < 4; i++) { //look for attackers on diagonal
        Color enemy = ((i == 1 || i == 2) ? WHITE : BLACK);
        int count;
        if (i < 2)
            count = count_right;
        else
            count = count_left;
            
        from = s;
        for (int k = 0; k < count; k++) {
            from = from + BISHOP_DISP[i];
            if (!Square_is_valid(from)) break;
            if (is_empty(from)) continue;
            else {
                Piece p = PieceID_to_Piece(board[from]);
                if (Piece_is_queen(p) || Piece_is_bishop(p)) //bishop or queen attacking
                    add_move(from);
                else if (k == 0 && Piece_is_pawn(p) && Piece_is_white(p) == enemy) //pawn immediately attacking
                    add_move(from);
                break;
            }
        }
    }
    for (int i = 0; i < 4; i++) { //look for attackers on file/rank
        int count;
        if (i == 1)
            count = count_right;
        else if (i == 3)
            count = count_left;
        else count = 7;
        
        from = s;
        for (int k = 0; k < count; k++) {
            from = from + ROOK_DISP[i];
            if (!Square_is_valid(from)) break;
            if (is_empty(from)) continue;
            else {
                Piece p = PieceID_to_Piece(board[from]);
                if (Piece_is_rook(p) || Piece_is_queen(p))
                    add_move(from);
                break;
            }
        }
    }
    for (int i = 0; i < 8; i++) { //look for knight attackers
        if (x == H && i < 4) continue;
        else if (x == G && (i == 1 || i == 2)) continue;
        else if (x == B && (i == 5 || i == 6)) continue;
        else if (x == A && i > 3) continue;
        
        from = s;
        
        from = from + KNIGHT_DISP[i];
        if (!Square_is_valid(from)) continue;
        if (is_empty(from)) continue;
        else {
            Piece p = PieceID_to_Piece(board[from]);
            if (Piece_is_knight(p))
                add_move(from);
            break;
        }
    }
}

void SeePosition::update(Move m) {
    Square from = Move_from(m);
    int dx = Square_x(s) - Square_x(from);
    int dy = Square_y(s) - Square_y(from);
    
    //remove old move from imm_atk_squ
    for (int i = 0; i < move_index; i++) {
        if (immediately_attacking_squares[i] == from) {
            move_index--;
            immediately_attacking_squares[i] = NOWHERE;
            break;
        }
    }
    
    //add new moves that may have been blocked (can't be pawn or knight)
    if (dx == 0) { //up or down movement from move m
        Square disp;
        if (dy > 0) {
            dy = -1;
            disp = DISP_S;
        }
        else {
            dy = 1;
            disp = DISP_N;
        }
        
        for (int i = 0; i < 7; i++) {
            from = from + disp;
            if (!Square_is_valid(from)) break;
            if (is_empty(from)) continue;
            else {
                Piece p = PieceID_to_Piece(board[from]);
                if (Piece_is_rook(p) || Piece_is_queen(p)) {
                    add_move(from);
                }
                break;
            }
        }
        
    }
    else if (dy == 0) {
        int x = Square_x(from);
        int count;
        int count_right = H - x;
        int count_left = x - A;
        Square disp;
        
        if (dx > 0) {
            dx = -1;
            disp = DISP_W;
            count = count_left;
        }
        else {
            dx = 1;
            disp = DISP_E;
            count = count_right;
        }
        
        for (int i = 0; i < count; i++) {
            from = from + disp;
            if (is_empty(from)) continue;
            else {
                Piece p = PieceID_to_Piece(board[from]);
                if (Piece_is_rook(p) || Piece_is_queen(p)) {
                    add_move(from);
                }
                break;
            }
        }
    }
    else if (abs(dx) == abs(dy)) { //diagonal movement
        int x = Square_x(from);
        int count;
        int count_right = H - x;
        int count_left = x - A;
        Square disp;
        
        if (dx > 0) {
            dx = -1;
            disp = DISP_W;
            count = count_left;
        }
        else {
            dx = 1;
            disp = DISP_E;
            count = count_right;
        }
        if (dy > 0) {
            dy = -1;
            disp += DISP_S; //adding it changes E/W to SE/SW
        }
        else {
            dy = 1;
            disp += DISP_N;
        }
        
        for (int i = 0; i < count; i++) {
            from = from + disp;
            if (!Square_is_valid(from)) break;
            if (is_empty(from)) continue;
            else {
                Piece p = PieceID_to_Piece(board[from]);
                if (Piece_is_bishop(p) || Piece_is_queen(p)) {
                    add_move(from);
                }
                break;
            }
        }
    }
    
    move_count++;
}

Move SeePosition::get_smallest_attacker_move() {
    Move move = 0;
    
    Color side_to_move = (move_count % 2 == 0 ? WHITE : BLACK);
    Eval min_value = EVAL_MAX;
    for (int i = 0; i < move_index; i++) {
        if (immediately_attacking_squares[i] == NOWHERE) continue;
        Piece p = PieceID_to_Piece(board[immediately_attacking_squares[i]]);
        if (Piece_is_white(p) == side_to_move) {
            Eval value;
            if (Piece_is_king(p)) value = 1000; //arbitrary value greater than queen value
            else value = abs(Piece_value(p));
            
            if (value < min_value) {
                min_value = value;
                move = Move_new(immediately_attacking_squares[i], s);
            }
        }
    }
    
    return move;
}

bool SeePosition::is_enemy(Square s) {
    return is_enemy(PieceID_to_Piece(board[s]));
}

bool SeePosition::is_empty(Square s) {
    return board[s] == EMPTYID;
}

PieceID SeePosition::piece_id_at(Square s) {
    return board[s];
}

Piece SeePosition::piece_at(Square s) {
    return PieceID_to_Piece(piece_id_at(s));
}

bool SeePosition::is_enemy(Piece p) {
    if (side_to_move() == WHITE) {
        return Piece_is_black(p);
    }
    else {
        return Piece_is_white(p);
    }
}

bool SeePosition::side_to_move() {
    return (move_count % 2 == 0 ? WHITE : BLACK);
}

//TODO: move this to some object ListOfMoves
// as well as other functions that access the list
void SeePosition::add_move(Square from) {
    immediately_attacking_squares[move_index] = from;
    move_index++;
}

