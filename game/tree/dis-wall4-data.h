/* wall4 display */

Displayer dis_wall4;
Displayer dis_wall4_v0;
Displayer dis_wall4_v1;
Displayer dis_wall4_v2;
Displayer dis_wall4_v3;

double wall4_vertices[4 * 3] =
{
	-7.0,	-7.0,	 0.0,
	7.0,	-7.0,	 0.0,
	7.0,	 7.0,	 0.0,
	-7.0,	 7.0,	 0.0
};

void dis_wall4_init()
{
	DisplayerMake( &dis_wall4 );
	
	dis_wall4.super.task = (Task)draw_quadrilateral;
	
	DisplayerMake( &dis_wall4_v0 );
	DisplayerMake( &dis_wall4_v1 );
	DisplayerMake( &dis_wall4_v2 );
	DisplayerMake( &dis_wall4_v3 );
	
	dis_wall4_v0.x = wall4_vertices;
	dis_wall4_v1.x = wall4_vertices + 3;
	dis_wall4_v2.x = wall4_vertices + 6;
	dis_wall4_v3.x = wall4_vertices + 9;
	
	dis_wall4.x = kin_wall4.x;
	
	TtActivateChild( (Tt *)&dis_wall4, (Tt *)&dis_wall4_v0 );
	TtActivateChild( (Tt *)&dis_wall4, (Tt *)&dis_wall4_v1 );
	TtActivateChild( (Tt *)&dis_wall4, (Tt *)&dis_wall4_v2 );
	TtActivateChild( (Tt *)&dis_wall4, (Tt *)&dis_wall4_v3 );
}
