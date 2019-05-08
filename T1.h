//
// Created by kostas on 4/21/19.
//

#ifndef QTD_T1_H
#define QTD_T1_H
#include <vector>
using namespace std;

struct T1node{

    int id;
    T1node *parent;
    vector<T1node*> children;

    /*
     *
     * book keeping necessary for finding shared triplets
     *
     */

    int numLeaves;
    ulli white, red, blue, green, redI, blueI, greenI, redL, blueL, greenL; //see Algorithm 4 in the paper for definition
    ulli redblue, redgreen, bluegreen, redbluegreen;

    T1node (){
        numLeaves = 0;
    }

};

T1node *T1;
int T1numLeafs;
int T1size;

#endif //QTD_T1_H
