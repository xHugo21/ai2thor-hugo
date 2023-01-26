/*
** matcher.h
** 
** Made by Raquel Fuentetaja Pizán & Sergio Jiménez Celorrio
** 
** Started on Mon Oct 20 14:19:48 2008 Raquel
*/

#ifndef   	MATCHER_H_
# define   	MATCHER_H_


#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include "ff.h"

/* maximum number of different variables in the learned tree */
#define MAX_LT_VARS 1000 
#define MAX_LT_ACTIONS 1000 

#define NODETYPE_TEST 0

#define NODETYPE_LEAF -1
#define NODETYPE_TAG -2

#define PREDICATETYPE_CANDIDATE 0
#define PREDICATETYPE_TARGET 1
#define PREDICATETYPE_STATIC 2
#define PREDICATETYPE_EXECUTED 3



/* ----------- externals -----------------------------------------------------------------*/

/* helpful actions */
extern int *gH;

/* array of actions */
extern OpConn *gop_conn;
extern int gnum_op_conn;

/* array of action predicates */
extern Operator_pointer goperators[MAX_OPERATORS];
extern int gnum_operators;

/* predicate symbols*/
extern Token gpredicates[MAX_PREDICATES];
extern int gnum_predicates;

/* logic goals */
extern int *gflogic_goal;
extern int gnum_flogic_goal;

/* relevant facts*/
extern Fact grelevant_facts[MAX_RELEVANT_FACTS];
extern Fact **ginitial_predicate;
extern  int *gnum_initial_predicate;

/* to extract static facts */

/* facts added by actions  */
extern Bool gis_added[MAX_PREDICATES];

/* facts deleted by actions  */
extern Bool gis_deleted[MAX_PREDICATES];

/* time */
extern float matching_time;


/* ----------- structures ------------------------------------------------------------------*/

/* for the global table of variables */
typedef struct _LTvar {

  char* name;
  int index; 


} LTvar;


/* for variables in each LTnode*/
typedef struct _LTnodevar
{
  int index;
  Bool free;

} LTnodevar;


/* proposal element */
typedef struct _LTproposal_element {
  char *action_type;
  float value;
  int op_index;
} LTproposal_element;


/* proposal of proposal LTnode*/
typedef struct _LTproposal {
  
  int nactions;
  LTproposal_element elements[MAX_OPERATORS];

  int op; /* for results */

} LTproposal;


/*for matches*/
typedef struct _LTmatch {

  int *bindings;
  int index; /* complete node match*/

  struct _LTmatch *next;
} LTmatch;


/* learned tree nodes */
typedef struct _LTnode {


  int type; /* 0: candidate, 1:target-goal, 2:static, 3: executed, -1:
	       finish, -2: tag */

  char *pred; /* if type == 0 or 3 --> action name , if type = 1 or 2
		 -->predicate name */

  int pred_index; /* if type == 1 or 2 -->predicate index. 
		     if type == 0 or 3-->action index.
                     It is computed automatically */   


  /* variables */
  int nvars;
  LTnodevar *vars;

  LTproposal *proposal; /* for type == -1 --> Leaf node */
  
  /* for matching : not assigned initially */
  LTmatch  *matches;
  
  /* father */
  struct _LTnode *father;

  /* sucessors */
  struct _LTnode *node_yes;
  struct _LTnode *node_no;
  
} LTnode;


/* ----------- global variables ---------------------------------------------------------------*/

LTvar LT_var_table[MAX_LT_VARS];
int LT_num_vars;


/* LT tree */
LTnode *LT_tree_head;

/* tag: selected...*/
LTnode *LT_tree_tag;

/* list of LT bindings trees*/
LTnode *LT_tree_bindings_head[MAX_OPERATORS];
int n_LTtree_bindings;

State *LT_current_state;
Action *LT_last_executed_action;

LTproposal *selected_proposal;



/* ----------- interface ----------------------------------------------------------------------*/

void initialize_matches(LTnode *root);

void free_matches(LTmatch *matches_root);

void free_bindings(int *bindings);

void copy_bindings(LTmatch *match, int *bindings);


/* main function for external calls */
void match_all(void);

LTnode* match(LTnode *root);

LTnode* match_bindings_tree(LTnode *root, int ha);


Bool match_candidate_ha(LTnode *root);

Bool match_target_goal(LTnode *root);

Bool match_static(LTnode *root);

Bool match_executed(LTnode *root);

Bool compute_matches(LTnode *root);

void fill_predicate_and_action_indexes(LTnode *root);

void fill_general_action_indexes(void);

Bool pending_goal(int fact_index);


/* functions to be executed externally before the matching */
void assign_LT_current_state(State *S);

void assign_LT_last_executed_action(int op);


/* functions to parse and load the TILDE tree */
void string_replace (char * string, const char *old, const char *new);

void parse_tilde_line (char *tilde_line);

LTnode * load_tilde_node (char * tilde_line, LTnode * father, Bool is_proposal_tree);

LTnode * load_tilde_tree(const char *tilde_out_file_name, Bool is_bindings_tree);

void load_all_tilde_trees(void);

int extract_operator_from_helpful(int ha);

void check_static_facts(void);

/* trace*/
void print_LTtype(int type);

void print_LTnode(LTnode *node);

void print_LTproposal(LTproposal *prop);

void print_LTtree(LTnode *root, int tab);

void print_LTspace(int tab);

void print_context (void);

void print_matches(LTnode *node);

void print_new_variables_values_LTnode(LTnode *node, int *bindings);

void print_partially_bounded_LTnode(LTnode *node, int *bindings);

/* evaluation */
void pruebaLT(void);


#endif 	    /* !MATCHER_H_ */
