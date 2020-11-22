ProjTracker tra_ball;

void tra_ball_init()
{
	ProjTrackerMake( &tra_ball );
	
	tra_ball.super.task = (Task)track_projectile;

	tra_ball.k = &kin_ball;
	tra_ball.obstacle = new SphereObstacle( SlVector3( 0.0 ), SlVector3( 0.0 ), 1.0 );
}
