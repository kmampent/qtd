//
// Created by kostas on 4/20/19.
//

#ifndef QTD_COMMON_H
#define QTD_COMMON_H
#include <vector>
#define BILLION  1000000000LL
#define ulli unsigned long long int
timespec startT, endT;
using namespace std;
void startTimer(){
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startT);
}

double endTimer(){
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endT);
    return endT.tv_sec * BILLION + endT.tv_nsec - (startT.tv_sec * BILLION + startT.tv_nsec);
}

vector<int> dfsIDs; //the array used to relabel T2
vector<vector<int>> C; //the C table
int q; //number of internal nodes in T1
int n; //number of leaves
int totalWhite;
struct color{
    int redMin, redMax;
    int blueMin, blueMax;
    int greenMin, greenMax;
};

color coloring;

ulli sharedTriplets;

#endif //QTD_COMMON_H
