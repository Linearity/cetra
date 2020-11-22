/* wall2 display */

Displayer dis_wall2;
DisplayerMake( &dis_wall2 );

dis_wall2.super.task = (Task)draw_quadrilateral;
dis_wall2.x = kin_wall2.x;



Displayer dis_wall2_v0;
DisplayerMake( &dis_wall2_v0 );

Displayer dis_wall2_v1;
DisplayerMake( &dis_wall2_v1 );

Displayer dis_wall2_v2;
DisplayerMake( &dis_wall2_v2 );

Displayer dis_wall2_v3;
DisplayerMake( &dis_wall2_v3 );



double wall2_vertices[4 * 3] =
{
	0.0,	-10.0,	-10.0,
	0.0,	-10.0,	 10.0,
	0.0,	 10.0,	 10.0,
	0.0,	 10.0,	-10.0
};

dis_wall2_v0.x = wall2_vertices;
dis_wall2_v1.x = wall2_vertices + 3;
dis_wall2_v2.x = wall2_vertices + 6;
dis_wall2_v3.x = wall2_vertices + 9;

TtActivateChild( (Tt *)&dis_wall2, (Tt *)&dis_wall2_v0 );
TtActivateChild( (Tt *)&dis_wall2, (Tt *)&dis_wall2_v1 );
TtActivateChild( (Tt *)&dis_wall2, (Tt *)&dis_wall2_v2 );
TtActivateChild( (Tt *)&dis_wall2, (Tt *)&dis_wall2_v3 );
