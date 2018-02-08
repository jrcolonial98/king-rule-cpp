//
//  ptable.cpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 7/8/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#include "ptable.hpp"

Hash hash_values[781];

PawnData *pawn_table;
PosData *pos_table;

int INIT_POS_HASH;
int INIT_PAWN_HASH;

void pt_pos_init() {
    pos_table = (PosData*)malloc(sizeof(PosData) * PT_POS_SIZE);
}
void pt_pos_insert(Hash hash, PosData data) {
    int address = pt_pos_hash_to_address(hash);
    pos_table[address] = data;
}

PosData* pt_pos_get(Hash hash) {
    int address = pt_pos_hash_to_address(hash);
    PosData *pd = &(pos_table[address]);
    if (pd->hash != hash) return NULL;
    return pd;
}
int pt_pos_hash_to_address(Hash hash) {
    return (hash % PT_POS_SIZE);
}

void pt_pawn_init() {
    pawn_table = (PawnData*)malloc(sizeof(PawnData) * PT_PAWN_SIZE);
}
void pt_pawn_insert(Hash hash, PawnData data) {
    int address = pt_pawn_hash_to_address(hash);
    pawn_table[address] = data;
}

PawnData* pt_pawn_get(Hash hash) {
    int address = pt_pawn_hash_to_address(hash);
    PawnData *pd = &(pawn_table[address]);
    if (pd->hash != hash) return NULL;
    return pd;
}
int pt_pawn_hash_to_address(Hash hash) {
    return (hash % PT_PAWN_SIZE);
}
void pt_pawn_reset() {
    free(pawn_table);
    pt_pawn_init();
}

Hash pt_get_xor(Piece p, Square s) {
    if (p == EMPTY) return 0;
    int index = p * 64 + s;
    return hash_values[index];
}

void pt_hash_values_init() {
    srand((int)time(NULL));
    for (int i = 0; i < 781; i++) {
        hash_values[i] = (Hash)rand();
    }
    
    INIT_POS_HASH = 0;
    INIT_POS_HASH ^= pt_get_xor(WROOK, A1);
    INIT_POS_HASH ^= pt_get_xor(WKNIGHT, B1);
    INIT_POS_HASH ^= pt_get_xor(WBISHOP, C1);
    INIT_POS_HASH ^= pt_get_xor(WQUEEN, C1);
    INIT_POS_HASH ^= pt_get_xor(WKING, E1);
    INIT_POS_HASH ^= pt_get_xor(WBISHOP, F1);
    INIT_POS_HASH ^= pt_get_xor(WKNIGHT, G1);
    INIT_POS_HASH ^= pt_get_xor(WROOK, H1);
    INIT_POS_HASH ^= pt_get_xor(WPAWN, A2);
    INIT_POS_HASH ^= pt_get_xor(WPAWN, B2);
    INIT_POS_HASH ^= pt_get_xor(WPAWN, C2);
    INIT_POS_HASH ^= pt_get_xor(WPAWN, D2);
    INIT_POS_HASH ^= pt_get_xor(WPAWN, E2);
    INIT_POS_HASH ^= pt_get_xor(WPAWN, F2);
    INIT_POS_HASH ^= pt_get_xor(WPAWN, G2);
    INIT_POS_HASH ^= pt_get_xor(WPAWN, H2);
    INIT_POS_HASH ^= pt_get_xor(BPAWN, A7);
    INIT_POS_HASH ^= pt_get_xor(BPAWN, B7);
    INIT_POS_HASH ^= pt_get_xor(BPAWN, C7);
    INIT_POS_HASH ^= pt_get_xor(BPAWN, D7);
    INIT_POS_HASH ^= pt_get_xor(BPAWN, E7);
    INIT_POS_HASH ^= pt_get_xor(BPAWN, F7);
    INIT_POS_HASH ^= pt_get_xor(BPAWN, G7);
    INIT_POS_HASH ^= pt_get_xor(BPAWN, H7);
    INIT_POS_HASH ^= pt_get_xor(BROOK, A8);
    INIT_POS_HASH ^= pt_get_xor(BKNIGHT, B8);
    INIT_POS_HASH ^= pt_get_xor(BBISHOP, C8);
    INIT_POS_HASH ^= pt_get_xor(BQUEEN, D8);
    INIT_POS_HASH ^= pt_get_xor(BKING, E8);
    INIT_POS_HASH ^= pt_get_xor(BBISHOP, F8);
    INIT_POS_HASH ^= pt_get_xor(BKNIGHT, G8);
    INIT_POS_HASH ^= pt_get_xor(BROOK, H8);
    
    INIT_PAWN_HASH = 0;
    INIT_PAWN_HASH ^= pt_get_xor(WPAWN, A2);
    INIT_PAWN_HASH ^= pt_get_xor(WPAWN, B2);
    INIT_PAWN_HASH ^= pt_get_xor(WPAWN, C2);
    INIT_PAWN_HASH ^= pt_get_xor(WPAWN, D2);
    INIT_PAWN_HASH ^= pt_get_xor(WPAWN, E2);
    INIT_PAWN_HASH ^= pt_get_xor(WPAWN, F2);
    INIT_PAWN_HASH ^= pt_get_xor(WPAWN, G2);
    INIT_PAWN_HASH ^= pt_get_xor(WPAWN, H2);
    INIT_PAWN_HASH ^= pt_get_xor(BPAWN, A7);
    INIT_PAWN_HASH ^= pt_get_xor(BPAWN, B7);
    INIT_PAWN_HASH ^= pt_get_xor(BPAWN, C7);
    INIT_PAWN_HASH ^= pt_get_xor(BPAWN, D7);
    INIT_PAWN_HASH ^= pt_get_xor(BPAWN, E7);
    INIT_PAWN_HASH ^= pt_get_xor(BPAWN, F7);
    INIT_PAWN_HASH ^= pt_get_xor(BPAWN, G7);
    INIT_PAWN_HASH ^= pt_get_xor(BPAWN, H7);
}
