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
 * File: relax.c
 * Description: this file handles the relaxed planning problem, i.e.,
 *              the code is responsible for the heuristic evaluation
 *              of states during search.
 *
 *              --- THE HEART PEACE OF THE FF PLANNER ! ---
 *
 *              here: linear tasks +=,-=,:= / le / le <comp> le
 *
 *
 * Author: Joerg Hoffmann 2001
 *
 * Modified by Raquel Fuentetaja 2008 to include cost heuristics
 * The new code implements: h_level_max, h_level_add, h_max and h_add
 * and adds new adds new functionalities needed by different 
 * search algorithms
 *        
 *********************************************************************/ 









#include "ff.h"

#include "output.h"
#include "memory.h"

#include "expressions.h"

#include "relax.h"
#include "search.h"







/* local globals
 */





/* fixpoint
 */
int *lFa;
int lnum_Fa;
int *lE;
int lnum_E;

int *lch_E;
int lnum_ch_E;

int *l0P_E;
int lnum_0P_E;





/* 1P extraction
 */
int **lgoals_at;
int *lnum_goals_at;

float **lf_goals_c_at;
Comparator **lf_goals_comp_at;

int lh;
int sum_difficulty;

int *lch_F;
int lnum_ch_F;

int *lused_O;
int lnum_used_O;

int *lin_plan_E;
int lnum_in_plan_E;


/* helpful actions numerical helpers
 */
Comparator *lHcomp;
float *lHc;




/* raquel: for hlevel computation */

/* cost associated to each level */

float *level_cost;

OpenApp *OpenApp_head;

/* applicable actions in level 0 to compute helpful actions (not deactivated) */
int *lE_level0;
int lnum_E_level0;
int *one_step_lgoals;
int lnum_one_step_lgoals;



int *lE_deactivated;
int lnum_E_deactivated;

/* applicable actions in level 0 but deactivated */
int *lE_level0_deactivated;
int lnum_E_level0_deactivated;








/*************************************
 * helper, for -1 == INFINITY method *
 *************************************/







/* raquel: I modify the type of arguments of the LESS 
   function to be float
*/

float mi_abs(float a)
{
  return (a > 0 ? a : (-1) *a);
}


Bool LESS( float a, float b )

{
  /* precision: 2 decimals */
  

  if ( a == INFINITY ) {
    return FALSE;
  }

  if ( b == INFINITY ) {
    return TRUE;
  }

  return ( (a < b && mi_abs(a-b) > 0.001) ? TRUE : FALSE );

}

Bool LESS_EQ( float a, float b )

{
  /* precision: 2 decimals */
  

  if ( a == INFINITY ) {
    return FALSE;
  }

  if ( b == INFINITY ) {
    return TRUE;
  }


  return (  ((a < b && mi_abs(a-b) > 0.001) ||  mi_abs(a-b) < 0.001)  ? TRUE : FALSE );

}
Bool FLOAT_EQUAL( float a, float b )

{
  /* precision: 2 decimals */
  

  if (( a == INFINITY &&  b != INFINITY ) ||
      ( b == INFINITY &&  a != INFINITY )) {
  return FALSE;
  }

  return ( mi_abs((float)(a-b)) < 0.001 ? TRUE : FALSE );

}












/***********************************
 * FUNCTIONS ACCESSED FROM OUTSIDE *
 ***********************************/

















int get_1P( State *S )

{

  int max, h;
  Bool solvable;

  gevaluated_states++;

  solvable = build_fixpoint( S, &max );

  if ( gcmd_line.display_info == 126 ) {
    print_fixpoint_result();
  }

  if ( solvable ) {
    h = extract_1P( max );
  } else {
    h = INFINITY;
  }

  reset_fixpoint( max );

  if ( gcmd_line.display_info == 127 ) {
    printf("\n->integer-h %d, cost-h %f\n", h, gcost);
  }

  return h;

}


int get_1P_and_H_prueba( State *S )
{ int h;
  int gnum_RP2;
  int *g_RP2;
  int igual1= 0;
  int igual2 = 0;
  float gcost1;

  int i;
  int j;
  int ok=1;

      printf("\n*******************************************************************");
      gcmd_line.heuristic = 2;
      get_1P_and_H(S);
      /*      print_relaxed_plan();*/
      gcost1= gcost;
      gnum_RP2= gnum_RP;
      g_RP2 =  ( int * ) calloc( gnum_RP2, sizeof( int ) );

      for (i = gnum_RP2 -1; i>=0 ;i --)
	{
	  g_RP2[i]=g_RP[i];
	}

      gcmd_line.heuristic =1;
      h = get_1P_and_H(S);
      /*      print_relaxed_plan();*/

      for(i = 0; i<gnum_RP; i++)
	{
	  if (ok == 0)
	    {
	      printf("\n --distinto1");
	      break;
	    }
	  ok = 0;
	  for (j=0; j <gnum_RP2;j++)
	    {
	      if (g_RP[i]==g_RP2[j])
		{
		  ok = 1;
                  break;
		}

	    }
	}
      if( ok == 1){
	      printf("\n --igual1");
	      igual1 = 1;
      }
      ok = 1;
      for(i = 0; i<gnum_RP2; i++)
	{
	  if (ok == 0)
	    {
	      printf("\n --distinto2");
	      break;
	    }
	  ok = 0;
	  for (j=0; j <gnum_RP;j++)
	    {
	      if (g_RP2[i]==g_RP[j])
		{
		  ok = 1;
                  break;
		}

	    }
	}
      if( ok == 1)
	{     igual2=1;
	      printf("\n --igual2");
	}

      if (igual1== 1 && igual2==1)
	{
	  numero_rp_igual++;
	}
      else
	{
	  numero_rp_distinto++;
	  printf("\n----%f, %f", gcost1, gcost); 
	  if (FLOAT_EQUAL(gcost1,gcost))
	    {
	      distinto_misma_h++;
	    }
	  else if (LESS_EQ(gcost,gcost1))
	    {
	      distinto_mayor_add++;
	    }
	}
      if (LESS_EQ(gcost,gcost1))
	diff = diff + gcost1 - gcost;
      else	
	diff = diff + gcost - gcost1;
      
      printf("\n*******---> igual %d, distinto %d, misma %d mayor_add %d mayor_max %d diff %f",numero_rp_igual, numero_rp_distinto, distinto_misma_h, distinto_mayor_add,
	     numero_rp_distinto - distinto_misma_h - distinto_mayor_add,
             diff/(numero_rp_igual+numero_rp_distinto)
);
      
      return h;

}

int get_1P_and_H( State *S )

{

  int max, h;
  Bool solvable;

 
  gevaluated_states++;

  solvable = build_fixpoint( S, &max );
  


  if ( gcmd_line.display_info == 126 ) {
    print_fixpoint_result();
  }

  if ( solvable ) {
    h = extract_1P( max );
    collect_H_info();
  } else {
    h = INFINITY;
  }

  //  print_relaxed_plan();

  reset_fixpoint( max );

  if ( gcmd_line.display_info == 127 ) {
    printf("\n->integer-h %d, cost-h %f\n", h, gcost);
  }


  return h;


}

int get_1P_and_H_and_nonH( State *S )

{

  int max, h;
  Bool solvable;

  gevaluated_states++;

  solvable = build_fixpoint( S, &max );

  if ( gcmd_line.display_info == 126 ) {
    print_fixpoint_result();
  }

  if ( solvable ) {
    h = extract_1P( max );
    collect_H_info();
    collect_A_info(TRUE);
  } else {
    h = INFINITY;
  }

  reset_fixpoint( max );

  if ( gcmd_line.display_info == 127 ) {
    printf("\n->integer-h %d, cost-h %f\n", h, gcost);
  }

  return h;


}



int get_1P_and_A( State *S )

{

  int max, h, i;
  Bool solvable;

  gevaluated_states++;

  solvable = build_fixpoint( S, &max );

  if ( gcmd_line.display_info == 126 ) {
    print_fixpoint_result();
  }

  if ( solvable ) {
    h = extract_1P( max );
  } else {
    h = INFINITY;
  }

  collect_A_info(FALSE);
  reset_fixpoint( max );

  if ( gcmd_line.display_info == 127 ) {
    printf("\n->integer-h %d, cost-h %f\n", h, gcost);
  }

  if ( gcmd_line.display_info == 128 ) {
    printf("\ncollected A: ");
    for ( i = 0; i < gnum_A; i++ ) {
      print_op_name( gA[i] );
      printf("\n             ");
    }
  }

  return h;

}



void get_A( State *S )

{

  int i;

  initialize_fixpoint( S );
  
  for ( i = 0; i < lnum_Fa; i++ ) {
    activate_ft( lFa[i], 0 );
  }
  for ( i = 0; i < lnum_0P_E; i++ ) {
    if ( gef_conn[l0P_E[i]].in_E ) {
      continue;
    }
    new_ef( l0P_E[i],0 );
  }
  for ( i = 0; i < gnum_fl_conn; i++ ) {
    activate_fl( i, 0 );
  }
  
  /* fixpoint is not build, so collect actions from lE */
  lnum_E_level0 = lnum_E;
  for ( i = 0; i< lnum_E; i++)
    {
      lE_level0[i] = lE[i];
    }

  for ( i = 0; i< lnum_E_deactivated; i++)
    {
      lE_level0_deactivated[i] = lE_deactivated[i];
    }


  collect_A_info(FALSE);

  /* 0 should be enough here...
   */
  reset_fixpoint( 1 );

  if ( gcmd_line.display_info == 128 ) {
    printf("\ncollected A: ");
    for ( i = 0; i < gnum_A; i++ ) {
      print_op_name( gA[i] );
      printf("\n             ");
    }
  }

}

/* raquel: when ignore_HA is true, this fucntion computes applicable
   non-helpful actions. In that case, this function must be executed always
   after computing helpful actions (function collect_H_info).  The result will
   be in gA
*/

void collect_A_info( Bool ignore_HA )

{

  static Bool first_call = TRUE;



  int i;

  if ( first_call ) {
    gA = ( int * ) calloc( gnum_op_conn, sizeof( int ) );
    gnum_A = 0;
    first_call = FALSE;
  }

  for ( i = 0; i < gnum_A; i++ ) {
    gop_conn[gA[i]].is_in_A = FALSE;
  }
  gnum_A = 0;

  for ( i = 0; i < lnum_E_level0; i++ ) {

    if ( ignore_HA && gop_conn[gef_conn[lE_level0[i]].op].is_in_H ) {
      continue;
    }
    if ( gop_conn[gef_conn[lE_level0[i]].op].is_in_A ) {
      continue;
    }
    gop_conn[gef_conn[lE_level0[i]].op].is_in_A = TRUE;
    gA[gnum_A++] = gef_conn[lE_level0[i]].op;
  }

  for ( i = 0; i < lnum_E_level0_deactivated; i++ ) {
    if (ignore_HA && gop_conn[gef_conn[lE_level0[i]].op].is_in_H ) {
      continue;
    }

    if ( gop_conn[gef_conn[lE_level0_deactivated[i]].op].is_in_A ) {
      continue;
    }
    gop_conn[gef_conn[lE_level0_deactivated[i]].op].is_in_A = TRUE;
    gA[gnum_A++] = gef_conn[lE_level0_deactivated[i]].op;
  }

  if ( gcmd_line.display_info == 128 ) {
    printf("\ncollected A (ignore HA = %d): ", ignore_HA);
    for ( i = 0; i < gnum_A; i++ ) {
      print_op_name( gA[i] );
      printf("\n              ");
    }

  }
}


























/*******************************
 * RELAXED FIXPOINT ON A STATE *
 *******************************/


























Bool build_fixpoint(State *S, int *max)
{
  Bool result; 

  struct tms start, end;
  float time_relax=0;

  times(&start);

  switch (gcmd_line.heuristic) {

  case H_MFF: result=build_fixpoint_mff(S,max); break;
    /* functions to compute hlevel   
     */
  case H_LEVEL_MAX: result=build_fixpoint_hlevel_max(S, max); break;
		    
  case H_LEVEL_ADD: result=build_fixpoint_hlevel_add(S, max); break;

    /* h max computed using the RPG generated by hlevel max
     */
  case H_MAX:
  case H_MAX_HA:
  case H_SUM_MAX:
  case H_SUM_MAX_HA: result=build_fixpoint_hlevel_max(S, max); break; 

    /* h add computed using the RPG generated by hlevel add
     */
  case H_ADD:
  case H_ADD_HA:
  case H_MAX_ADD:
  case H_MAX_ADD_HA: result=build_fixpoint_hlevel_add(S, max); break;


  } 
  times (&end);
  TIME (time_relax);
  total_time_relax+=time_relax;
  total_levels_relax+=*max;
  return result;
}




Bool build_fixpoint_mff( State *S, int *max )

