/* wall3 display */

Displayer dis_wall3;
Displayer dis_wall3_v0;
Displayer dis_wall3_v1;
Displayer dis_wall3_v2;
Displayer dis_wall3_v3;

double wall3_vertices[4 * 3] =
{
	0.0,	-7.0,	-7.0,
	0.0,	 7.0,	-7.0,
	0.0,	 7.0,	 10.0,
	0.0,	-7.0,	 10.0
};

void dis_wall3_init()
{
	DisplayerMake( &dis_wall3 );

	dis_wall3.super.task = (Task)draw_quadrilateral;
	
	DisplayerMake( &dis_wall3_v0 );
	DisplayerMake( &dis_wall3_v1 );
	DisplayerMake( &dis_wall3_v2 );
	DisplayerMake( &dis_wall3_v3 );
	
	dis_wall3_v0.x = wall3_vertices;
	dis_wall3_v1.x = wall3_vertices + 3;
	dis_wall3_v2.x = wall3_vertices + 6;
	dis_wall3_v3.x = wall3_vertices + 9;
	
	dis_wall3.x = kin_wall3.x;
	
	TtActivateChild( (Tt *)&dis_wall3, (Tt *)&dis_wall3_v0 );
	TtActivateChild( (Tt *)&dis_wall3, (Tt *)&dis_wall3_v1 );
	TtActivateChild( (Tt *)&dis_wall3, (Tt *)&dis_wall3_v2 );
	TtActivateChild( (Tt *)&dis_wall3, (Tt *)&dis_wall3_v3 );
}
