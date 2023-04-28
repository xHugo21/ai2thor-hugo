/*
** Learning Linear Temporal Control Knowledge for TLPlan
* 
** Made by Tomás de la Rosa * 
** Started on  Oct 27 2009
*/


#include "ff.h"
#include <string.h>
#include "learner.h"
// #include "learn-helpful.h"
#include "learn-ltl.h"
#include "search.h"


int gltl_example_num = 0;
char file_tag[MAX_LENGTH];

Solution **sorted_ltlsols;
int num_ltl_learnSolutions;

/* =================================================================
Writing the Language Bias File
================================================================= */
/*
void write_icl_bias_file(){
}

void set_dlab_pred(){
}
*/

void write_fact_prolog(FILE *ex_file, Fact *f, char *pred_prefix){
//   Fact *f;
  int j;

//   f = &(grelevant_facts[fact_index]);
  if (pred_prefix==NULL)
    fprintf(ex_file, "\n%s",toStr_prolog(gpredicates[f->predicate]));
  else
    fprintf(ex_file, "\n%s_%s", pred_prefix, toStr_prolog(gpredicates[f->predicate]));
  
  //  printf("\nProbando string_prolog %s", toStr_prolog(gpredicates[f->predicate]));

  if (garity[f->predicate]>0){
    fprintf(ex_file, "(");

    for ( j=0; j<garity[f->predicate]; j++ ){
      if (j>0)
	fprintf(ex_file, ",");
      if ( f->args[j] >= 0 ) 
	fprintf(ex_file,"%s", toStr_prolog(gconstants[(f->args)[j]]));
    }
    fprintf(ex_file, ")");
  }
  fprintf(ex_file,".");
}


void write_ltl_static_facts(FILE *ex_file){
  int i, k;
  Fact *fact;
  Facts *flist;

   for(i = 1; i < gnum_orig_predicates; i++){
     if (!gis_added[i] && !gis_deleted[i]){
       for ( k = 0; k < gnum_initial_predicate[i]; k++ ){
	 fact = &(ginitial_predicate[i][k]) ; 
	 write_fact_prolog(ex_file, fact, NULL);
       }
     }
   }
   for (flist = gStatic_literals; flist !=NULL; flist=flist->next){
     write_fact_prolog(ex_file, flist->fact, NULL);
   }
}

void write_ltl_episode(FILE *ex_file, BfsNode *node, int class){
  char example_id[MAX_LENGTH];
  int i, igoal;
  BfsNode *parent;
  State next_S, parent_S;

//   if (node->tag < 0)
//     return;
	     
  parent = node->father;
  parent_S = parent->S;
  next_S = node->S;

  gltl_example_num++;
  sprintf(example_id, "%s_E%d", file_tag, gltl_example_num);

  fprintf(ex_file, "\n%% Example %d at depth %d", gltl_example_num, node->g);
  fprintf(ex_file, "\nbegin(model(%s)).", example_id);
  
  if (class==1)
    fprintf(ex_file,"\npositive.");
  else
    fprintf(ex_file, "\nnegative.");

  write_ltl_static_facts(ex_file);
  /*previous state*/
  for(i=0; i < parent->S.num_F; i++){
    write_fact_prolog(ex_file, &(grelevant_facts[parent->S.F[i]]), NULL);
  }
  
  /*problem goals*/
  for(i = 0; i < gnum_flogic_goal; i++){
    igoal = gflogic_goal[i]; 
    write_fact_prolog(ex_file, &(grelevant_facts[igoal]), PRED_LTL_GOAL);
  }


  /*target goals*/
  for(i = 0; i < gnum_flogic_goal; i++){
    igoal = gflogic_goal[i]; 

    if (is_target_goal(igoal, &parent_S))
      write_fact_prolog(ex_file, &(grelevant_facts[igoal]), PRED_LTL_TARGET_GOAL);
  }

    /*achieved goals*/
  for(i = 0; i < gnum_flogic_goal; i++){
    igoal = gflogic_goal[i]; 

    if (!is_target_goal(igoal, &parent_S))
      write_fact_prolog(ex_file, &(grelevant_facts[igoal]), PRED_LTL_ACHIEVED_GOAL);
  }


  /*next state*/
  for(i=0; i < node->S.num_F; i++){
    //falta if-change-able
    write_fact_prolog(ex_file, &(grelevant_facts[node->S.F[i]]), PRED_LTL_NEXT);
  }
    
  fprintf(ex_file, "\nend(model(%s)).\n", example_id);
	

  // Avoiding repeated examples 
//   node->tag = -1;

}
  



void ltl_traverse_tree(FILE *example_file, BfsNode *node){
  //char example_id[MAX_LENGTH];
  BfsNode *child;

  //  printf ("\nEntering traverse at depth %d", node->g);

  if (node->father!=NULL){
    if (node->tag != 0)
      write_ltl_episode(example_file, node,1);
    else
      write_ltl_episode(example_file, node,0);
  }
  if (node->tag ==1){
    for (child=node->children; child!=NULL; child=child->child_next)
      ltl_traverse_tree(example_file, child);
  }
}

