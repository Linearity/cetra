/* ball 2 display */

Displayer dis_ball2;
DisplayerMake( &dis_ball2 );

dis_ball2.super.task = (Task)draw_sphere;
dis_ball2.x = kin_ball2.x;
