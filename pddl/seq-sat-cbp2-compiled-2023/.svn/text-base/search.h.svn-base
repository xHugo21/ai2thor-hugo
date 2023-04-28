


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
 * File: search.h
 *
 * Description: headers of routines that search the state space
 *
 *              ADL version, Enforced Hill-climbing enhanced with
 *                           Goal-adders deletion heuristic
 *
 * Author: Joerg Hoffmann 2000
 * Modified by Raquel Fuentetaja 2008
 *********************************************************************/ 






#ifndef _SEARCH_H
#define _SEARCH_H

extern float final_total_cost;
extern int gexpanded_states;

extern int Search_upper_bound;
extern BfsHashEntry_pointer lbfs_hash_entry[BFS_HASH_SIZE];


Bool do_enforced_hill_climbing( void );
Bool do_enforced_hill_climbing_cost( void );
Bool do_enforced_hill_climbing_cost_order_first_level( void );


Bool search_for_better_state( State *S, int h, State *S_, int *h_ );
void add_to_ehc_space( State *S, int op, EhcNode *father );
int expand_first_node( int h );

Bool search_for_better_state_cost( State *S, int h, float h_cost, State *S_, int *h_ , float *h_cost_);
int expand_first_node_cost( int h, float h_cost, float *h_cost_, float* g_cost_);
int compare_helpful_actions( const void* a, const void* b ); 
void add_to_ehc_space_cost( State *S, int op, EhcNode *father );

Bool do_enforced_hill_climbing_cost_hdiff( void );
Bool search_for_better_state_cost_hdiff( State *S, int h, float h_cost, State *S_, int *h_ , float *h_cost_);
int expand_first_node_cost_hdiff( int h, float h_cost, float *h_cost_, float* g_cost_);


Bool search_for_better_state_cost_order_first_level( State *S, int h, float h_cost, State *S_, int *h_ , float *h_cost_);

void hash_ehc_node( EhcNode *n );
Bool ehc_state_hashed( State *S );
Bool superior_ehc_state_hashed( State *S );
Bool superior_state( State *S1, State *S2 );
void reset_ehc_hash_entrys( void );



void extract_plan_fragment( State *S );
PlanHashEntry *hash_plan_state( State *S, int step );
PlanHashEntry *plan_state_hashed( State *S );
Bool same_state( State *S1, State *S2 );



Bool do_hill_climbing_cost( void );



Bool do_best_first_search( void );
void add_to_bfs_space( State *S, int op, BfsNode *father );
float state_cost( State *S, BfsNode *father );
void extract_plan( BfsNode *last );



void hash_bfs_node( BfsNode *n );
Bool bfs_state_hashed( State *S );
Bool superior_bfs_state_hashed( State *S );
int state_sum( State *S );



Bool result_to_dest( State *dest, State *source, int op );
Bool determine_source_val( State *source, int fl, float *val );
void copy_source_to_dest( State *dest, State *source );
void source_to_dest( State *dest, State *source );

/* Raquel functions
 */
EhcNode*  same_no_metric_ehc_state_hashed( State *S);


BfsNode * same_no_metric_bfs_state_hashed( State *S, BfsNode *father);
Bool same_no_metric_state( State *S1, State *S2 );
void unhash_bfs_node (BfsNode *n);

Bool do_anytime_best_first_search( void );
void add_to_anytime_best_first_search_space( State *S, int op, BfsNode *father );

Bool do_anytime_bfs_bnb_learn( void );
void add_to_anytime_bfs_bnb_learn_space( State *S, int op, BfsNode *father );

Bool do_anytime_bfs_HA( void );
BfsNode* add_to_anytime_bfs_HA_space( State *S, int op, BfsNode *father );

Bool do_anytime_bfs_HA_lookahead( void );
BfsNode *add_to_anytime_bfs_HA_space_lookahead( State *S, int op, BfsNode *father );

