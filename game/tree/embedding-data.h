/* target embedding */

/* for each tet in the mesh, compute and store its beta matrix */
void compute_beta_matrices( SlMatrix3x3 *beta_matrices, TetMesh *mesh );

/* barymap each surface vertex, add map to its embedding tet's list of kids */
void add_barymap_kids( Tt *t_kids, TetMesh *mesh, double *surf_v_0, int surv_v_count);

/* for each tet kid, add the tet vertices to its list of kids */
void add_vertex_kids( Tt *t_kids, TetMesh *mesh );



Tt embedding;

SlVector3 *sim_vertices = NULL;
IndexTet *sim_tets = NULL;
int v_count = -1, t_count = -1;

void embedding_init()
{
	TtMake( &embedding );
	
	embedding.task = embed_surface_in_tetmesh;
	
	std::ifstream in_stream;
	std::ofstream out_stream;
	int dummy = -1;
	
	// read node file
	in_stream.open( "../tetmesh/cetra/cetra-belch-tetmesh.node" );
	//in_stream.open( "../homer-med/homer-tetmesh-0.2-0.6_0.527.node" );
	in_stream >> v_count;
	sim_vertices = new SlVector3[v_count];
	in_stream.ignore( 256, '\n' );
	for ( int i = 0; !in_stream.eof() && (i < v_count); i++ )
		in_stream >> dummy >> sim_vertices[i][0] >> sim_vertices[i][1] >> sim_vertices[i][2];
	in_stream.close();
	
	// read ele file
	in_stream.open( "../tetmesh/cetra/cetra-belch-tetmesh.ele" );
	//in_stream.open( "../homer-med/homer-tetmesh-0.2-0.6_0.527.ele" );
	in_stream >> t_count;
	sim_tets = new IndexTet[t_count];
	in_stream.ignore(256, '\n');
	for ( int i = 0; !in_stream.eof() && (i < t_count); i++ )
	{
		in_stream >> dummy >> sim_tets[i][1] >> sim_tets[i][0] >> sim_tets[i][2] >> sim_tets[i][3];
		sim_tets[i][0]--;
		sim_tets[i][1]--;
		sim_tets[i][2]--;
		sim_tets[i][3]--;
	}
	in_stream.close();
	
	TetMesh mesh( sim_vertices, v_count, sim_tets, t_count );
	
	Tt *tet_kids = (Tt *)malloc( t_count * sizeof( Tt ) );
	for ( int i = 0; i < t_count; i++ )
		TtMake( tet_kids + i );
	
	int surf_v_count = (int)TtListFirst( dis_surface.super.kids );
	double *surf_v_0 = ((Displayer *)TtListFirst( TtListRest( dis_surface.super.kids ) ))->x;
	
	add_barymap_kids( tet_kids, &mesh, surf_v_0, surf_v_count );
	
	add_vertex_kids( tet_kids, &mesh );

	for ( int i = 0; i < t_count; i++ )
		TtListAdd( &embedding.kids, (Tt *)(tet_kids + i) );
}



void compute_beta_matrices( SlMatrix3x3 *beta_matrices, TetMesh *mesh )
{
	IndexTet t;
	for ( int i = 0; i < mesh->tet_count; i++ )
	{
		t = *mesh->getTetPointer(i);
		beta_matrices[i] = 
			inverse(
				SlMatrix3x3( 
					*mesh->getVertexPointer( t[0] ) - *mesh->getVertexPointer( t[3] ),
					*mesh->getVertexPointer( t[1] ) - *mesh->getVertexPointer( t[3] ),
					*mesh->getVertexPointer( t[2] ) - *mesh->getVertexPointer( t[3] ),
					SlMatrix3x3::col ) );
	}
	
	return;
}



void add_barymap_kids( Tt *t_kids, TetMesh *mesh, double *surf_v_0, int surf_v_count)
{
	int t_count = mesh->tet_count;
	
	Barymap *b_kids = (Barymap *)malloc( surf_v_count * sizeof( Barymap ) );
	
	SlMatrix3x3 *beta_matrices = new SlMatrix3x3[t_count];
	compute_beta_matrices( beta_matrices, mesh );
	
	SlVector3 surf_v;
	IndexTet *t;
	for ( int i = 0; i < surf_v_count; i++ )
	{
		surf_v = SlVector3( surf_v_0 + 3 * i );
		int j;
		for ( j = 0; j < t_count; j++ )
		{
			t = mesh->getTetPointer( j );
			if ( mesh->pointInTet( surf_v, t ) )
			{
				BarymapMake( b_kids + i );
				b_kids[i].x = surf_v_0 + 3 * i;
				vec_copy(
					b_kids[i].b,
					vec_make(
						(beta_matrices[j] * (surf_v - *mesh->getVertexPointer( (*t)[3] ))).data ) );
					
				TtListAdd( &t_kids[j].kids, (Tt *)(b_kids + i) );
				break;
			}
		}
		assert( j < t_count );
	}
	
	delete[] beta_matrices;
	
	return;
}



void add_vertex_kids( Tt *t_kids, TetMesh *mesh )
{
	int t_count = mesh->tet_count;
	
	Displayer *v_kids = (Displayer *)malloc( t_count * 4 * sizeof( Displayer ) );
	
	for ( int i = 0; i < t_count; i++ )
	{
		for ( int j = 3; j >= 0; j-- )
		{
			DisplayerMake( v_kids + 4 * i + j );
			v_kids[4 * i + j].x = mesh->vertex_array[mesh->tet_array[i][j]].data;
			
			TtListAdd( &t_kids[i].kids, (Tt *)(v_kids + 4 * i + j) );
		}
	}
	
	return;
}
