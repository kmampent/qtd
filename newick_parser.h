//
// Created by kostas on 4/20/19.
//

#ifndef QTD_NEWICK_PARSER_H
#define QTD_NEWICK_PARSER_H

#include <string>
#include <fstream>
#include "common.h"
#include "T1.h"
#include "T2.h"

using namespace std;

/*
 *
 *
 * Reading and writing for T1. We store T1 in a pre order layout.
 *
 *
 */

void parseTrees(const char* file1, const char* file2){

    string T1newickTemp;
    ifstream fin;
    fin.open(file1, ios_base::in);

    //read newick string of T1
    fin>>T1newickTemp;
    fin.close();

    int i;
    int up = T1newickTemp.size() - 1;
    T1size = 0;
    T1numLeafs = 0;
    i = 0;
    while(i<up){
        if(T1newickTemp[i] == '('){
            i++;
            T1size++;
        }
        else if(T1newickTemp[i] == ',' || T1newickTemp[i] == ')') {
            i++;
        }
        else{
            //leaf
            T1size++;
            T1numLeafs++;
            i++;
            while(T1newickTemp[i] >= '0' && T1newickTemp[i] <= '9'){
                i++;
            }
        }
    }

    string T2newickTemp;
    fin.open(file2, ios_base::in);

    //read newick string of T2
    fin>>T2newickTemp;
    fin.close();

    up = T2newickTemp.size() - 1;
    T2size = 0;
    T2numLeafs = 0;
    i = 0;
    while(i<up){
        if(T2newickTemp[i] == '('){
            i++;
            T2size++;
        }
        else if(T2newickTemp[i] == ',' || T2newickTemp[i] == ')') {
            i++;
        }
        else{
            //leaf
            T2size++;
            T2numLeafs++;
            i++;
            while(T2newickTemp[i] >= '0' && T2newickTemp[i] <= '9'){
                i++;
            }
        }
    }

    string & T1newick = T1newickTemp;
    string & T2newick = T2newickTemp;

    n = T1numLeafs;
    q = T1size - n;
    if(T2size - n < q){
        q = T2size - n;
        swap(T1size, T2size);
        swap(T1newick, T2newick);
    }

    /*
     *
     * we know the number of nodes, so let's just initialize the memory for T1
     *
     *
     */

    T1 = new T1node[T1size];

    /*
     * create the connections in T1, following the pre order layout
     * O(N/B) I/Os
     *
     */

    int curInternalID = n+1;

    int *myConnectionStack = new int[T1size];
    int myConnectionStackTop = 0;
    int *degreeStack = new int[T1size];
    int degreeStackTop = 0;

    i = 0;
    int curNode = 0;
    T1[0].parent = nullptr;
    while(i<T1newick.size()){

        if(T1newick[i] == '('){
            T1[curNode].id = curInternalID++;
            myConnectionStack[myConnectionStackTop++] = curNode;
            if(degreeStackTop > 0)
                degreeStack[degreeStackTop-1]++;
            degreeStack[degreeStackTop++] = 0;
            i++;
            curNode++;
        }
        else if(T1newick[i] == ')'){

            int degree = degreeStack[degreeStackTop-1];
            int parentIndex = myConnectionStackTop-degree-1;
            int toMergePos =  myConnectionStack[parentIndex];

            for(int d=1;d<=degree;d++) {
                T1[toMergePos].children.push_back(&T1[myConnectionStack[parentIndex + d]]);
                T1[myConnectionStack[parentIndex + d]].parent = &T1[toMergePos];
            }

            myConnectionStackTop-=degree;
            degreeStackTop--;
            i++;
        }
        else if(T1newick[i] >= '0' && T1newick[i] <= '9'){

            char treeLabel[20]; //buffer used for the label
            int j = 0;
            while(T1newick[i] >= '0' && T1newick[i] <= '9'){
                treeLabel[j] = T1newick[i];
                i++;
                j++;
            }

            treeLabel[j] = '\0';
            T1[curNode].id = atoi(treeLabel);
            myConnectionStack[myConnectionStackTop++] = curNode;
            degreeStack[degreeStackTop-1]++;
            curNode++;

        }
        else{
            i++;
        }
    }

    delete[] myConnectionStack;
    delete[] degreeStack;



    /*
     *
     * we know the number of nodes, so let's just initialize the memory for T2
     *
     *
     */

    T2 = new T2node[T2size];

    /*
     * create the connections in T2, following the pre order layout
     * O(N/B) I/Os
     *
     */

    curInternalID = n+1;

    myConnectionStack = new int[T2size];
    myConnectionStackTop = 0;
    degreeStack = new int[T2size];
    degreeStackTop = 0;

    i = 0;
    curNode = 0;
    while(i<T2newick.size()){

        if(T2newick[i] == '('){
            T2[curNode].id = curInternalID++;
            myConnectionStack[myConnectionStackTop++] = curNode;
            if(degreeStackTop > 0)
                degreeStack[degreeStackTop-1]++;
            degreeStack[degreeStackTop++] = 0;
            i++;
            curNode++;
        }
        else if(T2newick[i] == ')'){

            int degree = degreeStack[degreeStackTop-1];
            int parentIndex = myConnectionStackTop-degree-1;
            int toMergePos =  myConnectionStack[parentIndex];

            for(int d=1;d<=degree;d++)
                T2[toMergePos].children.push_back(&T2[myConnectionStack[parentIndex+d]]);

            myConnectionStackTop-=degree;
            degreeStackTop--;
            i++;
        }
        else if(T2newick[i] >= '0' && T2newick[i] <= '9'){

            char treeLabel[20]; //buffer used for the label
            int j = 0;
            while(T2newick[i] >= '0' && T2newick[i] <= '9'){
                treeLabel[j] = T2newick[i];
                i++;
                j++;
            }

            treeLabel[j] = '\0';
            T2[curNode].id = atoi(treeLabel);
            myConnectionStack[myConnectionStackTop++] = curNode;
            degreeStack[degreeStackTop-1]++;
            curNode++;

        }
        else{
            i++;
        }
    }

    delete[] myConnectionStack;
    delete[] degreeStack;
}

#endif //QTD_NEWICK_PARSER_H
