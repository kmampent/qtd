#include <vector>
#include <random>
#include <fstream>
#include <chrono>       // std::chrono::system_clock
#include <algorithm>
#include <iostream>
using namespace std;

struct node {
    node* parent;
    vector<node*> children;
    int key;
    node(){
        parent = nullptr;
        children.clear();
        key = -1;
    }
};

void printOut(node* cur, ofstream& fout){

    if(cur->children.size() == 0){
        fout<<cur->key;
        return;
    }
    else {
        fout << "(";
    }

    int i;
    for(i = 0; i < cur->children.size() - 1; i++){
        printOut(cur->children[i], fout);
        fout << ",";
    }
    printOut(cur->children[i], fout);
    fout<<")";

}

void bstPrintDotAux(node* cur, ofstream& treeFout) {

    if(cur->children.size() == 0) return;

    int i;
    for(i=0;i<cur->children.size();i++){
        treeFout << "    " << cur->key << " -> " << cur->children[i]->key
                 << "\n"; //for colored edges just have "[color=green];\n";
        bstPrintDotAux(cur->children[i], treeFout);
    }

}

void printDotHelper(node* cur, ofstream& treeFout){
    treeFout<<"digraph BST {\n";
    treeFout<<"    node [fontname=\"Arial\"];\n";

    if(cur == nullptr){
        treeFout<<"\n";
    }
    else if(cur->children.size() == 0){
        treeFout<<"    "<<cur->key<<";\n";
    }
    else{
        bstPrintDotAux(cur, treeFout);
    }

    treeFout<<"}\n";
}

void printDot(const char * file, node * root){

    ofstream treeFout;
    treeFout.open(file, ios_base::out);
    printDotHelper(root, treeFout);
    treeFout.close();

}


void freeMemoryHelper(node* cur){

    if(cur->children.size() == 0){
        delete cur;
        return;
    }

    for(int i = 0; i < cur->children.size(); i++){
        freeMemoryHelper(cur->children[i]);
    }

    delete cur;
}

void freeMemory(node* root){

    freeMemoryHelper(root);

}

//generate tree with q internal nodes and n leaves
bool generate(int n, int q, string& out) {

    struct node *root;
    vector<node*> nodes;
    root = new node();
    nodes.push_back(root);

    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());

    while (nodes.size()!=q) {
        std::uniform_int_distribution<int> distr(0, nodes.size() - 1);
        //pick a random node and create a child for it
        int randomNodeIndex = distr(generator);
        node* randomNode = nodes[randomNodeIndex];
        node* nn = new node();
        randomNode->children.push_back(nn);
        nodes.push_back(nn);
    }

    //we have a tree with q internal nodes. Append the leaves. Make sure that every internal node has at least two children.

    vector<node*> leaves;
    int numLeaves = 0;
    for(int i=0;i<nodes.size();i++){
        if(nodes[i]->children.size() <= 1){
            node * nn = new node();
            nodes[i]->children.push_back(nn);
            leaves.push_back(nn);
            numLeaves++;
            if(nodes[i]->children.size() == 1) {
                nn = new node();
                nodes[i]->children.push_back(nn);
                leaves.push_back(nn);
                numLeaves++;
            }
        }
    }

    if(leaves.size() > n){
        freeMemory(root);
        return false;
    }

    while(leaves.size() != n){
        std::uniform_int_distribution<int> distr(0, nodes.size() - 1);
        //pick a random node and add a child leaf under it
        int randomNodeIndex = distr(generator);
        node* randomNode = nodes[randomNodeIndex];
        node * nn  = new node();
        randomNode->children.push_back(nn);
        leaves.push_back(nn);
    }

    //labels. Make every internal node have the label n+1.... maxSize and the leaves 1 to n

    vector<int> leafLabels(n);
    for(int i=0;i<n;i++) leafLabels[i] = i+1;
    //shuffle the labels

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(leafLabels.begin(), leafLabels.end(), std::default_random_engine(seed));

    for(int i=0;i<leaves.size();i++) leaves[i]->key = leafLabels[i];

    for(int i=0;i<nodes.size();i++) nodes[i]->key = ++n;

    ofstream fout;
    fout.open(out);
    printOut(root, fout);
    fout<<";"<<endl;
    fout.close();
    //printDot("dot", root);
    freeMemory(root);
    return true;

}

//generate binary tree with n-1 internal nodes and n leaves
void generate(int n, string& out){

    struct edge {
        struct node *cur;
        struct node *parent;
    };

    struct node *root;
    vector<edge> edges;
    vector<node*> inodes, lnodes;
    root = new node();

    edge e;
    e.cur = root;
    e.parent = nullptr;
    edges.push_back(e);
    lnodes.push_back(root);

    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());

    while (edges.size() != 2 * n - 1) {

        std::uniform_int_distribution<int> distr(0, edges.size() - 1);
        //pick a random edge to expand with the same probability

        int randomEdgeIndex = distr(generator);
        edge randomEdge = edges[randomEdgeIndex];
        //make one internal node and one leaf
        node *inode = new node();
        node *lnode = new node();
        inodes.push_back(inode);
        lnodes.push_back(lnode);
        inode->parent = randomEdge.parent;
        lnode->parent = inode;
        randomEdge.cur->parent = inode;

        if (randomEdge.cur == root) {

            //by convention assume that  the root edge is a left going edge
            inode->children.push_back(randomEdge.cur);
            inode->children.push_back(lnode);
            root = inode;

        } else {

            if (randomEdge.cur == randomEdge.parent->children[0]) {
                //current node on the edge is the left child of the parent
                inode->parent->children[0] = inode;
                inode->children.push_back(randomEdge.cur);
                inode->children.push_back(lnode);
            } else {
                //current node on the edge is the right child of the parent
                inode->parent->children[1] = inode;
                inode->children.push_back(lnode);
                inode->children.push_back(randomEdge.cur);

            }

        }

        edge eUP, eDOWN, eDIRECTION;
        eUP.cur = inode;
        eUP.parent = inode->parent;
        eDOWN.cur = randomEdge.cur;
        eDOWN.parent = inode;
        eDIRECTION.cur = lnode;
        eDIRECTION.parent = inode;

        edges[randomEdgeIndex] = eUP;
        edges.push_back(eDOWN);
        edges.push_back(eDIRECTION);

    }

    //labels. Make every internal node have the label n+1.... maxSize and the leaves 1 to n

    vector<int> leafLabels(n);
    for(int i=0;i<n;i++) leafLabels[i] = i+1;
    //shuffle the labels

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(leafLabels.begin(), leafLabels.end(), std::default_random_engine(seed));

    for(int i=0;i<lnodes.size();i++) lnodes[i]->key = leafLabels[i];
    for(int i=0;i<inodes.size();i++) inodes[i]->key = ++n;

    ofstream fout;
    fout.open(out);
    printOut(root, fout);
    fout<<";"<<endl;
    fout.close();
    //printDot("dot", root);
    freeMemory(root);

}

int main(int argc, char* argv[]) {

    if (argc != 4) {
        cout << "type ./executable q n outputFile" << endl;
        return 1;
    }

    int n, q;
    q = atoi(argv[1]);
    n = atoi(argv[2]);
    string out = argv[3];

    if(q != -1){
        if(!generate(n, q, out)){
            cout<<"you need to have more leaves"<<endl;
            return 1;
        }
    }
    else{
        //create a binary tree with n leaves.
        generate(n, out);
    }



    return 0;
}