{

  int start_ft, stop_ft, start_ef, stop_ef, i, time = 0;

  initialize_fixpoint( S );

  start_ft = 0;
  start_ef = 0;
  while ( TRUE ) {
    if ( all_goals_activated( time ) ) {
      break;
    }
    if ( start_ft == lnum_Fa ) {
      if ( fluents_hopeless( time ) ) {
	/* fixpoint, goals not reached
	 */
	*max = time;
	return FALSE;
      }
    }
    /* make space if necessary, and copy over
     * info from time to time+1 for fluents
     */
    extend_fluent_levels( time );
    for ( i = 0; i < gnum_fl_conn; i++ ) {
      if ( gfl_conn[i].def[time] ) {
	gfl_conn[i].def[time+1] = TRUE;
	gfl_conn[i].level[time+1] = gfl_conn[i].level[time];
      }
    }

    /* determine the next effect layer:
     * - activate the facts
     * - activate the fluents at their <time> level
     * - if level 0 activate the no preconds-ops
     */
    stop_ft = lnum_Fa;
    for ( i = start_ft; i < stop_ft; i++ ) {
      activate_ft( lFa[i], time );
    }

    for ( i = 0; i < gnum_fl_conn; i++ ) {
      activate_fl( i, time );
    }

    if ( time == 0 ) {
      for ( i = 0; i < lnum_0P_E; i++ ) {
	if ( gef_conn[l0P_E[i]].in_E ) {
	  continue;
	}
	new_ef( l0P_E[i],0 );
      }

      /* raquel: save the applicable actions in level 0 */
      for ( i = 0; i < lnum_E; i++ ){
	lE_level0[lnum_E_level0++]=lE[i];
	gef_conn[lE[i]].level_applicable = 0;
      }
      for ( i = 0; i < lnum_E_deactivated; i++ ){
	lE_level0_deactivated[lnum_E_level0_deactivated++]=lE_deactivated[i];
      }


    }


    /* now say what the benefits of applying the new
     * effect layer are:
     * 
     * the facts,
     * plus the new fluent levels at time + 1.
     */
    stop_ef = lnum_E;
    for ( i = start_ef; i < stop_ef; i++ ) {
      activate_ef( lE[i], time );
    }
    /* on top of what the new effects have added to the fluents, all effects 
     * strictly below <time> can be applied again. Their effect might be 
     * different from what they've done before, as it might depend on
     * the rh fluent fl_
     */
    if ( time > 0 ) {
      for ( i = 0; i < start_ef; i++ ) {
	apply_ef( lE[i], time );
      }
    }
    /* now check whether there became any assigner applicable that is
     * mightier than all the increasers put together.
     *
     * this needs only be done for the non-artificial
     * fluents as the others obviously don't get
     * assigned at all.
     */
    for ( i = 0; i < gnum_real_fl_conn; i++ ) {
      if ( !gfl_conn[i].curr_assigned ) {
	/* no assigner in yet
	 */
	continue;
      }
      if ( !gfl_conn[i].def[time] ) {
	gfl_conn[i].def[time+1] = TRUE;
	gfl_conn[i].level[time+1] = gfl_conn[i].curr_max_assigned;
	continue;
      }
      if ( gfl_conn[i].curr_max_assigned > gfl_conn[i].level[time+1] ) {
	gfl_conn[i].level[time+1] = gfl_conn[i].curr_max_assigned;
      }
    }
    /* finally, determine the new levels of the artificial fluents
     * at the new time step.
     */
    determine_artificial_fl_levels( time + 1 );

    start_ft = stop_ft;
    start_ef = stop_ef;
    time++;
  }

  *max = time;
  return TRUE;

}


Bool build_fixpoint_hlevel_max( State *S, int *max )

{

  int start_ft, stop_ft, start_ef, i, time = 0;

  initialize_fixpoint( S );

  start_ft = 0;

  OpenApp *first;

  OpenApp_head = new_OpenApp();

  /* raquel: initialize level cost*/
  level_cost[0]=0.0;


  while ( TRUE ) {
    if ( all_goals_activated( time ) ) {
      break;
    }
    if ( start_ft == lnum_Fa && !OpenApp_head->next) {
      if ( fluents_hopeless( time ) ) {
	/* fixpoint, goals not reached
	 */
	*max = time;
	return FALSE;
      }
    }
    /* make space if necessary, and copy over
     * info from time to time+1 for fluents
     */
    extend_fluent_levels( time );
    for ( i = 0; i < gnum_fl_conn; i++ ) {
      if ( gfl_conn[i].def[time] ) {
	gfl_conn[i].def[time+1] = TRUE;
	gfl_conn[i].level[time+1] = gfl_conn[i].level[time];
      }
    }

    /* determine the next effect layer:
     * - activate the facts
     * - activate the fluents at their <time> level
     * - if level 0 activate the no preconds-ops
     */
    stop_ft = lnum_Fa;
    for ( i = start_ft; i < stop_ft; i++ ) {
      activate_ft_hlevel_max( lFa[i], time );
    }

    for ( i = 0; i < gnum_fl_conn; i++ ) {
      activate_fl_hlevel_max( i, time );
    }

    if ( time == 0 ) {


      for ( i = 0; i < lnum_0P_E; i++ ) {
	if ( gef_conn[l0P_E[i]].in_E ) {
	  continue;
	}
	new_ef_hlevel( l0P_E[i],0 );
      }
      
      first = OpenApp_head->next;

      /* raquel: save the applicable actions in level 0 */
      while (first){
	lE_level0[lnum_E_level0++]=first->ef;
	gef_conn[first->ef].level_applicable = 0;
	//printf("\n%d",first->ef);
	/*
        printf("\n");
        print_op_name(gef_conn[first->ef].op);
	fflush(stdout);
	*/
	first=first->next;
      }
      for ( i = 0; i < lnum_E_deactivated; i++ ){
	lE_level0_deactivated[lnum_E_level0_deactivated++]=lE_deactivated[i];
      }

   
    }

    /* now say what the benefits of applying the new
     * effect layer are:
     * 
     * the facts,
     * plus the new fluent levels at time + 1.
     */
  
    first = OpenApp_head->next;

    /* raquel: fix the level cost to the acc_cost of the first in OpenApp 
     */
    if (first)
      level_cost[time+1] = gef_conn[first->ef].acc_cost;
 
    start_ef = lnum_E;



    /* raquel: apply all actions with the same cost as first
     */
    //    print_OpenApp();
    while(first && FLOAT_EQUAL(gef_conn[first->ef].acc_cost, 
				    level_cost[time+1])){

      lE[lnum_E++] = first->ef;
      activate_ef(first->ef, time );
      

      OpenApp_head->next = first->next;
      free(first);
      first = OpenApp_head->next;
      
    }
    /* on top of what the new effects have added to the fluents, all effects 
     * strictly below <time> can be applied again. Their effect might be 
     * different from what they've done before, as it might depend on
     * the rh fluent fl_
     */
    if ( time > 0 ) {
      for ( i = 0; i < start_ef; i++ ) {
	apply_ef( lE[i], time );
      }
    }
    /* now check whether there became any assigner applicable that is
     * mightier than all the increasers put together.
     *
     * this needs only be done for the non-artificial
     * fluents as the others obviously don't get
     * assigned at all.
     */
    for ( i = 0; i < gnum_real_fl_conn; i++ ) {
      if ( !gfl_conn[i].curr_assigned ) {
	/* no assigner in yet
	 */
	continue;
      }
      if ( !gfl_conn[i].def[time] ) {
	gfl_conn[i].def[time+1] = TRUE;
	gfl_conn[i].level[time+1] = gfl_conn[i].curr_max_assigned;
	continue;
      }
      if ( gfl_conn[i].curr_max_assigned > gfl_conn[i].level[time+1] ) {
	gfl_conn[i].level[time+1] = gfl_conn[i].curr_max_assigned;
      }
    }
    /* finally, determine the new levels of the artificial fluents
     * at the new time step.
     */
    determine_artificial_fl_levels( time + 1 );

    start_ft = stop_ft;
    time++;
  }

  *max = time;

  return TRUE;

}

Bool build_fixpoint_hlevel_add( State *S, int *max )

{

  int start_ft, stop_ft, start_ef, i, time = 0;

  initialize_fixpoint( S );

  start_ft = 0;

  OpenApp *first;

  OpenApp_head = new_OpenApp();

  /* raquel: initialize level cost*/
  level_cost[0]=0.0;



  while ( TRUE ) {
    if ( all_goals_activated( time ) ) {
      break;
    }
    if ( start_ft == lnum_Fa && !OpenApp_head->next) {
      if ( fluents_hopeless( time ) ) {
	/* fixpoint, goals not reached
	 */
	*max = time;
	return FALSE;
      }
    }
    /* make space if necessary, and copy over
     * info from time to time+1 for fluents
     */
    extend_fluent_levels( time );
    for ( i = 0; i < gnum_fl_conn; i++ ) {
      if ( gfl_conn[i].def[time] ) {
	gfl_conn[i].def[time+1] = TRUE;
	gfl_conn[i].level[time+1] = gfl_conn[i].level[time];
      }
    }

    /* determine the next effect layer:
     * - activate the facts
     * - activate the fluents at their <time> level
     * - if level 0 activate the no preconds-ops
     */
    stop_ft = lnum_Fa;
    for ( i = start_ft; i < stop_ft; i++ ) {
      activate_ft_hlevel_add( lFa[i], time );
    }

    for ( i = 0; i < gnum_fl_conn; i++ ) {
      activate_fl_hlevel_add( i, time );
    }

    if ( time == 0 ) {

      for ( i = 0; i < lnum_0P_E; i++ ) {
	if ( gef_conn[l0P_E[i]].in_E ) {
	  continue;
	}
	new_ef_hlevel( l0P_E[i], 0 );
      }
      
      first = OpenApp_head->next;
    /* raquel: save the applicable actions in level 0 */
      while (first){
	lE_level0[lnum_E_level0++]=first->ef;
	gef_conn[first->ef].level_applicable = 0;
	first=first->next;
      }
      for ( i = 0; i < lnum_E_deactivated; i++ ){
	lE_level0_deactivated[lnum_E_level0_deactivated++]=lE_deactivated[i];
      }

   
    }

    /* now say what the benefits of applying the new
     * effect layer are:
     * 
     * the facts,
     * plus the new fluent levels at time + 1.
     */
  
    first = OpenApp_head->next;

    /* raquel: fix the level cost to the acc_cost of the first in OpenApp 
     */
    if (first)
      {
      level_cost[time+1] = gef_conn[first->ef].acc_cost;
      }
 
    start_ef = lnum_E;



    /* raquel: apply all actions with the same cost as first
     */
    //    print_OpenApp();
    while(first && FLOAT_EQUAL(gef_conn[first->ef].acc_cost, 
				    level_cost[time+1])){

      lE[lnum_E++] = first->ef;
      
      /*
      printf("\nACTIVATING AT TIME %d", time);
      print_op_name(gef_conn[first->ef].op);
      */
      activate_ef(first->ef, time );
      

      OpenApp_head->next = first->next;
      free(first);
      first = OpenApp_head->next;
      
    }
    /* on top of what the new effects have added to the fluents, all effects 
     * strictly below <time> can be applied again. Their effect might be 
     * different from what they've done before, as it might depend on
     * the rh fluent fl_
     */
    if ( time > 0 ) {
      for ( i = 0; i < start_ef; i++ ) {
	apply_ef( lE[i], time );
      }
    }
    /* now check whether there became any assigner applicable that is
     * mightier than all the increasers put together.
     *
     * this needs only be done for the non-artificial
     * fluents as the others obviously don't get
     * assigned at all.
     */
    for ( i = 0; i < gnum_real_fl_conn; i++ ) {
      if ( !gfl_conn[i].curr_assigned ) {
	/* no assigner in yet
	 */
	continue;
      }
      if ( !gfl_conn[i].def[time] ) {
	gfl_conn[i].def[time+1] = TRUE;
	gfl_conn[i].level[time+1] = gfl_conn[i].curr_max_assigned;
	continue;
      }
      if ( gfl_conn[i].curr_max_assigned > gfl_conn[i].level[time+1] ) {
	gfl_conn[i].level[time+1] = gfl_conn[i].curr_max_assigned;
      }
    }
    /* finally, determine the new levels of the artificial fluents
     * at the new time step.
     */
    determine_artificial_fl_levels( time + 1 );

    start_ft = stop_ft;
    time++;
  }

  *max = time;
  
  return TRUE;

}



Bool fluents_hopeless( int time )

{

  int i, j;
  Bool minusinfty;
  float mneed;

  /* if no real fluent has improved from the previous step to this one,
   * then - with facts not improving either - the process has reached a fixpoint.
   *
   * for the formal details of this, ie. the mneed values, see the JAIR article.
   */

  if ( time == 0 ) {
    /* nothing has happened yet...
     */
    return FALSE;
  }

  for ( i = 0; i < gnum_real_fl_conn; i++ ) {
    gmneed_start_D[i] = gfl_conn[i].def[time-1];
    gmneed_start_V[i] = gfl_conn[i].level[time-1];
  }

  for ( i = 0; i < gnum_real_fl_conn; i++ ) {
    if ( !gfl_conn[i].def[time-1] &&
	 !gfl_conn[i].def[time] ) {
      /* this one has obviously not been made any better
       */
      continue;
    }
    if ( gfl_conn[i].def[time-1] &&
	 gfl_conn[i].level[time] == gfl_conn[i].level[time-1] ) {
      /* this one has not improved either
       */
      continue;
    }
    get_mneed( i, &minusinfty, &mneed );
    if ( gcmd_line.display_info == 333 ) {
      printf("\nstart values:");
      for ( j = 0; j < gnum_real_fl_conn; j++ ) {
	printf("\n"); print_fl_name( j ); 
	printf(" --- %d, %f", gmneed_start_D[j], gmneed_start_V[j]);
      }
      printf("\nmneed "); print_fl_name( i ); 
      printf(" --- %d, %f", minusinfty, mneed);
    }

    if ( minusinfty ) {
      /* this one is not needed at all
       */
      continue;
    }
    if ( gfl_conn[i].def[time-1] && gfl_conn[i].level[time-1] > mneed ) {
      /* here we already had a sufficient value at the last layer.
       */
      continue;
    }
    return FALSE;
  }

  return TRUE;
    
}



void initialize_fixpoint( State *S )

