/* fire */

Fire fire;

void fire_init()
{
	FireMake( &fire );
	fire.x = kin_gun.x;
	fire.rotations = kin_gun.rotations;
	
	fire.super.task = (Task)launch_projectile;
}
