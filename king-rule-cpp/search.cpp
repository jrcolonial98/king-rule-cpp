//
//  search.cpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 7/5/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#include "search.hpp"
#include <vector>
#include <pthread/pthread.h>

bool should_stop_searching = false;

Search::Search(Position *game_pos, int num_threads) {
    this->game_pos = game_pos;
    this->num_threads = num_threads;
}
void Search::start() {
    should_stop_searching = false;
    
    t = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
    running = true;
    
    //std::vector<std::thread> tv{};
    
    
    for (int i = 0; i < num_threads; i++) {
        std::cout<<"create\n";
        //std::thread t = std::thread(search_master);
        //tv.push_back(search_master);
        
        pthread_create(t + i, NULL, search_master, (void*)game_pos);
        //int ret = pthread_create(&t[i], NULL, search_master, NULL);
        //t[i].join();
    }
}
void Search::stop() {
    should_stop_searching = true;
    join_all();
    running = false;
    print_data();
}
Move Search::get_pv_move() {
    return game_pos->get_pv_move();
}
void Search::join_all() {
    for (int i = 0; i < num_threads; i++) {
        pthread_join(t[i], NULL);
    }
}
void Search::print_data() {
    std::cout << "Principal Variation: \n";
    Position *p = game_pos;
    int eval_depth = 0;
    while (p != NULL) {
        Move_to_string(p->get_pv_move());
        eval_depth++;
        p = p->get_next();
    }
    std::cout << "\nSearch depth: ";
}

//function that threads run
void* search_master(void* data) {
    Position* pdata = (Position*)data;
    Position *root = (Position*)malloc(sizeof(Position));
    
    *root = Position(*pdata);
    
    const int DEPTH = 5; //temporary
    search(*root, EVAL_MIN, EVAL_MAX, DEPTH, 1);
    
    Move pv_move = root->get_pv_move();
    pdata->set_pv_move(pv_move);
    
    return NULL;
}

Eval search_leaf(Position &r, Eval alpha, Eval beta) { //r is root
    if (should_stop_searching) return alpha;
    Eval baseline = Evaluate::evaluate(r);
    
    //better way of doing this: find all squares that can be captured, and do see() once per square
    //in Position::find_moves(), update a list of squares (64-bit int, one bit per square) that can be taken
    Square s;
    Piece piece;
    
    Move *move = r.get_next_move();
    //WE GET STUCK IN THIS LOOP
    while (move != NULL) {
        s = Move_to(*move);
        piece = r.piece_at(s);
        if (piece != EMPTY && !Piece_is_king(piece)) { //if move is valid capture
            
            //if move is interesting enough (i.e., if down a rook, don't look at pawn captures)
            if ((baseline - Piece_value(piece)) + EVAL_INTERESTING >= alpha) {
                SeePosition sp = SeePosition(r, s);
                Eval eval = baseline + see(sp, s) * (r.side_to_move() ? 1 : -1);
                //TODO: above line does not take into account material taken by Move move
                
                if (eval >= beta) return beta;
                if (eval > alpha) alpha = eval;
            }
        }
        std::cout<<"again";
        move = r.get_next_move();
    }
    int k = 0;
    int l = 4 / k;
    //WE GET STUCK IN THAT LOOP
    
    return alpha;
}
Eval search(Position &r, Eval alpha, Eval beta, int depth, int current_depth) {
    if (should_stop_searching) return alpha;
    //TT data
    Move hash_move = MOVE_NONE;
    //PosData pd;
    PosData pd;
    if (!pt_pos_get(&pd, r.get_pos_hash())) {
        int pdepth = pd.eval_depth;
        char node_type = pd.node_type;
        
        if (pdepth >= depth && node_type == EVAL_EXACT) {
            r.set_pv_move(pd.best_or_refutation_move); //TODO: make pv_move not a pointer
            return pd.eval;
        }
        hash_move = pd.best_or_refutation_move;
        current_depth = pdepth + 1;
    }
    else { //create new data
        
    }
    
    //ID search
    int d;
    for (d = current_depth; d < depth; d++) {
        r.reset_move_iterator();
        Move *best_move = r.get_next_move();
        while (best_move != NULL) {
            r.update_next(*best_move);
            Position* child = r.get_next();
            //child->update(*best_move);
            Eval eval = (d <= 1 ?
                         -search_leaf(*child, -beta, -alpha) :
                         -search(*child, -beta, -alpha, d, 1));
            if (eval >= beta) { //beta cutoff
                r.set_pv_move(*best_move);
                child->unmake_move();
                break;
            }
            if (eval > alpha) { //alpha raiser
                alpha = eval;
                r.set_pv_move(*best_move);
            }
            child->unmake_move();
        }
    }
    
    //max depth search
    d = depth;
    r.reset_move_iterator();
    Move *best_move = r.get_next_move();
    bool alpha_raised = false;
    while (best_move != NULL) {
        //update child
        r.update_next(*best_move);
        Position* child = r.get_next();
        
        Eval eval = (d - 1 == 1 ?
                     -search_leaf(*child, -beta, -alpha) :
                     -search(*child, -beta, -alpha, d - 1, 1));
        
        if (eval >= beta) {
            r.set_pv_move(*best_move);
            
            //TT data
            pd.eval = beta;
            pd.eval_depth = depth;
            pd.best_or_refutation_move = r.get_pv_move();
            pd.node_type = EVAL_LOWER;
            pt_pos_insert(r.get_pos_hash(), &pd);
            
            child->unmake_move();
            
            return eval;
        }
        if (eval > alpha) {
            alpha = eval;
            r.set_pv_move(*best_move);
            alpha_raised = true;
        }
        
        child->unmake_move();
        best_move = r.get_next_move();
    }
    
    //TT data
    pd.eval = alpha;
    pd.eval_depth = depth;
    pd.best_or_refutation_move = r.get_pv_move();
    if (alpha_raised) pd.node_type = EVAL_EXACT;
    else pd.node_type = EVAL_UPPER; //this is an all-node, failed low
        //will become beta cutoff in parent node
    pt_pos_insert(r.get_pos_hash(), &pd);
    
    return alpha;
}



//killer moves can only be returned by beta


Eval see(SeePosition sp, Square s) {
    Move m = sp.get_smallest_attacker_move();
    
    if (m != 0) {
        Piece piece_taken = sp.piece_at(Move_to(m));
        Eval value_captured = abs(Piece_value(piece_taken));
        
        //make capture, update board
        sp.update(m);
        
        //you can either make a capture or do nothing, depending upon favorability of capture
        Eval eval = std::max(0, value_captured - see(sp, s));
        
        //sp.unmake_move();
        
        return eval;
    }
    else { //no pieces left to capture with
        return 0;
    }
}
