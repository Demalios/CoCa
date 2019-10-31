#ifndef COCA_SOLVING_H_
#define COCA_SOLVING_H_

#include "Graph.h"
#include <z3.h>
#include "Z3Tools.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

// Renvoie l'entier le plus grand entre a et b

int max(int a , int b){
    if(a<b){
        return b;
    }
    return a;
}

int min(int a , int b){
    if(a>b){
        return b;
    }
    return a;
}

// Génère une formule consistant en une variable représentant le fait que node du graphe number soit à la position position d'un chemin acceptant.

Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node){
    char* str = (char*)malloc(sizeof(char)*(int)ceil(10+log10(max(number,1))+log10(max(position,1))+log10(max(k,1))+log10(max(node,1))));
    if(str == NULL){
        printf("Not enough memory to allocate str in getNodeVariable\n");
        exit(EXIT_FAILURE);
    }
    sprintf(str, "X_%d,%d,%d,%d", (number+1), position, k, node);
    Z3_ast x = mk_bool_var(ctx, str);
    free(str);
    return x; //son nom sera x_i,j,k,q
}


// Génère la sous-formule ɸ​1 pour le graphe i. ("Point de départ s")

Z3_ast graphToPhi1Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    for(int j = 0; j < orderG(graphs[i]); j++){
        if(isSource(graphs[i],j)){
            return getNodeVariable(ctx,i,0,pathLength,j);
        }
    }
}
/*
void graphToPhi1FormulaB(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength, Z3_ast * ret){
    for(int j = 0; j < orderG(graphs[i]); j++){
        if(isSource(graphs[i],j)){
            ret[0] = getNodeVariable(ctx,i,0,pathLength,j);
        }
    }
}
*/
// Génère la sous-formule ɸ​2 pour le graphe i. ("Point d'arrivée t")

Z3_ast graphToPhi2Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    for(int j = 0; j < orderG(graphs[i]); j++){
        if(isTarget(graphs[i],j)){
            return getNodeVariable(ctx,i,pathLength,pathLength,j);
        }
    }
}

// Génère la sous-formule ɸ​3 pour le graphe i. ("Au moins 1 sommet par position")

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

// Génère la sous-formule ɸ​4 pour le graphe i. ("Au plus 1 sommet par position")

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
                    //Z3_ast formulaOR[2]; //nombre magique
                    formulaOR[0] = Z3_mk_not(ctx,getNodeVariable(ctx,i,j,pathLength,u));
                    formulaOR[1] = Z3_mk_not(ctx,getNodeVariable(ctx,i,j,pathLength,v));
                    formulaAND3[AND3index++] = Z3_mk_or(ctx,2,formulaOR);
                }
            }
            free(formulaOR);
            formulaAND2[u] = Z3_mk_and(ctx,orderG(graphs[i])-1,formulaAND3);
            free(formulaAND3);
        }
        formulaAND1[j] = Z3_mk_and(ctx,orderG(graphs[i]),formulaAND2);
    }
    Z3_ast finalAND = Z3_mk_and(ctx,pathLength+1,formulaAND1);
    free(formulaAND1);
    return finalAND;
}

// Génère la sous-formule ɸ​5 pour le graphe i. ("Chaque sommet apparaît au plus une fois")

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
            formulaAND2[j] = Z3_mk_and(ctx,pathLength,formulaAND3);
        }
        formulaAND1[u] = Z3_mk_and(ctx,pathLength+1,formulaAND2);
        free(formulaAND3);
    }
    free(formulaAND2);
    return Z3_mk_and(ctx,orderG(graphs[i]),formulaAND1);;
}

