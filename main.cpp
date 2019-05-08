//
// Created by kostas on 4/20/19.
//

#include <iostream>
#include "newick_parser.h"
#include "triplet_dist.h"

using namespace std;

int main(int argc, char* argv[]) {

    startTimer();
    parseTrees(argv[1], argv[2]);
    preprocessing();
    cout<<"Triplet distance: "<<computeTripletDistance()<<endl;

    return 0;
}
