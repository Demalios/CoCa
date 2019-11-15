/**
 * @file Solving.c
 * @author Alan Aneas & Alexis Cataldi (alan.aneas@etu.u-bordeaux.fr & alexis.cataldi@etu.u-bodeaux.fr)
 * @brief  An implementation of the Coca project for 2019 year. Convert a set of graphs to a formula true if and only if they all contain a simple accepting path
 *         of a given length, and provides functions to display the result if the formula is satisfiable.
 * @version 1
 * 
 * 
 */

#ifndef COCA_SOLVING_H_
#define COCA_SOLVING_H_

#include "Graph.h"
#include <z3.h>
#include "Z3Tools.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * @brief Small function to return the maximum between the two parameter
 * 
 * @param a, an integer
 * @param b, another integer
 * @return either a or b depending on which one is bigger
 */
int max(int a , int b){
    if(a<b){
        return b;
    }
    return a;
}

/**
 * @brief Small function to return the minimum between the two parameter
 * 
 * @param a, an integer
 * @param b, another integer
 * @return either a or b depending on which one is smaller
 */
int min(int a , int b){
    if(a>b){
        return b;
    }
    return a;
}

/**
 * @brief Small function to return the maximum value of the common length
 * 
 * @param graph, a list containing the graphs
 * @param numGraph, an int corresponding to the number of graphs in graph
 * @return maxK, the maximum value of the common length
 */
int getMaxK(Graph * graph, int numGraph){
    int maxK = orderG(graph[0]); //vérifier si ça ne commence pas à l'indice 1
    for(int i = 1; i < numGraph; i++){
        maxK = min(maxK, orderG(graph[i]));
    }
    return maxK;
}

/**
 * @brief Generates a formula consisting of a variable representing the fact that @p node of graph number @p number is at position @p position of an accepting path.
 * Génère une formule consistant en une variable représentant le fait que node du graphe number soit à la position position d'un chemin acceptant.
 * 
 * @param ctx, The solver context. 
 * @param number, The number of the graph. 
 * @param position, The position in the path. 
 * @param k, The mysterious k from the subject of this assignment.
 * @param node, The node identifier.
 * @return Z3_ast, The formula.
 */
Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node){
    char* str = (char*)malloc(sizeof(char)*(int)ceil(10+log10(max(number,1))+log10(max(position,1))+log10(max(k,1))+log10(max(node,1))));
    if(str == NULL){
        printf("Not enough memory to allocate str in getNodeVariable\n");
        exit(EXIT_FAILURE);
    }
    sprintf(str, "X_%d,%d,%d,%d", (number+1), position, k, node);
    Z3_ast x = mk_bool_var(ctx, str);
    free(str);
    return x; // His name will be X_i,j,k,q ( i = number, j = position, k = k, q = node )
}


/**
 * @brief A function used to build our ɸ1 formula
 * 
 * @param ctx, The solver context.
 * @param graphs, An array of graphs.
 * @param i, The number of graphs in @p graphs.
 * @param pathLength, The length of the path to check.
 * @return Z3_ast, The formula.
 */
Z3_ast graphToPhi1Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    for(int j = 0; j < orderG(graphs[i]); j++){
        if(isSource(graphs[i],j)){
            return getNodeVariable(ctx,i,0,pathLength,j);
        }
    }
}

/**
 * @brief A function used to build our ɸ2 formula
 * 
 * @param ctx, The solver context.
 * @param graphs, An array of graphs.
 * @param i, The number of graphs in @p graphs.
 * @param pathLength, The length of the path to check.
 * @return Z3_ast, The formula.
 */
Z3_ast graphToPhi2Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    for(int j = 0; j < orderG(graphs[i]); j++){
        //printf("j = %d\n",j);
        if(isTarget(graphs[i],j)){
            return getNodeVariable(ctx,i,pathLength,pathLength,j);
        }
    }
}

