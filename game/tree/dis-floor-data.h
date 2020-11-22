/* floor display */

Displayer dis_floor;
Displayer dis_floor_v0;
Displayer dis_floor_v1;
Displayer dis_floor_v2;
Displayer dis_floor_v3;

double floor_vertices[4 * 3] =
{
	-7.0, 0.0, -7.0,
	7.0, 0.0, -7.0,
	7.0, 0.0, 10.0,
	-7.0, 0.0, 10.0
};

void dis_floor_init()
{
	DisplayerMake( &dis_floor );
	
	dis_floor.super.task = (Task)draw_quadrilateral;
	
	DisplayerMake( &dis_floor_v0 );
	DisplayerMake( &dis_floor_v1 );
	DisplayerMake( &dis_floor_v2 );
	DisplayerMake( &dis_floor_v3 );
	
	dis_floor_v0.x = floor_vertices;
	dis_floor_v1.x = floor_vertices + 3;
	dis_floor_v2.x = floor_vertices + 6;
	dis_floor_v3.x = floor_vertices + 9;
	
	dis_floor.x = kin_floor.x;
	
	TtActivateChild( (Tt *)&dis_floor, (Tt *)&dis_floor_v0 );
	TtActivateChild( (Tt *)&dis_floor, (Tt *)&dis_floor_v1 );
	TtActivateChild( (Tt *)&dis_floor, (Tt *)&dis_floor_v2 );
	TtActivateChild( (Tt *)&dis_floor, (Tt *)&dis_floor_v3 );
}
