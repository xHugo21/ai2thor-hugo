/*
** matcher.h
** 
** Made by Raquel Fuentetaja & Tomas de la Rosa
** 
** Started on Mon Oct 20 14:19:48 2008 Raquel
*/

#ifndef   	LEARNER_H_
# define   	LEARNER_H_


#include <strings.h>
#include <ctype.h>
#include "ff.h"

#define PREFERENCE_PARALELISM 1
#define PREFERENCE_DIFFICULTY 2


char * toStr_prolog( char *string);
char * toStr_lower( char *string);
void add_found_solution( BfsNode *last );
void print_solution_path(Solution *sol);
Bool is_target_goal(int fact_index, State *S);

float rank_solution_preference(Solution *iSol, int Type);
int compare_ranked_sols( const void* a, const void* b );
void compute_static_literals();

#endif /* _LEARNER_H */


