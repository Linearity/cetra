#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "tt.h"

typedef struct Camera Camera;

struct Camera
{
	Tt super;
	
	double *x;
	double *rotations;
};

void CameraMake( Camera *c );

Event render( Camera *camera );

#endif
