/* gun kinematics */

KinMover kin_gun;

void kin_gun_init()
{
	KinMoverMake( &kin_gun );
	
	kin_gun.super.task = (Task)&input_move;
	
	/* initial gun position in meters */
	kin_gun.x[0] = 3.0;
	kin_gun.x[1] = 1.5;
	kin_gun.x[2] = 3.0;
	
	/* initial gun velocity in m/s */
	kin_gun.v[0] = 0.0;
	kin_gun.v[1] = 0.0;
	kin_gun.v[2] = 0.0;
	
	/* initial gun orientation in radians */
	kin_gun.rotations[1] = g_pi / 8.0;
}
