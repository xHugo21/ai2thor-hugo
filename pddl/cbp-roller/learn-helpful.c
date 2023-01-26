/*
** Learning Helpful context with TILDE
* 
** Made by Raquel Fuentetaja Pizán, Tomás de la Rosa & Sergio Jiménez 
** Login   <rfuentet@inf.uc3m.es>
** 
** Started on  Mon Oct 20 15:05:58 2008 Raquel
*/
  

#include "ff.h"
#include "relax.h"
#include <string.h>
#include "learner.h"
#include "learn-helpful.h"
#include "matcher.h"
#include "search.h"


/*Para ejecutar después de resolver un problema.
Hay que saber de alguna forma que exploró el árbol completo en un time bound. Preguntame de esto.
*/

Solution **sorted_sols;
int num_learnSolutions;
char roller_file_tag[MAX_LENGTH];
char str_example_id[MAX_LENGTH];
int gRoller_example_num;

char * fast_op_name(int index)
{
  Action *a = gop_conn[index].action; 
  return a->name;
}

void set_example_id(void){
  sprintf(str_example_id, "%s_E%d",roller_file_tag, gRoller_example_num);
}




void select_solutions_for_learning(void){
  Solution *iSol, *top_ranked;
  int i;
  
  sorted_sols = (Solution**) calloc(numSolutions, sizeof(Solution *));
					       
  for (iSol = Sols_for_learning,i=0; iSol!= NULL; iSol=iSol->next,i++){
    iSol->paralelism = rank_solution_preference(iSol,PREFERENCE_PARALELISM);
    iSol->difficulty = rank_solution_preference(iSol,PREFERENCE_DIFFICULTY);
    sorted_sols[i] = iSol;
  }
  
  printf("\n Previous Solution Ranking...");
  for (i=0; i<numSolutions; i++){
    printf("\n %d P:%.2f D:%.2f    ,L:%d", i, sorted_sols[i]->paralelism, sorted_sols[i]->difficulty, sorted_sols[i]->path_len);
 
  }

  qsort(sorted_sols, numSolutions, sizeof(Solution*), &compare_ranked_sols);
  top_ranked = sorted_sols[0];
  
  num_learnSolutions = numSolutions; // if all of them have the same ranking

  for (i=0; i<numSolutions;i++){
    iSol = sorted_sols[i];
    if (!(iSol->paralelism == top_ranked->paralelism &&
	  iSol->difficulty == top_ranked->difficulty)){
	  num_learnSolutions = i;
	  break;
    }
  }
}

void generate_episodes_static_info(FILE *ex_file){
  int i, k;
  Fact *fact;
  Facts *flist;

  fprintf(ex_file, "\n%% Static Predicates of problem \n");

  for(i = 1; i < gnum_orig_predicates; i++){
    if (!gis_added[i] && !gis_deleted[i]){
      for ( k = 0; k < gnum_initial_predicate[i]; k++ ){
	 fact = &(ginitial_predicate[i][k]); 
	 write_tilde_static_fact(ex_file, fact, STR_PRED_STATIC_FACT);
       }
     }
   }
  for (flist = gStatic_literals; flist !=NULL; flist=flist->next){
    write_tilde_static_fact(ex_file, flist->fact, STR_PRED_STATIC_FACT);
  }
  fprintf(ex_file, "\n");
}


void generate_siblings_episodes(void){
  char filepath[MAX_LENGTH];
  FILE *example_file;  
  Solution *iSol;
  SolPath *iSolPath;
  int i;
  int used_sols = MAX_SOLSFORLEARNING;

  
  if (used_sols > num_learnSolutions)
    used_sols = num_learnSolutions;

  gRoller_example_num = 1;

  strncpy(roller_file_tag, toStr_prolog(&(gcmd_line.fct_file_name[9])), strlen( gcmd_line.fct_file_name )-14);
  sprintf(filepath, "%s%s%s", gcmd_line.path, ROLLER_DIR, "siblings-episodes/siblings-episodes.kb");
  example_file = fopen(filepath, "a");
  fprintf(example_file, "\n %% TRAINING EXAMPLES FROM FF-LEARNER (ROLLER)\n");

  if (domain_with_static_facts)  
    generate_episodes_static_info(example_file);
  
  for(i=0; i< used_sols; i++){
    iSol = sorted_sols[i];
    for (iSolPath = iSol->path; iSolPath!= NULL; iSolPath = iSolPath->next){
      set_example_id();
      siblings_episode(example_file, iSolPath); 
      gRoller_example_num++;

    }
  }
  fclose(example_file);


}


