/* ball 2 display */

Displayer dis_ball2;

void dis_ball2_init()
{
	DisplayerMake( &dis_ball2 );
	
	dis_ball2.super.task = (Task)draw_big_sphere;
	dis_ball2.x = kin_ball2.x;
}
