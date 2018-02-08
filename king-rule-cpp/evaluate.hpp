//
//  evaluate.hpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 7/9/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#ifndef evaluate_h
#define evaluate_h

#include "types.hpp"
#include "position.hpp"

Eval evaluate(Position &p); //returns relative to side to move

const Eval
    BONUS_PAWN_PASSED = 0,
    BONUS_PAWN_DOUBLED = 0,
    BONUS_PAWN_BACKWARDS = 0,
    BONUS_ROOK_OPENFILE = 0,
    BONUS_ROOK_HALFOPENFILE = 0,
    BONUS_ROOK_CONNECTED = 0;

const int KNIGHT_MOBILITY_TABLE[64] =  {
    -30, -20, -5, -5, -5, -5, -20, -30,
    -20, -5,   5,  5,  5,  5, -5,  -20,
    -5,   5,   10, 10, 10, 10, 5,  -5,
    -5,   5,   10, 10, 10, 10, 5,  -5,
    -5,   5,   10, 10, 10, 10, 5,  -5,
    -5,   5,   10, 10, 10, 10, 5,  -5,
    -20,  -5,  5,  5,  5,  5, -5,  -20,
    -30, -20, -5,  5,  5,  5, -20, -30
};

const unsigned char OCCUPIED_FILES[8] = {
    1,   // 00000001 // A file occupied
    2,   // 00000010 // B file occupied
    4,   // 00000100 // C file occupied
    8,   // 00001000 // D file occupied
    16,  // 00010000 // E file occupied
    32,  // 00100000 // F file occupied
    64,  // 01000000 // G file occupied
    128  // 10000000 // H file occupied
};

const unsigned int BACKWARDS_SQUARES[32] = {
    //each represents that square being occupied
    //a3, b3, c3, ... , h3
    //a4, b4, c4, ... , h4
    //...
    //a6, b6, c6, ... , h6
    1, 2, 4, 8, 16, 32, 64, 128,
    256, 512, 1024, 2048, 4096, 8192, 16384, 32768,
    65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608,
    16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648
};

typedef char FileData; //one bit per file
typedef unsigned int SquareData; //one bit per square, ranks 3-6 only

inline void apply_bonus(Eval* eval, Eval bonus);

inline void set_file_occupied(FileData* data, int file, bool occupied);
inline void set_square_backwards(SquareData* data, Square square, bool backwards);
inline bool get_file_occupied(FileData* data, int file);
inline bool get_square_backwards(SquareData* data, Square square);


#endif /* evaluate_hpp */
