// from the Sealab project called "CoarseMesher"

#include <float.h>
#include <vector>

#include "slUtil.H"

using std::cout;
using std::endl;
using std::flush;

bool readObjFile(char *fname, std::vector<SlVector3> &pts, std::vector<SlTri> &triangles) {
  char c[500];
  SlVector3 lc(DBL_MAX), uc(-DBL_MAX);
  
  int numVertices=0, numFaces=0;
  bool normals = false, texture = false;
  int tint;
  char ch;
  int p, q, r;
  double x, y, z;
	std::vector<SlVector3>::iterator v;
	std::vector<SlTri>::iterator t;
  std::ifstream in1(fname, std::ios::in);
  in1.flags(in1.flags() & ~std::ios::skipws);

  while (in1>>ch) {
    if (ch == 'v') {
      in1>>ch;
      if (ch == ' ') numVertices++;
      else if (ch == 'n') normals = true;
      else if (ch == 't') texture = true;
      else std::cerr<<"error \'"<<ch<<"\'"<<std::endl;
    } else if (ch == 'f') numFaces++;
  }
  in1.close();

  pts.resize(numVertices);
  triangles.resize(numFaces);
	v = pts.begin();
	t = triangles.begin();

  std::ifstream in(fname, std::ios::in);
  while (in>>ch) {
    if (ch == '#') {
      in.getline(c,500);
      continue;
    }
    if (ch == 'g') {
      in.getline(c,500);
      continue;
    }
    if (ch == 's') {
      in.getline(c,500);
      continue;
    }
    if (ch == 'm') {
      in.getline(c,500);
      continue;
    }
    if (ch == 'u') {
      in.getline(c,500);
      continue;
    }
    if (ch == 'v') {
      ch = in.peek();
      if (ch != 't' && ch != 'n') {
				in>>x>>y>>z;
				(*v).set(x,y,z);
				if ((*v)[0] < lc[0]) lc[0] = (*v)[0];
				if ((*v)[1] < lc[1]) lc[1] = (*v)[1];
				if ((*v)[2] < lc[2]) lc[2] = (*v)[2];
				if ((*v)[0] > uc[0]) uc[0] = (*v)[0];
				if ((*v)[1] > uc[1]) uc[1] = (*v)[1];
				if ((*v)[2] > uc[2]) uc[2] = (*v)[2];
				v++;
      } else {
				in.getline(c, 500);
      }
      continue;
    }
    if (ch == 'f') {
      if (normals && texture) {
				in>>p>>ch>>tint>>ch>>tint>>q>>ch>>tint>>ch>>tint>>r>>ch>>tint>>ch>>tint;
      } else if (normals) {
				in>>p>>ch>>ch>>tint>>q>>ch>>ch>>tint>>r>>ch>>ch>>tint;
      } else if (texture) {
				in>>p>>ch>>tint>>q>>ch>>tint>>r>>ch>>tint;
      } else {
				in>>p>>q>>r;
      }
      (*t)[0] = p-1;
      (*t)[1] = q-1;
      (*t)[2] = r-1;
      t++;
      continue;
    }
  }
  in.close();
  return true;
}

void objDump(char *fname, const std::vector<SlVector3> &meshPts, const std::vector<SlTri> &triangles) {
	std::ofstream out;
	std::vector<SlVector3>::const_iterator p;
	std::vector<SlTri>::const_iterator t;

	out.open(fname);

	for (p=meshPts.begin(); p!=meshPts.end(); p++) 
		out<<"v "<<(*p)[0]<<" "<<(*p)[1]<<" "<<(*p)[2]<<std::endl;
	
	for (t=triangles.begin(); t!=triangles.end(); t++) 
		out<<"f "<<(*t)[0]+1<<" "<<(*t)[1]+1<<" "<<(*t)[2]+1<<std::endl;

	out.close();
}
