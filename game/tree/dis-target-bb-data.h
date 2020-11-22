Displayer dis_target_bb;

Event draw_bounding_box( Tt *t )
{
	Vector min =
		vec_sum(
			vec_make( col_target.x ),
			vec_make( col_target.min ) );
	Vector max =
		vec_sum(
			vec_make( col_target.x ),
			vec_make( col_target.max ) );
	
	glDisable( GL_LIGHTING );
	glColor3f( 1.0f, 1.0f, 1.0f );
	
	glPushMatrix();
	glBegin( GL_LINES );
	
	
	glVertex3d( min.x[0], min.x[1], min.x[2] );
	glVertex3d( max.x[0], min.x[1], min.x[2] );
	
	glVertex3d( min.x[0], max.x[1], min.x[2] );
	glVertex3d( max.x[0], max.x[1], min.x[2] );
	
	glVertex3d( min.x[0], max.x[1], max.x[2] );
	glVertex3d( max.x[0], max.x[1], max.x[2] );
	
	glVertex3d( min.x[0], min.x[1], max.x[2] );
	glVertex3d( max.x[0], min.x[1], max.x[2] );
	
	
	glVertex3d( min.x[0], min.x[1], min.x[2] );
	glVertex3d( min.x[0], max.x[1], min.x[2] );
	                         
	glVertex3d( max.x[0], min.x[1], min.x[2] );
	glVertex3d( max.x[0], max.x[1], min.x[2] );
	                         
	glVertex3d( max.x[0], min.x[1], max.x[2] );
	glVertex3d( max.x[0], max.x[1], max.x[2] );
	                         
	glVertex3d( min.x[0], min.x[1], max.x[2] );
	glVertex3d( min.x[0], max.x[1], max.x[2] );
	
	
	glVertex3d( min.x[0], min.x[1], min.x[2] );
	glVertex3d( min.x[0], min.x[1], max.x[2] );
	                                   
	glVertex3d( max.x[0], min.x[1], min.x[2] );
	glVertex3d( max.x[0], min.x[1], max.x[2] );
	                                   
	glVertex3d( max.x[0], max.x[1], min.x[2] );
	glVertex3d( max.x[0], max.x[1], max.x[2] );
	                                   
	glVertex3d( min.x[0], max.x[1], min.x[2] );
	glVertex3d( min.x[0], max.x[1], max.x[2] );
	
	
	glEnd();
	glPopMatrix();
	
	glEnable( GL_LIGHTING );
	
	return NULL_EVENT;
}

void dis_target_bb_init()
{
	DisplayerMake( &dis_target_bb );
	
	dis_target_bb.super.task = draw_bounding_box;
}
	
