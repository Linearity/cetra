#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>

// IO
#include <CGAL/IO/Polyhedron_iostream.h>

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3<K> Polyhedron;
typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, K> Mesh_domain;

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

// To avoid verbose function and named parameters call
using namespace CGAL::parameters;

int main()
{
  // Create input polyhedron
  Polyhedron polyhedron;
  std::ifstream input("test-dragon.off");
  input >> polyhedron;
   
  // Create domain
  Mesh_domain domain(polyhedron);
  
  // Mesh criteria (no cell_size set)
  Mesh_criteria criteria(cell_size=0.1);
  //Mesh_criteria criteria(25, 1.5, 0.008, 3);
  
  // Mesh generation
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, no_perturb(), no_exude());

  // Output
  std::ofstream medit_file("out_1.mesh");
  c3t3.output_to_medit(medit_file);
  medit_file.close();

  return 0;
}
