/*
 * THIS SOURCE CODE IS SUPPLIED  ``AS IS'' WITHOUT WARRANTY OF ANY KIND, 
 * AND ITS AUTHOR AND THE JOURNAL OF ARTIFICIAL INTELLIGENCE RESEARCH 
 * (JAIR) AND JAIR'S PUBLISHERS AND DISTRIBUTORS, DISCLAIM ANY AND ALL 
 * WARRANTIES, INCLUDING BUT NOT LIMITED TO ANY IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND
 * ANY WARRANTIES OR NON INFRINGEMENT.  THE USER ASSUMES ALL LIABILITY AND
 * RESPONSIBILITY FOR USE OF THIS SOURCE CODE, AND NEITHER THE AUTHOR NOR
 * JAIR, NOR JAIR'S PUBLISHERS AND DISTRIBUTORS, WILL BE LIABLE FOR 
 * DAMAGES OF ANY KIND RESULTING FROM ITS USE.  Without limiting the 
 * generality of the foregoing, neither the author, nor JAIR, nor JAIR's
 * publishers and distributors, warrant that the Source Code will be 
 * error-free, will operate without interruption, or will meet the needs 
 * of the user.
 */











/*********************************************************************
 *
 * File: search.c
 *
 * Description: implementation of routines that search the state space
 *
 *              PDDL level 2 version.
 *
 *              here: basic best-first search, using helpful actions
 *
 *
 * Author: Joerg Hoffmann 2001
 *
 *         Last functions implemented by Raquel Fuentetaja 2008 to
 *         include more search algorithms
 * 
 *********************************************************************/ 







#include "ff.h"

#include "output.h"
#include "memory.h"

#include "expressions.h"

#include "relax.h"
#include "search.h"
#include "matcher.h"
#include "learner.h"
#include <malloc.h>






  




   
/*****************
 * LOCAL GLOBALS *
 *****************/




int nopen = 0;






/* search space for EHC
 */
EhcNode *lehc_space_head, *lehc_space_end, *lehc_current_start, *lehc_current_end;



/* memory (hash table) for states that are already members
 * of the breadth - first search space in EHC
 */
EhcHashEntry_pointer lehc_hash_entry[EHC_HASH_SIZE];
int lnum_ehc_hash_entry[EHC_HASH_SIZE];
int lchanged_ehc_entrys[EHC_HASH_SIZE];
int lnum_changed_ehc_entrys;
Bool lchanged_ehc_entry[EHC_HASH_SIZE];



/* memory (hash table) for states that are already 
 * encountered by current serial plan
 */
PlanHashEntry_pointer lplan_hash_entry[PLAN_HASH_SIZE];



/* search space
 */
BfsNode *lbfs_space_head, *lbfs_space_had;
BfsNode *lbfs_space_current_first;

/* helpful actions list
 */
BfsNode *lbfs_primary_current_head, *lbfs_primary_current_tail;

/* secondary non helpful actions list
 */
BfsNode *lbfs_secondary_current_head, *lbfs_secondary_current_tail;

/* lookahead intermediate list
 */
BfsNode *lbfs_lookahead_current_head, *lbfs_lookahead_current_tail;

/* memory (hash table) for states that are already members
 * of the best first search space
 */
BfsHashEntry_pointer lbfs_hash_entry[BFS_HASH_SIZE];



HA_h *gH_h_values;


/**********************/
/*  memory check */
/**********************/
/* raquel: I include this function to control memory used */

Bool memory_check(void)
{
 struct mallinfo info; 
 float used;

 info=mallinfo(); 
 used=(info.hblkhd + info.uordblks)/(1024*1024);
 /* max memory is aprox. 6000 MB  */
    if (used  > 6000)
   {
     fprintf(stdout,"\n\n out of memory: %f\n",used );
     return FALSE;
   } 
    return TRUE;  
}




/**********************/
/*  trace */
/**********************/


void show_collected_actions(int *actions, int gnum_actions)
{  
  int i;

   if ( gcmd_line.display_info == 128 ||  gcmd_line.display_info == 131 ) {
	printf("\ncollected actions: ");
	for ( i = 0; i < gnum_actions; i++ ) {
	  print_op_name( actions[i] );
	  printf(" cost: %f\n              ",gef_conn[actions[i]].cost+ gtt);
	}
      }

}
	  






/********************************
 * EHC FUNCTION, CALLED BY MAIN *
 ********************************/





Bool lH;










Bool do_enforced_hill_climbing( void )

{

  State S, S_;
  int i, h, h_;

  make_state( &S, gnum_ft_conn, gnum_fl_conn );
  make_state( &S_, gnum_ft_conn, gnum_fl_conn );

  /* initialize plan hash table, search space, search hash table
   */
  for ( i = 0; i < PLAN_HASH_SIZE; i++ ) {
    lplan_hash_entry[i] = NULL;
  }
  hash_plan_state( &ginitial_state, 0 );
  
  lehc_space_head = new_EhcNode();
  lehc_space_end = lehc_space_head;
  
  for ( i = 0; i < EHC_HASH_SIZE; i++ ) {
    lehc_hash_entry[i] = NULL;
    lnum_ehc_hash_entry[i] = 0;
    lchanged_ehc_entry[i] = FALSE;
  }
  lnum_changed_ehc_entrys = 0;
  
  /* start enforced Hill-climbing
   */
  lH = TRUE;

  source_to_dest( &S, &ginitial_state );

  h = get_1P_and_H( &S );

  if ( h == INFINITY ) {
    return FALSE;
  }
  if ( h == 0 ) {
    return TRUE;
  }  
  if ( gcmd_line.display_info ) {
    printf("\n\nCueing down from goal distance: %4d into depth ", h);
  }

  while ( h != 0 ) {
    if ( !search_for_better_state( &S, h, &S_, &h_ ) ) {
      printf(" --- pruning stopped --- ");
      get_1P_and_A( &S );
      lH = FALSE;
      if ( !search_for_better_state( &S, h, &S_, &h_ ) ) {
	return FALSE;
      }
      lH = TRUE;
      get_1P_and_H( &S_ );/* to set up gH info for new start state */
    }
    source_to_dest( &S, &S_ );
    h = h_;
    if ( gcmd_line.display_info ) {
      printf("\n                                %4d            ", h);
    }
  }

  return TRUE;

}













/*************************************************
 * FUNCTIONS FOR BREADTH FIRST SEARCH IN H SPACE *
 *************************************************/















Bool search_for_better_state( State *S, int h, State *S_, int *h_ )

{

  static Bool fc = TRUE;
  static State S__;

  int i, h__, depth = 0;
  EhcNode *tmp;

  if ( fc ) {
    make_state( &S__, gnum_ft_conn, gnum_fl_conn );
    fc = FALSE;
  }

  /* don't hash states, but search nodes.
   * this way, don't need to keep states twice in memory
   */
  tmp = new_EhcNode();
  source_to_dest( &(tmp->S), S);
  hash_ehc_node( tmp );

  lehc_current_end = lehc_space_head->next;
  if ( lH ) {
    for ( i = 0; i < gnum_H; i++ ) {
      if ( result_to_dest( &S__, S, gH[i] ) ) {
	add_to_ehc_space( &S__, gH[i], NULL );
      }
    }
  } else {
    for ( i = 0; i < gnum_A; i++ ) {
      if ( result_to_dest( &S__, S, gA[i] ) ) {
	add_to_ehc_space( &S__, gA[i], NULL );
      }
    }
  }

  lehc_current_start = lehc_space_head->next;

  while ( TRUE ) {  
    if ( lehc_current_start == lehc_current_end ) {
      reset_ehc_hash_entrys();
      free( tmp );
      return FALSE;
    }
    if ( lehc_current_start->depth > depth ) {
      depth = lehc_current_start->depth;
      if ( depth > gmax_search_depth ) {
	gmax_search_depth = depth;
      }
      if ( gcmd_line.display_info ) {
	printf("[%d]", depth);
	fflush( stdout );
      }
    }
    h__ = expand_first_node( h );
    if ( LESS( h__, h ) ) {
      break;
    }
  }

  reset_ehc_hash_entrys();
  free( tmp );

  extract_plan_fragment( S );

  source_to_dest( S_, &(lehc_current_start->S) );
  *h_ = h__;

  return TRUE;

}



void add_to_ehc_space( State *S, int op, EhcNode *father )

{

  /* see if superior state (in terms of goal reachability)
   * is already a part of this search space
   */
  if ( superior_ehc_state_hashed( S ) ) {
    return;
  }

  if ( !lehc_current_end ) {
    lehc_current_end = new_EhcNode();
    lehc_space_end->next = lehc_current_end;
    lehc_space_end = lehc_current_end;
  }

  source_to_dest( &(lehc_current_end->S), S );
  lehc_current_end->op = op;
  lehc_current_end->father = father;
  if ( !father ) {
    lehc_current_end->depth = 1;
  } else {
    lehc_current_end->depth = father->depth + 1;
  }

  hash_ehc_node( lehc_current_end );

  lehc_current_end = lehc_current_end->next;

}



int expand_first_node( int h )

{

  static Bool fc = TRUE;
  static State S_;

  int h_, i;

  if ( fc ) {
    make_state( &S_, gnum_ft_conn, gnum_fl_conn );
    fc = FALSE;
  }

  if ( lH ) {
    h_ = get_1P_and_H( &(lehc_current_start->S) );
  } else {
     h_ = get_1P_and_A( &(lehc_current_start->S) );
  }   

  if ( h_ == INFINITY ) {
    lehc_current_start = lehc_current_start->next;
    return h_;
  }

  if ( h_ < h ) {
    return h_;
  }

  if ( lH ) {
    for ( i = 0; i < gnum_H; i++ ) {
      if ( result_to_dest( &S_, &(lehc_current_start->S), gH[i] ) ) {
	add_to_ehc_space( &S_, gH[i], lehc_current_start );
      }
    }
  } else {
    for ( i = 0; i < gnum_A; i++ ) {
      if ( result_to_dest( &S_, &(lehc_current_start->S), gA[i] ) ) {
	add_to_ehc_space( &S_, gA[i], lehc_current_start );
      }
    }
  }
    
  lehc_current_start = lehc_current_start->next;

  return h_;

}




Bool do_enforced_hill_climbing_cost( void )

{

  State S, S_;
  int i, h, h_;
  float h_cost, h_cost_;

  make_state( &S, gnum_ft_conn, gnum_fl_conn );
  make_state( &S_, gnum_ft_conn, gnum_fl_conn );

  /* initialize plan hash table, search space, search hash table
   */
  for ( i = 0; i < PLAN_HASH_SIZE; i++ ) {
    lplan_hash_entry[i] = NULL;
  }
  hash_plan_state( &ginitial_state, 0 );
  
  lehc_space_head = new_EhcNode();
  lehc_space_end = lehc_space_head;
  
  for ( i = 0; i < EHC_HASH_SIZE; i++ ) {
    lehc_hash_entry[i] = NULL;
    lnum_ehc_hash_entry[i] = 0;
    lchanged_ehc_entry[i] = FALSE;
  }
  lnum_changed_ehc_entrys = 0;
  

  /* start enforced Hill-climbing
   */
  lH = TRUE;

  source_to_dest( &S, &ginitial_state );

  if (gcmd_line.display_info == 131) 
    print_State(S);

  h = get_1P_and_H( &S );

  /*
  gcmd_line.heuristic = 2;
  get_1P_and_H( &S );
  gcmd_line.heuristic = 1;
  gevaluated_states--;
  */

  if (!gcmd_line.hdiff)
    {
     qsort(gH, gnum_H, sizeof(int), compare_helpful_actions);
    }

  show_collected_actions(gH, gnum_H);

  if ( h == INFINITY ) {
    return FALSE;
  }
  if ( h == 0 ) {
    return TRUE;
  }  

  if ((gcmd_line.optimize && goptimization_established && gcmd_line.use_cost_heuristic) || gcmd_line.hdiff)
    h_cost =  gcost + h * gtt;
  else
    h_cost = (float) h;
 

  if ( gcmd_line.display_info ) {
    printf("\n\nCueing down from goal distance: %4d (%f) into depth ", h, h_cost);
  }

   /* raquel: trace */
   if ( gcmd_line.display_info == 131 ) {
     if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.heuristic ) {
       printf(" with heuristic cost: %f\n", h_cost);
     }
   }
   /* end raquel */

  while ( h != 0 ) {

    if (!memory_check())
      return FALSE;

    if ( !search_for_better_state_cost( &S, h, h_cost, &S_, &h_, &h_cost_ ) ) {
      printf(" --- pruning stopped --- ");
      get_1P_and_A( &S );
      if (!gcmd_line.hdiff)
	{
	  qsort(gA, gnum_A, sizeof(int), compare_helpful_actions);
	}
      show_collected_actions(gA, gnum_A);

      lH = FALSE;
      if ( !search_for_better_state_cost( &S, h, h_cost, &S_, &h_ , &h_cost_) ) {
	return FALSE;
      }
      lH = TRUE;
      
      get_1P_and_H( &S_ );/* to set up gH info for new start state */
      /*
      gcmd_line.heuristic = 2;
      get_1P_and_H( &S );
      gcmd_line.heuristic = 1;
      */

      if (!gcmd_line.hdiff)
	{
	  qsort(gH, gnum_H, sizeof(int), compare_helpful_actions);
	}
      show_collected_actions(gH, gnum_H);
    }
    source_to_dest( &S, &S_ );
    h = h_;
    h_cost = h_cost_;
    if ( gcmd_line.display_info ) {
      printf("\n                                %4d (%f)           ", h, h_cost);
    }
  }

  /* compute final total cost */
  compute_total_cost_from_metric(&S_);

  return TRUE;

}













/*************************************************
 * FUNCTIONS FOR BREADTH FIRST SEARCH IN H SPACE *
 *************************************************/















Bool search_for_better_state_cost( State *S, int h, float h_cost, State *S_, int *h_ , float *h_cost_)

{

  static Bool fc = TRUE;
  static State S__;

  int i, h__, depth = 0;
  float h_cost__= 0, g_cost__= 0 ;
  EhcNode *tmp;

  if ( fc ) {
    make_state( &S__, gnum_ft_conn, gnum_fl_conn );
    fc = FALSE;
  }

  /* don't hash states, but search nodes.
   * this way, don't need to keep states twice in memory
   */
  tmp = new_EhcNode();
  source_to_dest( &(tmp->S), S);
  hash_ehc_node( tmp );

  lehc_current_end = lehc_space_head->next;
  if ( lH ) {
    for ( i = 0; i < gnum_H; i++ ) {
      if ( result_to_dest( &S__, S, gH[i] ) ) {
	add_to_ehc_space_cost( &S__, gH[i], NULL );
      }
    }
  } else {
    for ( i = 0; i < gnum_A; i++ ) {
      if ( result_to_dest( &S__, S, gA[i] ) ) {
	add_to_ehc_space_cost( &S__, gA[i], NULL );
      }
    }
  }

  lehc_current_start = lehc_space_head->next;

  while ( TRUE ) {  
    if (!memory_check())
      return FALSE;

    if ( lehc_current_start == lehc_current_end ) {
      reset_ehc_hash_entrys();
      free( tmp );
      return FALSE;
    }
    if ( lehc_current_start->depth > depth ) {
      depth = lehc_current_start->depth;
      if ( depth > gmax_search_depth ) {
	gmax_search_depth = depth;
      }
      if ( gcmd_line.display_info ) {
	printf("[%d]", depth);
	fflush( stdout );
      }
    }
    h__ = expand_first_node_cost( h, h_cost, &h_cost__, &g_cost__);

    /* raquel trace */
    if ( gcmd_line.display_info == 131) {
      if (lehc_current_start!=NULL){
	printf(" \n\n --> new current start (%d) ",lehc_current_start->op); 
      if (lehc_current_start->op != -1) {
          print_op_name(lehc_current_start->op);
        }
      }
    }
    /*end raquel*/


    if ( LESS( gcmd_line.h_weight * h_cost__ + (gcmd_line.g_weight * g_cost__) , h_cost)
      || (FLOAT_EQUAL(gcmd_line.h_weight * h_cost__  + (gcmd_line.g_weight * g_cost__)  , h_cost) && h__ < h)) {
      break;
    }

  }



  reset_ehc_hash_entrys();
  free( tmp );

  extract_plan_fragment( S );

  source_to_dest( S_, &(lehc_current_start->S) );
  *h_ = h__;
  *h_cost_ = h_cost__;


  /* raquel trace */
  if ( gcmd_line.display_info == 131 ) {
      printf(" --> chosen (%d) ",lehc_current_start->op); 
      if (lehc_current_start->op != -1) {
          print_op_name(lehc_current_start->op);
	  if (lehc_current_start->father && lehc_current_start->father->op != -1) {
            printf(" father: ");
            print_op_name(lehc_current_start->father->op);
	  }

          print_State(lehc_current_start->S);
        }
    
     }
   /*end raquel*/


  return TRUE;

}
  



int expand_first_node_cost( int h, float h_cost, float *h_cost_, float *g_cost_ )

{

  static Bool fc = TRUE;
  static State S_;

  int h_, i;

  if ( fc ) {
    make_state( &S_, gnum_ft_conn, gnum_fl_conn );
    fc = FALSE;
  }

  if ( lH ) {
    h_ = get_1P_and_H( &(lehc_current_start->S) );
    /*
    gcmd_line.heuristic = 2;
    get_1P_and_H( &(lehc_current_start->S) );
    gevaluated_states--;
    gcmd_line.heuristic = 1;
    */
    if (!gcmd_line.hdiff)
      {
	qsort(gH, gnum_H, sizeof(int), compare_helpful_actions);
      }
    show_collected_actions(gH, gnum_H);
  } else {
     h_ = get_1P_and_A( &(lehc_current_start->S) );

     if (!gcmd_line.hdiff)
       {
	 qsort(gA, gnum_A, sizeof(int), compare_helpful_actions);
       }
     show_collected_actions(gA, gnum_A);
  }   

  if ( h_ == INFINITY ) {
    lehc_current_start = lehc_current_start->next;
    return h_;
  }

  if ((gcmd_line.optimize && goptimization_established && gcmd_line.use_cost_heuristic) || gcmd_line.hdiff)
    *h_cost_ =  gcost + h_ * gtt;
  else
    *h_cost_ = (float) h_;

  if ( gcmd_line.display_info == 131 || gcmd_line.display_info == 130 ) {
                  printf("\n------------------------------------------------");
                  printf("\n Evaluation of the state achieved by: ");
                  print_op_name(lehc_current_start->op);
		  if ( gcmd_line.optimize && goptimization_established ) {
		    printf("\n       cost heuristic: %f", *h_cost_);
		  }
                  printf("\n       heuristic: %d", h_);
                  printf("\n       depth: %d\n",lehc_current_start->depth);
 }


  *g_cost_ = lehc_current_start->float_gn;


  if ( LESS(gcmd_line.h_weight *  *h_cost_  + ((float) gcmd_line.g_weight * lehc_current_start->float_gn), h_cost) ||
       (FLOAT_EQUAL(gcmd_line.h_weight * *h_cost_  + ((float) gcmd_line.g_weight * lehc_current_start->float_gn), h_cost) && h_ < h)) {
    return h_;
  }

  if ( lH ) {
    for ( i = 0; i < gnum_H; i++ ) {
      if ( result_to_dest( &S_, &(lehc_current_start->S), gH[i] ) ) {
	add_to_ehc_space_cost( &S_, gH[i], lehc_current_start );
      }
    }
  } else {
    for ( i = 0; i < gnum_A; i++ ) {
      if ( result_to_dest( &S_, &(lehc_current_start->S), gA[i] ) ) {
	add_to_ehc_space_cost( &S_, gA[i], lehc_current_start );
      }
    }
  }
    
  lehc_current_start = lehc_current_start->next;

  return h_;

}




Bool do_enforced_hill_climbing_cost_hdiff( void )

{

  State S, S_;
  int i, h, h_;
  float h_cost, h_cost_;

  make_state( &S, gnum_ft_conn, gnum_fl_conn );
  make_state( &S_, gnum_ft_conn, gnum_fl_conn );

  /* initialize plan hash table, search space, search hash table
   */
  for ( i = 0; i < PLAN_HASH_SIZE; i++ ) {
    lplan_hash_entry[i] = NULL;
  }
  hash_plan_state( &ginitial_state, 0 );
  
  lehc_space_head = new_EhcNode();
  lehc_space_end = lehc_space_head;
  
  for ( i = 0; i < EHC_HASH_SIZE; i++ ) {
    lehc_hash_entry[i] = NULL;
    lnum_ehc_hash_entry[i] = 0;
    lchanged_ehc_entry[i] = FALSE;
  }
  lnum_changed_ehc_entrys = 0;
  

  /* start enforced Hill-climbing
   */
  lH = TRUE;

  source_to_dest( &S, &ginitial_state );

  if (gcmd_line.display_info == 131) 
    print_State(S);

  h = get_1P_and_H( &S );

  /*
  gcmd_line.heuristic = 2;
  get_1P_and_H( &S );
  gcmd_line.heuristic = 1;
  gevaluated_states--;
  */

  if (!gcmd_line.hdiff)
    {
     qsort(gH, gnum_H, sizeof(int), compare_helpful_actions);
    }
 
  
  show_collected_actions(gH, gnum_H);

  if ( h == INFINITY ) {
    return FALSE;
  }
  if ( h == 0 ) {
    return TRUE;
  }  

  if ((gcmd_line.optimize && goptimization_established && gcmd_line.use_cost_heuristic) || gcmd_line.hdiff)
    h_cost =  gcost + h * gtt;
  else
    h_cost = (float) h;
 

  if ( gcmd_line.display_info ) {
    printf("\n\nCueing down from goal distance: %4d (%f) into depth ", h, h_cost);
  }

   /* raquel: trace */
   if ( gcmd_line.display_info == 131 ) {
     if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.heuristic ) {
       printf(" with heuristic cost: %f\n", h_cost);
     }
   }
   /* end raquel */

  while ( h != 0 ) {

    if (!memory_check())
      return FALSE;

    if ( !search_for_better_state_cost_hdiff( &S, h, h_cost, &S_, &h_, &h_cost_ ) ) {
      printf(" --- pruning stopped --- ");
      get_1P_and_A( &S );
      
      if (!gcmd_line.hdiff)
	{
	  qsort(gA, gnum_A, sizeof(int), compare_helpful_actions);
	}
      show_collected_actions(gA, gnum_A);

      lH = FALSE;
      if ( !search_for_better_state_cost_hdiff( &S, h, h_cost, &S_, &h_ , &h_cost_) ) {
	return FALSE;
      }
      lH = TRUE;
      
      get_1P_and_H( &S_ );/* to set up gH info for new start state */
      /*
      gcmd_line.heuristic = 2;
      get_1P_and_H( &S );
      gcmd_line.heuristic = 1;
      */

      if (!gcmd_line.hdiff)
	{
	  qsort(gH, gnum_H, sizeof(int), compare_helpful_actions);
	}
      show_collected_actions(gH, gnum_H);
    }
    source_to_dest( &S, &S_ );
    h = h_;
    h_cost = h_cost_;
    if ( gcmd_line.display_info ) {
      printf("\n                                %4d (%f)           ", h, h_cost);
    }
  }

  /* compute final total cost */
  compute_total_cost_from_metric(&S_);

  return TRUE;

}













/*************************************************
 * FUNCTIONS FOR BREADTH FIRST SEARCH IN H SPACE *
 *************************************************/















Bool search_for_better_state_cost_hdiff( State *S, int h, float h_cost, State *S_, int *h_ , float *h_cost_)

{

  static Bool fc = TRUE;
  static State S__;
  EhcNode *lehc_first_level_end;
  Bool second_pass = FALSE;

  int i, h__, depth = 0;
  float h_cost__= 0, g_cost__= 0 ;
  EhcNode *tmp;

  if ( fc ) {
    make_state( &S__, gnum_ft_conn, gnum_fl_conn );
    fc = FALSE;
  }

  /* don't hash states, but search nodes.
   * this way, don't need to keep states twice in memory
   */
  tmp = new_EhcNode();
  source_to_dest( &(tmp->S), S);
  hash_ehc_node( tmp );

  lehc_current_end = lehc_space_head->next;
  if ( lH ) {
    for ( i = 0; i < gnum_H; i++ ) {
      if ( result_to_dest( &S__, S, gH[i] ) ) {
	add_to_ehc_space_cost( &S__, gH[i], NULL );
      }
    }
  } else {
    for ( i = 0; i < gnum_A; i++ ) {
      if ( result_to_dest( &S__, S, gA[i] ) ) {
	add_to_ehc_space_cost( &S__, gA[i], NULL );
      }
    }
  }

  lehc_current_start = lehc_space_head->next;
  lehc_first_level_end = lehc_current_end;

  while ( TRUE ) {  
    if (!memory_check())
      return FALSE;

    if ( lehc_current_start == lehc_current_end ) {
      reset_ehc_hash_entrys();
      free( tmp );
      return FALSE;
    }

    if (lehc_current_start == lehc_first_level_end && !second_pass)
      {
	lehc_current_start = lehc_space_head->next;
	second_pass = TRUE;
	continue;
      }

    if (lehc_current_start == lehc_first_level_end && second_pass)
      {
	second_pass = FALSE;
	lehc_first_level_end =  lehc_current_end;
      }


    if ( lehc_current_start->depth > depth ) {
      depth = lehc_current_start->depth;
      if ( depth > gmax_search_depth ) {
	gmax_search_depth = depth;
      }
      if ( gcmd_line.display_info ) {
	printf("[%d]", depth);
	fflush( stdout );
      }
    }

    if (!second_pass)
      {
	h__ = expand_first_node_cost_hdiff( h, h_cost, &h_cost__, &g_cost__);

	/* raquel trace */
	if ( gcmd_line.display_info == 131) {
	  if (lehc_current_start!=NULL){
	    printf(" \n\n --> new current start (%d) ",lehc_current_start->op); 
	    if (lehc_current_start->op != -1) {
	      print_op_name(lehc_current_start->op);
	    }
	  }
	}
	/*end raquel*/

	if (h__ < h)
	  break;
      }
    else
      {
	if ( LESS( gcmd_line.h_weight * lehc_current_start->float_hn + 
		   (gcmd_line.g_weight * lehc_current_start->float_gn) , h_cost)) {
	  break;
	}
	  
	else{
	  lehc_current_start = lehc_current_start->next;

	}
      }
  }



  reset_ehc_hash_entrys();
  free( tmp );

  extract_plan_fragment( S );

  source_to_dest( S_, &(lehc_current_start->S) );
  *h_ = h__;
  *h_cost_ = h_cost__;


  /* raquel trace */
  if ( gcmd_line.display_info == 131 ) {
      printf(" --> chosen (%d) ",lehc_current_start->op); 
      if (lehc_current_start->op != -1) {
          print_op_name(lehc_current_start->op);
	  if (lehc_current_start->father && lehc_current_start->father->op != -1) {
            printf(" father: ");
            print_op_name(lehc_current_start->father->op);
	  }

          print_State(lehc_current_start->S);
        }
    
     }
   /*end raquel*/


  return TRUE;

}
  



