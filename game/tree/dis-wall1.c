/* wall1 display */

Displayer dis_wall1;
DisplayerMake( &dis_wall1 );

dis_wall1.super.task = (Task)draw_quadrilateral;
dis_wall1.x = kin_wall1.x;



Displayer dis_wall1_v0;
DisplayerMake( &dis_wall1_v0 );

Displayer dis_wall1_v1;
DisplayerMake( &dis_wall1_v1 );

Displayer dis_wall1_v2;
DisplayerMake( &dis_wall1_v2 );

Displayer dis_wall1_v3;
DisplayerMake( &dis_wall1_v3 );



double wall1_vertices[4 * 3] =
{
	-10.0,	-10.0,	0.0,
	-10.0,	 10.0,	0.0,
	 10.0,	 10.0,	0.0,
	 10.0,	-10.0,	0.0
};

dis_wall1_v0.x = wall1_vertices;
dis_wall1_v1.x = wall1_vertices + 3;
dis_wall1_v2.x = wall1_vertices + 6;
dis_wall1_v3.x = wall1_vertices + 9;

TtActivateChild( (Tt *)&dis_wall1, (Tt *)&dis_wall1_v0 );
TtActivateChild( (Tt *)&dis_wall1, (Tt *)&dis_wall1_v1 );
TtActivateChild( (Tt *)&dis_wall1, (Tt *)&dis_wall1_v2 );
TtActivateChild( (Tt *)&dis_wall1, (Tt *)&dis_wall1_v3 );
