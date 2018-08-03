//
//  search.hpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 7/5/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#ifndef search_hpp
#define search_hpp

#include "position.hpp"
#include "types.hpp"
#include "evaluate.hpp"
#include "ptable.hpp"
#include <pthread.h>

using namespace Evaluate;
using namespace PTable;

extern bool should_stop_searching;

extern Position *ps;
    //temporary, cannot create thread with function with parameter
    //so search_master has no parameter

class Search {
public:
    Search(Position *game_pos, int num_threads);
    void start();
    void stop();
    Move get_pv_move();
private:
    void print_data(); //temporary
    void join_all();
    bool running = false;
    Position* ps; 
    Position* game_pos; //pointer to game's official position
    int num_threads;
    pthread_t *t; //array of threads
    Position *root;
};

static void* search_master(void* data);

Eval search(Position &r, Eval alpha, Eval beta, int depth, int current_depth);
Eval search_leaf(Position &r, Eval alpha, Eval beta);

//see() looks at captures only and is used for the horizon effect
//returns absolute Eval (not relative to side to move)
Eval see(SeePosition sp, Square s);

#endif