int expand_first_node_cost_hdiff( int h, float h_cost, float *h_cost_, float *g_cost_ )

{

  static Bool fc = TRUE;
  static State S_;

  int h_, i;

  if ( fc ) {
    make_state( &S_, gnum_ft_conn, gnum_fl_conn );
    fc = FALSE;
  }

  if ( lH ) {
    h_ = get_1P_and_H( &(lehc_current_start->S) );
    /*
    gcmd_line.heuristic = 2;
    get_1P_and_H( &(lehc_current_start->S) );
    gevaluated_states--;
    gcmd_line.heuristic = 1;
    */
    if (!gcmd_line.hdiff)
      {
	qsort(gH, gnum_H, sizeof(int), compare_helpful_actions);
      }
    show_collected_actions(gH, gnum_H);
  } else {
     h_ = get_1P_and_A( &(lehc_current_start->S) );

     if (!gcmd_line.hdiff)
       {
	 qsort(gA, gnum_A, sizeof(int), compare_helpful_actions);
       }
     show_collected_actions(gA, gnum_A);
  }   

  if ( h_ == INFINITY ) {
    lehc_current_start = lehc_current_start->next;
    return h_;
  }

  if ((gcmd_line.optimize && goptimization_established && gcmd_line.use_cost_heuristic) || gcmd_line.hdiff)
    {
    *h_cost_ =  gcost + h_ * gtt;
    lehc_current_start->float_hn =  *h_cost_;
    }
  else
    {
    *h_cost_ = (float) h_;
    }

  if ( gcmd_line.display_info == 131 || gcmd_line.display_info == 130 ) {
                  printf("\n------------------------------------------------");
                  printf("\n Evaluation of the state achieved by: ");
                  print_op_name(lehc_current_start->op);
		  if ( gcmd_line.optimize && goptimization_established ) {
		    printf("\n       cost heuristic: %f", *h_cost_);
		  }
                  printf("\n       heuristic: %d", h_);
                  printf("\n       depth: %d\n",lehc_current_start->depth);
 }


  *g_cost_ = lehc_current_start->float_gn;
  /*

  if ( LESS(gcmd_line.h_weight *  *h_cost_  + ((float) gcmd_line.g_weight * lehc_current_start->float_gn), h_cost) ||
       (FLOAT_EQUAL(gcmd_line.h_weight * *h_cost_  + ((float) gcmd_line.g_weight * lehc_current_start->float_gn), h_cost) && h_ < h)) {
    return h_;
  }
  */
  if (h_ < h)
    return h_;

  if ( lH ) {
    for ( i = 0; i < gnum_H; i++ ) {
      if ( result_to_dest( &S_, &(lehc_current_start->S), gH[i] ) ) {
	add_to_ehc_space_cost( &S_, gH[i], lehc_current_start );
      }
    }
  } else {
    for ( i = 0; i < gnum_A; i++ ) {
      if ( result_to_dest( &S_, &(lehc_current_start->S), gA[i] ) ) {
	add_to_ehc_space_cost( &S_, gA[i], lehc_current_start );
      }
    }
  }
    
  lehc_current_start = lehc_current_start->next;

  return h_;

}




/* Raquel: como el anterior, pero ordenando sus sucesores a primer nivel por h */
/* o cost + h*/
Bool do_enforced_hill_climbing_cost_order_first_level( void )

{

  State S, S_;
  int i, h, h_;
  float h_cost, h_cost_;

  make_state( &S, gnum_ft_conn, gnum_fl_conn );
  make_state( &S_, gnum_ft_conn, gnum_fl_conn );

  /* initialize plan hash table, search space, search hash table
   */
  for ( i = 0; i < PLAN_HASH_SIZE; i++ ) {
    lplan_hash_entry[i] = NULL;
  }
  hash_plan_state( &ginitial_state, 0 );
  
  lehc_space_head = new_EhcNode();
  lehc_space_end = lehc_space_head;
  
  for ( i = 0; i < EHC_HASH_SIZE; i++ ) {
    lehc_hash_entry[i] = NULL;
    lnum_ehc_hash_entry[i] = 0;
    lchanged_ehc_entry[i] = FALSE;
  }
  lnum_changed_ehc_entrys = 0;
  

  /* start enforced Hill-climbing
   */
  lH = TRUE;

  source_to_dest( &S, &ginitial_state );

  if (gcmd_line.display_info == 131) 
    print_State(S);

  h = get_1P_and_H( &S );
  qsort(gH, gnum_H, sizeof(int), compare_helpful_actions);
  show_collected_actions(gH, gnum_H);

  if ( h == INFINITY ) {
    return FALSE;
  }
  if ( h == 0 ) {
    return TRUE;
  }  

  if ((gcmd_line.optimize && goptimization_established && gcmd_line.use_cost_heuristic) || gcmd_line.hdiff)
    h_cost =  gcost + h * gtt;
  else
    h_cost = (float) h;
 

  if ( gcmd_line.display_info ) {
    printf("\n\nCueing down from goal distance: %4d (%f) into depth ", h, h_cost);
  }

   /* raquel: trace */
   if ( gcmd_line.display_info == 131 ) {
     if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.heuristic ) {
       printf(" with heuristic cost: %f\n", h_cost);
     }
   }
   /* end raquel */

  while ( h != 0 ) {

    if (!memory_check())
      return FALSE;

    if ( !search_for_better_state_cost_order_first_level( &S, h, h_cost, &S_, &h_, &h_cost_ ) ) {
      printf(" --- pruning stopped --- ");
      get_1P_and_A( &S );
      qsort(gA, gnum_A, sizeof(int), compare_helpful_actions);
      
      show_collected_actions(gA, gnum_A);

      lH = FALSE;
      if ( !search_for_better_state_cost_order_first_level( &S, h, h_cost, &S_, &h_ , &h_cost_) ) {
	return FALSE;
      }
      lH = TRUE;
      get_1P_and_H( &S_ );/* to set up gH info for new start state */
      qsort(gH, gnum_H, sizeof(int), compare_helpful_actions);
      show_collected_actions(gH, gnum_H);
    }
    source_to_dest( &S, &S_ );
    h = h_;
    h_cost = h_cost_;
    get_1P_and_H( &S_ );/* to set up gH info for new start state */
    qsort(gH, gnum_H, sizeof(int), compare_helpful_actions);
    show_collected_actions(gH, gnum_H);
    if ( gcmd_line.display_info ) {
      printf("\n                                %4d (%f)           ", h, h_cost);
    }
  }

  /* compute final total cost */
  compute_total_cost_from_metric(&S_);

  return TRUE;

}













/*************************************************
 * FUNCTIONS FOR BREADTH FIRST SEARCH IN H SPACE *
 *************************************************/



Bool search_for_better_state_cost_order_first_level( State *S, int h, float h_cost, State *S_, int *h_ , float *h_cost_)

{

  static Bool fc = TRUE;
  static State S__;

  int i, h__, depth = 0;
  float h_cost__= 0 ;
  EhcNode *tmp;
  EhcNode *min_node=NULL;
  float min_float_h = INFINITY; 
  float action_cost=0;
  float min_int_h= INFINITY;

  if ( fc ) {
    make_state( &S__, gnum_ft_conn, gnum_fl_conn );
    fc = FALSE;
  }

  /* don't hash states, but search nodes.
   * this way, don't need to keep states twice in memory
   */
  tmp = new_EhcNode();
  source_to_dest( &(tmp->S), S);
  hash_ehc_node( tmp );

  lehc_current_end = lehc_space_head->next;
  if ( lH ) {
    for ( i = 0; i < gnum_H; i++ ) {
      if ( result_to_dest( &S__, S, gH[i] ) ) {
	add_to_ehc_space_cost( &S__, gH[i], NULL );
      }
    }
  } else {
    for ( i = 0; i < gnum_A; i++ ) {
      if ( result_to_dest( &S__, S, gA[i] ) ) {
	add_to_ehc_space_cost( &S__, gA[i], NULL );
      }
    }
  }

  lehc_current_start = lehc_space_head->next;

  while ( TRUE ) {  
    if (!memory_check())
      return FALSE;

    if ( lehc_current_start == lehc_current_end) {
      if (min_node) 
	break;
      reset_ehc_hash_entrys();
      free( tmp );
      return FALSE;
    }
    if ( lehc_current_start->depth > depth ) {
      depth = lehc_current_start->depth;
      if ( depth > gmax_search_depth ) {
	gmax_search_depth = depth;
      }
      if ( gcmd_line.display_info ) {
	printf("[%d]", depth);
	fflush( stdout );
      }
    }

    if ( lH ) {
      h__ = get_1P_and_H( &(lehc_current_start->S) );
      qsort(gH, gnum_H, sizeof(int), compare_helpful_actions);
    } else {
      h__ = get_1P_and_A( &(lehc_current_start->S) );
      qsort(gA, gnum_A, sizeof(int), compare_helpful_actions);
    } 
    

    if ( h__ == INFINITY ) {
      lehc_current_start = lehc_current_start->next;
      continue;
    }

    if ( gcmd_line.optimize && goptimization_established ) 
      {
	h_cost__ = gcost;
	h_cost__ =((float) gcmd_line.h_weight)  * (h_cost__ + h__ * gtt);
	
	/* if better heuristic than father and minimizes the sum h(n) +
	   cost(a) in first level*/
	if (lehc_current_start->depth == 1)
	  {
	    if ( LESS(h_cost__, h_cost) &&
		 LESS(h_cost__ + gef_conn[lehc_current_start->op].cost + gtt, min_float_h + action_cost) )
	      {
		min_node=lehc_current_start;
		min_float_h= h_cost__;
		action_cost= gef_conn[lehc_current_start->op].cost + gtt ;
		min_int_h=h__;
	      }
	  }
	else
	  {
	    /* other levels */
	    if ( LESS( h_cost__ , h_cost) || (FLOAT_EQUAL(h_cost__ , h_cost) && h__ < h)) 
	      {
		min_node = lehc_current_start;
		min_float_h = h_cost__;
		min_int_h=h__;
		break;
	      }
	  }

      }
    else
      {
	if (lehc_current_start->depth == 1)
	  {
	    if ( LESS(h__, h) && LESS(h__ , min_int_h)){
	      min_node = lehc_current_start;
	      min_int_h=h__;
	    }
	    else
	      {
		if ( LESS(h__, h)) {
		  min_node=lehc_current_start;
		  min_int_h=h__;
		  break;
		}
	      }
	  }
      }
       
   if (!min_node)
     /* generate sucessors */
     {
       if ( lH ) {
	 for ( i = 0; i < gnum_H; i++ ) {
	   if ( result_to_dest( &S__, &(lehc_current_start->S), gH[i] ) ) {
	     add_to_ehc_space_cost( &S__, gH[i], lehc_current_start );
	   }
	 }
       } else {
	 for ( i = 0; i < gnum_A; i++ ) {
	   if ( result_to_dest( &S__, &(lehc_current_start->S), gA[i] ) ) {
	     add_to_ehc_space_cost( &S__, gA[i], lehc_current_start );
	   }
	 }
       }
     }

  lehc_current_start = lehc_current_start->next;
  }

  lehc_current_start = min_node;
  reset_ehc_hash_entrys();
  free( tmp );

  extract_plan_fragment( S );

  source_to_dest( S_, &(lehc_current_start->S) );
  *h_ = min_int_h;
  if ( gcmd_line.optimize && goptimization_established ) 
    *h_cost_ = 	min_float_h;


  /* raquel trace */
  if ( gcmd_line.display_info == 131 ) {
      printf(" --> chosen (%d) ",lehc_current_start->op); 
      if (lehc_current_start->op != -1) {
          print_op_name(lehc_current_start->op);
	  if (lehc_current_start->father && lehc_current_start->father->op != -1) {
            printf(" father: ");
            print_op_name(lehc_current_start->father->op);
	  }

          print_State(lehc_current_start->S);
        }
    
     }
   /*end raquel*/


  return TRUE;

}
  






int compare_helpful_actions( const void* a, const void* b ) {
   int* arg1 = (int*) a;
   int* arg2 = (int*) b;

   if (LESS(gef_conn[*arg1].cost, gef_conn[*arg2].cost)) return -1;
       else if ( FLOAT_EQUAL(gef_conn[*arg1].cost,gef_conn[*arg2].cost) )      
	 {
	 if (*arg1 < *arg2) return -1;
	 else return 1;
	 }
       else return 1;

 }    





void add_to_ehc_space_cost( State *S, int op, EhcNode *father )

{  EhcNode* hashed;
  float float_gn;

  /* see if superior state (in terms of goal reachability)
   * is already a part of this search space
   */

  /*  
   if ( superior_ehc_state_hashed( S ) ) {
    return;
    }
  */
   if ( !father ) {
    float_gn =  gef_conn[op].cost + gtt ;
   } else {
    float_gn = father->float_gn + gef_conn[op].cost + gtt;
   }
   
  hashed = same_no_metric_ehc_state_hashed( S );
  if (hashed && (LESS_EQ(hashed->float_gn,float_gn)))
    return;
   
  if ( !lehc_current_end ) {
    lehc_current_end = new_EhcNode();
    lehc_space_end->next = lehc_current_end;
    lehc_space_end = lehc_current_end;
  }

  source_to_dest( &(lehc_current_end->S), S );
  lehc_current_end->op = op;
  lehc_current_end->father = father;

  if ( !father ) {
    lehc_current_end->depth = 1;
    lehc_current_end->float_gn =  gef_conn[op].cost + gtt ;
  } else {
    lehc_current_end->depth = father->depth + 1;
    lehc_current_end->float_gn = father->float_gn + gef_conn[op].cost + gtt;
  }

  hash_ehc_node( lehc_current_end );

    /* raquel */
    if ( gcmd_line.display_info == 131 ) {
      int j;  
      printf("\n");
      for(j=0; j<= lehc_current_end->depth; j++)
	{
	  printf("   ");
	}
      printf("(%d) ",lehc_current_end->op); 
      if (lehc_current_end->op != -1) {
          print_op_name(lehc_current_end->op);
        }
      if (lehc_current_end->father != NULL)
	 printf(" , father: %d ",lehc_current_end->father->op); 
    }
    /*end raquel*/


  lehc_current_end = lehc_current_end->next;

}




/********************************************************
 * HASHING ALGORITHM FOR RECOGNIZING REPEATED STATES IN *
 * EHC BREADTH FIRST SEARCH                             *
 ********************************************************/












void hash_ehc_node( EhcNode *n )

{

  int i, sum, index;
  EhcHashEntry *h, *prev = NULL;

  sum = state_sum( &(n->S) );
  index = sum & EHC_HASH_BITS;

  h = lehc_hash_entry[index];
  if ( !h ) {
    h = new_EhcHashEntry();
    h->sum = sum;
    h->ehc_node = n;
    lehc_hash_entry[index] = h;
    lnum_ehc_hash_entry[index]++;
    if ( !lchanged_ehc_entry[index] ) {
      lchanged_ehc_entrys[lnum_changed_ehc_entrys++] = index;
      lchanged_ehc_entry[index] = TRUE;
    }
    return;
  }
  i = 0;
  while ( h ) {
    if ( i == lnum_ehc_hash_entry[index] ) {
      break;
    }
    i++;
    prev = h;
    h = h->next;
  }

  if ( h ) {
    /* current list end is still in allocated list of hash entrys
     */
    h->sum = sum;
    h->ehc_node = n;
    lnum_ehc_hash_entry[index]++;
    if ( !lchanged_ehc_entry[index] ) {
      lchanged_ehc_entrys[lnum_changed_ehc_entrys++] = index;
      lchanged_ehc_entry[index] = TRUE;
    }
    return;
  }
  /* allocated list ended; connect a new hash entry to it.
   */
  h = new_EhcHashEntry();
  h->sum = sum;
  h->ehc_node = n;
  prev->next = h;
  lnum_ehc_hash_entry[index]++;
  if ( !lchanged_ehc_entry[index] ) {
    lchanged_ehc_entrys[lnum_changed_ehc_entrys++] = index;
    lchanged_ehc_entry[index] = TRUE;
  }
  return;
      
}



Bool ehc_state_hashed( State *S )

{

  int i, sum, index;
  EhcHashEntry *h;

  sum = state_sum( S );
  index = sum & EHC_HASH_BITS;

  h = lehc_hash_entry[index];
  for ( i = 0; i < lnum_ehc_hash_entry[index]; i++ ) {
    if ( h->sum != sum ) {
      h = h->next;
      continue;
    }
    if ( same_state( &(h->ehc_node->S), S ) ) {
      return TRUE;
    }
    h = h->next;
  }

  return FALSE;

}


EhcNode* same_no_metric_ehc_state_hashed( State *S )

{

  int i, sum, index;
  EhcHashEntry *h;

  sum = state_sum( S );
  index = sum & EHC_HASH_BITS;

  h = lehc_hash_entry[index];
  for ( i = 0; i < lnum_ehc_hash_entry[index]; i++ ) {
    if ( h->sum != sum ) {
      h = h->next;
      continue;
    }
    if ( same_no_metric_state( &(h->ehc_node->S), S ) ) {
      return h->ehc_node;
    }
    h = h->next;
  }

  return NULL;

}



Bool superior_ehc_state_hashed( State *S )

{

  int i, sum, index;
  EhcHashEntry *h;

  sum = state_sum( S );
  index = sum & EHC_HASH_BITS;

  h = lehc_hash_entry[index];
  for ( i = 0; i < lnum_ehc_hash_entry[index]; i++ ) {
    if ( h->sum < sum ) {
      h = h->next;
      continue;
    }
    if ( superior_state( &(h->ehc_node->S), S ) ) {
      return TRUE;
    }
    h = h->next;
  }

  return FALSE;

}



Bool superior_state( State *S1, State *S2 ) 

{

  int i, j;

  for ( i = 0; i < S2->num_F; i++ ) {
    for ( j = 0; j < S1->num_F; j++ ) {
      if ( S1->F[j] == S2->F[i] ) {
	break;
      }
    }
    if ( j == S1->num_F ) {
      return FALSE;
    }
  }

  /* check whether the fluent values are superior.
   * see JAIR article for explanation / justification
   */
  for ( i = 0; i < gnum_real_fl_conn; i++ ) {
    if ( !gfl_conn[i].relevant ) {
      continue;
    }

    if ( !S2->f_D[i] ) {
      continue;
    }

    if ( !S1->f_D[i] ||
	 S2->f_V[i] > S1->f_V[i] ) {
      return FALSE;
    }
  }

  return TRUE;

}



void reset_ehc_hash_entrys( void )

{

  int i;

  for ( i = 0; i < lnum_changed_ehc_entrys; i++ ) {
    lnum_ehc_hash_entry[lchanged_ehc_entrys[i]] = 0;
    lchanged_ehc_entry[lchanged_ehc_entrys[i]] = FALSE;
  }
  lnum_changed_ehc_entrys = 0;

}













/***************************************************
 * FUNCTIONS FOR UPDATING THE CURRENT SERIAL PLAN, *
 * BASED ON SEARCH SPACE INFORMATION .             *
 *                                                 *
 * EMPLOY SOMEWHAT TEDIOUS HASHING PROCEDURE TO    *
 * AVOID REPEATED STATES IN THE PLAN               *
 ***************************************************/














void extract_plan_fragment( State *S )

{

  EhcNode *i;
  int ops[MAX_PLAN_LENGTH], num_ops;
  State_pointer states[MAX_PLAN_LENGTH];
  int j;
  PlanHashEntry *start = NULL, *i_ph;

  num_ops = 0;
  for ( i = lehc_current_start; i; i = i->father ) {
    if ( (start = plan_state_hashed( &(i->S) )) != NULL ) {
      for ( i_ph = start->next_step; i_ph; i_ph = i_ph->next_step ) {
	i_ph->step = -1;
      }
      gnum_plan_ops = start->step;
      break;
    }
    if ( num_ops == MAX_PLAN_LENGTH ) {
      printf("\nincrease MAX_PLAN_LENGTH! currently %d\n\n",
	     MAX_PLAN_LENGTH);
      exit( 1 );
    }
    states[num_ops] = &(i->S);
    ops[num_ops++] = i->op;
  }
  if ( !start ) {
    start = plan_state_hashed( S );
    if ( !start ) {
      printf("\n\ncurrent start state not hashed! debug me!\n\n");
      exit( 1 );
    }
    if ( start->step == -1 ) {
      printf("\n\ncurrent start state marked removed from plan! debug me!\n\n");
      exit( 1 );
    }
  }

  for ( j = num_ops - 1; j > -1; j-- ) {
    if ( gnum_plan_ops == MAX_PLAN_LENGTH ) {
      printf("\nincrease MAX_PLAN_LENGTH! currently %d\n\n",
	     MAX_PLAN_LENGTH);
      exit( 1 );
    }
    start->next_step = hash_plan_state( states[j], gnum_plan_ops + 1 );
    start = start->next_step;
    copy_source_to_dest( &(gplan_states[gnum_plan_ops+1]), states[j] );
    gplan_ops[gnum_plan_ops++] = ops[j];
  }

}




PlanHashEntry *hash_plan_state( State *S, int step )

{

  int sum, index;
  PlanHashEntry *h, *tmp;

  sum = state_sum( S );
  index = sum & PLAN_HASH_BITS;

  for ( h = lplan_hash_entry[index]; h; h = h->next ) {
    if ( h->sum != sum ) continue;
    if ( same_state( S, &(h->S) ) ) break;
  }

  if ( h ) {
    if ( h->step != -1 ) {
      printf("\n\nreencountering a state that is already in plan! debug me\n\n");
      exit( 1 );
    }
    h->step = step;
    return h;
  }

  for ( h = lplan_hash_entry[index]; h && h->next; h = h->next );

  tmp = new_PlanHashEntry();
  tmp->sum = sum;
  copy_source_to_dest( &(tmp->S), S );
  tmp->step = step;

  if ( h ) {
    h->next = tmp;
  } else {
    lplan_hash_entry[index] = tmp;
  }

  return tmp;

}
  

 
PlanHashEntry *plan_state_hashed( State *S )

{

  int sum, index;
  PlanHashEntry *h;

  sum = state_sum( S );
  index = sum & PLAN_HASH_BITS;

  for ( h = lplan_hash_entry[index]; h; h = h->next ) {
    if ( h->sum != sum ) continue;
    if ( same_state( S, &(h->S) ) ) break;
  }

  if ( h && h->step != -1 ) {
    return h;
  }

  return NULL;

}



Bool same_state( State *S1, State *S2 ) 

{

  int i, j;

  if ( S1->num_F != S2->num_F ) {
    return FALSE;
  }

  for ( i = 0; i < S2->num_F; i++ ) {
    for ( j = 0; j < S1->num_F; j++ ) {
      if ( S1->F[j] == S2->F[i] ) {
	break;
      }
    }
    if ( j == S1->num_F ) {
      return FALSE;
    }
  }

  for ( i = 0; i < gnum_fl_conn; i++ ) {
    if ( S2->f_D[i] != S1->f_D[i] ||
	 S2->f_V[i] != S1->f_V[i] ) {
      return FALSE;
    }
  }

  return TRUE;

}
















/************************************
 * BEST FIRST SEARCH IMPLEMENTATION *
 ************************************/














Bool do_best_first_search( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool start = TRUE;

  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_bfs_space( &ginitial_state, -1, NULL );

  while ( TRUE ) {
    if ( (first = lbfs_space_head->next) == NULL ) {
      if ( gcmd_line.display_info ) {
	printf("\n\nsearch space empty! problem proven unsolvable.\n\n");
      }
      return FALSE;
    }

    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( start ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	start = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    if ( first->h == 0 ) {
      /* raquel: compute final_total_cost  */
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }
      
      break;

    }

    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_bfs_space( &S, first->H[i], first );
      }
    }

    first->next = lbfs_space_had;
    lbfs_space_had = first;
  }

  extract_plan( first );
  return TRUE;

}



