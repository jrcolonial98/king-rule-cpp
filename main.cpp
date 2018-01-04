//
//  main.cpp
//  KingRule Cpp
//
//  Created by Brendan Gregory on 5/21/17.
//  Copyright © 2017 Brendan Gregory. All rights reserved.
//

#include "game.hpp"

int main(int argc, const char * argv[]) {
    using namespace std;
    
    cout << "KingRule C++ Indev\nLoading...\n";
    
    pt_pos_init();
    pt_pawn_init();
    
    /*
    if (argc != 3
        || (*argv[1] != 'h' && *argv[1] != 'c')
        || (*argv[2] != 'h' && *argv[2] != 'c')) {
        cout << "Two args, one for white, one for black. 'h' for human, 'c' for computer.\n";
        return 1;
    }
     */
    char white, black;
    cout << "White is? (h for human, c for computer)\n";
    cin >> white;
    cout << "Black is? (h for human, c for computer)\n";
    cin >> black;
    
    
    //Game g(*argv[0] == 'h', *argv[1] == 'h');
    Game g(white == 'h', black == 'h');
    g.start();
    
    return 0;
}