{

  static Bool first_call = TRUE;

  int i;

  if ( first_call ) {
    /* make initial space for fluent levels
     */
    extend_fluent_levels( -1 );

    /* get memory for local globals
     */
    lFa = ( int * ) calloc( gnum_ft_conn, sizeof( int ) );
    lE = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );
    lE_level0 = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );
    lE_deactivated = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );
    lE_level0_deactivated = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );
    lch_E = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );
    l0P_E = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );

    /* raquel: the maximum number of levels is the number of actions */
    level_cost = ( float * ) calloc( gnum_ef_conn, sizeof( float ) );

    /* initialize connectivity graph members for
     * relaxed planning
     */
    lnum_0P_E = 0;
    for ( i = 0; i < gnum_ef_conn; i++ ) {      
      gef_conn[i].level = INFINITY; 
      gef_conn[i].level_applicable = INFINITY; 
      gef_conn[i].acc_PCcost = INFINITY; 
      gef_conn[i].acc_cost = INFINITY; 
      gef_conn[i].in_E = FALSE;
      gef_conn[i].in_E_deactivated = FALSE;
      gef_conn[i].num_active_PCs = 0;
      gef_conn[i].ch = FALSE;
      gef_conn[i].not_necessarilly_required = FALSE;
      gef_conn[i].num_active_f_PCs = 0;
      
      if ( gef_conn[i].num_PC == 0 &&
	   gef_conn[i].num_f_PC == 0 &&
	   !gef_conn[i].illegal ) {
	l0P_E[lnum_0P_E++] = i;
      }
    }
    for ( i = 0; i < gnum_op_conn; i++ ) {      
      gop_conn[i].is_in_A = FALSE;
      gop_conn[i].is_in_H = FALSE;
      gop_conn[i].level = INFINITY;
      gop_conn[i].level_required = INFINITY;
      gop_conn[i].level_required_prop = INFINITY;
      gop_conn[i].level_achievable = INFINITY;
    }
    for ( i = 0; i < gnum_ft_conn; i++ ) {
      gft_conn[i].level = INFINITY;
      gft_conn[i].level_required = INFINITY;
      gft_conn[i].level_required_prop = INFINITY;
      gft_conn[i].level_achievable = INFINITY;
      gft_conn[i].in_F = FALSE;
    }
    for ( i = 0; i < gnum_fl_conn; i++ ) {
      gfl_conn[i].curr_assigned = FALSE;
      gfl_conn[i].level_required = INFINITY;
      gfl_conn[i].level_achievable = INFINITY;
    }
    
    OpenApp_head = NULL;

    first_call = FALSE;
  }

  lnum_E = 0;
  /* raquel: initialize applicable actions in level 0
   */
  lnum_E_level0 = 0;

  lnum_E_level0_deactivated = 0;

  lnum_E_deactivated = 0;

  lnum_ch_E = 0;

  /* raquel: initialize the number of actions in the relaxed plan
   */
  gnum_RP=0;
  
  /* raquel: initialize the admissible related h to 0
   */
  float_admissible_h = 0;
  int_admissible_h = 0;

  lnum_Fa = 0;
  for ( i = 0; i < S->num_F; i++ ) {
    if ( gft_conn[S->F[i]].in_F ) {
      continue;
    }
    new_fact( S->F[i] );
  }
  /* only the real fls are ever in there
   */
  for ( i = 0; i < gnum_real_fl_conn; i++ ) {
    if ( !S->f_D[i] ) {
      continue;
    }
    gfl_conn[i].def[0] = TRUE;
    gfl_conn[i].level[0] = S->f_V[i];
  }
  /* now set the art. values from that.
   */
  determine_artificial_fl_levels( 0 );

}



void determine_artificial_fl_levels( int time )

{

  int i, j;
  float l;

  /* for all art. fls
   */
  for ( i = gnum_real_fl_conn; i < gnum_fl_conn; i++ ) {
    l = 0;
    for ( j = 0; j < gfl_conn[i].num_lnf; j++ ) {
      if ( !gfl_conn[gfl_conn[i].lnf_F[j]].def[time] ) break;
      l += (gfl_conn[i].lnf_C[j] * gfl_conn[gfl_conn[i].lnf_F[j]].level[time]);
    }
    if ( j < gfl_conn[i].num_lnf ) {
      /* one part yet undefined.
       */
      continue;
    } else {
      gfl_conn[i].def[time] = TRUE;
      gfl_conn[i].level[time] = l;
    }
  }

}



void extend_fluent_levels( int time )

{

  static int highest_seen;

  Bool *b;
  float *f1;

  int i, j;

  if ( time == -1 ) {
    highest_seen = RELAXED_STEPS_DEFAULT;
    for ( i = 0; i < gnum_fl_conn; i++ ) {
      gfl_conn[i].def = ( Bool * ) calloc( highest_seen, sizeof( Bool ) );
      for ( j = 0; j < highest_seen; j++ ) {
	gfl_conn[i].def[j] = FALSE;
      }
      gfl_conn[i].level = ( float * ) calloc( highest_seen, sizeof( float ) );
    }
    return;
  }

  if ( time + 1 < highest_seen ) return;

  b = ( Bool * ) calloc( time + 1, sizeof( Bool ) );
  f1 = ( float * ) calloc( time + 1, sizeof( float ) );

  highest_seen = time + 10;
  for ( i = 0; i < gnum_fl_conn; i++ ) {
    for ( j = 0; j <= time; j++ ) {
      b[j] = gfl_conn[i].def[j];
      f1[j] = gfl_conn[i].level[j];
    }

    free( gfl_conn[i].def );
    free( gfl_conn[i].level );
    gfl_conn[i].def = ( Bool * ) calloc( highest_seen, sizeof( Bool ) );
    gfl_conn[i].level = ( float * ) calloc( highest_seen, sizeof( float ) );

    for ( j = 0; j <= time; j++ ) {
      gfl_conn[i].def[j] = b[j];
      gfl_conn[i].level[j] = f1[j];
    }
    for ( j = time + 1; j < highest_seen; j++ ) {
      gfl_conn[i].def[j] = FALSE;
    }
  }

  free( b );
  free( f1 );

}



void activate_ft( int index, int time )

{

  int i;

  gft_conn[index].level = time;
  gft_conn[index].level_achievable = time;

  for ( i = 0; i < gft_conn[index].num_PC; i++ ) {
    /* never activate illegal effects.
     */
    if ( gef_conn[gft_conn[index].PC[i]].illegal ) continue;

    gef_conn[gft_conn[index].PC[i]].num_active_PCs++;

    if ( !gef_conn[gft_conn[index].PC[i]].ch ) {
      gef_conn[gft_conn[index].PC[i]].ch = TRUE;
      lch_E[lnum_ch_E++] = gft_conn[index].PC[i];
    }
    if ( gef_conn[gft_conn[index].PC[i]].num_active_PCs ==
	 gef_conn[gft_conn[index].PC[i]].num_PC &&
	 gef_conn[gft_conn[index].PC[i]].num_active_f_PCs ==
	 gef_conn[gft_conn[index].PC[i]].num_f_PC ) {

      new_ef( gft_conn[index].PC[i], time );
    }
  }
}

void activate_ft_hlevel_max( int index, int time )

{
  
  int i;

  gft_conn[index].level = time;

  if (time == 0 )
    {
    gft_conn[index].level_achievable = time;
    }

  for ( i = 0; i < gft_conn[index].num_PC; i++ ) {
    /* never activate illegal effects.
     */
    if ( gef_conn[gft_conn[index].PC[i]].illegal ) continue;


    gef_conn[gft_conn[index].PC[i]].num_active_PCs++;

    /* raquel: update acc_cost to max: max is this time
     */
    /*if (LESS(gef_conn[gft_conn[index].PC[i]].acc_PCcost, level_cost[time])){*/
      gef_conn[gft_conn[index].PC[i]].acc_PCcost = level_cost[time]; 
      /*    }*/


    
    if ( !gef_conn[gft_conn[index].PC[i]].ch ) {
      gef_conn[gft_conn[index].PC[i]].ch = TRUE;
      lch_E[lnum_ch_E++] = gft_conn[index].PC[i];
    }
    if ( gef_conn[gft_conn[index].PC[i]].num_active_PCs ==
	 gef_conn[gft_conn[index].PC[i]].num_PC &&
	 gef_conn[gft_conn[index].PC[i]].num_active_f_PCs ==
	 gef_conn[gft_conn[index].PC[i]].num_f_PC ) {

      if (LESS(gef_conn[gft_conn[index].PC[i]].level_applicable,gft_conn[index].level_achievable ) || 
	  gef_conn[gft_conn[index].PC[i]].level_applicable == INFINITY )
	{
	  gef_conn[gft_conn[index].PC[i]].level_applicable = gft_conn[index].level_achievable;
	  /*
	  printf("\n %d",gef_conn[gft_conn[index].PC[i]].level_applicable );
	  print_op_name(gef_conn[gft_conn[index].PC[i]].op);
	  fflush(stdout);
	  */
	}

      new_ef_hlevel( gft_conn[index].PC[i], time );



    }
  }
}


void activate_ft_hlevel_add( int index, int time )

{
  
  int i;

  gft_conn[index].level = time;

  if (time == 0 )
    {
    gft_conn[index].level_achievable = time;
    }


  for ( i = 0; i < gft_conn[index].num_PC; i++ ) {
    /* never activate illegal effects.
     */
    if ( gef_conn[gft_conn[index].PC[i]].illegal ) continue;

    gef_conn[gft_conn[index].PC[i]].num_active_PCs++;

    /* raquel: update acc_cost to sum of precondition costs
     */

    if (gef_conn[gft_conn[index].PC[i]].acc_PCcost == INFINITY)
      gef_conn[gft_conn[index].PC[i]].acc_PCcost =  level_cost[time]; 
     else
      gef_conn[gft_conn[index].PC[i]].acc_PCcost =
	gef_conn[gft_conn[index].PC[i]].acc_PCcost +  level_cost[time];
    

    
    if ( !gef_conn[gft_conn[index].PC[i]].ch ) {
      gef_conn[gft_conn[index].PC[i]].ch = TRUE;
      lch_E[lnum_ch_E++] = gft_conn[index].PC[i];
    }
    if ( gef_conn[gft_conn[index].PC[i]].num_active_PCs ==
	 gef_conn[gft_conn[index].PC[i]].num_PC &&
	 gef_conn[gft_conn[index].PC[i]].num_active_f_PCs ==
	 gef_conn[gft_conn[index].PC[i]].num_f_PC ) {
      
      if (LESS(gef_conn[gft_conn[index].PC[i]].level_applicable, gft_conn[index].level_achievable ) || 
	  gef_conn[gft_conn[index].PC[i]].level_applicable==INFINITY )
	gef_conn[gft_conn[index].PC[i]].level_applicable = gft_conn[index].level_achievable;


      new_ef_hlevel( gft_conn[index].PC[i], time );

    }
  }
}





void activate_fl( int index, int time )

{

  int i, ef;

  if ( !gfl_conn[index].def[time] ) return;

  for ( i = 0; i < gfl_conn[index].num_PC; i++ ) {
    ef = gfl_conn[index].PC[i];
    if ( gef_conn[ef].illegal ) continue;


    if ( gef_conn[ef].f_PC_direct_comp[index] == IGUAL ) {
      printf("\n\nprec of addressed ef does not care about fl!\n\n");
      exit( 1 );
    }
    if ( !number_comparison_holds( gef_conn[ef].f_PC_direct_comp[index],
				   gfl_conn[index].level[time],
				   gef_conn[ef].f_PC_direct_c[index] ) ) {
      /* the level is not yet high enough for this one
       */
      continue;
    }
    if ( time > 0 &&
	 gfl_conn[index].def[time-1] &&
	 number_comparison_holds( gef_conn[ef].f_PC_direct_comp[index],
				  gfl_conn[index].level[time-1],
				  gef_conn[ef].f_PC_direct_c[index] ) ) {
      /* last time this was already in: that one's old!
       * do not count it!
       */
      continue;
    }
    gef_conn[ef].num_active_f_PCs++;
    if ( !gef_conn[ef].ch ) {
      gef_conn[ef].ch = TRUE;
      lch_E[lnum_ch_E++] = ef;
    }
    if ( gef_conn[ef].num_active_PCs == gef_conn[ef].num_PC &&
	 gef_conn[ef].num_active_f_PCs == gef_conn[ef].num_f_PC ) {
      new_ef( ef, time );

    }
  }

}

void activate_fl_hlevel_max( int index, int time )

{

  int i, ef;

  if ( !gfl_conn[index].def[time] ) return;

  for ( i = 0; i < gfl_conn[index].num_PC; i++ ) {
    ef = gfl_conn[index].PC[i];
    if ( gef_conn[ef].illegal ) continue;


    if ( gef_conn[ef].f_PC_direct_comp[index] == IGUAL ) {
      printf("\n\nprec of addressed ef does not care about fl!\n\n");
      exit( 1 );
    }
    if ( !number_comparison_holds( gef_conn[ef].f_PC_direct_comp[index],
				   gfl_conn[index].level[time],
				   gef_conn[ef].f_PC_direct_c[index] ) ) {
      /* the level is not yet high enough for this one
       */
      continue;
    }
    if ( time > 0 &&
	 gfl_conn[index].def[time-1] &&
	 number_comparison_holds( gef_conn[ef].f_PC_direct_comp[index],
				  gfl_conn[index].level[time-1],
				  gef_conn[ef].f_PC_direct_c[index] ) ) {
      /* last time this was already in: that one's old!
       * do not count it!
       */
      continue;
    }
    gef_conn[ef].num_active_f_PCs++;

    /* raquel: update acc_cost to max
     */
    gef_conn[ef].acc_PCcost = level_cost[time]; 

    

    if ( !gef_conn[ef].ch ) {
      gef_conn[ef].ch = TRUE;
      lch_E[lnum_ch_E++] = ef;
    }
    if ( gef_conn[ef].num_active_PCs == gef_conn[ef].num_PC &&
	 gef_conn[ef].num_active_f_PCs == gef_conn[ef].num_f_PC ) {
      new_ef_hlevel( ef, time );

    }
  }

}