void add_to_bfs_space( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i;
  int j, h, intg, int_fn = 0;
  float cost = 0, floatg = 0, float_fn = 0;


  if ( gcmd_line.optimize && goptimization_established ) {
    if ( bfs_state_hashed( S ) ) {
      return;
    }
  } else {
    if ( superior_bfs_state_hashed( S ) ) {
      return;
    }
  }

  h = get_1P_and_A( S );
  if ( gcmd_line.optimize && goptimization_established ) {
    cost = gcost;
    /* gtt is mulitplicator of TOTAL-TIME in final metric; if no
     * total-time part in metric, it is 0
     */
    cost += h * gtt;
  }

  if ( h == INFINITY ) {
    return;
  }

  if ( father ) {
    intg = father->g + 1;
  } else {
    intg = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    floatg = state_cost( S, father );
    float_fn = (((float) gcmd_line.g_weight) * floatg) + (((float) gcmd_line.h_weight) * cost);
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->float_fn >= float_fn ) break;
    }
  } else {
    int_fn = (gcmd_line.g_weight * intg) + (gcmd_line.h_weight * h);
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= int_fn ) break;
    }
  }

  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = float_fn;
  } else {
    new->int_fn = int_fn;
  }
  new->father = father;
  new->g = intg;

  new->H = ( int * ) calloc( gnum_A, sizeof( int ) );
  for ( j = 0; j < gnum_A; j++ ) {
    new->H[j] = gA[j];
  }
  new->num_H = gnum_A;

  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );

}



float state_cost( State *S, BfsNode *father )

{

  float cost = 0;
  int i;

  for ( i = 0; i < glnf_metric.num_pF; i++ ) {
    if ( glnf_metric.pF[i] == -2 ) {
      /* cost is number of steps from I to S 
       */ 
      if ( father ) {
	cost += gtt * (father->g + 1);
      }/* no father, no steps, no cost */
    } else {
      cost += (glnf_metric.pC[i] * 
	       (S->f_V[glnf_metric.pF[i]] - ginitial_state.f_V[glnf_metric.pF[i]]));
    }
  }

  return cost;

}



void extract_plan( BfsNode *last )

{

  BfsNode *i;
  int ops[MAX_PLAN_LENGTH], num_ops;
  int j;

  num_ops = 0;
  for ( i = last; i->op != -1; i = i->father ) {
    if ( num_ops == MAX_PLAN_LENGTH ) {
      printf("\nincrease MAX_PLAN_LENGTH! currently %d\n\n",
	     MAX_PLAN_LENGTH);
      exit( 1 );
    }
    ops[num_ops++] = i->op;
  }

  gnum_plan_ops = 0;
  for ( j = num_ops - 1; j > -1; j-- ) {
    gplan_ops[gnum_plan_ops++] = ops[j];
  }
  //  compute_prec_order_from_solution();
  //update_prec_orders();


}
















/************************************************************
 * HASHING ALGORITHM FOR RECOGNIZING REPEATED STATES IN BFS *
 ************************************************************/












void hash_bfs_node( BfsNode *n )

{

  int sum, index;
  BfsHashEntry *h, *tmp;

  sum = state_sum( &(n->S) );
  index = sum & BFS_HASH_BITS;

  h = lbfs_hash_entry[index];
  if ( !h ) {
    h = new_BfsHashEntry();
    h->sum = sum;
    h->bfs_node = n;
    lbfs_hash_entry[index] = h;
    return;
  }
  for ( ; h->next; h = h->next );

  tmp = new_BfsHashEntry();
  tmp->sum = sum;
  tmp->bfs_node = n;
  h->next = tmp;
      
}



Bool bfs_state_hashed( State *S )

{

  int sum, index;
  BfsHashEntry *h;

  sum = state_sum( S );
  index = sum & BFS_HASH_BITS;

  h = lbfs_hash_entry[index];
  for ( h = lbfs_hash_entry[index]; h; h = h->next ) {
    if ( h->sum != sum ) {
      continue;
    }
    if ( same_state( &(h->bfs_node->S), S ) ) {
      return TRUE;
    }
  }

  return FALSE;

}



Bool superior_bfs_state_hashed( State *S )

{

  int sum, index;
  BfsHashEntry *h;

  sum = state_sum( S );
  index = sum & BFS_HASH_BITS;

  h = lbfs_hash_entry[index];
  for ( h = lbfs_hash_entry[index]; h; h = h->next ) {
    if ( h->sum < sum ) {
      continue;
    }
    if ( superior_state( &(h->bfs_node->S), S ) ) {
      return TRUE;
    }
  }

  return FALSE;

}



int state_sum( State *S )

{

  int i, sum = 0;

  for ( i = 0; i < S->num_F; i++ ) {
    sum += gft_conn[S->F[i]].rand;
  }

  for ( i = 0; i < gnum_real_fl_conn; i++ ) {
    if ( !gfl_conn[i].relevant ) {
      continue;
    }
    if ( !S->f_D[i] ) {
      continue;
    }
    sum += gfl_conn[i].rand * ( int ) S->f_V[i];
  }

  return sum;

}















/****************************
 * STATE HANDLING FUNCTIONS *
 ****************************/



















/* state transition function; here, takes in an action whose
 * logical and numerical preconds are fulfilled, and returns TRUE,
 * putting the result into *dest, iff the action has at least one
 * appearing effect and is legal, i.e. if
 * no illegal numeric effects occur.
 */
Bool result_to_dest( State *dest, State *source, int op )

{

  static Bool first_call = TRUE;
  static Bool *in_source, *in_dest, *in_del, *true_ef, *assigned;
  static int *del, num_del;

  int i, j, ef, fl;
  float val, source_val;
  Comparator comp;

  Bool one_appeared = FALSE;
  
  if ( first_call ) {
    in_source = ( Bool * ) calloc( gnum_ft_conn, sizeof( Bool ) );
    in_dest = ( Bool * ) calloc( gnum_ft_conn, sizeof( Bool ) );
    in_del = ( Bool * ) calloc( gnum_ft_conn, sizeof( Bool ) );
    true_ef = ( Bool * ) calloc( gnum_ef_conn, sizeof( Bool ) );
    assigned = ( Bool * ) calloc( gnum_fl_conn, sizeof( Bool ) );
    del = ( int * ) calloc( gnum_ft_conn, sizeof( int ) );
    for ( i = 0; i < gnum_ft_conn; i++ ) {
      in_source[i] = FALSE;
      in_dest[i] = FALSE;
      in_del[i] = FALSE;
    }
    for ( i = 0; i < gnum_ef_conn; i++ ) {
      true_ef[i] = FALSE;
    }
    for ( i = 0; i < gnum_fl_conn; i++ ) {
      assigned[i] = FALSE;
    }
    first_call = FALSE;
  }

  /* setup true facts for effect cond evaluation
   */
  for ( i = 0; i < source->num_F; i++ ) {
    in_source[source->F[i]] = TRUE;
  }

  /* evaluate effect conditions and setup deleted facts
   */
  num_del = 0;
  for ( i = 0; i < gop_conn[op].num_E; i++ ) {
    ef = gop_conn[op].E[i];
    /* logic cond true?
     */
    for ( j = 0; j < gef_conn[ef].num_PC; j++ ) {
      if ( !in_source[gef_conn[ef].PC[j]] ) break;
    }
    if ( j < gef_conn[ef].num_PC ) continue;
    /* numeric cond true?
     */
    for ( j = 0; j < gef_conn[ef].num_f_PC; j++ ) {
      fl = gef_conn[ef].f_PC_fl[j];
      val = gef_conn[ef].f_PC_c[j];
      comp = gef_conn[ef].f_PC_comp[j];
      if ( !determine_source_val( source, fl, &source_val ) ) {
	/* condition access to an undefined fluent!
	 */
	for ( i = 0; i < num_del; i++ ) {
	  in_del[del[i]] = FALSE;
	}
	for ( i = 0; i < gop_conn[op].num_E; i++ ) {
	  true_ef[i] = FALSE;
	}
	for ( i = 0; i < source->num_F; i++ ) {
	  in_source[source->F[i]] = FALSE;
	}
	return FALSE;
      }
      if ( !number_comparison_holds( comp, source_val, val ) ) break;
    }
    if ( j < gef_conn[ef].num_f_PC ) continue;

    if ( gef_conn[ef].illegal ) {
      /* effect always affects an undefined fluent, as we found out
       * earlier
       */
      for ( i = 0; i < source->num_F; i++ ) {
	in_source[source->F[i]] = FALSE;
      }
      for ( i = 0; i < num_del; i++ ) {
	in_del[del[i]] = FALSE;
      }
      for ( i = 0; i < gop_conn[op].num_E; i++ ) {
	true_ef[i] = FALSE;
      }
      return FALSE;
    }
    true_ef[i] = TRUE;
    one_appeared = TRUE; 
    for ( j = 0; j < gef_conn[ef].num_D; j++ ) {
      if ( in_del[gef_conn[ef].D[j]] ) continue;
      in_del[gef_conn[ef].D[j]] = TRUE;
      del[num_del++] = gef_conn[ef].D[j];
    }
  }
  if ( !one_appeared ) {
    /* no effect appeared which means that the action is either useless
     * here or its preconds are even not fulfilled (the latter
     * shouldn't happen by get_A but well....)
     */
    for ( i = 0; i < source->num_F; i++ ) {
      in_source[source->F[i]] = FALSE;
    }
    for ( i = 0; i < num_del; i++ ) {
      in_del[del[i]] = FALSE;
    }
    for ( i = 0; i < gop_conn[op].num_E; i++ ) {
      true_ef[i] = FALSE;
    }
    return FALSE;
  }

  /* first, see about the numeric effects - those might render
   * the op illegal here. start by copying numeric info.
   */
  for ( i = 0; i < gnum_fl_conn; i++ ) {
    dest->f_D[i] = source->f_D[i];
    dest->f_V[i] = source->f_V[i];
  }

  /* illegal is an op if the result is not well-defined,
   * or if it affects an undefined fluent.
   */
  for ( i = 0; i < gop_conn[op].num_E; i++ ) {
    if ( !true_ef[i] ) continue;
    ef = gop_conn[op].E[i];
    for ( j = 0; j < gef_conn[ef].num_AS; j++ ) {
      fl = gef_conn[ef].AS_fl[j];
      if ( gef_conn[ef].AS_fl_[j] == -1 ) {
	val = gef_conn[ef].AS_c[j];
      } else {
	if ( !determine_source_val( source, gef_conn[ef].AS_fl_[j], &val ) ) {
	  /* effect rh makes use of undefined fluent!
	   */
	  for ( i = 0; i < gnum_fl_conn; i++ ) {
	    assigned[i] = FALSE;
	  }
	  for ( i = 0; i < source->num_F; i++ ) {
	    in_source[source->F[i]] = FALSE;
	  }
	  for ( i = 0; i < num_del; i++ ) {
	    in_del[del[i]] = FALSE;
	  }
	  for ( i = 0; i < gop_conn[op].num_E; i++ ) {
	    true_ef[i] = FALSE;
	  }
	  return FALSE;
	}
	val += gef_conn[ef].AS_c[j];
      }
      if ( assigned[fl] &&
	   val != dest->f_V[fl] ) {
	/* two different values assigned --> result not well-defined --> illegal!
	 */
	for ( i = 0; i < gnum_fl_conn; i++ ) {
	  assigned[i] = FALSE;
	}
	for ( i = 0; i < source->num_F; i++ ) {
	  in_source[source->F[i]] = FALSE;
	}
	for ( i = 0; i < num_del; i++ ) {
	  in_del[del[i]] = FALSE;
	}
	for ( i = 0; i < gop_conn[op].num_E; i++ ) {
	  true_ef[i] = FALSE;
	}
	return FALSE;
      }
      dest->f_D[fl] = TRUE;
      dest->f_V[fl] = val;
      assigned[fl] = TRUE;
    }
    for ( j = 0; j < gef_conn[ef].num_IN; j++ ) {
      fl = gef_conn[ef].IN_fl[j];
      if ( assigned[fl] || 
	   !source->f_D[fl]) {
	/* assign and increase --> result not well-defined --> illegal!
	 * affects an undefined fluent --> illegal!
	 */
	for ( i = 0; i < gnum_fl_conn; i++ ) {
	  assigned[i] = FALSE;
	}
	for ( i = 0; i < source->num_F; i++ ) {
	  in_source[source->F[i]] = FALSE;
	}
	for ( i = 0; i < num_del; i++ ) {
	  in_del[del[i]] = FALSE;
	}
	for ( i = 0; i < gop_conn[op].num_E; i++ ) {
	  true_ef[i] = FALSE;
	}
	return FALSE;
      }
      if ( gef_conn[ef].IN_fl_[j] == -1 ) {
	val = gef_conn[ef].IN_c[j];
      } else {
	if ( !determine_source_val( source, gef_conn[ef].IN_fl_[j], &val ) ) {
	  /* effect rh makes use of undefined fluent!
	   */
	  for ( i = 0; i < gnum_fl_conn; i++ ) {
	    assigned[i] = FALSE;
	  }
	  for ( i = 0; i < source->num_F; i++ ) {
	    in_source[source->F[i]] = FALSE;
	  }
	  for ( i = 0; i < num_del; i++ ) {
	    in_del[del[i]] = FALSE;
	  }
	  for ( i = 0; i < gop_conn[op].num_E; i++ ) {
	    true_ef[i] = FALSE;
	  }
	  return FALSE;
	}
	val += gef_conn[ef].IN_c[j];
      }
      dest->f_V[fl] += val;
    }
  }

  /* put all non-deleted facts from source into dest.
   * need not check for put-in facts here,
   * as initial state is made doubles-free, and invariant keeps
   * true through the transition procedure
   */
  dest->num_F = 0;
  for ( i = 0; i < source->num_F; i++ ) {
    if ( in_del[source->F[i]] ) {
      continue;
    }
    dest->F[dest->num_F++] = source->F[i];
    in_dest[source->F[i]] = TRUE;
  }

  /* now add all fullfilled effect adds to dest; each fact at most once!
   */
  for ( i = 0; i < gop_conn[op].num_E; i++ ) {
    if ( !true_ef[i] ) continue;
    ef = gop_conn[op].E[i];
    for ( j = 0; j < gef_conn[ef].num_A; j++ ) {
      if ( in_dest[gef_conn[ef].A[j]] ) {
	continue;
      }
      dest->F[dest->num_F++] = gef_conn[ef].A[j];
      in_dest[gef_conn[ef].A[j]] = TRUE;
    }
  }

  /* unset infos
   */
  for ( i = 0; i < source->num_F; i++ ) {
    in_source[source->F[i]] = FALSE;
  }
  for ( i = 0; i < dest->num_F; i++ ) {
    in_dest[dest->F[i]] = FALSE;
  }
  for ( i = 0; i < num_del; i++ ) {
    in_del[del[i]] = FALSE;
  }
  for ( i = 0; i < gop_conn[op].num_E; i++ ) {
    true_ef[i] = FALSE;
  }
  for ( i = 0; i < gnum_fl_conn; i++ ) {
    assigned[i] = FALSE;
  }

  return TRUE;

}



Bool determine_source_val( State *source, int fl, float *val )

{

  int i;

  if ( gfl_conn[fl].artificial ) {
    *val = 0;
    for ( i = 0; i < gfl_conn[fl].num_lnf; i++ ) {
      if ( !source->f_D[gfl_conn[fl].lnf_F[i]] ) {
	return FALSE;
      }
      *val += (gfl_conn[fl].lnf_C[i] * source->f_V[gfl_conn[fl].lnf_F[i]]);
    }
  } else {
    if ( !source->f_D[fl] ) {
      return FALSE;
    }
    *val = source->f_V[fl];
  }

  return TRUE;

}



void copy_source_to_dest( State *dest, State *source )

{

  int i;

  make_state( dest, source->num_F, gnum_fl_conn );

  for ( i = 0; i < source->num_F; i++ ) {
    dest->F[i] = source->F[i];
  }
  dest->num_F = source->num_F;

  for ( i = 0; i < gnum_fl_conn; i++ ) {
    dest->f_D[i] = source->f_D[i];
    dest->f_V[i] = source->f_V[i];
  }

}



void source_to_dest( State *dest, State *source )

{

  int i;

  for ( i = 0; i < source->num_F; i++ ) {
    dest->F[i] = source->F[i];
  }
  dest->num_F = source->num_F;

  for ( i = 0; i < gnum_fl_conn; i++ ) {
    dest->f_D[i] = source->f_D[i];
    dest->f_V[i] = source->f_V[i];
  }

}




/********************************
 * RAQUEL FUNCTIONS
 ********************************/

/************************************************************
 * HASHING ALGORITHM FOR RECOGNIZING REPEATED STATES IN BFS *
 ************************************************************/

/*
  Function to know if there is a hashed node with the same state than S I
  consider two states are the same when the facts are the same and also the
  fluents not taken part in the metric expression are the same (fluents taken
  part on the metric expression never can appear as preconditions given that
  in such case action costs would be state dependant)


  The function superior_bfs_state by Hoffmann is true only 
  when ALL fluents in S have lower values than ALL fluents in the hashed
  state. In that case, the state S is considered hashed.

  Initially I prefer to consider that states with different values
  for fluents that are not in the metric expression are different states

  On the other hand, the function same_state by Hoffmann only is true when the
  states are exactly the same
  
 */
BfsNode * same_no_metric_bfs_state_hashed( State *S, BfsNode *father)

{

  int sum, index;
  BfsHashEntry *h;

  sum = state_sum( S );
  index = sum & BFS_HASH_BITS;

  h = lbfs_hash_entry[index];
  for ( h = lbfs_hash_entry[index]; h; h = h->next ) {
    if ( h->sum < sum ) {
      continue;
    }
    if (same_no_metric_state( &(h->bfs_node->S), S) ) {
      return h->bfs_node;
    }
  }

  return NULL;

}


Bool same_no_metric_state( State *S1, State *S2 ) 
{
  int i, j;

  /* first: all atoms are the same */
  if ( S1->num_F != S2->num_F ) {
    return FALSE;
  }

  for ( i = 0; i < S2->num_F; i++ ) {
    for ( j = 0; j < S1->num_F; j++ ) {
      if ( S1->F[j] == S2->F[i] ) {
	break;
      }
    }
    if ( j == S1->num_F ) {
      return FALSE;
    }
  }

  /* second: all fluents not in the metric expression are the same */

  for ( i = 0; i < gnum_real_fl_conn; i++ ) {

    if ( !gfl_conn[i].relevant ) {
      continue;
    }

    if ( S2->f_D[i] != S1->f_D[i])
      return FALSE;

    if ( !FLOAT_EQUAL(S2->f_V[i],S1->f_V[i]) )

      for (j =0 ; j < glnf_metric.num_pF; j++){
	
	/* if the fluent does not participates in the metric */ 
	if ( !glnf_metric.pF[j] == i ) 
	  return FALSE;
      }
  }

  return TRUE;
}


void unhash_bfs_node (BfsNode *n)

{

  int sum, index;
  BfsHashEntry *h;
  BfsHashEntry **p_next;

  sum = state_sum( &(n->S) );
  index = sum & BFS_HASH_BITS;
  h = lbfs_hash_entry[index];
 
  if ( !h ) {
    return;
  }
 
  p_next=&(lbfs_hash_entry[index]);  
  while (h != NULL && h->bfs_node != n) {
    p_next = &h->next;
    h = h->next;
  }

  if (h != NULL && h->bfs_node==n){
    *p_next = h->next;
    free(h);
  }
	
}

/***********************************
  AUX FUNCTIONS USED BY THE SEARCH
************************************/
void copy_BfsNode_evaluated(BfsNode *source, BfsNode *dest)
{
  
 dest->h = source->h;
 
 if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) 
    dest->float_hn = source->float_hn;

 dest->num_H = source->num_H;
 dest->H = source->H;

 dest->num_nonH = source->num_nonH;
 dest->nonH= source->nonH;

 /* relaxed plan */
 dest->gnum_RP = source->gnum_RP;
 dest->g_RP = source->g_RP;
 dest->RPorder = source->RPorder;
 dest->max_RP_order = source->max_RP_order;
 dest->min_RP_order = source->min_RP_order;

}

void copy_BfsNode_evaluated_rescue(BfsNode *source, BfsNode *dest)
{
  
 dest->h = source->h;
 
  if ( gcmd_line.optimize && goptimization_established ) 
    dest->float_hn = source->float_hn;

 dest->num_H = source->num_H;
 dest->H = source->H;

 if (source->rescue){
   dest->num_nonH = source->rescue->num_nonH;
   dest->nonH= source->rescue->nonH;
 }
 else{
   dest->num_nonH = 0;
 }

 /* relaxed plan */
 /* for rescue nodes no relaxed plan is saved */
 dest->gnum_RP = 0; 
 
}

/* primary list 
 */
void add_to_primary_list ( State* S, int op, BfsNode* father )
{  
  BfsNode* new;
 

  /* Create a new item and copy the state */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );

  /* Copy the operator that led to this state as well as its
     predecessor */
  new->op = op;
  new->father = father;
  
  /* And insert it at the very end of the local (unordered) list */

  new->next = (BfsNode*)0;

  if (lbfs_primary_current_head)
    lbfs_primary_current_tail->next = new;
  else
    lbfs_primary_current_head = new;
  
  /* Copy where the tail is */
    lbfs_primary_current_tail = new;
}


/* secondary list 
 */
void add_to_secondary_list ( State* S, int op, BfsNode* father )
{  
  BfsNode* new;
 

  /* Create a new item and copy the state */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );

  /* Copy the operator that led to this state as well as its
     predecessor */
  new->op = op;
  new->father = father;
  
  /* And insert it at the very end of the local (unordered) list */

  new->next = (BfsNode*)0;

  if (lbfs_secondary_current_head)
    lbfs_secondary_current_tail->next = new;
  else
    lbfs_secondary_current_head = new;
  
  /* Copy where the tail is */
    lbfs_secondary_current_tail = new;
}

void add_to_secondary_list_reverse ( State* S, int op, BfsNode* father )
{  
  BfsNode* new;
  BfsNode* aux;
 

  /* Create a new item and copy the state */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );

  /* Copy the operator that led to this state as well as its
     predecessor */
  new->op = op;
  new->father = father;
  
  /* And insert it at the very begining of the local (unordered) list */
  if (lbfs_secondary_current_head)
    {
    aux = lbfs_secondary_current_head;
    lbfs_secondary_current_head = new;
    new->next =  aux;
    }
  else
    {
    lbfs_secondary_current_head = new;
    new->next =  NULL;
    lbfs_secondary_current_tail = new;
    }

}

void add_to_secondary_list_order_g ( State* S, int op, BfsNode* father )
{  
  BfsNode* new, *i;

 

  /* Create a new item and copy the state */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );

  /* Copy the operator that led to this state as well as its
     predecessor */
  new->op = op;
  new->father = father;
  
  /* compute g*/

  if ( father )
    {
      new->g = father->g +1;
      if ( gcmd_line.optimize && goptimization_established )
	new->float_gn = state_cost (S, father);
    }
  else
    {
      new->g = 0;
      if ( gcmd_line.optimize && goptimization_established )
	new->float_gn = 0;
    }


  /* And insert it ordererd by g i the secondary list */

  new->next = (BfsNode*)0;


  if (!lbfs_secondary_current_head)
    {
      lbfs_secondary_current_head = new;
      lbfs_secondary_current_tail = new;
      return;
    }

  else
    {
      
      if ( (gcmd_line.optimize && goptimization_established)) {
	if (LESS_EQ(new->float_gn, lbfs_secondary_current_head->float_gn))
	  {
	    i = lbfs_secondary_current_head;
	  }
	else
	  {
	    for ( i = lbfs_secondary_current_head; i->next; i = i->next ) {
	      if ( LESS_EQ(new->float_gn, i->next->float_gn))
		break;

	    }
	  }
      }
      else
	{
	  if (new->g < lbfs_secondary_current_head->g)
	  {
	    i = lbfs_secondary_current_head;
	  }
	else
	  {
	    for ( i = lbfs_secondary_current_head; i->next; i = i->next ) {
	      if (new->g < i->next->g)
		break;

	    }
	  }
	}

      new->next = i->next;
      new->prev = i;
      i->next = new;
      if ( new->next ) {
	new->next->prev = new;
      }
      else
	{
	  lbfs_secondary_current_tail = new;
	}
    }
}


/* list for lookahead intermediate states
 */
BfsNode *add_to_lookahead_list ( State* S, int op, BfsNode* father )
{  
  BfsNode* new;
 

  /* Create a new item and copy the state */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );

  /* Copy the operator that led to this state as well as its
     predecessor */
  new->op = op;
  new->father = father;

  /* compute g */
  if ( father )
    {
      new->g = father->g +1;
      if ( gcmd_line.optimize && goptimization_established )
	new->float_gn = state_cost (S, father);
    }
  else
    {
      new->g = 0;
      if ( gcmd_line.optimize && goptimization_established )
	new->float_gn = 0;
    }

  
  /* And insert it at the very end of the local (unordered) list */

  new->next = (BfsNode*)0;

  if (lbfs_lookahead_current_head)
    lbfs_lookahead_current_tail->next = new;
  else
    lbfs_lookahead_current_head = new;
  
  /* Copy where the tail is */
    lbfs_lookahead_current_tail = new;

    return new;
}



/**********************************************************************************
 * ANYTIME BEST FIRST SEARCH IMPLEMENTATION: g prune


************************************************************************************/



