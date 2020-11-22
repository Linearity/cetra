/* ball kinematics */

KinMover kin_ball;

void kin_ball_init()
{
	KinMoverMake( &kin_ball );
	
	kin_ball.super.task = (Task)&gravity_move;
	
	/* initial ball position in meters */
	kin_ball.x[0] = 0.0;
	kin_ball.x[1] = 2.0;
	kin_ball.x[2] = 5.0;
	
	/* initial ball velocity in m/s */
	kin_ball.v[0] = 0.0;
	kin_ball.v[1] = 0.0;
	kin_ball.v[2] = 0.0;
}
