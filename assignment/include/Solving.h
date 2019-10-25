/**
 * @file Solving.h
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief An implementation of the Coca project for 2019 year. Convert a set of graphs to a formula true if and only if they all contain a simple accepting path
 *        of a given length, and provides functions to display the result if the formula is satisfiable.
 * @version 1
 * @date 2019-08-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef COCA_SOLVING_H_
#define COCA_SOLVING_H_

#include "Graph.h"
#include <z3.h>

/**
 * @brief Generates a formula consisting of a variable representing the fact that @p node of graph number @p number is at position @p position of an accepting path.
 * Génère une formule consistant en une variable représentant le fait que node du graphe number soit à la position position d'un chemin acceptant.
 * 
 * @param ctx, The solver context. Le contexte du solveur.
 * @param number, The number of the graph. Le numéro du graphe.
 * @param position, The position in the path. La position du chemin.
 * @param k, The mysterious k from the subject of this assignment. Le k dans x_i,j,k,q.
 * @param node, The node identifier. L'identifiant du noeud.
 * @return Z3_ast, The formula. La formule.
 */
Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node);

/**
 * @brief Generates a SAT formula satisfiable if and only if all graphs of @p graphs contain an accepting path of length @p pathLength.
 * Génère une formule SAT satisfaisable si et seulement si tous les graphes de graphs contiennent un chemin acceptant de longueur pathLength.
 * 
 * @param ctx, The solver context. Le contexte du solveur.
 * @param graphs, An array of graphs. Une suite de graphes.
 * @param numGraphs, The number of graphs in @p graphs. Le numéro (indice ?) du graphe dans graphs.
 * @param pathLength, The length of the path to check. La longueur du chemin à vérifier.
 * @return Z3_ast, The formula. La formule.
 */
Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength);

/**
 * @brief Generates a SAT formula satisfiable if and only if all graphs of @p graphs contain an accepting path of common length.
 * Génère une formule SAT satisfaisable si et seulement si tous les graphes de graphs contiennent un chemin acceptant de longueur commune.
 * 
 * @param ctx, The solver context. Le contexte du solveur.
 * @param graphs, An array of graphs. Une suite de graphes.
 * @param numGraphs, The number of graphs in @p graphs. Le numéro (indice ?) du graphe dans graphs.
 * @return Z3_ast, The formula. La formule.
 */
Z3_ast graphsToFullFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs);

/**
 * @brief Gets the length of the solution from a given model.
 * Obtient la longueur de la solution d'un modèle donné.
 * 
 * @param ctx, The solver context. Le contexte du solveur.
 * @param model, A variable assignment. Une affectation de variables (exemple : x1 = Vrai, x2 = Faux, X3 = Vrai).
 * @param graphs, An array of graphs. Une liste de graphes.
 * @return int, The length of a common simple accepting path in all graphs from @p graphs. La longueur d'un chemin simple et acceptant dans tous les graphes de graphs.
 */ 
int getSolutionLengthFromModel(Z3_context ctx, Z3_model model, Graph *graphs);

/**
 * @brief Displays the paths of length @p pathLength of each graphs in @p graphs described by @p model.
 * Affiche les chemins de longueur pathLength pour tous les graphes décrits dans model.
 * 
 * @param ctx, The solver context. Le contexte du solveur.
 * @param model, A variable assignment. Une affectation de variables (exemple : x1 = Vrai, x2 = Faux, X3 = Vrai).
 * @param graphs, An array of graphs. Une liste de graphes.
 * @param numGraph, The number of graphs in @p graphs. Le nombre de graphes dans graphs.
 * @param pathLength, The length of path. La longueur du chemin.
 */
void printPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength);

/**
 * @brief Creates the file ("%s-l%d.dot",name,pathLength) representing the solution to the problem described by @p model, or ("result-l%d.dot,pathLength") if name is NULL.
 * Crée le fichier représentant la solution du problème décrit par model, ou ("result-l%d.dot,pathLength") si name == NULL
 * 
 * @param ctx, The solver context. Le contexte du solveur.
 * @param model, A variable assignment. Une affectation de variables (exemple : x1 = Vrai, x2 = Faux, X3 = Vrai).
 * @param graphs, An array of graphs. Une liste de graphes.
 * @param numGraph, The number of graphs in @p graphs. Le nombre de graphes dans graphs.
 * @param pathLength, The length of path. La longueur du chemin.
 * @param name, The name of the output file. Le nom du fichier en sortie.
 */
void createDotFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength, char* name);

#endif