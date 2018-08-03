//
//  main.cpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 5/21/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#include "game.hpp"
#include "ptable.hpp"

int main(int argc, const char * argv[]) {
    using namespace std;
    
    cout << "KingRule C++ Indev\nLoading...\n";
    
    PTable::init(); //init pawn_table and pos_table
    hash_values_init(); //init random ints used for hashes
    
    char white, black;
    cout << "White is? (h for human, c for computer)\n";
    cin >> white;
    cout << "Black is? (h for human, c for computer)\n";
    cin >> black;
    
    
    Game g(white == 'h', black == 'h');
    g.play();
    
    return 0;
}