void activate_fl_hlevel_add( int index, int time )

{

  int i, ef;

  if ( !gfl_conn[index].def[time] ) return;

  for ( i = 0; i < gfl_conn[index].num_PC; i++ ) {
    ef = gfl_conn[index].PC[i];
    if ( gef_conn[ef].illegal ) continue;


    if ( gef_conn[ef].f_PC_direct_comp[index] == IGUAL ) {
      printf("\n\nprec of addressed ef does not care about fl!\n\n");
      exit( 1 );
    }
    if ( !number_comparison_holds( gef_conn[ef].f_PC_direct_comp[index],
				   gfl_conn[index].level[time],
				   gef_conn[ef].f_PC_direct_c[index] ) ) {
      /* the level is not yet high enough for this one
       */
      continue;
    }
    if ( time > 0 &&
	 gfl_conn[index].def[time-1] &&
	 number_comparison_holds( gef_conn[ef].f_PC_direct_comp[index],
				  gfl_conn[index].level[time-1],
				  gef_conn[ef].f_PC_direct_c[index] ) ) {
      /* last time this was already in: that one's old!
       * do not count it!
       */
      continue;
    }
    gef_conn[ef].num_active_f_PCs++;

    /* raquel: update acc_cost to sum
     */
    
    if (gef_conn[ef].acc_PCcost == INFINITY)
      gef_conn[ef].acc_PCcost =  level_cost[time]; 
    else
      gef_conn[ef].acc_PCcost = gef_conn[ef].acc_PCcost +
      level_cost[time]; 
    

    if ( !gef_conn[ef].ch ) {
      gef_conn[ef].ch = TRUE;
      lch_E[lnum_ch_E++] = ef;
    }
    if ( gef_conn[ef].num_active_PCs == gef_conn[ef].num_PC &&
	 gef_conn[ef].num_active_f_PCs == gef_conn[ef].num_f_PC ) {
      new_ef_hlevel( ef, time );

    }
  }

}



void activate_ef( int index, int time )

{

  int i, fl;
  float val;

  if ( gef_conn[index].removed ) {
    printf("\n\nactivating removed effect!!\n\n");
    exit( 1 );
  }
  if ( gef_conn[index].illegal ) {
    printf("\n\nactivating illegal effect!!\n\n");
    exit( 1 );
  }

  gef_conn[index].level = time;

  for ( i = 0; i < gef_conn[index].num_A; i++ ) {
    if ( gft_conn[gef_conn[index].A[i]].in_F ) {
      continue;
    }
    new_fact( gef_conn[index].A[i] );
  }

  for ( i = 0; i < gef_conn[index].num_IN; i++ ) {
    fl = gef_conn[index].IN_fl[i];
    if ( !gfl_conn[fl].def[time] ) {
      /* in principle, we could skip this action here as
       * it affects a fluent that is yet undefined.
       *
       * ...seems difficult to integrate into implementation,
       * does not matter much probably (?): so we relax
       * the task further in the sense that we allow this action
       * here, only it has of course no effect on the fluent.
       */
      continue;
    }
    /* value is either the constant, or the (artificial lnf?) fl_ level
     * at <time> plus the constant.
     */
    if ( gef_conn[index].IN_fl_[i] == -1 ) {
      val = gef_conn[index].IN_c[i];
    } else {
      if ( !gfl_conn[gef_conn[index].IN_fl_[i]].def[time] ) {
	/* this one does not help us here.
	 */
	continue;
      }
      val = gfl_conn[gef_conn[index].IN_fl_[i]].level[time] + gef_conn[index].IN_c[i];
    }
    /* we only consider the effect if it helps us.
     */
    if ( val > 0 ) {
      gfl_conn[fl].level[time+1] += val;
    }
  }

  /* the assigners are remembered in a parallel sort of way...
   */
  for ( i = 0; i < gef_conn[index].num_AS; i++ ) {
    fl = gef_conn[index].AS_fl[i];
    if ( gef_conn[index].AS_fl_[i] == -1 ) {
      val = gef_conn[index].AS_c[i];
    } else {
      if ( !gfl_conn[gef_conn[index].AS_fl_[i]].def[time] ) {
	/* this one does not help us here.
	 */
	continue;
      }
      val = gfl_conn[gef_conn[index].AS_fl_[i]].level[time] + gef_conn[index].AS_c[i];
    }
    if ( gfl_conn[fl].curr_assigned ) {
      if ( gfl_conn[fl].curr_max_assigned < val ) {
	gfl_conn[fl].curr_max_assigned = val;
      }
    } else {
      gfl_conn[fl].curr_assigned = TRUE;
      gfl_conn[fl].curr_max_assigned = val;
    }
  }

}



/* this one is used to apply effects already there
 * at later time steps - necessary because their
 * numeric effects might have changed.
 */
void apply_ef( int index, int time )

{

  int i, fl;
  float val;

  if ( gef_conn[index].removed ) {
    printf("\n\napplying removed effect!!\n\n");
    exit( 1 );
  }
  if ( gef_conn[index].illegal ) {
    return;
  }

  /* only numerical effects matter.
   */
  for ( i = 0; i < gef_conn[index].num_IN; i++ ) {
    fl = gef_conn[index].IN_fl[i];
    if ( !gfl_conn[fl].def[time] ) {
      continue;
    }
    if ( gef_conn[index].IN_fl_[i] == -1 ) {
      val = gef_conn[index].IN_c[i];
    } else {
      if ( !gfl_conn[gef_conn[index].IN_fl_[i]].def[time] ) {
	/* no effect here.
	 */
	continue;
      }
      val = gfl_conn[gef_conn[index].IN_fl_[i]].level[time] + gef_conn[index].IN_c[i];
    }
    if ( val > 0 ) {
      gfl_conn[fl].level[time+1] += val;
    }
  }

  for ( i = 0; i < gef_conn[index].num_AS; i++ ) {
    fl = gef_conn[index].AS_fl[i];
    if ( gef_conn[index].AS_fl_[i] == -1 ) {
      val = gef_conn[index].AS_c[i];
    } else {
      if ( !gfl_conn[gef_conn[index].AS_fl_[i]].def[time] ) {
	/* no effect here.
	 */
	continue;
      }
      val = gfl_conn[gef_conn[index].AS_fl_[i]].level[time] + gef_conn[index].AS_c[i];
    }
    if ( gfl_conn[fl].curr_assigned ) {
      if ( gfl_conn[fl].curr_max_assigned < val ) {
	gfl_conn[fl].curr_max_assigned = val;
      }
    } else {
      gfl_conn[fl].curr_assigned = TRUE;
      gfl_conn[fl].curr_max_assigned = val;
    }
  }

}



void new_fact( int index )

{

  lFa[lnum_Fa++] = index;
  gft_conn[index].in_F = TRUE;

}



void new_ef( int index, int time)

{

  if (gop_conn[gef_conn[index].op].RP_deactivated) 
      {
	if (!gef_conn[index].in_E_deactivated){
	  lE_deactivated[lnum_E_deactivated++]= index;
	  gef_conn[index].in_E_deactivated = TRUE;
	}
	return;
      }

  lE[lnum_E++] = index;
  gef_conn[index].in_E = TRUE;
  gef_conn[index].level_applicable = time;

 


}

void new_ef_hlevel( int index, int time)

{
  OpenApp *i, *new;
  float acc_cost;
  int j;


  if (gop_conn[gef_conn[index].op].RP_deactivated) 
      {
	if (!gef_conn[index].in_E_deactivated){
	  lE_deactivated[lnum_E_deactivated++]= index;
	  gef_conn[index].in_E_deactivated = TRUE;
	}
	return;
      }


  if (gef_conn[index].acc_PCcost == INFINITY)
    {
      /* preconditions are static facts */
      gef_conn[index].acc_PCcost = 0;
    }

  if ( gcmd_line.optimize && goptimization_established ) {
    acc_cost = gef_conn[index].acc_PCcost +  gef_conn[index].cost + (float) gtt;
  }
  else {
    acc_cost = gef_conn[index].acc_PCcost + (float) gtt;
  }

  /* raquel: insert the effect in the ordered OpenApp list */
  /* inserts with LESS instead LESS_EQ to maintain the mff order in lE*/
  for(i = OpenApp_head; i->next; i=i->next){

    if (LESS(acc_cost,
	     gef_conn[i->next->ef].acc_cost))
      break;
  }
  /*
  printf("\n time %d ", time);
  print_op_name(gef_conn[index].op );
  */
  new = new_OpenApp();
  new->ef = index;
  gef_conn[index].acc_cost = acc_cost;
  new->next = i->next;
  i->next = new;  


  gef_conn[index].in_E = TRUE;
  //  gef_conn[index].level_applicable = time;


  for ( j = 0; j < gef_conn[index].num_A; j++ ) {
    /*
    if ( gft_conn[gef_conn[index].A[j]].in_F ) {
      continue;
    } 
    */
    //    if (gft_conn[gef_conn[index].A[j]].level_achievable == INFINITY)
    if ( LESS( gef_conn[index].level_applicable + 1,gft_conn[gef_conn[index].A[j]].level_achievable))
      gft_conn[gef_conn[index].A[j]].level_achievable = gef_conn[index].level_applicable + 1;
    
  }


}

void reset_fixpoint( int max )

{

  int i, j;
  OpenApp *first, *aux;

  for ( i = 0; i < lnum_Fa; i++ ) {
    gft_conn[lFa[i]].level = INFINITY;
    gft_conn[lFa[i]].level_required = INFINITY;
    gft_conn[lFa[i]].level_required_prop = INFINITY;
    gft_conn[lFa[i]].level_achievable = INFINITY;
    gft_conn[lFa[i]].in_F = FALSE;
    for ( j = 0; j < gft_conn[lFa[i]].num_PC; j++ ) {
      gef_conn[gft_conn[lFa[i]].PC[j]].acc_PCcost= INFINITY;
    }
  }

  for ( i = 0; i < lnum_E; i++ ) {
    gef_conn[lE[i]].level = INFINITY;
    gef_conn[lE[i]].in_E = FALSE;
    gef_conn[lE[i]].level_applicable = INFINITY; 
    gef_conn[lE[i]].acc_PCcost = INFINITY; 
    gef_conn[lE[i]].acc_cost = INFINITY; 
    gef_conn[lE[i]].not_necessarilly_required = FALSE;

  }

  for ( i = 0; i < lnum_E_level0; i++ ) {
    gef_conn[lE_level0[i]].level_applicable = INFINITY; 
  }

  for ( i = 0; i < lnum_E_deactivated; i++ ) {
    gef_conn[lE_deactivated[i]].in_E_deactivated = FALSE; 
  }

  for ( i = 0; i < lnum_ch_E; i++ ) {
    gef_conn[lch_E[i]].num_active_PCs = 0;
    gef_conn[lch_E[i]].num_active_f_PCs = 0;
    gef_conn[lch_E[i]].ch = FALSE;
  }

  for ( i = 0; i < gnum_fl_conn; i++ ) {
    for ( j = 0; j <= max; j++ ) {
      gfl_conn[i].def[j] = FALSE;
      gfl_conn[i].level_required = INFINITY;
      gfl_conn[i].level_achievable = INFINITY;
    }
    gfl_conn[i].curr_assigned = FALSE;
  }

  /* raquel: reset pending effs in OpenApp */
  //  print_OpenApp();
  if (OpenApp_head) {
    first = OpenApp_head->next;

    while (first){
      gef_conn[first->ef].level = INFINITY;
      gef_conn[first->ef].in_E = FALSE;
      gef_conn[first->ef].level_applicable = INFINITY; 
      gef_conn[first->ef].acc_PCcost = INFINITY; 
      gef_conn[first->ef].acc_cost = INFINITY; 
      aux = first;
      first = first->next;
      free(aux);
    }
    free(OpenApp_head);
    OpenApp_head = NULL;
  }

  /*  
  for (i =0; i < gnum_ef_conn; i++)
    {
      printf("\n");
      print_op_name(gef_conn[i].op);
      //  printf("%f", gef_conn[i].acc_cost);
      //printf(" %f", gef_conn[i].acc_PCcost);
      printf(" %d", gef_conn[i].level_applicable);

    }
  */
  /*
  for (i =0; i < max; i++)
    {
      printf("\n");
      //      level_cost[i]=0;
      printf("%f", level_cost[i]);
    }
  */
}




Bool all_goals_activated( int time ) 

{

  int i;

  for ( i = 0; i < gnum_flogic_goal; i++ ) {
    if ( !gft_conn[gflogic_goal[i]].in_F ) {
      return FALSE;
    }
  }

  for ( i = 0; i < gnum_fnumeric_goal; i++ ) {
    if ( !gfl_conn[gfnumeric_goal_fl[i]].def[time] ) {
      return FALSE;
    }
    if ( !number_comparison_holds( gfnumeric_goal_comp[i],
				   gfl_conn[gfnumeric_goal_fl[i]].level[time],
				   gfnumeric_goal_c[i] ) ) {
      return FALSE;
    }
  }

  for ( i = 0; i < gnum_flogic_goal; i++ ) {
    if ( gft_conn[gflogic_goal[i]].level == INFINITY ) {
      gft_conn[gflogic_goal[i]].level = time;
      if (gft_conn[gflogic_goal[i]].level_achievable == INFINITY)
	gft_conn[gflogic_goal[i]].level_achievable = time;

    }
  }

  return TRUE;

}



