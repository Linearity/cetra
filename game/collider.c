#include <string.h>
#include <stdio.h>

#include "collider.h"
#include "tt.h"
#include "vector.h"

/* normal-direction velocity of first object after collision with second object */
static double post_normal_velocity( double a_m, double a_v, double b_m, double b_v );

/* velocity of first object after collision with second object */
static Vector post_velocity( double a_m, Vector a_v, double b_m, Vector b_v, Vector n );



/* constructor for Collider */
void ColliderMake( Collider *c )
{	
	TtMake( (Tt *)c );
	
	c->resoluble = 1;
	
	c->dt = 0.0;
	c->m = 0.0;
	memset( c->min, 0, 3 * sizeof( double ) );
	memset( c->max, 0, 3 * sizeof( double ) );
	
	memset( c->n, 0, 3 * sizeof( double ) );
	memset( c->other_v, 0, 3 * sizeof( double ) );
	c->other_m = 0.0;
	
	c->x = NULL;
	c->v = NULL;
	
	return;
}



/* set position and velocity as if an elastic collision occurred */
Event elastic_response( Collider *c )
{
	/*vec_copy(
		c->n,
		vec_normed(
			vec_diff( vec_make( c->other->x ), vec_make( c->x ) ) ) );*/
	
	Vector c_v, other_v, n;
	c_v = vec_make( c->v );
	other_v = vec_make( c->other_v ); 
	n = vec_make( c->n );
			
	vec_copy( c->v, post_velocity( c->m, c_v, c->other_m, other_v, n ) );
	
	vec_copy(
		c->x,
		vec_sum(
			vec_make( c->x ),
			vec_scaled( vec_make( c->v ), c->dt ) ) );
	
	return NULL_EVENT;
}



/* set position and velocity as if a plastic collision occurred */
Event plastic_response( Collider *c )
{	
	Vector c_v, other_v, n;
	c_v = vec_make( c->v );
	other_v = vec_make( c->other_v ); 
	n = vec_make( c->n );
			
	vec_copy(
		c->v,
		vec_diff(
			c_v,
			vec_scaled(
				n,
				vec_dot( n, c_v ) ) ) );
	vec_copy(
		c->x,
		vec_sum(
			vec_make( c->x ),
			vec_scaled( vec_make( c->v ), c->dt ) ) );
	
	return NULL_EVENT;
}



double post_normal_velocity( double a_m, double a_v, double b_m, double b_v )
{
	return (a_v * (a_m - b_m) + 2 * b_m * b_v) / (a_m + b_m);
}



Vector post_velocity( double a_m, Vector a_v, double b_m, Vector b_v, Vector n )
{
	double a_speed_n = 0.0, b_speed_n = 0.0;
	
	a_speed_n = vec_dot( n, a_v );
	b_speed_n = vec_dot( n, b_v );
	
	return
		vec_sum(
			vec_diff(
				a_v,
				vec_scaled(
					n,
					a_speed_n ) ),
			vec_scaled(
				n,
				post_normal_velocity( a_m, a_speed_n, b_m, b_speed_n ) ) );
}



Event identify_floor_collider( Tt *col_floor )
{
	Event e = EventMake();
	SigListAdd( &e.sigs, COLLIDER_FLOOR );
	return e;
}



Event identify_gun_collider( Tt *col_floor )
{
	Event e = EventMake();
	SigListAdd( &e.sigs, COLLIDER_GUN );
	return e;
}



Event identify_wall_collider( Tt *col_wall )
{
	Event e = EventMake();
	SigListAdd( &e.sigs, COLLIDER_WALL );
	return e;
}



Event identify_target_collider( Tt *col_target )
{
	Event e = EventMake();
	SigListAdd( &e.sigs, COLLIDER_TARGET );
	return e;
}



Event identify_projectile_collider( Tt *col_proj )
{
	Event e = EventMake();
	SigListAdd( &e.sigs, COLLIDER_PROJ );
	return e;
}



Event identify_ball_collider( Tt *col_ball )
{
	Event e = EventMake();
	SigListAdd( &e.sigs, COLLIDER_BALL );
	return e;
}
