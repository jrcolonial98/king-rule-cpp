//
//  types.h
//  KingRule Cpp
//
//  Created by Brendan Gregory on 5/22/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#ifndef types_h
#define types_h

#include <stdint.h>
#include <iostream>

/*
 
TABLE OF CONTENTS
 
1. Misc info in comment form
2. Eval
3. Color
4. Piece
5. Square
6. Move/Score
7. Hash
 
*/
 

// -----------------------------------------------------------


/*
 Powers of 2:
 Bit 1   2^0   1
 Bit 2   2^1   2
 Bit 3   2^2   4
 Bit 4   2^3   8
 Bit 5   2^4   16
 Bit 6   2^5   32
 Bit 7   2^6   64
 Bit 8   2^7   128
 Bit 9   2^8   256
 Bit 10  2^9   512
 Bit 11  2^10  1024
 Bit 12  2^11  2048
 Bit 13  2^12  4096
 Bit 14  2^13  8192
 Bit 15  2^14  16384
 Bit 16  2^15  32768
 Bit 17  2^16  65536
 Bit 18  2^17  131072
 Bit 19  2^18  262144
 Bit 20  2^19  524288
 Bit 21  2^20  1048576
 Bit 22  2^21  2097152
 Bit 23  2^22  4194304
 Bit 24  2^23  8388608
 Bit 25  2^24  16777216
 Bit 26  2^25  33554432
 Bit 27  2^26  67108864
 Bit 28  2^27  134217728
 Bit 29  2^28  268435456
 Bit 30  2^29  536870912
 Bit 31  2^30  1073741824
 Bit 32  2^31  2147483648
*/


typedef short Eval;
const Eval EVAL_MIN = -32768;
const Eval EVAL_MAX = 32767;
const Eval EVAL_INTERESTING = 200; //used for quiescence
inline float Eval_to_decimal(Eval eval) {
    return ((float)eval) / 100;
}


typedef bool Color;
const Color WHITE = true, BLACK = false;


enum Piece : char {
    WPAWN = 0, WBISHOP, WKNIGHT, WROOK, WQUEEN, WKING,
    BPAWN, BBISHOP, BKNIGHT, BROOK, BQUEEN, BKING,
    EMPTY
};
const Eval PIECE_VALUES[13] = {
    100, 300, 300, 500, 900, 0,
    -100, -300, -300, -500, -900, 0,
    0
};
inline Eval Piece_value(Piece p) {
    return PIECE_VALUES[p];
}
inline bool Piece_is_pawn(Piece p) {
    return (p == WPAWN || p == BPAWN);
}
inline bool Piece_is_bishop(Piece p) {
    return (p == WBISHOP || p == BBISHOP);
}
inline bool Piece_is_knight(Piece p) {
    return (p == WKNIGHT || p == BKNIGHT);
}
inline bool Piece_is_rook(Piece p) {
    return (p == WROOK || p == BROOK);
}
inline bool Piece_is_queen(Piece p) {
    return (p == WQUEEN || p == BQUEEN);
}
inline bool Piece_is_king(Piece p) {
    return (p == WKING || p == BKING);
}
inline bool Piece_is_white(Piece p) {
    return (p < BPAWN);
}
inline bool Piece_is_black(Piece p) {
    return (p > WKING && p != EMPTY);
}
enum PieceID : char {
    WK = 0, WQ, WKR, WQR, WKN, WQN, WKB, WQB,
    WAP, WBP, WCP, WDP, WEP, WFP, WGP, WHP,
    BK, BQ, BKR, BQR, BKN, BQN, BKB, BQB,
    BAP, BBP, BCP, BDP, BEP, BFP, BGP, BHP,
    EMPTYID
};
inline bool PieceID_is_white(PieceID pid) {
    return pid < BK;
}
inline bool PieceID_is_black(PieceID pid) {
    return pid > WHP && pid < EMPTYID;
}
inline Piece PieceID_to_Piece(PieceID pid) {
    if (pid == WK) return WKING;
    else if (pid == WQ) return WQUEEN;
    else if (pid == WKR || pid == WQR) return WROOK;
    else if (pid == WKN || pid == WQN) return WKNIGHT;
    else if (pid == WKB || pid == WQB) return WBISHOP;
    else if (pid < BK) return WPAWN;
    else if (pid == BK) return BKING;
    else if (pid == BQ) return BQUEEN;
    else if (pid == BKR || pid == BQR) return BROOK;
    else if (pid == BKN || pid == BQN) return BKNIGHT;
    else if (pid == BKB || pid == BQB) return BBISHOP;
    else if (pid < EMPTYID) return BPAWN;
    else return EMPTY;
}


