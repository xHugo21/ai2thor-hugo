/*
** learn-ltl.h
** 
** Made by Tomas de la Rosa** 
** Started on Oct 27 2009 
*/

#ifndef   	LEARNLTL_H_
# define   	LEARNLTL_H_


//TOMAS: No se si faltarán mas
#include <strings.h>
#include <ctype.h>
#include "ff.h"


/*-------------some constants-------------------------------*/
#define MAX_SOLSFORLEARNING_LTL 20

#define LTL_DIR "ltl/"

#define PRED_LTL_TARGET_GOAL "targetgoal"
#define PRED_LTL_ACHIEVED_GOAL "achievedgoal"
#define PRED_LTL_GOAL "goal"
#define PRED_LTL_NEXT "next"

/* ----------- externals -----------------------------------------------------------------*/


/* ----------- structures ------------------------------------------------------------------*/



/*Functions*/
void write_fact_prolog(FILE *ex_file, Fact *f, char *pred_prefix);
void write_ltl_episode(FILE *ex_file, BfsNode *node, int class);
void ltl_traverse_tree(FILE *example_file, BfsNode *node);
void execute_learner_ltl(char *path);


#endif
