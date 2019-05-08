//
// Created by kostas on 4/21/19.
//

#ifndef QTD_T2_H
#define QTD_T2_H
#include <vector>
using namespace std;

struct T2node{
    int id;
    int minDFSid, maxDFSid;
    vector<T2node*> children;
};

T2node *T2;
int T2numLeafs;
int T2size;

#endif //QTD_T2_H
