/* ball display */

Displayer dis_ball;
DisplayerMake( &dis_ball );

dis_ball.super.task = (Task)draw_sphere;
dis_ball.x = kin_ball.x;