void print_fixpoint_result( void )

{

  int time, i;
  Bool hit, hit_F, hit_E, hit_FL;

  time = 0;
  while ( 1 ) {
    hit = FALSE;
    hit_F = FALSE;
    hit_E = FALSE;
    hit_FL = FALSE;
    for ( i = 0; i < gnum_ft_conn; i++ ) {
      if ( gft_conn[i].level == time ) {
	hit = TRUE;
	hit_F = TRUE;
	break;
      }
    }
    for ( i = 0; i < gnum_ef_conn; i++ ) {
      if ( gef_conn[i].level == time ) {
	hit = TRUE;
	hit_E = TRUE;
	break;
      }
    }
    for ( i = 0; i < gnum_fl_conn; i++ ) {
      if ( gfl_conn[i].def[time] ) {
	hit = TRUE;
	hit_FL = TRUE;
	break;
      }
    }
    if ( !hit ) {
      break;
    }
 
    printf("\n\nLEVEL %d (cost %f):", time, level_cost[time]);
    if ( hit_F ) {
      printf("\n\nFACTS:");
      for ( i = 0; i < gnum_ft_conn; i++ ) {
	if ( gft_conn[i].level == time ) {
	  printf("\n");
	  print_ft_name( i );
	}
      }
    }
    if ( hit_FL ) {
      printf("\n\nFLUENTS:");
      for ( i = 0; i < gnum_fl_conn; i++ ) {
	if ( gfl_conn[i].def[time] ) {
	  printf("\n");
	  print_fl_name( i );
	  printf(": %f", gfl_conn[i].level[time]);
	} else {
	  printf("\n");
	  print_fl_name( i );
	  printf(": UNDEF");
	}
      }
    }
    if ( hit_E ) {
      printf("\n\nEFS:");
      for ( i = 0; i < gnum_ef_conn; i++ ) {
	if ( gef_conn[i].level == time ) {
	  printf("\neffect %d to ", i);
	  print_op_name( gef_conn[i].op );
	}
      }
    }

    time++;
  }
  fflush( stdout );

}
 


/* naive implementation of the mneed computation. speedups
 * should be possible by utilizing effective data access, and
 * dynamic programming according to topological sorting wrp to
 * := dependencies.
 * 
 * wonder whether that process is time relevant at all?
 *
 * function is recursive, and will terminate because the :=
 * dependencies are cycle-free.
 */
void get_mneed( int fl, Bool *minusinfty, float *val )

{

  int ef, pc, i, fl_, c, in, g, as;
  float val_, mneed_;
  Bool minusinfty_;

  /* this counts the number of hits; 0 --> mneed is -infty
   */
  c = 0;
  /* check through the actions, i.e. effects. I suppose this could be made
   * *a lot* faster by checking through the PC information of the fl;
   * additionally, we'd need fast access to the art. fluents that fl 
   * participates in.
   */
  for ( ef = 0; ef < gnum_ef_conn; ef++ ) {
    /* the preconds must be supported above their required value.
     */
    for ( pc = 0;  pc < gef_conn[ef].num_f_PC; pc++ ) {
      /* constraint here is gef_conn[ef].f_PC_fl[pc] >= [>] gef_conn[ef].f_PC_c[pc]
       * where lh side can be lnf expression.
       */
      fl_ = gef_conn[ef].f_PC_fl[pc];
      if ( fl_ < 0 ) continue;
      if ( fl_ == fl ) {
	c++;
	val_ = gef_conn[ef].f_PC_c[pc];
	if ( c == 1 || val_ > *val ) {
	  *val = val_;
	}
	continue;
      }
      if ( !gfl_conn[fl_].artificial ) continue;
      for ( i = 0; i < gfl_conn[fl_].num_lnf; i++ ) {
	if ( gfl_conn[fl_].lnf_F[i] == fl ) {
	  /* might be that the expression can't be supported --
	   * if one of the fluents is undefined.
	   */
	  if ( supv( &val_, fl, fl_, gef_conn[ef].f_PC_c[pc] ) ) {
	    c++;
	    if ( c == 1 || val_ > *val ) {
	      *val = val_;
	    }
	  }
	  break;
	}
      }
    }

    /* the += effs must be supported above 0.
     */
    for ( in = 0; in < gef_conn[ef].num_IN; in++ ) {
      /* += eff here is gef_conn[ef].IN_fl[in] += gef_conn[ef].IN_fl_[in] + 
       *                                          gef_conn[ef].IN_c[in]
       * where gef_conn[ef].IN_fl_[in] can be lnf expression.
       */
      /* relevance...
       */
      if ( !gfl_conn[gef_conn[ef].IN_fl[in]].relevant ) {
	continue;
      }
      fl_ = gef_conn[ef].IN_fl_[in];
      if ( fl_ < 0 ) continue;
      if ( fl_ == fl ) {
	c++;
	val_ = (-1) * gef_conn[ef].IN_c[in];
	if ( c == 1 || val_ > *val ) {
	  *val = val_;
	}
	continue;
      }
      if ( !gfl_conn[fl_].artificial ) continue;
      for ( i = 0; i < gfl_conn[fl_].num_lnf; i++ ) {
	if ( gfl_conn[fl_].lnf_F[i] == fl ) {
	  if ( supv( &val_, fl, fl_, (-1) * gef_conn[ef].IN_c[in] ) ) {
	    c++;
	    if ( c == 1 || val_ > *val ) {
	      *val = val_;
	    }
	  }
	  break;
	}
      }
    }

    /* the := effs must be supported above the mneed value of the affected
     * varuable..
     */
    for ( as = 0; as < gef_conn[ef].num_AS; as++ ) {
      /* := eff here is gef_conn[ef].AS_fl[as] := gef_conn[ef].AS_fl_[as] + 
       *                                          gef_conn[ef].AS_c[as]
       * where gef_conn[ef].AS_fl_[as] can be lnf expression.
       */
      /* relevance...
       */
      if ( !gfl_conn[gef_conn[ef].AS_fl[as]].relevant ) {
	continue;
      }
      /* after this, -infty handling is actually superfluous... or so I'd suppose...
       */
      fl_ = gef_conn[ef].AS_fl_[as];
      if ( fl_ < 0 ) continue;
      if ( fl_ == fl ) {
	get_mneed( gef_conn[ef].AS_fl[as], &minusinfty_, &mneed_ );
	if ( !minusinfty_ ) {
	  c++;
	  val_ = mneed_ - gef_conn[ef].AS_c[as];
	  if ( c == 1 || val_ > *val ) {
	    *val = val_;
	  }
	}
	continue;
      }
      if ( !gfl_conn[fl_].artificial ) continue;
      for ( i = 0; i < gfl_conn[fl_].num_lnf; i++ ) {
	if ( gfl_conn[fl_].lnf_F[i] == fl ) {
	  get_mneed( gef_conn[ef].AS_fl[as], &minusinfty_, &mneed_ );
	  if ( !minusinfty_ ) {
	    if ( supv( &val_, fl, fl_, mneed_ - gef_conn[ef].AS_c[as] ) ) {
	      c++;
	      if ( c == 1 || val_ > *val ) {
		*val = val_;
	      }
	    }
	    break;
	  }
	}
      }
    }

  }/* end of ef loop */

  /* check through the numerical goals.
   */
  for ( g = 0; g < gnum_fnumeric_goal; g++ ) {
    /* constraint here is gfnumeric_goal_fl[g] >= [>] gfnumeric_goal_c[g]
     * where lh side can be lnf expression.
     */
    fl_ = gfnumeric_goal_fl[g];
    if ( fl_ < 0 ) continue;
    if ( fl_ == fl ) {
      c++;
      val_ = gfnumeric_goal_c[g];
      if ( c == 1 || val_ > *val ) {
	*val = val_;
      }
    }
    if ( !gfl_conn[fl_].artificial ) continue;
    for ( i = 0; i < gfl_conn[fl_].num_lnf; i++ ) {
      if ( gfl_conn[fl_].lnf_F[i] == fl ) {
	if ( supv( &val_, fl, fl_, gfnumeric_goal_c[g] ) ) {
	  c++;
	  if ( c == 1 || val_ > *val ) {
	    *val = val_;
	  }
	}
	break;
      }
    }
  }

  *minusinfty = ( c == 0 ) ? TRUE : FALSE;

}   



Bool supv( float *val, int fl, int expr, float c_ )

{

  int i, pos = 0;

  *val = c_;

  for ( i = 0; i < gfl_conn[expr].num_lnf; i++ ) {
    if ( gfl_conn[expr].lnf_F[i] == fl ) {
      pos = i;
      continue;
    }
    if ( !gmneed_start_D[gfl_conn[expr].lnf_F[i]] ) {
      /* this expression contains an undef fluent -->
       * no matter how much we increase fl, it won't help
       * at all.
       */
      return FALSE;
    }
    *val -= gfl_conn[expr].lnf_C[i] * 
      gmneed_start_V[gfl_conn[expr].lnf_F[i]];
  }

  /* this here is > 0 (hopefully -- yes, checked that)
   */
  *val /= gfl_conn[expr].lnf_C[pos];

  return TRUE;

}




















/**************************************
 * FIRST RELAXED PLAN (1P) EXTRACTION *
 **************************************/



























int extract_1P( int max )

{

  static Bool first_call = TRUE;
  int i, j,  max_goal_level, time, level, ft, fl;
  float val;
  Comparator comp;

  /* raquel: for the max and max-add heuristics */
  if (gcmd_line.heuristic == H_MAX || gcmd_line.heuristic == H_MAX_ADD )
    {
      lh = 0;
      /* logic goals */
      for ( i = 0; i < gnum_flogic_goal; i++ ) {
	ft = gflogic_goal[i];
	if (gcmd_line.display_info == 127)
	  {
	    printf("\n          ");
	    print_ft_name( ft );
	    printf("\n            (level %3d, level-cost %10f)", gft_conn[ft].level,  level_cost[gft_conn[ft].level]);

	    
	  }
	if ( gft_conn[ft].level > lh) {
	  lh = gft_conn[ft].level;
	}
      }

      /* numeric goals */
      for ( i = 0; i < gnum_fnumeric_goal; i++ ) {
	fl = gfnumeric_goal_fl[i];
	val = gfnumeric_goal_c[i];
	comp = gfnumeric_goal_comp[i];
	for ( j = 0; j <= max; j++ ) {
	  if ( !gfl_conn[fl].def[j] ) continue;
	  if ( number_comparison_holds( comp, gfl_conn[fl].level[j], val ) ) break;
	}
	/* j is the first level where the numeric goal is true ¿?*/
	if ( j > lh ) {
	  lh = j;
	}
      }
      if ( gcmd_line.optimize && goptimization_established ) {
	gcost = level_cost[lh] - lh * gtt;
      }
      return lh;
    }

  /* raquel for the add and sum-max heuristic  */
  if (gcmd_line.heuristic == H_ADD || gcmd_line.heuristic == H_SUM_MAX ) {
    lh = 0;
    gcost = 0;
    /* logic goals */
    for ( i = 0; i < gnum_flogic_goal; i++ ) {
      ft = gflogic_goal[i];      
      level =  gft_conn[ft].level;

	if (gcmd_line.display_info == 127)
	  {
	    printf("\n          ");
	    print_ft_name( ft );
	    printf("\n            (level %3d, level-cost %10f)", level, level_cost[level] );
	    
	  }
      lh = lh + level;

      if ( gcmd_line.optimize && goptimization_established ) {
	gcost = gcost + level_cost[level];
      }
    }
    
    /* numeric goals */
    for ( i = 0; i < gnum_fnumeric_goal; i++ ) {
      fl = gfnumeric_goal_fl[i];
      val = gfnumeric_goal_c[i];
      comp = gfnumeric_goal_comp[i];
      for ( j = 0; j <= max; j++ ) {
	if ( !gfl_conn[fl].def[j] ) continue;
	if ( number_comparison_holds( comp, gfl_conn[fl].level[j], val ) ) break;
      }
      /* j is the first level where the numeric goal is true ¿?*/
      lh = lh + j;
      gcost = gcost + level_cost[j];
    }    
      if ( gcmd_line.optimize && goptimization_established ) {
	gcost = gcost - lh * gtt;
      }

     return lh;
  }



  if ( first_call ) {
    for ( i = 0; i < gnum_ft_conn; i++ ) {
      gft_conn[i].is_true = INFINITY;
      gft_conn[i].is_goal = FALSE;
      gft_conn[i].ch = FALSE;
    }
    for ( i = 0; i < gnum_op_conn; i++ ) {
      gop_conn[i].is_used = INFINITY;
      gop_conn[i].level_required = INFINITY;
      gop_conn[i].level_required_prop = INFINITY;
      gop_conn[i].level_achievable = INFINITY;
    }
    for ( i = 0; i < gnum_ef_conn; i++ ) {
      gef_conn[i].in_plan = INFINITY;
    }
    lch_F = ( int * ) calloc( gnum_ft_conn, sizeof( int ) );
    lnum_ch_F = 0;
    lused_O = ( int * ) calloc( gnum_op_conn, sizeof( int ) );
    lnum_used_O = 0;
    lin_plan_E = ( int * ) calloc( gnum_ef_conn, sizeof( int ) );
    lnum_in_plan_E = 0;

    one_step_lgoals = ( int * ) calloc( gnum_ft_conn, sizeof( int ) );
    lnum_one_step_lgoals = 0;



    first_call = FALSE;
  }

  lnum_one_step_lgoals = 0;


  reset_search_info();

  max_goal_level = initialize_goals( max );

  if ( gcmd_line.optimize && goptimization_established ) {
    /* we are optimizing cost;
     * initialize global cost of the relaxed plan.
     */
    gcost = 0;
  }

  lh = 0;
  sum_difficulty = 0;
  for ( time = max_goal_level; time > 0; time-- ) {
    achieve_goals( time );
  }

  /* raquel */
  if (gcmd_line.heuristic == H_LEVEL_MAX)
    {
      float_admissible_h = level_cost[max_goal_level];
      int_admissible_h = max_goal_level;
    }

  /* raquel for the add  and sum-max heuristics with helpful actions*/
  if (gcmd_line.heuristic == H_ADD_HA  || gcmd_line.heuristic == H_SUM_MAX_HA ) {
    lh = 0;
    gcost = 0;
    /* logic goals */
    for ( i = 0; i < gnum_flogic_goal; i++ ) {
      ft = gflogic_goal[i];      
      level =  gft_conn[ft].level;

	if (gcmd_line.display_info == 127)
	  {
	    printf("\n          ");
	    print_ft_name( ft );
	    printf("\n            (level %3d, level-cost %10f)", level, level_cost[level] );
	    
	  }
      lh = lh + level;

      if ( gcmd_line.optimize && goptimization_established ) {
	gcost = gcost + level_cost[level];
      }
    }
    
    /* numeric goals */
    for ( i = 0; i < gnum_fnumeric_goal; i++ ) {
      fl = gfnumeric_goal_fl[i];
      val = gfnumeric_goal_c[i];
      comp = gfnumeric_goal_comp[i];
      for ( j = 0; j <= max; j++ ) {
	if ( !gfl_conn[fl].def[j] ) continue;
	if ( number_comparison_holds( comp, gfl_conn[fl].level[j], val ) ) break;
      }
      /* j is the first level where the numeric goal is true ¿?*/
      lh = lh + j;
      gcost = gcost + level_cost[j];
    }    
      if ( gcmd_line.optimize && goptimization_established ) {
	gcost = gcost - lh * gtt;
      }

     return lh;
  }
  /* end add heuristic with helpful actions*/
  /* raquel: for the max and max-add heuristics */
  if (gcmd_line.heuristic == H_MAX_HA || gcmd_line.heuristic == H_MAX_ADD_HA )
    {
      lh = 0;
      /* logic goals */
      for ( i = 0; i < gnum_flogic_goal; i++ ) {
	ft = gflogic_goal[i];
	if (gcmd_line.display_info == 127)
	  {
	    printf("\n          ");
	    print_ft_name( ft );
	    printf("\n            (level %3d, level-cost %10f)", gft_conn[ft].level,  level_cost[gft_conn[ft].level]);

	    
	  }
	if ( gft_conn[ft].level > lh) {
	  lh = gft_conn[ft].level;
	}
      }

      /* numeric goals */
      for ( i = 0; i < gnum_fnumeric_goal; i++ ) {
	fl = gfnumeric_goal_fl[i];
	val = gfnumeric_goal_c[i];
	comp = gfnumeric_goal_comp[i];
	for ( j = 0; j <= max; j++ ) {
	  if ( !gfl_conn[fl].def[j] ) continue;
	  if ( number_comparison_holds( comp, gfl_conn[fl].level[j], val ) ) break;
	}
	/* j is the first level where the numeric goal is true ¿?*/
	if ( j > lh ) {
	  lh = j;
	}
      }
      if ( gcmd_line.optimize && goptimization_established ) {
	gcost = level_cost[lh] - lh * gtt;
      }
      return lh;
    }


  if (gcmd_line.hdiff == 1)
    {
      if (sum_difficulty > 0)
	gcost = 1.0 - (1.0/(float) sum_difficulty);
      else
	gcost = 0;
    }
  else if (gcmd_line.hdiff == 2)
    {
      if (sum_difficulty > 0)
	gcost = (1.0/(float) sum_difficulty);
      else
	gcost = 0;
      
    }
    

  return lh;

}



