#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

#include "collider.h"
#include "tt.h"
#include "vector.h"

extern double g_dt;

static int intersecting( Collider *a, Collider *b );
static double collision_time( Collider *a, Collider *b );
static Vector collision_normal( Collider *a, Collider *b );
static Event intersecting( Event a, Event b );

static int g_normal_dim = -1;



/* whether A and B are intersecting */
int intersecting( Collider *a, Collider *b )
{
	unsigned int overlap_count = 0;
	
	for ( int i = 0; i < 3; i++ )
		overlap_count +=
			( a->x[i] + a->max[i] > b->x[i] + b->min[i]	&&
				b->x[i] + b->max[i] > a->x[i] + a->min[i] );
	
	return overlap_count == 3;
}



/* time after last step that A and B first collided */
double collision_time( Collider *a, Collider *b )
{
	double t = 0.0, t_max = DBL_MIN;
	double x_1 = 0.0, x_2 = 0.0, max_1 = 0.0, min_2 = 0.0, v_rel = 0.0;
	
	for ( int i = 0; i < 3; i++ )
	{
		v_rel = a->v[i] - b->v[i];
		if ( fabs( v_rel ) > 0.0001 ) {
			if ( v_rel > 0.0 )
			{
				x_1 = a->x[i] - a->v[i] * g_dt;
				x_2 = b->x[i] - b->v[i] * g_dt;
				max_1 = a->max[i];
				min_2 = b->min[i];
			}
			else
			{
				x_1 = b->x[i] - b->v[i] * g_dt;
				x_2 = a->x[i] - a->v[i] * g_dt;
				max_1 = b->max[i];
				min_2 = a->min[i];
				v_rel *= -1.0;
			}
			
			if ( x_1 + max_1 < x_2 + min_2 )
			{
				assert( v_rel > 0.0 );
				t = (x_2 + min_2 - (x_1 + max_1)) / v_rel;
				
				if ( t > t_max )
				{
					t_max = t;
					g_normal_dim = i; /* record the dimension of maximum overlap */
				}
			}
		}
	}
	
	assert( t_max > 0.0 );
	
	return t_max;
}



Vector collision_normal( Collider *a, Collider *b )
{
	Vector normal = { 0.0, 0.0, 0.0 };
	normal.x[g_normal_dim] = 1.0;
	
	return normal;
}



/* event for a particular pair of colliders */
Event interaction( Event a, Event b )
{
	Event e = EventMake();
	Signal s = -1;
	
	switch ( SigListFirst( a.sigs ) )
	{
	case COLLIDER_PROJ:
		switch ( SigListFirst( b.sigs ) )
		{
		case COLLIDER_PROJ:
			s = COLLISIONS_PROJ_PROJ;
			break;
		case COLLIDER_TARGET:
			s = COLLISIONS_PROJ_TARGET;
			break;
		case COLLIDER_GUN:
			s = COLLISIONS_PROJ_GUN;
			break;
		case COLLIDER_BALL:
			s = COLLISIONS_PROJ_BALL;
		default:
			break;
		}
		break;
	case COLLIDER_TARGET:
		switch ( SigListFirst( b.sigs ) )
		{
		case COLLIDER_PROJ:
			s = COLLISIONS_PROJ_TARGET;
			break;
		default:
			break;
		}
		break;
	case COLLIDER_BALL:
		switch ( SigListFirst( b.sigs ) )
		{
		case COLLIDER_PROJ:
			s = COLLISIONS_PROJ_BALL;
			break;
		}
		break;
	default:
		break;
	}
	
	SigListAdd( &e.sigs, s );
	return e;
}