void generate_action_episodes(int op_index){
  char filepath[MAX_LENGTH];
  FILE *example_file;  
  int i;
  Solution *iSol;
  SolPath *iSolPath;
  int used_sols = MAX_SOLSFORLEARNING;
  
  if (used_sols > num_learnSolutions)
    used_sols = num_learnSolutions;

  gRoller_example_num = 1;
  strncpy(roller_file_tag, toStr_prolog(&(gcmd_line.fct_file_name[9])), strlen( gcmd_line.fct_file_name )-14);
  sprintf(filepath, "%s%s%s-episodes/%s-episodes.kb", gcmd_line.path, ROLLER_DIR,toStr_lower(goperators[op_index]->name),
	  toStr_lower(goperators[op_index]->name));
  example_file = fopen(filepath, "a");
  fprintf(example_file, "%% TRAINING EXAMPLES FROM FF-LEARNER (ROLLER)\n");
  
  if (domain_with_static_facts)
    generate_episodes_static_info(example_file);
  
  for(i=0; i<used_sols; i++){
    iSol = sorted_sols[i];
    for (iSolPath = iSol->path; iSolPath!= NULL; iSolPath = iSolPath->next){
      if (op_index == gop_conn[iSolPath->node->op].action->name_index) 
	set_example_id();
	siblings_args_episode(example_file, iSolPath, op_index); 
	gRoller_example_num++;
    }
  }
  fclose(example_file);
}


void write_tilde_static_fact(FILE *ex_file, Fact *f, char *pred_prefix){

  int j;


  if (pred_prefix==NULL)
    fprintf(ex_file, "\n%s",toStr_prolog(gpredicates[f->predicate]));
  else
    fprintf(ex_file, "\n%s_%s", pred_prefix, toStr_prolog(gpredicates[f->predicate]));
  
  fprintf(ex_file, "(%s", roller_file_tag);

  for ( j=0; j<garity[f->predicate]; j++ ){
    if ( f->args[j] >= 0 ) 
      fprintf(ex_file,",%s", toStr_prolog(gconstants[(f->args)[j]]));
  }
  fprintf(ex_file, ").");
}



void write_tilde_fact(FILE *ex_file, Fact *f, char *pred_prefix){

  int j;

  if (pred_prefix==NULL)
    fprintf(ex_file, "\n%s",toStr_prolog(gpredicates[f->predicate]));
  else
    fprintf(ex_file, "\n%s_%s", pred_prefix, toStr_prolog(gpredicates[f->predicate]));
  
  if (domain_with_static_facts) 
    fprintf(ex_file, "(%s,%s", str_example_id, roller_file_tag);
  else
    fprintf(ex_file, "(%s", str_example_id);

//   fprintf(ex_file, "(%s", ex_id);
//   if (extra_id!=NULL)
//     fprintf(ex_file, ",%s", extra_id);

  for ( j=0; j<garity[f->predicate]; j++ ){
    if ( f->args[j] >= 0 ) 
      fprintf(ex_file,",%s", toStr_prolog(gconstants[(f->args)[j]]));
  }
  fprintf(ex_file, ").");
}



Bool Node_is_helpful(BfsNode *node){
  BfsNode *father = node->father;
  int i;

  for (i=0;i<father->num_H;i++){
    if (node->op == father->H[i])
      return TRUE;
  }
  return FALSE;

}




void siblings_episode(FILE *ex_file, SolPath *iSolPath){
  BfsNode *iNode = iSolPath->node;
  BfsNode *child;
  int applied_action = iNode->op;
  State parent_S = iNode->father->S;
  int i, igoal;
  
  fprintf(ex_file, "\n\n%% Example %s_E%d",roller_file_tag, gRoller_example_num);
  
  tilde_gaction_predicate(ex_file, str_example_id, roller_file_tag, applied_action, PRED_SELECTED);

  for(child = iNode->father->children; child != NULL; child = child->child_next){
    if (Node_is_helpful(child))
	tilde_gaction_predicate(ex_file, str_example_id, roller_file_tag, child->op, PRED_CANDIDATE);
  }

    /*target goals*/
  for(i = 0; i < gnum_flogic_goal; i++){
    igoal = gflogic_goal[i]; 

    if (is_target_goal(igoal, &parent_S))
      write_tilde_fact(ex_file, &(grelevant_facts[igoal]), STR_PRED_TARGET_GOAL);
  }
  fprintf(ex_file, "\n");
}