/**
 * @brief A function used to build our ɸ3 formula
 * 
 * @param ctx, The solver context.
 * @param graphs, An array of graphs.
 * @param i, The number of graphs in @p graphs.
 * @param pathLength, The length of the path to check.
 * @return Z3_ast, The formula.
 */
Z3_ast graphToPhi3Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    Z3_ast formulaAND[pathLength+1]; 
    for(int j = 0 ; j <= pathLength ; j++ ){
        Z3_ast formulaOR[orderG(graphs[i])];
        for(int u = 0 ; u < orderG(graphs[i]) ; u++ ){
            formulaOR[u] = getNodeVariable(ctx,i,j,pathLength,u);
        }
        formulaAND[j] = Z3_mk_or(ctx,orderG(graphs[i]),formulaOR);
    }
    return Z3_mk_and(ctx,pathLength+1,formulaAND);
}

/**
 * @brief A function used to build our ɸ4 formula
 * 
 * @param ctx, The solver context.
 * @param graphs, An array of graphs.
 * @param i, The number of graphs in @p graphs.
 * @param pathLength, The length of the path to check.
 * @return Z3_ast, The formula.
 */
Z3_ast graphToPhi4Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    Z3_ast* formulaAND1 = (Z3_ast*)malloc(sizeof(Z3_ast)*pathLength+1);
    if(formulaAND1 == NULL){
        printf("Not enough memory to allocate formulaAND1 in Phi4\n");
        exit(EXIT_FAILURE);
    }
    for(int j = 0 ; j <= pathLength ; j++){
        Z3_ast* formulaAND2 = (Z3_ast*)malloc(sizeof(Z3_ast)*orderG(graphs[i]));
        if(formulaAND2 == NULL){
            printf("Not enough memory to allocate formulaAND2 in Phi4\n");
            exit(EXIT_FAILURE);
        }
        for(int u = 0 ; u < orderG(graphs[i]) ; u++){
            Z3_ast* formulaAND3 = (Z3_ast*)malloc(sizeof(Z3_ast)*(orderG(graphs[i])-1));
            if(formulaAND3 == NULL){
                printf("Not enough memory to allocate formulaAND3 in Phi4\n");
                exit(EXIT_FAILURE);
            }
            int AND3index = 0;
            Z3_ast* formulaOR = (Z3_ast*)malloc(sizeof(Z3_ast)*2);
            if(formulaOR == NULL){
                printf("Not enough memory to allocate formulaOR in Phi4\n");
                exit(EXIT_FAILURE);
            }
            for(int v = 0 ; v < orderG(graphs[i]) ; v++){
                if(v!=u){
                    formulaOR[0] = Z3_mk_not(ctx,getNodeVariable(ctx,i,j,pathLength,u));
                    formulaOR[1] = Z3_mk_not(ctx,getNodeVariable(ctx,i,j,pathLength,v));
                    formulaAND3[AND3index++] = Z3_mk_or(ctx,2,formulaOR);
                }
            }
            free(formulaOR);
            formulaAND2[u] = Z3_mk_and(ctx,AND3index/*orderG(graphs[i])-1*/,formulaAND3);
            free(formulaAND3);
        }
        formulaAND1[j] = Z3_mk_and(ctx,orderG(graphs[i]),formulaAND2);
        free(formulaAND2);
    }
    Z3_ast finalAND = Z3_mk_and(ctx,pathLength+1,formulaAND1);
    free(formulaAND1);
    return finalAND;
}

/**
 * @brief A function used to build our ɸ5 formula
 * 
 * @param ctx, The solver context.
 * @param graphs, An array of graphs.
 * @param i, The number of graphs in @p graphs.
 * @param pathLength, The length of the path to check.
 * @return Z3_ast, The formula.
 */
