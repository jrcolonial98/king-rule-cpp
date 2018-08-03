//
//  ptable.cpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 7/8/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#include "ptable.hpp"

using namespace PTable;
using namespace std;

std::atomic<PosData>* PTable::pos_table;
std::atomic<PawnData>* PTable::pawn_table;
//std::mutex PTable::pos_mtx[PT_POS_SIZE];
//std::mutex PTable::pawn_mtx[PT_PAWN_SIZE];

void PTable::init() {
    pt_pos_init();
    pt_pawn_init();
}

void PTable::pt_pos_init() {
    pos_table = (atomic<PosData>*)malloc(sizeof(atomic<PosData>) * PT_POS_SIZE);
}
void PTable::pt_pos_insert(Hash hash, PosData *data) {
    int address = pt_pos_hash_to_address(hash);
    
    //pos_table[address].mtx.lock();
    //pos_mtx[address].lock();
    //std::memcpy(pos_table + address, data, sizeof(PosData));
    pos_table[address].store(*data);
    //pos_mtx[address].unlock();
    //pos_table[address].mtx.unlock();
    //because PosData has a mtx in it, you can't copy using '=' operator
}

bool PTable::pt_pos_get(PosData *pd, Hash hash) {
    int address = pt_pos_hash_to_address(hash);
    //PosData *data = &(pos_table[address]);
    PosData data = pos_table[address].load();
    if (pd->hash != hash) {
        return false;
    }
    //data->mtx.lock();
    //pos_mtx[address].lock();
    //std::memcpy(pd, data, sizeof(PosData));
    //pos_mtx[address].unlock();
    //data->mtx.unlock();
    return true;
}
int PTable::pt_pos_hash_to_address(Hash hash) {
    return (hash % PT_POS_SIZE);
}

void PTable::pt_pawn_init() {
    pawn_table = (atomic<PawnData>*)malloc(sizeof(atomic<PawnData>) * PT_PAWN_SIZE);
}
void PTable::pt_pawn_insert(Hash hash, PawnData *data) {
    int address = pt_pawn_hash_to_address(hash);
    //pawn_table[address].mtx.lock();
    //pawn_mtx[address].lock();
    //std::memcpy(pawn_table + address, data, sizeof(PawnData));
    pawn_table[address].store(*data);
    //pawn_mtx[address].unlock();
    //pawn_table[address].mtx.unlock();
}

bool PTable::pt_pawn_get(PawnData *pd, Hash hash) {
    int address = pt_pawn_hash_to_address(hash);
    //PawnData *data = &(pawn_table[address]);
    PawnData data = pawn_table[address].load();
    if (pd->hash != hash) {
        return NULL;
    }
    //data->mtx.lock();
    //pawn_mtx[address].lock();
    //std::memcpy(pd, data, sizeof(PawnData));
    //pawn_mtx[address].unlock();
    //data->mtx.unlock();
    return pd;
}
int PTable::pt_pawn_hash_to_address(Hash hash) {
    return (hash % PT_PAWN_SIZE);
}
void PTable::pt_pawn_reset() {
    free(pawn_table);
    pt_pawn_init();
}
void PTable::pt_pos_reset() {
    free(pos_table);
    pt_pos_init();
}
