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

#define mini(a,b) (a<=b?a:b)
#define MAX_NAME_LENGTH 50

bool DEFAULT_DISP_G = false;
bool DEFAULT_DISP_F = false;
bool DEFAULT_DISP_f = false;
bool DEFAULT_DISP_P = false;
bool DEFAULT_DISP_s = false;
bool DEFAULT_DISP_d = false;
bool DEFAULT_DISP_a = false;
bool DEFAULT_DISP_o = false;
char DEFAULT_FILE_NAME[MAX_NAME_LENGTH] = "result";
int numArg = 1;

/**
 * @brief A function testing if the given formula is SAT and will apply the different option given.
 * 
 * @param ctx, The solver context.
 * @param formula, A Z3_ast formula
 * @param graphs, An array of graphs.
 * @param numGraph, The number of graphs in @p graphs.
 * @return A boolean indicating if the formula was SAT or not.
 */
bool SAT(Z3_context ctx, Z3_ast formula, Graph * graphs, int numGraph){
    if(formula==NULL){
        printf("Non\n");
        return false;
    }
    Z3_lbool isSat = isFormulaSat(ctx,formula);
    switch (isSat){
        case Z3_L_FALSE:
            printf("Non\n");
            return false;

        case Z3_L_UNDEF:
            printf("We don't know if %s is satisfiable.\n",Z3_ast_to_string(ctx,formula));
            return false;

        case Z3_L_TRUE:
            if(DEFAULT_DISP_F){
                printf("%s \n",Z3_ast_to_string(ctx,formula));
            }
            printf("Oui\n");
            
            if(DEFAULT_DISP_P){
                Z3_model model = getModelFromSatFormula(ctx, formula);
                int k = getSolutionLengthFromModel(ctx,model,graphs);
                printPathsFromModel( ctx, model, graphs, numGraph, k);
            }
            if(DEFAULT_DISP_f){
                Z3_model model = getModelFromSatFormula(ctx, formula);
                int k = getSolutionLengthFromModel(ctx,model,graphs);
                createDotFromModel(ctx, model, graphs, numGraph, k, DEFAULT_FILE_NAME);
            }
            return true;
    }
}

int main(int argc, char* argv[]){
    for(int i = 1 ; i < argc ; i++ ){
        if(strcmp(argv[i],"-h") == 0){
            printf("You can use the following argument :\n");
            printf("-h  Displays this help.\n");
            printf("-F  Displays the formula computed (obviously not in this version, but you should really display it in your code).\n");
            printf("-v  Activate verbose mode (displays parsed graphs).\n");
            printf("-t  Displays the paths found on the terminal [if not present, only displays the existence of the path].\n");
            printf("-f  Writes the result with colors in a .dot file. See next option for the name. These files will be produced in the folder 'sol'.\n");
            printf("-s  Tests separately all formula by depth [if not present: uses the global formula].\n");
            printf("-d  Only if -s is present. Explore the length in decreasing order. [if not present: in increasing order].\n");
            printf("-a  Only if -s is present. Computes a result for every length instead of stopping at the first positive result (default behaviour).\n");
            printf("-o NAME Writes the output in \"NAME-lLENGTH.dot\" where LENGTH is the length of the solution. Writes several files in this format if both -s and -a are present. [if not present: \"result-lLENGTH.dot\".\n");
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
        if(strcmp(argv[i],"-f") == 0){
            DEFAULT_DISP_f = true;
            numArg ++;
            for(int j = 0 ; j < argc-1 ; j++){
                if(strcmp(argv[j],"-o") == 0 && strstr(argv[j+1], ".dot") == NULL && argv[j+1][0] != '-'){
                    strncpy (DEFAULT_FILE_NAME, argv[j+1], mini(strlen(argv[j+1]),MAX_NAME_LENGTH));
                    DEFAULT_FILE_NAME[strlen(argv[j+1])] = '\0';
                    DEFAULT_DISP_o = true;
                    numArg += 2;
                }
            }
        }
        if(strcmp(argv[i],"-s") == 0){
            for(int j = 0 ; j < argc ; j++ ){
                if( strcmp(argv[j],"-d") == 0){
                    DEFAULT_DISP_d = true;
                    numArg ++;
                }
                if( strcmp(argv[j],"-a") == 0){
                    DEFAULT_DISP_a = true;
                    numArg ++;
                }
            }
            DEFAULT_DISP_s = true;
            numArg ++;
        }
    }
    int numGraph = argc-numArg;
    if (numGraph < 1){
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
    Z3_context ctx = makeContext();
    //Z3_ast formula = graphsToFullFormula(ctx,graph,numGraph);
    if(DEFAULT_DISP_s){
        int maxK = orderG(graph[0]); //vérifier si ça ne commence pas à l'indice 1
        for(int i = 1; i < numGraph; i++){
            maxK = mini(maxK,orderG(graph[i]));
        }
        if(DEFAULT_DISP_d){
            for(int i = maxK -1 ; i >= 0 ; i--){
                Z3_ast formula = graphsToPathFormula(ctx,graph,numGraph,i);
                printf("Pour k = %d : \n",i);
                if(SAT(ctx,formula,graph,numGraph) && DEFAULT_DISP_a == false){
                    break;
                }
            }
        }else{
            for(int i = 0 ; i < maxK ; i++){
                Z3_ast formula = graphsToPathFormula(ctx,graph,numGraph,i);
                printf("Pour k = %d : \n",i);
                if(SAT(ctx,formula,graph,numGraph) && DEFAULT_DISP_a == false){
                    break;
                }
            }
        }
    }else{
        Z3_ast formula = graphsToFullFormula(ctx,graph,numGraph);
        SAT(ctx,formula,graph,numGraph);
    }
    for(int i = 0 ; i < numGraph ; i++){
        deleteGraph(graph[i]);
    }
    printf("All graphs deleted.\n");

    Z3_del_context(ctx);
    printf("Context deleted, memory is now clean.\n");
    return EXIT_SUCCESS;
}