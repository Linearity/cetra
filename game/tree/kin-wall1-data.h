/* wall1 kinematics */

KinMover kin_wall1;

void kin_wall1_init()
{
	KinMoverMake( &kin_wall1 );
	
	kin_wall1.super.task = NULL_TASK;
	
	/* initial wall1 position in meters */
	kin_wall1.x[0] = 0.0;
	kin_wall1.x[1] = 7.0;
	kin_wall1.x[2] = -7.0;
	
	/* initial wall1 velocity in m/s */
	kin_wall1.v[0] = 0.0;
	kin_wall1.v[1] = 0.0;
	kin_wall1.v[2] = 0.0;
}