Z3_ast graphToPhi5Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    Z3_ast formulaAND1[orderG(graphs[i])];
    Z3_ast* formulaAND2 = (Z3_ast*)malloc(sizeof(Z3_ast)*pathLength+1);
    if(formulaAND2 == NULL){
        printf("Not enough memory to allocate formulaAND2 in Phi5\n");
        exit(EXIT_FAILURE);
    }
    for(int u = 0 ; u < orderG(graphs[i]) ; u++ ){
        Z3_ast* formulaAND3 = (Z3_ast*)malloc(sizeof(Z3_ast)*pathLength);
        if(formulaAND3 == NULL){
            printf("Not enough memory to allocate formulaAND3 in Phi5\n");
            exit(EXIT_FAILURE);
        }
        for(int j = 0 ; j <= pathLength ; j++){
            int id = 0;
            for(int j2 = 0 ; j2 <= pathLength ; j2++ ){
                Z3_ast formulaOR[2];
                if(j2!=j){
                    formulaOR[0] = Z3_mk_not(ctx,getNodeVariable(ctx,i,j,pathLength,u));
                    formulaOR[1] = Z3_mk_not(ctx,getNodeVariable(ctx,i,j2,pathLength,u));
                    formulaAND3[id] = Z3_mk_or(ctx,2,formulaOR);
                    id++;
                }
            }
            formulaAND2[j] = Z3_mk_and(ctx,id/*pathLength*/,formulaAND3);
        }
        formulaAND1[u] = Z3_mk_and(ctx,pathLength+1,formulaAND2);
        free(formulaAND3);
    }
    free(formulaAND2);
    return Z3_mk_and(ctx,orderG(graphs[i]),formulaAND1);;
}

/**
 * @brief A function used to build our ɸ6 formula
 * 
 * @param ctx, The solver context.
 * @param graphs, An array of graphs.
 * @param i, The number of graphs in @p graphs.
 * @param pathLength, The length of the path to check.
 * @return Z3_ast, The formula.
 */
Z3_ast graphToPhi6Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    Z3_ast formulaAND[pathLength];
    for(int j = 0 ; j <= pathLength - 1; j++ ){
        int ind = 0;
        Z3_ast formulaOR[sizeG(graphs[i])];
        Z3_ast formulaLittleAND[2];
        for(int u = 0 ; u < orderG(graphs[i]) ; u++ ){
            for(int v = 0 ; v < orderG(graphs[i]) ; v++ ){
                if(isEdge(graphs[i],u,v)){
                    formulaLittleAND[0] = getNodeVariable(ctx,i,j,pathLength,u);
                    formulaLittleAND[1] = getNodeVariable(ctx,i,j+1,pathLength,v);
                    formulaOR[ind] = Z3_mk_and(ctx,2,formulaLittleAND);
                    ind ++;
                }
            }
        }
        formulaAND[j] = Z3_mk_or(ctx,ind/*sizeG(graphs[i])*/,formulaOR);
    }
    return Z3_mk_and(ctx,pathLength,formulaAND);
}

/**
 * @brief Small function returning a different value depending on the returned value of isFormulaSat
 * 
 * @param val, A Z3_bool returned by isFormulaSat
 * @return An integer ( -1 if the formula wasn't SAT, 0 if it was not determined & 1 if it is SAT )
 */
int isSatisfiable(Z3_lbool val){
    switch (val)
        {
        case Z3_L_FALSE:
            return -1;

        case Z3_L_UNDEF:
            return 0;

        case Z3_L_TRUE:
            return 1;
        }
}

/**
 * @brief Generates a SAT formula satisfiable if and only if all graphs of @p graphs contain an accepting path of length @p pathLength.
 * 
 * @param ctx, The solver context.
 * @param graphs, An array of graphs.
 * @param numGraphs, The number of graphs in @p graphs.
 * @param pathLength, The length of the path to check.
 * @return Z3_ast, The formula.
 */