Bool do_anytime_best_first_search( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;

  make_state( &S, gnum_ft_conn, gnum_fl_conn );


  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_best_first_search_space( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {

    if (!memory_check())
      return FALSE;


    if ( (first = lbfs_space_head->next) == NULL ) {

      if ( gcmd_line.display_info ) {
	printf("\n\nsearch space empty!\n\n");
      }
      return FALSE;
    }

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }


    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }



    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      

      /* Raquel: function TIME is accumulative, so it is necessary to
	 initialize &start in this point. Other option is to set gsearch_time
	 to zero*/
      times (&end);
      TIME (gsearch_time);
      times (&start);

      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      if (gcmd_line.stop_at_first_sol)
	break;

    }

     /*include in open the sucessors */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_anytime_best_first_search_space( &S, first->H[i], first );
      }
    }

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;

  }

  return TRUE;

}



void add_to_anytime_best_first_search_space( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  


  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return;
    }


  /* test duplicates 
   */

  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      /* if hashed rep_node has lower g */
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}
      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
      
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }	   
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);
      
      /* remove hashed node from open  */
      if (!rep_node->closed){
	
	rep_node->prev->next = rep_node ->next;
	if (rep_node->next)
	  rep_node->next->prev = rep_node->prev;
      }

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
      
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       
      if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
      
    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

      new_aux.h = get_1P_and_A( S );

      if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {
	new_aux.float_hn = gcost;
	/* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	 * total-time part in metric, it is 0
	 */
	new_aux.float_hn += new_aux.h * gtt;

      }

      if ( new_aux.h == INFINITY ) {
	return;
      }
    }

  /* update f */
  if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }

  /* introduce the current node in OPEN */
  /* search for the position in OPEN 
   */
  
  if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn )
	break;
    }
  }


/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = new_aux.h;
  if (( gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {
    new->float_fn = new_aux.float_fn;
    new->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      new->H = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new->H[j] = gA[j];
      }
      new->num_H = gnum_A;
    }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;
    }


  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );

}


/**********************************************************************************
 Tomas de la Rosa
* ANYTIME BEST FIRST SEARCH IMPLEMENTATION
 FOR CREATING TRACES OF MULTIPLE SOLUTIONS
 REPEATED NODES ARE NOT PRUNEC
************************************************************************************/



Bool do_anytime_bfs_bnb_learn( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;

  make_state( &S, gnum_ft_conn, gnum_fl_conn );


  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }
  add_to_anytime_bfs_bnb_learn_space( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {

    if (!memory_check())
      return FALSE;


    if ( (first = lbfs_space_head->next) == NULL ) {

      if ( gcmd_line.display_info ) {
	printf("\n\nsearch space empty!\n\n");
      }
      return FALSE;
    }

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }


    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
//       if (first->g + first->adm_h > Search_upper_bound){
      if (first->int_fn > Search_upper_bound){
// 	printf("\n Podando in upper bound.. Current [%d] Upper-Bound [%d]",first->int_fn, Search_upper_bound);
        first->closed = TRUE;
 	first->next = lbfs_space_had;
 	lbfs_space_had = first;
 	continue;
      }
    }


    /* stdout*/
    if (Sols_for_learning == NULL){
      if ( LESS( first->h, min ) ) {
	min = first->h;
	if ( sstart ) {
	  if ( gcmd_line.display_info ) {
	    printf("\n\nadvancing to distance: %4d", min);
	    fflush(stdout);
	  }
	  sstart = FALSE;
	} else {
	  if ( gcmd_line.display_info ) {
	    printf("\n                       %4d", min);
	    fflush(stdout);
	  }
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else
	{
	  final_total_cost=first->g;
	  if (first->g < Search_upper_bound){
	    Search_upper_bound = first->g;
	    printf("\n New cost bound [%d]",first->g);
	}
      }

      /* extract and print solution */
      // extract_plan( first );
      

      /* Raquel: function TIME is accumulative, so it is necessary to
	 initialize &start in this point. Other option is to set gsearch_time
	 to zero*/
      times (&end);
      TIME (gsearch_time);
      times (&start);

      //print_plan();  

      add_found_solution( first );
      printf ("\n Found Solution no.:%d",numSolutions);
// //       output_planner_info();
//       if (gcmd_line.o_file){
// 	output_planner_info_to_file();
//       }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
//       if (gcmd_line.stop_at_first_sol)
// 	break;

    }
    
     /*include in open the sucessors */
//     if (first->op != -1){
//       printf("\n Successors of:(%d)[%d]",first->num_H, first->op);
//       print_op_name(first->op);
// 	}
    
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_anytime_bfs_bnb_learn_space( &S, first->H[i], first );
      }
    }

    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_anytime_bfs_bnb_learn_space( &S, first->nonH[i], first );
      }
    }


    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;

  }

  return TRUE;

}



void add_to_anytime_bfs_bnb_learn_space( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *last_child;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;

  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS(final_total_cost, new_aux.float_gn))
      return;
  }
  else
    {
      if (LESS(final_total_cost,new_aux.g))
	return;
    }


  /* test duplicates 
   */

//   if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){
// 
//     if ( gcmd_line.optimize && goptimization_established ) {
// 
//       /* if hashed rep_node has lower g */
//       if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
// 	return;
//  
//      /* else 
//        */
//       
//       /* copy evaluation data in the variables of the new future rep_node to avoid
// 	a re-evaluation
//       */
//       copy_BfsNode_evaluated(rep_node,&new_aux);
// 
//       
//       /* remove hashed node from open  */
//       
//       if (!rep_node->closed)
// 	{
// 	  rep_node->prev->next = rep_node->next;
// 	  if (rep_node->next)
// 	    rep_node->next->prev = rep_node->prev;
// 	}
//       /* unhash hashed rep_node */
//       /* the new future node correspond to the same state and is
// 	 hashed below */
//       
//        unhash_bfs_node(rep_node);
//       
//       /* free memory */
//       /* if the node is closed does not matter: it could be also removed but
// 	  for the moment is not. If you want to remove it (memory
// 	  requirements) be care of the closed list pointer lbfs_space_had */
//        if (!rep_node->closed)
// 	 {
// 	   free (rep_node->S.F); 
// 	   free (rep_node);
// 	 }
//     }	   
//     else {
//       /* if hashed node has lower g */
//       if (rep_node->g <= new_aux.g)
// 	return;
// 
//       /* else 
//        */
//       
//       /* copy evaluation data in the variables of the new future node to avoid
// 	a re-evaluation
//       */
//       copy_BfsNode_evaluated(rep_node,&new_aux);
//       
//       /* remove hashed node from open  */
//       if (!rep_node->closed){
// 	
// 	rep_node->prev->next = rep_node ->next;
// 	if (rep_node->next)
// 	  rep_node->next->prev = rep_node->prev;
//       }
// 
//       /* unhash hashed node */
//       /* the new future node correspond to the same state and will be next
// 	 hashed */
//       
//        unhash_bfs_node(rep_node);
//       
//       /* free memory */
//       /* if the node is closed does not matter: it could be also removed but
// 	  for the moment is not. If you want to remove it (memory
// 	  requirements) be care of the closed list pointer lbfs_space_had */
//        
//       if (!rep_node->closed)
// 	 {
// 	   free (rep_node->S.F); 
// 	   free (rep_node);
// 	 }
//       
//     }
// 
//     already_evaluated = TRUE;
//   }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

      new_aux.h = get_1P_and_H_and_nonH( S );
      new_aux.adm_h = int_admissible_h;

      if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {
	new_aux.float_hn = gcost;
	/* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	 * total-time part in metric, it is 0
	 */
	new_aux.float_hn += new_aux.h * gtt;

      }

      if ( new_aux.h == INFINITY ) {
	return;
      }
    }

  /* update f */
  if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }

  /* introduce the current node in OPEN */
  /* search for the position in OPEN 
   */

// TD.Debug  
//   printf ("\n Node: g:%d ,h:%d , f:%d >> ", new_aux.g, new_aux.h, new_aux.int_fn);
//   if (op != -1){
//     print_op_name(op);
//   }

  if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn )
	break;
    }
  }


/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = new_aux.h;
  if (( gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {
    new->float_fn = new_aux.float_fn;
    new->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new->g = new_aux.g;

  /*Tomas: Recording children for learning stuff */

  if ( father ){
   if (father->children == NULL) {
      
     father->num_children = 1;
     father->children = new;
    }
    else{
      father->num_children++;
      for (last_child = father->children; last_child->child_next; last_child = last_child->child_next);
      last_child->child_next = new;
      
    }
  }
  else{
    //keeping the root node in a global variable
    gSearch_tree = new;
  }
    
    

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;

      new->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new->nonH[j] = gA[j];
      }
      new->num_nonH = gnum_A;

    }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;

      new->num_nonH = new_aux.num_nonH;
      new->nonH= new_aux.nonH;     

    }


  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
//   hash_bfs_node( new );

}





/**********************************************************************************
 * ANYTIME BEST FIRST SEARCH IMPLEMENTATION: helpful actions high priority, g prune

   - Inconsistent heuristic: node re-expansions without new evaluations. Only
     an inconsisten heuristic is used. g is used to prune search space

   - Helpful actions are included first in open. Non-helpful actions are
     included only when the open list is empty

************************************************************************************/



Bool do_anytime_bfs_HA( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;


  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_space( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {
    
    if ( (first = lbfs_space_head->next) == NULL ) {
 
      /* include all nodes in the secondary list*/
      include_list_in_open(lbfs_secondary_current_head,lbfs_secondary_current_tail,
		      add_to_anytime_bfs_HA_space);
      lbfs_secondary_current_head = (BfsNode*)0;
      lbfs_secondary_current_tail = (BfsNode*)0;

      if ( (first = lbfs_space_head->next) == NULL )
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      else continue;
    }

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }


    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }



    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);


      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;


    }

    /*include in open the sucessors due to helpful actions */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_anytime_bfs_HA_space( &S, first->H[i], first );
      }
    }

    /*include in a secondary list sucessors due to non helpful actions */
    
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	add_to_secondary_list( &S, first->nonH[i], first );

      }
    }
    

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;
  



  }

  return TRUE;

}



BfsNode *add_to_anytime_bfs_HA_space( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;  
  


  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      /* if hashed rep_node has lower g */
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return NULL;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}

      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
      
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }	   
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return NULL;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);
      
      /* remove hashed node from open  */
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node ->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {
      /*
      FILE *output_file1; 
      FILE *output_file2; 
      FILE *output_file3; 
      output_file1 = fopen("/home/raquel/planificadores/domains/driverlog03-metric-simple/H1f8", "a+");
      output_file2 = fopen("/home/raquel/planificadores/domains/driverlog03-metric-simple/H2f8", "a+");
      output_file3 = fopen("/home/raquel/planificadores/domains/driverlog03-metric-simple/H5f8", "a+");

      gcmd_line.heuristic=1;
      new_aux.h = get_1P_and_H_and_nonH( S );
      fprintf(output_file1,"\n%s ", gcmd_line.fct_file_name);
      fprintf(output_file1,"%f ", gcost + new_aux.h * gtt);

      gcmd_line.heuristic= 5;
      new_aux.h = get_1P_and_H_and_nonH( S );
      fprintf(output_file3,"\n%f ", gcost + new_aux.h * gtt);

      gcmd_line.heuristic= 2;
      new_aux.h = get_1P_and_H_and_nonH( S );
      fprintf(output_file2,"\n%f ", gcost + new_aux.h * gtt);
      
      fclose(output_file1);
      fclose(output_file2);
      fclose(output_file3);
      */
      
      new_aux.h = get_1P_and_H_and_nonH( S );

      if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_hn = gcost;
	/* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	 * total-time part in metric, it is 0
	 */
	new_aux.float_hn += new_aux.h * gtt;

      }

      if ( new_aux.h == INFINITY ) {
	return NULL;
      }
    }

  /* update f */
  if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }


  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  
  if ( gcmd_line.optimize && goptimization_established ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn)
	break;
    }
  }


/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = new_aux.h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = new_aux.float_fn;
    new->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {

       new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;

      new->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new->nonH[j] = gA[j];
      }
      new->num_nonH = gnum_A;
    }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;
 
      new->num_nonH = new_aux.num_nonH;
      new->nonH= new_aux.nonH;

    }


  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );
  return new;

}






/**********************************************************************************
 * ANYTIME BEST FIRST SEARCH IMPLEMENTATION: helpful actions high priority, g prune

   - Inconsistent heuristic: node re-expansions without new evaluations. Only
     an inconsisten heuristic is used. g is used to prune search space

   - Helpful actions are included first in open. Non-helpful actions are
     included only when the open list is empty

************************************************************************************/
void include_list_in_open(BfsNode *list_head, BfsNode *list_tail, BfsNode* (*F_add_to_open) ( State *S, int op, BfsNode *father ) )
{
  BfsNode *first;
  BfsNode *next;

  first =  list_head;

  while ( first ) {
	 
    F_add_to_open(&(first->S), first->op, first->father);

    next = first->next;


    free (first->S.F); 
    free (first); 

    first = next;
  }
	    
}


void include_list_in_open2(BfsNode *list_head, BfsNode *list_tail, BfsNode* (*F_add_to_open) ( State *S, int op, BfsNode *father ), int max_number_elements )
{
  BfsNode *first;
  BfsNode *next;
  int number_elements = 0;

  first =  list_head;

  while ( first ) {
	 
    F_add_to_open(&(first->S), first->op, first->father);

    next = first->next;


    free (first->S.F); 
    free (first); 

    first = next;

    number_elements ++;

    if (number_elements == max_number_elements)
      { 
	lbfs_secondary_current_head =  next;
	return;
      }
    
  }
  lbfs_secondary_current_head=  (BfsNode*)0;
  lbfs_secondary_current_tail =  (BfsNode*)0;
	    
}


void include_list_in_open3(BfsNode *list_head, BfsNode *list_tail, BfsNode* (*F_add_to_open) ( State *S, int op, BfsNode *father ), int max_number_elements )
{
  BfsNode *first;
  BfsNode *next;
  int number_elements = 0;

  first =  list_head;

  while ( first ) {
	 
    F_add_to_open(&(first->S), first->op, first->father);

    next = first->next;


    free (first->S.F); 
    free (first); 

    first = next;

    number_elements ++;

    if (number_elements == max_number_elements)
      { 
	lbfs_primary_current_head =  next;
	return;
      }
    
  }
  lbfs_primary_current_head=  (BfsNode*)0;
  lbfs_primary_current_tail =  (BfsNode*)0;
	    
}




void include_list_in_open_elements(BfsNode *list_head, BfsNode *list_tail, BfsNode* (*F_add_to_open) ( State *S, int op, BfsNode *father ), int max_number_elements )           
{
  BfsNode *first;
  BfsNode *next;                                                                                                                      
  int number_elements = 0;                                                                                                            
                                                                                                                                      
  first =  list_head;                                                                                                                 
  
  while ( first ) {                                                                                                                   
                                                                                                                                      
    F_add_to_open(&(first->S), first->op, first->father);                                                                             
                                                                                                                                      
    next = first->next;                                                                                                               
                                                                                                                                      
                                                                                                                                      
    free (first->S.F);                                                                                                                
    free (first);                                                                                                                     
                                                                                                                                      
    first = next;                                                                                                                     
                                                                                                                                      
    number_elements ++;                                                                                                               
                                                                                                                                      
    if (number_elements == max_number_elements)                                                                                       
      {                                                                                                                               
        lbfs_secondary_current_head =  next;                                                                                          
        return;                                                                                                                       
      }                                                                                                                               

    if (first == NULL)                                                                                       
      {                                                                                                                               
        lbfs_secondary_current_head =  NULL;                                                                                          
        return;                                                                                                                       
      }                                                                                                                               

                                                                                                                                      
  }                                                                                                                                   
  /* lbfs_secondary_current_head=  (BfsNode*)0;                                                                                          
  lbfs_secondary_current_tail =  (BfsNode*)0;                                                                                         
  */
                                                                                                                                      
}                  



Bool do_anytime_bfs_HA_lookahead( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  BfsNode *lookahead, *added_node;
  Bool primary =TRUE;


  deactivate_actions_deleting_top_level_logic_goals();

  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_space_lookahead( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {
    
    if ( (first = lbfs_space_head->next) == NULL) {

      if (!lbfs_primary_current_head && !lbfs_secondary_current_head)
	{
	  
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      
      if (primary)
	{
	  if (lbfs_primary_current_head)
	    {
	      //	      printf("\nincluding primary");
	      //	      fflush(stdout);
	      include_list_in_open( lbfs_primary_current_head,lbfs_primary_current_tail,
				    add_to_anytime_bfs_HA_space_lookahead );
	      lbfs_primary_current_head = (BfsNode*)0;
	      lbfs_primary_current_tail = (BfsNode*)0;

	    }
	  primary = FALSE;
	  continue;

	}
      else 
	{
	  if (lbfs_secondary_current_head)
	    {
	      //	      printf("\nincluding secondary");
	      //	      fflush(stdout);
	      include_list_in_open( lbfs_secondary_current_head,lbfs_secondary_current_tail,
			   add_to_anytime_bfs_HA_space_lookahead);
	      lbfs_secondary_current_head = (BfsNode*)0;
	      lbfs_secondary_current_tail = (BfsNode*)0;

	    }
	  primary = TRUE;
	  continue;
	}
    }

    //    printf(".");

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }


    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }



    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);

      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;


    }

    /* generate lookahead states */
    if (!first->is_lookahead)
      lookahead = obtain_lookahead_state_order_rep(first);
    else
      lookahead = NULL;

    while (lookahead)
       {
	added_node= add_to_anytime_bfs_HA_space_lookahead( &(lookahead->S), 
						  lookahead->op, lookahead->father );

	if (added_node){
	  added_node->is_lookahead = TRUE;
	  /*
	  printf("\nfather: %f, added:%f,%f", first->float_hn, added_node->float_hn, added_node->float_gn);
	  */
	  /* compute the solution cost: final_total_cost  */
	  if ( added_node->h == 0 ) {
	    if ( gcmd_line.optimize && goptimization_established ){
	      final_total_cost=added_node->float_gn;
	    }
	    else{
	      final_total_cost=added_node->g;
	    }

	  /* extract and print solution */
	    extract_plan( added_node);
      
	    times (&end);
	    TIME (gsearch_time);
	    times (&start);

	    print_plan();  
	    output_planner_info();
	    if (gcmd_line.o_file){
	      output_planner_info_to_file();
	    }

	    sstart = TRUE;
	    min = INFINITY;

	    if (gcmd_line.stop_at_first_sol)
	      return TRUE;
	  }

	  lookahead = obtain_lookahead_state_order_rep(added_node);
	}

	else
	  break;
       }
    //    printf("\n--");
     
    /*include in open the sucessors due to helpful actions */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */

	//add_to_anytime_bfs_HA_space_lookahead( &S, first->H[i], first );
	add_to_primary_list( &S, first->H[i], first );
      }
    }

    /*include in a secondary list sucessors due to non helpful actions */
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->nonH[i], first );
	//add_to_anytime_bfs_HA_space_lookahead( &S, first->nonH[i], first );
      }
    }

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;

   
  }


  return TRUE;

}



BfsNode* add_to_anytime_bfs_HA_space_lookahead( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  


  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      /* if hashed rep_node has lower g */
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return rep_node;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}
      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
      
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }	   
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return NULL;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);
      
      /* remove hashed node from open  */
       if (!rep_node->closed)
	 {
	   rep_node->prev->next = rep_node ->next;
	   if (rep_node->next)
	     rep_node->next->prev = rep_node->prev;
	 }

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

      new_aux.h = get_1P_and_H_and_nonH( S );

      if (new_aux.h == INFINITY){
	 activate_actions_deleting_top_level_logic_goals();
	 new_aux.h = get_1P_and_H_and_nonH( S );
	 deactivate_actions_deleting_top_level_logic_goals();
       }


      if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_hn = gcost;
	/* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	 * total-time part in metric, it is 0
	 */
	new_aux.float_hn += new_aux.h * gtt;

      }

      if ( new_aux.h == INFINITY ) {
	return NULL;
      }
    }

  /* update f */
  if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }


  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  
  if ( gcmd_line.optimize && goptimization_established ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn)
	break;
    }
  }


/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = new_aux.h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = new_aux.float_fn;
    new->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;

      new->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new->nonH[j] = gA[j];
      }
      new->num_nonH = gnum_A;

      /* relaxed plan */
      new->g_RP = ( int * ) calloc( gnum_RP, sizeof( int ) );
      new->RPorder = ( int * ) calloc( gnum_RP, sizeof( int ) );
      new->max_RP_order = 0;
      new->min_RP_order = INFINITY;
      for ( j = 0; j < gnum_RP; j++ ) {
	new->g_RP[j] = g_RP[j];
	//	new->RPorder[j] = gop_conn[g_RP[j]].level_required -
	//	gop_conn[g_RP[j]].level_achievable;
	new->RPorder[j] = gop_conn[g_RP[j]].level_required_prop - gop_conn[g_RP[j]].level_achievable;
	// - gop_conn[g_RP[j]].level_achievable ;
	//new->RPorder[j] = gop_conn[g_RP[j]].level_required - gop_conn[g_RP[j]].level_achievable;
	if (new->RPorder[j] > new->max_RP_order)
	  new->max_RP_order = new->RPorder[j];
	if (LESS(new->RPorder[j], new->min_RP_order))
	  new->min_RP_order = new->RPorder[j];

      }
      new->gnum_RP = gnum_RP;

    }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;

      new->num_nonH = new_aux.num_nonH;
      new->nonH= new_aux.nonH;
     
      /* relaxed plan */
      new->g_RP = new_aux.g_RP;
      new->gnum_RP = new_aux.gnum_RP;
      new->RPorder = new_aux.RPorder;
      new->max_RP_order = new_aux.max_RP_order;
      new->min_RP_order = new_aux.min_RP_order;


    }


  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );

  return new;

}











Bool do_anytime_bfs_HA_lookahead2( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  BfsNode *lookahead, *added_node, *last_added_node;
  Bool primary = FALSE;
  int iterations;


  deactivate_actions_deleting_top_level_logic_goals();

  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_space_lookahead2( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {
    
    if ( (first = lbfs_space_head->next) == NULL) {

      if (!lbfs_primary_current_head && !lbfs_secondary_current_head)
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      
      if (primary)
	{
	  if (lbfs_primary_current_head)
	    {
	      //	      printf("\nincluding primary");
	      fflush(stdout);
	      include_list_in_open( lbfs_primary_current_head,lbfs_primary_current_tail,
				     add_to_anytime_bfs_HA_space_lookahead2);
	      lbfs_primary_current_head = (BfsNode*)0;
	      lbfs_primary_current_tail = (BfsNode*)0;

	    }
	  else
	    primary = FALSE;
	  continue;

	}
      else 
	{
	  if (lbfs_secondary_current_head)
	    {
	      //	      printf("\nincluding secondary");
	      fflush(stdout);
	      include_list_in_open( lbfs_secondary_current_head,lbfs_secondary_current_tail,
				     add_to_anytime_bfs_HA_space_lookahead2);
	      lbfs_secondary_current_head = (BfsNode*)0;
	      lbfs_secondary_current_tail = (BfsNode*)0;

	    }
	  //primary = TRUE;
	  primary = FALSE;
	  continue;
	}
    }

 
    //    printf(".");

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }


    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }

    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);

      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;


    }

    /* generate lookahead states */
    if (!first->is_lookahead)
      {
	//      printf("\n-- expanding non look");
      lookahead = obtain_lookahead_state_order_rep(first);
     
      }

    else
      {
      lookahead = NULL;
      /*
      if (lbfs_secondary_current_head)
      {
	printf("\nincluding 1 sec");
	include_list_in_open2( lbfs_secondary_current_head,lbfs_secondary_current_tail,
			     add_to_anytime_bfs_HA_space_lookahead2,1);
      }
      */

      }
    iterations = 0;
    last_added_node = first;
    added_node = NULL;
    while (lookahead)
       {
	 iterations++;
	 //	 printf("\n+");
	added_node= add_to_anytime_bfs_HA_space_lookahead2( &(lookahead->S), 
						  lookahead->op, lookahead->father );

	if (added_node){
	  added_node->is_lookahead = TRUE;
	  /*
	  printf("\nfather: %f, added:%f,%f", first->float_hn, added_node->float_hn, added_node->float_gn);
	  */
	  /* compute the solution cost: final_total_cost  */
	  if ( added_node->h == 0 ) {
	    if ( gcmd_line.optimize && goptimization_established ){
	      final_total_cost=added_node->float_gn;
	    }
	    else{
	      final_total_cost=added_node->g;
	    }

	  /* extract and print solution */
	    extract_plan( added_node);
      
	    times (&end);
	    TIME (gsearch_time);
	    times (&start);

	    print_plan();  
	    output_planner_info();
	    if (gcmd_line.o_file){
	      output_planner_info_to_file();
	    }

	    sstart = TRUE;
	    min = INFINITY;

	    if (gcmd_line.stop_at_first_sol)
	      return TRUE;
	  }
	  //if (LESS(added_node->float_hn, last_added_node->float_hn))
	      lookahead = obtain_lookahead_state_order_rep(added_node);
	      //else
	      //break;
	      
	}
	else
	  {
	    //	    printf("\n--look fail");

	    break;

	  }
	//	printf("\n--");
       }

    //    printf("\nit:%d", iterations);


    /*include in open the sucessors due to helpful actions */
    //qsort(first->H, first->num_H, sizeof(int), compare_helpful_actions);
    //int number = 0;
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	//add_to_primary_list( &S, first->nonH[i], first );
		
	  add_to_anytime_bfs_HA_space_lookahead2( &S, first->H[i], first );
	
	
	
      }
    }

    /*include in a secondary list sucessors due to non helpful actions */
    //qsort(first->nonH, first->num_nonH, sizeof(int), compare_helpful_actions);
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	//add_to_anytime_bfs_HA_space_lookahead2( &S, first->nonH[i], first );
	add_to_secondary_list( &S, first->nonH[i], first );
	//add_to_anytime_bfs_HA_space_lookahead2( &S, first->nonH[i], first );

      }
    }
    /*
    if (lbfs_primary_current_head)
	    {
	      include_list_in_open3( lbfs_primary_current_head,lbfs_primary_current_tail,
				     add_to_anytime_bfs_HA_space_lookahead2,1);
	    }

    */
    /*
    if (lbfs_secondary_current_head)
	    {
	      include_list_in_open2( lbfs_secondary_current_head,lbfs_secondary_current_tail,
				     add_to_anytime_bfs_HA_space_lookahead2,1);
	    }
    */   
    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;

   
  }


  return TRUE;

}



