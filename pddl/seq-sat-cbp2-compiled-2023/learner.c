/*********************************************************************
 *
 * File: learner.c
 *
 * Description: Metric-ff Extension for handling the generation of 
 * training examples for different planning-learners
 *
 * Author: Tomas de la Rosa 2009
 *
 * 
 *********************************************************************/ 

#include "ff.h"
#include "learner.h"
#include "memory.h"
#include "output.h"
#include "relax.h"

char prolog_string[MAX_LENGTH];
char temp_string[MAX_LENGTH];


char * toStr_prolog( char *string)
{
  int i; 
  
  for (i=0; string[i] !='\0'; i++){
    if (string[i] == '-') 
      prolog_string[i] = '_';
    else
      prolog_string[i] = tolower(string[i]);
  }
  prolog_string[i] = '\0';
  return prolog_string;
}

char * toStr_lower( char *string)
{
  int i; 
  
  for (i=0; string[i] !='\0'; i++){
    temp_string[i] = tolower(string[i]);
  }
  temp_string[i] = '\0';
  return temp_string;
}




Bool is_target_goal(int fact_index, State *S)
{
   int i;

   for ( i = 0; i < S->num_F; i++ ) {
     if (fact_index == S->F[i])
       return FALSE;
   }
   
   return TRUE;
}



SolPath *new_SolPath(BfsNode *inode){
  SolPath *result = (SolPath *) calloc(1, sizeof(SolPath));
  result->node = inode;
  result->next = NULL;
  
  return result;
}


Solution *new_Solution(BfsNode *last){
  Solution *isol = (Solution *) calloc(1, sizeof(Solution));
  SolPath *isol_path, *ahead;
  BfsNode *inode;
  int i;
 
  //printf("\n Solution trace");
  ahead = NULL;
  isol->last_node = last;
  for (inode = last, i=1; inode->op != -1; inode = inode->father, i++){
    //   printf("\n NumEff %d", gop_conn[inode->op].num_E);
    isol_path = new_SolPath(inode);
    isol_path->next = ahead;
    ahead = isol_path;
  }
  
  isol->path = isol_path;
  isol->path_len = i-1;
  isol->paralelism = 1.0;
  isol->difficulty = 1.0; 
  isol->next = NULL; 

  return isol;
}


void add_found_solution( BfsNode *last)
{
  Solution *sol;

  sol = new_Solution(last);
  
  if (Sols_for_learning==NULL) 
    Sols_for_learning = sol;
  else if (Sols_for_learning->path_len > sol->path_len){
    Sols_for_learning = sol;
    numSolutions = 0;
    printf("\n Restarting Solution List ....previous solution discarded!");
  }
  else{
    sol->next = Sols_for_learning;
    Sols_for_learning = sol;
  }
  
  numSolutions++;
  
}

void print_solution_path(Solution *sol){
  SolPath *iNodePath;
 
  for(iNodePath=sol->path;iNodePath != NULL; iNodePath = iNodePath->next){
    printf("\n ");
    print_op_name(iNodePath->node->op);
  }
}


//The number of siblings that belongs to a best-cost solution.
float rank_node_paralelism(BfsNode *node){
  int good_siblings = 0;
  BfsNode *i_sibling;

  for (i_sibling = node->father->children; i_sibling != NULL; i_sibling = i_sibling->child_next){
    if (i_sibling->tag > 0)
      good_siblings++;
  }
  return good_siblings;
}

float rank_node_difficulty(BfsNode *node){
  int ft, i;
  int min = INFINITY;

  for (i=0; i < gef_conn[node->op].num_A; i++)
    {
      ft = gef_conn[node->op].A[i];
      
      if (LESS(gft_conn[ft].num_A, min))
	{
	  min = gft_conn[ft].num_A;
	}
      
    }
  
    return 1.0/min;
}


float rank_solution_preference(Solution *iSol, int Type){
  float node_preference, rank_value; 
  int step;
  SolPath *iSolPath;
  rank_value = 0;

  for (iSolPath = iSol->path, step=0; iSolPath!= NULL; iSolPath = iSolPath->next, step++){
    if (Type==PREFERENCE_PARALELISM)
      node_preference = rank_node_paralelism(iSolPath->node);
    else 
      node_preference = rank_node_difficulty(iSolPath->node);
    rank_value = rank_value + (node_preference * ((1.0 * (iSol->path_len - step)) / iSol->path_len));
  }  
  return rank_value;

}


int compare_ranked_sols( const void* a, const void* b ) {
  Solution ** isola = (Solution**) a; 
  Solution ** isolb = (Solution**) b; 

  if (LESS_EQ((*isola)->paralelism, (*isolb)->paralelism))
    {
      return 1;
    }
  
  if (FLOAT_EQUAL((*isola)->paralelism, (*isolb)->paralelism) &&
      LESS((*isola)->difficulty,(*isolb)->difficulty))
      return 1;
  else 
    return -1;
  
}

void print_static_literals(){

  Facts *f;
  
  printf("\nStatic Literals:");
  for ( f = gStatic_literals; f; f = f->next ) {
    printf("\n");
    print_Fact( f->fact );
  }


}

