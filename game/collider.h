#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "tt.h"

typedef struct Collider Collider;

struct Collider
{
	Tt super;
	
	int resoluble;
	
	double dt;
	double m;
	double min[3];
	double max[3];
	
	double n[3];
	double other_v[3];
	double other_m;
	
	double *x;
	double *v;
};

void ColliderMake( Collider *c );

Event elastic_response( Collider *c );
Event plastic_response( Collider *c );

Event identify_floor_collider( Tt *col_floor );
Event identify_gun_collider( Tt *col_gun );
Event identify_wall_collider( Tt *col_wall );
Event identify_target_collider( Tt *col_target );
Event identify_projectile_collider( Tt *col_proj );
Event identify_ball_collider( Tt *col_ball );

#endif
