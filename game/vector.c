#include <assert.h>
#include <math.h>

#include "vector.h"

/* constructor for Vector */
struct Vector vec_make( double *source )
{
	struct Vector destination;
	
	for ( int i = 0; i < 3; i++ )
		destination.x[i] = source[i];
	
	return destination;
}



/* copy data from a Vector to an array */
void vec_copy( double *destination, struct Vector source )
{
	for ( int i = 0; i < 3; i++ )
		destination[i] = source.x[i];
}



/* the result of multiplying a Vector by a scalar */
struct Vector vec_scaled( struct Vector original, double s )
{
	struct Vector scaled;
	
	for ( int i = 0; i < 3; i++ )
		scaled.x[i] = original.x[i] * s;
	
	return scaled;
}



/* the square of the magnitude of a Vector */
double vec_mag_squared( struct Vector v )
{
	return v.x[0] * v.x[0] + v.x[1] * v.x[1] + v.x[2] * v.x[2];
}



/* the magnitude of a Vector */
double vec_mag( struct Vector v )
{
	return sqrt( vec_mag_squared( v ) );
}



/* the result of scaling a Vector to unit magnitude */
struct Vector vec_normed( struct Vector original )
{
	double mag = vec_mag( original );
	assert( mag != 0.0 );
	
	return vec_scaled( original, 1.0 / mag );
}



/* the result of adding two Vectors */
struct Vector vec_sum( struct Vector augend, struct Vector addend )
{
	struct Vector sum;
	
	for ( int i = 0; i < 3; i++ )
		sum.x[i] = augend.x[i] + addend.x[i];
	
	return sum;
}



/* the result of subtracting two Vectors */
struct Vector vec_diff( struct Vector minuend, struct Vector subtrahend )
{
	struct Vector difference;
	
	for ( int i = 0; i < 3; i++ )
		difference.x[i] = minuend.x[i] - subtrahend.x[i];
	
	return difference;
}



/* the dot product of two Vectors */
double vec_dot( struct Vector a, struct Vector b )
{
	return a.x[0] * b.x[0] + a.x[1] * b.x[1] + a.x[2] * b.x[2];
}




struct Vector vec_cross( struct Vector a, struct Vector b )
{
	struct Vector product;
	
	product.x[0] = a.x[1] * b.x[2] - a.x[2] * b.x[1];
	product.x[1] = a.x[2] * b.x[0] - a.x[0] * b.x[2];
	product.x[2] = a.x[0] * b.x[1] - a.x[1] * b.x[0];
	
	return product;
}
	