int initialize_goals( int max )

{

  static Bool first_call = TRUE;
  static int highest_seen;

  int i, j, max_goal_level, ft, fl;
  Comparator comp;
  float val;

  if ( first_call ) {
    lgoals_at = ( int ** ) calloc( RELAXED_STEPS_DEFAULT, sizeof( int * ) );
    lnum_goals_at = ( int * ) calloc( RELAXED_STEPS_DEFAULT, sizeof( int ) );
    lf_goals_c_at = ( float ** ) calloc( RELAXED_STEPS_DEFAULT, sizeof( float * ) );
    lf_goals_comp_at = ( Comparator ** ) calloc( RELAXED_STEPS_DEFAULT, sizeof( Comparator * ) );
    for ( i = 0; i < RELAXED_STEPS_DEFAULT; i++ ) {
      lgoals_at[i] = ( int * ) calloc( gnum_ft_conn, sizeof( int ) );
      lf_goals_c_at[i] = ( float * ) calloc( gnum_fl_conn, sizeof( float ) );
      lf_goals_comp_at[i] = ( Comparator * ) calloc( gnum_fl_conn, sizeof( Comparator ) );
    }
    highest_seen = RELAXED_STEPS_DEFAULT;

    lHcomp = ( Comparator * ) calloc( gnum_fl_conn, sizeof( Comparator ) );
    lHc = ( float * ) calloc( gnum_fl_conn, sizeof( float ) );
    first_call = FALSE;
  }

  if ( max + 1 > highest_seen ) {
    for ( i = 0; i < highest_seen; i++ ) {
      free( lgoals_at[i] );
      free( lf_goals_c_at[i] );
      free( lf_goals_comp_at[i] );
    }
    free( lgoals_at );
    free( lnum_goals_at );
    free( lf_goals_c_at );
    free( lf_goals_comp_at );
    highest_seen = max + 10;
    lgoals_at = ( int ** ) calloc( highest_seen, sizeof( int * ) );
    lnum_goals_at = ( int * ) calloc( highest_seen, sizeof( int ) );
    lf_goals_c_at = ( float ** ) calloc( highest_seen, sizeof( float * ) );
    lf_goals_comp_at = ( Comparator ** ) calloc( highest_seen, sizeof( Comparator * ) );
    for ( i = 0; i < highest_seen; i++ ) {
      lgoals_at[i] = ( int * ) calloc( gnum_ft_conn, sizeof( int ) );
      lf_goals_c_at[i] = ( float * ) calloc( gnum_fl_conn, sizeof( float ) );
      lf_goals_comp_at[i] = ( Comparator * ) calloc( gnum_fl_conn, sizeof( Comparator ) );
    }
  }

  for ( i = 0; i < max + 1; i++ ) {
    lnum_goals_at[i] = 0;
    for ( j = 0; j < gnum_fl_conn; j++ ) {
      lf_goals_comp_at[i][j] = IGUAL;
      /* probably not necessary; igual...
       */
      lHcomp[j] = IGUAL;
    }
  }

  max_goal_level = 0;
  for ( i = 0; i < gnum_flogic_goal; i++ ) {
    ft = gflogic_goal[i];
    /* level can't be INFINITY as otherwise 1P wouldn't have
     * been called.
     */
    
    gft_conn[ft].level_required = gft_conn[ft].level;
    gft_conn[ft].level_required_prop = gft_conn[ft].level;
    
    if ( gft_conn[ft].level > max_goal_level ) {
      max_goal_level = gft_conn[ft].level;
    }
    lgoals_at[gft_conn[ft].level][lnum_goals_at[gft_conn[ft].level]++] = ft;
    gft_conn[ft].is_goal = TRUE;
    if ( !gft_conn[ft].ch ) {
      lch_F[lnum_ch_F++] = ft;
      gft_conn[ft].ch = TRUE;
    }
  }

  /*
  for ( i = 0; i < gnum_flogic_goal; i++ ) {
    ft = gflogic_goal[i];
    gft_conn[ft].level_required = max_goal_level;
    gft_conn[ft].level_required_prop = max_goal_level;

  }
  */

  for ( i = 0; i < gnum_fnumeric_goal; i++ ) {
    fl = gfnumeric_goal_fl[i];
    val = gfnumeric_goal_c[i];
    comp = gfnumeric_goal_comp[i];
    for ( j = 0; j <= max; j++ ) {
      if ( !gfl_conn[fl].def[j] ) continue;
      if ( number_comparison_holds( comp, gfl_conn[fl].level[j], val ) ) break;
    }
    if ( j > max ) {
      printf("\n\nnumeric goal not reached in 1P??\n\n");
      exit( 1 );
    }

    gfl_conn[fl].level_required = j;

    if ( j > max_goal_level ) {
      max_goal_level = j;
    }
    lf_goals_c_at[j][fl] = val;
    lf_goals_comp_at[j][fl] = comp;
  }

  return max_goal_level;

}



void achieve_goals (int time )

