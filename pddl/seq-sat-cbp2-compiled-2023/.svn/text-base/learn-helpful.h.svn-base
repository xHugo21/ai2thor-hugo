/*
** matcher.h
** 
** Made by Raquel Fuentetaja & Tomas de la Rosa
** 
** Started on Mon Oct 20 14:19:48 2008 Raquel
*/

#ifndef   	LEARNHELPUL_H_
# define   	LEARHELPFUL_H_


//TOMAS: No se si faltarán mas
#include <strings.h>
#include <ctype.h>
#include "ff.h"


/*-------------some constants-------------------------------*/
#define ROLLER_DIR "roller/"


#define MAX_SOLSFORLEARNING 20

#define STR_PRED_TARGET_GOAL "target_goal"
#define STR_PRED_STATIC_FACT "static_fact"

#define PRED_SELECTED 0
#define PRED_CANDIDATE 1
#define PRED_ARGS_SELECTED 2
#define PRED_ARGS_REJECTED 3



/* ----------- externals -----------------------------------------------------------------*/


/* ----------- structures ------------------------------------------------------------------*/



/*Functions*/

void execute_learner_roller(void);
void tilde_gaction_predicate(FILE *ex_file, char *example_id, 
			     char *exprob_id,  int action, int predicate_type);
void write_tilde_fact(FILE *ex_file, Fact *f, char *pred_prefix);
void siblings_episode(FILE *ex_file, SolPath *iSolPath);
void siblings_args_episode(FILE *ex_file, SolPath *iSolPath, int operator);
void write_comma_args(FILE *ex_file, int action);
void write_tilde_static_fact(FILE *ex_file, Fact *f, char *pred_prefix);

#endif