BfsNode* add_to_anytime_bfs_HA_space_lookahead2( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  


  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      /* if hashed rep_node has lower g */
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return rep_node;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}
      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
      
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }	   
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return rep_node;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);
      
      /* remove hashed node from open  */
       if (!rep_node->closed)
	 {
	   rep_node->prev->next = rep_node ->next;
	   if (rep_node->next)
	     rep_node->next->prev = rep_node->prev;
	 }

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

      new_aux.h = get_1P_and_H_and_nonH( S );
      
      if (new_aux.h == INFINITY){
	 activate_actions_deleting_top_level_logic_goals();
	 new_aux.h = get_1P_and_H_and_nonH( S );
	 deactivate_actions_deleting_top_level_logic_goals();
       }
     

      if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_hn = gcost;
	/* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	 * total-time part in metric, it is 0
	 */
	new_aux.float_hn += new_aux.h * gtt;

      }

      if ( new_aux.h == INFINITY ) {
	return NULL;
      }

      if ( new_aux.h == 0 ) {
	printf("\nsol included");
      }


    }

  /* update f */
  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }


  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
 
  if ( gcmd_line.optimize && goptimization_established ) {

	for ( i = lbfs_space_head; i->next; i = i->next ) {

	    if ( LESS_EQ(new_aux.float_fn  , i->next->float_fn ))
	    break;
        }
  
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn)
	break;
    }
  }


/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = new_aux.h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = new_aux.float_fn;
    new->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;

      new->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new->nonH[j] = gA[j];
      }
      new->num_nonH = gnum_A;

      /* relaxed plan */
      new->g_RP = ( int * ) calloc( gnum_RP, sizeof( int ) );
      new->RPorder = ( int * ) calloc( gnum_RP, sizeof( int ) );
      new->max_RP_order = 0;
      new->min_RP_order = INFINITY;
      for ( j = 0; j < gnum_RP; j++ ) {
	new->g_RP[j] = g_RP[j];
	//	new->RPorder[j] = gop_conn[g_RP[j]].level_required -
	//	gop_conn[g_RP[j]].level_achievable;
	//	new->RPorder[j] = gop_conn[g_RP[j]].level_required_prop - gop_conn[g_RP[j]].level_achievable;
	new->RPorder[j] = gop_conn[g_RP[j]].level_required_prop;
	// - gop_conn[g_RP[j]].level_achievable ;
	//new->RPorder[j] = gop_conn[g_RP[j]].level_required - gop_conn[g_RP[j]].level_achievable;
	if (new->RPorder[j] > new->max_RP_order)
	  new->max_RP_order = new->RPorder[j];
	if (LESS(new->RPorder[j], new->min_RP_order))
	  new->min_RP_order = new->RPorder[j];

      }
      new->gnum_RP = gnum_RP;

    }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;

      new->num_nonH = new_aux.num_nonH;
      new->nonH= new_aux.nonH;
     
      /* relaxed plan */
      new->g_RP = new_aux.g_RP;
      new->gnum_RP = new_aux.gnum_RP;
      new->RPorder = new_aux.RPorder;
      new->max_RP_order = new_aux.max_RP_order;
      new->min_RP_order = new_aux.min_RP_order;


    }


  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );

  return new;

}




Bool do_anytime_bfs_HA_lookahead3( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  BfsNode *lookahead, *added_node;
  Bool primary = FALSE;
  int iterations;


  deactivate_actions_deleting_top_level_logic_goals();

  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_space_lookahead3( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {
    
    if ( (first = lbfs_space_head->next) == NULL) {

      if (!lbfs_primary_current_head && !lbfs_secondary_current_head)
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      
      if (primary)
	{
	  if (lbfs_primary_current_head)
	    {
	      printf("\nincluding primary");
	      fflush(stdout);
	      include_list_in_open( lbfs_primary_current_head,lbfs_primary_current_tail,
				     add_to_anytime_bfs_HA_space_lookahead3);
	      lbfs_primary_current_head = (BfsNode*)0;
	      lbfs_primary_current_tail = (BfsNode*)0;

	    }
	  //	  primary = FALSE;
	  continue;

	}
      else 
	{
	  if (lbfs_secondary_current_head)
	    {
	      printf("\nincluding secondary");
	      fflush(stdout);
	      include_list_in_open2( lbfs_secondary_current_head,lbfs_secondary_current_tail,
				     add_to_anytime_bfs_HA_space_lookahead3,10);
	      //lbfs_secondary_current_head = (BfsNode*)0;
	      //lbfs_secondary_current_tail = (BfsNode*)0;

	    }
	  //	  primary = TRUE;
	  primary = FALSE;
	  continue;
	}
    }

 
    //    printf(".");

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }


    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }

    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);

      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;


    }

    /* generate lookahead states */
    if (!first->is_lookahead)
      {
	//      printf("\n-- expanding non look");
      lookahead = obtain_lookahead_state_order_rep(first);
     
      }

    else
      {
      lookahead = NULL;
      /*
      if (lbfs_secondary_current_head)
      {
	printf("\nincluding 1 sec");
	include_list_in_open2( lbfs_secondary_current_head,lbfs_secondary_current_tail,
			     add_to_anytime_bfs_HA_space_lookahead3,1);
      }
      */

      }
    iterations = 0;
    while (lookahead)
       {
	 iterations++;
	 //	 printf("\n+");
	added_node= add_to_anytime_bfs_HA_space_lookahead3( &(lookahead->S), 
						  lookahead->op, lookahead->father );

	if (added_node){
	  added_node->is_lookahead = TRUE;
	  /*
	  printf("\nfather: %f, added:%f,%f", first->float_hn, added_node->float_hn, added_node->float_gn);
	  */
	  /* compute the solution cost: final_total_cost  */
	  if ( added_node->h == 0 ) {
	    if ( gcmd_line.optimize && goptimization_established ){
	      final_total_cost=added_node->float_gn;
	    }
	    else{
	      final_total_cost=added_node->g;
	    }

	  /* extract and print solution */
	    extract_plan( added_node);
      
	    times (&end);
	    TIME (gsearch_time);
	    times (&start);

	    print_plan();  
	    output_planner_info();
	    if (gcmd_line.o_file){
	      output_planner_info_to_file();
	    }

	    sstart = TRUE;
	    min = INFINITY;

	    if (gcmd_line.stop_at_first_sol)
	      return TRUE;
	  }

	    lookahead = obtain_lookahead_state_order_rep(added_node);
	}
	else
	  {
	    //	    printf("\n--look fail");

	    break;

	  }
	//	printf("\n--");
       }

    //    printf("\nit:%d", iterations);

    /*include in open the sucessors due to helpful actions */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */

	add_to_anytime_bfs_HA_space_lookahead( &S, first->H[i], first );
      }
    }

    /*include in a secondary list sucessors due to non helpful actions */
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->nonH[i], first );
	//add_to_anytime_bfs_HA_space_lookahead( &S, first->nonH[i], first );

      }
    }
    /*
    if (lbfs_secondary_current_head)
	    {
	      include_list_in_open2( lbfs_secondary_current_head,lbfs_secondary_current_tail,
				     add_to_anytime_bfs_HA_space_lookahead3,1);
	    }
    */
    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;

   
  }


  return TRUE;

}



BfsNode* add_to_anytime_bfs_HA_space_lookahead3( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  


  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      /* if hashed rep_node has lower g */
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return rep_node;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}
      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
      
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }	   
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return rep_node;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);
      
      /* remove hashed node from open  */
       if (!rep_node->closed)
	 {
	   rep_node->prev->next = rep_node ->next;
	   if (rep_node->next)
	     rep_node->next->prev = rep_node->prev;
	 }

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

      new_aux.h = get_1P_and_H_and_nonH( S );
      
      if (new_aux.h == INFINITY){
	 activate_actions_deleting_top_level_logic_goals();
	 new_aux.h = get_1P_and_H_and_nonH( S );
	 deactivate_actions_deleting_top_level_logic_goals();
       }
     

      if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_hn = gcost;
	/* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	 * total-time part in metric, it is 0
	 */
	new_aux.float_hn += new_aux.h * gtt;

      }

      if ( new_aux.h == INFINITY ) {
	return NULL;
      }

    }

  /* update f */
  if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) *  new_aux.h * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }


  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  
  if ( gcmd_line.optimize && goptimization_established ) {

	for ( i = lbfs_space_head; i->next; i = i->next ) {
	  if ( LESS_EQ(new_aux.float_fn , i->next->float_fn ))
	    break;
        }
  
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn)
	break;
    }
  }


/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = new_aux.h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = new_aux.float_fn;
    new->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;

      new->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new->nonH[j] = gA[j];
      }
      new->num_nonH = gnum_A;

      /* relaxed plan */
      new->g_RP = ( int * ) calloc( gnum_RP, sizeof( int ) );
      new->RPorder = ( int * ) calloc( gnum_RP, sizeof( int ) );
      new->max_RP_order = 0;
      new->min_RP_order = INFINITY;
      for ( j = 0; j < gnum_RP; j++ ) {
	new->g_RP[j] = g_RP[j];
	//	new->RPorder[j] = gop_conn[g_RP[j]].level_required -
	//	gop_conn[g_RP[j]].level_achievable;
	//	new->RPorder[j] = gop_conn[g_RP[j]].level_required_prop - gop_conn[g_RP[j]].level_achievable;
	new->RPorder[j] = gop_conn[g_RP[j]].level_required_prop;
	// - gop_conn[g_RP[j]].level_achievable ;
	//new->RPorder[j] = gop_conn[g_RP[j]].level_required - gop_conn[g_RP[j]].level_achievable;
	if (new->RPorder[j] > new->max_RP_order)
	  new->max_RP_order = new->RPorder[j];
	if (LESS(new->RPorder[j], new->min_RP_order))
	  new->min_RP_order = new->RPorder[j];

      }
      new->gnum_RP = gnum_RP;

    }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;

      new->num_nonH = new_aux.num_nonH;
      new->nonH= new_aux.nonH;
     
      /* relaxed plan */
      new->g_RP = new_aux.g_RP;
      new->gnum_RP = new_aux.gnum_RP;
      new->RPorder = new_aux.RPorder;
      new->max_RP_order = new_aux.max_RP_order;
      new->min_RP_order = new_aux.min_RP_order;


    }


  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );

  return new;

}







Bool do_anytime_bfs_HA_lookahead4( void )

{

  BfsNode *first;
  State S;
  int i, j, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  BfsNode *lookahead, *added_node;
  Bool primary = TRUE;
  int iterations;


  deactivate_actions_deleting_top_level_logic_goals();

  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_space_lookahead4( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {
    
    if ( (first = lbfs_space_head->next) == NULL) {

      if (!lbfs_primary_current_head && !lbfs_secondary_current_head)
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      
      if (primary)
	{
	  if (lbfs_primary_current_head)
	    {
	      //	      printf("\nincluding primary");
	      //lush(stdout);
	      include_list_in_open(lbfs_primary_current_head,lbfs_primary_current_tail, 
				   add_to_anytime_bfs_HA_space_lookahead4);
	      //			     add_to_anytime_bfs_HA_space_lookahead4,100);
	      //include_list_in_open3( lbfs_primary_current_head,lbfs_primary_current_tail,
	      //			     add_to_anytime_bfs_HA_space_lookahead4,100);
	       lbfs_primary_current_head = (BfsNode*)0;
	      lbfs_primary_current_tail = (BfsNode*)0;
	      //printf(" ok");

	    }
	  else
	    {
	      primary = FALSE;
	    }
	  continue;

	}
      else 
	{
	  if (lbfs_secondary_current_head)
	    {
	      //	      printf("\nincluding secondary");
	      //fflush(stdout);
	      include_list_in_open2( lbfs_secondary_current_head,lbfs_secondary_current_tail,
				     add_to_anytime_bfs_HA_space_lookahead4,10);
	      //lbfs_secondary_current_head = (BfsNode*)0;
	      //lbfs_secondary_current_tail = (BfsNode*)0;

	    }
	  primary = TRUE;
	  //primary = FALSE;
	  continue;
	}
    }

 
    //    printf(".");

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }


    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }


    /* evaluate */
    first->h = get_1P_and_H_and_nonH( &(first->S) );
    if (first->h == INFINITY){
	 activate_actions_deleting_top_level_logic_goals();
	 first->h = get_1P_and_H_and_nonH( &(first->S) );
	 deactivate_actions_deleting_top_level_logic_goals();
       }

    if ( gcmd_line.optimize && goptimization_established ) {
	first->float_hn = gcost;
	/* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	 * total-time part in metric, it is 0
	 */
	first->float_hn += first->h * gtt;

      }

    if ( first->h == INFINITY ) {
      continue;
    }
    /*
    if ( first->father && LESS(first->father->float_hn, first->float_hn )) {
      continue;
      }*/

    /*

    if (first->father && LESS(first->father->float_hn,first->float_hn))
      continue;
    */

    /* update f */
    if ( gcmd_line.optimize && goptimization_established ) {
      first->float_fn = (((float) gcmd_line.g_weight) * first->float_gn) + 
	(((float) gcmd_line.h_weight) *  first->h * first->float_hn);

    }
    else {
      first->int_fn = (gcmd_line.g_weight * first->g) + (gcmd_line.h_weight * first->h);
    }

    /* update relaxed plan */

      first->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	first->H[j] = gH[j];
      }
      first->num_H = gnum_H;

      first->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	first->nonH[j] = gA[j];
      }
      first->num_nonH = gnum_A;

      /* relaxed plan */
      first->g_RP = ( int * ) calloc( gnum_RP, sizeof( int ) );
      first->RPorder = ( int * ) calloc( gnum_RP, sizeof( int ) );
      first->max_RP_order = 0;
      first->min_RP_order = INFINITY;
      for ( j = 0; j < gnum_RP; j++ ) {
	first->g_RP[j] = g_RP[j];
	//	first->RPorder[j] = gop_conn[g_RP[j]].level_required -
	//	gop_conn[g_RP[j]].level_achievable;
	first->RPorder[j] = gop_conn[g_RP[j]].level_required_prop - gop_conn[g_RP[j]].level_achievable;
	//first->RPorder[j] = gop_conn[g_RP[j]].level_required_prop;
	// - gop_conn[g_RP[j]].level_achievable ;
	//first->RPorder[j] = gop_conn[g_RP[j]].level_required - gop_conn[g_RP[j]].level_achievable;
	if (first->RPorder[j] > first->max_RP_order)
	  first->max_RP_order = first->RPorder[j];
	if (LESS(first->RPorder[j], first->min_RP_order))
	  first->min_RP_order = first->RPorder[j];

      }
      first->gnum_RP = gnum_RP;




    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);

      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;


    }

    /* generate lookahead states */
    if (!first->is_lookahead /*&& (!first->father || LESS(first->float_hn, first->father->float_hn))*/)
      {
	//      printf("\n-- expanding non look");
      lookahead = obtain_lookahead_state_order_rep(first);
     
      }

    else
      {
      lookahead = NULL;
      /*
      if (lbfs_secondary_current_head)
      {
	printf("\nincluding 1 sec");
	include_list_in_open2( lbfs_secondary_current_head,lbfs_secondary_current_tail,
			     add_to_anytime_bfs_HA_space_lookahead2,1);
      }
      */

      }
    iterations = 0;
    while (lookahead)
       {
	 iterations++;
	 //	 printf("\n+");
	added_node= add_to_anytime_bfs_HA_space_lookahead4( &(lookahead->S), 
						  lookahead->op, lookahead->father );


	if (added_node){
	  added_node->is_lookahead = TRUE;
	  /*
	  printf("\nfather: %f, added:%f,%f", first->float_hn, added_node->float_hn, added_node->float_gn);
	  */




	  /* evaluate */
	  added_node->h = get_1P_and_H_and_nonH( &(added_node->S) );
	  if (added_node->h == INFINITY){
	    activate_actions_deleting_top_level_logic_goals();
	    added_node->h = get_1P_and_H_and_nonH( &(added_node->S) );
	    deactivate_actions_deleting_top_level_logic_goals();
	  }

	  if ( gcmd_line.optimize && goptimization_established ) {
	    added_node->float_hn = gcost;
	    /* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	     * total-time part in metric, it is 0
	     */
	    added_node->float_hn += added_node->h * gtt;

	  }

	  if ( added_node->h == INFINITY ) {
	    continue;
	  }

	  /* update f */
	  if ( gcmd_line.optimize && goptimization_established ) {
	    added_node->float_fn = (((float) gcmd_line.g_weight) * added_node->float_gn) + 
	      (((float) gcmd_line.h_weight) *  added_node->h * added_node->float_hn);
	    
	  }
	  else {
	    added_node->int_fn = (gcmd_line.g_weight * added_node->g) + (gcmd_line.h_weight * added_node->h);
	  }

	  /* update relaxed plan */

	  added_node->H = ( int * ) calloc( gnum_H, sizeof( int ) );
	  for ( j = 0; j < gnum_H; j++ ) {
	    added_node->H[j] = gH[j];
	  }
	  added_node->num_H = gnum_H;

	  added_node->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
	  for ( j = 0; j < gnum_A; j++ ) {
	    added_node->nonH[j] = gA[j];
	  }
	  added_node->num_nonH = gnum_A;
	  
	  /* relaxed plan */
	  added_node->g_RP = ( int * ) calloc( gnum_RP, sizeof( int ) );
	  added_node->RPorder = ( int * ) calloc( gnum_RP, sizeof( int ) );
	  added_node->max_RP_order = 0;
	  added_node->min_RP_order = INFINITY;
	  for ( j = 0; j < gnum_RP; j++ ) {
	    added_node->g_RP[j] = g_RP[j];
	    //	added_node->RPorder[j] = gop_conn[g_RP[j]].level_required -
	    //	gop_conn[g_RP[j]].level_achievable;
	    //	added_node->RPorder[j] = gop_conn[g_RP[j]].level_required_prop - gop_conn[g_RP[j]].level_achievable;
	    added_node->RPorder[j] = gop_conn[g_RP[j]].level_required_prop;
	    // - gop_conn[g_RP[j]].level_achievable ;
	    //added_node->RPorder[j] = gop_conn[g_RP[j]].level_required - gop_conn[g_RP[j]].level_achievable;
	    if (added_node->RPorder[j] > added_node->max_RP_order)
	      added_node->max_RP_order = added_node->RPorder[j];
	    if (LESS(added_node->RPorder[j], added_node->min_RP_order))
	      added_node->min_RP_order = added_node->RPorder[j];
	    
	  }
	  added_node->gnum_RP = gnum_RP;


	  /* compute the solution cost: final_total_cost  */
	  if ( added_node->h == 0 ) {
	    if ( gcmd_line.optimize && goptimization_established ){
	      final_total_cost=added_node->float_gn;
	    }
	    else{
	      final_total_cost=added_node->g;
	    }

	  /* extract and print solution */
	    extract_plan( added_node);
      
	    times (&end);
	    TIME (gsearch_time);
	    times (&start);

	    print_plan();  
	    output_planner_info();
	    if (gcmd_line.o_file){
	      output_planner_info_to_file();
	    }

	    sstart = TRUE;
	    min = INFINITY;

	    if (gcmd_line.stop_at_first_sol)
	      return TRUE;
	  }

	    lookahead = obtain_lookahead_state_order_rep(added_node);
	}
	else
	  {
	    //	    printf("\n--look fail");

	    break;

	  }
	//	printf("\n--");
       }

    //    printf("\nit:%d", iterations);

    /*include in open the sucessors due to helpful actions */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */

	//add_to_anytime_bfs_HA_space_lookahead4( &S, first->H[i], first );
	add_to_primary_list( &S, first->H[i], first );
      }
    }

    /*include in a secondary list sucessors due to non helpful actions */
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->nonH[i], first );
	//add_to_anytime_bfs_HA_space_lookahead( &S, first->nonH[i], first );

      }
    }
    
    /*    
    if (lbfs_secondary_current_head)
	    {
	      include_list_in_open2( lbfs_secondary_current_head,lbfs_secondary_current_tail,
				     add_to_anytime_bfs_HA_space_lookahead4,1);
	    }
    */  
    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;

   
  }


  return TRUE;

}



BfsNode* add_to_anytime_bfs_HA_space_lookahead4( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;

  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
   }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      /* if hashed rep_node has lower g */
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return rep_node;
 
    }	   
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return rep_node;
    }
  }
  
   /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  /*
  if (father)
    {
      if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) *  father->h * father->float_hn);

      }
      else {
	new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * father->h);
      }
    }
  else
    {
      new_aux.float_fn = new_aux.float_gn;
      new_aux.int_fn =  new_aux.g;
    }
  */

  if ( gcmd_line.optimize && goptimization_established ) {

	for ( i = lbfs_space_head; i->next; i = i->next ) {
	  /* if ( LESS_EQ(new_aux.float_gn , i->next->float_gn ))*/
	   if ( LESS_EQ(i->next->float_gn ,new_aux.float_gn))

	    break;
        }
  
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->g >= new_aux.g)
	break;
    }
  }
 

/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_gn = new_aux.float_gn;
    new->float_fn = new_aux.float_fn;
 
  }
  new->father = father;
  new->g = new_aux.g;

  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );

  return new;

}






/**********************************************************************************
 * ANYTIME BEST FIRST SEARCH IMPLEMENTATION: helpful actions high priority,
 * admissible heuristic prune

   - Inconsistent heuristic: node re-expansions without new evaluations. g is
     used to prune search space. An admissible heuristic is used to compute
     the error

   - Helpful actions are included first in open. Non-helpful actions are
     included only when the open list is empty

************************************************************************************/



Bool do_anytime_bfs_HA_adm( void )

{

  BfsNode *first, *next;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  float min_adm_open = INFINITY;

  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_adm_space( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {
    

    if ( (first = lbfs_space_head->next) == NULL ) {
      /* include all nodes in the secondary list*/
      include_list_in_open(lbfs_secondary_current_head,lbfs_secondary_current_tail,
		      add_to_anytime_bfs_HA_adm_space);
      lbfs_secondary_current_head = (BfsNode*)0;
      lbfs_secondary_current_tail = (BfsNode*)0;

      if ( (first = lbfs_space_head->next) == NULL )
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      else continue;

    }

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    /* prune first by h admissible values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn + first->adm_float_hn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g + first->adm_h)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }


    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_fn;

	if (!lbfs_space_head->next)
	  {
	    printf("\n\nERROR: 0");
	  }
	else
	  {
	    min_adm_open = INFINITY;

	    next = lbfs_space_head->next;
	    while(next)
	      {
		if (LESS(next->adm_float_hn, min_adm_open)){
		  min_adm_open = next->adm_float_hn;
		}
		next = next->next;
	      }
	    
	    printf("\n\nERROR: %f", final_total_cost-min_adm_open);
	  }

	  

      }
      else{
	final_total_cost=first->int_fn;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);

      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;


    }

    /*include in open the sucessors due to helpful actions */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_anytime_bfs_HA_adm_space( &S, first->H[i], first );
      }
    }

    /*include in a secondary list sucessors due to non helpful actions */
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->nonH[i], first );
      }
    }

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;


 

  }

  return TRUE;

}



BfsNode *add_to_anytime_bfs_HA_adm_space( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  

  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      /* if hashed rep_node has lower g */
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return NULL;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);
      new_aux.adm_h = rep_node->adm_h;
      new_aux.adm_float_hn = rep_node->adm_float_hn;
      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}
      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
      
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }	   
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return NULL;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);
      new_aux.adm_h = rep_node->adm_h;

      /* remove hashed node from open  */
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node ->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

      new_aux.h = get_1P_and_H_and_nonH( S );
      new_aux.adm_h = int_admissible_h;

      if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_hn = gcost;
	/* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	 * total-time part in metric, it is 0
	 */
	new_aux.float_hn += new_aux.h * gtt;

	/* gtt is yet summed up*/
	new_aux.adm_float_hn = float_admissible_h;

      }

      if ( new_aux.h == INFINITY ) {
	return NULL;
      }
    }

  /* update f */
  if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }


  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  
  if ( gcmd_line.optimize && goptimization_established ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn) 
	break;
    }
  }