Z3_ast graphsToPathFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength){
    Z3_ast* formulaAND = (Z3_ast*)malloc(sizeof(Z3_ast)*pathLength);
    if(formulaAND == NULL){
        printf("Not enough memory to allocate formulaLittleAND in graphsToPathFormula\n");
        exit(EXIT_FAILURE);
    }
    Z3_ast* formulaLittleAND = (Z3_ast*)malloc(sizeof(Z3_ast)*6); // 6 = number of formula
    if(formulaLittleAND == NULL){
        printf("Not enough memory to allocate formulaLittleAND in graphsToPathFormula\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0 ; i < numGraphs ; i++){
        formulaLittleAND[0] = graphToPhi1Formula(ctx, graphs, i, pathLength);
        formulaLittleAND[1] = graphToPhi2Formula(ctx, graphs, i, pathLength);
        formulaLittleAND[2] = graphToPhi3Formula(ctx, graphs, i, pathLength);
        formulaLittleAND[3] = graphToPhi4Formula(ctx, graphs, i, pathLength);
        formulaLittleAND[4] = graphToPhi5Formula(ctx, graphs, i, pathLength);
        formulaLittleAND[5] = graphToPhi6Formula(ctx, graphs, i, pathLength);
        formulaAND[i] = Z3_mk_and(ctx,6,formulaLittleAND);
    }
    
    Z3_ast x = Z3_mk_and(ctx,numGraphs,formulaAND);
    free(formulaLittleAND);
    free(formulaAND);
    return x;
}


/**
 * @brief Gets the length of the solution from a given model.
 * 
 * @param ctx, The solver context. 
 * @param model, A variable assignment. 
 * @param graphs, An array of graphs.
 * @return int, The length of a common simple accepting path in all graphs from @p graphs
 */ 
int getSolutionLengthFromModel(Z3_context ctx, Z3_model model, Graph *graphs){
    int maxLength = orderG(graphs[0]);
    for(int k = 1 ; k < maxLength+1 ; k++ ){
        int numK = 0;
        for(int j = 0 ; j < k+1 ; j++ ){
            int numVal = 0;
            for(int u = 0 ; u < maxLength ; u++ ){
                if(valueOfVarInModel(ctx, model, getNodeVariable(ctx,0,j,k,u))){
                    if(j==0){
                        if( isSource(graphs[0],u)){
                            numVal++;
                        }else{
                            // Falsification of the values if a node which isn't the starting point is proposed for the step 0. 
                            numVal++;
                            numVal++;
                        }
                    }else{
                        if(j==k){
                            if(isTarget(graphs[0],u)){
                                numVal++;
                            }else{
                                // Falsification of the values if a node which isn't the ending point is proposed for the step k. 
                                numVal++;
                                numVal++;
                            }
                        }else{
                            numVal++;
                        }
                    }
                }
            }
            if(numVal == 1){
                numK++;
            }
        }
        if(numK-1 == k){
            return k;
        }
    }
}


/**
 * @brief Generates a SAT formula satisfiable if and only if all graphs of @p graphs contain an accepting path of common length.
 * Génère une formule SAT satisfaisable si et seulement si tous les graphes de graphs contiennent un chemin acceptant de longueur commune.
 * 
 * @param ctx, The solver context.
 * @param graphs, An array of graphs.
 * @param numGraphs, The number of graphs in @p graphs. 
 * @return Z3_ast, The formula.
 */
Z3_ast graphsToFullFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs){
    int commonLength = getMaxK(graphs,numGraphs);
    int index = 0;
    Z3_ast formulaOR[commonLength];
    for(int l = 1; l < commonLength; l++){
        Z3_ast formulaToTest = graphsToPathFormula(ctx, graphs, numGraphs, l);
        if(isSatisfiable(isFormulaSat(ctx,formulaToTest)) == 1){
            formulaOR[index] = formulaToTest;
            index++;
        } 
    }
    Z3_ast x = Z3_mk_or(ctx,index,formulaOR);
    return x;
}