// Renvoie le nombre de voisin d'un sommet ( Ancienne version )
/*
int numberNeighbour(Graph g, int u){
    int n = 0 ;
    for(int v = 0 ; v < orderG(g) ; v++ ){
        if(isEdge(g,u,v)){
            n ++;
        }
    }
    return n;
}
*/
// Génère la sous-formule ɸ​6 pour le graph i. ("Chemin de taille k continue")
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
        formulaAND[j] = Z3_mk_or(ctx,sizeG(graphs[i]),formulaOR);
    }
    return Z3_mk_and(ctx,pathLength,formulaAND);
}
/* ( Ancienne version )
Z3_ast graphToPhi6Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    Z3_ast formulaAND1[pathLength];
    for(int j = 0 ; j <= pathLength -1; j++ ){
        Z3_ast formulaAND2[orderG(graphs[i])];
        for(int u = 0 ; u < orderG(graphs[i]) ; u++ ){
            int numN = numberNeighbour(graphs[i],u);
            Z3_ast formulaAND3[2];
            formulaAND3[0] = getNodeVariable(ctx,i,j,pathLength,u);
            Z3_ast formulaOR[numN];
            int id = 0;
            for(int v = 0 ; v < orderG(graphs[i]) ; v++){
                if(isEdge(graphs[i],u,v)){
                    formulaOR[id] = getNodeVariable(ctx,i,j+1,pathLength,v);
                    id ++;
                }
            }
            formulaAND3[1] = Z3_mk_or(ctx,numN,formulaOR);
            formulaAND2[u] = Z3_mk_and(ctx,2,formulaAND3);
        }
        formulaAND1[j] = Z3_mk_or(ctx,orderG(graphs[i]),formulaAND2);
    }
    return Z3_mk_and(ctx,pathLength,formulaAND1);
}
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

// Génère une formule SAT satisfaisable si et seulement si tous les graphes de graphs contiennent un chemin acceptant de longueur pathLength.

Z3_ast graphsToPathFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength){
    Z3_ast formulaAND[pathLength];
    Z3_ast* formulaLittleAND = (Z3_ast*)malloc(sizeof(Z3_ast)*6); //nombre magique
    if(formulaLittleAND == NULL){
        printf("Not enough memory to allocate formulaLittleAND in graphsToPathFormula\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0 ; i < numGraphs ; i++){
        formulaLittleAND[0] = graphToPhi1Formula(ctx, graphs, i, pathLength);
        //printf("Formula 1 %s created.\n",Z3_ast_to_string(ctx,formulaLittleAND[0]));
        //printf("F1 = %d\n",isSatisfiable(isFormulaSat(ctx,formulaLittleAND[0])));
        //Z3_model model = getModelFromSatFormula(ctx,absurd);
        formulaLittleAND[1] = graphToPhi2Formula(ctx, graphs, i, pathLength);
        //printf("Formula 2 %s created.\n",Z3_ast_to_string(ctx,formulaLittleAND[1]));
        //printf("F2 = %d\n",isSatisfiable(isFormulaSat(ctx,formulaLittleAND[1])));
        formulaLittleAND[2] = graphToPhi3Formula(ctx, graphs, i, pathLength);
        //printf("Formula 3 %s created.\n",Z3_ast_to_string(ctx,formulaLittleAND[2]));
        //printf("F3 = %d\n",isSatisfiable(isFormulaSat(ctx,formulaLittleAND[2])));
        formulaLittleAND[3] = graphToPhi4Formula(ctx, graphs, i, pathLength);
        //printf("Formula 4 %s created.\n",Z3_ast_to_string(ctx,formulaLittleAND[3]));
        //printf("F4 = %d\n",isSatisfiable(isFormulaSat(ctx,formulaLittleAND[3])));
        formulaLittleAND[4] = graphToPhi5Formula(ctx, graphs, i, pathLength);
        //printf("Formula 5 %s created.\n",Z3_ast_to_string(ctx,formulaLittleAND[4]));
        //printf("F5 = %d\n",isSatisfiable(isFormulaSat(ctx,formulaLittleAND[4])));
        formulaLittleAND[5] = graphToPhi6Formula(ctx, graphs, i, pathLength);
        //formulaLittleAND[5] = phi6test(ctx, graphs, i, pathLength);
        //printf("Formula 6 %s created.\n",Z3_ast_to_string(ctx,formulaLittleAND[5]));
        //printf("F6 = %d\n",isSatisfiable(isFormulaSat(ctx,formulaLittleAND[5])));
        formulaAND[i] = Z3_mk_and(ctx,6,formulaLittleAND);
        //printf("Formule finale = %d\n",isSatisfiable(isFormulaSat(ctx,formulaAND[i])));
    }
    free(formulaLittleAND);
    return Z3_mk_and(ctx,numGraphs,formulaAND);
}


// Génère une formule SAT satisfaisable si et seulement si tous les graphes de graphs contiennent un chemin acceptant de longueur commune.

Z3_ast graphsToFullFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs){
    int commonLength = orderG(graphs[0]); //vérifier si ça ne commence pas à l'indice 1
    for(int i = 1; i > numGraphs; i++){
        commonLength = min(commonLength, orderG(graphs[i]));
    }
    Z3_ast formulaOR[commonLength-1];
    for(int l = 1; l < commonLength; l++){
        formulaOR[l-1] = graphsToPathFormula(ctx, graphs, numGraphs, l);
        //printf("Formula k = %d, %s created.\n",l,Z3_ast_to_string(ctx,formulaOR[l-1]));
        //printf("F = %d\n",isSatisfiable(isFormulaSat(ctx,formulaOR[l-1])));
    }
    return Z3_mk_or(ctx,commonLength-1,formulaOR);
}


// Obtient la longueur de la solution d'un modèle donné.

int getSolutionLengthFromModel(Z3_context ctx, Z3_model model, Graph *graphs);



// Affiche les chemins de longueur pathLength pour tous les graphes décrits dans model.

void printPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength);



// Crée le fichier représentant la solution du problème décrit par model, ou ("result-l%d.dot,pathLength") si name == NULL

void createDotFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength, char* name);

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