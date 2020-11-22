/* wall2 kinematics */

KinMover kin_wall2;

void kin_wall2_init()
{
	KinMoverMake( &kin_wall2 );
	
	kin_wall2.super.task = NULL_TASK;
	
	/* initial wall2 position in meters */
	kin_wall2.x[0] = -7.0;
	kin_wall2.x[1] = 7.0;
	kin_wall2.x[2] = 0.0;
	
	/* initial wall2 velocity in m/s */
	kin_wall2.v[0] = 0.0;
	kin_wall2.v[1] = 0.0;
	kin_wall2.v[2] = 0.0;
}