void generate_ltl_episodes(FILE *example_file){
  Solution *iSol;
  SolPath *iSolPath;
  BfsNode *child, *parent;
  int i;
  int used_sols = MAX_SOLSFORLEARNING_LTL;
  
  if (used_sols > num_ltl_learnSolutions)
    used_sols = num_ltl_learnSolutions;


  //gRoller_example_num = 1;
  for(i=0; i< used_sols; i++){
    iSol = sorted_ltlsols[i];
    for (iSolPath = iSol->path; iSolPath!= NULL; iSolPath = iSolPath->next){
      parent = iSolPath->node->father;
      for(child = parent->children; child != NULL; child = child->child_next){
	if (child == iSolPath->node) {
	  write_ltl_episode(example_file, child, 1);
	}
	else if (child->tag == 0){
	  write_ltl_episode(example_file, child, 0);
	}
      }
    }
  }
}


 

/*I check if the state is equal to a visited one 
and if the acion to achieved it was the same */
Bool node_tag_visited( BfsNode *node)
{

  State *S;
  int sum, index;
  BfsHashEntry *h;
  
  S = &(node->S);
  
  sum = state_sum( S );
  index = sum & BFS_HASH_BITS;

  h = lbfs_hash_entry[index];
  for ( h = lbfs_hash_entry[index]; h; h = h->next ) {
    if ( h->sum != sum ) {
      continue;
    }
    if (h->bfs_node->op != node->op)
      continue;
        
    if ( same_state( &(h->bfs_node->S), S ) ) {
      return TRUE;
    }
  }

  return FALSE;

}



void ltl_tag_solution_tree(){
  Solution *iSol;
  SolPath *iSolPath;
  
  printf("\nTagging Solution Tree...");
  // The root node
  gSearch_tree->tag = 1;
  
  for (iSol = Sols_for_learning; iSol!= NULL; iSol=iSol->next){
    for (iSolPath = iSol->path; iSolPath!= NULL; iSolPath = iSolPath->next){
      if (iSol->last_node == iSolPath->node)
	iSolPath->node->tag = 2;
      else{
	if(iSolPath->node->tag==0){
	  if (node_tag_visited (iSolPath->node)){
	    iSolPath->node->tag = 3;
	  }
	  else{
	    iSolPath->node->tag = 1;
	    hash_bfs_node(iSolPath->node);
	  }
	}
      }
    }
  }
}

// Copied from learn-helpful
void select_solutions_for_learning_ltl(void){
  Solution *iSol, *top_ranked;
  int i;
  
  sorted_ltlsols = (Solution**) calloc(numSolutions, sizeof(Solution *));
					       
  for (iSol = Sols_for_learning,i=0; iSol!= NULL; iSol=iSol->next,i++){
    iSol->paralelism = rank_solution_preference(iSol,PREFERENCE_PARALELISM);
    iSol->difficulty = rank_solution_preference(iSol,PREFERENCE_DIFFICULTY);
    sorted_ltlsols[i] = iSol;
  }
  
//   printf("\n Previous Solution Ranking...");
//   for (i=0; i<numSolutions; i++){
//     printf("\n %d P:%.2f D:%.2f    ,L:%d", i, sorted_ltlsols[i]->paralelism, sorted_ltlsols[i]->difficulty, sorted_ltlsols[i]->path_len);
//  
//   }

  qsort(sorted_ltlsols, numSolutions, sizeof(Solution*), &compare_ranked_sols);
  top_ranked = sorted_ltlsols[0];
  
  num_ltl_learnSolutions = numSolutions; // if all of them have the same ranking

  for (i=0; i<numSolutions;i++){
    iSol = sorted_ltlsols[i];
    if (!(iSol->paralelism == top_ranked->paralelism &&
	  iSol->difficulty == top_ranked->difficulty)){
	  num_ltl_learnSolutions = i;
	  break;
    }
  }
  printf("\n Number of Solutions for learning: %d", num_ltl_learnSolutions);
}
    

void execute_learner_ltl(char *path)
{ 
  char filepath[MAX_LENGTH];
  FILE *example_file;


  // Check this out.  Only valid for problems called fom probsets/ dir and ending with ".pddl"
  //file_tag = (char*)calloc(strlen( gcmd_line.fct_file_name )-13, sizeof(char));
  strncpy(file_tag, toStr_prolog(&(gcmd_line.fct_file_name[9])), strlen( gcmd_line.fct_file_name )-14);

  printf("\n\nWriting Example File ...for %s tag %s", gcmd_line.fct_file_name, file_tag);
  
  ltl_tag_solution_tree();
  
  
  sprintf(filepath, "%s%s%s", gcmd_line.path, LTL_DIR, "ltl-episodes.kb");
  example_file = fopen(filepath, "a");
  fprintf(example_file, "%% TRAINING EXAMPLES FROM FF-LEARNER (LTL)\n");

  ltl_traverse_tree(example_file, gSearch_tree);
  
//   select_solutions_for_learning_ltl();
//   generate_ltl_episodes(example_file);
  
  printf("\nDone.");
  fclose(example_file);
}

