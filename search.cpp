//
//  search.cpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 7/5/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#include "search.hpp"

Move search(Position p, Eval alpha, Eval beta, int depth) {
    Eval eval = 0;
    
    //get TT data
    Move hash_move = 0;
    PosData *pd = pt_pos_get(p.get_pos_hash());
    if (pd != NULL) {
        //if pos has already been searched to sufficient depth, we're done
        if (pd->eval_depth >= depth && pd->node_type == EVAL_EXACT) {
            return pd->best_or_refutation_move;
        }
        
        //otherwise, grab previously calculated hash move
        hash_move = pd->best_or_refutation_move;
    }
    
    //if depth = 0, go to alphabeta_leaf
    
    //alphabeta search at every depth
    for (int i = 1; i < depth + 1; i++) {
        eval = alphabeta(p, pd, alpha, beta, hash_move, i);
        
        //reset move scores
        for (int j = 0; j < p.move_index; j++) {
            Move m = p.moves[j];
            Score score = Move_score(m);
            score = (Score)((int)score * -1);
            Move_set_score(&m, score);
            p.moves[j] = m;
            
            if (m == p.get_pv_move()) {
                Move_set_score(&(p.moves[j]), SC_PV); //set pv move to PV score?
            }
        }
        
        //aspiration window
        alpha = eval - 40;
        beta = eval + 40;
    }
    
    //put new TT data in
    pd->eval = eval;
    pd->eval_depth = depth;
    pd->hash = p.get_pos_hash();
    pd->best_or_refutation_move = p.get_pv_move();
    
    pt_pos_insert(p.get_pos_hash(), pd);
    
    return p.get_pv_move();
}

Eval alphabeta(Position p, PosData *pd, Eval alpha, Eval beta, Move hash_move, int depth) {
    std::cout << "alphabeta\n";
    if (depth == 0) return alphabeta_leaf(p, alpha, beta);
    
    Eval original_alpha = alpha;
    
    Eval best_eval = EVAL_MIN;
    
    while (true) {
        int best_score = SC_MINIMUM;
        Move *best_move = NULL;
        
        //find best move
        for (int i = 0; i < p.move_index; i++) {
            Move *move = &(p.moves[i]);
            if (Move_compare(*move, hash_move) && Move_score(*move) >= 0) {
                Move_set_score(move, SC_HASH);
            }
            
            if (Move_score(*move) > best_score) {
                best_move = move;
                best_score = Move_score(*move);
            }
        }
        if (best_score == SC_MINIMUM) break; //no moves left
        
        //update position with best move and evaluate it
        Position pos = Position(p);
        pos.update(*best_move); //TODO: this function should take a pointer
        Eval eval = -alphabeta(pos, pd, -beta, -alpha, hash_move, depth);
        
        //beta cutoff - move is so good that you should stop considering this pos
        if (eval >= beta) {
            //if (move.is_not_a_capture)
                //TODO: add move to parent's list of killer moves
            
            p.set_pv_move(*best_move);
            pd->eval = beta;
            pd->best_or_refutation_move = *best_move;
            pd->eval_depth = depth;
            pd->node_type = EVAL_LOWER; //not necessarily the best move, but enough for a cutoff
            
            return eval;
        }
        
        //alpha raiser - set alpha to best move eval
        if (eval > best_eval) {
            p.set_pv_move(*best_move);
            best_eval = eval;
            if (eval > alpha) {
                alpha = eval;
            }
        }
        
        Move_set_score(best_move, (Score)(best_score * -1));
    }
    
    pd->eval = best_eval;
    pd->best_or_refutation_move = p.get_pv_move();
    pd->eval_depth = depth;
    if (alpha == original_alpha) { //alpha was never raised
        pd->node_type = EVAL_UPPER; //the node is an all-node, and failed low
        pd->eval = alpha;
        //this will become a beta cutoff in the parent node
    }
    else {
        pd->node_type = EVAL_EXACT; //eval is in between original alpha and beta
        pd->eval = best_eval;
    }
    
    return alpha;
}

/*
 PV move only matters for root node. Otherwise will never be used.
 Same with hash.
 If parameters are -max & max there can be no cutoff
 Killer moves can only be returned by beta.
 Iterative deepening should be on every node
 */

Eval alphabeta_leaf(Position p, Eval alpha, Eval beta) {
    std::cout << "alphabeta_leaf\n";
    Eval baseline = evaluate(p);
    
    //better way of doing this: find all squares that can be captured, and do see() once per square
    //in Position::find_moves(), update a list of squares (64-bit int, one bit per square) that can be taken
    for (int i = 0; i < p.move_index; i++) {
        Move move = p.moves[i];
        if (true) { //TODO: if move is capture, not just true
            Square s = Move_to(move);
            Piece piece = p.piece_at(s);
            
            //if move is interesting enough (i.e., if down a rook, don't look at pawn captures)
            if ((baseline - Piece_value(piece)) + EVAL_INTERESTING >= alpha) {
                Position pos = Position(p);
                pos.update(move);
                SeePosition sp = SeePosition(pos, s);
                Eval score = baseline + see(sp, s) * (p.side_to_move() ? 1 : -1);
                
                if (score >= beta) return beta;
                if (score > alpha) alpha = score;
            }
        }
    }
    
    return alpha;
}

Eval see(SeePosition sp, Square s) {
    std::cout << "see\n";
    Eval value = 0;
    
    sp.find_moves();
    Move m = sp.get_smallest_attacker_move();
    
    Piece piece = sp.piece_at(Move_to(m));
    
    if (piece != EMPTY) {
        Eval value_just_captured = abs(Piece_value(piece));
        
        //make capture, update board
        sp.update(m);
        
        value = std::max(0, value_just_captured - see(sp, s));
    }
    
    return value;
}
