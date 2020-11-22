/* ball 2 kinematics */

KinMover kin_ball2;

void kin_ball2_init()
{
	KinMoverMake( &kin_ball2 );
	
	kin_ball2.super.task = (Task)&gravity_move;
	
	/* initial ball position in meters */
	kin_ball2.x[0] = 0.0;
	kin_ball2.x[1] = 8.0;
	kin_ball2.x[2] = 0.0;
	
	/* initial ball velocity in m/s */
	kin_ball2.v[0] = 0.0;
	kin_ball2.v[1] = 0.0;
	kin_ball2.v[2] = 0.0;
}
