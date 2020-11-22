#include <cxxtest/TestSuite.h>
#include <math.h>

#include "TetMesh.h"

class MyTestSuite1 : public CxxTest::TestSuite
{
public:
	void testVolume(void)
	{
		SlVector3 verts[4];
		verts[0] = SlVector3(0, 0, 0);
		verts[1] = SlVector3(1, 0, 0);
		verts[2] = SlVector3(0, 1, 0);
		verts[3] = SlVector3(0, 0, 1);
		
		IndexTet tets[1];
		tets[0] = IndexTet(0, 1, 2, 3);
		
		TetMesh mesh(verts, 4, tets, 1);
		
		TS_ASSERT_EQUALS(mesh.getVolume(mesh.getTetPointer(0)), 1.0 / 6.0);
	}
	
	void testDihedralAngle1(void)
	{
		SlVector3 verts[4];
		verts[0] = SlVector3(0, 0, 0);
		verts[1] = SlVector3(1, 0, 0);
		verts[2] = SlVector3(0, 1, 0);
		verts[3] = SlVector3(0, 0, 1);
		
		IndexTet tets[1];
		tets[0] = IndexTet(0, 1, 2, 3);
		
		TetMesh mesh(verts, 4, tets, 1);
		
		TS_ASSERT_EQUALS(mesh.getDihedralAngle(mesh.getTetPointer(0), 0, 1), M_PI / 2.0);
		TS_ASSERT_EQUALS(mesh.getDihedralAngle(mesh.getTetPointer(0), 1, 0), M_PI / 2.0);
		TS_ASSERT_EQUALS(mesh.getDihedralAngle(mesh.getTetPointer(0), 0, 2), M_PI / 2.0);
		TS_ASSERT_EQUALS(mesh.getDihedralAngle(mesh.getTetPointer(0), 2, 0), M_PI / 2.0);
		TS_ASSERT_EQUALS(mesh.getDihedralAngle(mesh.getTetPointer(0), 0, 3), M_PI / 2.0);
		TS_ASSERT_EQUALS(mesh.getDihedralAngle(mesh.getTetPointer(0), 3, 0), M_PI / 2.0);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 1, 2), acos(1.0 / sqrt(3)), 0.0001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 2, 1), acos(1.0 / sqrt(3)), 0.0001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 1, 3), acos(1.0 / sqrt(3)), 0.0001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 3, 1), acos(1.0 / sqrt(3)), 0.0001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 2, 3), acos(1.0 / sqrt(3)), 0.0001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 3, 2), acos(1.0 / sqrt(3)), 0.0001);
	}
	
	void testDihedralAngle2(void)
	{
		SlVector3 verts[4];
		verts[0] = SlVector3(0, 0, 1);
		verts[1] = SlVector3(0.943, 0, -0.333);
		verts[2] = SlVector3(-0.471, 0.816, -0.333);
		verts[3] = SlVector3(-0.471, -0.816, -0.333);
		
		IndexTet tets[1];
		tets[0] = IndexTet(0, 1, 2, 3);
		
		TetMesh mesh(verts, 4, tets, 1);
		
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 0, 1), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 1, 0), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 0, 2), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 2, 0), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 0, 3), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 3, 0), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 1, 2), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 2, 1), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 1, 3), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 3, 1), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 2, 3), acos(1.0/3.0), 0.001);
		TS_ASSERT_DELTA(mesh.getDihedralAngle(mesh.getTetPointer(0), 3, 2), acos(1.0/3.0), 0.001);
	}
	
	void testLol(void)
	{
		SlVector3 verts[4];
		verts[0] = SlVector3(0.00233972, 0.82743, 0.826054);
		verts[1] = SlVector3(0.856835, 0.853189, 0.823941);
		verts[2] = SlVector3(0.280612, 1.05436, 1.05196);
		verts[3] = SlVector3(0.462067, 1.35562, 0.436048);
		
		IndexTet tets[1];
		tets[0] = IndexTet(0, 1, 2, 3);
		
		TetMesh mesh(verts, 4, tets, 1);
		
		printf("Dihedral angle at edge (0, 1): %f\n", 360 / (2 * M_PI) * mesh.getDihedralAngle(mesh.getTetPointer(0), 0, 1));
		printf("Dihedral angle at edge (0, 2): %f\n", 360 / (2 * M_PI) * mesh.getDihedralAngle(mesh.getTetPointer(0), 0, 2));
		printf("Dihedral angle at edge (0, 3): %f\n", 360 / (2 * M_PI) * mesh.getDihedralAngle(mesh.getTetPointer(0), 0, 3));
		printf("Dihedral angle at edge (1, 2): %f\n", 360 / (2 * M_PI) * mesh.getDihedralAngle(mesh.getTetPointer(0), 1, 2));
		printf("Dihedral angle at edge (1, 3): %f\n", 360 / (2 * M_PI) * mesh.getDihedralAngle(mesh.getTetPointer(0), 1, 3));
		printf("Dihedral angle at edge (2, 3): %f\n", 360 / (2 * M_PI) * mesh.getDihedralAngle(mesh.getTetPointer(0), 2, 3));
	}
	
	void testPseudoNormal(void)
	{
		SlVector3 verts[4];
		verts[0] = SlVector3(0, 0, 0);
		verts[1] = SlVector3(1, 0, 0);
		verts[2] = SlVector3(0, 1, 0);
		verts[3] = SlVector3(0, 0, -1);
		
		IndexTet tets[1];
		tets[0] = IndexTet(0, 1, 2, 3);
		
		TetMesh mesh(verts, 4, tets, 1);
		
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 0, 1).x(), 0.0, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 0, 1).y(), -sqrt(2.0) / 2.0, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 0, 1).z(), sqrt(2.0) / 2.0, 0.001);
		
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 0, 2).x(), -sqrt(2.0) / 2.0, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 0, 2).y(), 0.0, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 0, 2).z(), sqrt(2.0) / 2.0, 0.001);
		
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 0, 3).x(), -sqrt(2.0) / 2.0, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 0, 3).y(), -sqrt(2.0) / 2.0, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 0, 3).z(), 0, 0.001);
		
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 1, 2).x(), (sqrt(3.0) / 3.0) / 0.9194, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 1, 2).y(), (sqrt(3.0) / 3.0) / 0.9194, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 1, 2).z(), (-sqrt(3.0) / 3.0 + 1) / 0.9194, 0.001);
		
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 1, 3).x(), (sqrt(3.0) / 3.0) / 0.9194, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 1, 3).y(), (sqrt(3.0) / 3.0 - 1.0) / 0.9194, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 1, 3).z(), -(sqrt(3.0) / 3.0) / 0.9194, 0.001);
		
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 2, 3).x(), (sqrt(3.0) / 3.0 - 1.0) / 0.9194, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 2, 3).y(), (sqrt(3.0) / 3.0) / 0.9194, 0.001);
		TS_ASSERT_DELTA(mesh.getPseudoNormal(mesh.getTetPointer(0), 2, 3).z(), (-sqrt(3.0) / 3.0) / 0.9194, 0.001);
	}
};

