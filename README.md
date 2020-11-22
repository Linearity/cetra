## Cetra ##

### A coarse tetrahedral mesh generator ###

This is the research code used to develop the paper "[Automatic Construction of
Coarse, High-Quality Tetrahedralizations that Enclose and Approximate Surfaces
for Animation][project]" ([DOI: 10.1145/2522628.2522648][doi]), presented at
[Motion in Games][mig] in November 2013.

This repository contains the core meshing code, mostly free of the unnecessary files
and scripts used in the research. Unfortunately it's also free of some necessary
files as well, namely:

* [mesh_query][meshquery], for some geometric queries during meshing
* [El Topo][eltopo], for continuous collision detection
* [cxxtest][cxxtest], for unit testing
* some header files from the lab's common codebase

This means the old makefile doesn't work. 😩

You can still see how things in the paper are implemented. Also included is
a very experimental game architecture in C used for the example game seen in
the supplementary video (see the [project page][project]).

[mig]: https://www.motioningames.org/
[doi]: https://doi.org/10.1145/2522628.2522648
[project]: https://cal.cs.umbc.edu/Papers/Stuart-2013-ACC/
[eltopo]: https://github.com/tysonbrochu/eltopo
[cxxtest]: https://github.com/CxxTest/cxxtest
[meshquery]: https://www.cs.ubc.ca/~rbridson/download/mesh_query0.1.tar.gz  