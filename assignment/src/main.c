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
    bool DEFAULT_DISP_G = false;
    bool DEFAULT_DISP_F = false;
    bool DEFAULT_DISP_f = false;
    bool DEFAULT_DISP_P = false;
    int numArg = 1;

    for(int i = 1 ; i < argc ; i++ ){
        if(strcmp(argv[i],"-h") == 0){
            printf("You can use the following argument :\n");
            printf("-h : To display this message\n");
            printf("-F : To display the computed formula\n");
            printf("-v : To display the parsed graphs\n");
            printf("-t : To display a valid path for each graph of size found in common\n");
            printf("-o : To create a .dot file in the sol folder with the valid paths written in them\n");
            numArg ++;
        }
        if(strcmp(argv[i],"-F") == 0){
            DEFAULT_DISP_F = true;
            numArg ++;
        }
        if(strcmp(argv[i],"-t") == 0){
            DEFAULT_DISP_P = true;
            numArg ++;
        }
        if(strcmp(argv[i],"-v") == 0){
            DEFAULT_DISP_G = true;
            numArg ++;
        }
        if(strcmp(argv[i],"-o") == 0){
            DEFAULT_DISP_f = true;
            numArg ++;
        }
    }
    /*
    if(argc>3 && (strcmp(argv[1],"-h") == 0 || strcmp(argv[2],"-h") == 0 || strcmp(argv[3],"-h") == 0)){
        printf("You can use the following argument :\n");
        printf("-h : To display this message\n");
        printf("-F : To display the computed formula\n");
        printf("-v : To display the parsed graphs\n");
        numArg ++;
    }
    if(argc>3 && (strcmp(argv[1],"-F") == 0 || strcmp(argv[2],"-F") == 0 || strcmp(argv[3],"-F") == 0)){
        printf("ici\n");
        DEFAULT_DISP_F = true;
        numArg ++;
    }
    if(argc>3 && (strcmp(argv[1],"-v") == 0 || strcmp(argv[2],"-v") == 0 || strcmp(argv[3],"-v") == 0)){
        DEFAULT_DISP_G = true;
        numArg ++;
    }
    */
    int numGraph = argc-numArg;
    if (numGraph < 1){
        //usage
        printf("Please enter at least one graph, for example: ./equalPath -h graphs/assignment-instance/G1.dot graphs/assignment-instance/triangle.dot\n");
        return EXIT_SUCCESS;
    }
    Graph graph[numGraph];
    for(int i = 0 ; i < numGraph ; i++){
        graph[i] = getGraphFromFile(argv[i+numArg]);
        if(DEFAULT_DISP_G){
            printGraph(graph[i]);
        }
    }
    //graph = getGraphFromFile(argv[1]);
    //printGraph(graph[0]);
    Z3_context ctx = makeContext();
    // Pour un k précis
    /*
    Z3_ast formula = graphsToPathFormula(ctx, graph, numGraph, pathLength);
    Z3_lbool isSat = isFormulaSat(ctx,formula);
    switch (isSat){
        case Z3_L_FALSE:
            printf("%s is not satisfiable.\n",Z3_ast_to_string(ctx,formula));
            break;

        case Z3_L_UNDEF:
            printf("We don't know if %s is satisfiable.\n",Z3_ast_to_string(ctx,formula));
            break;

        case Z3_L_TRUE:
            printf("%s is satisfiable.\n",Z3_ast_to_string(ctx,formula));
            Z3_model model = getModelFromSatFormula( ctx, formula);
            printPathsFromModel( ctx, model, graph, numGraph, pathLength);
            /*Z3_model model = getModelFromSatFormula(ctx,absurd);
            printf("Model obtained for %s:\n",Z3_ast_to_string(ctx,absurd));
            printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,x),valueOfVarInModel(ctx,model,x));
            printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,y),valueOfVarInModel(ctx,model,y));
            printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,negX),valueOfVarInModel(ctx,model,negX));
            
            break;
    }
    */
    // Pour tout k entre 0 et kmax
    
    Z3_ast formula = graphsToFullFormula(ctx,graph,numGraph);
    Z3_lbool isSat = isFormulaSat(ctx,formula);
    switch (isSat){
        case Z3_L_FALSE:
            printf("Non\n");
            break;

        case Z3_L_UNDEF:
            printf("We don't know if %s is satisfiable.\n",Z3_ast_to_string(ctx,formula));
            break;

        case Z3_L_TRUE:
            if(DEFAULT_DISP_F){
                printf("%s \n",Z3_ast_to_string(ctx,formula));
            }
            printf("Oui\n");
            
            if(DEFAULT_DISP_P){
                Z3_model model = getModelFromSatFormula( ctx, formula);
                int k = getSolutionLengthFromModel(ctx,model,graph);
                printPathsFromModel( ctx, model, graph, numGraph, k);
            }

            if(DEFAULT_DISP_f){
                Z3_model model = getModelFromSatFormula(ctx, formula);
                int k = getSolutionLengthFromModel(ctx,model,graph);
                createDotFromModel(ctx, model, graph, numGraph, k, "result");
            }
            
            /*Z3_model model = getModelFromSatFormula(ctx,absurd);
            printf("Model obtained for %s:\n",Z3_ast_to_string(ctx,absurd));
            printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,x),valueOfVarInModel(ctx,model,x));
            printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,y),valueOfVarInModel(ctx,model,y));
            printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,negX),valueOfVarInModel(ctx,model,negX));
            */
            break;
    }

    for(int i = 0 ; i < numGraph ; i++){
        deleteGraph(graph[i]);
    }
    printf("All graphs deleted.\n");

    Z3_del_context(ctx);
    printf("Context deleted, memory is now clean.\n");
    return EXIT_SUCCESS;
}