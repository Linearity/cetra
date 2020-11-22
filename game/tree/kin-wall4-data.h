/* wall4 kinematics */

KinMover kin_wall4;

void kin_wall4_init()
{
	KinMoverMake( &kin_wall4 );
	
	kin_wall4.super.task = NULL_TASK;
	
	/* initial wall4 position in meters */
	kin_wall4.x[0] = 0.0;
	kin_wall4.x[1] = 7.0;
	kin_wall4.x[2] = 10.0;
	
	/* initial wall4 velocity in m/s */
	kin_wall4.v[0] = 0.0;
	kin_wall4.v[1] = 0.0;
	kin_wall4.v[2] = 0.0;
}
