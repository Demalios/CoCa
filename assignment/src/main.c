/* A compléter */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../include/Parsing.h"
#include "../include/Graph.h"
#include <z3.h>
#include "../include/Z3Tools.h"
#include <string.h>
#include <math.h>

int main(int argc, char* argv[]){
    Graph graph;
    graph = getGraphFromFile("G1.dot");
    //graph = getGraphFromFile(argv[1]);
    printGraph(graph);
    void deleteGraph(graph);
    return 1;
}