/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = new_aux.h;
  new->adm_h = new_aux.adm_h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = new_aux.float_fn;
    new->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
    new->adm_float_hn = new_aux.adm_float_hn;
  } else {
    new->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;

      new->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new->nonH[j] = gA[j];
      }
      new->num_nonH = gnum_A;
    }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;

      new->num_nonH = new_aux.num_nonH;
      new->nonH= new_aux.nonH;

    }


  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );
  return new;

}






/**********************************************************************************
 * ANYTIME BEST FIRST SEARCH IMPLEMENTATION: each node is included in open
 * twice. Once with helpful actions and once with non helpful (rescue) actions
 * as in Vidal ICAPS04

   - Inconsistent heuristic: node re-expansions without new evaluations. Only
     an inconsistent heuristic is used. g is used to prune search space

   - Rescue nodes are not hashed


************************************************************************************/



Bool do_anytime_bfs_HA_rescue( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;

  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_rescue_space( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {
    

    if ( (first = lbfs_space_head->next) == NULL ) {

      if ( gcmd_line.display_info ) {
	printf("\n\nsearch space empty!\n\n");
      }
      return FALSE;
    }

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }


    /* prune first by g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }



    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);

      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;


    }

    

    /*include in open the sucessors due to helpful actions */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_anytime_bfs_HA_rescue_space( &S, first->H[i], first );
      }
    }

    /*include in a sucessors due to non helpful actions */
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_anytime_bfs_HA_rescue_space( &S, first->nonH[i], first );
      }
    }

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;

 
  }

  return TRUE;

}



void add_to_anytime_bfs_HA_rescue_space( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node, *new_rescue;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  

  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( (gcmd_line.optimize && goptimization_established)){

    /* if hashed rep_node has lower g then return */      
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated_rescue(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}

      /* remove the correspond rescue node from open */
      if (rep_node->rescue && !rep_node->rescue->closed)
	{
	  rep_node->rescue->prev->next = rep_node->rescue->next;
	  if (rep_node->rescue->next)
	    rep_node->rescue->next->prev = rep_node->rescue->prev;
	}

      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
       if (rep_node->rescue && !rep_node->rescue->closed)
	 {
	   free (rep_node->rescue->S.F); 
	   free (rep_node->rescue);
	 }

    }
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated_rescue(rep_node,&new_aux);
      
      /* remove hashed node from open  */
      if (!rep_node->closed){
	rep_node->prev->next = rep_node ->next;
	if (rep_node->next)
	  rep_node->next->prev = rep_node->prev;
      }

      /* remove the correspond rescue node from open */
      if (rep_node->rescue &&!rep_node->rescue->closed)
	{
	  rep_node->rescue->prev->next = rep_node->rescue->next;
	  if (rep_node->rescue->next)
	    rep_node->rescue->next->prev = rep_node->rescue->prev;
	}

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
       if (rep_node->rescue && !rep_node->rescue->closed)
	 {
	   free (rep_node->rescue->S.F); 
	   free (rep_node->rescue);
	 }

    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

       new_aux.h = get_1P_and_H_and_nonH( S );

      if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_hn = gcost;
	/* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	 * total-time part in metric, it is 0
	 */
	new_aux.float_hn += new_aux.h * gtt;

      }

      if ( new_aux.h == INFINITY ) {
	return;
      }
    }

  /* update f */
  if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }


  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  
  if ( gcmd_line.optimize && goptimization_established ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn)
	break;
    }
  }


/* create the new node and the rescue and include them in the found position
   in open. First the new node, then the new rescue node */
  new = new_BfsNode();
  new_rescue = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  copy_source_to_dest( &(new_rescue->S), S );
  new->op = op;
  new_rescue->op = op;
  new->h = new_aux.h;
  new_rescue->h = new_aux.h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = new_aux.float_fn;
    new_rescue->float_fn = new_aux.float_fn;
    new->float_hn = new_aux.float_hn;
    new_rescue->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
    new_rescue->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
    new_rescue->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new_rescue->father = father;
  new->g = new_aux.g;
  new_rescue->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;
      new->num_nonH = 0;

      new_rescue->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new_rescue->nonH[j] = gA[j];
      }
      new_rescue->num_nonH = gnum_A;
      new_rescue->num_H = 0;
    }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;
      new->nonH= 0;

      new_rescue->num_nonH = new_aux.num_nonH;
      new_rescue->nonH= new_aux.nonH;
      new_rescue->num_H = 0;
    }

  /* update open list pointers*/
  new->rescue = new_rescue;

  new->next = new_rescue;
  new_rescue->next=i->next;
  new->prev = i;
  new_rescue->prev=new;
  i->next = new;

  if ( new_rescue->next ) {
    new_rescue->next->prev = new_rescue;
  }

  /* the rescue node is not hashed */
  hash_bfs_node( new );

}


/**********************************************************************************
 * ANYTIME BEST FIRST SEARCH IMPLEMENTATION WITH LOOKAHEAD: each node is included in open
 * twice. Once with helpful actions and once with non helpful (rescue) actions
 * as in Vidal ICAPS04

   - Inconsistent heuristic: node re-expansions without new evaluations. Only
     an inconsistent heuristic is used. g is used to prune search space

   - Rescue nodes are not hashed


************************************************************************************/



Bool do_anytime_bfs_HA_rescue_lookahead( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  BfsNode *lookahead, *added_node;

  deactivate_actions_deleting_top_level_logic_goals();

  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  
  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_rescue_space_lookahead( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {
    

    if ( (first = lbfs_space_head->next) == NULL ) {
      /* include all nodes in the secondary list*/
      include_list_in_open(lbfs_secondary_current_head,lbfs_secondary_current_tail,
		       add_to_anytime_bfs_HA_rescue_space_lookahead );
      lbfs_secondary_current_head = (BfsNode*)0;
      lbfs_secondary_current_tail = (BfsNode*)0;

      if ( (first = lbfs_space_head->next) == NULL )
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      else continue;

    }


    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }


    /* prune first by g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }



    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* generate lookahead states */


     if (!first->is_lookahead )
	lookahead = obtain_lookahead_state_order_rep(first);
     else
      lookahead = NULL;
     
     while (lookahead)
      {
	
	added_node= add_to_anytime_bfs_HA_rescue_space_lookahead( &(lookahead->S), 
						  lookahead->op, lookahead->father );


	lookahead = (BfsNode *) 0;
	if (added_node){
	  added_node->is_lookahead = TRUE;
	  /*
	  printf("\nfather: %f, added:%f,%f", first->float_hn, added_node->float_hn, added_node->float_gn);
	  */
	  /* compute the solution cost: final_total_cost  */
	  if ( added_node->h == 0 ) {
	    if ( gcmd_line.optimize && goptimization_established ){
	      final_total_cost=added_node->float_gn;
	    }
	    else{
	      final_total_cost=added_node->g;
	    }

	  /* extract and print solution */
	    extract_plan( added_node);
      
	    times (&end);
	    TIME (gsearch_time);
	    times (&start);
	    
	    print_plan();  
	    output_planner_info();
	    if (gcmd_line.o_file){
	      output_planner_info_to_file();
	    }

	    sstart = TRUE;
	    min = INFINITY;
	    if (gcmd_line.stop_at_first_sol)
	      return TRUE;

	    break;
	  }

	  lookahead = obtain_lookahead_state_order_rep(added_node);

	}
     }



    /*include in open the sucessors due to helpful actions */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->H[i], first );
      }
    }

    /*include in a sucessors due to non helpful actions */
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->nonH[i], first );
      }
    }

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;
 
  }

  return TRUE;

}



BfsNode *add_to_anytime_bfs_HA_rescue_space_lookahead( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node, *new_rescue;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  

  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( (gcmd_line.optimize && goptimization_established)){

    /* if hashed rep_node has lower g then return */      
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return NULL;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated_rescue(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}

      /* remove the correspond rescue node from open */
      if (rep_node->rescue && !rep_node->rescue->closed)
	{
	  rep_node->rescue->prev->next = rep_node->rescue->next;
	  if (rep_node->rescue->next)
	    rep_node->rescue->next->prev = rep_node->rescue->prev;
	}

      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
       if (rep_node->rescue && !rep_node->rescue->closed)
	 {
	   free (rep_node->rescue->S.F); 
	   free (rep_node->rescue);
	 }

    }
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return NULL;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated_rescue(rep_node,&new_aux);
      
      /* remove hashed node from open  */
      if (!rep_node->closed){
	rep_node->prev->next = rep_node ->next;
	if (rep_node->next)
	  rep_node->next->prev = rep_node->prev;
      }

      /* remove the correspond rescue node from open */
      if (rep_node->rescue &&!rep_node->rescue->closed)
	{
	  rep_node->rescue->prev->next = rep_node->rescue->next;
	  if (rep_node->rescue->next)
	    rep_node->rescue->next->prev = rep_node->rescue->prev;
	}

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
       if (rep_node->rescue && !rep_node->rescue->closed)
	 {
	   free (rep_node->rescue->S.F); 
	   free (rep_node->rescue);
	 }

    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

       new_aux.h = get_1P_and_H_and_nonH( S );
       
       
       if (new_aux.h == INFINITY){
	 activate_actions_deleting_top_level_logic_goals();
	 new_aux.h = get_1P_and_H_and_nonH( S );
	 deactivate_actions_deleting_top_level_logic_goals();
       }
	 

      if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_hn = gcost;
	new_aux.float_hn += new_aux.h * gtt;

      }
       
      if ( new_aux.h == INFINITY ) {
	return NULL;
      }
    }

  /* update f */
  if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }


  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  
  if ( gcmd_line.optimize && goptimization_established ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn)
	break;
    }
  }


/* create the new node and the rescue and include them in the found position
   in open. First the new node, then the new rescue node */
  new = new_BfsNode();
  new_rescue = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  copy_source_to_dest( &(new_rescue->S), S );
  new->op = op;
  new_rescue->op = op;
  new->h = new_aux.h;
  new_rescue->h = new_aux.h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = new_aux.float_fn;
    new_rescue->float_fn = new_aux.float_fn ;
    new->float_hn = new_aux.float_hn;
    new_rescue->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
    new_rescue->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
    new_rescue->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new_rescue->father = father;
  new->g = new_aux.g;
  new_rescue->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      /*sucessors*/
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;
      new->num_nonH = 0;

      new_rescue->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new_rescue->nonH[j] = gA[j];
      }
      new_rescue->num_nonH = gnum_A;
      new_rescue->num_H = 0;

      /* relaxed plan */
      new->g_RP = ( int * ) calloc( gnum_RP, sizeof( int ) );
      new->RPorder = ( int * ) calloc( gnum_RP, sizeof( int ) );
      new->max_RP_order = 0;
      new->min_RP_order = INFINITY;
      for ( j = 0; j < gnum_RP; j++ ) {
	new->g_RP[j] = g_RP[j];
	new->RPorder[j] = gop_conn[g_RP[j]].level_required - gop_conn[g_RP[j]].level_achievable;
	if (new->RPorder[j] > new->max_RP_order)
	  new->max_RP_order = new->RPorder[j];
	if (LESS(new->RPorder[j], new->min_RP_order))
	  new->min_RP_order = new->RPorder[j];
	
      }
      new->gnum_RP = gnum_RP;

    }
  else
    {
      /* sucessors*/
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;
      new->nonH= 0;

      new_rescue->num_nonH = new_aux.num_nonH;
      new_rescue->nonH= new_aux.nonH;
      new_rescue->num_H = 0;
      
      /* relaxed plan */
      new->g_RP = new_aux.g_RP;
      new->gnum_RP = new_aux.gnum_RP;
    }

  /* update open list pointers*/
  
  new->rescue = new_rescue;
  new->next = new_rescue;
  new_rescue->next=i->next;
  new->prev = i;
  new_rescue->prev=new;
  i->next = new;

  if ( new_rescue->next ) {
    new_rescue->next->prev = new_rescue;
  }

  /*
  new->next = i->next;
  new->prev = i;
  i->next = new;

  if (new->next){
    new->next->prev=new;
  }
  */
  /* the rescue node is not hashed */
  hash_bfs_node( new );

  return new;
}



/**********************************************************************************
 * ANYTIME BEST FIRST SEARCH IMPLEMENTATION WITH LOOKAHEAD: each node is included in open
 * twice. Once with helpful actions and once with non helpful (rescue) actions
 * as in Vidal ICAPS04

   - Inconsistent heuristic: node re-expansions without new evaluations. Only
     an inconsistent heuristic is used. g is used to prune search space

   - Rescue nodes are not hashed

   - Heuristic is recomputed


************************************************************************************/


Bool do_anytime_bfs_HA_rescue_lookahead_recomputeh( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  BfsNode *lookahead, *added_node, *next;

  deactivate_actions_deleting_top_level_logic_goals();

  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  
  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_rescue_space_lookahead_recomputeh( &ginitial_state, -1, NULL, FALSE );

  times (&start);

  while ( TRUE ) {
    
    if ( (first = lbfs_space_head->next) == NULL ) {

      if ( gcmd_line.display_info ) {
	printf("\n\nsearch space empty!\n\n");
      }
      return FALSE;
    }

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }


    /* prune first by g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }



    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }


    /*include in open the sucessors due to helpful actions */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->H[i], first );
      }
    }

    /*include in a sucessors due to non helpful actions */
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->nonH[i], first );
      }
    }

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;


    /* When the open list is empty, include all actions in the
       secondary list
     */
    if (/*!lbfs_space_head->next || !lbfs_space_head->next->is_lookahead*/ TRUE){

      first = lbfs_secondary_current_head;
      while ( first ) {

	added_node =add_to_anytime_bfs_HA_rescue_space_lookahead_recomputeh(&(first->S), 
								 first->op, first->father, TRUE);


	/* compute the solution cost: final_total_cost  */
	if ( added_node && added_node->h == 0 ) {
	  if ( gcmd_line.optimize && goptimization_established ){
	    final_total_cost=added_node->float_gn;
	  }
	  else{
	    final_total_cost=added_node->g;
	  }

	  /* extract and print solution */
	  extract_plan( added_node);
      
	  times (&end);
	  TIME (gsearch_time);
	  times (&start);

	  print_plan();  
	  output_planner_info();
	  if (gcmd_line.o_file){
	    output_planner_info_to_file();
	  }
	  sstart = TRUE;
	  min = INFINITY;

	  if (gcmd_line.stop_at_first_sol)
	    return TRUE;



	}

	next = first->next;

	/* clean up */
	free (first->S.F); /* delete this list of facts */
	free (first); 

	first = next;
      }

   
      /* Getting ready for the next iteration ... */
      lbfs_secondary_current_head = (BfsNode*)0;
      lbfs_secondary_current_tail = (BfsNode*)0;
    }

 
  }

  return TRUE;

}



BfsNode *add_to_anytime_bfs_HA_rescue_space_lookahead_recomputeh( State *S, int op, BfsNode *father, Bool is_lookahead )

{

  BfsNode *new, *i, *rep_node, *new_rescue, *added_node, *lookahead;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  

  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( (gcmd_line.optimize && goptimization_established)){

    /* if hashed rep_node has lower g then return */      
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return NULL;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated_rescue(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}

      /* remove the correspond rescue node from open */
      if (rep_node->rescue && !rep_node->rescue->closed)
	{
	  rep_node->rescue->prev->next = rep_node->rescue->next;
	  if (rep_node->rescue->next)
	    rep_node->rescue->next->prev = rep_node->rescue->prev;
	}

      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
       if (rep_node->rescue && !rep_node->rescue->closed)
	 {
	   free (rep_node->rescue->S.F); 
	   free (rep_node->rescue);
	 }

    }
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return NULL;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated_rescue(rep_node,&new_aux);
      
      /* remove hashed node from open  */
      if (!rep_node->closed){
	rep_node->prev->next = rep_node ->next;
	if (rep_node->next)
	  rep_node->next->prev = rep_node->prev;
      }

      /* remove the correspond rescue node from open */
      if (rep_node->rescue &&!rep_node->rescue->closed)
	{
	  rep_node->rescue->prev->next = rep_node->rescue->next;
	  if (rep_node->rescue->next)
	    rep_node->rescue->next->prev = rep_node->rescue->prev;
	}

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
       if (rep_node->rescue && !rep_node->rescue->closed)
	 {
	   free (rep_node->rescue->S.F); 
	   free (rep_node->rescue);
	 }

    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

       new_aux.h = get_1P_and_H_and_nonH( S );
       
       
       if (new_aux.h == INFINITY){
	 activate_actions_deleting_top_level_logic_goals();
	 new_aux.h = get_1P_and_H_and_nonH( S );
	 deactivate_actions_deleting_top_level_logic_goals();
       }
	 

      if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_hn = gcost;
	new_aux.float_hn += new_aux.h * gtt;

      }
       
      if ( new_aux.h == INFINITY ) {
	return NULL;
      }
    }


  /* update f */
  if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }





  /* create the new node and the rescue  */

  new = new_BfsNode();
  new_rescue = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  copy_source_to_dest( &(new_rescue->S), S );
  new->op = op;
  new_rescue->op = op;
  new->h = new_aux.h;
  new_rescue->h = new_aux.h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = new_aux.float_fn;
    new_rescue->float_fn = new_aux.float_fn ;
    new->float_hn = new_aux.float_hn;
    new_rescue->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
    new_rescue->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
    new_rescue->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new_rescue->father = father;
  new->g = new_aux.g;
  new_rescue->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      /*sucessors*/
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;
      new->num_nonH = 0;

      new_rescue->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new_rescue->nonH[j] = gA[j];
      }
      new_rescue->num_nonH = gnum_A;
      new_rescue->num_H = 0;

      /* relaxed plan */
      new->g_RP = ( int * ) calloc( gnum_RP, sizeof( int ) );
      new->RPorder = ( int * ) calloc( gnum_RP, sizeof( int ) );
      new->max_RP_order = 0;
      new->min_RP_order = INFINITY;
      for ( j = 0; j < gnum_RP; j++ ) {
	new->g_RP[j] = g_RP[j];
	new->RPorder[j] = gop_conn[g_RP[j]].level_required - gop_conn[g_RP[j]].level_achievable;
	if (new->RPorder[j] > new->max_RP_order)
	  new->max_RP_order = new->RPorder[j];
	if (LESS(new->RPorder[j], new->min_RP_order))
	  new->min_RP_order = new->RPorder[j];
	
      }
      new->gnum_RP = gnum_RP;

    }
  else
    {
      /* sucessors*/
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;
      new->nonH= 0;

      new_rescue->num_nonH = new_aux.num_nonH;
      new_rescue->nonH= new_aux.nonH;
      new_rescue->num_H = 0;
      
      /* relaxed plan */
      new->g_RP = new_aux.g_RP;
      new->gnum_RP = new_aux.gnum_RP;
    }

  /* the rescue node is not hashed */
    hash_bfs_node( new );

   /* generate lookahead states */
   if (!is_lookahead )
     lookahead = obtain_lookahead_state_order_rep(new);
     else
     lookahead = NULL;
   
    if (lookahead)
      {

	added_node= add_to_anytime_bfs_HA_rescue_space_lookahead_recomputeh( &(lookahead->S), 
								  lookahead->op, lookahead->father, TRUE );

	if (added_node){
	  added_node->is_lookahead = TRUE;
	  /*
	  printf("\nfather: %f, added:%f,%f", first->float_hn, added_node->float_hn, added_node->float_gn);
	  */
	  /* compute the solution cost: final_total_cost  */
	  
	  if ( added_node->h == 0 ) {
	    if ( gcmd_line.optimize && goptimization_established ){
	      final_total_cost=added_node->float_gn;
	    }
	    else{
	      final_total_cost=added_node->g;
	    }

	  
	    extract_plan( added_node);
	    /*
	    times (&end);
	    TIME (gsearch_time);
	    */
	    print_plan();  
	    output_planner_info();
	    if (gcmd_line.o_file){
	      output_planner_info_to_file();
	    }
	    /*
	    sstart = TRUE;
	    min = INFINITY;
	    */
	    /*  break;*/
	  }
	  
	  
	  if ( gcmd_line.optimize && goptimization_established ) {
	    new->float_fn = added_node->float_fn;
	    new_rescue->float_fn = added_node->float_fn;

	  }
	  else {
	    new->int_fn = added_node->int_fn;
	    new_rescue->int_fn = added_node->int_fn;
	  }
	  

	}
      }


  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  
  if ( gcmd_line.optimize && goptimization_established ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS(new->float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn > new->int_fn)
	break;
    }
  }




  /* and include them in the found position
   in open. First the new node, then the new rescue node */



  /* update open list pointers*/
  
  new->rescue = new_rescue;
  new->next = new_rescue;
  new_rescue->next=i->next;
  new->prev = i;
  new_rescue->prev=new;
  i->next = new;

  if ( new_rescue->next ) {
    new_rescue->next->prev = new_rescue;
  }

  /*
  new->next = i->next;
  new->prev = i;
  i->next = new;

  if (new->next){
    new->next->prev=new;
  }
  */


  return new;
}




/**********************************************************************************
 * LOOKAHEAD STATE: obtain a lookahead state from a relaxed plan
***********************************************************************************/



/* father_node has a relaxed plan, which is applied to obtain a lookahead
   state. The returned bfsnode is the lookahead node
*/

BfsNode *obtain_lookahead_state_basic (BfsNode *father_node) 
{
 static Bool first_call = TRUE; 
 static State S_prev;
 static State S_new;
 BfsNode *new_father = father_node, *new_node ;
 Bool *executed, new_action_applied;
 int lookahead_length;

 int i;

  if ( first_call ) {
   make_state( &S_new, gnum_ft_conn, gnum_fl_conn );
   make_state( &S_prev, gnum_ft_conn, gnum_fl_conn );
   first_call = FALSE;
  }

  if (!father_node)
    return NULL;

  if (father_node->gnum_RP < 1)
    return NULL;

  /* to mark executed actions */
  executed = ( Bool * ) calloc( father_node->gnum_RP, sizeof( Bool ) );

  /* initialize executed actions */
  for ( i = father_node->gnum_RP-1; i >= 0 ; i-- ) {
    executed[i] = FALSE;
  }

  /* initialize new_action_applied */
  new_action_applied = TRUE;

  lookahead_length = 0;
  
  source_to_dest(&S_prev, &(father_node->S));
  
  
  while (new_action_applied)
    {
      new_action_applied = FALSE;

      for ( i = father_node->gnum_RP-1; i >= 0 ; i-- ) { 

	/* if not executed and applicable */
	if ( !executed[i] && result_to_dest( &S_new, &S_prev, father_node->g_RP[i]))
	  {
	    new_node = add_to_lookahead_list( &S_new, father_node->g_RP[i], new_father );

	    /* mark the action as executed */
	    executed[i] = TRUE;

	    new_action_applied = TRUE;

	    lookahead_length++;

	    new_father = new_node;
	    
            source_to_dest(&S_prev, &S_new);      	      

	  }
      }
    }

  free(executed);
  if (new_father != father_node)
    return new_father;
  else return NULL;
 
}
  

	  
	  


BfsNode *obtain_lookahead_state_order (BfsNode *father_node) 
{
 static Bool first_call = TRUE; 
 static State S_prev;
 static State S_new;
 BfsNode *new_father = father_node, *new_node ;
 Bool *executed, new_action_applied;
 int lookahead_length;

 int max_order;
 int min_order;
 int actual_order;

 int i;

  if ( first_call ) {
   make_state( &S_new, gnum_ft_conn, gnum_fl_conn );
   make_state( &S_prev, gnum_ft_conn, gnum_fl_conn );
   first_call = FALSE;
  }

  if (!father_node)
    return NULL;

  if (father_node->gnum_RP < 1)
    return NULL;

  /* to mark executed actions */
  executed = ( Bool * ) calloc( father_node->gnum_RP, sizeof( Bool ) );

  /* initialize executed actions */
  for ( i = father_node->gnum_RP-1; i >= 0 ; i-- ) {
    executed[i] = FALSE;
  }

  /* initialize new_action_applied */
  new_action_applied = TRUE;

  lookahead_length = 0;
  
  max_order = father_node->max_RP_order;
  min_order = father_node->min_RP_order;
  actual_order = min_order;
  
  source_to_dest(&S_prev, &(father_node->S));
  

  
  while (actual_order <= max_order)
    {
      new_action_applied = FALSE;

      for ( i = father_node->gnum_RP-1; i >= 0 ; i-- ) { 

	/* if not executed and applicable */
	if ( !executed[i] && father_node->RPorder[i] == actual_order ){
	  if ( result_to_dest( &S_new, &S_prev, father_node->g_RP[i]))
	  {

	    new_node = add_to_lookahead_list( &S_new, father_node->g_RP[i], new_father );

	    if (LESS(final_total_cost, new_node->float_gn))
	      return NULL;

	    /* mark the action as executed */
	    executed[i] = TRUE;

	    new_action_applied = TRUE;

	    lookahead_length++;

	    new_father = new_node;
	    
            source_to_dest(&S_prev, &S_new);      	      

	    actual_order = min_order;

	  }
	}
      }
      
      if (!new_action_applied) 
	{
	actual_order++;
	new_action_applied = TRUE;
	}
    }

  free(executed);
  if (new_father != father_node)
    return new_father;


  else 
    {
      printf("\n FALLO LOOKAHEAD\n");
      return NULL;
    }
 
}
  



