//
// Created by kostas on 4/21/19.
//

#ifndef QTD_TRIPLET_DIST_H
#define QTD_TRIPLET_DIST_H

#include "common.h"
#include "T1.h"
#include "T2.h"

void relabelT2(T2node* cur, int & curDFSid){
    if(cur->children.size() == 0){
        cur->minDFSid = cur->maxDFSid = curDFSid;
        dfsIDs[cur->id] = curDFSid;
        cur->id = curDFSid++;
        return;
    }
    for(T2node* child: cur->children){
        relabelT2(child, curDFSid);
    }
    cur->minDFSid = cur->children[0]->minDFSid;
    cur->maxDFSid = cur->children[cur->children.size()-1]->maxDFSid;
}

void prepareT1andC(){

    for(int i=0;i<T1size;i++){
        T1node* cur = &T1[i];
        if(cur->children.size() == 0){
            cur->id = dfsIDs[cur->id];
            int leafID = cur->id;
            cur = cur->parent;
            while(cur!= nullptr){
                cur->numLeaves++;
                C[cur->id - n - 1][leafID] = 1;
                cur = cur->parent;
            }
        }
        else {
            int lastLeaf = -1;
            for (int j = 0; j < cur->children.size(); j++) {
                T1node *child = cur->children[j];
                if (child->children.size() == 0) {
                    if (lastLeaf == -1) lastLeaf = j;
                }
                else{
                    if(lastLeaf!=-1)
                        swap(cur->children[lastLeaf++], cur->children[j]);
                }
            }
        }
    }

    for(int i=0;i<q;i++){
        for(int j=1;j<=n;j++){
            C[i][j] += C[i][j-1];
        }
    }

}

void preprocessing(){

    /*
     *
     * build T2 with the labels changed
     *
     */
    dfsIDs.resize(n+1);
    int curDFSid = 1;
    relabelT2(&T2[0], curDFSid);

    C.resize(q, vector<int>(n + 1, 0));
    prepareT1andC();

}

//given a coloring, find the shared triplets, algorithm 4 from the paper