/* find intersecting objects, calculate collision times and positions */
Event resolve_collisions( Tt *collisions )
{
	TtList *a_l = NULL, *b_l = NULL;
	Collider *a = NULL, *b = NULL;
	double dt_c = g_dt;
	Vector a_x, a_v, b_x, b_v, n;
	Event e = EventMake();
	
	/* for every pair of objects... */
	for ( a_l = collisions->kids; !TtListEmpty( a_l ); a_l = TtListRest( a_l ) ) {
		for ( b_l = TtListRest( a_l ); !TtListEmpty( b_l ); b_l = TtListRest( b_l ) )
		{
			a = (Collider *)TtListFirst( a_l );
			b = (Collider *)TtListFirst( b_l );
						
			/* if objects colliding... */
			if ( intersecting( a, b ) )
			{
				EventInclude(
					&e,
					interaction( TtExec( (Tt *)a ), TtExec( (Tt *)b ) ) );
				
				if ( a->resoluble && b->resoluble )
				{				
					a_x = vec_make( a->x );
					a_v = vec_make( a->v );
					b_x = vec_make( b->x );
					b_v = vec_make( b->v );
		
					/* find instant of collision */
					dt_c = 0.99 * collision_time( a, b );
					
					/* set positions at instant of collision */
					vec_copy(
						a->x,
						vec_diff( a_x, vec_scaled( a_v, g_dt - dt_c ) ) );
					vec_copy(
						b->x,
						vec_diff( b_x, vec_scaled( b_v, g_dt - dt_c ) ) );
					
					//assert( !intersecting( a, b ) );
					
					/* set collision normal */
					n = collision_normal( a, b );
					vec_copy(
						a->n,
						vec_normed(
							vec_scaled(
								n,
								vec_dot( n, vec_diff( b_x, a_x ) ) ) ) );
					vec_copy(
						b->n,
						vec_normed(
							vec_scaled(
								n,
								vec_dot( n, vec_diff( a_x, b_x ) ) ) ) );
					
					/* inform objects of each other */
					vec_copy( a->other_v, b_v );
					a->other_m = b->m;
					vec_copy( b->other_v, a_v );
					b->other_m = a->m;
					
					/* set remaining time */
					a->dt = g_dt - dt_c;
					b->dt = g_dt - dt_c;
					
					/* execute both objects' responses */
					(*TtListFirst( ((Tt *)a)->kids )->task)( (Tt *)a );
					(*TtListFirst( ((Tt *)b)->kids )->task)( (Tt *)b );				
					//assert( !intersecting( a, b ) );
				}
			}
		}
	}
	
	return e;
}



// /* find intersecting objects, calculate collision times and positions */
// Event resolve_collisions( Tt *collisions )
// {
	// TtList *a_l = NULL, *b_l = NULL;
	// Collider *a_min = NULL, *b_min = NULL;
	// double dt_c = g_dt, dt_c_min = g_dt;
	// Vector a_x, a_v, b_x, b_v, n;
	// Event e = EventMake();
	// 
	// for ( a_l = collisions->kids; !TtListEmpty( a_l ); a_l = TtListRest( a_l ) ) {
		// for ( b_l = TtListRest( a_l ); !TtListEmpty( b_l ); b_l = TtListRest( b_l ) )
		// {
			// Collider *a = NULL, *b = NULL;
			// 
			// a = (Collider *)TtListFirst( a_l );
			// b = (Collider *)TtListFirst( b_l );
			// 
			// if ( intersecting( a, b ) ) {				
				// if ( a->resoluable && b->resoluable )
				// {
					// dt_c = collision_time( a, b );
					// if (dt_c < dt_c_min)
					// {
						// dt_c_min = dt_c;
						// a_min = a;
						// b_min = b;
					// }
				// }
			// }
		// }
	// }
	// 
	// if ( a_min != NULL )
	// {		
		// e = interaction( TtExec( (Tt *)a_min ), TtExec( (Tt *)b_min ) );
		// 
		// /* set all positions at instant of collision */
		// for ( a_l = collisions->kids; !TtListEmpty( a_l ); a_l = TtListRest( a_l ) )
		// {
			// Collider *a = (Collider *)TtListFirst( a_l );
			// 
			// a_x = vec_make( a->x );
			// a_v = vec_make( a->v );
			// 
			// vec_copy(
				// a->x,
				// vec_diff( a_x, vec_scaled( a_v, g_dt - dt_c_min ) ) );
		// }
		// 
		// //assert( !intersecting( a_min, b_min ) );
		// 
		// a_x = vec_make( a_min->x );
		// a_v = vec_make( a_min->v );
		// b_x = vec_make( b_min->x );
		// b_v = vec_make( b_min->v );
		// 
		// /* set collision normal */
		// n = collision_normal( a_min, b_min );
		// vec_copy(
			// a_min->n,
			// vec_normed(
				// vec_scaled(
					// n,
					// vec_dot( n, vec_diff( b_x, a_x ) ) ) ) );
		// vec_copy(
			// b_min->n,
			// vec_normed(
				// vec_scaled(
					// n,
					// vec_dot( n, vec_diff( a_x, b_x ) ) ) ) );
		// 
		// /* inform objects of each other */
		// vec_copy( a_min->other_v, b_v );
		// a_min->other_m = b_min->m;
		// vec_copy( b_min->other_v, a_v );
		// b_min->other_m = a_min->m;
		// 
		// /* set remaining time */
		// a_min->dt = g_dt - dt_c;
		// b_min->dt = g_dt - dt_c;
		// 
		// /* execute both objects' responses */
		// (*TtListFirst( ((Tt *)a_min)->kids )->task)( (Tt *)a_min );
		// (*TtListFirst( ((Tt *)b_min)->kids )->task)( (Tt *)b_min );
	// }
	// 
	// return e;
// }