typedef char Square;
const Square
    A1 = 0, B1 = 1, C1 = 2, D1 = 3, E1 = 4, F1 = 5, G1 = 6, H1 = 7,
    A2 = 8, B2 = 9, C2 = 10, D2 = 11, E2 = 12, F2 = 13, G2 = 14, H2 = 15,
    A3 = 16, B3 = 17, C3 = 18, D3 = 19, E3 = 20, F3 = 21, G3 = 22, H3 = 23,
    A4 = 24, B4 = 25, C4 = 26, D4 = 27, E4 = 28, F4 = 29, G4 = 30, H4 = 31,
    A5 = 32, B5 = 33, C5 = 34, D5 = 35, E5 = 36, F5 = 37, G5 = 38, H5 = 39,
    A6 = 40, B6 = 41, C6 = 42, D6 = 43, E6 = 44, F6 = 45, G6 = 46, H6 = 47,
    A7 = 48, B7 = 49, C7 = 50, D7 = 51, E7 = 52, F7 = 53, G7 = 54, H7 = 55,
    A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63,
    NOWHERE = 64;
/*
const char* SQUARE_TO_STRING[64] = {
    "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8",
    "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8",
    "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8",
    "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8",
    "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8",
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8",
    "G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8",
    "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8",
};
 */
const int
A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7,
ONE = 0, TWO = 1, THREE = 2, FOUR = 3, FIVE = 4, SIX = 5, SEVEN = 6, EIGHT = 7, NINE = 8;
inline Square Square_new(int x, int y) { return (Square)(y * 8 + x); }
inline int Square_x(Square s) { return s % 8; }
inline int Square_y(Square s) { return s / 8; }
inline bool Square_is_valid(Square s) { return (s >= A1 && s <= H8); }


typedef unsigned int Move;
// bit organization:
// TODO: Move_is_castle() function
//   and Move_is_capture()
// [25: checked off (used for search)
// [24: includes claim]
// [23: includes offer]
// [22: is en passant]
// [21: is promotion]
// [17-20: score (move ordering)]
// [13-16: piece]
// [7-12: to square]
// [1-6: from square]
const Move MOVE_CLEAR_SCORE = 32571391; // 1111100001111111111111111
const Move MOVE_CLEAR_PIECE = 33492991; // 1111111110000111111111111
enum Score {
    SC_MINIMUM = 0, SC_NEG_CAPTURE, SC_NON_KILLER, SC_KILLER, SC_EQUAL_CAPTURE,
    SC_POS_CAPTURE, SC_HASH, SC_PV
    //THIS IS TOTALLY WRONG. KILLER MOVES CAUSE BETA CUTOFFS IN SIBLING NODES
    //SO A POSITION MUST HOLD A COLLECTION OF KILLERS FROM ITS CHILDREND
    //usually 2-3 of them. so a node checks its parent for killers.
    //this can be done in Position.update()
};
inline Move Move_new(Square from, Square to) {
    return (Move)(from + (to << 6));
}
inline Square Move_from(Move m) {
    return (Square)(m % 64);
}
inline Square Move_to(Move m) {
    return (Square)((m >> 6) % 64);
}
inline Piece Move_piece(Move m) {
    return (Piece)((m >> 12) % 16);
}
inline void Move_set_piece(Move *m, Piece p) {
    int new_piece = (((int)p) << 12);
    *m &= MOVE_CLEAR_PIECE;
    *m |= new_piece;
}
inline Score Move_score(Move m) {
    return (Score)((m >> 16) % 16);
}
inline void Move_set_score(Move *m, Score s) {
    int new_score = (((int)s) << 16);
    *m &= MOVE_CLEAR_SCORE;
    *m |= new_score;
}
inline bool Move_is_promotion(Move m) {
    return (m & 1048576);
}
inline void Move_set_is_promotion(Move *m, bool promotion) {
    if (promotion) *m |= 1048576;
    else           *m &= (~(1048576));
}
inline bool Move_is_en_passant(Move m) {
    return (m & 2097152);
}
inline void Move_set_is_en_passant(Move *m, bool ep) {
    if (ep) *m |= 2097152;
    else    *m &= (~(2097152));
}
inline bool Move_includes_offer(Move m) {
    return (m & 4194304);
}
inline void Move_set_includes_offer(Move *m, bool offer) {
    if (offer) *m |= 4194304;
    else       *m &= (~(4194304));
}
inline bool Move_includes_claim(Move m) {
    return (m & 8388608);
}
inline void Move_set_includes_claim(Move *m, bool claim) {
    if (claim) *m |= 8388608;
    else       *m &= (~(8388608));
}
inline bool Move_checked(Move m) {
    return (m & 16777216);
}
inline void Move_set_checked(Move *m, bool checked) {
    if (checked) *m |= 16777216;
    else         *m &= (~(16777216));
}

inline bool Move_compare(Move m1, Move m2) {
    return (Move_from(m1) == Move_from(m2) && Move_to(m1) == Move_to(m2));
}

inline void Move_to_string(Move m) {
    std::cout << "Move from " << (int)Move_from(m) << " to " << (int)Move_to(m) << "\n";
}


typedef unsigned long long int Hash;


#endif // types_h