void findSharedTripletsGivenAColoring(T1node * u){

    int uID = u->id - n - 1;
    if(u->children[0]->children.size() == 0){
        //cur has no children that are internal nodes
        u->red = C[uID][coloring.redMax] - C[uID][coloring.redMin-1];
        u->blue = C[uID][coloring.blueMax] - C[uID][coloring.blueMin-1];
        u->green = 0;
        if(coloring.greenMax != -1)
            u->green = C[uID][coloring.greenMax] - C[uID][coloring.greenMin-1];
        u->redI = u->blueI = u->greenI = 0;
        u->redL = u->red;
        u->blueL = u->blue;
        u->greenL = u->green;
        u->white = totalWhite - u->numLeaves + u->red + u->blue + u->green;
        u->redblue = u->redgreen = u->bluegreen = u->redbluegreen = 0;

        //add contribution of fans
        sharedTriplets = sharedTriplets + u->redL*u->blueL*u->greenL;
        //add contribution of resolved
        sharedTriplets = sharedTriplets + u->redL*u->blueL*u->white;

        return;
    }


    //we don't need to explicitly define I, since in the preprocessing step we made sure that every child that is an internal node
    //appears first in the list of the children of u.

    for(int i=0;i<u->children.size();i++){
        if(u->children[i]->children.size() == 0) break;
        findSharedTripletsGivenAColoring(u->children[i]);
    }

    //dynamic programming

    u->red = C[uID][coloring.redMax] - C[uID][coloring.redMin-1];
    u->blue = C[uID][coloring.blueMax] - C[uID][coloring.blueMin-1];
    u->green = 0;
    if(coloring.greenMax != -1)
        u->green = C[uID][coloring.greenMax] - C[uID][coloring.greenMin-1];
    T1node* w = u->children[0];
    int wID = w->id - n - 1;
    u->redI = w->red;
    u->blueI = w->blue;
    u->greenI = w->green;
    u->redL = u->red - (C[wID][coloring.redMax] - C[wID][coloring.redMin-1]);
    u->blueL = u->blue  - (C[wID][coloring.blueMax] - C[wID][coloring.blueMin-1]);
    u->greenL = u->green;
    if(coloring.greenMax!=-1)
        u->greenL = u->greenL - (C[wID][coloring.greenMax] - C[wID][coloring.greenMin-1]);
    u->white = totalWhite - u->numLeaves + u->red + u->blue + u->green;
    u->redblue = u->redgreen = u->bluegreen = u->redbluegreen = 0;

    for(int i=1;i<u->children.size();i++){
        w = u->children[i];
        if(w->children.size() == 0) break;
        u->redbluegreen = u->redbluegreen + u->redblue*w->green + u->redgreen*w->blue + u->bluegreen*w->red;
        u->redblue = u->redblue + u->redI*w->blue + u->blueI*w->red;
        u->redgreen = u->redgreen + u->redI*w->green + u->greenI*w->red;
        u->bluegreen = u->bluegreen + u->blueI*w->green + u->greenI*w->blue;
        u->redI = u->redI + w->red;
        u->blueI = u->blueI + w->blue;
        u->greenI = u->greenI + w->green;
        wID = w->id - n -1;
        u->redL = u->redL - (C[wID][coloring.redMax] - C[wID][coloring.redMin-1]);
        u->blueL = u->blueL - (C[wID][coloring.blueMax] - C[wID][coloring.blueMin-1]);
        if(coloring.greenMax != -1)
            u->greenL = u->greenL - (C[wID][coloring.greenMax] - C[wID][coloring.greenMin-1]);
    }

    //add contribution of fans (Algorithm 5 in paper)
    sharedTriplets = sharedTriplets + u->redbluegreen
            + u->redblue*u->greenL + u->redgreen*u->blueL + u->bluegreen*u->redL
            + u->redI*u->blueL*u->greenL + u->blueI*u->redL*u->greenL + u->greenI*u->redL*u->blueL
            + u->redL*u->blueL*u->greenL;
    //add contribution of resolved
    sharedTriplets = sharedTriplets + u->redblue*u->white
            + u->redI*u->blueL*u->white + u->blueI*u->redL*u->white
            + u->redL*u->blueL*u->white;

}

ulli computeTripletDistance(){

    sharedTriplets = 0;
    for(int i=0;i<T2size;i++){
        if(i == 0 && T2[i].children.size() == 2) continue; //small optimization if there are only two children, no fans and resolved can be rooted in the root.
        T2node * cur = &T2[i];
        if(cur->children.size() == 0) continue;
        coloring.redMin = cur->children[0]->minDFSid;
        for(int j=1;j<cur->children.size();j++){
            coloring.redMax = cur->children[j-1]->maxDFSid;
            totalWhite = n - (coloring.redMax - coloring.redMin + 1);
            T2node* v = cur->children[j];
            coloring.blueMin = v->minDFSid;
            coloring.blueMax = v->maxDFSid;
            totalWhite = totalWhite - (coloring.blueMax - coloring.blueMin + 1);
            coloring.greenMax = coloring.greenMin = -1;
            if(j+1 < cur->children.size()){
                coloring.greenMin = cur->children[j+1]->minDFSid;
                coloring.greenMax = cur->children[cur->children.size()-1]->maxDFSid;
                totalWhite = totalWhite - (coloring.greenMax - coloring.greenMin + 1);
            }
            findSharedTripletsGivenAColoring(&T1[0]);
        }
    }

    ulli n1, n2;
    n1 = n-1;
    n2 = n-2;
    if(n%2==0) n = n/2;
    else n1 = n1/2;
    if(n2%3==0) n2 = n2/3;
    else if(n1%3==0) n1 = n1/3;
    else n = n/3;

    delete[] T1;
    delete[] T2;

    return n*n1*n2 - sharedTriplets;

}

#endif //QTD_TRIPLET_DIST_H