void siblings_args_episode(FILE *ex_file, SolPath *iSolPath, int operator){
  BfsNode *iNode = iSolPath->node;
  BfsNode *sibling;
  State parent_S = iNode->father->S;
  int i,igoal;
  
  fprintf(ex_file, "\n%% Example %s_E%d",roller_file_tag, gRoller_example_num);
  
  for(sibling = iNode->father->children; sibling != NULL; sibling = sibling->child_next){
    if(operator == gop_conn[sibling->op].action->name_index){
      if (sibling->tag==TRUE)
	tilde_gaction_predicate(ex_file, str_example_id, roller_file_tag, sibling->op, PRED_ARGS_SELECTED);
      else
	tilde_gaction_predicate(ex_file, str_example_id, roller_file_tag, sibling->op, PRED_ARGS_REJECTED);
    }
  }

  for(sibling = iNode->father->children; sibling != NULL; sibling = sibling->child_next){
    if (Node_is_helpful(sibling))
      tilde_gaction_predicate(ex_file, str_example_id, roller_file_tag, sibling->op, PRED_CANDIDATE);
  }
  
  /*target goals*/
  for(i = 0; i < gnum_flogic_goal; i++){
    igoal = gflogic_goal[i]; 
    
    if (is_target_goal(igoal, &parent_S))
      write_tilde_fact(ex_file, &(grelevant_facts[igoal]), STR_PRED_TARGET_GOAL);
  }
  fprintf(ex_file, "\n");
}




void tilde_gaction_predicate(FILE *ex_file, char *example_id, 
                              char *exprob_id,  int action, int predicate_type){
  char example_string[MAX_LENGTH];

  if (domain_with_static_facts) 
    sprintf(example_string, "%s_E%d,%s", roller_file_tag, gRoller_example_num, roller_file_tag);
  else
    sprintf(example_string, "%s_E%d", roller_file_tag, gRoller_example_num);

  switch(predicate_type){
  case PRED_SELECTED:
    fprintf(ex_file, "\nselected(%s,%s",example_string, toStr_prolog(fast_op_name(action)));
    break;
  case PRED_CANDIDATE:
    fprintf(ex_file, "\ncandidate_%s(%s", toStr_prolog(fast_op_name(action)), example_string);
    write_comma_args(ex_file, action);
    break;
  case PRED_ARGS_SELECTED:
    fprintf(ex_file, "\nselected_%s(%s",toStr_prolog(fast_op_name(action)), example_string);
    write_comma_args(ex_file, action);
    fprintf(ex_file, ",selected");
     break;
  case PRED_ARGS_REJECTED:  
    fprintf(ex_file, "\nselected_%s(%s",toStr_prolog(fast_op_name(action)), example_string);
    write_comma_args(ex_file, action);
    fprintf(ex_file, ",rejected");
    break;
}
  fprintf(ex_file,").");

} 

void write_comma_args(FILE *ex_file, int action){
  int i;
  Action *a = gop_conn[action].action;

  for ( i = 0; i < a->num_name_vars; i++ ) 
    fprintf(ex_file, ",%s", toStr_prolog(gconstants[a->name_inst_table[i]]));
}


/*Cada nodo debe tener un campo tag para ponerle el tipo de nodo
  dentro de este computo.  PONEMOS A TRUE los que pertenecen a una solución en el search tree*/
void tag_solution_tree(){
  Solution *iSol;
  SolPath *iSolPath;
  
  printf("\nTagging Solution Tree...");
  // The root node
  
  for (iSol = Sols_for_learning; iSol!= NULL; iSol=iSol->next){
    for (iSolPath = iSol->path; iSolPath!= NULL; iSolPath = iSolPath->next){
      if(iSolPath->node->tag==0){
	iSolPath->node->tag = 1;
      }
  }
  }
}



void execute_learner_roller()
{
  int i;
  
  check_static_facts();
  tag_solution_tree();
  fill_general_action_indexes();
  select_solutions_for_learning();
  
  printf("\n Solution Ranking...");
  for (i=0; i<numSolutions; i++){
    printf("\n %d P:%.2f D:%.2f    ,L:%d", i, sorted_sols[i]->paralelism, sorted_sols[i]->difficulty, sorted_sols[i]->path_len);
 
  }

  generate_siblings_episodes();

  for(i=0; i<gnum_operators;i++){
    generate_action_episodes(i); 
  }
}
