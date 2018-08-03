//
//  evaluate.cpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 7/9/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#include "evaluate.hpp"

using namespace Evaluate;
using namespace PTable;

Eval Evaluate::evaluate(Position &p) {
        
    Eval eval;
    
    eval = p.material_imbalance; //get/set
    
    //load up king shield and files
    
    //look up pawn structure
    PawnData pd;

    //entry either is empty or contains a different pos
    if (!pt_pawn_get(&pd, p.get_pawn_hash())) {
        unsigned int backwards_squares[2] = {4294967295, 4294967295};
        //one int per color, one bit per square, ranks 3-6, initialized to "all squares are backwards"
        char files_occupied[2] = {0, 0};
        //one char per side, one bit per file, initialized to "all files not occupied"
        //bits go in reverse order like hgfedcba
        
        Eval eval = 0;

        for (bool c : {true, false}) {
            FileData* fo = (c ? files_occupied : files_occupied + 1);
            SquareData* bs = (c ? backwards_squares : backwards_squares + 1);
            
            int island_count = 0, isolated_count = 0;
            int current_island_size = 0;
            int index = 0;
            
            int start = (c ? TWO : SEVEN);
            int end = (c ? SEVEN : TWO);
            int backwards_end = (c ? SIX : THREE);
            int increment = (c ? 1 : -1);
            
            Piece friendly_pawn = (c ? WPAWN : BPAWN);
            
            //for file
            for (int file = A; file <= H; file++) {
                
                //for rank
                int pawns_found = 0;
                for (int rank = start; rank != end; rank += increment) {
                    Square s = Square_new(file, rank);
                    
                    //if pawn found
                    if (PieceID_to_Piece(p.piece_id_at(s)) == friendly_pawn) {
                        
                        pawns_found++;
                        
                        //FIRST PAWN FOUND
                        if (pawns_found == 1) {
                            //pawn structure data
                            if (get_file_occupied(fo, file - 1)) {
                                current_island_size++;
                            }
                            else {
                                island_count++;
                                
                                if (current_island_size == 1) {
                                    isolated_count++;
                                }
                                current_island_size = 1;
                            }
        
                            //file occupied data
                            set_file_occupied(fo, file, true);
                            
                            //backwards square data
                            for (int r = rank + 1; r < backwards_end; r++) {
                                s = (Square)(s + increment * 8);
                                set_square_backwards(bs, s, false);
                                if (file != A) {
                                    set_square_backwards(bs, (Square)(s + 1), false);
                                }
                                if (file != H) {
                                    set_square_backwards(bs, (Square)(s - 1), false);
                                }
                            }
                        }
                    }
                }
                //TODO: apply penalty for doubled, tripled, etc
            }
            //TODO: apply penalties for island count, isolated
        }
        
        PawnData pd_new;
        
        pd.files_occupied[0] = files_occupied[0];
        pd.files_occupied[1] = files_occupied[1];
        pd.backwards_squares[0] = backwards_squares[0];
        pd.backwards_squares[1] = backwards_squares[1];
        pd.eval = eval; //this must be dependent on pawn structure, not "eval"
        pd.hash = p.get_pawn_hash();
        
        pt_pawn_insert(p.get_pawn_hash(), &pd_new);
    }
    
    eval += pd.eval;
    
    
    //GO THRU PIECES, USING PD.BACKWARDS_SQUARES AND PD.FILES_OCCUPIED
    char* fo_friendly = pd.files_occupied;
    char* fo_enemy = pd.files_occupied + 1;
    unsigned int* bs_friendly = pd.backwards_squares;
    unsigned int* bs_enemy = pd.backwards_squares + 1;
    int increment = 1;
    for (int j = 0; j < 32; j++) {
        if (j == 16) { //switch to black data
            fo_friendly = pd.files_occupied + 1;
            fo_enemy = pd.files_occupied;
            bs_friendly = pd.backwards_squares + 1;
            bs_enemy = pd.backwards_squares;
            increment = -1;
        }
        
        PieceID piece_id = (PieceID)j;
        Piece piece = PieceID_to_Piece(piece_id);
        Square square = p.location_of(piece_id);
        
        if (Piece_is_king(piece) || square == NOWHERE) continue;
        
        //TODO: move this pawn section into the pawn structure code
        //so it isn't run a million times
        if (Piece_is_pawn(piece)) {
            if (get_square_backwards(bs_enemy, square)) {
                apply_bonus(&eval, BONUS_PAWN_PASSED);
            }
            else if (get_square_backwards(bs_friendly, (Square)(square + 8 * increment))) {
                apply_bonus(&eval, BONUS_PAWN_BACKWARDS);
            }
            //TODO: if covering a knight, it's an outpost
        }
        else if (Piece_is_queen(piece)) {
            //TODO: if it moves before move_count is 17, penalty
        }
        else if (Piece_is_rook(piece)) {
            if (!get_file_occupied(fo_friendly, Square_x(square))) {
                //file is either open or half-open
                if (!get_file_occupied(fo_enemy, Square_x(square))) {
                    apply_bonus(&eval, BONUS_ROOK_OPENFILE);
                }
                else {
                    apply_bonus(&eval, BONUS_ROOK_HALFOPENFILE);
                }
            }
        }
        else if (Piece_is_knight(piece)) {
            apply_bonus(&eval, KNIGHT_MOBILITY_TABLE[square]);
            //TODO: outpost
            //TODO: blockade
        }
        else if (Piece_is_bishop(piece)) {
            //bishop pair is in Position
        }
        
        //FOR ALL PIECES
        //TODO: mobility, influence, etc
        //if (touching a square in the king's shield) apply bonus
        
    }
    
    if (p.side_to_move()) return eval;
    else return -eval;
}

inline void Evaluate::apply_bonus(Eval* eval, Eval bonus) {
    *eval += bonus;
}

inline void Evaluate::set_file_occupied
(char* data, int file, bool occupied) {
    if (occupied) {
        *data |= OCCUPIED_FILES[file];
    }
    else {
        *data ^= OCCUPIED_FILES[file];
    }
}
inline void Evaluate::set_square_backwards
(unsigned int* data, Square square, bool backwards) {
    int s = square - 16; //realign it since a3 is now 0 not 16
    
    if (backwards) {
        *data |= BACKWARDS_SQUARES[s];
    }
    else {
        *data ^= BACKWARDS_SQUARES[s];
    }
}

inline bool Evaluate::get_file_occupied(char* data, int file) {
    if (file < A || file > H) return false;
    return *data & OCCUPIED_FILES[file];
}
inline  bool Evaluate::get_square_backwards
(unsigned int* data, Square square) {
    if (square < A1 || square > H4) return false;
    return *data & BACKWARDS_SQUARES[square - 16];
}
