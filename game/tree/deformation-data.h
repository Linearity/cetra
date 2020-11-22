/* deformation */

#include "def-target-data.h"
#include "tra-ball-data.h"

Deformation deformation;

void deformation_init()
{
	DeformationMake( &deformation );
	
	deformation.simulator = new FemSimulator;
	char *lolpadding = new char[256];
	
	deformation.super.task = (Task)step_simulation;
	
	def_target_init();
	tra_ball_init();
	
	deformation.simulator->nobjects = 1;
	deformation.simulator->objects = def_target.object;
	deformation.simulator->dt = 1/30.0 + 1e-12;
	deformation.simulator->ground = true;
	deformation.simulator->initCollisions();
	
	TtActivateChild( (Tt *)&deformation, (Tt *)&def_target );
	TtActivateChild( (Tt *)&deformation, (Tt *)&tra_ball );
}