{

  int i, j, k, ft, min_e, ef;
  float val;
  float min_p, p;

  /* achieve the goals set at level time >= 1
   */
  
  if ( gcmd_line.display_info == 127 ) {
    printf("\nselecting at step %3d: ", time-1);
  }

  /* before we start, we must translate the artificial goals
   * into real goals.
   */
  for ( i = gnum_real_fl_conn; i < gnum_fl_conn; i++ ) {
    if ( lf_goals_comp_at[time][i] == IGUAL ) {
      /* this one isn't needed
       */
      continue;
    }
    enforce_artificial_goal( i, time );
  }

  /* for helpful actions:
   * remember at time 1 what the goals were.
   */
  /* raquel: I comment this code to be coherent with all the heuristic
     see below to know where the numeric goals are remembered
  */
  /*
  if ( time == 1 ) {
    for ( i = 0; i < gnum_real_fl_conn; i++ ) {
      lHcomp[i] = lf_goals_comp_at[time][i];
      lHc[i] = lf_goals_c_at[time][i];
    }
  }
  */

  /* first, push the numeric goals at this level so far down that
   * the requirement for each of them can be fulfilled in the previous
   * level.
   */
  for ( i = 0; i < gnum_real_fl_conn; i++ ) {
    if ( lf_goals_comp_at[time][i] == IGUAL ) {
      continue;
    }
    if ( gfl_conn[i].def[time-1] &&
	 number_comparison_holds( lf_goals_comp_at[time][i],
				  gfl_conn[i].level[time-1],
				  lf_goals_c_at[time][i] ) ) {
      /* this can be solved one step earlier.
       * propagate it downwards and mark as OK.
       */
      update_f_goal( i, time-1, lf_goals_comp_at[time][i], lf_goals_c_at[time][i] );
      lf_goals_comp_at[time][i] = IGUAL;
      continue;
    }
    /* if there is a good assigner, then take it.
     */
    for ( j = 0; j < gfl_conn[i].num_AS; j++ ) {
      ef = gfl_conn[i].AS[j];
      if ( !LESS( gef_conn[ef].level, time ) ) {
	/* we allow any effect that's already there
	 */
	continue;
      }
      if ( gfl_conn[i].AS_fl_[j] != -1 &&
	   !gfl_conn[gfl_conn[i].AS_fl_[j]].def[time-1] ) {
	/* accesses an undefined value.
	 */
	continue;
      }
      if ( gfl_conn[i].AS_fl_[j] != -1 ) {
	val = gfl_conn[gfl_conn[i].AS_fl_[j]].level[time-1] + gfl_conn[i].AS_c[j];
      } else {
	val = gfl_conn[i].AS_c[j];
      }
      if ( !number_comparison_holds( lf_goals_comp_at[time][i],
				     val,
				     lf_goals_c_at[time][i] ) ) {
	/* that one is not strong enough.
	 */
	continue;
      }
      break;
    }
    if ( j < gfl_conn[i].num_AS ) {
      /* ef is an assigner that is strong enough and already there.
       */
      if ( gef_conn[ef].in_plan == time - 1 ) {
	printf("\n\nassigner already selected, nevertheless goal still there\n\n");
	exit( 1 );
      } else {
	if ( gef_conn[ef].in_plan == INFINITY ) {
	  lin_plan_E[lnum_in_plan_E++] = ef;
	}
	gef_conn[ef].in_plan = time - 1;
	if ( gcmd_line.optimize && goptimization_established ) {
	  /* we want to execute this effect here, so we gotta live with the cost.
	   */
	  gcost += gef_conn[ef].cost;
	}
      }
      /* now select the resp. op at this level, if necessary
       */
      select_op( time, ef, gfl_conn[i].level_required,gfl_conn[i].level_required, gfl_conn[i].level_achievable,0 );
      /* now mark the benefits of that effect, introducing
       * also the fl_ level enforcement goals for each effect
       * that is useful for solving a goal at time: in particular,
       * this will be the one we have just selected.
       */      

      /* raquel: remember the one-step numeric goals */
      
      /*if ( gef_conn[ef].level_applicable == 0) {*/
      if (gfl_conn[i].level_achievable == 1){
	for ( k = 0; k < gnum_real_fl_conn; k++ ) {
	  lHcomp[k] = lf_goals_comp_at[time][k];
	  lHc[k] = lf_goals_c_at[time][k];
	}
      }
      

      introduce_benefits_and_enforcements( time, ef );
      /* now introduce the new goals
       */
      introduce_pc_goals( time, ef );

      /* care about next fluent
       */
      continue;
    }
    /* debug...
     */
    if ( !gfl_conn[i].def[time-1] ) {
      printf("\n\nall assignerss applied yet goal not fulfilled - undefined below.\n\n");
      exit( 1 );
    }



    /* no good assigner available. thus, push the goal at this level so far 
     * down that its requirement can be fulfilled in the previous level.
     */
    for ( j = 0; j < gfl_conn[i].num_IN; j++ ) {
      /* go through increasers in constant quantity order top to
       * bottom (see inst_final.c);
       */
      ef = gfl_conn[i].IN[j];
      if ( !LESS( gef_conn[ef].level, time ) ) {
	continue;
      }
      if ( gfl_conn[i].IN_fl_[j] != -1 &&
	   !gfl_conn[gfl_conn[i].IN_fl_[j]].def[time-1] ) {
	/* accesses an undefined fluent.
	 */
	continue;
      }
      if ( gfl_conn[i].IN_fl_[j] != -1 ) {
	val = gfl_conn[gfl_conn[i].IN_fl_[j]].level[time-1] + gfl_conn[i].IN_c[j];
      } else {
	val = gfl_conn[i].IN_c[j];
      }
      if ( val <= 0 ) {
	/* that one does not help us at all.
	 */
	continue;
      }
      /* if ef is already selected here, we can not use it anymore;
       * else, record it as selected.
       */
      if ( gef_conn[ef].in_plan == time - 1 ) {
	continue;
      } else {
	if ( gef_conn[ef].in_plan == INFINITY ) {
	  lin_plan_E[lnum_in_plan_E++] = ef;
	}
	gef_conn[ef].in_plan = time - 1;
	if ( gcmd_line.optimize && goptimization_established ) {
	  gcost += gef_conn[ef].cost;
	}
      }
      /* do the usual stuff...
       */
      select_op( time, ef, gfl_conn[i].level_required, gfl_conn[i].level_required,gfl_conn[i].level_achievable,0 );
      /* raquel: remember the one-step numeric goals */
      if ( gfl_conn[i].level_achievable == 1) {
      /*if ( gef_conn[ef].level_applicable == 0) {*/
	for ( k = 0; k < gnum_real_fl_conn; k++ ) {
	  lHcomp[k] = lf_goals_comp_at[time][k];
	  lHc[k] = lf_goals_c_at[time][k];
	}
      }


      introduce_benefits_and_enforcements( time, ef );
      introduce_pc_goals( time, ef );
      /* stop as soon as 
       * goal can be fulfilled one step below.
       */
      if ( number_comparison_holds( lf_goals_comp_at[time][i],
				    gfl_conn[i].level[time-1],
				    lf_goals_c_at[time][i] ) ) {
	break;
      }
    }
    /* now propagate the revised goal downward, and say we are finished with
     * this one.
     */
    update_f_goal( i, time-1, lf_goals_comp_at[time][i], lf_goals_c_at[time][i] );
    lf_goals_comp_at[time][i] = IGUAL;

    /* debug...
     */
    if ( !number_comparison_holds( lf_goals_comp_at[time-1][i],
				   gfl_conn[i].level[time-1],
				   lf_goals_c_at[time-1][i] ) ) {
      printf("\n\nall increasers applied yet goal not fulfilled.\n\n");
      exit( 1 );
    }
  }/* fluents at level time */



  /* now achieve also the remaining logic goals here.
   */
  for ( i = 0; i < lnum_goals_at[time]; i++ ) {

    ft = lgoals_at[time][i];

     // raquel: remember the one-step logic goals 
    //     if (time == 1)
    /*
    if (gft_conn[ft].level_achievable == 1)
    // Para HA original 

      {
        one_step_lgoals[lnum_one_step_lgoals++]= ft;
      }
    */
    
        
    if ( gft_conn[ft].is_true == time ) {
      // fact already added by prev now selected op
       

      if (gft_conn[ft].level_achievable == 1 
	  //&& 
	  //gft_conn[ft].level_required == gft_conn[ft].level_required_prop
	  )
      {
        one_step_lgoals[lnum_one_step_lgoals++]= ft;
      }



      continue;
    }
    

    min_p = INFINITY;
    min_e = -1;
    for ( j = 0; j < gft_conn[ft].num_A; j++ ) {
      ef = gft_conn[ft].A[j];
      if ( gef_conn[ef].level != time - 1 ) continue; 
      p = 0;
      for ( k = 0; k < gef_conn[ef].num_PC; k++ ) {
	if (level_cost[gft_conn[gef_conn[ef].PC[k]].level] > gft_conn[gef_conn[ef].PC[k]].level)
	  p +=level_cost[gft_conn[gef_conn[ef].PC[k]].level];
	else  
	  p += gft_conn[gef_conn[ef].PC[k]].level;
      }
      if ( LESS( p, min_p ) ) {
	min_p = p;
	min_e = ef;
      }
    }
    ef = min_e;
    /* if ef is already selected, we can not use it anymore;
     * else, record it as selected.
     *
     * actually it can't happen here that the ef
     * is already selected as then the goal is true already.
     * nevermind.
     */
    
    if ( gef_conn[ef].in_plan == time - 1 ) {
      continue;
    
    } else {
      if ( gef_conn[ef].in_plan == INFINITY ) {
	lin_plan_E[lnum_in_plan_E++] = ef;
      }
      gef_conn[ef].in_plan = time - 1;
      if ( gcmd_line.optimize && goptimization_established ) {
	gcost += gef_conn[ef].cost;
      }
    }
    if (gcmd_line.display_info == 127)
      {
	print_ft_name(ft);
	printf("ft achievable: %d,ft required: %d, ft required prop: %d", gft_conn[ft].level_achievable, 
	   gft_conn[ft].level_required, gft_conn[ft].level_required_prop);
      }
    select_op( time, ef, gft_conn[ft].level_required, gft_conn[ft].level_required_prop, gft_conn[ft].level_achievable, min_p);
    
    // raquel: remember the one-step logic goals 
    
    
    if (gef_conn[ef].level_applicable == 0)
      {
      // ft was a one-step needed fact: for helpful actions computation
	//	if (gft_conn[ft].level_required == gft_conn[ft].level_required_prop)
	  one_step_lgoals[lnum_one_step_lgoals++]= ft;

      }
    
   
    introduce_benefits_and_enforcements( time, ef );
    introduce_pc_goals( time, ef );
  }

}



void enforce_artificial_goal( int fl, int time )

{

  int i;

  if ( !gfl_conn[fl].artificial ) {
    printf("\n\ntrying to enforce non-artificial goal\n\n");
    exit( 1 );
  }

  /* for now, we simply require the maximum levels of all 
   * composites.
   */
  for ( i = 0; i < gfl_conn[fl].num_lnf; i++ ) {
    update_f_goal( gfl_conn[fl].lnf_F[i], time, GEQ,
		   gfl_conn[gfl_conn[fl].lnf_F[i]].level[time] );
  }
  
}



void select_op( int time, int ef, int level_required, int level_required_prop, int level_achievable, int difficulty)

{
  int op = gef_conn[ef].op;

  if ( gop_conn[op].is_used != time - 1 ) {
    /* we don't have this op yet at this level
     * --> count it and record it for setting back info.
     */
    if ( gop_conn[op].is_used == INFINITY ) {
      lused_O[lnum_used_O++] = op;
    }
    gop_conn[op].is_used = time - 1;
    lh++;
    sum_difficulty = sum_difficulty + difficulty;
    
    /* raquel: save the selected op in the relaxed plan
     */
    g_RP[gnum_RP++]= op;
    
    if (LESS(level_required, gop_conn[op].level_required))
      gop_conn[op].level_required = level_required;

    if (LESS(level_required_prop, gop_conn[op].level_required_prop))
      gop_conn[op].level_required_prop = level_required_prop;

    if (level_achievable == gef_conn[ef].level_applicable + 1 &&
	level_required == level_required_prop)
      {
	gop_conn[op].level_required_prop =   gop_conn[op].level_required ;	
      }
    else
      {
	gef_conn[ef].not_necessarilly_required = TRUE; 
      }

    /*        
    if (LESS(gop_conn[op].level_achievable, level_achievable))
      gop_conn[op].level_achievable = level_achievable;
    */
    gop_conn[op].level_achievable = gef_conn[ef].level_applicable;
    gop_conn[op].level = gef_conn[ef].level;

    if ( gcmd_line.display_info == 127 ) {
      printf("\n                       ");
      print_op_name( op );
      printf("\n                       ");
      printf(" (cost + gtt %f) (level %d) (level-cost %f)\n", 
	     gef_conn[ef].cost + gtt, gef_conn[ef].level, level_cost[gef_conn[ef].level]);
      printf("\n                       ");
      printf(" achievable: %d, required: %d, required_prop: %d\n", gop_conn[op].level_achievable, gop_conn[op].level_required, gop_conn[op].level_required_prop);
    }
  }

}



void introduce_benefits_and_enforcements( int time, int ef )

{

  int k, l, ft, fl;
  float val;

  for ( k = 0; k < gef_conn[ef].num_A; k++ ) {
    ft = gef_conn[ef].A[k];
    gft_conn[ft].is_true = time;
    if ( !gft_conn[ft].ch ) {
      lch_F[lnum_ch_F++] = ft;
      gft_conn[ft].ch = TRUE;
    }
  }

  for ( k = 0; k < gef_conn[ef].num_AS; k++ ) {
    fl = gef_conn[ef].AS_fl[k];
    if ( lf_goals_comp_at[time][fl] == IGUAL ) {
      continue;
    }
    if ( !assign_value( ef, time - 1, k, &val ) ) {
      continue;
    }
    if ( number_comparison_holds( lf_goals_comp_at[time][fl],
				  val,
				  lf_goals_c_at[time][fl] ) ) {
      /* this effect assigns what we wanted there. enforce its
       * dependency fluent fl_
       */
      lf_goals_comp_at[time][fl] = IGUAL;
      enforce_assign( ef, time - 1, k );
    }
  }
  for ( k = 0; k < gef_conn[ef].num_IN; k++ ) {
    fl = gef_conn[ef].IN_fl[k];
    if ( lf_goals_comp_at[time][fl] == IGUAL ) {
      continue;
    }
    if ( !increase_value( ef, time - 1, k, &val ) ) {
      continue;
    }
    if ( val > 0 ) {
      lf_goals_c_at[time][fl] -= val;
      enforce_increase( ef, time - 1, k );
    }
  }

  for ( k = 0; k < gef_conn[ef].num_I; k++ ) {
    if ( gef_conn[gef_conn[ef].I[k]].in_plan == time - 1 ) {
      continue;
    } else {
      if ( gef_conn[gef_conn[ef].I[k]].in_plan == INFINITY ) {
	lin_plan_E[lnum_in_plan_E++] = gef_conn[ef].I[k];
      }
      gef_conn[gef_conn[ef].I[k]].in_plan = time - 1;
      if ( gcmd_line.optimize && goptimization_established ) {
	gcost += gef_conn[gef_conn[ef].I[k]].cost;
      }
    }
    for ( l = 0; l < gef_conn[gef_conn[ef].I[k]].num_A; l++ ) {
      ft = gef_conn[gef_conn[ef].I[k]].A[l];
      gft_conn[ft].is_true = time;
      if ( !gft_conn[ft].ch ) {
	lch_F[lnum_ch_F++] = ft;
	gft_conn[ft].ch = TRUE;
      }
    }
    for ( l = 0; l < gef_conn[gef_conn[ef].I[k]].num_AS; l++ ) {
      fl = gef_conn[gef_conn[ef].I[k]].AS_fl[l];
      if ( lf_goals_comp_at[time][fl] == IGUAL ) {
	continue;
      }
      if ( !assign_value( gef_conn[ef].I[k], time - 1, l, &val ) ) {
	continue;
      }
      if ( number_comparison_holds( lf_goals_comp_at[time][fl],
				    val,
				    lf_goals_c_at[time][fl] ) ) {
	lf_goals_comp_at[time][fl] = IGUAL;
	enforce_assign( gef_conn[ef].I[k], time - 1, l );
      }
    }
    for ( l = 0; l < gef_conn[gef_conn[ef].I[k]].num_IN; l++ ) {
      fl = gef_conn[gef_conn[ef].I[k]].IN_fl[l];
      if ( lf_goals_comp_at[time][fl] == IGUAL ) {
	continue;
      }
      if ( !increase_value( gef_conn[ef].I[k], time - 1, l, &val ) ) {
	continue;
      }
      if ( val > 0 ) {
	lf_goals_c_at[time][fl] -= val;
	enforce_increase( gef_conn[ef].I[k], time - 1, l );
      }
    }
  }/* implied effects */

}



Bool assign_value( int ef, int at_time, int nr, float *val )

{

  if ( gef_conn[ef].AS_fl_[nr] == -1 ) {
    /* no dependency.
     */
    *val = gef_conn[ef].AS_c[nr];
    return TRUE;
  }

  if ( !gfl_conn[gef_conn[ef].AS_fl_[nr]].def[at_time] ) {
    return FALSE;
  }
  
  *val = gfl_conn[gef_conn[ef].AS_fl_[nr]].level[at_time] + gef_conn[ef].AS_c[nr];
  return TRUE;

}



Bool increase_value( int ef, int at_time, int nr, float *val )

{

  if ( gef_conn[ef].IN_fl_[nr] == -1 ) {
    /* no dependency.
     */
    *val = gef_conn[ef].IN_c[nr];
    return TRUE;
  }

  if ( !gfl_conn[gef_conn[ef].IN_fl_[nr]].def[at_time] ) {
    return FALSE;
  }
  
  *val = gfl_conn[gef_conn[ef].IN_fl_[nr]].level[at_time] + gef_conn[ef].IN_c[nr];
  return TRUE;

}



void enforce_assign( int ef, int at_time, int nr )

