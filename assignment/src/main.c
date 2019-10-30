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
#include "Solving.h"

int main(int argc, char* argv[]){
    Graph graph;
    graph = getGraphFromFile("graphs/assignment-instance/G1.dot");
    //graph = getGraphFromFile(argv[1]);
    printGraph(graph);
    printf("step1\n");
    Z3_context ctx = makeContext();
    printf("step2\n");
    Z3_ast formula = graphsToPathFormula(ctx, &graph, 1, 2);
    printf("step3\n");
    Z3_lbool isSat = isFormulaSat(ctx,formula);
    printf("step4\n");
    switch (isSat){
        case Z3_L_FALSE:
            printf("%s is not satisfiable.\n",Z3_ast_to_string(ctx,formula));
            break;

        case Z3_L_UNDEF:
            printf("We don't know if %s is satisfiable.\n",Z3_ast_to_string(ctx,formula));
            break;

        case Z3_L_TRUE:
            printf("%s is satisfiable.\n",Z3_ast_to_string(ctx,formula));
            /*Z3_model model = getModelFromSatFormula(ctx,absurd);
            printf("Model obtained for %s:\n",Z3_ast_to_string(ctx,absurd));
            printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,x),valueOfVarInModel(ctx,model,x));
            printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,y),valueOfVarInModel(ctx,model,y));
            printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,negX),valueOfVarInModel(ctx,model,negX));
            */
            break;
    }


    deleteGraph(graph);
    return 1;
}