BfsNode *obtain_lookahead_state_order_rep (BfsNode *father_node) 
{
 static Bool first_call = TRUE; 
 static State S_prev;
 static State S_new;
 BfsNode *new_father = father_node, *new_node, *hashed ;
 Bool *executed, new_action_applied;
 int lookahead_length;

 int max_order;
 int min_order;
 int actual_order;

 int i,j,h;
 int *old_g_RP;
 int old_gnum_RP;
 int *old_RPorder;
 int old_max_RP_order;
 int old_min_RP_order;

 // printf("\nobtaining lookahead!!!");

  if ( first_call ) {
   make_state( &S_new, gnum_ft_conn, gnum_fl_conn );
   make_state( &S_prev, gnum_ft_conn, gnum_fl_conn );
   first_call = FALSE;
  }

  if (!father_node)
    return NULL;

  if (father_node->gnum_RP < 1)
    return NULL;


  if (father_node->lookahead_generated)
    return NULL;

  father_node->lookahead_generated = TRUE;
  /* to mark executed actions */
  executed = ( Bool * ) calloc( father_node->gnum_RP, sizeof( Bool ) );

  /* initialize executed actions */
  for ( i = father_node->gnum_RP-1; i >= 0 ; i-- ) {
    executed[i] = FALSE;
  }

  /* initialize new_action_applied */
  new_action_applied = TRUE;

  lookahead_length = 0;
  
  max_order = father_node->max_RP_order;
  min_order = father_node->min_RP_order;
  actual_order = min_order;
  
  source_to_dest(&S_prev, &(father_node->S));
  
  /*
  printf("\n\n----********************+");
  */
  
  while (actual_order <= max_order)
    {
      new_action_applied = FALSE;

      for ( i = father_node->gnum_RP-1; i >= 0 ; i-- ) { 

	/* if not executed and applicable */
	if ( !executed[i] && father_node->RPorder[i] == actual_order &&
	     result_to_dest( &S_new, &S_prev, father_node->g_RP[i]))
	  {

	    hashed = same_no_metric_bfs_state_hashed(&S_new, new_father);

	    new_node = add_to_lookahead_list( &S_new, father_node->g_RP[i], new_father );

	    if (LESS(final_total_cost, new_node->float_gn))
	      return NULL;

	    if (hashed){
	      //return NULL;
	      //continue;
	      
	      if (LESS(hashed->float_gn,new_node->float_gn))
		{
		  return NULL;
		  new_node = hashed;
		  //		  return hashed;
		}
	      }
	      
	      /*
	      if (LESS(hashed->float_gn,new_node->float_gn)){
		if (!gop_conn[father_node->g_RP[i]].RP_deactivated)
		  gop_conn[father_node->g_RP[i]].RP_deactivated = TRUE;
		else continue;


		h=get_1P(&S_prev);

		if (h == INFINITY){
		  activate_all();
		  gop_conn[father_node->g_RP[i]].RP_deactivated = TRUE;
		  h=get_1P(&S_prev);
		}
		  

		if (h!=INFINITY){
		  if (new_father == father_node)
		    {
		      old_g_RP= father_node->g_RP;
		      old_gnum_RP = father_node->gnum_RP;
		      old_RPorder = father_node->RPorder;
		      old_max_RP_order = father_node->max_RP_order;
		      old_min_RP_order = father_node->min_RP_order;
		    }

		  new_father->g_RP = ( int * ) calloc( gnum_RP, sizeof( int ) );
		  new_father->RPorder = ( int * ) calloc( gnum_RP, sizeof( int ) );

		  new_father->max_RP_order = 0;
		  new_father->min_RP_order = INFINITY;
		  for ( j = 0; j < gnum_RP; j++ ) {
		    new_father->g_RP[j] = g_RP[j];
		    // new_father->RPorder[j] = gop_conn[g_RP[j]].level_required -
		    //  gop_conn[g_RP[j]].level_achievable;
		    new_father->RPorder[j] = gop_conn[g_RP[j]].level_required_prop;
		    

		    if (new_father->RPorder[j] >new_father->max_RP_order)
		      new_father->max_RP_order = new_father->RPorder[j];
		    if (LESS(new_father->RPorder[j], new_father->min_RP_order))
		      new_father->min_RP_order = new_father->RPorder[j];
	
		  }
		  new_father->gnum_RP = gnum_RP;
		  

		  new_node= obtain_lookahead_state_order_rep(new_father);
		  
		  activate_all();
		  deactivate_actions_deleting_top_level_logic_goals();
		  
		  free(new_father->g_RP);
                  free(new_father->RPorder);
		  
		  if (new_father==father_node)
		    {
		      father_node->g_RP= old_g_RP;
		      father_node->gnum_RP = old_gnum_RP;
		      father_node->RPorder = old_RPorder;
		      father_node->max_RP_order = old_max_RP_order;
		      father_node->min_RP_order =  old_min_RP_order;
		    }
		  
		  if (new_node != father_node)
		    return new_node;
		  else return NULL;
		    
		  

		}
		else
		  {
		  activate_all();
		  deactivate_actions_deleting_top_level_logic_goals();

		  continue;
		  }
	      }
	    }
	      */
	    /* mark the action as executed */
	    executed[i] = TRUE;

	    new_action_applied = TRUE;

	    lookahead_length++;

	    new_father = new_node;
	    
            source_to_dest(&S_prev, &S_new);      	      

	    actual_order = min_order;

	    /*
	    printf("\n"); 
            print_op_name(father_node->g_RP[i]);
	    printf("\n order: %d",father_node->RPorder[i]);
	    */

	  }
      }
      
      if (!new_action_applied) 
	{
	actual_order++;
	new_action_applied = TRUE;
	}
    }

  free(executed);
  if (new_father != father_node)
    {
      //printf("\n\nlookahead length %d",  lookahead_length);
    return new_father;
    }


  else 
    {
      /*printf("\n FALLO LOOKAHEAD\n");*/
      return NULL;
    }
 
}

/* RANDOM search*/


int compare_actions_random( const void* a, const void* b ) {
   int* arg1 = (int*) a;
   int* arg2 = (int*) b;

   srand(gcmd_line.seed++);
   int r1 = rand();
   srand(gcmd_line.seed++);
   int r2 = rand();

   if (r1 < r2)
     return -1;
   else
     return 1;
}
  

 


Bool do_depth_first_chronological_backtraking_random( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;



  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;


  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_depth_first_space_random( &ginitial_state, -1, NULL );


  times (&start);


  printf("\n\nsearching...\n\n");
  while ( TRUE ) {
    
    if ( (first = lbfs_space_head->next) == NULL ) 
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
    
  /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    gexpanded_states ++ ;
    //    printf(".");

    /* prune first by  g values using best solution found cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }


    /* to generate all successors comment previous lines and remove next
       comment. Also replace first->h == 0 below */
 
      get_A( &(first->S));
      for ( i = 0; i < gnum_A; i++ ) {
	gop_conn[gA[i]].is_in_A = FALSE;
	gop_conn[gA[i]].is_in_H = TRUE;
      }
      gH = gA;
      gnum_H = gnum_A;
      gnum_A = 0;
      
      /*

    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

      */

    /* compute the solution cost: final_total_cost  */
      /*    if ( first->h == 0 ) {*/
    if (!are_there_pending_logic_goals( &(first->S))){
      
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }
      
      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);

      print_plan();  
      output_planner_info();
      printf("\n\nINITIAL STATE:");
      print_static_facts();
      print_State( (first->S));
      
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      if (gcmd_line.stop_at_first_sol)
	break;


    }

    /* sort actions randomly */
    qsort(gH, gnum_H, sizeof(int), compare_actions_random);

    /*include in open the sucessors*/
    for ( i = gnum_H - 1; i >= 0; i-- ) {
      if ( result_to_dest( &S, &(first->S), gH[i] ) ) {
	  add_to_depth_first_space_roller( &S, gH[i], first );
      }
    }
    
    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;
    
  }

  return TRUE;

}




BfsNode *add_to_depth_first_space_random( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int g;
  float float_gn;
  


  /* compute g 
   */
  if ( father ) {
    g = father->g + 1;
  } else {
    g = 0;
  }

  
  if ( gcmd_line.optimize && goptimization_established ) {
    float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost,float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,g))
	return NULL;
    }


  // test duplicates 
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      // if hashed rep_node has lower g 
      if (LESS_EQ(rep_node->float_gn, float_gn))
	return NULL;
  
    }	   
    else {
      // if hashed node has lower g 
      if (rep_node->g <= g)
	return NULL;

    }
  }

  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  i =  lbfs_space_head;

  /*
  if ( gcmd_line.optimize && goptimization_established ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn)
	break;
    }
  }
  */

/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_gn = float_gn;
  } 
  new->father = father;
  new->g = g;

  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );
  return new;
}



  


/* ROLLER search*/
Bool do_depth_first_chronological_backtraking_roller( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;



  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;


  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_depth_first_space_roller( &ginitial_state, -1, NULL );

  /* load roller trees */
  load_all_tilde_trees();

  times (&start);

  while ( TRUE ) {


    if (!memory_check())
      return FALSE;
    
    if ( (first = lbfs_space_head->next) == NULL ) {

      /*  for eliminating the  secondary list
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space of HA successors empty!\n\n");
	  }
	  return FALSE;
      */
  

      /* include all nodes in the secondary list*/                                                                                    
      include_list_in_open_elements(lbfs_secondary_current_head,lbfs_secondary_current_tail, add_to_depth_first_space_roller,1);         
      /*
      lbfs_secondary_current_head = (BfsNode*)0;                                                                                      
      lbfs_secondary_current_tail = (BfsNode*)0;                                                                                      
      */                                                                                                                              
      if ( lbfs_space_head->next == NULL && lbfs_secondary_current_head==NULL )
	{                                                       
          if ( gcmd_line.display_info ) {                                                                                             
            printf("\n\nsearch space empty!\n\n");                                                                                    
          }                                                                                                                           
          return FALSE;                                                                                                               
        }                                                                                                                             
      else continue;      
    }

  

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    gexpanded_states ++ ;
    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }


    
    //    printf("\n%d", gexpanded_states);
    /* raquel sucessors generation */
    /* evaluate the node to generate helpful actions */


     first->h= get_1P_and_H_and_nonH( &(first->S));
     
     if ( first->h == INFINITY ) {
       /*trace*/
       if ( gcmd_line.display_info == 130 ) {
	 printf("\n\nROLLER backtracking");
       }
	continue;
      }

    /* to generate all successors comment previous lines and remove next
       comment. Also replace first->h == 0 below */
     /*
      get_A( &(first->S));
      for ( i = 0; i < gnum_A; i++ ) {
	gop_conn[gA[i]].is_in_A = FALSE;
	gop_conn[gA[i]].is_in_H = TRUE;
      }
      gH = gA;
      gnum_H = gnum_A;
      gnum_A = 0;
     */



    /* stdout*/
     /*
    if ( gcmd_line.display_info ) {
       printf("\n%d", gexpanded_states);
     }
     */
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
    /*if (!are_there_pending_logic_goals( &(first->S))){*/
      
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }
      
      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);


      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      if (gcmd_line.stop_at_first_sol)
	break;


    }

  
    /* for roller matching*/
    assign_LT_last_executed_action(first->op);
    assign_LT_current_state(&(first->S));
    match_all();


    /*include in open the sucessors due to helpful actions */
    /* last added node is the first in open */
    /* for eliminating secondary */
    /*
    for ( i = gnum_H - 1; i >= 0; i-- ) {
      if ( result_to_dest( &S, &(first->S), gH[i] ) ) {
	if (gop_conn[gH[i]].roller_value > 0)
	  add_to_depth_first_space_roller( &S, gH[i], first );
      }
    }
    */

    /*include in open the sucessors due to helpful actions */                                                                         
    /* last added node is the first in open */                                                                                        
    for ( i = gnum_H - 1; i >= 0; i-- ) {                                                                                             
      if ( result_to_dest( &S, &(first->S), gH[i] ) ) {                                                                               
        if (gop_conn[gH[i]].roller_value > 0)                                                                                         
          add_to_depth_first_space_roller( &S, gH[i], first );                                                                        
        else                                                                                                                          
          add_to_secondary_list_reverse( &S, gH[i], first );                                                                                  
                                                                                                                                      
      }                                                                                                                               
    }                                                                                                                               
                                                                                                                                      
    /* include non helpful actions in the secondary list*/                                                                            
    for ( i = 0; i < gnum_A; i++ ) {              
      if ( result_to_dest( &S, &(first->S), gA[i] ) ) {                                                                               
        if (gop_conn[gA[i]].is_in_A)
	  // otherwise the action is in gH
          add_to_secondary_list_reverse( &S, gA[i], first );                                                                                  
        }                                                                                                                         
    }                                                                                                                             



    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;
    
  }

  return TRUE;

}



/* As ROLLER search, but using sorting applicable actions using ff heuristic. Baseline*/
Bool do_depth_first_chronological_backtraking_ff( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;



  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;


  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_depth_first_space_ff( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {


    if (!memory_check())
      return FALSE;
    
    if ( (first = lbfs_space_head->next) == NULL ) {

      /*  for eliminating the  secondary list
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space of HA successors empty!\n\n");
	  }
	  return FALSE;
      */
  

      /* include all nodes in the secondary list*/                                                                                    
      include_list_in_open_elements(lbfs_secondary_current_head,lbfs_secondary_current_tail, add_to_depth_first_space_ff,1);           /*
      lbfs_secondary_current_head = (BfsNode*)0;                                                                                     
      lbfs_secondary_current_tail = (BfsNode*)0;                                                                                      
      */                                                                                                                              
      if ( lbfs_space_head->next == NULL && lbfs_secondary_current_head==NULL )
	{                                                       
          if ( gcmd_line.display_info ) {                                                                                             
            printf("\n\nsearch space empty!\n\n");                                                                                    
          }                                                                                                                           
          return FALSE;                                                                                                               
        }                                                                                                                             
      else continue;      
    }

  

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    gexpanded_states ++ ;
    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }


    /* stdout*/
     /*
    if ( gcmd_line.display_info ) {
       printf("\n%d", gexpanded_states);
     }
     */
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
    /*if (!are_there_pending_logic_goals( &(first->S))){*/
      
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }
      
      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);


      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      if (gcmd_line.stop_at_first_sol)
	break;


    }

  
 
    /*include in open the sucessors due to helpful actions */
    /* all sucessors are introduced before the current first, so remember the
       current first */


     lbfs_space_current_first = lbfs_space_head->next;
     if ( gcmd_line.display_info == 131) {
       printf("\n\n->EXTRACTED NODE:\n");
       if (first->op != -1) {
	 print_op_name(first->op);       
       }
       printf("\n**********************helpful sucessors: %d", first->num_H);
     }
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_depth_first_space_ff( &S, first->H[i], first );

      }
    }
    if ( gcmd_line.display_info == 131) {	      
      print_list(lbfs_space_head);                                                                                              }
    /* include non helpful actions in the secondary list*/                                                                            
    for ( i = 0; i < first->num_nonH; i++ ) {              
          add_to_secondary_list_reverse( &S, gA[i], first );                                                                                  
    }
            



    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;
    
  }

  return TRUE;

  }




/* ROLLER search*/
/* with secondary list */
Bool do_depth_first_chronological_backtraking_roller_sec( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;



  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;


  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_depth_first_space_roller( &ginitial_state, -1, NULL );

  /* load roller trees */
  load_all_tilde_trees();

  times (&start);

  while ( TRUE ) {


    if (!memory_check())
      return FALSE;
    
    if ( (first = lbfs_space_head->next) == NULL ) {

      /* include all nodes in the secondary list*/
      include_list_in_open(lbfs_secondary_current_head,lbfs_secondary_current_tail, add_to_depth_first_space_roller);
      lbfs_secondary_current_head = (BfsNode*)0;
      lbfs_secondary_current_tail = (BfsNode*)0;

      if ( (first = lbfs_space_head->next) == NULL )
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      else continue;

    }
    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    gexpanded_states ++ ;
    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }


    
    //    printf("\n%d", gexpanded_states);
    /* raquel sucessors generation */
    /* evaluate the node to generate helpful actions */


     first->h= get_1P_and_H_and_nonH( &(first->S));
     
     if ( first->h == INFINITY ) {
       /*trace*/
       if ( gcmd_line.display_info == 130 ) {
	 printf("\n\nROLLER backtracking");
       }
	continue;
      }

    /* to generate all successors comment previous lines and remove next
       comment. Also replace first->h == 0 below */
     /*
      get_A( &(first->S));
      for ( i = 0; i < gnum_A; i++ ) {
	gop_conn[gA[i]].is_in_A = FALSE;
	gop_conn[gA[i]].is_in_H = TRUE;
      }
      gH = gA;
      gnum_H = gnum_A;
      gnum_A = 0;
     */



    /* stdout*/
     /*
    if ( gcmd_line.display_info ) {
       printf("\n%d", gexpanded_states);
     }
     */
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
    /*if (!are_there_pending_logic_goals( &(first->S))){*/
      
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }
      
      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);


      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      if (gcmd_line.stop_at_first_sol)
	break;


    }

  
    /* for roller matching*/
    assign_LT_last_executed_action(first->op);
    assign_LT_current_state(&(first->S));
    match_all();


    /*include in open the sucessors due to helpful actions */
    /* last added node is the first in open */
    for ( i = gnum_H - 1; i >= 0; i-- ) {
      if ( result_to_dest( &S, &(first->S), gH[i] ) ) {
	if (gop_conn[gH[i]].roller_value > 0)
	  add_to_depth_first_space_roller( &S, gH[i], first );
	else
	  add_to_secondary_list( &S, gH[i], first );
	
      }
    }
    
    /* include non helpful actions in the secondary list*/
    for ( i = 0; i < gnum_A; i++ ) {
      if ( result_to_dest( &S, &(first->S), gA[i] ) ) {
	if ( FLOAT_EQUAL(gop_conn[gH[i]].roller_value,0.0)){
	  add_to_secondary_list( &S, gA[i], first );
	    }
      }
    }  

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;
    
  }

  return TRUE;

}



BfsNode * add_to_depth_first_space_roller( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int g;
  float float_gn;
  


  /* compute g 
   */
  if ( father ) {
    g = father->g + 1;
  } else {
    g = 0;
  }

  
  if ( gcmd_line.optimize && goptimization_established ) {
    float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost,float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,g))
	return NULL;
    }


  // test duplicates 
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      // if hashed rep_node has lower g 
      if (LESS_EQ(rep_node->float_gn, float_gn))
	return NULL;
  
    }	   
    else {
      // if hashed node has lower g 
      if (rep_node->g <= g)
	return NULL;

    }
  }

  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  i =  lbfs_space_head;

  /*
  if ( gcmd_line.optimize && goptimization_established ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn)
	break;
    }
  }
  */

/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_gn = float_gn;
  } 
  new->father = father;
  new->g = g;

  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );
  return new;
}




/* next function is a copy of  add_to_modified_hill_climbing_space, with the only difference the evaluation computes 
also non helpful actions */
BfsNode * add_to_depth_first_space_ff( State *S, int op, BfsNode *father )

{
  BfsNode *new, *i, *rep_node;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  


  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      /* if hashed rep_node has lower g */
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return NULL;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}
      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
      
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }	   
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return NULL;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);
      
      /* remove hashed node from open  */
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node ->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

      new_aux.h = get_1P_and_H_and_nonH( S );

      if (( gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {
	new_aux.float_hn = gcost;
	new_aux.float_hn += new_aux.h * gtt;

      }

      if ( new_aux.h == INFINITY ) {
	return NULL;
      }
    }

   /* introduce the current node before the current first and sort by h
     value */

  if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {

    for ( i = lbfs_space_head; ( i->next && i->next != lbfs_space_current_first); i = i->next ) {
      if (LESS_EQ(new_aux.float_hn, i->next->float_hn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; ( i->next && i->next != lbfs_space_current_first); i = i->next ) {
      if ( (int) (gcmd_line.h_weight) * i->next->h >= (int) (gcmd_line.h_weight) * new_aux.h)
	break;
    }
  }


  /* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = new_aux.h;
  if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {
    new->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
  } 
  new->father = father;
  new->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/

  if ( gcmd_line.display_info == 131) {
    if (new){
      if (new->op != -1) {
	printf("\n**");
	print_op_name(new->op);
	printf("h: %d", new->h);
      }
    }
  }


  if (!already_evaluated)
    {
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;

      show_collected_actions(gH,gnum_H);


      new->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new->nonH[j] = gA[j];
      }
      new->num_nonH = gnum_A;


 }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;

      show_collected_actions(new_aux.H,new_aux.num_H);



      new->num_nonH = new_aux.num_nonH;
      new->nonH= new_aux.nonH;     


    }


  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );

  return new;

}



/* MODIFIED HILL CLIMBING: This algorithm is not a pure Hill-Climbing, but a
   depth first search where sucessors are sorted in an open list in increasing
   h values. Only helpful sucessors are generated */

Bool do_modified_hill_climbing( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;



  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_modified_hill_climbing_space( &ginitial_state, -1, NULL );

  times (&start);

  while ( TRUE ) {

    if (!memory_check())
      return FALSE;

    if ( (first = lbfs_space_head->next) == NULL ) {

      if ( gcmd_line.display_info ) {
	printf("\n\nsearch space empty!\n\n");
      }
      return FALSE;
    }

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    gexpanded_states ++ ;
    
    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }
   

    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }
   
    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }
      
      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);


      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      if (gcmd_line.stop_at_first_sol)
	break;


    }

    /*include in open the sucessors due to helpful actions */
    /* all sucessors are introduced before the current first, so remember the
       current first */
    
    lbfs_space_current_first = lbfs_space_head->next;

    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_modified_hill_climbing_space( &S, first->H[i], first );
      }
    }

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;

  }

  return TRUE;

}



void add_to_modified_hill_climbing_space( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  


  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      /* if hashed rep_node has lower g */
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}
      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
      
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }	   
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);
      
      /* remove hashed node from open  */
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node ->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

      new_aux.h = get_1P_and_H( S );

      if (( gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {
	new_aux.float_hn = gcost;
	new_aux.float_hn += new_aux.h * gtt;

      }

      if ( new_aux.h == INFINITY ) {
	return;
      }
    }

   /* introduce the current node before the current first and sort by h
     value */

  if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {

    for ( i = lbfs_space_head; ( i->next && i->next != lbfs_space_current_first); i = i->next ) {
      if (LESS_EQ(new_aux.float_hn, i->next->float_hn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; ( i->next && i->next != lbfs_space_current_first); i = i->next ) {
      if ( i->next->h >= new_aux.h)
	break;
    }
  }


  /* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = new_aux.h;
  if ( (gcmd_line.optimize && goptimization_established) || gcmd_line.hdiff ) {
    new->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
  } 
  new->father = father;
  new->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;
 }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;
    }


  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );


}

Bool are_there_pending_logic_goals(State *S)
{
  int i;
  int j;

  for(i = 0; i < gnum_flogic_goal; i++)
    {
      for ( j = 0; j < S->num_F; j++ ) {
	if (gflogic_goal[i] == S->F[j])
	  {
	    break;  
	  }
      }
      if (j == S->num_F)
	return TRUE;
    }

  return FALSE;

}

void compute_total_cost_from_metric(State *S)
{
  int i;
   final_total_cost = 0;

   for ( i = 0; i < glnf_metric.num_pF; i++ ) {

    if ( glnf_metric.pF[i] == -2 ) {

      final_total_cost += gtt * gnum_plan_ops;

    }  
    else{
      final_total_cost += (glnf_metric.pC[i] * 
			   (S->f_V[glnf_metric.pF[i]] - ginitial_state.f_V[glnf_metric.pF[i]]));
    }
   }
}
  


/**********************************************************************************
 * ANYTIME BEST FIRST SEARCH IMPLEMENTATION: helpful actions high priority, g prune

   - Inconsistent heuristic: node re-expansions without new evaluations. Only
     an inconsistent heuristic is used. g is used to prune search space

   - Helpful actions are included first in open. Non-helpful actions are
     included only when the open list is empty

   - Roller policy

************************************************************************************/


Bool do_anytime_bfs_roller_policy( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  BfsNode *last_policy_node;


  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_roller_policy( &ginitial_state, -1, NULL );

  /* load roller trees */
  load_all_tilde_trees();

  times (&start);

  while ( TRUE ) {
    
    if (!memory_check())
      return FALSE;

    if ( (first = lbfs_space_head->next) == NULL ) {
      /* include all nodes in the secondary list*/
      include_list_in_open(lbfs_secondary_current_head,lbfs_secondary_current_tail,
		      add_to_anytime_bfs_roller_policy);
      lbfs_secondary_current_head = (BfsNode*)0;
      lbfs_secondary_current_tail = (BfsNode*)0;

      if ( (first = lbfs_space_head->next) == NULL )
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      else continue;
 

    }


    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    /*    printf(".");*/
    gexpanded_states ++ ;

    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }



    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);


      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;

    }

    last_policy_node =apply_reactive_policy(first,0,add_to_anytime_bfs_roller_policy);

    if (last_policy_node && last_policy_node != first)
      {	    
	    if ( last_policy_node->h == 0 ) {
	      if ( gcmd_line.optimize && goptimization_established ){
		final_total_cost = last_policy_node->float_gn;
	      }
	      else{
		final_total_cost=last_policy_node->g;
	      }

	      /* extract and print solution */
	      extract_plan( last_policy_node );
      
	      times (&end);
	      TIME (gsearch_time);
	      times (&start);


	      print_plan();  
	      output_planner_info();
	      if (gcmd_line.o_file){
		output_planner_info_to_file();
	      }
	      
	      sstart = TRUE;
	      min = INFINITY;
	    
	      /* next break is to stop at first solution: commented */
	      /*break;*/
	      if (gcmd_line.stop_at_first_sol)
		break;
	    }
      }
   
    //include in open the sucessors due to helpful actions
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	add_to_anytime_bfs_roller_policy( &S, first->H[i], first );
      }
    }

   
    //include in a secondary list sucessors due to non helpful actions
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	add_to_anytime_bfs_roller_policy( &S, first->nonH[i], first );
	//add_to_secondary_list( &S, first->nonH[i], first );
      }
    }
  
   

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;
  



  }

  return TRUE;

}



