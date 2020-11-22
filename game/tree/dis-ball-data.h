/* ball display */

Displayer dis_ball;

void dis_ball_init()
{
	DisplayerMake( &dis_ball );
	
	dis_ball.super.task = (Task)draw_big_sphere;
	dis_ball.x = kin_ball.x;
}