{

  if ( gef_conn[ef].AS_fl_[nr] == -1 ) {
    return;
  }

  if ( !gfl_conn[gef_conn[ef].AS_fl_[nr]].def[at_time] ) {
    printf("\n\ntrying to enforce an undefined value.\n\n");
    exit( 1 );
  }

  /* for now, simply require the maximum benefit of the effect.
   */
  update_f_goal( gef_conn[ef].AS_fl_[nr], at_time, GEQ,
		 gfl_conn[gef_conn[ef].AS_fl_[nr]].level[at_time] );

}



void enforce_increase( int ef, int at_time, int nr )

{

  if ( gef_conn[ef].IN_fl_[nr] == -1 ) {
    return;
  }

  if ( !gfl_conn[gef_conn[ef].IN_fl_[nr]].def[at_time] ) {
    printf("\n\ntrying to enforce an undefined value.\n\n");
    exit( 1 );
  }

  /* for now, simply require the maximum benefit of the effect.
   */
  update_f_goal( gef_conn[ef].IN_fl_[nr], at_time, GEQ,
		 gfl_conn[gef_conn[ef].IN_fl_[nr]].level[at_time] );

}



void introduce_pc_goals( int time, int ef )

{

  int k, l, ft, fl;
  float val;
  Comparator comp;

  /* now introduce the new goals
   */
  for ( k = 0; k < gef_conn[ef].num_PC; k++ ) {
    ft = gef_conn[ef].PC[k];

    if (LESS(time-1,gft_conn[ft].level_required)){
      gft_conn[ft].level_required = time -1;
    }
    /*
    if (gef_conn[ef].level_applicable > gft_conn[ft].level_achievable )
      gft_conn[ft].level_required_prop = gop_conn[gef_conn[ef].op].level_required;
    else
      gft_conn[ft].level_required_prop = gft_conn[ft].level_required;
      
    */

    if (gef_conn[ef].not_necessarilly_required)
     {
        if (LESS(gop_conn[gef_conn[ef].op].level_required_prop,gft_conn[ft].level_required_prop)){
	  gft_conn[ft].level_required_prop = gop_conn[gef_conn[ef].op].level_required_prop;  

	}
     }
    else
      {
	gft_conn[ft].level_required_prop = gft_conn[ft].level_required;
      }
       
	

    /*
    if (LESS(gop_conn[gef_conn[ef].op].level_required_prop,gft_conn[ft].level_required_prop)){
      gft_conn[ft].level_required_prop = gop_conn[gef_conn[ef].op].level_required_prop;
    }
    */


    if ( gft_conn[ft].is_goal ) {
      /* this fact already is a goal
       */
      continue;
    }
    lgoals_at[gft_conn[ft].level][lnum_goals_at[gft_conn[ft].level]++] = ft;
    gft_conn[ft].is_goal = TRUE;
    if ( !gft_conn[ft].ch ) {
      lch_F[lnum_ch_F++] = ft;
      gft_conn[ft].ch = TRUE;
    }
  }
  /* now for the numeric precs.
   */
  for ( k = 0; k < gef_conn[ef].num_f_PC; k++ ) {
    fl = gef_conn[ef].f_PC_fl[k];
    val = gef_conn[ef].f_PC_c[k];
    comp = gef_conn[ef].f_PC_comp[k];
    /* determine the first level where prec can be fulfilled.
     */
    for ( l = 0; l < time; l++ ) {
      if ( !gfl_conn[fl].def[l] ) continue;
      if ( number_comparison_holds( comp, gfl_conn[fl].level[l], val ) ) break;
    }
    if ( l >= time ) {
      printf("\n\nnumeric prec not reached in 1P??\n\n");
      exit( 1 );
    }

    if (LESS(time,gfl_conn[fl].level_required)){
      gfl_conn[fl].level_required = time;
    }

    /* if new requirement is stronger than old, then insert it.
     */
    update_f_goal( fl, l, comp, val );
  }

}



void update_f_goal( int fl, int time, Comparator comp, float val )

{

  if ( lf_goals_comp_at[time][fl] == IGUAL ) {
    lf_goals_comp_at[time][fl] = comp;
    lf_goals_c_at[time][fl] = val;
    return;
  }

  if ( lf_goals_c_at[time][fl] < val ) {
    lf_goals_comp_at[time][fl] = comp;
    lf_goals_c_at[time][fl] = val;
    return;
  }

  if ( lf_goals_c_at[time][fl] == val &&
       comp == GE ) {
    lf_goals_comp_at[time][fl] = GE;
  }
   
}



void reset_search_info( void )

{

  int i;

  for ( i = 0; i < lnum_ch_F; i++ ) {
    gft_conn[lch_F[i]].is_true = INFINITY;
    gft_conn[lch_F[i]].is_goal = FALSE;
    gft_conn[lch_F[i]].ch = FALSE;
  }
  lnum_ch_F = 0;

  for ( i = 0; i < lnum_used_O; i++ ) {
    gop_conn[lused_O[i]].is_used = INFINITY;
    gop_conn[lused_O[i]].level = INFINITY;
    gop_conn[lused_O[i]].level_required = INFINITY;
    gop_conn[lused_O[i]].level_required_prop = INFINITY;
    gop_conn[lused_O[i]].level_achievable = INFINITY;
  }
  lnum_used_O = 0;
  
  for ( i = 0; i < lnum_in_plan_E; i++ ) {
    gef_conn[lin_plan_E[i]].in_plan = INFINITY;
  }
  lnum_in_plan_E = 0;

}


int float_cmp( const void* a, const void* b)
{
  if (* (float *) a== *(float *) b)
    return 0;
  else
    if (LESS(* (float *)a,  * (float *)b))
        return -1;
     else
     return 1;
}




void collect_H_info( void )

{

  static Bool first_call = TRUE;

  int i, j, ft, ef, op;
  float val;
  //  int initial_gH_index;


  /* raquel: for the case of hmax and hadd there aren't helpful actions: no
     relaxed plan is extracted. So consider as helpful all applicable actions
     in level 0 */
  if (gcmd_line.heuristic == H_MAX || gcmd_line.heuristic == H_ADD || gcmd_line.heuristic == H_SUM_MAX || 
      gcmd_line.heuristic == H_MAX_ADD)
    {
      if ( first_call ){
	printf("\n\nff WARNING: the selected heuristic does not compute helpful actions\n");
	printf("            all applicable actions are considered helpful!!\n");
	first_call = FALSE;
      }
      collect_A_info(FALSE);

      /* raquel: copy gA into gH */
      for ( i = 0; i < gnum_A; i++ ) {
	gop_conn[gA[i]].is_in_A = FALSE;
	gop_conn[gA[i]].is_in_H = TRUE;
      }
      gH = gA;
      gnum_H = gnum_A;
      gnum_A = 0;

      if ( gcmd_line.display_info == 128 ) {
	printf("\ncollected H: ");
	for ( i = 0; i < gnum_H; i++ ) {
	  print_op_name( gH[i] );
	  printf("\n              ");
	}
      }

      return;
    }



  if ( first_call ) {
    gH = ( int * ) calloc( gnum_op_conn, sizeof( int ) );
    gnum_H = 0;
    first_call = FALSE;
  }
      

  for ( i = 0; i < gnum_H; i++ ) {
    gop_conn[gH[i]].is_in_H = FALSE;
  }

  /* first the logical guys
   */
  gnum_H = 0;

  /* raquel: I mofified next to be able of computing always helpful
     actions */

  /*for ( i = lnum_goals_at[1] - 1; i >= 0; i-- ) {*/
    for ( i = lnum_one_step_lgoals - 1; i >= 0; i--) {
      /* ft = lgoals_at[1][i];*/
    ft = one_step_lgoals[i];
    /*
    printf("\n");
    print_ft_name(ft);
    fflush(stdout);
    */
    //    initial_gH_index = gnum_H;
    for ( j = 0; j < gft_conn[ft].num_A; j++ ) {
      ef = gft_conn[ft].A[j];
      if ( gef_conn[ef].level_applicable != 0 ) {
	continue;
      }
      op = gef_conn[ef].op;

      if ( gop_conn[op].is_in_H ) {
	continue;
      }
      gop_conn[op].is_in_H = TRUE;
      /*
      printf("\n");
      print_op_name(op);
      fflush(stdout);
      */

      gH[gnum_H++] = op;
    }
    // qsort(&gH[initial_gH_index], gnum_H - initial_gH_index, sizeof(float),float_cmp);
  }
  /* then the numerical ones.
   */
  for ( i = 0; i < gnum_real_fl_conn; i++ ) {
    if ( lHcomp[i] == IGUAL ) {
      /* don't need this one at all.
       */
      continue;
    }
    if ( gfl_conn[i].def[0] &&
	 number_comparison_holds( lHcomp[i], gfl_conn[i].level[0], lHc[i] ) ) {
      /* this one's already ok initially... was only pushed down
       * through RPG.
       */
      continue;
    }
				  
    /* assigners
     */
    for ( j = 0; j < gfl_conn[i].num_AS; j++ ) {
      ef = gfl_conn[i].AS[j];
      op = gef_conn[ef].op;
      if ( gop_conn[op].is_in_H ) {
	continue;
      }

      if ( gef_conn[ef].level_applicable != 0 ) {
	continue;
      }
      if ( gef_conn[ef].illegal ) {
	continue;
      }
      if ( gfl_conn[i].AS_fl_[j] != -1 &&
	   !gfl_conn[gfl_conn[i].AS_fl_[j]].def[0] ) {
	continue;
      }
      if ( gfl_conn[i].AS_fl_[j] == -1 ) {
	val = gfl_conn[i].AS_c[j];
      } else {
	val = gfl_conn[gfl_conn[i].AS_fl_[j]].level[0] + gfl_conn[i].AS_c[j];
      }
      if ( !number_comparison_holds( lHcomp[i], val, lHc[i] ) ) {
	continue;
      }

      gop_conn[op].is_in_H = TRUE;
      gH[gnum_H++] = op;
    }

    if ( !gfl_conn[i].def[0] ) {
      /* gotta be assigned.
       */
      continue;
    }

    /* increasers
     */
    for ( j = 0; j < gfl_conn[i].num_IN; j++ ) {
      ef = gfl_conn[i].IN[j];
      op = gef_conn[ef].op;
      if ( gop_conn[op].is_in_H ) {
	continue;
      }

      if ( gef_conn[ef].level_applicable != 0 ) {
	continue;
      }
      if ( gef_conn[ef].illegal ) {
	continue;
      }
      if ( gfl_conn[i].IN_fl_[j] != -1 &&
	   !gfl_conn[gfl_conn[i].IN_fl_[j]].def[0] ) {
	continue;
      }
      if ( gfl_conn[i].IN_fl_[j] == -1 ) {
	val = gfl_conn[i].IN_c[j];
      } else {
	val = gfl_conn[gfl_conn[i].IN_fl_[j]].level[0] + gfl_conn[i].IN_c[j];
      }
      if ( val <= 0 ) {
	continue;
      }

      gop_conn[op].is_in_H = TRUE;
      gH[gnum_H++] = op;
    }
  }

  if ( gcmd_line.display_info == 128 ) {
    printf("\ncollected H: ");
    for ( i = 0; i < gnum_H; i++ ) {
      print_op_name( gH[i] );
      printf("\n              ");
    }
    fflush(stdout);
  }

}



/* raquel: deactivate all actions deleting top-level logic goals */

void deactivate_actions_deleting_top_level_logic_goals(void)
{
  int index,i, k;
  static Bool first_call = TRUE;
  Bool next_ef = FALSE;
  
  if (first_call)
    {
      for ( index = 0; index < gnum_ef_conn; index++ ){

	next_ef = FALSE;

	for (i = 0; i < gef_conn[index].num_D && !next_ef; i++ ){

	  for ( k = 0; k < gnum_flogic_goal; k++ ){

	    if (gflogic_goal[k] == gef_conn[index].D[i]){
		gop_conn[gef_conn[index].op].RP_deactivated = TRUE;

		gef_conn[index].deletes_top_level_goal = TRUE;
		
		/* printf("\n deactivated: ");
		   print_op_name(gef_conn[index].op);
		*/
		
		next_ef = TRUE;

		break;
		
	    }
	  }
	}

      }

      first_call = FALSE;
    }

  else
    {
      for ( index = 0; index < gnum_ef_conn; index++ ){
	if (gef_conn[index].deletes_top_level_goal){
	  gop_conn[gef_conn[index].op].RP_deactivated = TRUE;
	}
      }
    }

}


void activate_all(void)
{
  int index;
  
  for ( index = 0; index < gnum_op_conn; index++ ){
    gop_conn[index].RP_deactivated = FALSE;
  }
}


void activate_actions_deleting_top_level_logic_goals(void)
{

  int index;
  for ( index = 0; index < gnum_ef_conn; index++ ){
    if (gef_conn[index].deletes_top_level_goal){
	gop_conn[gef_conn[index].op].RP_deactivated = FALSE;

    }
  }
}
  
  

void print_relaxed_plan()
{ int i;
  int op;

  printf("\n--------%d",gcmd_line.heuristic);
  for (i = gnum_RP-1; i>=0; i--)
    {
      op = g_RP[i];
      printf("\n                       ");
      print_op_name( op );
      printf(" (cost + gtt %f)\n", 
	     gef_conn[op].cost + gtt);

    }
}


void print_OpenApp(void)
{

  OpenApp *first;
  
  printf("\n\nOPEN APP begin:");

  if (OpenApp_head) {
    first = OpenApp_head->next;

    while (first){
      printf("\n");
      print_op_name(gef_conn[first->ef].op);
      printf(" %f",gef_conn[first->ef].acc_cost);
      first = first->next;
    }
  }
 printf("\nOPEN APP end");
}