BfsNode* add_to_anytime_bfs_roller_policy( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  


  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( gcmd_line.optimize && goptimization_established ) {

      /* if hashed rep_node has lower g */
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return NULL;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}
      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
      
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }	   
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return NULL;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated(rep_node,&new_aux);
      
      /* remove hashed node from open  */
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node ->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

      new_aux.h = get_1P_and_H_and_nonH( S );

      if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_hn = gcost;
	/* gtt is mulitplicator of TOTAL-TIME in final metric; if no
	 * total-time part in metric, it is 0
	 */
	new_aux.float_hn += new_aux.h * gtt;

      }

      if ( new_aux.h == INFINITY ) {
	return NULL;
      }
    }

  /* update f */
  if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }


  /* introduce the current node in OPEN */
  /* search for the position in OPEN
   */

  if ( gcmd_line.optimize && goptimization_established ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn)
	break;
    }
  }
  

/* create the new node and include it in the found position in open */
  new = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  new->op = op;
  new->h = new_aux.h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = new_aux.float_fn;
    new->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;

      new->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new->nonH[j] = gA[j];
      }
      new->num_nonH = gnum_A;
    }
  else
    {
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;

      new->num_nonH = new_aux.num_nonH;
      new->nonH= new_aux.nonH;

    }


  new->next = i->next;
  new->prev = i;
  i->next = new;
  if ( new->next ) {
    new->next->prev = new;
  }
  
  hash_bfs_node( new );
  return new;

}





/* similar to roller policy but generating the policy for the lookahead using FF heuristic */
Bool do_anytime_bfs_FF_heuristic_policy( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  BfsNode *last_policy_node;


  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_roller_policy( &ginitial_state, -1, NULL );


  times (&start);

  while ( TRUE ) {
    
    if (!memory_check())
      return FALSE;

    if ( (first = lbfs_space_head->next) == NULL ) {
      /* include all nodes in the secondary list*/
      include_list_in_open(lbfs_secondary_current_head,lbfs_secondary_current_tail,
		      add_to_anytime_bfs_roller_policy);
      lbfs_secondary_current_head = (BfsNode*)0;
      lbfs_secondary_current_tail = (BfsNode*)0;

      if ( (first = lbfs_space_head->next) == NULL )
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      else continue;
 

    }


    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    /*    printf(".");*/
    gexpanded_states ++ ;

    /* prune first by  g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }



    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

    /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);


      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;

    }

    last_policy_node =apply_reactive_policy_FF_heuristic(first,0,add_to_anytime_bfs_roller_policy);

    if (last_policy_node && last_policy_node != first)
      {	    
	    if ( last_policy_node->h == 0 ) {
	      if ( gcmd_line.optimize && goptimization_established ){
		final_total_cost = last_policy_node->float_gn;
	      }
	      else{
		final_total_cost=last_policy_node->g;
	      }

	      /* extract and print solution */
	      extract_plan( last_policy_node );
      
	      times (&end);
	      TIME (gsearch_time);
	      times (&start);


	      print_plan();  
	      output_planner_info();
	      if (gcmd_line.o_file){
		output_planner_info_to_file();
	      }
	      
	      sstart = TRUE;
	      min = INFINITY;
	    
	      /* next break is to stop at first solution: commented */
	      /*break;*/
	      if (gcmd_line.stop_at_first_sol)
		break;
	    }
      }
   
    //include in open the sucessors due to helpful actions
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	add_to_anytime_bfs_roller_policy( &S, first->H[i], first );
      }
    }

   
    //include in a secondary list sucessors due to non helpful actions
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	add_to_anytime_bfs_roller_policy( &S, first->nonH[i], first );
	//add_to_secondary_list( &S, first->nonH[i], first );
      }
    }
  
   

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;
  



  }

  return TRUE;

}



BfsNode *apply_reactive_policy (BfsNode *node, int horizon, BfsNode* (*F_add_to_open) ( State *S, int op, BfsNode *father ))
{
  int i;

 static Bool first_call = TRUE; 
 static State S_prev;
 static State S_new;
 BfsNode *next_node;
 int op;


  if ( first_call ) {
   make_state( &S_new, gnum_ft_conn, gnum_fl_conn );
   make_state( &S_prev, gnum_ft_conn, gnum_fl_conn );
   first_call = FALSE;
  }

  if (gcmd_line.display_info == 130)
    {
      if (horizon == 0)
	{
	  printf("\n\nROLLER: applying reactive policy");
	}
    }

  if (horizon == gcmd_line.roller_bfs_horizon)
    {
      if (gcmd_line.display_info == 130)
	  printf("\nROLLER horizon reached: %d", horizon);
      return node;
    }

  source_to_dest(&S_prev, &(node->S));


  /*update helpful gH */
  for (i = 0; i < gnum_H; i++)
    {
      	gop_conn[gH[i]].is_in_H = FALSE;
    }

  gnum_H = node-> num_H;

  for (i = 0; i < gnum_H; i++)
    {
      gH[i] = node->H[i];
      gop_conn[gH[i]].is_in_H = TRUE;
    }
  
   assign_LT_last_executed_action(node->op);
   assign_LT_current_state(&S_prev);
   match_all();

   /* helpful actions are ordered in increasing roller value order */
   for (i = 0; i < gnum_H; i++)
     {
       op = gH[i];
       /* generate the sucessor */
       if ( gop_conn[gH[i]].roller_value > 0 && result_to_dest( &S_new, &S_prev, gH[i]))
	  {
	    if ((next_node = F_add_to_open( &S_new, gH[i], node)) != NULL)
	      
	      {
		
		if (next_node->h == 0)
		  return next_node;
		else
		  {
		    if (gcmd_line.display_info == 130)
		      {
			printf("\nROLLER policy action number %d (",i);
			print_op_name(op);
			printf("), horizon: %d", horizon);  
			fflush(stdout);
		      }
		    return apply_reactive_policy(next_node, horizon +1, F_add_to_open);
		  }
	      }

	    /* when next_node == NULL --> repeated state or recognized
	       dead-end--> continue with the next node indicated by the
	       roller */
// 	    else                          //--> for stopping the lookahead if repeated node
// 	      return NULL;                //--> otherwise the lookahead continues with the second better option.    
	  }
       //printf("\nROLLER next action to %d, horizon: %d",i, horizon);
     }
   if (gcmd_line.display_info == 130)
     printf("\nROLLER end, horizon: %d", horizon);

   return node;
}

     
/* Apply reactive policy sorting helpful actions using the FF heuristic*/
BfsNode *apply_reactive_policy_FF_heuristic (BfsNode *node, int horizon, BfsNode* (*F_add_to_open) ( State *S, int op, BfsNode *father ))
{
  int i;

 static Bool first_call = TRUE; 
 static State S_prev;
 static State S_new;
 BfsNode *next_node;
 int op;


  if ( first_call ) {
   make_state( &S_new, gnum_ft_conn, gnum_fl_conn );
   make_state( &S_prev, gnum_ft_conn, gnum_fl_conn );
   first_call = FALSE;
  }

  if (gcmd_line.display_info == 130)
    {
      if (horizon == 0)
	{
	  printf("\n\nROLLER: applying reactive policy (FF heuristic)");
	}
    }

  if (horizon == gcmd_line.roller_bfs_horizon)
    {
      if (gcmd_line.display_info == 130)
	  printf("\nROLLER horizon reached (FF heuristic): %d", horizon);
      return node;
    }

  source_to_dest(&S_prev, &(node->S));

  

  /*update helpful gH */
  for (i = 0; i < gnum_H; i++)
    {
      	gop_conn[gH[i]].is_in_H = FALSE;
    }

  gnum_H = node-> num_H;

  for (i = 0; i < gnum_H; i++)
    {
      gH[i] = node->H[i];
      gop_conn[gH[i]].is_in_H = TRUE;
    }
  
  /* vector of heuristic values for sorting helpful actions */
  gH_h_values =  ( HA_h * ) calloc( gnum_H, sizeof( HA_h) );
  generate_heuristic_values_for_HA(node);
  qsort(gH_h_values, gnum_H, sizeof(HA_h), compare_helpful_actions_FF_heuristic);



   /* helpful actions are ordered in increasing FF heuristic value order */
   for (i = 0; i < gnum_H; i++)
     {
       op = gH_h_values[i].HA;
       /* generate the sucessor */
       if (result_to_dest( &S_new, &S_prev,op))
	  {
	    if ((next_node = F_add_to_open( &S_new, op, node)) != NULL)
	      
	      {
		free(gH_h_values);
		if (next_node->h == 0)
		  return next_node;
		else
		  {
		    if (gcmd_line.display_info == 130)
		      {
			printf("\nROLLER FF heuristic policy action number %d (",i);
			print_op_name(op);
			printf("), horizon: %d", horizon);  
			fflush(stdout);
		      }
		    return apply_reactive_policy_FF_heuristic(next_node, horizon +1, F_add_to_open);
		  }
	      }

	    /* when next_node == NULL --> repeated state or recognized
	       dead-end--> continue with the next node indicated by the
	       roller */
// 	    else                          //--> for stopping the lookahead if repeated node
// 	      return NULL;                //--> otherwise the lookahead continues with the second better option.    
	  }
       //printf("\nROLLER next action to %d, horizon: %d",i, horizon);
     }
   if (gcmd_line.display_info == 130)
     printf("\nROLLER end (FF heuristic), horizon: %d", horizon);
   
   free(gH_h_values);
   return node;
}

     
void  generate_heuristic_values_for_HA(BfsNode *node)	 
{
  int i;
  static State S_prev;
  static State S_new;
  static Bool first_call = TRUE; 

  if ( first_call ) {
   make_state( &S_new, gnum_ft_conn, gnum_fl_conn );
   make_state( &S_prev, gnum_ft_conn, gnum_fl_conn );
   first_call = FALSE;
  }



  source_to_dest(&S_prev, &(node->S));

   for (i = 0; i < gnum_H; i++)
     {
       gH_h_values[i].HA = gH[i];
       if (result_to_dest( &S_new, &S_prev, gH[i]))
	 {
	   gH_h_values[i].h = get_1P(&S_new);
	   gevaluated_states_lookahead_FF_heuristic ++;
	 }
       else
	 {
	 gH_h_values[i].h = INFINITY;
	 }
     }
}
	 

int compare_helpful_actions_FF_heuristic( const void* a, const void* b ) {
   HA_h* arg1 = (HA_h*) a;
   HA_h* arg2 = (HA_h*) b;

   if (arg2->h == INFINITY) return 1;
   else
     {
     if (arg1->h <= arg2->h) return -1;
     else return 1;
     }
 }    



/**********************************************************************************
 * ANYTIME BEST FIRST SEARCH IMPLEMENTATION WITH LOOKAHEAD: each node is included in open
 * twice. Once with helpful actions and once with non helpful (rescue) actions
 * as in Vidal ICAPS04

   - Inconsistent heuristic: node re-expansions without new evaluations. Only
     an inconsistent heuristic is used. g is used to prune search space

   - Rescue nodes are not hashed

   - Roller policy


************************************************************************************/



Bool do_anytime_bfs_HA_rescue_roller_policy( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  BfsNode *last_policy_node;



  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  
  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_rescue_space_roller_policy( &ginitial_state, -1, NULL );

  /* load roller trees */
  load_all_tilde_trees();


  times (&start);

  while ( TRUE ) {
    
    if (!memory_check())
      return FALSE;

    if ( (first = lbfs_space_head->next) == NULL ) {
      /* include all nodes in the secondary list*/
      include_list_in_open(lbfs_secondary_current_head,lbfs_secondary_current_tail,
		       add_to_anytime_bfs_HA_rescue_space_roller_policy);
      lbfs_secondary_current_head = (BfsNode*)0;
      lbfs_secondary_current_tail = (BfsNode*)0;

      if ( (first = lbfs_space_head->next) == NULL )
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      else continue;
 
    }

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    gexpanded_states ++ ;

    /* prune first by g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }



    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

       /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);


      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;

    }

    last_policy_node =apply_reactive_policy(first,0, add_to_anytime_bfs_HA_rescue_space_roller_policy);

    if (last_policy_node && last_policy_node != first)
      {	    
	    if ( last_policy_node->h == 0 ) {
	      if ( gcmd_line.optimize && goptimization_established ){
		final_total_cost = last_policy_node->float_gn;
	      }
	      else{
		final_total_cost=last_policy_node->g;
	      }

	      /* extract and print solution */
	      extract_plan( last_policy_node );
      
	      times (&end);
	      TIME (gsearch_time);
	      times (&start);


	      print_plan();  
	      output_planner_info();
	      if (gcmd_line.o_file){
		output_planner_info_to_file();
	      }
	      
	      sstart = TRUE;
	      min = INFINITY;
	    
	      if (gcmd_line.stop_at_first_sol)
		break;
	    }
      }
   

    /*include in open the sucessors due to helpful actions */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->H[i], first );
      }
    }

    /*include in a sucessors due to non helpful actions */
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->nonH[i], first );
      }
    }

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;


  
 
  }

  return TRUE;

}


Bool do_anytime_bfs_HA_rescue_FF_heuristic_policy( void )

{

  BfsNode *first;
  State S;
  int i, min = INFINITY;
  Bool sstart = TRUE;
  struct tms start, end;
  BfsNode *last_policy_node;



  make_state( &S, gnum_ft_conn, gnum_fl_conn );

  
  lbfs_secondary_current_head = (BfsNode*)0;
  lbfs_secondary_current_tail = (BfsNode*)0;

  lbfs_space_head = new_BfsNode();
  lbfs_space_had = NULL;

  for ( i = 0; i < BFS_HASH_SIZE; i++ ) {
    lbfs_hash_entry[i] = NULL;
  }

  add_to_anytime_bfs_HA_rescue_space_roller_policy( &ginitial_state, -1, NULL );


  times (&start);

  while ( TRUE ) {
    
    if (!memory_check())
      return FALSE;

    if ( (first = lbfs_space_head->next) == NULL ) {
      /* include all nodes in the secondary list*/
      include_list_in_open(lbfs_secondary_current_head,lbfs_secondary_current_tail,
		       add_to_anytime_bfs_HA_rescue_space_roller_policy);
      lbfs_secondary_current_head = (BfsNode*)0;
      lbfs_secondary_current_tail = (BfsNode*)0;

      if ( (first = lbfs_space_head->next) == NULL )
	{
	  if ( gcmd_line.display_info ) {
	    printf("\n\nsearch space empty!\n\n");
	  }
	  return FALSE;
	}
      else continue;
 
    }

    /* extract first from open */
    lbfs_space_head->next = first->next;
    if ( first->next ) {
      first->next->prev = lbfs_space_head;
    }

    gexpanded_states ++ ;

    /* prune first by g values using best solution found
       cost */
    if ( gcmd_line.optimize && goptimization_established ) {
      if (LESS_EQ(final_total_cost, first->float_gn)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
      
    }
    else
    {
      if  (LESS_EQ(final_total_cost, first->g)){
	/* close the pruned node */
	first->closed = TRUE;
	first->next = lbfs_space_had;
	lbfs_space_had = first;
	continue;
      }
    }



    /* stdout*/
    if ( LESS( first->h, min ) ) {
      min = first->h;
      if ( sstart ) {
	if ( gcmd_line.display_info ) {
	  printf("\n\nadvancing to distance: %4d", min);
	  fflush(stdout);
	}
	sstart = FALSE;
      } else {
	if ( gcmd_line.display_info ) {
	  printf("\n                       %4d", min);
	  fflush(stdout);
	}
      }
    }

       /* compute the solution cost: final_total_cost  */
    if ( first->h == 0 ) {
      if ( gcmd_line.optimize && goptimization_established ){
	final_total_cost=first->float_gn;
      }
      else{
	final_total_cost=first->g;
      }

      /* extract and print solution */
      extract_plan( first );
      
      times (&end);
      TIME (gsearch_time);
      times (&start);


      print_plan();  
      output_planner_info();
      if (gcmd_line.o_file){
	output_planner_info_to_file();
      }

      sstart = TRUE;
      min = INFINITY;
	    
      /* next break is to stop at first solution: commented */
      /*break;*/
      if (gcmd_line.stop_at_first_sol)
	break;

    }

    last_policy_node =apply_reactive_policy_FF_heuristic(first,0, add_to_anytime_bfs_HA_rescue_space_roller_policy);

    if (last_policy_node && last_policy_node != first)
      {	    
	    if ( last_policy_node->h == 0 ) {
	      if ( gcmd_line.optimize && goptimization_established ){
		final_total_cost = last_policy_node->float_gn;
	      }
	      else{
		final_total_cost=last_policy_node->g;
	      }

	      /* extract and print solution */
	      extract_plan( last_policy_node );
      
	      times (&end);
	      TIME (gsearch_time);
	      times (&start);


	      print_plan();  
	      output_planner_info();
	      if (gcmd_line.o_file){
		output_planner_info_to_file();
	      }
	      
	      sstart = TRUE;
	      min = INFINITY;
	    
	      if (gcmd_line.stop_at_first_sol)
		break;
	    }
      }
   

    /*include in open the sucessors due to helpful actions */
    for ( i = 0; i < first->num_H; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->H[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->H[i], first );
      }
    }

    /*include in a sucessors due to non helpful actions */
    for ( i = 0; i < first->num_nonH; i++ ) {
      if ( result_to_dest( &S, &(first->S), first->nonH[i] ) ) {
	/* we must include a check here whether the numerical part of the action
	 * is entirely fulfilled; only those actions are applied.
	 */
	add_to_secondary_list( &S, first->nonH[i], first );
      }
    }

    /* closed list*/
    first->closed = TRUE;
    first->next = lbfs_space_had;
    lbfs_space_had = first;


  
 
  }

  return TRUE;

}



BfsNode *add_to_anytime_bfs_HA_rescue_space_roller_policy( State *S, int op, BfsNode *father )

{

  BfsNode *new, *i, *rep_node, *new_rescue;
  int j;
  Bool already_evaluated = FALSE;
  BfsNode new_aux;
  

  /* compute g 
   */
  if ( father ) {
    new_aux.g = father->g + 1;
  } else {
    new_aux.g = 0;
  }

  if ( gcmd_line.optimize && goptimization_established ) {
    new_aux.float_gn = state_cost( S, father );
    if (LESS_EQ(final_total_cost, new_aux.float_gn))
      return NULL;
  }
  else
    {
      if (LESS_EQ(final_total_cost,new_aux.g))
	return NULL;
    }


  /* test duplicates 
   */
  if ((rep_node = same_no_metric_bfs_state_hashed( S,father ))){


    if ( (gcmd_line.optimize && goptimization_established)){

    /* if hashed rep_node has lower g then return */      
      if (LESS_EQ(rep_node->float_gn, new_aux.float_gn))
	return NULL;
 
     /* else 
       */
      
      /* copy evaluation data in the variables of the new future rep_node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated_rescue(rep_node,&new_aux);

      
      /* remove hashed node from open  */
      
      if (!rep_node->closed)
	{
	  rep_node->prev->next = rep_node->next;
	  if (rep_node->next)
	    rep_node->next->prev = rep_node->prev;
	}

      /* remove the correspond rescue node from open */
      if (rep_node->rescue && !rep_node->rescue->closed)
	{
	  rep_node->rescue->prev->next = rep_node->rescue->next;
	  if (rep_node->rescue->next)
	    rep_node->rescue->next->prev = rep_node->rescue->prev;
	}

      /* unhash hashed rep_node */
      /* the new future node correspond to the same state and is
	 hashed below */
       unhash_bfs_node(rep_node);
      
      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
       if (rep_node->rescue && !rep_node->rescue->closed)
	 {
	   free (rep_node->rescue->S.F); 
	   free (rep_node->rescue);
	 }

    }
    else {
      /* if hashed node has lower g */
      if (rep_node->g <= new_aux.g)
	return NULL;

      /* else 
       */
      
      /* copy evaluation data in the variables of the new future node to avoid
	a re-evaluation
      */
      copy_BfsNode_evaluated_rescue(rep_node,&new_aux);
      
      /* remove hashed node from open  */
      if (!rep_node->closed){
	rep_node->prev->next = rep_node ->next;
	if (rep_node->next)
	  rep_node->next->prev = rep_node->prev;
      }

      /* remove the correspond rescue node from open */
      if (rep_node->rescue &&!rep_node->rescue->closed)
	{
	  rep_node->rescue->prev->next = rep_node->rescue->next;
	  if (rep_node->rescue->next)
	    rep_node->rescue->next->prev = rep_node->rescue->prev;
	}

      /* unhash hashed node */
      /* the new future node correspond to the same state and will be next
	 hashed */
       unhash_bfs_node(rep_node);

      /* free memory */
      /* if the node is closed does not matter: it could be also removed but
	  for the moment is not. If you want to remove it (memory
	  requirements) be care of the closed list pointer lbfs_space_had */
       if (!rep_node->closed)
	 {
	   free (rep_node->S.F); 
	   free (rep_node);
	 }
       if (rep_node->rescue && !rep_node->rescue->closed)
	 {
	   free (rep_node->rescue->S.F); 
	   free (rep_node->rescue);
	 }

    }

    already_evaluated = TRUE;
  }
  
  /*never re-evaluate evaluated nodes */
  if (!already_evaluated)
    {

       new_aux.h = get_1P_and_H_and_nonH( S );
       
       if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_hn = gcost;
	new_aux.float_hn += new_aux.h * gtt;

       }
       
      if ( new_aux.h == INFINITY ) {
	return NULL;
      }
    }

  /* update f */
  if ( gcmd_line.optimize && goptimization_established ) {
	new_aux.float_fn = (((float) gcmd_line.g_weight) * new_aux.float_gn) + 
	  (((float) gcmd_line.h_weight) * new_aux.float_hn);

  }
  else {
    new_aux.int_fn = (gcmd_line.g_weight * new_aux.g) + (gcmd_line.h_weight * new_aux.h);
  }


  /* introduce the current node in OPEN */
  /* search for the position in OPEN: solve ties in favor of lower h
   */
  
  if ( gcmd_line.optimize && goptimization_established ) {

    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( LESS_EQ(new_aux.float_fn, i->next->float_fn))
	break;
    }
  } else {
    for ( i = lbfs_space_head; i->next; i = i->next ) {
      if ( i->next->int_fn >= new_aux.int_fn)
	break;
    }
  }


/* create the new node and the rescue and include them in the found position
   in open. First the new node, then the new rescue node */
  new = new_BfsNode();
  new_rescue = new_BfsNode();
  copy_source_to_dest( &(new->S), S );
  copy_source_to_dest( &(new_rescue->S), S );
  new->op = op;
  new_rescue->op = op;
  new->h = new_aux.h;
  new_rescue->h = new_aux.h;
  if ( gcmd_line.optimize && goptimization_established ) {
    new->float_fn = new_aux.float_fn;
    new_rescue->float_fn = new_aux.float_fn ;
    new->float_hn = new_aux.float_hn;
    new_rescue->float_hn = new_aux.float_hn;
    new->float_gn = new_aux.float_gn;
    new_rescue->float_gn = new_aux.float_gn;
  } else {
    new->int_fn = new_aux.int_fn;
    new_rescue->int_fn = new_aux.int_fn;
  }
  new->father = father;
  new_rescue->father = father;
  new->g = new_aux.g;
  new_rescue->g = new_aux.g;

  /* only ask for memory for applicable actions when the node has been
     evaluated here. Otherwise copy them*/
  if (!already_evaluated)
    {
      /*sucessors*/
      new->H = ( int * ) calloc( gnum_H, sizeof( int ) );
      for ( j = 0; j < gnum_H; j++ ) {
	new->H[j] = gH[j];
      }
      new->num_H = gnum_H;
      new->num_nonH = 0;

      new_rescue->nonH = ( int * ) calloc( gnum_A, sizeof( int ) );
      for ( j = 0; j < gnum_A; j++ ) {
	new_rescue->nonH[j] = gA[j];
      }
      new_rescue->num_nonH = gnum_A;
      new_rescue->num_H = 0;

    }
  else
    {
      /* sucessors*/
      new->num_H = new_aux.num_H;
      new->H = new_aux.H;
      new->nonH= 0;

      new_rescue->num_nonH = new_aux.num_nonH;
      new_rescue->nonH= new_aux.nonH;
      new_rescue->num_H = 0;
      
    }

  /* update open list pointers*/
  
  new->rescue = new_rescue;
  new->next = new_rescue;
  new_rescue->next=i->next;
  new->prev = i;
  new_rescue->prev=new;
  i->next = new;

  if ( new_rescue->next ) {
    new_rescue->next->prev = new_rescue;
  }

  hash_bfs_node( new );

  return new;
}


void print_list(BfsNode *list_head)
{
  BfsNode *first;
  BfsNode *next;

  if (list_head)
    first =  list_head->next;
  else return;

  printf("\n------ LISTA-------\n");
  while ( first ) {

    if (first->op != -1)
      {
	print_op_name(first->op);
        printf("\n");
      }
    next = first->next;
    first = next;
  }
}
