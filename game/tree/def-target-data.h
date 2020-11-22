/* target deformer */

Deformer def_target;

void def_target_init()
{
	DeformerMake( &def_target );
	
	def_target.super.task = (Task)deform;
	
	def_target.object = new FemObject;
	def_target.object->initMesh(
		(SlVector3 *)sim_vertices,
		v_count,
		(Tet *)sim_tets,
		t_count,
		37500, /* lambda */
		75000, /* mu */
		5e-2, /* scale */
		1000.0 /* density */ );
	def_target.object->active = true;
	for ( int i = 0; i < def_target.object->nv; i++ )
	{
		def_target.object->pos[i][2] += 10.0;
		def_target.object->pos[i][1] += 4.0;
	}
	
	def_target.vertices = (double *)sim_vertices;
}