/**
 * @brief Small function printing the path for a graph
 * 
 * @param ctx, The solver context.
 * @param model, A variable assignment.
 * @param graph, A graph. 
 * @param graphIndex,  The index of the graph in the array.
 * @param pathLength, The length of path. 
 */
void oneGraphPrintPathsFromModel(Z3_context ctx, Z3_model model, Graph graph, int graphIndex, int pathLength){
    int tab[pathLength+1];
    for(int j = 0 ; j <= pathLength ; j++){
        for(int u = 0 ; u < orderG(graph) ; u++){
            if(valueOfVarInModel(ctx, model, getNodeVariable(ctx,graphIndex,j,pathLength,u))){
                tab[j] = u;
            }
        }
    }
    for(int k = 0 ; k < pathLength ; k++){
        printf("%s -> ",getNodeName(graph,tab[k]));
    }
    printf("%s;\n",getNodeName(graph,tab[pathLength]));
}

/**
 * @brief Displays the paths of length @p pathLength of each graphs in @p graphs described by @p model.
 * Affiche les chemins de longueur pathLength pour tous les graphes décrits dans model.
 * 
 * @param ctx, The solver context. 
 * @param model, A variable assignment. 
 * @param graphs, An array of graphs. 
 * @param numGraph, The number of graphs in @p graphs. 
 * @param pathLength, The length of path.
 */
void printPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength){
    for(int i = 0 ; i < numGraph ; i++ ){
        printf("Chemin valide pour le graphe %d\n",i);
        oneGraphPrintPathsFromModel(ctx, model, graphs[i], i, pathLength);
    }
}

/**
 * @brief Small function returning the integers corresponding to the starting and ending nodes of the graph.
 * 
 * @param ctx, The solver context. 
 * @param model, A variable assignment. 
 * @param graphNumber, The index of the graph in the array.
 * @param pathLength, The length of path.
 * @param tab, The array in which we save the integers with the one for the starting node in the case 0 and the ending one in the case 1.
 */
void get_source_and_destination(Z3_context ctx, Z3_model model, Graph graph, int graphNumber, int pathLength, int * tab){
    for(int u = 0 ; u < orderG(graph) ; u++){
        if(valueOfVarInModel(ctx, model, getNodeVariable(ctx,graphNumber,0,pathLength,u))){
            tab[0] = u;
        }numGraphs
        else if(valueOfVarInModel(ctx, model, getNodeVariable(ctx,graphNumber,pathLength,pathLength,u))){
            tab[1] = u;
        }
    }
    return;
}

/**
 * @brief Creates the file ("%s-l%d.dot",name,pathLength) representing the solution to the problem described by @p model, or ("result-l%d.dot,pathLength") if name is NULL.
 * Crée le fichier représentant la solution du problème décrit par model, ou ("result-l%d.dot,pathLength") si name == NULL
 * 
 * @param ctx, The solver context.
 * @param model, A variable assignment. 
 * @param graphs, An array of graphs.
 * @param numGraph, The number of graphs in @p graphs. 
 * @param pathLength, The length of path. 
 * @param name, The name of the output file. 
 */
void createDotFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength, char* name){
    // open the file for writing //
    char* tmp = (char*)malloc(sizeof(char)*(13+strlen(name)+log10(pathLength))); 
    if(tmp == NULL){
        printf("Not enough memory to allocate tmp in createDotFromModel\n");
        exit(EXIT_FAILURE);
    }
    int save_out = dup(STDOUT_FILENO);
    sprintf(tmp, "./sol/%s-l%d.dot",name,pathLength);
    int fp = open(tmp, O_CREAT | O_RDWR | O_TRUNC, 0777);
    close(1);
    dup2(fp,1);
    printf ("digraph %s{\n",name);
    for(int graphNumber = 0; graphNumber < numGraph; graphNumber++){
        int tab[2];
        // Display the starting and ending nodes.
        get_source_and_destination(ctx, model, graphs[graphNumber], graphNumber, pathLength, tab);
        printf ("_%d_%s [initial=1,color=green][style=filled,fillcolor=lightblue];\n",graphNumber,getNodeName(graphs[graphNumber],tab[0]));
        printf ("_%d_%s [final=1,color=red][style=filled,fillcolor=lightblue];\n",graphNumber,getNodeName(graphs[graphNumber],tab[1]));
        int numNode = orderG(graphs[graphNumber]);
        // Display every other nodes with the one in the path in 'light-blue'.
        for(int ind = 0 ; ind < numNode ; ind++ ){
            if(!isTarget(graphs[graphNumber],ind) && !isSource(graphs[graphNumber],ind)){
                bool used = false;
                for(int j = 0 ; j < pathLength+1 ; j++){
                    if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,graphNumber,j,pathLength,ind))){
                        used = true;
                        break;
                    }
                }
                if(used){
                    printf ("_%d_%s [style=filled,fillcolor=lightblue];\n",graphNumber,getNodeName(graphs[graphNumber],ind));
                }else{
                    printf ("_%d_%s ;\n",graphNumber,getNodeName(graphs[graphNumber],ind));
                }
            }
        }
        // Display every edges with the ones in the path in 'blue'.
        for(int u = 0 ; u < numNode ; u++ ){
            for(int v = 0 ; v < numNode ; v++ ){
                if(isEdge(graphs[graphNumber],u,v)){
                    bool used = false;
                    for(int j = 0 ; j < pathLength ; j++){
                        if(valueOfVarInModel(ctx,model,getNodeVariable(ctx,graphNumber,j,pathLength,u)) && valueOfVarInModel(ctx,model,getNodeVariable(ctx,graphNumber,j+1,pathLength,v))){
                            used = true;
                            break;
                        }
                    }
                    if(used){
                        printf ("_%d_%s -> _%d_%s [color=blue];\n",graphNumber,getNodeName(graphs[graphNumber],u),graphNumber,getNodeName(graphs[graphNumber],v));
                    }else{
                        printf ("_%d_%s -> _%d_%s;\n",graphNumber,getNodeName(graphs[graphNumber],u),graphNumber,getNodeName(graphs[graphNumber],v));
                    }
                }
            }
        }
    }
    printf ("}\n");
    dup2(save_out, STDOUT_FILENO);
    close(fp);
    free(tmp);
}

#endif

/*
Minimum syndical : implémenter les fonctions getNodeVariable, getPathFormula
et getFullFormula ainsi qu’un programme principal les utilisant pour résoudre le
problème  en  explorant  chaque  longueur  indépendamment.  Il  pourra  afficher  la
formule générée, ainsi que les graphes parsés via un système d’option.
—  Pour avoir la moyenne : En plus, récupérer le chemin calculé et l’afficher sur le
terminal, en implémentant la fonction printPathFromModels.
—  Amélioration 1 : Implémenter getFullFormula, et modifier le main de manière à
obtenir uniquement une réponse au problème (oui/non).
—  Amélioration 2 : Modifier getFullFormula de manière à ce que si la formule admet
un modèle, il contienne uniquement les chemins témoins de la solution. Vous aurez
également besoin d’implémenter getSolutionLengthFromModel.
—  Amélioration 3 : Implémenter createDotFromModel pour afficher les chemins solu-
tion au format .dot (comme dans l’exécutable fourni). Il devra notamment contenir
les informations de sommets sources et destination. Vous pouvez utiliser les cou-
leurs  de  votre  choix,  mais  nous  serons  content  si  vous  respecter  le  même  code
couleur que nous.
—  Amélioration 4 : Produire un exécutable avec un comportement similaire à celui
fourni, c’est à dire capable de proposer les différents algorithmes et affichages via
un système d’option
*/