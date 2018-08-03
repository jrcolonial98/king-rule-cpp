//
//  threads.h
//  king-rule-cpp
//
//  Created by Brendan Gregory on 3/5/18.
//  Copyright © 2018 Brendan Gregory. All rights reserved.
//

#ifndef threads_h
#define threads_h

#include <thread>
#include "position.hpp"

using namespace std;

namespace Threads {
    
void init();

struct SearchData {
    Move pv_move;
    Eval eval;
};

class Search {
public:
    Search(Position &p);
    void start();
private:
    thread t;
    SearchData data;
};

class InputThread {
public:
    InputThread();
    void start();
};
    
} //namespace Threads

#endif /* threads_h */
