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

void pt_hash_values_init() {
    srand((int)time(NULL));
    for (int i = 0; i < 781; i++) {
        hash_values[i] = (Hash)rand();
    }
}
