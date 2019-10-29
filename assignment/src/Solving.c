#ifndef COCA_SOLVING_H_
#define COCA_SOLVING_H_

#include "Graph.h"
#include <z3.h>
#include "Z3Tools.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// Génère une formule consistant en une variable représentant le fait que node du graphe number soit à la position position d'un chemin acceptant.

int max(int a , int b){
    if(a<b){
        return b;
    }
    return a;
}

Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node){
    char* name;
    name = strcat(name,"X_");

    int greater = max(number,max(position,max(k,node)));
    char str[(int)log10(greater)+1];
    
    sprintf(str, "%d", number); // à factoriser
    name = strcat(name,str);
    sprintf(str, ",%d", position);
    name = strcat(name,str);
    sprintf(str, ",%d", k);
    name = strcat(name,str);
    sprintf(str, ",%d", node);
    name = strcat(name,str);

    return mk_bool_var(ctx, name); //son nom sera x_i,j,k,q
}


// Génère la sous-formule ɸ​1 pour le graph i. ( " Point de départ 's' ")

Z3_ast graphsToPhi1Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    for(int j = 0; j < orderG(graphs[i]); j++){
        if(isSource(graphs[i],j)){
            return getNodeVariable(ctx,i,0,pathLength,j);
        }
    }
}

// Génère la sous-formule ɸ​2 pour le graph i. ( " Point d'arrivée 't' ")

Z3_ast graphToPhi2Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    for(int j = 0; j < orderG(graphs[i]); j++){
        if(isTarget(graphs[i],j)){
            return getNodeVariable(ctx,i,pathLength,pathLength,j);
        }
    }
}

// Génère la sous-formule ɸ​3 pour le graph i. ( " Au moins 1 sommet par position ")

Z3_ast graphToPhi3Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    Z3_ast formulaAND[pathLength]; 
    for(int j = 0 ; j < pathLength ; j++ ){
        Z3_ast formulaOR[orderG(graphs[i])];
        for(int u = 0 ; u < orderG(graphs[i]) ; u++ ){
            formulaOR[u] = getNodeVariable(ctx,i,j,pathLength,u);
        }
        formulaAND[j] = Z3_mk_or(ctx,orderG(graphs[i]),formulaOR);
    }
    return Z3_mk_and(ctx,pathLength,formulaAND);
}

// Génère la sous-formule ɸ​4 pour le graph i. ( " Au plus 1 sommet par position ")

Z3_ast graphToPhi4Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    Z3_ast formulaAND1[pathLength];
    for(int j = 0 ; j < pathLength ; j++ ){
         Z3_ast formulaAND2[orderG(graphs[i])];
        for(int u = 0 ; u < orderG(graphs[i]) ; u++ ){
            Z3_ast formulaAND3[orderG(graphs[i])-1];
            for(int v = 0 ; v < orderG(graphs[i]) ; v++ ){
                Z3_ast formulaOR[2];
                if(v!=u){
                    formulaOR[0] = Z3_mk_not(ctx,getNodeVariable(ctx,i,j,pathLength,u));
                    formulaOR[1] = Z3_mk_not(ctx,getNodeVariable(ctx,i,j,pathLength,v));
                }
                formulaAND3[v] = Z3_mk_or(ctx,2,formulaOR);
            }
            formulaAND2[u] = Z3_mk_and(ctx,orderG(graphs[i])-1,formulaAND3);
        }
        formulaAND1[j] = Z3_mk_and(ctx,orderG(graphs[i]),formulaAND2);
    }
    return Z3_mk_and(ctx,pathLength,formulaAND1);
}

// Génère la sous-formule ɸ​5 pour le graph i. ( " Chaque spmmetapparaît au plus une fois ")

Z3_ast graphToPhi5Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength){
    Z3_ast formulaAND1[orderG(graphs[i])];
    for(int u = 0 ; u < orderG(graphs[i]) ; u++ ){
         Z3_ast formulaAND2[pathLength];
        for(int j = 0 ; j < pathLength ; j++){
            Z3_ast formulaAND3[pathLength-1];
            for(int j2 = 0 ; j2 < pathLength ; j2++ ){
                Z3_ast formulaOR[2];
                if(j2!=j){
                    formulaOR[0] = getNodeVariable(ctx,i,j,pathLength,u);
                    formulaOR[1] = getNodeVariable(ctx,i,j2,pathLength,u);
                }
                formulaAND3[j2] = Z3_mk_or(ctx,2,formulaOR);
            }
            formulaAND2[j] = Z3_mk_and(ctx,pathLength-1,formulaAND3);
        }
        formulaAND1[u] = Z3_mk_and(ctx,pathLength,formulaAND2);
    }
    return Z3_mk_and(ctx,orderG(graphs[i]),formulaAND1);
}

// Génère la sous-formule ɸ​6 pour le graph i. ( " Chemin de taille k continue")

Z3_ast graphToPhi6Formula(Z3_context ctx, Graph *graphs, unsigned int i, int pathLength);


// Génère une formule SAT satisfaisable si et seulement si tous les graphes de graphs contiennent un chemin acceptant de longueur pathLength.

Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);


// Génère une formule SAT satisfaisable si et seulement si tous les graphes de graphs contiennent un chemin acceptant de longueur commune.

// Z3_ast graphsToFullFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs) return graphsToPathFormula( ctx, graphs, numGraphs, commonLength);



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