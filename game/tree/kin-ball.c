/* ball kinematics */

KinMover kin_ball;
KinMoverMake( &kin_ball );

kin_ball.super.task = (Task)&gravity_move;

/* initial ball position in meters */
kin_ball.x[0] = 1.0;
kin_ball.x[1] = 3.0;
kin_ball.x[2] = 0.0;

/* initial ball velocity in m/s */
kin_ball.v[0] = -0.5;
kin_ball.v[1] = 0.0;
kin_ball.v[2] = 0.0;
