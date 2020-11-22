#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct Vector Vector;

struct Vector
{
	double x[3];
};

struct Vector vec_make( double *source );

void vec_copy( double *destination, struct Vector source );

struct Vector vec_scaled( struct Vector original, double s );
double vec_mag_squared( struct Vector v );
double vec_mag( struct Vector v );
struct Vector vec_normed( struct Vector original );

struct Vector vec_sum( struct Vector augend, struct Vector addend );
struct Vector vec_diff( struct Vector minuend, struct Vector subtrahend );

double vec_dot( struct Vector a, struct Vector b );
struct Vector vec_cross( struct Vector a, struct Vector b );

#endif
