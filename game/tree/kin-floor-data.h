/* floor kinematics */

KinMover kin_floor;

void kin_floor_init()
{
	KinMoverMake( &kin_floor );
	
	kin_floor.super.task = NULL_TASK;
	
	/* initial floor position in meters */
	kin_floor.x[0] = 0.0;
	kin_floor.x[1] = 0.0;
	kin_floor.x[2] = 0.0;
	
	/* initial floor velocity in m/s */
	kin_floor.v[0] = 0.0;
	kin_floor.v[1] = 0.0;
	kin_floor.v[2] = 0.0;
}
