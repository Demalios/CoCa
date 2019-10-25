/**
 * @file Z3Tools.h
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief This file contains functions to easily and transparently manipulate the Z3 SMT solver as a SAT solver without delving too much in the documentation.
 *        Note that most functions ask for a Z3_context. This is needed by Z3, but in this case, simply create a context at the beginning and pass it as an argument
 *        every time needed.
 * @version 1
 * @date 2019-08-01
 * 
 * @copyright Creative Commons
 * 
 */

#ifndef COCA_Z3TOOLS_H_
#define COCA_Z3TOOLS_H_

#include <z3.h>

/**
 * @brief Creates a basic Z3 context with basic config (sufficient for this project). Must be freed at end of program with Z3_del_context.
 * Crée un context Z3 basique avec une configuration basique (suffisante pour ce projet). Doit être libéré à la fin du programme avec Z3_del_context.
 * 
 * @return Z3_context, The created context. Le contexte créé.
 */
Z3_context makeContext(void);

/**
 * @brief Creates a formula containing a single variable whose name is given in parameter. Example mk_bool_var(ctx,"toto") will create the formula «toto». Each call with
 *        same name will produce the same formula (so it can be used to have the same variable in different formulae.)
 * Crée une formule contenant une seule variable dont le nom est donné en paramètre. Exemple : mk_bool_var(ctx,"toto") créera la formule "toto". Chaque appel avec le même nom
 * produira la même formule (pour qu'elle soit utilisée pour avoir la même variable dans différentes formules)
 * 
 * @param ctx, The context of the solver. Le contexte du solveur.
 * @param name, The name the variable. Le nom de la variable.
 * @return Z3_ast, The formula consisting in the variable. La formule consitant en la variable.
 */
Z3_ast mk_bool_var(Z3_context ctx, const char * name);

/**
 * @brief Tells if a formula is satisfiable, unsatisfiable, or cannot be decided.
 * Dis si une formule est satisfaisable, non-satisfaisable, ou si on ne peut pas le décider.
 * 
 * @param ctx, The context of the solver. Le contexte du solveur.
 * @param formula, The formula to check. La formule à vérifier.
 * @return Z3_lbool, Z3_L_FALSE if @p formula is unsatisfiable, Z3_L_TRUE if @p formula is satisfiable and Z3_L_UNDEF if the solver cannot decide if @p formula is
 *         satisfiable or not.
 * Renvoie Z3_L_FALSE si formula est non-satisfaisable, Z3_L_TRUE si formula est satisfaisable, et Z3_L_UNDEF si le solveur ne peut pas décider si formula est satisfaisable ou non.
 */
Z3_lbool isFormulaSat(Z3_context ctx, Z3_ast formula);

/**
 * @brief Returns an assignment of variables satisfying the formula if it is satisfiable. Exits the program if the formula is unsatisfiable.
 * Renvoie une affectation de variables satisfaisant la formule si elle est satisfaisable. Quitte le programme si la formule est non-satisfaisable.
 * 
 * @param ctx, The context of the solver. Le contexte du solveur.
 * @param formula, The formula to get a model from. La formule qui nous donne un modèle.
 * @return Z3_model, An assignment of variable satisfying @p formula. Une affectation de variables satisfaisant la formule (exemple : x1 = Vrai, x2 = Faux, X3 = Vrai).
 */
Z3_model getModelFromSatFormula(Z3_context ctx, Z3_ast formula);

/**
 * @brief Returns the truth value of the formula @p variable in the variable assignment @p model. Very usefull if @p variable is a formula containing a single variable.
 * Renvoie la valeur de vérité de la formule nommée variable dans l'affectation model. Très utile si variable est une formule contenant une seule variable.
 * 
 * @param ctx, The context of the solver. Le contexte du solveur.
 * @param model, A variable assignment. Une affectation de variables (exemple : x1 = Vrai, x2 = Faux, X3 = Vrai).
 * @param variable, A formula of which we want the truth value. Une formule dont on veut la valeur de vérité.
 * @return true, if @p variable is true in @p model. Renvoie vrai si variable est vraie dans model.
 * @return false, otherwise. Renvoie faux, sinon.
 */
bool valueOfVarInModel(Z3_context ctx, Z3_model model, Z3_ast variable);

#endif