Bool do_anytime_bfs_HA_lookahead2( void );
BfsNode *add_to_anytime_bfs_HA_space_lookahead2( State *S, int op, BfsNode *father );

Bool do_anytime_bfs_HA_lookahead3( void );
BfsNode *add_to_anytime_bfs_HA_space_lookahead3( State *S, int op, BfsNode *father );

Bool do_anytime_bfs_HA_lookahead4( void );
BfsNode *add_to_anytime_bfs_HA_space_lookahead4( State *S, int op, BfsNode *father );

Bool do_anytime_bfs_bnb_learn( void );
void add_to_anytime_bfs_bnb_learn_space( State *S, int op, BfsNode *father );


void add_to_secondary_list ( State* S, int op, BfsNode* father );

Bool do_anytime_bfs_HA_adm( void );
BfsNode* add_to_anytime_bfs_HA_adm_space( State *S, int op, BfsNode *father );

Bool do_anytime_bfs_HA_rescue( void );
void add_to_anytime_bfs_HA_rescue_space( State *S, int op, BfsNode *father );

Bool do_anytime_bfs_HA_rescue_lookahead( void );
BfsNode *add_to_anytime_bfs_HA_rescue_space_lookahead( State *S, int op, BfsNode *father );

Bool do_anytime_bfs_HA_rescue_lookahead_recomputeh( void );
BfsNode *add_to_anytime_bfs_HA_rescue_space_lookahead_recomputeh( State *S, int op, BfsNode *father, Bool is_lookahead );



BfsNode *obtain_lookahead_state_basic (BfsNode *father_node);
BfsNode *obtain_lookahead_state_order (BfsNode *father_node);
BfsNode *obtain_lookahead_state_order_rep (BfsNode *father_node);

BfsNode *add_to_lookahead_list ( State* S, int op, BfsNode* father );


/* Random search functions*/
Bool do_depth_first_chronological_backtraking_random( void );
BfsNode* add_to_depth_first_space_random( State *S, int op, BfsNode *father );

/* Roller search functions*/
Bool do_depth_first_chronological_backtraking_roller( void );
BfsNode* add_to_depth_first_space_roller( State *S, int op, BfsNode *father );


/* Depth first search functions using ff heuristic instead Roller. Baseline*/
Bool do_depth_first_chronological_backtraking_ff( void );
BfsNode* add_to_depth_first_space_ff( State *S, int op, BfsNode *father );

Bool are_there_pending_logic_goals(State *S);
void compute_total_cost_from_metric(State *S);

/* Modified Hill Climbing */
Bool do_modified_hill_climbing( void );
void add_to_modified_hill_climbing_space( State *S, int op, BfsNode *father );

/* Best firs roller policy */
Bool do_anytime_bfs_roller_policy( void );
BfsNode* add_to_anytime_bfs_roller_policy( State *S, int op, BfsNode *father );
BfsNode* apply_reactive_policy (BfsNode *node, int horizon, BfsNode* (*F_add_to_open) ( State *S, int op, BfsNode *father ) );
BfsNode* apply_reactive_policy_FF_heuristic(BfsNode *node, int horizon, BfsNode* (*F_add_to_open) ( State *S, int op, BfsNode *father ) );

Bool do_anytime_bfs_HA_rescue_roller_policy( void );
Bool do_anytime_bfs_HA_rescue_FF_heuristic_policy( void );
BfsNode *add_to_anytime_bfs_HA_rescue_space_roller_policy( State *S, int op, BfsNode *father );


void include_list_in_open(BfsNode *list_head, BfsNode *list_tail, BfsNode* (*F_add_to_open) ( State *S, int op, BfsNode *father ) );

void print_list(BfsNode *list_head);

void  generate_heuristic_values_for_HA(BfsNode *node);
int compare_helpful_actions_FF_heuristic( const void* a, const void* b );
#endif /* _SEARCH_H */
