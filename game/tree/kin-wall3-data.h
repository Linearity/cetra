/* wall3 kinematics */

KinMover kin_wall3;

void kin_wall3_init()
{
	KinMoverMake( &kin_wall3 );
	
	kin_wall3.super.task = NULL_TASK;
	
	/* initial wall3 position in meters */
	kin_wall3.x[0] = 7.0;
	kin_wall3.x[1] = 7.0;
	kin_wall3.x[2] = 0.0;
	
	/* initial wall3 velocity in m/s */
	kin_wall3.v[0] = 0.0;
	kin_wall3.v[1] = 0.0;
	kin_wall3.v[2] = 0.0;
}
